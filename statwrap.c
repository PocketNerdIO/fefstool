// Wrapper for stat.h (POSIX) and GetFileAttributes (WIN32)
// By Alex Brown (PocketNerdIO)
#include "statwrap.h"

// Does something exist AND is it a file (not a directory)
#ifdef _WIN32
BOOL fileexists(LPCTSTR szPath) {
    DWORD dwAttrib = GetFileAttributes(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
#else
bool fileexists(const char *filename){
    struct stat path_stat;
    return (stat(filename, &path_stat) == 0 && S_ISREG(path_stat.st_mode));
}
#endif

// Does something exist AND is it a directory (not a file)
#ifdef _WIN32
BOOL direxists(LPCTSTR szPath) {
    DWORD dwAttrib = GetFileAttributes(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
#else
bool direxists(const char *filename){
    struct stat path_stat;
    return (stat(filename, &path_stat) == 0 && S_ISDIR(path_stat.st_mode));
}
#endif

// Does something exist (don't care what it is)
#ifdef _WIN32
BOOL fsitemexists(LPCTSTR szPath) {
    DWORD dwAttrib = GetFileAttributes(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}
#else
bool fsitemexists(const char *filename){
    struct stat path_stat;
    return (stat(filename, &path_stat) == 0);
}
#endif