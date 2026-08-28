#include <stdint.h>
#include <stdlib.h>

#include "lex.h"
#include "reader.h"

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
                return 1;
        }

        return 0;

}

static int peek(source_buf_t *sbuf) {

        return sbuf -> buffer[sbuf -> position];

}
