#include "stack.h"
#include "huffman.h"
#include "node.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};


Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack));
    assert(s);
    s->capacity = capacity;
    s->top = 0;
    s->items = (Node **) calloc(capacity, sizeof(Node *));
    assert(s->items);
    return s;
}


void stack_delete(Stack **s) {
    if (*s) {
        for (uint32_t i = 0; i < (*s)->top; i++) {
            delete_tree(&(*s)->items[i]);
        }
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
    return;
}

bool stack_empty(Stack *s) {
    return s->top == 0;
}


bool stack_full(Stack *s) {
    return s->top == s->capacity;
}


uint32_t stack_size(Stack *s) {
    return s->top;
}

bool stack_push(Stack *s, Node *n) {
    if (!stack_full(s)) {
        if (s->items == NULL) {
            return false;
        }
        s->items[s->top] = n;
        s->top++;
        return true;
    }
    return false;
}

bool stack_pop(Stack *s, Node **n) {
    if (!stack_empty(s)) {
        s->top--;
        *n = s->items[s->top];
        return true;
    }
    return false;
}

void stack_print(Stack *s) {
    for (uint32_t i = 0; i < s->top; i++) {
        node_print(s->items[i]);
    }
}
