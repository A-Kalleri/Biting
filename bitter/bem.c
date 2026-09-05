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
        free(*bem_o);

        *bem_o = NULL;
        return;

}

bem_t *bem_init (size_t _mem_size) {

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

        return _bem_image;

}

int bem_execute (bem_t *bem_o, instruction_set_t instruction, uint8_t valueA, uint8_t valueB) {

        switch (instruction) {

        case OR:
                bem_o -> regs -> acc |= valueA;
                break;

        case AND:
                bem_o -> regs -> acc &= valueA;
                break;

        case XOR:
                bem_o -> regs -> acc ^= valueA;
                break;

        case FEED:
                bem_o -> memory[valueA] = valueB;
                break;

        case RST_LHS:
                bem_o -> regs -> acc = EMPTY_CELL;
                break;
        case NOT:
                bem_o -> regs -> acc = !bem_o -> regs -> acc;
                break;

        case SHOW:
                write_stdout_char(valueA);
                break;

        case READ:
                bem_o -> regs -> read = read_stdin_bit();
                break;

        case LOAD:
                if (valueA == R_ACC) {
                        bem_o -> regs -> acc = valueB;
                        break;
                } else if (valueA == R_PRINT) {
                        bem_o -> regs -> print = valueB;
                        break;
                } else {
                        return 1; // unknown register
                }

        default:
                return 1; // unknown instruction
 
        }

        return 0;

}