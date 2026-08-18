#include <stdlib.h>
#include <stdio.h>

#include "lex.h"
#include "parse.h"
#include "printer.h"

#define REG_EMPTY UINT8_MAX

typedef struct registers {

        uint8_t lhs;
        uint8_t rhs;
        uint8_t op;

} registers_t;

typedef struct parse {

        lex_t *lex;
        registers_t *reg_o;
        token_t current;
        int error_code;

} parse_t;

static inline void reset_registers (registers_t *reg_o) {

        reg_o -> lhs = REG_EMPTY;
        reg_o -> rhs = REG_EMPTY;
        reg_o -> op = REG_EMPTY;

}

static registers_t *registers_constructor (void) {

        registers_t *_registers_image = malloc(sizeof(*_registers_image));
        if (!_registers_image) {
                return NULL;
        }

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

void dump_registers (parse_t *parse_o) {
        write_stderr("\nLHS: %d\n", parse_o -> reg_o -> lhs);
        write_stderr("RHS: %d\n", parse_o -> reg_o -> rhs);
        write_stderr("OP: %d\n", parse_o -> reg_o -> op);
}

void advance_parse (parse_t *parse_o) {
        parse_o -> current = lex_next(parse_o -> lex);
}

static int evaluate(parse_t *parse_o) {

        registers_t *reg_o = parse_o -> reg_o;

        if (reg_o -> op == '!') {

                if (reg_o -> lhs != REG_EMPTY) {
                        write_stderr("PARSE ERROR: Syntax_Error.\nExpected LEFT_HAND_SIDE to be not EMPTY.\nABORT.");
                        parse_o -> error_code = PAR_SYNTAX_ERROR;
                        return 1;
                }

                reg_o -> lhs = !reg_o -> lhs;
                reg_o -> op = REG_EMPTY;

        }

        if (
                reg_o -> lhs == REG_EMPTY ||
                reg_o -> rhs == REG_EMPTY
        ) {
                write_stderr("\
                        PARSE ERROR: Syntax_Error.\n\
                        Expected LEFT_HAND_SIDE to be not EMPTY.\n\
                        Expected RIGHT_HAND_SIDE to be not EMPTY.\n\
                        ABORT."
                );
                parse_o -> error_code = PAR_SYNTAX_ERROR;
                return 1;
        }

        switch (reg_o -> op) {
        
        case '|':
                reg_o -> lhs = reg_o -> lhs | reg_o -> rhs;
                break;
        
        case '&':
                reg_o -> lhs = reg_o -> lhs & reg_o -> rhs;
                break;
        
        case '^':
                reg_o -> lhs = reg_o -> lhs ^ reg_o -> rhs;
                break;

        default:
                write_stderr("PARSE ERROR: Syntax_Error.\nUnknown_Operator: %c\nABORT.", reg_o -> op);
                parse_o -> error_code = PAR_SYNTAX_ERROR;
                return 1;

        }

        reg_o -> rhs = REG_EMPTY;
        reg_o -> op = REG_EMPTY;

        return 0;

}

static int parse_operand(parse_t *parse_o) {

        switch (parse_o -> current.type) {
                case TOKEN_HIGH:
                        printf("ONE ");
                        return 1;

                case TOKEN_LOW:
                        printf("ZERO ");
                        return 0;

                case TOKEN_IDENTIFIER: // token holds the value
                        return parse_o -> current.value;

                default:
                        write_stderr("PARSE ERROR: Unknown_Operand: '%c'.\nABORT.", parse_o -> current.value);
                        parse_o -> error_code = PAR_UNKNOWN_OPERAND;
                        return REG_EMPTY;

        }

}

static int parse_expression(parse_t *parse_o) {

        if (parse_o -> reg_o -> lhs == REG_EMPTY) {

                parse_o -> reg_o -> lhs = parse_operand(parse_o);
                if (parse_o -> reg_o -> lhs == REG_EMPTY) {
                        return 1;
                }
                advance_parse(parse_o);

        }

        for (;;) {

                switch (parse_o -> current.type) {

                case TOKEN_OR:
                        printf("OR ");
                        parse_o -> reg_o -> op = '|';
                        break;

                case TOKEN_AND:
                        printf("AND ");
                        parse_o -> reg_o -> op = '&';
                        break;

                case TOKEN_XOR:
                        printf("XOR ");
                        parse_o -> reg_o -> op = '^';
                        break;
                
                case TOKEN_NOT:
                        printf("NOT ");
                        parse_o -> reg_o -> op = '!';
                        break;

                default:
                        return 0;

                }

                advance_parse(parse_o);
                parse_o -> reg_o -> rhs = parse_operand(parse_o);
                if (parse_o -> reg_o -> rhs == REG_EMPTY) {
                        return 1;
                }
                advance_parse(parse_o);

                if (evaluate(parse_o) != 0) {
                        return 1;
                }

        }

}

static int parse_statement(parse_t *parse_o) {

        if (parse_expression(parse_o) != 0) {
                return 1;
        }

        switch (parse_o -> current.type) {

        case TOKEN_RST_LHS:
                advance_parse(parse_o);
                parse_o -> reg_o -> lhs = REG_EMPTY;
                break;

        case TOKEN_FEED:
                advance_parse(parse_o);
                if (parse_o -> current.type == TOKEN_IDENTIFIER) {
                        //write expression to identifier
                } else if (parse_o -> current.type == TOKEN_IDENTIFIER) {//register statndin
                        //write to register
                }
                break; // to statement

        }

        return 0;

}

static int parse_program (parse_t *parse_o) {

        advance_parse(parse_o);

        while (parse_o -> current.type != TOKEN_EOF) {

                if (parse_o -> current.type == TOKEN_RD_ERR) {
                        return parse_o -> lex -> error_code;
                }

                if (parse_o -> current.type == TOKEN_UNKNOWN) {
                        return parse_o -> lex -> error_code;
                }

                if (parse_statement(parse_o) != 0) {
                        return parse_o -> error_code;
                }

        }

        return 0;

}

int parse_start (parse_t *parse_o) {
        return parse_program(parse_o);
}