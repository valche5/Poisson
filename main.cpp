#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>

#include "Image.h"
#include "CImg.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace cimg_library;

Image<uchar> loadImage(const std::string &filename) {
    int width, height, n;
    uchar *data = stbi_load(filename.c_str(), &width, &height, &n, 3);
    Image<uchar> img = Image<uchar>(data, width, height, 3);
    delete[] data;
    return img;
}

template <typename T>
CImg<T> toCImg(Image<T> &img) {
    CImg<T> cimg(img.data, img.channels, img.dataCols, img.dataRows);
    cimg.permute_axes("YZCX");
    return cimg;
}

int main(int argc, char* argv[]) {
    // Reconstruire a, à partir des pixels de B
    Image<uchar> a = loadImage("../a.png");



    CImg<uchar> imga = toCImg(a);
    CImgDisplay aDisp(imga, "a.png");

    aDisp.show();

    while(!aDisp.is_closed()) {
        aDisp.wait();
    }

    return 0;
}

