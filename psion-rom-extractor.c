#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <utime.h>

#ifdef _WIN32
    #include <windows.h>
#else
    // Assume it's something POSIX-compliant
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

#include "argparse/argparse.h"
static const char *const usage[] = {
    "psirom [options] [[--] args]",
    "psirom [options]",
    NULL,
};

#define FLASH_TYPE   0xf1a5
#define IMAGE_ISROM  0xffffffff
#define NULL_PTR     0xffffff

#define IMAGE_ROOTPTR_OFFSET        11
#define IMAGE_ROOTPTR_LENGTH        3
#define IMAGE_NAME_OFFSET           14
#define IMAGE_NAME_LENGTH           11
#define IMAGE_FLASHCOUNT_OFFSET     25
#define IMAGE_FLASHCOUNT_LENGTH     4
#define IMAGE_FLASHIDSTRING_OFFSET  33
#define IMAGE_ROMIDSTRING_OFFSET    29

#define ENTRY_NEXTENTRYPTR_OFFSET         0
#define ENTRY_NEXTENTRYPTR_LENGTH         3
#define ENTRY_NAME_OFFSET                 3
#define ENTRY_NAME_LENGTH                 8
#define ENTRY_EXT_OFFSET                  11
#define ENTRY_EXT_LENGTH                  3
#define ENTRY_FLAGS_OFFSET                14
#define ENTRY_FLAGS_LENGTH                1
#define ENTRY_FIRSTENTRYRECORDPTR_OFFSET  15
#define ENTRY_FIRSTENTRYRECORDPTR_LENGTH  3
#define ENTRY_ALTRECORDPTR_OFFSET         18
#define ENTRY_ALTRECORDPTR_LENGTH         3
#define ENTRY_ENTRYPROPERTIES_OFFSET      21
#define ENTRY_ENTRYPROPERTIES_LENGTH      1
#define ENTRY_TIMECODE_OFFSET             22
#define ENTRY_TIMECODE_LENGTH             2
#define ENTRY_DATECODE_OFFSET             24
#define ENTRY_DATECODE_LENGTH             2
#define ENTRY_FIRSTDATARECORDPTR_OFFSET   26
#define ENTRY_FIRSTDATARECORDPTR_LENGTH   3
#define ENTRY_FIRSTDATALEN_OFFSET         29
#define ENTRY_FIRSTDATALEN_LENGTH         2

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
#define FILE_TIMECODE_OFFSET         13
#define FILE_TIMECODE_LENGTH         2
#define FILE_DATECODE_OFFSET         15
#define FILE_DATECODE_LENGTH         2


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

#ifdef _WIN32
BOOL fileexists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
#else
bool fileexists(const char *filename){
    struct stat path_stat;

    return (stat(filename, &path_stat) == 0 && S_ISREG(path_stat.st_mode));
}
#endif


void datedecode(unsigned int date, unsigned int *year, char *month, char *day) {
    *day = date % 0x20;
    *month = (date >> 5) % 0x10;
    *year = (date >> 9) + 1980;
}

void timedecode(unsigned int time, char *hour, char *min, char *sec) {
    *sec = (time % 0x20) * 2;
    *min = (time >> 5) % 0x40;
    *hour = (time >> 11);
}

void getfile(int pos, char path[], char *buffer[], const char localpath[]) {
    FILE *fp;
    unsigned int cur_data_ptr = 0, cur_data_len = 0;
    unsigned int cur_pos = pos, next_pos = 0, file_len = 0;
    char file_flags;
    unsigned int entry_count = 0;

    printf("File scanning...\n");

    memcpy(&file_flags, *buffer + (cur_pos + FILE_FLAGS_OFFSET), FILE_FLAGS_LENGTH);

    remove(localpath);
    fp = fopen(localpath, "wb");

    while (1) {
        entry_count++;
        printf("Entry %d:\n", entry_count);
        if (entry_count == 1) {
            memcpy(&cur_data_ptr, *buffer + (cur_pos + ENTRY_FIRSTDATARECORDPTR_OFFSET), ENTRY_FIRSTDATARECORDPTR_LENGTH);
            memcpy(&cur_data_len, *buffer + (cur_pos + ENTRY_FIRSTDATALEN_OFFSET), ENTRY_FIRSTDATALEN_LENGTH);
            memcpy(&file_flags, *buffer + (cur_pos + ENTRY_FLAGS_OFFSET), ENTRY_FLAGS_LENGTH);
            memcpy(&next_pos, *buffer + (cur_pos + ENTRY_FIRSTENTRYRECORDPTR_OFFSET), ENTRY_FIRSTENTRYRECORDPTR_LENGTH);
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
            printf ("Last entry flag set.\n");
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
    char entry_name[9], entry_ext[4], entry_filename[13];
    char newpath[128];
    char entry_flags;
    unsigned int date = 0, time = 0;
    char day = 0, month = 0, hour = 0, min = 0, sec = 0;
    unsigned int year = 0;
    unsigned int first_entry_ptr = 0, next_entry_ptr = 0;
    bool is_last_entry, is_file;
    char localpath[256];
    struct stat st = {0};
    struct tm tm;
    time_t unixtime;
    char timeresult[30];

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

        is_file = (entry_flags & ENTRY_FLAG_ISFILE);
        is_last_entry = (entry_flags & ENTRY_FLAG_ISLASTENTRY);

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
            memcpy(&date, *buffer + (pos + ENTRY_DATECODE_OFFSET), ENTRY_DATECODE_LENGTH);
            datedecode(date, &year, &month, &day);
            memcpy(&time, *buffer + (pos + ENTRY_TIMECODE_OFFSET), ENTRY_TIMECODE_LENGTH);
            timedecode(time, &hour, &min, &sec);
            printf("Timestamp: %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, min, sec);

            tm.tm_year = year;
            tm.tm_mon = month;
            tm.tm_mday = day;
            tm.tm_hour = hour;
            tm.tm_min = min;
            tm.tm_sec = sec;
            unixtime = mktime(&tm);
            strftime(timeresult, sizeof(timeresult), "%Y-%m-%d %H:%M:%S", &tm);
            printf("Checktime: %s\n", timeresult);


            if (!(entry_flags & ENTRY_FLAG_NOALTRECORD)) {
                printf("Has an alternative record.\n");
            }

            memcpy(&first_entry_ptr, *buffer + (pos + ENTRY_FIRSTENTRYRECORDPTR_OFFSET), ENTRY_FIRSTENTRYRECORDPTR_LENGTH);
            printf("First Entry Pointer: 0x%06x\n", first_entry_ptr);
            if (is_file) {
                strcpy(localpath, img_name);
                strcat(localpath, path);
                strcat(localpath, entry_filename);
                printf("File to be made: %s\n", localpath);
                getfile(pos, path, buffer, localpath);
            } else { // it's a directory
                if (strlen(path)) {
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
        memcpy(&pos, *buffer + (pos + ENTRY_NEXTENTRYPTR_OFFSET), ENTRY_NEXTENTRYPTR_LENGTH);
        printf("\n");
    }
}

int main(int argc, const char **argv) {
    FILE *fp;
    int i, c;
    long file_len;
    char *buffer;
    char img_name[12];
    char volume_id[33];
    unsigned short img_type;
    unsigned int img_flashcount = 0;
    unsigned int img_rootstart = 0; // TODO: Will probably replace this with something more flexible later.

    char called_with[strlen(argv[0] + 1)];
    bool only_list;
    bool ignore_attributes;


    strcpy(called_with, argv[0]);
    strcat(called_with, "\0");

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('l', "list", &only_list, "only list the contents of the image, don't extract files"),
        OPT_BOOLEAN('n', "no-attributes", &ignore_attributes, "ignore file attributes"),
        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nExtracts files from Psion SIBO Flash and ROM SSD images.", "");
    argc = argparse_parse(&argparse, argc, argv);

    if (argv[0] == NULL) {
        printf("%s: missing argument\n", called_with);
        exit(EXIT_FAILURE);
    } else if (argc > 1) {
        printf("%s: too many arguments\n", called_with);
        exit(EXIT_FAILURE);
    }

    if (!fileexists(argv[0])) {
        printf("%s: %s: file not found\n", called_with, argv[0]);
        exit(EXIT_FAILURE);
    }

    fp = fopen(argv[0], "rb");
    fread(&img_type, 2, 1, fp);

    if (img_type != FLASH_TYPE) {
        printf("%s: %s: Not a Psion Flash or ROM SSD image.\n", called_with, argv[0]);
        fclose(fp);
        exit(EXIT_FAILURE); 
    }

    // Get file size
    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp);
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

    // Fetch Flash Count (or identify as ROM)
    memcpy(&img_flashcount, buffer + IMAGE_FLASHCOUNT_OFFSET, IMAGE_FLASHCOUNT_LENGTH);
    if (img_flashcount == IMAGE_ISROM) {
        printf("ROM image.\n");
        memcpy(volume_id, buffer + IMAGE_ROMIDSTRING_OFFSET, 32);
    } else {
        printf("Flashed %d times.\n", img_flashcount);
        memcpy(volume_id, buffer + IMAGE_FLASHIDSTRING_OFFSET, 32);
    }
    for (i = 0; i <=32; i++) {
        if ((unsigned char) volume_id[i] == 0xFF) {
            printf("0xFF found at %d\n", i);
            volume_id[i] = 0;
            break;
        }
    }
    volume_id[32] = 0;
    printf("Volume ID: %s\n", volume_id);

    memcpy(&img_rootstart, buffer + IMAGE_ROOTPTR_OFFSET, IMAGE_ROOTPTR_LENGTH);
    printf("Root directory starts at: 0x%06x\n\n", img_rootstart);

    walkpath(img_rootstart, "", &buffer, img_name);

    free(buffer);
    return(0);
}