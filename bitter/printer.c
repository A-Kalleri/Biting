#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

void write_stderr (const char *fmt, ...) {

        va_list args;
        va_start(args, fmt);

        vfprintf(stderr, fmt, args);
        fflush(stderr);

        va_end(args);

}

void write_stdout_char (const uint8_t c) {
        printf("%d", c);
        fflush(stdout);
}
