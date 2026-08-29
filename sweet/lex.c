#include <alloca.h>
#include <stdint.h>
#include <stdlib.h>

#include "lex.h"
#include "reader.h"
#include "helper.h"

#define BUF_RD_OKK      411636 // nellatha
#define BUF_RD_ERR      272726 // reatearr
#define BUF_RD_EOF      742677 // eantaeae

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

lexout_t *lexout_constructor (void) {

        lexout_t *_lexout_image = calloc(1, sizeof(*_lexout_image));
        if (!_lexout_image) {
                return NULL;
        }

        _lexout_image -> source = source_buf_constructor(512);
        if (!_lexout_image -> source) {
                free(_lexout_image);
                return NULL;
        }

        return _lexout_image;

}

void lexout_destructor (lexout_t **lexout_o) {

        if (!lexout_o || !*lexout_o) {
                return;
        }

        free(*lexout_o);
        *lexout_o = NULL;

}

int lexout_init (lexout_t *lexout_o, const char *filename) {

        if (load_file(lexout_o -> source, filename) != 0) {
                lexout_o -> error_code = LEX_INIT_FAIL;
                return -1;
        }

        return 0;

}

static int peek(source_buf_t *sbuf) {

        /* Requires bufferStatus() == BUF_RD_OKK.
         * Otherwise, buffer[position] may be out of bounds.
         */

        return sbuf -> buffer[sbuf -> position];

}

static int bufferStatus (source_buf_t *sbuf) {

        if (sbuf -> position < sbuf -> length) {
                return BUF_RD_OKK;
        }

        if (read_to_buffer(sbuf) != 0) {
                return BUF_RD_ERR;
        }

        if (sbuf -> length == 0) {
                return BUF_RD_EOF;
        }

        return BUF_RD_OKK;

}

static int skipSpaces (lexout_t *lexout_o) {

        for (;;) {

                int status = bufferStatus(lexout_o -> source);
                if (status != BUF_RD_OKK) {
                        return status;
                }

                if (!IS_SPACE[peek(lexout_o -> source)]) {
                        return 0;
                }

                consume(lexout_o -> source);

        }

}

static int skipComment (lexout_t *lexout_o) {

        consume(lexout_o -> source); // ';'

        for (;;) {

                int status = bufferStatus(lexout_o -> source);
                if (status != BUF_RD_OKK) {
                        return status;
                }

                if (peek(lexout_o -> source) == '\n') {
                        consume(lexout_o -> source);
                        return 0;
                }

                consume(lexout_o -> source);

        }

}

static inline void prepareEofToken (lexout_t *lexout_o) {
        lexout_o -> current.type = TOKEN_EOF;
        lexout_o -> current.value = UINT8_MAX;
}

static inline void prepareRdErrToken (lexout_t *lexout_o) {
        lexout_o -> current.type = TOKEN_ERR;
        lexout_o -> current.value = UINT8_MAX;
}

static inline void prepareUnknownToken (lexout_t *lexout_o) {
        lexout_o -> current.type = TOKEN_UNKNOWN;
        lexout_o -> current.value = UINT8_MAX;
}

static int lexRegister (lexout_t *lexout_o) {

        consume(lexout_o -> source); // '*'

        int status = bufferStatus(lexout_o -> source);
        if (status != BUF_RD_OKK) {
                return status;
        }

        switch (peek(lexout_o -> source)) {

        case 'P':
        case 'p':
                lexout_o -> current.type = TOKEN_REG_PRINT;
                lexout_o -> current.value = 'p';
                consume(lexout_o -> source);
                return 0;

        case 'L':
        case 'l':
                lexout_o -> current.type = TOKEN_REG_LOOP;
                lexout_o -> current.value = 'l';
                consume(lexout_o -> source);
                return 0;

        case 'R':
        case 'r':
                lexout_o -> current.type = TOKEN_REG_RETURN;
                lexout_o -> current.value = 'r';
                consume(lexout_o -> source);
                return 0;

        default:
                return LEX_UNKNOWN_TOKEN;

        }

}

static int lexIdentifier (lexout_t *lexout_o) {

        consume(lexout_o -> source); // 'x'

        int status = bufferStatus(lexout_o -> source);
        if (status != BUF_RD_OKK) {
                return status;
        }

        switch (peek(lexout_o -> source)) {

        case 'n':
                lexout_o -> current.type = TOKEN_NIBBLE;
                consume(lexout_o -> source);
                break;

        case 'b':
                lexout_o -> current.type = TOKEN_BYTE;
                consume(lexout_o -> source);
                break;

        case 'w':
                lexout_o -> current.type = TOKEN_WORD;
                consume(lexout_o -> source);
                break;

        case 'd':
                lexout_o -> current.type = TOKEN_DWORD;
                consume(lexout_o -> source);
                break;

        case 'q':
                lexout_o -> current.type = TOKEN_QWORD;
                consume(lexout_o -> source);
                break;

        case 'p':
                lexout_o -> current.type = TOKEN_PARAGRAPH;
                consume(lexout_o -> source);
                break;

        default:
                if (!IS_NUM[peek(lexout_o -> source)]) {
                        return LEX_UNKNOWN_TOKEN;
                }
                lexout_o -> current.type = TOKEN_BIT;

        }

        if (!IS_NUM[peek(lexout_o -> source)]) {
                return LEX_UNKNOWN_TOKEN;
        }

        int found_digit = 0;
        uint8_t value = 0;
        for (;;) {

                int status = bufferStatus(lexout_o -> source);
                if (status == BUF_RD_EOF) {
                        lexout_o -> current.value = value;
                        return found_digit ? 0 : LEX_NO_IDENTIFIER_NAME;
                }

                if (status == BUF_RD_ERR) {
                        return status;
                }

                int c = peek(lexout_o -> source);
                if (!IS_NUM[c]) {
                        lexout_o -> current.value = value;
                        return found_digit ? 0 : LEX_NO_IDENTIFIER_NAME;
                }

                if (value > UINT8_MAX / 10 || (value == UINT8_MAX / 10 && c - '0' > UINT8_MAX % 10)) {
                        lexout_o -> current.value = value;
                        return LEX_IDENTIFIER_OVERFLOW;
                }

                value = value * 10 + (c - '0');

                found_digit = 1;
                consume(lexout_o -> source);

        }

        lexout_o -> current.value = value;
        return found_digit ? 0 : LEX_NO_IDENTIFIER_NAME;

}

int lex_next (lexout_t *lexout_o) {

        for (;;) {

                int status = skipSpaces(lexout_o);

                if (status == BUF_RD_EOF) {
                        prepareEofToken(lexout_o);
                        return 0;
                }

                if (status == BUF_RD_ERR) {

                        write_stderr("LEX ERROR: Read_Error.\n Inside space skipping.");
                        lexout_o -> error_code = LEX_READ_ERROR;
                        prepareRdErrToken(lexout_o);
                        return -1;

                }

                if (peek(lexout_o -> source) != ';') {
                        break;
                }

                status = skipComment(lexout_o);

                if (status == BUF_RD_EOF) {
                        prepareEofToken(lexout_o);
                        return 0;
                }

                if (status == BUF_RD_ERR) {

                        write_stderr("LEX ERROR: Read_Error.\n Inside space skipping.");
                        lexout_o -> error_code = LEX_READ_ERROR;
                        prepareRdErrToken(lexout_o);
                        return -1;

                }
        }

        /* Safe to access once after the loop:
         * skipSpaces() maintains a valid buffer position.
         */

        switch (peek(lexout_o -> source)) {

        case '1':
                lexout_o -> current.type = TOKEN_HIGH;
                lexout_o -> current.value = '1';
                consume(lexout_o -> source);
                return 0;

        case '0':
                lexout_o -> current.type = TOKEN_LOW;
                lexout_o -> current.value = '0';
                consume(lexout_o -> source);
                return 0;

        case '|':
                lexout_o -> current.type = TOKEN_OR;
                lexout_o -> current.value = '|';
                consume(lexout_o -> source);
                return 0;

        case '&':
                lexout_o -> current.type = TOKEN_AND;
                lexout_o -> current.value = '&';
                consume(lexout_o -> source);
                return 0;

        case '^':
                lexout_o -> current.type = TOKEN_XOR;
                lexout_o -> current.value = '^';
                consume(lexout_o -> source);
                return 0;

        case '~':
                lexout_o -> current.type = TOKEN_NOT;
                lexout_o -> current.value = '~';
                consume(lexout_o -> source);
                return 0;

        case '!':
                lexout_o -> current.type = TOKEN_NOT;
                lexout_o -> current.value = '!';
                consume(lexout_o -> source);
                return 0;

        case '>':
                lexout_o -> current.type = TOKEN_FEED;
                lexout_o -> current.value = '>';
                consume(lexout_o -> source);
                return 0;

        case '<':
                lexout_o -> current.type = TOKEN_READ;
                lexout_o -> current.value = '<';
                consume(lexout_o -> source);
                return 0;

        case '.':
                lexout_o -> current.type = TOKEN_RST_LHS;
                lexout_o -> current.value = '.';
                consume(lexout_o -> source);
                return 0;

        case '*': {

                int status = lexRegister(lexout_o);

                if (status == BUF_RD_ERR) {
                        prepareRdErrToken(lexout_o);
                        lexout_o -> error_code = LEX_READ_ERROR;
                        return -1;
                }

                if (status == BUF_RD_EOF) {
                        write_stderr("LEX ERROR: Unexpected_EOF: After '*'.");
                        prepareUnknownToken(lexout_o);
                        lexout_o -> error_code = LEX_UNKNOWN_TOKEN;
                        return -1;
                }

                if (status == LEX_UNKNOWN_TOKEN) {
                        write_stderr("LEX ERROR: Encountered_an_Unknown_Register_Name: '%c'.", peek(lexout_o -> source));
                        prepareUnknownToken(lexout_o);
                        lexout_o -> error_code = LEX_UNKNOWN_TOKEN;
                        return -1;
                }

                return 0;

        }

        case 'X':
        case 'x': {

                int status = lexIdentifier(lexout_o);

                if (status == BUF_RD_ERR) {
                        prepareRdErrToken(lexout_o);
                        lexout_o -> error_code = LEX_READ_ERROR;
                        return -1;
                }

                if (status == BUF_RD_EOF) {
                        write_stderr("LEX ERROR: Unexpected_EOF: After 'x'.");
                        prepareUnknownToken(lexout_o);
                        lexout_o -> error_code = LEX_UNKNOWN_TOKEN;
                        return -1;
                }

                if (status == LEX_IDENTIFIER_OVERFLOW) {
                        write_stderr("LEX ERROR: Identifier_Overflow: '%d...',\nGreater than bucket <INT(8): (%d)>", lexout_o -> current.value, UINT8_MAX);
                        prepareUnknownToken(lexout_o);
                        lexout_o -> error_code = LEX_IDENTIFIER_OVERFLOW;
                        return -1;
                }

                if (status == LEX_NO_IDENTIFIER_NAME) {
                        write_stderr("LEX ERROR: Identifier_Has_No_Name.");
                        prepareUnknownToken(lexout_o);
                        lexout_o -> error_code = LEX_NO_IDENTIFIER_NAME;
                        return -1;
                }

                if (status == LEX_UNKNOWN_TOKEN) {
                        write_stderr("LEX ERROR: Expected a number but got '%c'.", peek(lexout_o -> source));
                        lexout_o -> error_code = LEX_UNKNOWN_TOKEN;
                        return -1;
                }

                return 0;

        }

        default:
                write_stderr("LEX ERROR: Unknown_Token_Detected: '%c',\nbarely know her", peek(lexout_o -> source));
                prepareUnknownToken(lexout_o);
                lexout_o -> error_code = LEX_UNKNOWN_TOKEN;
                return -1;

        }

}
