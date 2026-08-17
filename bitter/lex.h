#pragma once

#include <stdint.h>

/* Lexer magic numbers */
#define LEX_IDENTIFIER_OVERFLOW     9165119 // ovarflo
#define LEX_NO_IDENTIFIER_NAME      1076116 // perella
#define LEX_UNKNOWN_TOKEN           6287116 // areyela
#define LEX_READ_ERROR              1674116 // vaynala

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
        TOKEN_IDENTIFIER,
        TOKEN_EOF,
        TOKEN_RD_ERR,

} token_type_t;

typedef struct {

        token_type_t type;
        int value;

} token_t;

typedef struct {

        source_buf_t *source;
        int error_code;

} lex_t;

const char *tok_to_s (token_type_t type);

lex_t *lex_constructor ();
void lex_destructor (lex_t **lex_o);
token_t lex_next (lex_t *lex_o);
