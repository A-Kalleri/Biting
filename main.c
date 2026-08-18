#include <stdio.h>

#include "bitter/parse.h"

int main (void) {

        parse_t *parse = parse_constructor();

        int status = parse_start(parse);
        printf("\n");
        dump_registers(parse, "l");
        dump_symbol_array(parse);

        parse_destructor(&parse);

        return status;

}