#pragma once

/* Parser magic numbers */
#define PAR_UNKNOWN_OPERAND 9674116 //operanla
#define PAR_SYNTAX_ERROR    6711629 //ariylato

typedef struct parse parse_t;

parse_t *parse_constructor (void);
void parse_destructor (parse_t **parse_o);

int parse_start (parse_t *parse_o);

void dump_registers (parse_t *parse_o, const char *opts);
void dump_symbol_array (parse_t *parse_o);
