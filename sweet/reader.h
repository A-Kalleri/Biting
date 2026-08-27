#pragma once

#include <stdint.h>

typedef struct file file_t;

typedef struct {

        file_t *source_file;
        uint8_t *buffer;
        size_t capacity;
        size_t position;
        size_t length;

} source_buf_t;


source_buf_t *source_buf_constructor (size_t _size);
void source_buf_destructor (source_buf_t **sbuf_o);

int load_file (source_buf_t *sbuf_o, const char *filename);
int read_to_buffer (source_buf_t *sbuf_o);

void consume (source_buf_t *sbuf_o);