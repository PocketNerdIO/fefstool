#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FLASH_TYPE   0xf1a5
#define IMAGE_ISROM  0xffffffff
#define NULL_PTR     0xFFFFFF

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

#define FILE_FLAGS_OFFSET            0
#define FILE_FLAGS_LENGTH            1
#define FILE_NEXTRECORDPTR_OFFSET    1
#define FILE_NEXTRECORDPTR_LENGTH    3
#define FILE_ALTRECORDPTR_OFFSET     4
#define FILE_ALTRECORDPTR_LENGTH     3
#define FILE_DATARECORDPTR_OFFSET    7
#define FILE_DATARECORDPTR_LENGTH    3
#define FILE_DATARECORDLEN_OFFSET    10
#define FILE_DATARECORDLEN_LENGTH    2
#define FILE_ENTRYPROPERTIES_OFFSET  12
#define FILE_ENTRYPROPERTIES_LENGTH  1
#define FILE_TIME_OFFSET             13
#define FILE_TIME_LENGTH             2
#define FILE_DATE_OFFSET             15
#define FILE_DATE_LENGTH             2


char *rtrim(char *s) {
	char *p = s + strlen(s) - 1;
	const char *end = p;
	while (p >= s && isspace((unsigned char) *p)) {
		p--;
	}
	if (p != end) {
        p++;
		*p = '\0';
    }
	return s;
}

void datedecode(int date, int *year, int *month, int *day) {
    *day = date % 0x20;
    *month = (date >> 5) % 0x10;
    *year = (date >> 9) + 1980;
}

void timedecode(int time, int *hour, int *min, int *sec) {
    *sec = (time % 0x20) * 2;
    *min = (time >> 5) % 0x40;
    *hour = (time >> 11);
}

void getfile(int pos, char path[], char *buffer[], const char localpath[]) {
    FILE *fp;
    int cur_data_ptr = 0, cur_data_len = 0;
    int cur_pos = pos, next_pos = 0, file_len = 0;
    char file_flags;
    int entry_count = 0;

    printf("File scanning...\n");

    memcpy(&file_flags, *buffer + (cur_pos + FILE_FLAGS_OFFSET), FILE_FLAGS_LENGTH);

    remove(localpath);
    fp = fopen(localpath, "wb");

    while (1) {
        entry_count++;
        printf("Entry %d:\n", entry_count);
        if (entry_count == 1) {
            memcpy(&cur_data_ptr, *buffer + (cur_pos + ENTRY_FIRSTDATARECORD_OFFSET), ENTRY_FIRSTDATARECORD_LENGTH);
            memcpy(&cur_data_len, *buffer + (cur_pos + ENTRY_FIRSTDATALEN_OFFSET), ENTRY_FIRSTDATALEN_LENGTH);
            memcpy(&file_flags, *buffer + (cur_pos + ENTRY_FLAGS_OFFSET), ENTRY_FLAGS_LENGTH);
            memcpy(&next_pos, *buffer + (cur_pos + ENTRY_FIRSTENTRYRECORD_OFFSET), ENTRY_FIRSTENTRYRECORD_LENGTH);
        } else {
            memcpy(&cur_data_ptr, *buffer + (cur_pos + FILE_DATARECORDPTR_OFFSET), FILE_DATARECORDPTR_LENGTH);
            memcpy(&cur_data_len, *buffer + (cur_pos + FILE_DATARECORDLEN_OFFSET), FILE_DATARECORDLEN_LENGTH);
            memcpy(&file_flags, *buffer + (cur_pos + FILE_FLAGS_OFFSET), FILE_FLAGS_LENGTH);
            memcpy(&next_pos, *buffer + (cur_pos + FILE_NEXTRECORDPTR_OFFSET), FILE_NEXTRECORDPTR_LENGTH);
        }
        printf("Data record: 0x%06x\n", cur_data_ptr);
        printf("Data length: 0x%04x (%d)\n", cur_data_len, cur_data_len);

        file_len += cur_data_len;

        fwrite(*buffer + cur_data_ptr, 1, cur_data_len, fp);

        printf("Next entry record: 0x%06x\n", next_pos);

        if (file_flags & ENTRY_FLAG_NOENTRYRECORD) {
            printf ("Last entry flag set...\n");
        } else {
            printf ("Last entry flag IS NOT set...\n");
        }

        if (!((file_flags & ENTRY_FLAG_NOENTRYRECORD) == 0 && next_pos != NULL_PTR)) {
            printf("End of file.\n");
            break;
        }
        cur_pos = next_pos;
    }
    
    fclose(fp);
    printf("Total file size: %d (in %d records)\n", file_len, entry_count);
}


void walkpath(int pos, char path[], char *buffer[], const char img_name[]) {
    char entry_name[9], entry_ext[4], entry_filename[12];
    char newpath[128];
    char entry_flags;
    int date = 0, day = 0, month = 0, year = 0;
    int time = 0, hour = 0, min = 0, sec = 0;
    int first_entry_ptr = 0, next_entry_ptr = 0;
    bool is_last_entry, is_file;
    char localpath[256];
    struct stat st = {0};

    while (true) {
        memcpy(&entry_flags, *buffer + (pos + ENTRY_FLAGS_OFFSET), ENTRY_FLAGS_LENGTH);
        memcpy(entry_name, *buffer + (pos + ENTRY_NAME_OFFSET), ENTRY_NAME_LENGTH);
        entry_name[8] = 0;
        rtrim(entry_name);
        memcpy(entry_ext, *buffer + (pos + ENTRY_EXT_OFFSET), ENTRY_EXT_LENGTH);
        entry_ext[3] = 0;
        rtrim(entry_ext);

        strcpy(entry_filename, entry_name);
        if (strlen(entry_ext)) {
            strcat(entry_filename, ".");
            strcat(entry_filename, entry_ext);
        }

        if (entry_flags & ENTRY_FLAG_ISFILE) {
            is_file = true;
        } else {
            is_file = false;
        }
        if (entry_flags & ENTRY_FLAG_ISLASTENTRY) {
            is_last_entry = true;
        } else {
            is_last_entry = false;
        }
        

        if (entry_flags & ENTRY_FLAG_ENTRYISVALID) {   
            if (strlen(path)) {
                printf("%s%s", path, entry_filename);
            }

            if (!is_file) {
                printf("/");
            }

            printf(" (");
            if (!strlen(path)) {
                printf("root directory");
            } else if (is_file) {
                printf("file");
            } else {
                printf("directory");
            }

            printf(")\n");
            memcpy(&date, *buffer + (pos + ENTRY_DATE_OFFSET), ENTRY_DATE_LENGTH);
            datedecode(date, &year, &month, &day);
            memcpy(&time, *buffer + (pos + ENTRY_TIME_OFFSET), ENTRY_TIME_LENGTH);
            timedecode(time, &hour, &min, &sec);
            printf("Timestamp: %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, min, sec);

            if (!(entry_flags & ENTRY_FLAG_NOALTRECORD)) {
                printf("Has an alternative record.\n");
            }

            memcpy(&first_entry_ptr, *buffer + (pos + ENTRY_FIRSTENTRYRECORD_OFFSET), ENTRY_FIRSTENTRYRECORD_LENGTH);
            printf("First Entry Pointer: 0x%06x\n", first_entry_ptr);
            if (is_file) {
                strcpy(localpath, img_name);
                strcat(localpath, path);
                strcat(localpath, entry_filename);
                printf("File to be made: %s\n", localpath);
                getfile(pos, path, buffer, localpath);
            } else { // it's a directory
                if(strlen(path)) {
                    strcpy(newpath, path);
                    strcat(newpath, entry_filename);
                    strcat(newpath, "/");
                } else {
                    strcpy(newpath, "/");
                }
                printf("\n");

                strcpy(localpath, img_name);
                strcat(localpath, newpath);
                if (stat(localpath, &st) == -1) {
                    mkdir(localpath, 0700);
                }

                walkpath (first_entry_ptr, newpath, buffer, img_name);
            }
        } else {
            printf("Invalid entry found, skipping ");
            if (is_file) {
                printf ("file");
            } else {
                printf("directory");
            }
            printf(": %s\n", entry_filename);
        }
        if (is_last_entry) {
            return;
        }
        memcpy(&pos, *buffer + (pos + ENTRY_NEXTENTRY_OFFSET), ENTRY_NEXTENTRY_LENGTH);
        printf("\n");
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
    img_name[11] = 0;
    rtrim(img_name);
    printf("Image name: %s\n", img_name);
    printf("Length: %ld bytes\n", file_len);

    // Fetch ROM Size
    memcpy(&img_flashcount, buffer + IMAGE_FLASHCOUNT_OFFSET, IMAGE_FLASHCOUNT_LENGTH);
    if (img_flashcount == IMAGE_ISROM) {
        printf("ROM image.\n");
    } else {
        printf("Flashed %d times.\n", img_flashcount);
    }

    memcpy(&img_rootstart, buffer + IMAGE_ROOTPTR_OFFSET, IMAGE_ROOTPTR_LENGTH);
    printf("Root directory starts at: 0x%06x\n\n", img_rootstart);

    walkpath(img_rootstart, "", &buffer, img_name);

    free(buffer);
    return(0);
}