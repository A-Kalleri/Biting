#include <stdio.h>
#include <stdarg.h>

void write_stderr (const char *fmt, ...) {

        va_list args;
        va_start(args, fmt);

        vfprintf(stderr, fmt, args);
        fflush(stderr);

        va_end(args);

}
