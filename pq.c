#include "pq.h"

#include "huffman.h"
#include "node.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


struct PriorityQueue {
    uint32_t size;
    uint32_t capacity;
    Node **node;
};

static inline void swap(PriorityQueue *q, uint32_t i, uint32_t j) {
    Node *temp = q->node[i];
    q->node[i] = q->node[j];
    q->node[j] = temp;
    return;
}

static inline uint32_t parent(uint32_t i) {
    return ((i - 1) / 2);
}


static inline uint32_t right(uint32_t i) {
    return ((2 * i) + 2);
}


static inline uint32_t left(uint32_t i) {
    return ((2 * i) + 1);
}

static inline void heapify(PriorityQueue *q, uint32_t pos) {
    uint32_t l = left(pos);
    uint32_t r = right(pos);
    uint32_t smallest = pos;
    if (l < q->size && q->node[smallest]->frequency > q->node[l]->frequency) {
        smallest = l;
    }
    if (r < q->size && q->node[smallest]->frequency > q->node[r]->frequency) {
        smallest = r;
    }
    if (pos != smallest) {
        swap(q, smallest, pos);
        heapify(q, smallest);
    }
    return;
}

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    assert(q);
    q->size = 0;
    q->capacity = capacity;
    q->node = (Node **) calloc(capacity, sizeof(Node *));
    assert(q->node);
    return q;
}

void pq_delete(PriorityQueue **q) {
    if (*q) {
        for (uint32_t i = 0; i < (*q)->size; i++) {
            delete_tree(&(*q)->node[i]);
        }
        free((*q)->node);
        free(*q);
        *q = NULL;
    }
    return;
}


bool pq_empty(PriorityQueue *q) {
    return q->size <= 0;
}


bool pq_full(PriorityQueue *q) {
    return q->size == q->capacity;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

bool enqueue(PriorityQueue *q, Node *n) {
    if (q) {
        if (pq_full(q)) {
            return false;
        }
        q->size++;
        uint32_t t = q->size - 1;
        q->node[t] = n;
        while (t != 0 && q->node[parent(t)]->frequency > q->node[t]->frequency) {
            swap(q, t, parent(t));
            t = parent(t);
        }
        return true;
    }
    return false;
}

bool dequeue(PriorityQueue *q, Node **n) {
    if (q) {
        if (pq_empty(q)) {
            return false;
        }
        if (q->size == 1) {
            q->size--;
            *n = q->node[0];
            return true;
        }
        *n = q->node[0];
        q->node[0] = q->node[q->size - 1];
        q->size--;
        heapify(q, 0);
        return true;
    }
    return false;
}


void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size / 2; i++) {
        printf("Parent: %" PRId64 "", q->node[i]->frequency);
        printf(" Left Child: %" PRId64 "", q->node[left(i)]->frequency);
        printf(" Right Child: %" PRId64 "\n", q->node[right(i)]->frequency);
    }
}
