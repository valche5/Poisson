#ifndef FINAL_SEAMLESSCLONING_H
#define FINAL_SEAMLESSCLONING_H

#include "EigenImage.h"
#include "Eigen/Sparse"

extern EigenImage clone(const EigenImage &mask, const EigenImage &source, const EigenImage &target, bool mixingGradients = false);

#endif //FINAL_SEAMLESSCLONING_H
