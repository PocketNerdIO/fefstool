#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define FLASH_TYPE    0xf1a5
#define IMAGE_IS_ROM  0xffffffff
#define IMAGE_STARTOF_ROOTPOINTER 11
#define IMAGE_STARTOF_NAME        14
#define IMAGE_STARTOF_FLASHCOUNT  25


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

    memcpy(&img_rootstart, buffer + IMAGE_STARTOF_ROOTPOINTER, 3);
    printf("Root directory starts at: 0x%X\n", img_rootstart);

    free(buffer);
    return(0);
}