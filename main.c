#include <stdio.h>

#include "bitter/parse.h"

int main (void) {

        parse_t *parse = parse_constructor();

        int status = parse_start(parse);

        parse_destructor(&parse);

        return status;

}