#pragma once

typedef struct parse parse_t;

parse_t *parse_constructor (void);
void parse_destructor (parse_t **parse_o);

int parse_start (parse_t *parse_o);

