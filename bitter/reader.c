#include <stdio.h>
#include <stdlib.h>

#include "reader.h"

source_buf_t *source_buffer_constructor (size_t _size, const char *filename) {

    source_buf_t *_source_buf_image = calloc(1, sizeof(*_source_buf_image));
    if (!_source_buf_image) {
        return NULL;
    }

    _source_buf_image -> file_ptr = fopen(filename, "rb");
    if (_source_buf_image -> file_ptr == NULL) {
        free(_source_buf_image);
        return NULL;
    }

    _source_buf_image -> buffer = malloc(_size);
    if (!_source_buf_image -> buffer) {
        free(_source_buf_image);
        fclose(_source_buf_image -> file_ptr);
        return NULL;
    }

    _source_buf_image -> capacity = _size;
    return _source_buf_image;

}

void source_buffer_destructor (source_buf_t **source_buf_o) {

    if (!source_buf_o || !*source_buf_o) {
        return;
    }

    free((*source_buf_o) -> buffer);
    fclose((*source_buf_o) -> file_ptr);
    free(*source_buf_o);
    *source_buf_o = NULL;

}

int read_to_buffer (source_buf_t *source_buf_o) {

    if (!source_buf_o) {
        return 1;
    }

    source_buf_o -> length = fread(source_buf_o -> buffer, 1, source_buf_o -> capacity, source_buf_o -> file_ptr);
    if (ferror(source_buf_o -> file_ptr)) {
        return 1;
    }

    source_buf_o -> position = 0;

    return 0;

}

void consume (source_buf_t *source_buf_o) {
    if (source_buf_o -> position < source_buf_o -> length) {
        source_buf_o -> position++;
    }
}