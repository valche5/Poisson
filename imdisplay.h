#ifndef IMDISPLAY_H
#define IMDISPLAY_H

#include <string>
#include "EigenImage.h"

void imShow(const EigenImage &img, const std::string &name = "");

void waitImClosed();

#endif //IMDISPLAY_H


