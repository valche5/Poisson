#include "SeamlessCloning.h"

#include <iostream>

#include "Eigen/Sparse"
#include "Eigen/Dense"

using namespace Eigen;

EigenImage SeamlessCloning::compute(const EigenImage &mask, const EigenImage &source, const EigenImage &target, bool mixingGradients) {
    // Usefull constants
    const int rows = mask.rows();
    const int cols = mask.cols();
    const int channels = target.channels();

    /* 1 : map is used to assign an unique ID to all selected pixels (in mask) */
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

    std::vector<std::pair<int, int>> neighbors{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // Function for gradient calculations (mixed gradient or not)
    std::function<float(const EigenImage &, const EigenImage &, int, int, int)> grad;
    if (mixingGradients) {
        // Mixing gradient
        grad = [&](const EigenImage &source, const EigenImage &target, int row, int col, int c) -> float {
            float v = 0;
            float fp = target(row, col, c), gp = source(row, col, c);
            float fq, gq;

            for (auto dq : neighbors) {
                fq = target(row + dq.first, col + dq.second, c), gq = source(row + dq.first, col + dq.second, c);
                v += (std::abs(fp - fq) > std::abs(gp - gq)) ? fp - fq : gp - gq;
            }

            return v;
        };
    } else {
        // Simple gradient
        grad = [&](const EigenImage &source, const EigenImage &target, int row, int col, int c) -> float {
            float v = 0;
            for (auto dq : neighbors) {
                v += source(row, col, c) - source(row + dq.first, col + dq.second, c);
            }
            return v;
        };
    }

    /* 2 : Create matrices A and B to solve Ax=B */
    MatrixXf B(pixelCount, channels);
    SparseMatrix<float> A(pixelCount,pixelCount);
    A.reserve(VectorXi::Constant(pixelCount,5));

    // Initialize A in an efficient manner
    for (int col = minCol; col <= maxCol; col++) {
        for (int row = minRow; row <= maxRow; row++) {
            if (mask(row, col) > 0) {
                // ID of point p
                int id = map(row, col);

                // Fill B with gradient of couples <p, q>
                for (int c = 0; c < channels; c++)
                    B(id, c) = grad(source, target, row, col, c);

                // Fill A and fill B with border values
                int nNeighbors = 0;
                for (auto &dq : neighbors) {
                    int qRow = row + dq.first, qCol = col + dq.second;

                    // Handle Borders
                    if (qRow >= rows || qRow < 0 || qCol >= cols || qCol < 0)
                        continue;

                    int otherId = map(qRow, qCol);
                    if (otherId != -1) {
                        // q is not on the border
                        A.insert(id, otherId) = -1;
                    } else {
                        // q is on the border
                        for (int c = 0; c < channels; c++)
                            B(id, c) += target(qRow, qCol, c);
                    }

                    nNeighbors++;
                }
                A.insert(id, id) = (float) nNeighbors;
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
