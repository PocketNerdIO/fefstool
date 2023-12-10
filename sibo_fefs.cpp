#include "sibo_fefs.h"

FEFSVolume::FEFSVolume(char *offered_buffer, uint buf_size) {
    u_int16_t img_type;
    u_int8_t  img_ptrsize;

    _buffer = offered_buffer;
    _size = buf_size;

    std::memcpy(&img_type, _buffer, 2);

    _is_FEFS = (img_type == FLASH_TYPE);

    if (!_is_FEFS) {
        return;
    }

    memcpy(&img_ptrsize, _buffer + IMAGE_POINTERSIZE_OFFSET, 1);
    _is_FEFS32 = ((img_ptrsize && 1) ? true : false);

    memcpy(&_img_name, _buffer + (_is_FEFS32 ? IMAGE_NAME_OFFSET_32 : IMAGE_NAME_OFFSET_24), IMAGE_NAME_LENGTH);
    _img_name[IMAGE_NAME_LENGTH] = 0;
    rtrim(_img_name);
}

bool FEFSVolume::isFEFS() {
    return _is_FEFS;
}

bool FEFSVolume::isFEFS32() {
    return _is_FEFS32;
}

std::string FEFSVolume::getVolName() {
    return _img_name;
}
