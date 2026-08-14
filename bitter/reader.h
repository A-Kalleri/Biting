#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct {

    FILE *file_ptr;
    uint8_t *buffer;
    size_t capacity;
    size_t position;
    size_t length;

} source_buf_t;

source_buf_t *source_buffer_constructor (size_t _size, const char *filename);
void source_buffer_destructor (source_buf_t **source_buf_o);
int read_to_buffer (source_buf_t *source_buf_o);
void consume (source_buf_t *source_buf_o);