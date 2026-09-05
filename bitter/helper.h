#pragma once

#include <stdint.h>
#include <stddef.h>

void write_stderr (const char *fmt, ...);
void write_stdout_char (const uint8_t c);
void write_stderr_info (size_t line, size_t column, const char *fmt, ...);

uint8_t read_stdin_bit (void);
