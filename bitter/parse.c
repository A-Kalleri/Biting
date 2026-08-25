#include <stdlib.h>

#include "lex.h"
#include "parse.h"
#include "symbol_array.h"
#include "helper.h"

#define REG_EMPTY UINT8_MAX

typedef struct registers {

        uint8_t lhs;
        uint8_t rhs;
        uint8_t op;
        uint8_t rp;

} registers_t;

typedef struct parse {

        lex_t *lex;
        registers_t *reg_o;
        uint8_t *symbol_array;
        token_t current;
        int error_code;

} parse_t;

static inline void resetRegisters (registers_t *reg_o) {

        reg_o -> lhs = REG_EMPTY;
        reg_o -> rhs = REG_EMPTY;
        reg_o -> op = REG_EMPTY;
        reg_o -> rp = REG_EMPTY;

}

static registers_t *registersConstructor (void) {

        registers_t *_registers_image = malloc(sizeof(*_registers_image));
        if (!_registers_image) {
                return NULL;
        }

        resetRegisters(_registers_image);

        return _registers_image;

}

static void registersDestructor (registers_t **reg_o) {

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
        registersDestructor(&(*parse_o) -> reg_o);
        array_free(&(*parse_o) -> symbol_array);
        free(*parse_o);

        *parse_o = NULL;

}

parse_t *parse_constructor (const char *filename) {

        parse_t *_parse_image = calloc(1, sizeof(*_parse_image));
        if (!_parse_image) {
                return NULL;
        }

        _parse_image -> symbol_array = array_alloc((size_t)(UINT8_MAX + 1));
        if (!_parse_image -> symbol_array) {
                parse_destructor(&_parse_image);
                return NULL;
        }

        _parse_image -> reg_o = registersConstructor();
        if (!_parse_image -> reg_o) {
                parse_destructor(&_parse_image);
                return NULL;
        }
        _parse_image -> lex = lex_constructor(filename);
        if (!_parse_image -> lex) {
                parse_destructor(&_parse_image);
                return NULL;
        }

        return _parse_image;

}

static int fetchValue (parse_t *parse_o) {

        uint8_t value = parse_o -> symbol_array[parse_o -> current.value];

        if (value == EMPTY_CELL) {
                write_stderr("PARSE ERROR: Value_Error.\nValue at x%d is EMPTY.\nABORT.", parse_o -> current.value);
                parse_o -> error_code = PAR_FETCH_EMPTY_VAR;
                return 1;
        }

        parse_o -> current.value = value;
        return 0;

}

static void advanceParse (parse_t *parse_o) {
        parse_o -> current = lex_next(parse_o -> lex);
}

static int evaluate(parse_t *parse_o) {

        registers_t *reg_o = parse_o -> reg_o;

        if (
                reg_o -> lhs == REG_EMPTY ||
                reg_o -> rhs == REG_EMPTY
        ) {
                write_stderr("PARSE ERROR: Syntax_Error.\nExpected LEFT_HAND_SIDE to be not EMPTY.\nExpected RIGHT_HAND_SIDE to be not EMPTY.\nABORT."
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

static int parseOperand(parse_t *parse_o) {

        switch (parse_o -> current.type) {

        case TOKEN_HIGH:
                return 1;

        case TOKEN_LOW:
                return 0;

        case TOKEN_IDENTIFIER: // token holds the value
                if (fetchValue(parse_o) != 0) {
                        return REG_EMPTY;
                }
                return parse_o -> current.value;

        default:
                write_stderr("PARSE ERROR: Unknown_Operand: '%c'.\nABORT.", parse_o -> current.value);
                parse_o -> error_code = PAR_UNKNOWN_OPERAND;
                return REG_EMPTY;

        }

}

static int parseExpression(parse_t *parse_o) {

        if (parse_o -> reg_o -> lhs == REG_EMPTY) {

                parse_o -> reg_o -> lhs = parseOperand(parse_o);
                if (parse_o -> reg_o -> lhs == REG_EMPTY) {
                        return 1;
                }
                advanceParse(parse_o);

        }

        for (;;) {

                switch (parse_o -> current.type) {

                case TOKEN_OR:
                        parse_o -> reg_o -> op = '|';
                        break;

                case TOKEN_AND:
                        parse_o -> reg_o -> op = '&';
                        break;

                case TOKEN_XOR:
                        parse_o -> reg_o -> op = '^';
                        break;

                default:
                        return 0;

                }

                advanceParse(parse_o);
                parse_o -> reg_o -> rhs = parseOperand(parse_o);
                if (parse_o -> reg_o -> rhs == REG_EMPTY) {
                        return 1;
                }
                advanceParse(parse_o);

                if (evaluate(parse_o) != 0) {
                        return 1;
                }

        }

}

static int parseStatement(parse_t *parse_o) {

        switch (parse_o -> current.type) {

        case TOKEN_NOT:
                advanceParse(parse_o);
                if (parse_o -> reg_o -> lhs == REG_EMPTY) {

                        write_stderr("PARSE ERROR: Syntax_Error.\nExpected LEFT_HAND_SIDE to be not EMPTY.\nABORT.");
                        parse_o -> error_code = PAR_SYNTAX_ERROR;
                        return 1;

                }
                parse_o -> reg_o -> lhs = !parse_o -> reg_o -> lhs;
                return 0;

        case TOKEN_RST_LHS:
                advanceParse(parse_o);
                parse_o -> reg_o -> lhs = REG_EMPTY;
                return 0;

        case TOKEN_READ:
                advanceParse(parse_o);
                if (parse_o -> reg_o -> rp != REG_EMPTY){
                        write_stdout_char(parse_o -> reg_o -> rp);
                }
                else if (parse_o -> reg_o -> lhs != REG_EMPTY) {
                        write_stdout_char(parse_o -> reg_o -> lhs);
                } else {

                        write_stderr("PARSE ERROR: Read_on_Empty_Registers\nRP and LHS are both empty.\nABORT.");
                        parse_o -> error_code = PAR_OP_ON_EMPTY_REG;
                        return 1;

                }
                return 0;

        default:
                break;

        }

        if (parseExpression(parse_o) != 0) {
                return 1;
        }

        switch (parse_o -> current.type) {

        case TOKEN_FEED:
                advanceParse(parse_o);
                if (parse_o -> current.type == TOKEN_IDENTIFIER) {
                        parse_o -> symbol_array[parse_o -> current.value] = parse_o -> reg_o -> lhs;
                        advanceParse(parse_o);
                } else if (parse_o -> current.type == TOKEN_REG_PRNT) {
                        parse_o -> reg_o -> rp = parse_o -> reg_o -> lhs;
                        advanceParse(parse_o);
                } else {
                        write_stderr("PARSE ERROR: Syntax_Error:\nExpected an identifier of register.\nABORT.");
                        parse_o -> error_code = PAR_SYNTAX_ERROR;
                        return 1;
                }

                return 0;

        case TOKEN_HIGH:
        case TOKEN_LOW:
        case TOKEN_IDENTIFIER:
        case TOKEN_REG_PRNT:
                write_stderr("PARSE ERROR: Syntax_Error: Unexpected_Operand: ");
                if (parse_o -> current.type == TOKEN_IDENTIFIER) {
                        write_stderr("'x%d'.", parse_o -> current.value);
                } else {
                        write_stderr("'%c'.", parse_o -> current.value);
                }
                write_stderr("\nExpected an operator after an operand.\nABORT.");
                parse_o -> error_code = PAR_SYNTAX_ERROR;
                return 1;

        default:
                break;

        }

        return 0;

}

static int parseProgram (parse_t *parse_o) {

        advanceParse(parse_o);

        while (parse_o -> current.type != TOKEN_EOF) {

                if (parse_o -> current.type == TOKEN_RD_ERR) {
                        return parse_o -> lex -> error_code;
                }

                if (parse_o -> current.type == TOKEN_UNKNOWN) {
                        return parse_o -> lex -> error_code;
                }

                if (parseStatement(parse_o) != 0) {
                        return parse_o -> error_code;
                }

        }

        return 0;

}

int parse_start (parse_t *parse_o) {
        return parseProgram(parse_o);
}