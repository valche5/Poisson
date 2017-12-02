#include <iostream>

#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imdisplay.h"

Image<uchar> loadImage(const std::string &filename) {
    int width, height, n;
    uchar *data = stbi_load(filename.c_str(), &width, &height, &n, 3);
    Image<uchar> img = Image<uchar>(data, width, height, 3);
    delete[] data;
    return img;
}


int main(int argc, char* argv[]) {
    // Reconstruire a, à partir des pixels de B
    Image<uchar> a = loadImage("../a.png");

    imShow(a, "imga");
    imShow(a, "imgb");

    std::cout << "Hello !" << std::endl;

    waitImClosed();

    std::cout << "Ended !" << std::endl;

    return 0;
}

