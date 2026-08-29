#include "lex.h"
#include "helper.h"

int main (void) {

        lexout_t *lexout = lexout_constructor();
        lexout_init(lexout, "test");

        for (;;) {

                if (lex_next(lexout) != 0) {
                        write_stderr("lex error: %d", lexout -> error_code);
                        return lexout -> error_code;
                }

                if (lexout -> current.type == TOKEN_EOF) {
                        break;
                }

                write_stderr("lexout -> %c", lexout -> current.value);

        }

        lexout_destructor(&lexout);
        return 0;

}