#include "frodo_pke.h"

#include "mat.h"
#include "sha3.h"

#include <string.h>

uint16_t *compute_A(uchar seedA[LEN_BYTES_SEED_A]) {
    int lenA = sizeof(uint16_t) * N * N; // in bytes
    uint16_t *A = (uint16_t *)malloc(lenA);
    shake128(seedA, LEN_BYTES_SEED_A, lenA * sizeof(uint16_t), (uchar *)A);
    for (int i = 0; i < lenA; ++i)
        A[i] &= Q_PARAM; // make sure we're in bounds
    return A;
}

void frodo_encode(uchar k[L], uint16_t K[MBAR * NBAR]) {
    for (int i = 0; i < MBAR; ++i) {
        for (int j = 0; j < NBAR; ++j) {
            int idx = i * NBAR + j;
            uint16_t temp = 0;
            for (int l = 0; l < B_PARAM; ++l)
                if (get_bit_buffer(k, B_PARAM * idx + l))
                    temp += 1 << l;
            K[idx] = (temp * Q_PARAM) >> B_PARAM;
        }
    }
}

void frodo_decode(uint16_t K[MBAR * NBAR], uchar k[L]) {
    memset(k, 0, L);

    for (int i = 0; i < MBAR; ++i) {
        for (int j = 0; j < NBAR; ++j) {
            int idx = i * NBAR + j;

            uint16_t Kij = K[idx];
            uchar temp = (((Kij << B_PARAM) + (Q_PARAM >> 1)) / Q_PARAM) &
                         ((1 << B_PARAM) - 1);

            for (int l = 0; l < B_PARAM; ++l)
                set_bit_buffer(k, idx * B_PARAM + l, get_bit_u16(temp, l));
        }
    }
}

void frodo_pke_keygen(frodo_pke_pk *pk, frodo_pke_sk sk) {
    // 1 - Sampling seeds
    uchar seedSE[1 + LEN_BYTES_SEED_SE];
    seedSE[0] = 0x5f;
    random_bytes(pk->seedA, LEN_BYTES_SEED_A);
    random_bytes(&seedSE[1], LEN_BYTES_SEED_SE);

    // 2 - Generating LWE matrix
    uint16_t *A = compute_A(pk->seedA);

    // 3 - Sampling S and E
    uint16_t *S = (uint16_t *)malloc(sizeof(uint16_t) * N * NBAR);
    uint16_t *E = (uint16_t *)malloc(sizeof(uint16_t) * N * NBAR);
    uchar *r = (uchar *)malloc(2 * N * NBAR * LEN_CHI);
    shake128(seedSE, 1 + LEN_BYTES_SEED_SE, 2 * N * NBAR * LEN_CHI, r);
    sample_matrix(S, NBAR, N, (uint16_t *)r);
    sample_matrix(E, N, NBAR, (uint16_t *)&r[N * NBAR * LEN_CHI]);

    // 4 - Computing B
    mat_trans(S, N, NBAR, S);
    mat_mul(pk->B, N, N, NBAR, A, S);
    mat_add(pk->B, N, NBAR, pk->B, E);

    // 5 - Saving tS to sk
    mat_trans(sk, N, NBAR, S);

    free(A);
    free(S);
    free(E);
    free(r);
}

void frodo_pke_enc(frodo_pke_pk pk, uchar m[L],
                   uchar seedSE[1 + LEN_BYTES_SEED_SE], frodo_pke_cipher *c) {
    // 1 - Computing A
    uint16_t *A = compute_A(pk.seedA);

    // 3 - Sampling S', E' and E''
    uint16_t *S = (uint16_t *)malloc(sizeof(uint16_t) * MBAR * N);
    uint16_t *E1 = (uint16_t *)malloc(sizeof(uint16_t) * MBAR * N);
    uint16_t *E2 = (uint16_t *)malloc(sizeof(uint16_t) * MBAR * NBAR);
    size_t size_r = (2 * MBAR * N + MBAR * NBAR) * LEN_CHI;
    uchar *r = (uchar *)malloc(size_r);
    shake128(seedSE, 1 + LEN_BYTES_SEED_SE, size_r, r);
    sample_matrix(S, MBAR, N, (uint16_t *)r);
    sample_matrix(E1, MBAR, N, (uint16_t *)&r[MBAR * N * LEN_CHI]);
    sample_matrix(E2, MBAR, NBAR, (uint16_t *)&r[2 * MBAR * N * LEN_CHI]);

    // 4 - Computing B'
    mat_mul(c->C1, MBAR, N, N, S, A);
    mat_add(c->C1, MBAR, N, c->C1, E1);

    // 4 - Computing V
    mat_mul(c->C2, MBAR, N, NBAR, S, pk.B);
    mat_add(c->C2, MBAR, NBAR, c->C2, E2);

    // 5 - Encode message
    uint16_t *M = (uint16_t *)malloc(sizeof(uint16_t) * MBAR * NBAR);
    frodo_encode(m, M);
    mat_add(c->C2, MBAR, NBAR, c->C2, M);

    free(A);
    free(r);
    free(S);
    free(E1);
    free(E2);
    free(M);
}

void frodo_pke_dec(frodo_pke_sk sk, frodo_pke_cipher c, uchar m[L]) {
    // 1 - Computes M
    uint16_t *M = (uint16_t *)malloc(sizeof(uint16_t) * MBAR * NBAR);
    mat_mul(M, MBAR, N, NBAR, c.C1, sk);
    mat_sub(M, MBAR, NBAR, c.C2, M);

    // 2 - Decoding
    frodo_decode(M, m);

    free(M);
}
