#include <stdlib.h>
#include <string.h>

#include "symbol_array.h"

uint8_t *array_alloc (size_t _size) {

        uint8_t *_array_image = malloc(_size * sizeof(*_array_image));
        if (!_array_image) {
                return NULL;
        }

        memset(_array_image, EMPTY_CELL, _size);

        return _array_image;

}

void array_free (uint8_t **array_o) {

        if (!array_o || !*array_o) {
                return;
        }

        free(*array_o);
        *array_o = NULL;

}
