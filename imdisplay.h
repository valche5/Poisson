#ifndef IMDISPLAY_H
#define IMDISPLAY_H

#include <string>
#include "Image.h"

void imShow(const Image<uchar> &img, const std::string &name = "");

void waitImClosed();

#endif //IMDISPLAY_H


