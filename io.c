#include "io.h"

#include "variables.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

uint8_t buffer[BLOCK];
uint64_t bits_read = 0, bits_added = 0;
uint64_t bytes_read = 0, bytes_written = 0;
uint64_t total_bytes_read = 0;


int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int rb = 0;
    bytes_read = 0;
    for (int i = 0; i < nbytes; i++) {
        if (rb = read(infile, buf + bytes_read, nbytes - bytes_read), rb > 0) {
            bytes_read += rb;
            total_bytes_read += rb;
            if ((int)bytes_read == nbytes) {
                break;
            }
        } else {
            break;
        }
    }
    return rb;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int wb = 0;
    int bytes_w = 0;
    for (int i = 0; i < nbytes; i++) {
        if (wb = write(outfile, buf + bytes_w, nbytes - bytes_w), wb > 0) {
            bytes_w += wb;
            bytes_written += wb;
            if (bytes_w == nbytes) {
                break;
            }
        } else {
            break;
        }
    }
    return wb;
}

bool read_bit(int infile, uint8_t *bit) {
    int end = -1;
    if (bits_read == 0) {
        read_bytes(infile, buffer, BLOCK);

        if (bytes_read < BLOCK) {
            end = 8 * bytes_read + 1;
        }
    }
    *bit = (buffer[bits_read / 8] >> (bits_read % 8) & 0x1);
    bits_read = (bits_read + 1) % (BLOCK * 8);
    if ((int) bits_read == end) {
        return false;
    }
    return true;
}

void write_code(int outfile, Code *c) {
    for (uint32_t i = 0; i < code_size(c); i++) {
        uint8_t bit = (c->bits[i / 8] >> (i % 8) & 0x1);
        if (bit == 1) {
            buffer[bits_added / 8] |= (0x1 << (bits_added % 8));
        } else {
            buffer[bits_added / 8] &= ~(0x1 << (bits_added % 8));
        }
        bits_added = (bits_added + 1) % (BLOCK * 8);

        if (bits_added == 0) {
            write_bytes(outfile, buffer, BLOCK);
            memset(buffer, 0, sizeof(buffer));
        }
    }
    return;
}

void flush_codes(int outfile) {
    if (bits_added > 0) {
        uint64_t bytes = bits_added % (BLOCK * 8) / 8;
        if (bits_added % 8 > 0) {
            bytes++;
        }
        write_bytes(outfile, buffer, bytes);
    }
    return;
}
