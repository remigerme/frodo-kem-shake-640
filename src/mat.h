#ifndef MAT_H
#define MAT_H

#include "frodo_utils.h"

#include <inttypes.h>
#include <stdlib.h>

void mat_add(uint16_t *rop, size_t m, size_t n, uint16_t *A, uint16_t *B);

void mat_sub(uint16_t *rop, size_t m, size_t n, uint16_t *A, uint16_t *B);

void mat_mul(uint16_t *rop, size_t m, size_t n, size_t p, uint16_t *A,
             uint16_t *B);

void mat_trans(uint16_t *rop, size_t m, size_t n, uint16_t *A);

#endif // MAT_H
