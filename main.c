#include <stdio.h>
#include <stdint.h>

#define REG_EMPTY UINT8_MAX

typedef struct {

        uint8_t lhs;
        uint8_t rhs;
        uint8_t op;
        uint8_t acc;
        uint8_t mem;

} registers_t;

void reset_reg (registers_t *reg, int empty_mem) {

        reg -> lhs = REG_EMPTY;
        reg -> rhs = REG_EMPTY;
        reg -> op = REG_EMPTY;

        if (empty_mem) reg -> mem = REG_EMPTY;

}

int mov_operand (registers_t reg, int value) {

        if (reg.lhs == REG_EMPTY) {
                reg.lhs = value;
        } else {
                reg.rhs = value;
        }

        return 0;

}

int mov_values (registers_t reg, int c) {

        switch (c) {

        case '1':
                mov_operand(reg, 1);
                break;

        case '0':
                mov_operand(reg, 0);
                break;

        case '|':
                reg.op = 1;
                break;

        case '&':
                reg.op = 2;
                break;
        
        case '^':
                reg.op = 3;
                break;
        
        case '!':
                reg.op = 4;
                break;
 
        default:
                return 101;

        }

        return 0;

}

int evaluate (registers_t *reg) {

        if (reg -> op == REG_EMPTY) {
                return 0;
        }

        printf("%d\n", reg -> op);

        switch (reg -> op) {

        case 1:
                reg -> acc = reg -> lhs || reg -> rhs;
                reset_reg(reg, 0);
                break;
        
        case 2:
                reg -> acc = reg -> lhs && reg -> rhs;
                reset_reg(reg, 0);
                break;
        
        case 3:
                reg -> acc = reg -> lhs != reg -> rhs;
                reset_reg(reg, 0);
                break;
        
        case 4:
                reg -> acc = !reg -> lhs;
                reset_reg(reg, 0);
                break;
        
        default:
                return 102;

        }

        return 0;

}

int resolve_errorcodes (int code, int c) {

        switch (code) {

        case 101:
                fprintf(stderr, "BYTE ERROR: '%c' is not recognized.\nAbort.\n", c);
                return 0;
                break;
        
        case 102:
                fprintf(stderr, "EVALUATE ERROR: encountered an unknown operator.\nAbort.\n");
                return 1;
                break;
 
        }

        return 0;

}

int main (void) {

        registers_t reg;

        reset_reg(&reg, 1);
        reg.acc = REG_EMPTY;

        FILE *fp = fopen("test", "rb");
        if (fp == NULL) {
                return 1;
        }

        int c = -1;
        while ((c = fgetc(fp)) != EOF) {

                if (
                        c == ' '||
                        c == '\t'||
                        c == '\n'||
                        c == '\r'
                ) {
                        continue;
                }

                int result = mov_values(reg, c);
                if (resolve_errorcodes(result, c)) {
                        return result;
                }

                result = evaluate(&reg);
                if (resolve_errorcodes(result, c)) {
                        return result;
                }

                printf("last op: %d\n", reg.op);

        }

        printf("acc: %d\n", reg.acc);

        return 0;

}
