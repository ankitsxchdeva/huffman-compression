#include "huffman.h"

#include "node.h"
#include "pq.h"
#include "stack.h"
#include "variables.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

uint32_t symbols = 0;


static inline void traverse(Node *root, Code table[static ALPHABET], Code c) {
    if (root) {
        uint8_t temp;
        if (root->left == NULL && root->right == NULL) {
            table[root->symbol] = c;
        } 
            code_push_bit(&c, 0);
            traverse(root->left, table, c);
            code_pop_bit(&c, &temp);

            code_push_bit(&c, 1);
            traverse(root->right, table, c);
            code_pop_bit(&c, &temp);
        }
    }
    return;
}


Node *build_tree(uint64_t hist[static ALPHABET]) {
    Node *p;
    Node *left, *right;
    PriorityQueue *pq = pq_create(ALPHABET);
    for (uint32_t i = 0; i < ALPHABET; i++) {
        if (hist[i] > 0) {
            Node *n = node_create(i, hist[i]);
            enqueue(pq, n);
            symbols++;
        }
    }
    while (pq_size(pq) != 1) {
        dequeue(pq, &left);
        dequeue(pq, &right);
        Node *n = node_join(left, right);
        enqueue(pq, n);
    }
    dequeue(pq, &p);
    pq_delete(&pq);
    return p;
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    Code c = code_init();
    traverse(root, table, c);
    return;
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Node *root;
    Stack *s = stack_create(nbytes);
    for (uint16_t i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') {
            Node *n = node_create(tree[i + 1], 0);
            stack_push(s, n);
        } else if (tree[i] == 'I') {
            Node *right, *left;
            stack_pop(s, &right);
            stack_pop(s, &left);
            Node *n = node_join(left, right);
            stack_push(s, n);
        }
        if (i != nbytes - 1) {
            if ((tree[i + 1] == 'I' || tree[i + 1] == 'L') && tree[i] == 'L') {
                tree[i + 1] = '\0';
            }
        }
    }
    stack_pop(s, &root);
    stack_delete(&s);
    return root;
}


void delete_tree(Node **root) {
    if (*root) {
        if ((*root)->left) {
            delete_tree(&(*root)->left);
        }
        if ((*root)->right) {
            delete_tree(&(*root)->right);
        }
        node_delete(root); 
    }
}
