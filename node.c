#include "node.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    assert(n);
    n->left = NULL;
    n->right = NULL;
    n->symbol = symbol;
    n->frequency = frequency;
    return n;
}

void node_delete(Node **n) {
    if (*n) {
        // Free Memory
        free(*n);
        *n = NULL;
    }
    return;
}

Node *node_join(Node *left, Node *right) {
    Node *n = node_create('$', left->frequency + right->frequency);
    n->left = left;
    n->right = right;
    return n;
}


void node_print(Node *n) {
    printf("Parent Node Frequency: %" PRId64 " Parent node symbol: %d\n", n->frequency, n->symbol);
    if (n->left && n->right) {
        printf("Left child frequency '%c': %" PRId64 " Right child frequency '%c': %" PRId64 "\n",
            n->left->symbol, n->left->frequency, n->right->symbol, n->right->frequency);
    }
    return;
}
