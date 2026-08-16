#include <stdio.h>

#include "bitter/parse.h"

int main (void) {

        parse_t *parse = parse_constructor();

        parse_start(parse);

        parse_destructor(&parse);

        return 0;

}