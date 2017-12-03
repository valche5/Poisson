#include <iostream>

#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imdisplay.h"
#include "Eigen/Sparse"
#include "Eigen/Dense"
#include <vector>

Image<uchar> loadImage(const std::string &filename) {
    int width, height, n;
    uchar *data = stbi_load(filename.c_str(), &width, &height, &n, 3);
    Image<uchar> img = Image<uchar>(data, width, height, 3);
    stbi_image_free(data);
    return img;
}

int main(int argc, char* argv[]) {
    // Reconstruire a, à partir des pixels de B
    Image<uchar> a = loadImage("../a.png");

    std::cout << "nThreads : " << Eigen::nbThreads() << std::endl;

    Eigen::ArrayXXf target = Eigen::ArrayXXf::Random(10, 10);
    Eigen::ArrayXXf source = Eigen::ArrayXXf::Random(10, 10);

    target = (target + 1.f) * 125.f;
    source = (source + 1.f) * 125.f;

    Eigen::ArrayXXi mask(10, 10);
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

    //typedef Eigen::Triplet<double> T;
    //std::vector<T> tripletList;
    //std::vector<std::pair<int, int>> omegaPixelsCoords;

    Eigen::ArrayXXi map = Eigen::ArrayXXi::Constant(10, 10, -1);

    int pixelCount = 0;
    for (int col = 0; col < mask.cols(); col++) {
        for (int row = 0; row < mask.rows(); row++) {
            if (mask(row, col) == 1) {
                map(row, col) = pixelCount++;
            }
        }
    }

    std::cout << "Map : " << std::endl << map << std::endl;

    Eigen::VectorXf B(pixelCount);

    Eigen::SparseMatrix<float> A(pixelCount,pixelCount);
    A.reserve(Eigen::VectorXi::Constant(pixelCount,5));
    for (int col = 0; col < mask.cols(); col++) {
        for (int row = 0; row < mask.rows(); row++) {
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

    Eigen::MatrixXf Ad;
    Ad = Eigen::MatrixXf(A);
    //std::cout << "Nb Elem per column : " << Ad.array().colwise().count() << std::endl;

    std::cout << "A : " << std::endl << Ad << std::endl;
    std::cout << "B : " << std::endl << B.transpose() << std::endl;

    Eigen::ConjugateGradient<Eigen::SparseMatrix<float>, Eigen::Lower|Eigen::Upper> cg;
    cg.compute(A);
    if(cg.info()!=Eigen::Success) {
        std::cout << "Decomposition failed" << std::endl;
        return 1;
    }
    Eigen::VectorXf x = Eigen::VectorXf::Zero(pixelCount);
    x = cg.solve(B);
    if(cg.info()!=Eigen::Success) {
        std::cout << "Solving failed" << std::endl;
        //return 1;
    }
    std::cout << "x : " << std::endl << x.transpose() << std::endl;

    std::cout << "Source : " << std::endl << source.cast<int>() << std::endl;
    std::cout << "Target : " << std::endl << target.cast<int>() << std::endl;

    Eigen::MatrixXf result(10, 10);
    for (int col = 0; col < target.cols(); col++) {
        for (int row = 0; row < target.rows(); row++) {
            if (mask(row, col) == 0) {
                result(row, col) = target(row, col);
            } else {
                result(row, col) = x(map(row, col));
            }
        }
    }

    std::cout << "Result : " << std::endl << result.cast<int>() << std::endl;
    /*
    imShow(a, "imga");
    imShow(a, "imgb");
    std::cout << "Wait for windows to be closed !" << std::endl;
    waitImClosed();
    std::cout << "Closed !" << std::endl;
    */
    return 0;
}

