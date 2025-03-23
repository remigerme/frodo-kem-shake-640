#include "mat.h"

void mat_add(uint16_t *rop, size_t m, size_t n, uint16_t *A, uint16_t *B) {
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            rop[n * i + j] = (A[n * i + j] + B[n * i + j]) & (Q_PARAM - 1);
}

void mat_sub(uint16_t *rop, size_t m, size_t n, uint16_t *A, uint16_t *B) {
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            rop[n * i + j] = (A[n * i + j] - B[n * i + j]) & (Q_PARAM - 1);
}

void mat_mul(uint16_t *rop, size_t m, size_t n, size_t p, uint16_t *A,
             uint16_t *B) {
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < p; ++j) {
            rop[p * i + j] = 0;
            for (size_t k = 0; k < n; ++k)
                rop[p * i + j] =
                    (rop[p * i + j] + (A[n * i + k] * B[p * k + j]) &
                     (Q_PARAM - 1)) &
                    (Q_PARAM - 1);
        }
}

void mat_trans(uint16_t *rop, size_t m, size_t n, uint16_t *A) {
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
            rop[j * m + i] = A[i * n + j];
}
