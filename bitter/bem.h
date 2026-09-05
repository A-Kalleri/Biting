#pragma once

#include <stdint.h>
#include <stddef.h>

#define EMPTY_CELL      UINT8_MAX
#define MEMORY_SIZE     (UINT8_MAX + 1u)

#define R_ACC           240
#define R_PRINT         241
#define R_READ          242

#define IMM             243 // Immediate
#define DRT             244 // Direct
#define REG             245 // Register

typedef enum {

        OR,
        AND,
        XOR,
        FEED,
        RFEED,
        RFEEDR,
        RST_LHS,
        NOT,
        SHOW,
        READ,

        LOAD,

} instruction_set_t;

typedef struct {

        uint8_t acc_empty;

} flags_t;

typedef struct internal_registers registers_t;

typedef struct {

        registers_t *regs;
        flags_t *flags;
        uint8_t *memory;

} bem_t;


bem_t *bem_constructor (size_t _mem_size);
void bem_destroy (bem_t **bem_o);
int bem_execute (bem_t *bem_o, instruction_set_t instruction, uint8_t valueA, uint8_t valueB);