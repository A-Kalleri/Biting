#include <stdio.h>
#include <stdlib.h>

#include "reader.h"

typedef struct file {
        FILE *fptr;
} file_t;

void source_buf_destructor (source_buf_t **sbuf_o) {

        if (!sbuf_o || !*sbuf_o) {
                return;
        }

        if ((*sbuf_o) -> source_file -> fptr) {
                fclose((*sbuf_o) -> source_file -> fptr);
        }

        free((*sbuf_o) -> source_file);
        free((*sbuf_o) -> buffer);
        free(*sbuf_o);

        *sbuf_o = NULL;

}

source_buf_t *source_buf_constructor (size_t _size) {

        source_buf_t *_sbuf_image = calloc(1, sizeof(*_sbuf_image));
        if (!_sbuf_image) {
                return NULL;
        }

        _sbuf_image -> source_file = malloc(sizeof(*_sbuf_image -> source_file));
        if (!_sbuf_image -> source_file) {
                free(_sbuf_image);
                return NULL;
        }

        _sbuf_image -> buffer = malloc(_size);
        if (!_sbuf_image -> buffer) {
                source_buf_destructor(&_sbuf_image);
                return NULL;
        }

        _sbuf_image -> capacity = _size;
        return _sbuf_image;

}

int load_file (source_buf_t *sbuf_o, const char *filename) {

        sbuf_o -> source_file -> fptr = fopen(filename, "rb");
        if (!sbuf_o -> source_file -> fptr) {
                return 1;
        }
        return 0;

}

int read_to_buffer (source_buf_t *sbuf_o) {

        if (!sbuf_o) {
                return 1;
        }

        sbuf_o -> length = fread(sbuf_o -> buffer, 1, sbuf_o -> capacity, sbuf_o -> source_file -> fptr);
        if (ferror(sbuf_o -> source_file -> fptr)) {
                return 1;
        }

        sbuf_o -> position = 0;
        return 0;

}

void consume (source_buf_t *sbuf_o) {

        if (sbuf_o -> position < sbuf_o -> length) {
                sbuf_o -> position++;
        }

}

int lookahead (source_buf_t *sbuf_o) {

        if (sbuf_o -> position + 1 < sbuf_o -> length) {
                return sbuf_o -> buffer[sbuf_o -> position + 1];
        } else {
                return -1;
        }

}
