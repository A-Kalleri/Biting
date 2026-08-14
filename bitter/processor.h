#pragma once

#include "reader.h"

typedef enum {

    R_OP    = 0,
    R_LHS   = 2,
    R_RHS   = 4,
    R_ACC   = 6,

} reg_types_t;

#define REG_EMPTY UINT8_MAX

typedef enum {

    P_IN_PROC = 1001,
    P_UNKNOWN,
    P_SYNTAX_ERR,
    P_WAIT_EVAL,

} parser_states_t;

typedef struct registers registers_t;

registers_t *registers_constructor (void);
void registers_destructor (registers_t **reg_o);
int evaluate (registers_t *reg_o);
parser_states_t parse (source_buf_t *sbuf_o, registers_t *reg_o);
int inspect_reg (registers_t *reg_o, reg_types_t reg_type);