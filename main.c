#include <stdio.h>

#include "bitter/reader.h"
#include "bitter/lex.h"
#include "bitter/processor.h"

int main (void) {

        //source_buf_t *sbuf = source_buffer_constructor(4096, "test");
        //registers_t *reg = registers_constructor();
        //parser_states_t pstate = P_IN_PROC;

        lex_t *lex = lex_constructor();

        for (int i = 0; i < 3; ++i) {

                token_t tok = lex_next(lex);
                printf("%d: %s\n", tok.type, tok.value);

        //        if (sbuf -> position == sbuf -> length) {
        //                if (read_to_buffer(sbuf) != 0) {
        //                        break;
        //                }
        //        }

        //        if (sbuf -> length == 0) {
        //                break;
        //        }

        //        pstate = parse(sbuf, reg);

        //        if (pstate == P_UNKNOWN) {
        //                printf("unknown fault\n");
        //                break;
        //        }

        //        if (pstate == P_WAIT_EVAL) {
        //                if (evaluate(reg) != 0) {
        //                        printf("eval fault\n");
        //                        break;
        //                }
        //        }
        //        printf("ps: %d\n", pstate);

        }

        //pstate = parse(sbuf, reg);
        //if (pstate == P_WAIT_EVAL) {
        //        evaluate(reg);
        //}

        //printf("acc: %d\n", inspect_reg(reg, R_ACC));
        //printf("lhs: %d\n", inspect_reg(reg, R_LHS));

        //source_buffer_destructor(&sbuf);
        //registers_destructor(&reg);
        lex_destructor(&lex);
        return 0;

}