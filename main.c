#include <stdio.h>
#include <stdint.h>

int main (void) {

        FILE *fp = fopen("test", "rb");
        if (fp == NULL) {
                return 1;
        }

        int c = -1;
        int lhs = -1;
        int rhs = -1;
        int op = -1;
        int acc = -1;
        int mem = -1;

        int assign = 0;

        while ((c = fgetc(fp)) != EOF) {

                switch(c) {
                        case '1':
                        case '0': {
                                int val = c - '0';
                                if (lhs == -1) {
                                        lhs = val;
                                } else {
                                        rhs = val;
                                }
                                break;
                        }
                        case '|':
                                op = 0;
                                break;
                        case '&':
                                op = 1;
                                break;
                        case '-':
                                if (assign == 0) assign++;
                                continue;
                                break;
                        case '>':
                                if (assign == 1) assign++;
                                continue;
                                break;
                        case 'a':
                                if (assign == 2 && acc != -1) mem = acc;
                                assign = 0;
                                break;
                        default:
                                break;
                }

                if (lhs != -1 && rhs != -1 && op != -1) {
                        if (op == 0) {
                                acc = lhs || rhs;
                        } else {
                                acc = lhs && rhs;
                        }

                        lhs = acc;
                        rhs = -1;
                        op = -1;
                }

        }

        printf("acc: %d", acc);
        if (mem != -1) printf("mem: %d", mem);

        return 0;

}
