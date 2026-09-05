#include <stdlib.h>
#include <limits.h>

#include "reader.h"
#include "lex.h"
#include "helper.h"

#define RD_ERR 1011
#define RD_EOF 1012
#define RD_OKK 1013

static const uint8_t IS_SPACE[256] = {

        [' ']   = 1,
        ['\n']  = 1,
        ['\r']  = 1,
        ['\t']  = 1,

};

static const uint8_t IS_NUM[256] = {

        ['0'] = 1,
        ['1'] = 1,
        ['2'] = 1,
        ['3'] = 1,
        ['4'] = 1,
        ['5'] = 1,
        ['6'] = 1,
        ['7'] = 1,
        ['8'] = 1,
        ['9'] = 1,

};

static inline token_t getTokEof (void) {

        return (token_t) {
                .type = TOKEN_EOF,
                .value = UINT8_MAX,
        };

}

static inline token_t getTokRderr (void) {

        return (token_t) {
                .type = TOKEN_RD_ERR,
                .value = UINT8_MAX,
        };

}

static inline token_t getTokUnknown (int value) {

        token_t tok = {
                .type = TOKEN_UNKNOWN,
                .value = value,
        };
        return tok;

}

lex_t *lex_constructor (const char *filename) {

        lex_t *_lex_image = calloc(1, sizeof(*_lex_image));
        if (!_lex_image) {
                return NULL;
        }

        _lex_image -> source = source_buffer_constructor(256, filename);
        if (!_lex_image -> source) {
                free(_lex_image);
                return NULL;
        }

        _lex_image -> line_static = 1;
        _lex_image -> column_static = 1;

        return _lex_image;

}

void lex_destructor (lex_t **lex_o) {

        if (!lex_o || !*lex_o) {
                return;
        }

        source_buffer_destructor(&(*lex_o) -> source);
        free(*lex_o);
        *lex_o = NULL;

}

static int getChar (source_buf_t *sbuf) {

        /* Requires buf_status() == RD_OKK.
         * Otherwise, buffer[position] may be out of bounds.
         */

        return sbuf -> buffer[sbuf -> position];

}

static int bufStatus (source_buf_t *sbuf) {

        if (sbuf -> position < sbuf -> length) {
                return RD_OKK;
        }

        if (read_to_buffer(sbuf) != 0) {
                return RD_ERR;
        }

        if (sbuf -> length == 0) {
                return RD_EOF;
        }

        return RD_OKK;

}

static int skipSpaces (lex_t *lex_o) {

        for (;;) {

                int status = bufStatus(lex_o -> source);
                if (status != RD_OKK) {
                        return status;
                }

                if (!IS_SPACE[getChar(lex_o -> source)]) {
                        return 0;
                }

                if (getChar(lex_o -> source) == '\n') {
                        lex_o -> line_static++;
                        lex_o -> column_static = 1;
                        continue;
                }

                consume(lex_o -> source);
                lex_o -> column_static++;

        }

}

static int skipComment (lex_t *lex_o) {

        consume(lex_o -> source); // ';'
        lex_o -> column_static++;

        for (;;) {

                int status = bufStatus(lex_o -> source);
                if (status != RD_OKK) {
                        return status;
                }

                if (getChar(lex_o -> source) == '\n') {
                        consume(lex_o -> source);
                        lex_o -> line_static++;
                        lex_o -> column_static = 1;
                        return 0;
                }

                consume(lex_o -> source);
                lex_o -> column_static++;

        }

}

static int lexIdentifier (lex_t *lex_o, uint8_t *value) {

        int found_digit = 0;
        *value = 0;
        consume(lex_o -> source); // 'x'
        lex_o -> column_static++;

        for (;;) {

                int status = bufStatus(lex_o -> source);
                if (status == RD_EOF) {
                        return found_digit ? 0 : LEX_NO_IDENTIFIER_NAME;
                }
                if (status == RD_ERR) {
                        return RD_ERR;
                }

                int c = getChar(lex_o -> source);
                if (!IS_NUM[c]) {
                        return found_digit ? 0 : LEX_NO_IDENTIFIER_NAME;
                }

                if (*value > UINT8_MAX / 10 || (*value == UINT8_MAX / 10 && c - '0' > UINT8_MAX % 10)) {
                        return LEX_IDENTIFIER_OVERFLOW; // overflow
                }

                *value = *value * 10 + (c - '0');

                found_digit = 1;
                consume(lex_o -> source);
                lex_o -> column_static++;

        }

        return found_digit ? 0 : LEX_NO_IDENTIFIER_NAME;

}

token_t lex_next (lex_t *lex_o) {

        token_t tok;

        for (;;) {

                int status = skipSpaces(lex_o);

                if (status == RD_EOF) {
                        return getTokEof();
                }

                if (status == RD_ERR) {
                        write_stderr("LEX ERROR: Read_Error.\n Inside space skipping.\nABORT.\n");
                        lex_o -> error_code = LEX_READ_ERROR;
                        return getTokRderr();
                }

                if (getChar(lex_o -> source) != ';') {
                        break;
                }

                status = skipComment(lex_o);

                if (status == RD_EOF) {
                        return getTokEof();
                }

                if (status == RD_ERR) {
                        write_stderr("LEX ERROR: Read_Error.\n Inside comment skipping.\nABORT.\n");
                        lex_o -> error_code = LEX_READ_ERROR;
                        return getTokRderr();
                }

        }

        /* Safe to access once after the loop:
         * skip_spaces() maintains a valid buffer position.
         */

        switch (getChar(lex_o -> source)) {

        case '1':
                tok.type = TOKEN_HIGH;
                tok.value = '1';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '0':
                tok.type = TOKEN_LOW;
                tok.value = '0';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '|':
                tok.type = TOKEN_OR;
                tok.value = '|';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '&':
                tok.type = TOKEN_AND;
                tok.value = '&';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '^':
                tok.type = TOKEN_XOR;
                tok.value = '^';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '~':
                tok.type = TOKEN_NOT;
                tok.value = '~';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '!':
                tok.type = TOKEN_NOT;
                tok.value = '!';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '>':
                tok.type = TOKEN_FEED;
                tok.value = '>';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '<':
                tok.type = TOKEN_SHOW;
                tok.value = '<';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '.':
                tok.type = TOKEN_RST_LHS;
                tok.value = '.';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        case '?':
                tok.type = TOKEN_READ;
                tok.value = '?';
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;
        
        case '*':
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                if (
                        getChar(lex_o -> source) == 'P' ||
                        getChar(lex_o -> source) == 'p'
                ) {
                        tok.type = TOKEN_REG_PRNT;
                        tok.value = 'p';
                        consume(lex_o -> source);
                        lex_o -> column_static++;
                        return tok;
                } else if (
                        getChar(lex_o -> source) == 'R' ||
                        getChar(lex_o -> source) == 'r'
                ) {
                        tok.type = TOKEN_REG_READ;
                        tok.value = 'r';
                        consume(lex_o -> source);
                        lex_o -> column_static++;
                        return tok;
                }

                write_stderr("LEX ERROR: Unknown_Register_Named.\nWith value: '%c'.\nABORT.\n", getChar(lex_o -> source));
                return getTokUnknown('r');

        case 'x': {
                uint8_t value = 0;

                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;

                int status = lexIdentifier(lex_o, &value);
                if (status == RD_ERR) {
                        return getTokRderr();
                }

                if (status == LEX_IDENTIFIER_OVERFLOW) { // overflow
                        write_stderr("LEX ERROR: Identifier_Overflow: '%d...',\nGreater than bucket <INT(8): (%d)>.\nABORT.\n", value, UINT8_MAX);
                        lex_o -> error_code = LEX_IDENTIFIER_OVERFLOW;
                        return getTokUnknown('x');
                }

                if (status == LEX_NO_IDENTIFIER_NAME) {
                        write_stderr("LEX ERROR: Identifier_Has_No_Name.\nABORT.\n");
                        lex_o -> error_code = LEX_NO_IDENTIFIER_NAME;
                        return getTokUnknown('x');
                }

                tok.type = TOKEN_IDENTIFIER;
                tok.value = value;
                return tok;

        }

        default:
                tok = getTokUnknown(lex_o -> source -> buffer[lex_o -> source -> position]);
                write_stderr("LEX ERROR: Unknown_Token_Detected.\nABORT.\n");
                lex_o -> error_code = LEX_UNKNOWN_TOKEN;
                tok.line = lex_o -> line_static;
                tok.column = lex_o -> column_static;
                consume(lex_o -> source);
                lex_o -> column_static++;
                return tok;

        }

}
