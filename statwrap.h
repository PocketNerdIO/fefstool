#ifndef LIB_STATWRAP
#define LIB_STATWRAP

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef _WIN32

#include <windows.h>

BOOL fileexists(LPCTSTR szPath);
BOOL direxists(LPCTSTR szPath);
BOOL fsitemexists(LPCTSTR szPath);

#else // _WIN32

#include <stdbool.h>
#include <sys/stat.h>

bool fileexists(const char *filename);
bool direxists(const char *filename);
bool fsitemexists(const char *filename);

#endif // _WIN32

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // LIB_STATWRAP