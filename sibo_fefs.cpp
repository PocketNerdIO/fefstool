#include "sibo_fefs.h"

//#include <cstdint>
#include <cstring>
//#include <iostream>

FEFSVolume::FEFSVolume(char *offered_buffer, uint buf_size) {
    unsigned int img_type;
    unsigned int img_ptrsize;

    _buffer = offered_buffer;
    _size = buf_size;

    std::memcpy(&img_type, _buffer, 2);

    _is_FEFS = (img_type == 0xf1a5);

    if (!_is_FEFS) {
        return;
    }

    // memcpy(&img_ptrsize, _buffer + IMAGE_POINTERSIZE_OFFSET, 1);
    // _is_FEFS32 = ((img_ptrsize && 1) ? true : false);
}

bool FEFSVolume::isFEFS() {
    return _is_FEFS;
}

// bool FEFSVolume::isFEFS32() {
//     return _is_FEFS32;
// }

