#include <asm-generic/errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"
#include "bem.h"

typedef struct internal_registers {

        uint8_t acc;

        uint8_t print;
        uint8_t read;

} registers_t;

void bem_destroy (bem_t **bem_o) {

        if (!bem_o || !*bem_o) {
                return;
        }

        free((*bem_o) -> regs);
        free((*bem_o) -> memory);
        free((*bem_o) -> flags);
        free(*bem_o);

        *bem_o = NULL;
        return;

}

bem_t *bem_constructor (size_t _mem_size) {

        bem_t *_bem_image = calloc(1, sizeof(*_bem_image));
        if (!_bem_image) {
                write_stderr("ERROR: Bitter Execution Machine not Initialized.");
                return NULL;
        }

        _bem_image -> regs = calloc(1, sizeof(*_bem_image -> regs));
        if (!_bem_image -> regs) {

                write_stderr("ERROR: BEM Registers not Connected.");
                bem_destroy(&_bem_image);
                return NULL;

        }

        _bem_image -> memory = malloc(_mem_size * sizeof(*_bem_image -> memory));
        if (!_bem_image -> memory) {

                write_stderr("ERROR: No Memory detected for BEM.");
                bem_destroy(&_bem_image);
                return NULL;

        }

        memset(_bem_image -> memory, EMPTY_CELL, _mem_size);

        _bem_image -> flags = calloc(1, sizeof(*_bem_image -> flags));
        if (!_bem_image -> memory) {

                write_stderr("ERROR: Flags not Initialized.");
                bem_destroy(&_bem_image);
                return NULL;

        }

        return _bem_image;

}

int bem_execute (bem_t *bem_o, instruction_set_t instruction, uint8_t valueA, uint8_t valueB) {

        switch (instruction) {

        case OR:
                if (valueA == IMM) {
                        bem_o -> regs -> acc |= valueB;
                } else if (valueA == DRT) {
                        bem_o -> regs -> acc |= bem_o -> memory[valueB];
                } else {
                        return 1;
                }
                break;

        case AND:
                if (valueA == IMM) {
                        bem_o -> regs -> acc &= valueB;
                } else if (valueA == DRT) {
                        bem_o -> regs -> acc &= bem_o -> memory[valueB];
                } else {
                        return 1;
                }
                break;

        case XOR:
                if (valueA == IMM) {
                        bem_o -> regs -> acc ^= valueB;
                } else if (valueA == DRT) {
                        bem_o -> regs -> acc ^= bem_o -> memory[valueB];
                } else {
                        return 1;
                }
                break;

        case FEED:
                if (valueA == R_READ) {
                        bem_o -> memory[valueB] = bem_o -> regs -> read;
                } else if (valueA == R_ACC) {
                        bem_o -> memory[valueB] = bem_o -> regs -> acc;
                }
                if (valueA == R_PRINT) {
                        bem_o -> memory[valueB] = bem_o -> regs -> print;
                }
                else {
                        return 1;
                }
                break;

        case RFEED:
                if (valueA == R_READ) {
                        bem_o -> regs -> read = bem_o -> regs -> acc;
                } else if (valueA == R_PRINT) {
                        bem_o -> regs -> print = bem_o -> regs -> acc;
                } else {
                        return 1;
                }
                break;

        case RFEEDR:
                if (valueA == R_READ) {
                        bem_o -> regs -> print = bem_o -> regs -> read;
                } else if (valueA == R_PRINT) {
                        bem_o -> regs -> read = bem_o -> regs -> print;
                }

        case RST_LHS:
                bem_o -> regs -> acc = EMPTY_CELL;
                bem_o -> flags -> acc_empty = 1;
                break;

        case NOT:
                bem_o -> regs -> acc = !bem_o -> regs -> acc;
                break;

        case SHOW:
                if (valueA == IMM) {
                        write_stdout_char(bem_o -> regs -> print);
                } else if (valueA == DRT) {
                        write_stdout_char(bem_o -> regs -> acc);
                } else {
                        return 1;
                }
                break;

        case READ:
                bem_o -> regs -> read = read_stdin_bit();
                break;

        case LOAD:
                if (valueA == R_ACC) {
                        bem_o -> regs -> acc = valueB;
                        bem_o -> flags -> acc_empty = 0;
                } else if (valueA == R_PRINT) {
                        bem_o -> regs -> print = valueB;
                } else {
                        return 1; // unknown register
                }
                break;

        default:
                return 1; // unknown instruction
 
        }

        return 0;

}