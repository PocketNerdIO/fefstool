#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define FLASH_TYPE    0xf1a5
#define IMAGE_IS_ROM  0xffffffff

#define IMAGE_STARTOF_ROOTPTR    11
#define IMAGE_STARTOF_NAME       14
#define IMAGE_STARTOF_FLASHCOUNT 25

#define IMAGE_OFFSET_NEXTNODE         0
#define IMAGE_OFFSET_NAME             3
#define IMAGE_OFFSET_EXT              11
#define IMAGE_OFFSET_FLAGS            14
#define IMAGE_OFFSET_FIRSTENTRYRECORD 15
#define IMAGE_OFFSET_ALTRECORD        18
#define IMAGE_OFFSET_ENTRYPROPERTIES  21
#define IMAGE_OFFSET_TIME             22
#define IMAGE_OFFSET_DATE             24
#define IMAGE_OFFSET_FIRSTDATARECORD  26
#define IMAGE_OFFSET_FIRSTDATALEN     29

#define NODE_FLAG_ENTRYVALID              1
#define NODE_FLAG_PROPERTIESDATETIMEVALID 2
#define NODE_FLAG_ISFILE                  4
#define NODE_FLAG_NOENTRYRECORD           8
#define NODE_FLAG_NOALTRECORD             16
#define NODE_FLAG_LASTENTRY               32
#define NODE_FLAG_BIT6                    64
#define NODE_FLAG_BIT7                    128

#define NODE_PROPERTY_ISREADONLY   1
#define NODE_PROPERTY_ISHIDDEN     2
#define NODE_PROPERTY_SYSTEM       4
#define NODE_PROPERTY_ISVOLUMENAME 8
#define NODE_PROPERTY_ISDIRECTORY  16
#define NODE_PROPERTY_ISMODIFIED   32


void walkpath(int pos, char path[], char *buffer[]) {
    char node_name[9], node_ext[4];
    int node_flags;

    printf("Dir starts at: %p\n", (pos + 3));
    memcpy(node_name, *buffer + (pos + IMAGE_OFFSET_NAME), 8);
    node_name[8] = '\0';
    memcpy(node_ext, *buffer + (pos + IMAGE_OFFSET_EXT), 3);
    node_ext[3] = '\0';
    printf("\n=================\n");
    printf("DIR: %s", node_name);
    if(strncmp(node_ext, "   ", 3) != 0) {
        printf(".%s", node_ext);
    }
    printf("\n");
    memcpy(&node_flags, *buffer + (pos + IMAGE_OFFSET_FLAGS), 1);
    printf("Flags: %p\n", node_flags);
    if (node_flags & 4) {
        printf("Is a file.\n");
    } else {
        printf("Is a diretory.\n");
    }
    if (node_flags & 8) {
        printf("Entry record!\n");
    } else {
        printf("No entry record!\n");
    }

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
    } else if(argv[2] != NULL) {
        printf("%s: too many arguments\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fp = fopen(argv[1], "rb");

    fread(&img_type, 2, 1, fp);

    if(img_type != FLASH_TYPE) {
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
    memcpy(img_name, buffer + IMAGE_STARTOF_NAME, 11);
    img_name[11] = 0x0;
    printf("Image name: %s\n", img_name);
    printf("Length: %d\n", file_len);

    // Fetch ROM Size
    memcpy(&img_flashcount, buffer + IMAGE_STARTOF_FLASHCOUNT, 4);
    if(img_flashcount == IMAGE_IS_ROM) {
        printf("ROM image.\n");
    } else {
        printf("Flashed %d times.\n", img_flashcount);
    }

    memcpy(&img_rootstart, buffer + IMAGE_STARTOF_ROOTPTR, 3);
    printf("Root directory starts at: %p\n", img_rootstart);

    walkpath(img_rootstart, "", &buffer);

    free(buffer);
    return(0);
}