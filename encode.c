#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "variables.h"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define OPTIONS "hvi:o:"

struct stat statbuf;

static inline void write_header(int outfile, Header *h) {
    write_bytes(outfile, (uint8_t *) h, sizeof(Header));
}



static inline void write_stats(int infile, Header header) {
    fstat(infile, &statbuf);
    double stat
        = 100 * (1 - (((double) bytes_written - header.file_size) / (double) header.file_size));
    fprintf(stderr, "Uncompressed file size: %" PRId64 " bytes\n", header.file_size);
    fprintf(stderr, "Compressed file size: %" PRId64 " bytes\n", bytes_written - header.file_size);
    fprintf(stderr, "Space saving: %0.2lf%c\n", stat, '%');
}


static inline int seek(int tmp, int infile) {
    int file;
    if (infile == STDIN_FILENO && tmp != 0) {
        lseek(tmp, 0, SEEK_SET);
        file = tmp;
    } else {
        lseek(infile, 0, SEEK_SET);
        file = infile;
    }
    return file;
}

static inline Header create_header(int infile, int outfile) {
    Header header;
    fstat(infile, &statbuf);
    fchmod(outfile, statbuf.st_mode);
    header.magic = MAGIC;
    header.permissions = statbuf.st_mode;
    header.tree_size = (3 * symbols) - 1;
    header.file_size = statbuf.st_size;
    return header;
}


static inline void traverse(Node *root, int outfile) {
    if (root) {
        uint8_t l = 'L';
        uint8_t i = 'I';
        if (root->left == NULL && root->right == NULL) {
            write_bytes(outfile, &l, 1);
            write_bytes(outfile, &root->symbol, 1);
        } else {
            traverse(root->left, outfile);
            traverse(root->right, outfile);
            write_bytes(outfile, &i, 1);
        }
    }
}

static inline void read_file(int infile, uint64_t *hist, int fno) {
    uint8_t buf[BLOCK];
    int status = 0;
    do {
        status = read_bytes(infile, buf, sizeof(buf));
        for (uint64_t i = 0; i < bytes_read; i++) {
            hist[buf[i]] += 1;
        }
        if (infile == STDIN_FILENO && fno != 0) {
            write_bytes(fno, buf, bytes_read);
        }
    } while (status != 0);
    return;
}

uint8_t huffman(int infile, int outfile, bool verbose) {
    uint64_t hist[ALPHABET] = { 0 };
    hist[0]++;
    hist[255]++;

    int fno = 0;
    off_t offset;
    char file_name[255];
    if (offset = lseek(infile, 0, SEEK_SET), offset == -1) {
        time_t t;
        time(&t);
        sprintf(file_name, "/tmp/huffman.%s", ctime(&t));
        fno = open(file_name, O_RDWR | O_CREAT | O_EXCL | O_TRUNC, 0600);
    }
    read_file(infile, hist, fno);

    Node *root = build_tree(hist);
    Code table[ALPHABET] = { 0 };
    build_codes(root, table);

    int file = seek(fno, infile);

    Header header = create_header(file, outfile);
    write_header(outfile, &header);
    traverse(root, outfile);

    uint8_t buf[BLOCK];
    int status = 0;
    do {
        status = read_bytes(file, buf, sizeof(buf));
        for (uint64_t i = 0; i < bytes_read; i++) {
            Code code = table[buf[i]];
            write_code(outfile, &code);
        }
        memset(buf, 0, sizeof(buf));
    } while (status != 0);

    flush_codes(outfile);

    if (verbose) {
        write_stats(infile, header);
    }

    if (infile == STDIN_FILENO && fno != 0) {
        unlink(file_name);
        close(fno);
    }

    delete_tree(&root);
    close(infile);
    close(outfile);
    return 0;
}

static inline void no_argument() {
    fprintf(stderr, "SYNOPSIS\n   A Huffman encoder.\n"
                    "   Compresses a file using the Huffman coding algorithm."
                    "\n\nUSAGE\n     ./encode [-h] [-i infile] [-o outfile]"
                    "\n\nOPTIONS\n  -h             Program usage and help.\n"
                    "  -v             Print compression statistics.\n"
                    "  -i infile      Input data to encode.\n"
                    "  -o outfile     Output of encoded data.\n");
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
    return huffman(infile, outfile, verbose);
}
