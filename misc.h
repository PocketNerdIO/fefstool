#ifndef LIB_SIBOMISC
#define LIB_SIBOMISC

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <string.h>
#include <ctype.h>

char *rtrim(char *s);
void printlogf(const int verbosity, const char *format, ...);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // LIB_STATWRAP