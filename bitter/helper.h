#pragma once

#include <stdint.h>

void write_stderr (const char *fmt, ...);
void write_stdout_char (const uint8_t c);

uint8_t read_stdin_bit (void);
