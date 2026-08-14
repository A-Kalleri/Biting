#pragma once

#include <stdint.h>

typedef struct source_buf source_buf_t;

typedef enum {

        TOKEN_UNKNOWN,
        TOKEN_HIGH,
        TOKEN_LOW,
        TOKEN_OR,
        TOKEN_AND,
        TOKEN_XOR,
        TOKEN_NOT,
        TOKEN_FEED,
        TOKEN_READ,

} token_type_t;

typedef struct {

        token_type_t type;
        const uint8_t *value;
        uint8_t length;

} token_t;

typedef struct {

        source_buf_t *source;
        size_t position;
        size_t line;
        size_t column;

} lex_t;

lex_t *lex_constructor ();
void *lex_destructor (lex_t **lex_o);
token_t lex_next (lex_t *lex_o);
