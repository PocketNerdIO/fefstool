#ifndef _SIBO_FEFS_H
#define _SIBO_FEFS_H

#include <cstdlib>
// #include "sibo.h"

class FEFSVolume {
    public:
        FEFSVolume(char *offered_buffer, uint size);

        bool isFEFS();
        // bool isFEFS32();

    private:
        char *_buffer;
        int _size;
        bool _is_FEFS = false;
        // bool _is_FEFS32 = false;
};

#endif // _SIBO_FEFS_H