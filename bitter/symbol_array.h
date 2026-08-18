#pragma once

#include <stdint.h>

#define ARRAY_CAPACITY 255
#define EMPTY_CELL UINT8_MAX

uint8_t *array_alloc (size_t _size);
void array_free (uint8_t **array_o);
