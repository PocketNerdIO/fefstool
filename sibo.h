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


#ifdef _WIN32
    #include <windows.h>
    const char *slash = "\\";
#else // _WIN32
    // Assume it's something POSIX-compliant
    #include <unistd.h>
    const char *slash = "/";
#endif // _WIN32




struct PsiDateTime {
    uint16_t psi_time;
    uint16_t psi_date;
};

#endif // SIBO_LIB