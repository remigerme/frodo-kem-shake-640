#include "frodo_utils.h"

#include <assert.h>
#include <stdio.h>

void random_bytes(uchar *buf, size_t size) {
    FILE *fd = fopen("/dev/urandom", "rb");
    if (fd == NULL) {
        perror("Couldn't open /dev/urandom\n");
        exit(EXIT_FAILURE);
    }
    if (fread(buf, 1, size, fd) < size) {
        fprintf(stderr, "Couldn't read %lu bytes from /dev/urandom\n", size);
        exit(EXIT_FAILURE);
    }
    fclose(fd);
}

char SAMPLE_TABLE[DISTRIB_S] = {0}; // TODO

uint16_t sample(uint16_t r) {
    uint16_t t = r >> 1;
    uint16_t e = 0;
    for (int z = 0; z < DISTRIB_S; ++z)
        if (t > SAMPLE_TABLE[z])
            ++e;

    if (r & 1 && e > 0)
        e = Q_PARAM - e;

    return e;
}

void sample_matrix(uint16_t *A, size_t m, size_t n, uint16_t *r) {
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            A[i * n + j] = sample(r[i * n + j]);
}

int get_bit_buffer(uchar *m, int i) { return (m[i / 8] >> (i % 8)) & 1; }

void set_bit_buffer(uchar *b, int i, int val) {
    int byte = i / 8;
    int bit = i % 8;
    if (val)
        b[byte] |= (1 << bit);
    else
        b[byte] &= ~(1 << bit);
}

int get_bit_u16(uint16_t c, int i) {
    assert(0 <= i && i < 16);
    return (c >> i) & 1;
}

void set_bit_u16(uint16_t *c, int i, int val) {
    assert(0 <= i && i < 16);
    if (val)
        *c |= (1 << i);
    else
        *c &= ~(1 << i);
}
