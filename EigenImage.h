#ifndef FINAL_EIGENIMAGE_H
#define FINAL_EIGENIMAGE_H

#include <string>
#include <vector>

class EigenImage {
public:
    EigenImage() {};
    EigenImage(int rows, int cols, int channels);
    EigenImage(const std::string &filename);

    bool load(const std::string &filename);
    bool write(const std::string &filename);

    unsigned char &operator()(int row, int col, int channel = 0) { return _data[idx(row, col, channel)]; }
    const unsigned char &operator()(int row, int col, int channel = 0) const { return _data[idx(row, col, channel)]; }

    unsigned char * data() { return _data.data(); }
    const unsigned char * data() const { return _data.data(); }
    int rows() const { return _rows; }
    int cols() const { return _cols; }
    int channels() const { return _channels; }

    void printInfos();
private:
    // Row-Major interleaved
    inline int idx(int row, int col, int channel) const {
        return (col * _rows + row) * _channels + channel;
    }
private:
    std::vector<unsigned char> _data;
    int _rows = 0, _cols = 0, _channels = 0;
};

#endif //FINAL_EIGENIMAGE_H
