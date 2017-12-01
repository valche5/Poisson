#ifndef PATCHMATCH_MXTRAITS_H
#define PATCHMATCH_MXTRAITS_H

#include <mex.h>

template <typename T>
struct classId {
    static const mxClassID value = mxUNKNOWN_CLASS;
};

template <>
struct classId<int8_T> {
    static const mxClassID value = mxINT8_CLASS;
};

template <>
struct classId<uint8_T> {
    static const mxClassID value = mxUINT8_CLASS;
};

template <>
struct classId<int16_T> {
    static const mxClassID value = mxINT16_CLASS;
};

template <>
struct classId<uint16_T> {
    static const mxClassID value = mxUINT16_CLASS;
};

template <>
struct classId<int32_T> {
    static const mxClassID value = mxINT32_CLASS;
};

template <>
struct classId<uint32_T> {
    static const mxClassID value = mxUINT32_CLASS;
};

template <>
struct classId<int64_T> {
    static const mxClassID value = mxINT64_CLASS;
};

template <>
struct classId<uint64_T> {
    static const mxClassID value = mxUINT64_CLASS;
};

template <>
struct classId<real32_T > {
    static const mxClassID value = mxSINGLE_CLASS;
};

template <>
struct classId<real64_T> {
    static const mxClassID value = mxDOUBLE_CLASS;
};

#endif //PATCHMATCH_MXTRAITS_H
