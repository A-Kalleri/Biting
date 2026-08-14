#include <stdint.h>
#include <stdlib.h>

#include "processor.h"
#include "reader.h"

static const uint8_t IS_SPACE[256] = {

    [' ']   = 1,
    ['\n']  = 1,
    ['\t']  = 1,
    ['\r']  = 1

};

typedef enum: uint8_t {

    NONE    = 0,
    OR      = 1,
    AND     = 2,
    XOR     = 3,
    NOT     = 4,

} op_t;

static const uint8_t TO_BCODE[256] = {

    ['|'] = OR,
    ['&'] = AND,
    ['^'] = XOR,

};

static int do_or  (int a, int b) { return a | b; }
static int do_and (int a, int b) { return a & b; }
static int do_xor (int a, int b) { return a ^ b; }

typedef int (*do_op_t)(int lhs, int rhs);

static const do_op_t DO_OP[256] = {

    [OR]    = do_or,
    [AND]   = do_and,
    [XOR]   = do_xor

};

typedef struct registers {

    op_t op;
    uint8_t lhs;
    uint8_t rhs;
    uint8_t acc;

} registers_t;

static inline char get_char (source_buf_t *sbuf_o) {
    return sbuf_o -> buffer[sbuf_o -> position];
}

static int mov_operand (registers_t *reg_o, char c) {

    int value = REG_EMPTY;
    if (c == '1') {
        value = 1;
    } else if (c == '0') {
        value = 0;
    } else {
        return 1;
    }

    if (reg_o -> lhs == REG_EMPTY) {
        reg_o -> lhs = value;
    } else {
        reg_o -> rhs = value;
    }

    return 0;

}

static inline void reset_registers (registers_t *reg_o) {

    reg_o -> lhs = REG_EMPTY;
    reg_o -> rhs = REG_EMPTY;
    reg_o -> op  = NONE;

}

registers_t *registers_constructor (void) {

    registers_t *_registers_image = malloc(sizeof(*_registers_image));
    if (!_registers_image) {
        return NULL;
    }

    _registers_image -> acc = REG_EMPTY;
    reset_registers(_registers_image);

    return _registers_image;

}

void registers_destructor (registers_t **reg_o) {

    free(*reg_o);
    *reg_o = NULL;

}

int evaluate (registers_t *reg_o) {

    if (reg_o == NULL) {
        return 1;
    }

    if (reg_o -> op == NONE) {
        return 0;
    }

    if (reg_o -> rhs == REG_EMPTY) {
        if (reg_o -> lhs == REG_EMPTY) {
            printf("lhs empty\n");
            return 1;
        }
        if (reg_o -> op != NOT) {
            printf("not not\n");
            return 1;
        }

        reg_o -> acc = !reg_o -> lhs;
        reset_registers(reg_o);
        return 0;

    }

    if (reg_o -> lhs == REG_EMPTY) {
        printf("lhs empty\n");
        return 1;
    }

    if (DO_OP[reg_o -> op] != NULL) {
        reg_o -> acc = DO_OP[reg_o -> op](reg_o -> lhs, reg_o -> rhs);
        reset_registers(reg_o);
    }

    return 0;

}

int inspect_reg (registers_t *reg_o, reg_types_t reg_type) {

    if (reg_type == R_LHS) {
        return reg_o -> lhs;
    } else if (reg_type == R_RHS) {
        return reg_o -> rhs;
    } else if (reg_type == R_ACC) {
        return reg_o -> acc;
    } else if (reg_type == R_OP) {
        return reg_o -> op;
    } else {
        return -1;
    }

}

static int ready_to_eval (const registers_t *reg_o) {

    if (reg_o -> op == NONE) {
        return 0;
    }

    if (reg_o -> rhs == REG_EMPTY) {
        return reg_o -> op == NOT && reg_o -> lhs != REG_EMPTY;
    }

    return reg_o -> lhs != REG_EMPTY;

}

parser_states_t parse (source_buf_t *sbuf_o, registers_t *reg_o) {

    while (IS_SPACE[get_char(sbuf_o)]) {
        consume(sbuf_o);
    }

    if (ready_to_eval(reg_o)) {
        return P_WAIT_EVAL;
    }

    char c = get_char(sbuf_o);

    if (mov_operand(reg_o, c) == 0) {
        consume(sbuf_o);
        return P_IN_PROC;
    }

    if (c == '~' || c == '!') {

        consume(sbuf_o);
        if (mov_operand(reg_o, c) == 0) {
            consume(sbuf_o);
            return P_WAIT_EVAL;
        }
        return P_SYNTAX_ERR;

    }

    if (TO_BCODE[c] != NONE) {
        reg_o -> op = TO_BCODE[c];
        consume(sbuf_o);
        return P_IN_PROC;
    }

    return P_UNKNOWN;

}