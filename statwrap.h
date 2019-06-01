#ifndef LIB_STATWRAP
#define LIB_STATWRAP

// #include <stdio.h>

#ifdef _WIN32

#include <windows.h>

BOOL fileexists(LPCTSTR szPath);
BOOL direxists(LPCTSTR szPath);
BOOL fsitemexists(LPCTSTR szPath);

#else

#include <stdbool.h>
#include <sys/stat.h>

bool fileexists(const char *filename);
bool direxists(const char *filename);
bool fsitemexists(const char *filename);

#endif

#endif