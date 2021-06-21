#ifndef __CODE_H__
#define __CODE_H__

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct Code {
    uint32_t top;
    uint8_t bits[MAX_CODE_SIZE];
} Code;

Code code_init(void);

uint32_t code_size(Code *c);

bool code_empty(Code *c);

bool code_full(Code *c);

bool code_push_bit(Code *c, uint8_t bit);

bool code_pop_bit(Code *c, uint8_t *bit);

void code_print(Code *c);

#endif
