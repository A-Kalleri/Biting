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

                if (
                        lexout -> current.type == TOKEN_BIT ||
                        lexout -> current.type == TOKEN_NIBBLE ||
                        lexout -> current.type == TOKEN_BYTE ||
                        lexout -> current.type == TOKEN_WORD ||
                        lexout -> current.type == TOKEN_DWORD ||
                        lexout -> current.type == TOKEN_QWORD ||
                        lexout -> current.type == TOKEN_PARAGRAPH ||
                        lexout -> current.type == TOKEN_ASCII
                ) {
                        write_stdout("lexout -> %d, type -> %d\n", lexout -> current.value, lexout -> current.type);
                } else {
                        write_stdout("lexout -> %c, type -> %d\n", lexout -> current.value, lexout -> current.type);
                }

        }

        lexout_destructor(&lexout);
        return 0;

}