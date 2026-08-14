#include <stdlib.h>

#include "reader.h"
#include "lex.h"

static const uint8_t IS_SPACE[256] = {

        [' ']   = 1,
        ['\n']  = 1,
        ['\r']  = 1,
        ['\t']  = 1

};

lex_t *lex_constructor () {

        lex_t *_lex_image = calloc(1, sizeof(*_lex_image));
        if (!_lex_image) {
                return NULL;
        }

        _lex_image -> source = source_buffer_constructor(4096, "test");
        if (!_lex_image -> source) {
                free(_lex_image);
                return NULL;
        }

        return _lex_image;

}

void *lex_destructor (lex_t **lex_o) {

        if (!lex_o || !*lex_o) {
                return 0;
        }

        source_buffer_destructor(&(*lex_o) -> source);
        free(*lex_o);
        *lex_o = NULL;

}

static char get_char (source_buf_t *sbuf) {
        return sbuf -> buffer[sbuf -> position];
}

token_t lex_next (lex_t *lex_o) {

        token_t tok; //important to be allocated

        while (IS_SPACE[get_char(lex_o -> source)]) {
                consume(lex_o -> source);
        }

        if (get_char(lex_o -> source) == '1') {
                tok.type = TOKEN_HIGH;
                tok.value = "1";
                tok.length = 1;
                consume(lex_o -> source);
                return tok;
        } else {
                tok.type = TOKEN_UNKNOWN;
                tok.value = "o";
                tok.length = 1;
                consume(lex_o -> source);
                return tok;
        }

}
