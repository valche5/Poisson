#ifndef PATCHMATCH_UTILS_H
#define PATCHMATCH_UTILS_H

#include "mex.h"
#include "Image.h"
#include "mxTraits.h"

template <typename ArrayType, typename ImageType>
Image<ImageType> toImage(const mxArray *array) {
    mwSize nDim = mxGetNumberOfDimensions(array);
    const mwSize *dims = mxGetDimensions(array);
    ArrayType *mxData = (ArrayType *) mxGetData(array);

    int width = (int) dims[1];
    int height = (int) dims[0];
    int channels = (nDim == 3) ? (int) dims[2] : 1;

    Image<ImageType> image(width, height, channels);
    for (int row = 0; row < image.rows; row++) {
        for (int col = 0; col < image.cols; col++) {
            for (int c = 0; c < image.channels; c++) {
                int mxIdx = c * image.rows * image.cols + col * image.rows + row;
                image(row, col, c) = (ImageType) mxData[mxIdx];
            }
        }
    }

    return image;
}

template <typename ArrayType, typename ImageType>
mxArray *toMxArray(Image<ImageType> &image) {
    mwSize ndim = (image.channels == 1) ? 2 : 3;
    const mwSize dims[3] = { (mwSize) image.rows, (mwSize) image.cols, (mwSize) image.channels };
    mxClassID classid = classId<ArrayType>::value;

    mxArray *array = mxCreateNumericArray(ndim, dims, classid, mxREAL);
    ArrayType* mxData = (ArrayType *) mxGetData(array);

    for (int row = 0; row < image.rows; row++) {
        for (int col = 0; col < image.cols; col++) {
            for (int c = 0; c < image.channels; c++) {
                int mxIdx = c * image.rows * image.cols + col * image.rows + row;
                mxData[mxIdx] = (ArrayType) image(row, col, c);
            }
        }
    }

    return array;
}

#endif //PATCHMATCH_UTILS_H
