#include <stdlib.h>
#include <stdio.h>

#include "lex.h"
#include "parse.h"

#define REG_EMPTY UINT8_MAX

typedef enum {

        NONE,
        OR,
        AND,
        XOR,
        NOT,

} op_t;

typedef struct registers {

        uint8_t lhs;
        uint8_t rhs;
        uint8_t op;
        uint8_t acc;

} registers_t;

typedef struct parse {

        lex_t *lex;
        registers_t *reg_o;
        token_t current;

} parse_t;

static inline void reset_registers (registers_t *reg_o) {

        reg_o -> lhs = REG_EMPTY;
        reg_o -> rhs = REG_EMPTY;
        reg_o -> op = NONE;

}

static registers_t *registers_constructor (void) {

        registers_t *_registers_image = malloc(sizeof(*_registers_image));
        if (!_registers_image) {
                return NULL;
        }

        _registers_image -> acc = REG_EMPTY;
        reset_registers(_registers_image);

        return _registers_image;

}

static void registers_destructor (registers_t **reg_o) {

        if (!reg_o || !*reg_o) {
                return;
        }

        free(*reg_o);
        *reg_o = NULL;

}

void parse_destructor (parse_t **parse_o) {

        if (!parse_o || !*parse_o) {
                return;
        }

        lex_destructor(&(*parse_o) -> lex);
        registers_destructor(&(*parse_o) -> reg_o);
        free(*parse_o);

        *parse_o = NULL;

}

parse_t *parse_constructor (void) {

        parse_t *_parse_image = calloc(1, sizeof(*_parse_image));
        if (!_parse_image) {
                return NULL;
        }

        _parse_image -> reg_o = registers_constructor();
        if (!_parse_image -> reg_o) {
                parse_destructor(&_parse_image);
                return NULL;
        }
        _parse_image -> lex = lex_constructor();
        if (!_parse_image -> lex) {
                parse_destructor(&_parse_image);
                return NULL;
        }

        return _parse_image;

}

int parse_start (parse_t *parse_o) {

        token_t tok;

        for (;;) {

                tok = lex_next(parse_o -> lex);

                if (tok.type == TOKEN_RD_ERR) {
                        return 1;
                }

                if (tok.type == TOKEN_EOF) {
                        break;
                }

                //if (tok.type == TOKEN_IDENTIFIER) {
                //        printf("%s: %d\n", tok_to_s(tok.type), tok.value);
                //} else {
                //        printf("%s: %c\n", tok_to_s(tok.type), tok.value);
                //}

        }

        return 0;

}
