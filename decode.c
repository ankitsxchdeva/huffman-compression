
#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "pq.h"
#include "variables.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "hvi:o:"


static inline void read_header(int infile, Header *h) {
    read_bytes(infile, (uint8_t *) h, sizeof(Header));
    return;
}


static inline void set_permissions(int outfile, Header *h) {
    fchmod(outfile, h->permissions);
    return;
}

static inline void read_tree(Node *root, int infile, int outfile, Header *h) {
    Node *n = root;
    uint64_t i = 0;
    uint8_t buf[BLOCK];
    while (i < h->file_size) {
        uint8_t bit;
        if (n->left == NULL && n->right == NULL) {
            buf[i % BLOCK] = n->symbol;
            n = root;
            i++;
            if (i % BLOCK == 0 && i != 0) {
                write_bytes(outfile, buf, sizeof(buf));
            }
        }
        read_bit(infile, &bit);
        if (bit == 0 && n->left != NULL) {
            n = n->left;
        }
        if (bit == 1 && n->right != NULL) {
            n = n->right;
        }
    }
    if (i % BLOCK != 0 && i != 0) {
        write_bytes(outfile, buf, (i % BLOCK));
    }
    delete_tree(&root);
    return;
}

uint8_t read_file(int infile, int outfile, bool verbose) {
    Header h;
    read_header(infile, &h);
    if (h.magic != MAGIC) {
        fprintf(stderr, "Error: invalid MAGIC number.\n");
        return 1;
    }
    uint8_t tree[h.tree_size];
    set_permissions(outfile, &h);
    read_bytes(infile, tree, h.tree_size);

    Node *root = rebuild_tree(h.tree_size, tree);
    read_tree(root, infile, outfile, &h);
    if (verbose) {
        double stat = 100 * (1 - ((double) total_bytes_read / (double) h.file_size));
        fprintf(stderr, "Compressed file size: %" PRId64 " bytes\n", total_bytes_read);
        fprintf(stderr, "Decompressed file size: %" PRId64 " bytes\n", h.file_size);
        fprintf(stderr, "Space saving: %0.2lf%c\n", stat, '%');
    }
    close(infile);
    close(outfile);
    return 0;
}

static inline void no_argument() {
    fprintf(stderr, "SYNOPSIS\n   A Huffman decoder.\n"
                    "   Decompresses a file using the Huffman coding algorithm."
                    "\n\nUSAGE\n     ./decode [-h] [-i infile] [-o outfile]"
                    "\n\nOPTIONS\n  -h             Program usage and help.\n"
                    "  -v             Print compression statistics.\n"
                    "  -i infile      Input data to decompress.\n"
                    "  -o outfile     Output of decompressed data.\n");
    return;
}

int main(int argc, char *argv[]) {
    int infile = STDIN_FILENO, outfile = STDOUT_FILENO;
    bool verbose = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': no_argument(); return 1;
        case 'v': verbose = true; break;
        case 'i':
            if ((infile = open(optarg, O_RDONLY | O_CREAT)) == -1) {
                fprintf(stderr, "Error: failed to open infile.\n");
                return 1;
            }
            break;
        case 'o':
            if ((outfile = open(optarg, O_WRONLY | O_CREAT)) == -1) {
                fprintf(stderr, "Error: Unable to write file\n");
                return 1;
            }
            break;
        case '?': no_argument(); return 1;
        default: abort(); return 1;
        }
    }
    return read_file(infile, outfile, verbose);
}
