#include "EigenImage.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

EigenImage::EigenImage(int rows, int cols, int channels)
    : _data(rows * cols * channels), _rows(rows), _cols(cols), _channels(channels)
{}

EigenImage::EigenImage(const std::string &filename) {
    load(filename);
}

bool EigenImage::load(const std::string &filename) {
    unsigned char *data = stbi_load(filename.c_str(), &_rows, &_cols, &_channels, 0);
    if (data) {
        _data.assign(data, data + (_rows * _cols * _channels));
        stbi_image_free(data);
        return true;
    }
    return false;
}

bool EigenImage::write(const std::string &filename) {
    int res = stbi_write_png(filename.c_str(), _rows, _cols, _channels, _data.data(), _channels * _rows);
    return (res != 0);
}

void EigenImage::printInfos() {
    std::cout << "(" << _rows << ", " << _cols << ", " << _channels << ")" << std::endl;
}
