#include "SeamlessCloning.h"

#include <iostream>

#include "Eigen/Sparse"
#include "Eigen/Dense"

using namespace Eigen;

void SeamlessCloning::test() {
    std::cout << "nThreads : " << nbThreads() << std::endl;

//    ArrayXXd target = ArrayXXd::Random(10, 10);
    //ArrayXXd source = ArrayXXd::Random(10, 10);

    ArrayXXd target = ArrayXXd::Constant(10, 10, 1.d);
    ArrayXXd source = ArrayXXd::Constant(10, 10, 0.d);

    target = (target + 1.d) * 125.d;
    source = (source + 1.d) * 125.d;

    ArrayXXi mask(10, 10);
    mask << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
            0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
            0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    std::cout << "Mask : " << std::endl << mask << std::endl;

    ArrayXXi map = ArrayXXi::Constant(10, 10, -1);

    // # of selected pixels
    int pixelCount = 0;
    // Bounding Rect
    int minCol = mask.cols(), maxCol = 0;
    int minRow = mask.rows(), maxRow = 0;
    for (int col = 0; col < mask.cols(); col++) {
        for (int row = 0; row < mask.rows(); row++) {
            if (mask(row, col) == 1) {
                // Set selected pixel id
                map(row, col) = pixelCount++;

                // Update Selection bounding rect
                if (col < minCol) minCol = col;
                if (row < minRow) minRow = row;

                if (col > maxCol) maxCol = col;
                if (row > maxRow) maxRow = row;
            }
        }
    }

    std::cout << "Map : " << std::endl << map << std::endl;

    VectorXd B(pixelCount);

    SparseMatrix<double> A(pixelCount,pixelCount);
    A.reserve(VectorXi::Constant(pixelCount,5));
    for (int col = minCol; col <= maxCol; col++) {
        for (int row = minRow; row <= maxRow; row++) {
            if (mask(row, col) == 1) {
                int id = map(row, col);
                int otherId, Np = 0;

                A.insert(id, id) = 4.f;
                B(id) = 4 * source(row, col);

                // Top
                if ((otherId = map(row - 1, col)) != -1) {
                    A.insert(id, otherId) = -1;
                } else {
                    B(id) += target(row - 1, col);
                }
                B(id) -= source(row - 1, col);

                // Bottom
                if ((otherId = map(row + 1, col)) != -1) {
                    A.insert(id, otherId) = -1;
                } else {
                    B(id) += target(row + 1, col);
                }
                B(id) -= source(row + 1, col);

                // Left
                if ((otherId = map(row, col - 1)) != -1) {
                    A.insert(id, otherId) = -1;
                } else {
                    B(id) += target(row, col - 1);
                }
                B(id) -= source(row, col - 1);

                // Right
                if ((otherId = map(row, col + 1)) != -1) {
                    A.insert(id, otherId) = -1;
                    Np++;
                } else {
                    B(id) += target(row, col + 1);
                }
                B(id) -= source(row, col + 1);
            }
        }
    }
    A.makeCompressed();

    MatrixXd Ad;
    Ad = MatrixXd(A);
    //std::cout << "Nb Elem per column : " << Ad.array().colwise().count() << std::endl;

    std::cout << "A : " << std::endl << Ad << std::endl;
    std::cout << "B : " << std::endl << B.transpose() << std::endl;

    //ConjugateGradient<SparseMatrix<double>, Lower|Upper> solver;
    SimplicialLDLT<SparseMatrix<double>> solver;
    solver.compute(A);
    if(solver.info()!=Success) {
        std::cout << "Decomposition failed" << std::endl;
        return;
    }
    VectorXd x = VectorXd::Zero(pixelCount);
    x = solver.solve(B);
    if(solver.info()!=Success) {
        std::cout << "Solving failed" << std::endl;
    }
    std::cout << "x : " << std::endl << x.transpose() << std::endl;

    std::cout << "Source : " << std::endl << source.cast<int>() << std::endl;
    std::cout << "Target : " << std::endl << target.cast<int>() << std::endl;

    ArrayXXd result(10, 10);
    for (int col = 0; col < target.cols(); col++) {
        for (int row = 0; row < target.rows(); row++) {
            if (mask(row, col) == 0) {
                result(row, col) = target(row, col);
            } else {
                result(row, col) = x(map(row, col));
            }
        }
    }

    std::cout << "Result : " << std::endl << result.round().cast<int>() << std::endl;
}

EigenImage SeamlessCloning::compute(const EigenImage &mask, const EigenImage &source, const EigenImage &target, bool mixingGradients) {
    // Usefull constants
    const int rows = mask.rows();
    const int cols = mask.cols();
    const int channels = target.channels();

    /* 1 : a Map is used Assign an ID to all selected pixels (in mask) */
    ArrayXXi map = ArrayXXi::Constant(rows, cols, -1);

    // # of selected pixels
    int pixelCount = 0;
    // Bounding Rect
    int minCol = cols, maxCol = 0;
    int minRow = rows, maxRow = 0;
    for (int col = 0; col < cols; col++) {
        for (int row = 0; row < rows; row++) {
            if (mask(row, col) > 0) {
                // Set selected pixel id
                map(row, col) = pixelCount++;

                // Update Selection bounding rect
                if (col < minCol) minCol = col;
                if (row < minRow) minRow = row;

                if (col > maxCol) maxCol = col;
                if (row > maxRow) maxRow = row;
            }
        }
    }

    /* 2 : Create matrices A and B to solve Ax=B */
    MatrixXf B(pixelCount, channels);
    SparseMatrix<float> A(pixelCount,pixelCount);
    A.reserve(VectorXi::Constant(pixelCount,5));

    // Function for gradient calculations (mixed gradient or not)
    std::function<float(const EigenImage &, const EigenImage &, int, int, int)> grad;
    if (mixingGradients) {
        // Mixing gradient
        grad = [](const EigenImage &source, const EigenImage &target, int row, int col, int c) -> float {
            float v = 0;
            float fp = target(row, col, c), gp = source(row, col, c);
            float fq, gq;

            // Left
            fq = target(row - 1, col, c), gq = source(row - 1, col, c);
            v += (std::abs(fp - fq) > std::abs(gp - gq)) ? fp - fq : gp - gq;
            // Right
            fq = target(row + 1, col, c); gq = source(row + 1, col, c);
            v += (std::abs(fp - fq) > std::abs(gp - gq)) ? fp - fq : gp - gq;
            // Top
            fq = target(row, col - 1, c); gq = source(row, col - 1, c);
            v += (std::abs(fp - fq) > std::abs(gp - gq)) ? fp - fq : gp - gq;
            // Bottom
            fq = target(row, col + 1, c); gq = source(row, col + 1, c);
            v += (std::abs(fp - fq) > std::abs(gp - gq)) ? fp - fq : gp - gq;

            return v;
        };
    } else {
        // Simple gradient
        grad = [](const EigenImage &source, const EigenImage &target, int row, int col, int c) -> float {
            return 4 * source(row, col, c) - source(row - 1, col, c) - source(row + 1, col, c)
                   - source(row, col - 1, c) - source(row, col + 1, c);
        };
    }

    for (int col = minCol; col <= maxCol; col++) {
        for (int row = minRow; row <= maxRow; row++) {
            if (mask(row, col) > 0) {
                int id = map(row, col);

                // Fill B with source image gradients
                for (int c = 0; c < channels; c++) {
                    B(id, c) = grad(source, target, row, col, c);
                }

                // Fill A, fill B with border values
                A.insert(id, id) = 4.f;
                int otherId;
                // Top
                if ((otherId = map(row - 1, col)) != -1) {
                    A.insert(id, otherId) = -1;
                } else {
                    for (int c = 0; c < channels; c++)
                        B(id, c) += target(row - 1, col, c);
                }

                // Bottom
                if ((otherId = map(row + 1, col)) != -1) {
                    A.insert(id, otherId) = -1;
                } else {
                    for (int c = 0; c < channels; c++)
                        B(id, c) += target(row + 1, col, c);
                }

                // Left
                if ((otherId = map(row, col - 1)) != -1) {
                    A.insert(id, otherId) = -1;
                } else {
                    for (int c = 0; c < channels; c++)
                        B(id, c) += target(row, col - 1, c);
                }

                // Right
                if ((otherId = map(row, col + 1)) != -1) {
                    A.insert(id, otherId) = -1;
                } else {
                    for (int c = 0; c < channels; c++)
                        B(id, c) += target(row, col + 1, c);
                }
            }
        }
    }
    A.makeCompressed();

    /* 3 : solve Ax=B */
    SimplicialLDLT<SparseMatrix<float>> solver;
    solver.compute(A);
    if(solver.info()!=Success) {
        std::cout << "Decomposition failed" << std::endl;
    }

    MatrixXf x(pixelCount, channels);
    x = solver.solve(B);
    if(solver.info()!=Success) {
        std::cout << "Solving failed" << std::endl;
    }

    x = x.array().round();

    EigenImage result(rows, cols, channels);
    for (int col = 0; col < cols; col++) {
        for (int row = 0; row < rows; row++) {
            if (mask(row, col) > 0) {
                for (int c = 0; c < channels; c++) {
                    result(row, col, c) = (unsigned char) std::min(255.f, std::max(x(map(row, col), c), 0.f));
                }
            } else {
                for (int c = 0; c < channels; c++) {
                    result(row, col, c) = target(row, col, c);
                }
            }
        }
    }

    return result;
}
