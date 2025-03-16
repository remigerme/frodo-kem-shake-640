#ifndef FRODO_UTILS_H
#define FRODO_UTILS_H

#include <inttypes.h>
#include <stdlib.h>

#define DISTRIB_S 12
#define D_PARAM 15
#define Q_PARAM ((1 << D_PARAM) - 1)
#define LEN_CHI 2

// Building a little wrapper for debugging purposes
#ifdef FRODO_DEBUG
#define _FRODO_DEBUG 1
#else
#define _FRODO_DEBUG 0
#endif

// https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c
#define DEBUG_LOG(...)                                                         \
    do {                                                                       \
        if (_FRODO_DEBUG)                                                      \
            fprintf(stderr, __VA_ARGS__);                                      \
    } while (0)

typedef unsigned char uchar;

void random_bytes(uchar *buf, size_t size);

void sample_matrix(uint16_t *A, size_t m, size_t n, uint16_t *r);

int get_bit_buffer(uchar *m, int i);           // buffer of size L
void set_bit_buffer(uchar *b, int i, int val); // buffer of size L

int get_bit_u16(uint16_t c, int i);
void set_bit_u16(uint16_t *c, int i, int val);

#endif // FRODO_UTILS_H
