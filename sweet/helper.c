#include <stdio.h>
#include <stdarg.h>

#include "helper.h"

void write_stderr (const char *fmt, ...) {

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\nABORT.\n");
    fflush(stderr);

    va_end(args);

}

void write_stdout (const char *fmt, ...) {

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    fflush(stderr);

    va_end(args);

}