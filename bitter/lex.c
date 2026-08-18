#include <stdlib.h>
#include <limits.h>

#include "reader.h"
#include "lex.h"
#include "printer.h"

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

static const char *TYPE_TO_STRING[256] = {

       [TOKEN_UNKNOWN]    = "Unknown",
       [TOKEN_HIGH]       = "One",
       [TOKEN_LOW]        = "Zero",
       [TOKEN_OR]         = "OR",
       [TOKEN_AND]        = "AND",
       [TOKEN_XOR]        = "XOR",
       [TOKEN_NOT]        = "NOT",
       [TOKEN_FEED]       = "Feed",
       [TOKEN_READ]       = "Read",
       [TOKEN_IDENTIFIER] = "Identifier",
       [TOKEN_EOF]        = "End of file",
       [TOKEN_RD_ERR]     = "Read error",

};

const char *tok_to_s (token_type_t type) {
        return TYPE_TO_STRING[type];
}

static inline token_t get_tok_eof (void) {

        return (token_t) {
                .type = TOKEN_EOF,
                .value = -1,
        };

}

static inline token_t get_tok_rderr (void) {

        return (token_t) {
                .type = TOKEN_RD_ERR,
                .value = -1,
        };

}

static inline token_t get_tok_unknown (int value) {

        token_t tok = {
                .type = TOKEN_UNKNOWN,
                .value = value,
        };
        return tok;

}

lex_t *lex_constructor () {

        lex_t *_lex_image = calloc(1, sizeof(*_lex_image));
        if (!_lex_image) {
                return NULL;
        }

        _lex_image -> source = source_buffer_constructor(3, "test");
        if (!_lex_image -> source) {
                free(_lex_image);
                return NULL;
        }

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

static int get_char (source_buf_t *sbuf) {

        /* Requires buf_status() == RD_OKK.
         * Otherwise, buffer[position] may be out of bounds.
         */

        return sbuf -> buffer[sbuf -> position];

}

#define RD_ERR 1011
#define RD_EOF 1012
#define RD_OKK 1013

static int buf_status (source_buf_t *sbuf) {

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

static int skip_spaces (lex_t *lex_o) {

        for (;;) {

                int status = buf_status(lex_o -> source);
                if (status != RD_OKK) {
                        return status;
                }

                if (!IS_SPACE[get_char(lex_o -> source)]) {
                        return 0;
                }

                consume(lex_o -> source);

        }

}

static int skip_comment (lex_t *lex_o) {

        consume(lex_o -> source); // ';'

        for (;;) {

                int status = buf_status(lex_o -> source);
                if (status != RD_OKK) {
                        return status;
                }

                if (get_char(lex_o -> source) == '\n') {
                        consume(lex_o -> source);
                        return 0;
                }

                consume(lex_o -> source);

        }

}

static int lex_identifier (lex_t *lex_o, int *value) {

        *value = -1;
        consume(lex_o -> source); // 'x'

        for (;;) {

                int status = buf_status(lex_o -> source);
                if (status == RD_EOF) {
                        return 0;
                }
                if (status == RD_ERR) {
                        return RD_ERR;
                }

                int c = get_char(lex_o -> source);
                if (!IS_NUM[c]) {
                        return 0;
                }

                if (*value < 0) {
                        *value = 0;
                }

                if (*value > INT_MAX / 10 || (*value == INT_MAX / 10 && c - '0' > INT_MAX % 10)) {
                        return LEX_IDENTIFIER_OVERFLOW; // overflow
                }

                *value = *value * 10 + (c - '0');

                consume(lex_o -> source);

        }

}

token_t lex_next (lex_t *lex_o) {

        token_t tok;

        for (;;) {

                int status = skip_spaces(lex_o);

                if (status == RD_EOF) {
                        return get_tok_eof();
                }

                if (status == RD_ERR) {
                        write_stderr("LEX ERROR: Read_Error.\n Inside space skipping.\nABORT.\n");
                        lex_o -> error_code = LEX_READ_ERROR;
                        return get_tok_rderr();
                }

                if (get_char(lex_o -> source) != ';') {
                        break;
                }

                status = skip_comment(lex_o);

                if (status == RD_EOF) {
                        return get_tok_eof();
                }

                if (status == RD_ERR) {
                        write_stderr("LEX ERROR: Read_Error.\n Inside comment skipping.\nABORT.\n");
                        lex_o -> error_code = LEX_READ_ERROR;
                        return get_tok_rderr();
                }

        }

        /* Safe to access once after the loop:
         * skip_spaces() maintains a valid buffer position.
         */

        switch (get_char(lex_o -> source)) {

        case '1':
                tok.type = TOKEN_HIGH;
                tok.value = '1';
                consume(lex_o -> source);
                return tok;

        case '0':
                tok.type = TOKEN_LOW;
                tok.value = '0';
                consume(lex_o -> source);
                return tok;

        case '|':
                tok.type = TOKEN_OR;
                tok.value = '|';
                consume(lex_o -> source);
                return tok;

        case '&':
                tok.type = TOKEN_AND;
                tok.value = '&';
                consume(lex_o -> source);
                return tok;

        case '^':
                tok.type = TOKEN_XOR;
                tok.value = '^';
                consume(lex_o -> source);
                return tok;

        case '~':
                tok.type = TOKEN_NOT;
                tok.value = '~';
                consume(lex_o -> source);
                return tok;

        case '!':
                tok.type = TOKEN_NOT;
                tok.value = '!';
                consume(lex_o -> source);
                return tok;

        case '>':
                tok.type = TOKEN_FEED;
                tok.value = '>';
                consume(lex_o -> source);
                return tok;

        case '<':
                tok.type = TOKEN_READ;
                tok.value = '<';
                consume(lex_o -> source);
                return tok;
        
        case '.':
                tok.type = TOKEN_RST_LHS;
                tok.value = '.';
                consume(lex_o -> source);
                return tok;

        case 'x': {
                int value = -1;

                int status = lex_identifier(lex_o, &value);
                if (status == RD_ERR) {
                        return get_tok_rderr();
                }

                if (status == LEX_IDENTIFIER_OVERFLOW) { // overflow
                        write_stderr("LEX ERROR: Identifier_Overflow: '%d'.\nGreater than bucket <INT: (%d)>.\nABORT.\n", value, INT_MAX);
                        lex_o -> error_code = LEX_IDENTIFIER_OVERFLOW;
                        return get_tok_unknown('x');
                }

                if (value < 0) {
                        write_stderr("LEX ERROR: Identifier_Has_No_Name.\nABORT.\n");
                        lex_o -> error_code = LEX_NO_IDENTIFIER_NAME;
                        return get_tok_unknown('x');
                }

                tok.type = TOKEN_IDENTIFIER;
                tok.value = value;
                return tok;

        }

        default:
                tok = get_tok_unknown(lex_o -> source -> buffer[lex_o -> source -> position]);
                write_stderr("LEX ERROR: Unknown_Token_Detected.\nABORT.\n");
                lex_o -> error_code = LEX_UNKNOWN_TOKEN;
                consume(lex_o -> source);
                return tok;

        }

}
