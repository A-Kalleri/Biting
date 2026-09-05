#include <stdint.h>
#include <stdlib.h>

#include "lex.h"
#include "parse.h"
#include "helper.h"
#include "bem.h"

typedef struct parser {

        lex_t *lex;
        token_t current;
        bem_t *bem;
        int error_code;

} parser_t;

parser_t *parser_constructor (void) {

        parser_t *_parser_image = calloc(1, sizeof(*_parser_image));
        if (!_parser_image) {
                return NULL;
        }

        return _parser_image;

}

void parser_destructor (parser_t **parser_o) {

        if (!parser_o || !*parser_o) {
                return;
        }

        free(*parser_o);
        parser_o = NULL;

}

static void advanceParser (parser_t *parser_o) {
        parser_o -> current = lex_next(parser_o -> lex);
}

static int parseStatement (parser_t *parser_o) {

        switch (parser_o -> current.type) {

        case TOKEN_NOT:
                advanceParser(parser_o);
                if (parser_o -> bem -> flags -> acc_empty) {

                        write_stderr_info(
                                parser_o -> current.line, parser_o -> current.column,
                                "PARSE ERROR: Syntax_Error.\nExpected Accumulator to be not EMPTY."
                        );
                        parser_o -> error_code = PAR_SYNTAX_ERROR;
                        return 1;

                }
                bem_execute(parser_o -> bem, NOT, EMPTY_CELL, EMPTY_CELL);
                return 0;

        }

        return 1;

}

static int parseProgram (parser_t *parser_o) {

        advanceParser(parser_o);

        while (parser_o -> current.type != TOKEN_EOF) {

                if (parser_o -> current.type == TOKEN_RD_ERR) {
                        return parser_o -> lex -> error_code;
                }

                if (parser_o -> current.type == TOKEN_UNKNOWN) {
                        return parser_o -> lex -> error_code;
                }

                if (parseStatement(parser_o) != 0) {
                        return parser_o -> error_code;
                }

        }

        return 0;

}