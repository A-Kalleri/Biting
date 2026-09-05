#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>

void write_stderr (const char *fmt, ...) {

        va_list args;
        va_start(args, fmt);

        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\nABORT.\n");
        fflush(stderr);

        va_end(args);

}

void write_stderr_info (size_t line, size_t column, const char *fmt, ...) {

        va_list args;
        va_start(args, fmt);

        vfprintf(stderr, fmt, args);
        fprintf(stderr, "Error Detected on Line[%zu], Column[%zu]\nABORT.\n", line, column);
        fflush(stderr);

        va_end(args);

}

void write_stdout_char (const uint8_t c) {
        printf("%d", c);
        fflush(stdout);
}

uint8_t read_stdin_bit (void) {

        uint8_t c;
        scanf(" %c", &c);

        if ((char)c == '0') {
                return 0;
        } else if ((char)c == '1') {
                return 1;
        } else {
                return UINT8_MAX;
        }

}
