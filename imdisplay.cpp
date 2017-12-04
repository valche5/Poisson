#include "imdisplay.h"

#include "CImg.h"

using namespace cimg_library;

static std::vector<CImgDisplay> displays;

void imShow(const EigenImage &img, const std::string &name) {
    CImg<unsigned char> cimg(img.data(), img.channels(), img.rows(), img.cols());
    cimg.permute_axes("YZCX");

    displays.emplace_back(cimg, name.c_str());
}

void waitImClosed() {
    bool allClosed = false;
    while (!allClosed) {
        allClosed = true;
        for (auto &d : displays) {
            if (!d.is_closed()) {
                d.wait();
                allClosed = false;
            }
        }
    }
}
