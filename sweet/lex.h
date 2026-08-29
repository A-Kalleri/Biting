#pragma once

#include "reader.h"
#include <stdint.h>

/* Lexer magic numbers */
#define LEX_INIT_FAIL                   84824711 //enetfae-el
#define LEX_IDENTIFIER_OVERFLOW         9165119 // ovarflo
#define LEX_NO_IDENTIFIER_NAME          1076116 // perella
#define LEX_UNKNOWN_TOKEN               6287116 // areyela
#define LEX_READ_ERROR                  1674116 // vaynala

typedef enum {

        TOKEN_UNKNOWN,
        TOKEN_BIT,
        TOKEN_NIBBLE,
        TOKEN_BYTE,
        TOKEN_WORD,
        TOKEN_DWORD,
        TOKEN_QWORD,
        TOKEN_PARAGRAPH,
        TOKEN_ASCII,
        TOKEN_HIGH,
        TOKEN_LOW,
        TOKEN_REG_PRINT,
        TOKEN_REG_LOOP,
        TOKEN_REG_RETURN,
        TOKEN_OR,
        TOKEN_AND,
        TOKEN_XOR,
        TOKEN_NOT,
        TOKEN_FEED,
        TOKEN_READ,
        TOKEN_READ_LHS,
        TOKEN_READ_HIGH,
        TOKEN_READ_LOW,
        TOKEN_READ_BIT,
        TOKEN_READ_NIBBLE,
        TOKEN_READ_BYTE,
        TOKEN_READ_WORD,
        TOKEN_READ_DWORD,
        TOKEN_READ_QWORD,
        TOKEN_READ_PARAGRAPH,
        TOKEN_RST_LHS,
        TOKEN_LPARAN,
        TOKEN_RPARAN,
        TOKEN_LCURLY,
        TOKEN_RCURLY,
        TOKEN_ERR,
        TOKEN_EOF,

} token_type_t;

typedef struct {
        token_type_t type;
        uint8_t value;
} token_t;

typedef struct source_buf source_buf_t;

typedef struct {

        source_buf_t *source;
        token_t current;
        token_t lookahead;
        int error_code;

} lexout_t;

lexout_t *lexout_constructor (void);
void lexout_destructor (lexout_t **lexout_o);

int lexout_init (lexout_t *lexout_o, const char *filename);

int lex_next (lexout_t *lexout_o);
