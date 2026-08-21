#pragma once

/* Parser magic numbers */
#define PAR_UNKNOWN_OPERAND 9674116 //operanla
#define PAR_SYNTAX_ERROR    6711629 //ariylato
#define PAR_FETCH_EMPTY_VAR 7321176 //empteyaa
#define PAR_OP_ON_EMPTY_REG 2772117 //regempte

typedef struct parse parse_t;

parse_t *parse_constructor (void);
void parse_destructor (parse_t **parse_o);

int parse_start (parse_t *parse_o);

void dump_registers (parse_t *parse_o, const char *opts);
void dump_symbol_array (parse_t *parse_o);
