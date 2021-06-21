#include "code.h"
#include "defines.h"
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Code code_init(void) {
    Code c;
    c.top = 0;
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i++) {
        c.bits[i] = 0;
    }
    return c;
}

uint32_t code_size(Code *c) {
    return c->top;
}

bool code_empty(Code *c) {
    return c->top == 0;
}


bool code_full(Code *c) {
    return c->top == MAX_CODE_SIZE;
}


bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    c->bits[c->top / 8] |= (bit << (c->top % 8));
    c->top++;
    return true;
}


bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    c->top--;
    *bit = (c->bits[c->top / 8] >> (c->top % 8) & 0x1);
    c->bits[c->top / 8] &= ~(0x1 << (c->top % 8));
    return true;
}

void code_print(Code *c) {
    for (uint32_t i = 0; i < c->top; i++) {
        printf("%d", (c->bits[i / 8] >> (i % 8) & 0x1));
    }
    printf("\n");
    return;
}
