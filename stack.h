#ifndef __STACK_H__
#define __STACK_H__

#include "node.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct Stack Stack;

Stack *stack_create(uint32_t capacity);

void stack_delete(Stack **s);

bool stack_empty(Stack *s);

bool stack_full(Stack *s);

uint32_t stack_size(Stack *s);

bool stack_push(Stack *s, Node *n);

bool stack_pop(Stack *s, Node **n);

void stack_print(Stack *s);

#endif
