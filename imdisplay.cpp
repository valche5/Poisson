#include "imdisplay.h"

#include "CImg.h"

using namespace cimg_library;

static std::vector<CImgDisplay> displays;

void imShow(const Image<uchar> &img, const std::string &name) {
    CImg<uchar> cimg(img.data, img.channels, img.dataCols, img.dataRows);
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
