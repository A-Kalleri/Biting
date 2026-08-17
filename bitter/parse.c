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

void advance_parse (parse_t *parse_o) {
        parse_o -> current = lex_next(parse_o -> lex);
}

static int parse_operand(parse_t *parse_o) {
        
        switch (parse_o -> current.type) {
                case TOKEN_HIGH:
                        return 1;
                case TOKEN_LOW:
                        return 0;
                case TOKEN_IDENTIFIER: // token holds the value
                        return parse_o -> current.value;
                
                case TOKEN_NOT:
                        return !parse_o -> reg_o -> lhs;

        }

}

static int parse_operator(parse_t *parse_o) {

        parse_o -> reg_o -> lhs = parse_operand(parse_o);
        printf("lhs: stages: %d\n", parse_o -> reg_o -> lhs);
        advance_parse(parse_o);

        switch (parse_o -> current.type) {

        case TOKEN_OR:
                advance_parse(parse_o);
                parse_o -> reg_o -> lhs = parse_o -> reg_o -> lhs | parse_operand(parse_o);
                advance_parse(parse_o);
                break;

        case TOKEN_AND:
                advance_parse(parse_o);
                parse_o -> reg_o -> lhs = parse_o -> reg_o -> lhs & parse_operand(parse_o);
                advance_parse(parse_o);
                break;

        case TOKEN_XOR:
                advance_parse(parse_o);
                parse_o -> reg_o -> lhs = parse_o -> reg_o -> lhs ^ parse_operand(parse_o);
                advance_parse(parse_o);
                break;
        
        case TOKEN_FEED:
                advance_parse(parse_o);
                if (parse_o -> current.type == TOKEN_IDENTIFIER) {
                        //write expression to identifier
                } else if (parse_o -> current.type == TOKEN_IDENTIFIER) {//register statndin
                        //write to register
                }

                advance_parse(parse_o);
                break;

        }

}

static int parse_expression(parse_t *parse_o) {

        parse_operator(parse_o);

        return 0;

}

static int parse_statement(parse_t *parse_o) {

        parse_expression(parse_o);

        return 0;

}

static int parse_program (parse_t *parse_o) {

        advance_parse(parse_o); // initial call, loading first token.

        while (parse_o -> current.type != TOKEN_EOF) {

                if (parse_o -> current.type == TOKEN_RD_ERR) {
                        return 1;
                }

                if (parse_statement(parse_o) != 0) {
                        return 1;
                }

        }

        return 0;

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

                parse_program(parse_o);

                //if (tok.type == TOKEN_IDENTIFIER) {
                //        printf("%s: %d\n", tok_to_s(tok.type), tok.value);
                //} else {
                //        printf("%s: %c\n", tok_to_s(tok.type), tok.value);
                //}

        }

        printf("lhs: %d\n", parse_o -> reg_o -> lhs);

        return 0;

}
