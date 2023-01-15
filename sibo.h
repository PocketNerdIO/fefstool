#ifndef SIBO_LIB
#define SIBO_LIB

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <utime.h>
#include <stdarg.h>

// #ifdef _WIN32
//     #include <windows.h>
//     // const char *slash = "\\";
// #else
//     // Assume it's something POSIX-compliant
//     #include <stdbool.h>
//     #include <unistd.h>
//     #include <sys/stat.h>
//     // const char *slash = "/";
// #endif

#ifdef _WIN32
    #include <windows.h>
    const char *slash = "\\";
#else
    // Assume it's something POSIX-compliant
    #include <unistd.h>
    const char *slash = "/";
#endif


#define FLASH_TYPE   0xf1a5
#define IMAGE_ISROM  0xffffffff

#define FEFS24_NULL_PTR  0xffffff
#define FEFS32_NULL_PTR  0xffffffff

#define FEFS32_PTR_LEN   4
#define FEFS24_PTR_LEN   3

// IMAGE INFO OFFSETS AND SIZES
#define IMAGE_POINTERSIZE_OFFSET       10

#define IMAGE_ROOTPTR_OFFSET           11

#define IMAGE_NAME_OFFSET_24           14
#define IMAGE_NAME_OFFSET_32           15
#define IMAGE_NAME_LENGTH              11

#define IMAGE_FLASHCOUNT_OFFSET_24     25 
#define IMAGE_FLASHCOUNT_OFFSET_32     26 
#define IMAGE_FLASHCOUNT_LENGTH        4

#define IMAGE_FLASHIDSTRING_OFFSET_24  33
#define IMAGE_FLASHIDSTRING_OFFSET_32  34
#define IMAGE_ROMIDSTRING_OFFSET_24    29
#define IMAGE_ROMIDSTRING_OFFSET_32    30

// ENTRY INFO OFFSETS AND SIZES
// Next Entry Pointer
#define ENTRY_NEXTENTRYPTR_OFFSET            0x00 // 0
// Entry Name
#define ENTRY_NAME_OFFSET_24                 0x03 // 3
#define ENTRY_NAME_OFFSET_32                 0x04 // 4
#define ENTRY_NAME_LENGTH                    0x08 // 8
// Entry Extension
#define ENTRY_EXT_OFFSET_24                  0x0b // 11
#define ENTRY_EXT_OFFSET_32                  0x0c // 12
#define ENTRY_EXT_LENGTH                     0x03 // 3
// Entry Flags (Hidden, System, etc)
#define ENTRY_FLAGS_OFFSET_24                0x0e // 14
#define ENTRY_FLAGS_OFFSET_32                0x0f // 15
#define ENTRY_FLAGS_LENGTH                   0x01 // 1
// First entry record pointer
#define ENTRY_FIRSTENTRYRECORDPTR_OFFSET_24  0x0f // 15
#define ENTRY_FIRSTENTRYRECORDPTR_OFFSET_32  0x10 // 16
// Alternative Record Pointer (currently unused in SIBOIMG)
#define ENTRY_ALTRECORDPTR_OFFSET_24         0x12 // 18
#define ENTRY_ALTRECORDPTR_OFFSET_32         0x14 // 20
/// Properties
#define ENTRY_PROPERTIES_OFFSET_24           0x15 // 21
#define ENTRY_PROPERTIES_OFFSET_32           0x18 // 24
#define ENTRY_PROPERTIES_LENGTH              0x01 // 1
// Timecode
#define ENTRY_TIMECODE_OFFSET_24             0x16 // 22
#define ENTRY_TIMECODE_OFFSET_32             0x19 // 25
#define ENTRY_TIMECODE_LENGTH                0x02 // 2
// Datecode (Rolled into timecode in SIBOIMG)
#define ENTRY_DATECODE_OFFSET_24             0x18 // 24
#define ENTRY_DATECODE_OFFSET_32             0x1b // 27
#define ENTRY_DATECODE_LENGTH                0x02 // 2
// First data record pointer (for a file)
#define ENTRY_FIRSTDATARECORDPTR_OFFSET_24   0x1a // 26
#define ENTRY_FIRSTDATARECORDPTR_OFFSET_32   0x1d // 29
// First data record length
#define ENTRY_FIRSTDATALEN_OFFSET_24         0x1d // 29
#define ENTRY_FIRSTDATALEN_OFFSET_32         0x21 // 33
#define ENTRY_FIRSTDATALEN_LENGTH            0x02 // 2

// Entry Flags
#define ENTRY_FLAG_ENTRYISVALID               1 << 0
#define ENTRY_FLAG_PROPERTIESDATETIMEISVALID  1 << 1
#define ENTRY_FLAG_ISFILE                     1 << 2
#define ENTRY_FLAG_NOENTRYRECORD              1 << 3
#define ENTRY_FLAG_NOALTRECORD                1 << 4
#define ENTRY_FLAG_ISLASTENTRY                1 << 5
#define ENTRY_FLAG_BIT6                       1 << 6 // Unused/Reserved
#define ENTRY_FLAG_BIT7                       1 << 7 // Unused/Reserved

#define ENTRY_PROPERTY_ISREADONLY    1 << 0
#define ENTRY_PROPERTY_ISHIDDEN      1 << 1
#define ENTRY_PROPERTY_ISSYSTEM      1 << 2
#define ENTRY_PROPERTY_ISVOLUMENAME  1 << 3
#define ENTRY_PROPERTY_ISDIRECTORY   1 << 4
#define ENTRY_PROPERTY_ISMODIFIED    1 << 5

// FILE INFO OFFSETS AND LENGTHS
#define FILE_FLAGS_OFFSET               0
#define FILE_FLAGS_LENGTH               1

#define FILE_NEXTRECORDPTR_OFFSET       1

#define FILE_ALTRECORDPTR_OFFSET_24     4
#define FILE_ALTRECORDPTR_OFFSET_32     5

#define FILE_DATARECORDPTR_OFFSET_24    7
#define FILE_DATARECORDPTR_OFFSET_32    9

#define FILE_DATARECORDLEN_OFFSET_24    10
#define FILE_DATARECORDLEN_OFFSET_32    13
#define FILE_DATARECORDLEN_LENGTH       2

#define FILE_ENTRYPROPERTIES_OFFSET_24  12
#define FILE_ENTRYPROPERTIES_OFFSET_32  15
#define FILE_ENTRYPROPERTIES_LENGTH     1

#define FILE_TIMECODE_OFFSET_24         13
#define FILE_TIMECODE_OFFSET_32         16
#define FILE_TIMECODE_LENGTH            2

#define FILE_DATECODE_OFFSET_24         15
#define FILE_DATECODE_OFFSET_32         18
#define FILE_DATECODE_LENGTH            2


struct PsiDateTime {
    uint16_t psi_time;
    uint16_t psi_date;
};



#endif