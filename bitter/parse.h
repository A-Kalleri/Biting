#pragma once

typedef struct parse parse_t;

/* Parser magic numbers */
#define PAR_INIT_FAILED     9436116 //onumaela
#define PAR_UNKNOWN_OPERAND 9674116 //operanla
#define PAR_SYNTAX_ERROR    6711629 //ariylato
#define PAR_FETCH_EMPTY_VAR 7321176 //empteyaa
#define PAR_OP_ON_EMPTY_REG 2772117 //regempte

parse_t *parse_constructor (const char *filename);
void parse_destructor (parse_t **parse_o);

int parse_start (parse_t *parse_o);
