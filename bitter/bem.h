#pragma once

#include <stdint.h>
#include <stddef.h>

#define EMPTY_CELL      UINT8_MAX
#define MEMORY_SIZE     (UINT8_MAX + 1u)

#define R_ACC     251
#define R_PRINT   252
#define R_READ    253

typedef enum {

        OR,
        AND,
        XOR,
        FEED,
        RST_LHS,
        NOT,
        SHOW,
        READ,

        LOAD,

} instruction_set_t;

typedef struct internal_registers registers_t;

typedef struct {

        registers_t *regs;
        uint8_t *memory;

} bem_t;


bem_t *bem_init (size_t _mem_size);
void bem_destroy (bem_t **bem_o);
int bem_execute (bem_t *bem_o, instruction_set_t instruction, uint8_t valueA, uint8_t valueB);