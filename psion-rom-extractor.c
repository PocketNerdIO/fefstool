#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define FLASH_TYPE   0xf1a5
#define IMAGE_ISROM  0xffffffff

#define IMAGE_ROOTPTR_OFFSET        11
#define IMAGE_ROOTPTR_LENGTH        3
#define IMAGE_NAME_OFFSET           14
#define IMAGE_NAME_LENGTH           11
#define IMAGE_FLASHCOUNT_OFFSET     25
#define IMAGE_FLASHCOUNT_LENGTH     4
#define IMAGE_FLASHIDSTRING_OFFSET  33
#define IMAGE_ROMIDSTRING_OFFSET    29

#define ENTRY_NEXTENTRY_OFFSET         0
#define ENTRY_NEXTENTRY_LENGTH         3
#define ENTRY_NAME_OFFSET              3
#define ENTRY_NAME_LENGTH              8
#define ENTRY_EXT_OFFSET               11
#define ENTRY_EXT_LENGTH               3
#define ENTRY_FLAGS_OFFSET             14
#define ENTRY_FLAGS_LENGTH             1
#define ENTRY_FIRSTENTRYRECORD_OFFSET  15
#define ENTRY_FIRSTENTRYRECORD_LENGTH  3
#define ENTRY_ALTRECORD_OFFSET         18
#define ENTRY_ALTRECORD_LENGTH         3
#define ENTRY_ENTRYPROPERTIES_OFFSET   21
#define ENTRY_ENTRYPROPERTIES_LENGTH   1
#define ENTRY_TIME_OFFSET              22
#define ENTRY_TIME_LENGTH              2
#define ENTRY_DATE_OFFSET              24
#define ENTRY_DATE_LENGTH              2
#define ENTRY_FIRSTDATARECORD_OFFSET   26
#define ENTRY_FIRSTDATARECORD_LENGTH   3
#define ENTRY_FIRSTDATALEN_OFFSET      29
#define ENTRY_FIRSTDATALEN_LENGTH      2

#define ENTRY_FLAG_ENTRYISVALID               1
#define ENTRY_FLAG_PROPERTIESDATETIMEISVALID  2
#define ENTRY_FLAG_ISFILE                     4
#define ENTRY_FLAG_NOENTRYRECORD              8
#define ENTRY_FLAG_NOALTRECORD                16
#define ENTRY_FLAG_ISLASTENTRY                32
#define ENTRY_FLAG_BIT6                       64
#define ENTRY_FLAG_BIT7                       128

#define ENTRY_PROPERTY_ISREADONLY    1
#define ENTRY_PROPERTY_ISHIDDEN      2
#define ENTRY_PROPERTY_SYSTEM        4
#define ENTRY_PROPERTY_ISVOLUMENAME  8
#define ENTRY_PROPERTY_ISDIRECTORY   16
#define ENTRY_PROPERTY_ISMODIFIED    32


void datecode(int date, int *year, int *month, int *day) {
    *day = date % 0x20;
    *month = (date >> 5) % 0x10;
    *year = (date >> 9) + 1980;
}

void timecode(int time, int *hour, int *min, int *sec) {
    *sec = (time % 0x20) * 2;
    *min = (time >> 5) % 0x40;
    *hour = (time >> 11);
}

void walkpath(int pos, char path[], char *buffer[]) {
    char entry_name[9], entry_ext[4];
    int entry_flags;
    int date = 0, day = 0, month = 0, year = 0;
    int time = 0, hour = 0, min = 0, sec = 0;

    printf("Dir starts at: %p\n", (pos + 3));
    memcpy(entry_name, *buffer + (pos + ENTRY_NAME_OFFSET), ENTRY_NAME_LENGTH);
    entry_name[8] = '\0';
    memcpy(entry_ext, *buffer + (pos + ENTRY_EXT_OFFSET), ENTRY_EXT_LENGTH);
    entry_ext[3] = '\0';
    printf("\n=================\n");
    printf("DIR: %s", entry_name);
    if (strncmp(entry_ext, "   ", 3) != 0) {
        printf(".%s", entry_ext);
    }
    printf("\n");
    memcpy(&entry_flags, *buffer + (pos + ENTRY_FLAGS_OFFSET), ENTRY_FLAGS_LENGTH);
    printf("Flags: %p\n", entry_flags);
    if (entry_flags & ENTRY_FLAG_ENTRYISVALID) {
        printf("Valid entry.\n");
    } else {
        printf("Not a valid entry!\n");
    }
    if (entry_flags & ENTRY_FLAG_ISFILE) {
        printf("Is a file.\n");
    } else {
        printf("Is a diretory.\n");
    }
    if (entry_flags & ENTRY_FLAG_NOENTRYRECORD) {
        printf("No entry record.\n");
    } else {
        printf("Entry record.\n");
    }
    if (entry_flags & ENTRY_FLAG_NOALTRECORD) {
        printf("No alternative record.\n");
    } else {
        printf("Alternative record.\n");
    }
    if (entry_flags & ENTRY_FLAG_ISLASTENTRY) {
        printf("This is the last entry in the current directory.\n");
    } else {
        printf("Not the last entry in the current directory.\n");
    }

    memcpy(&date, *buffer + (pos + ENTRY_DATE_OFFSET), ENTRY_DATE_LENGTH);
    datecode(date, &year, &month, &day);
    printf("Date (%x): %4d-%2d-%2d\n", date, year, month, day);

    memcpy(&time, *buffer + (pos + ENTRY_TIME_OFFSET), ENTRY_TIME_LENGTH);
    timecode(time, &hour, &min, &sec);
    printf("Time (%x): %2d:%2d:%2d\n", time, hour, min, sec);    
}

int main(int argc, char *argv[]) {
    FILE *fp;
    int i, c;
    long file_len;
    char *buffer;
    char img_name[12];
    unsigned short img_type;
    int img_flashcount;
    int img_rootstart; // Will probably replace this with something more flexible later.


    if (argv[1] == NULL) {
        printf("%s: missing argument\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if (argv[2] != NULL) {
        printf("%s: too many arguments\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fp = fopen(argv[1], "rb");

    fread(&img_type, 2, 1, fp);

    if (img_type != FLASH_TYPE) {
        printf("%s: %s: Not a Psion Flash or ROM SSD image.\n", argv[0], argv[1]);
        fclose(fp);
        exit(EXIT_FAILURE); 
    }

    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp); // Get the current byte offset in the file
    rewind(fp);

    buffer = (char *)malloc((file_len + 1) * sizeof(char)); // Enough memory for file + \0
    fread(buffer, file_len, 1, fp);
    fclose(fp);
    
    // Fetch ROM Name
    memcpy(img_name, buffer + IMAGE_NAME_OFFSET, IMAGE_NAME_LENGTH);
    img_name[11] = 0x0;
    printf("Image name: %s\n", img_name);
    printf("Length: %d\n", file_len);

    // Fetch ROM Size
    memcpy(&img_flashcount, buffer + IMAGE_FLASHCOUNT_OFFSET, IMAGE_FLASHCOUNT_LENGTH);
    if (img_flashcount == IMAGE_ISROM) {
        printf("ROM image.\n");
    } else {
        printf("Flashed %d times.\n", img_flashcount);
    }

    memcpy(&img_rootstart, buffer + IMAGE_ROOTPTR_OFFSET, IMAGE_ROOTPTR_LENGTH);
    printf("Root directory starts at: %p\n", img_rootstart);

    walkpath(img_rootstart, "", &buffer);

    free(buffer);
    return(0);
}