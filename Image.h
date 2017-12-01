#ifndef PATCHMATCH_IMAGE_H
#define PATCHMATCH_IMAGE_H

#include <cstdio>
#include <iostream>

typedef unsigned char uchar;

template <typename T>
class Image {
public:
    Image(int width, int height, int channels = 1);
    Image(T *data, int width, int height, int channels = 1);
    Image(const Image<T> &m, int top, int left, int width, int height, bool share_data = false);
    ~Image();

    Image<T> &operator=(const Image<T> &other);

    Image<T> &fill(T value);

    T &operator()(int row, int col, int channel = 0) { return data[idx(row, col, channel)]; }
    const T &operator()(int row, int col, int channel = 0) const { return data[idx(row, col, channel)]; }
    T *getPtr(int row, int col) { return &data[idx(row, col, 0)]; }
    const T *getPtr(int row, int col) const { return &data[idx(row, col, 0)]; }

    Image<bool> overflowMask() const;
    Image<T> block(int top, int left, int width, int height) const;
    Image<T> blockCentered(int row, int col, int width, int height) const;
    Image<T> blockCentered(int row, int col, int size) const;
    Image<T> padded(int padding) const;

    void printInfo(const char *name) const;

private:
    // Colmajor interleaved
    inline int idx(int row, int col, int channel) const {
        return ((row + top) * dataCols +  (col + left)) * channels + channel;
    }

public:
    int top, left, cols, rows, channels;
    int &width, &height;
    int dataCols, dataRows;
    bool shared;
    T *data;
};



template <typename T>
Image<T>::Image(int width, int height, int channels)
        : top(0), left(0), cols(width), rows(height), channels(channels)
        , width(cols), height(rows)
        , dataCols(width), dataRows(height), shared(false)
{
    data = new T[rows*cols*channels];
}

template <typename T>
Image<T>::Image(T *data, int width, int height, int channels)
        : top(0), left(0), cols(width), rows(height), channels(channels)
        , width(cols), height(rows)
        , dataCols(width), dataRows(height), shared(false)
{
    int size = rows * cols * channels;
    this->data = new T[size];
    for (int i = 0; i < size; i++) this->data[i] = data[i];
}

template <typename T>
Image<T>::Image(const Image &m, int top, int left, int width, int height, bool share_data)
        : top(m.top + top), left(m.left + left), cols(width), rows(height), channels(m.channels)
        , width(cols), height(rows)
        , dataCols(m.dataCols), dataRows(m.dataRows), shared(share_data)
{
    if (!share_data) {
        int size = rows * cols * channels;
        this->data = new T[size];
        for (int i = 0; i < size; i++) this->data[i] = m.data[i];
    } else {
        data = m.data;
    }
}

template <typename T>
Image<T>::~Image() {
    if (!shared) delete[] data;
}

template <typename T>
Image<T> &Image<T>::operator=(const Image<T> &other) {
    if (&other == this) return *this;

    top = other.top; left = other.left; cols = other.cols; rows = other.rows; channels = other.channels;
    dataCols = other.dataCols; dataRows = other.dataRows;
    shared = other.shared;
    if (!shared) delete[] data;
    int size = rows * cols * channels;
    data = new T[size];
    for (int i = 0; i < size; i++) data[i] = other.data[i];
}

template <typename T>
Image<T> &Image<T>::fill(T value) {
    for (int row = 0; row < rows; row++)
        for (int col = 0; col < cols; col++)
            for (int c = 0; c < channels; c++)
                data[idx(row, col, c)] = value;
}

template <typename T>
Image<bool> Image<T>::overflowMask() const {
    Image<bool> mask(width, height, 1);
    mask.fill(true);

    if (top < 0) {
        int rowOverflow = -top;
        mask.block(0, 0, width, rowOverflow).fill(false);
    } else if (top >= dataRows) {
        int rowOverflow = top - dataRows + 1;
        mask.block(height - rowOverflow, 0, width, rowOverflow).fill(false);
    }
    if (left < 0) {
        int colOverflow = -left;
        mask.block(0, 0, colOverflow, height).fill(false);
    }
    else if (left >= dataCols) {
        int colOverflow = left - dataCols + 1;
        mask.block(width - colOverflow, 0, colOverflow, height).fill(false);
    }

    return mask;
}

template <typename T>
Image<T> Image<T>::block(int top, int left, int width, int height) const {
    return Image(*this, top, left, width, height, true);
}

template <typename T>
Image<T> Image<T>::blockCentered(int row, int col, int width, int height) const {
    int halfWidth = ((width % 2 == 0) ? width : (width - 1)) / 2;
    int halfHeight = ((height % 2 == 0) ? height : (height - 1)) / 2;
    int top = row - halfHeight;
    int left = col - halfWidth;
    return Image(*this, top, left, width, height, true);
}

template <typename T>
Image<T> Image<T>::blockCentered(int row, int col, int size) const {
    return blockCentered(row, col, size, size);
}

template <typename T>
Image<T> Image<T>::padded(int padding) const {
    return Image(*this, padding, padding, cols - padding, rows - padding, true);
}

template <typename T>
void Image<T>::printInfo(const char *name) const {
    printf("%s\n", name);
    printf("\t    Size : %dx%dx%d\n", rows, cols, channels);
    printf("\tdataSize : %dx%dx%d\n", dataRows, dataCols, channels);
    printf("\t TopLeft : [%d, %d]\n", top, left);
    printf("\t  Shared : %s\n", (shared) ? "True" : "False");
}



#endif //PATCHMATCH_IMAGE_H
