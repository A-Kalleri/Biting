#include <stdio.h>
#include <stdint.h>

#define REG_EMPTY UINT8_MAX

typedef enum: uint8_t {
        NONE    = REG_EMPTY,
        OR      = 1,
        AND     = 2,
        XOR     = 3,
        NOT     = 4,
} op_t;

int do_or (int a, int b) { return a | b; }
int do_and (int a, int b) { return a & b; }
int do_xor (int a, int b) { return a ^ b; }
int do_not (int a, int b) { return !a; }

typedef int (*do_op_t)(int lhs, int rhs);

static const do_op_t DO_OP[256] = {

        [OR] = do_or,
        [AND] = do_and,
        [XOR] = do_xor,
        [NOT] = do_not

};

static const uint8_t TO_CODE[256] = {

        ['|'] = OR,
        ['&'] = AND,
        ['^'] = XOR,
        ['!'] = NOT

};

static const uint8_t IS_SPACE[256] = {

        [' '] = 1,
        ['\n'] = 1,
        ['\t'] = 1,
        ['\r'] = 1

};

typedef struct {

        op_t op;
        uint8_t lhs;
        uint8_t rhs;
        uint8_t acc;
        uint8_t mem;

} registers_t;

void reset_reg (registers_t *reg, int empty_mem) {

        reg -> lhs = REG_EMPTY;
        reg -> rhs = REG_EMPTY;
        reg -> op  = NONE;

        if (empty_mem) reg -> mem = REG_EMPTY;

}

int mov_operand (registers_t *reg, int value) {

        if (reg -> lhs == REG_EMPTY) {
                reg -> lhs = value;
        } else {
                reg -> rhs = value;
        }

        return 0;

}

int mov_values (registers_t *reg, int c) {

        switch (c) {

        case '1':
                mov_operand(reg, 1);
                return 0;
                break;

        case '0':
                mov_operand(reg, 0);
                return 0;
                break;
        }

        if (TO_CODE[c] != NONE) {
                reg -> op = TO_CODE[c];
                return 0;
        }

        return 101;

}

int evaluate (registers_t *reg) {

        if (
                reg -> op == NONE ||
                reg -> lhs == REG_EMPTY ||
                reg -> rhs == REG_EMPTY
        ) {
                return 0;
        }

        if (DO_OP[reg -> op] != NULL) {
                reg -> acc = DO_OP[reg -> op](reg -> lhs, reg -> rhs);
                reset_reg(reg, 0);
        }

        return 0;

}

int resolve_errorcodes (int code, int c) {

        switch (code) {

        case 101:
                fprintf(stderr, "BYTE ERROR: '%c' is not recognized.\nAbort.\n", c);
                return 1;
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

                if (IS_SPACE[c]) {
                        continue;
                }

                int result = mov_values(&reg, c);
                if (resolve_errorcodes(result, c)) {
                        return result;
                }
                printf("lhs: %d, op: %d, rhs: %d\n", reg.lhs, reg.op, reg.rhs);

                result = evaluate(&reg);
                if (resolve_errorcodes(result, c)) {
                        return result;
                }

        }

        printf("acc: %d\n", reg.acc);

        fclose(fp);
        return 0;

}
