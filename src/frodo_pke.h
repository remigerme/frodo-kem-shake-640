#ifndef FRODO_PKE_H
#define FRODO_PKE_H

#include "frodo_utils.h"

#include <inttypes.h>

#define N 640
#define NBAR 8
#define MBAR 8
#define B_PARAM 2
#define L ((B_PARAM * MBAR * NBAR) / 8)
#define LEN_BYTES_SEED_A 16
#define LEN_BYTES_SEED_SE 32
#define DISTRIB_S 12

typedef struct frodo_pke_pk {
    uchar seedA[LEN_BYTES_SEED_A];
    uint16_t B[N * NBAR];
} frodo_pke_pk;

typedef uint16_t frodo_pke_sk[NBAR * N];

typedef struct frodo_pke_cipher {
    uint16_t C1[MBAR * N];
    uint16_t C2[MBAR * NBAR];
} frodo_pke_cipher;

void frodo_pke_keygen(frodo_pke_pk *pk, frodo_pke_sk sk);

void frodo_pke_enc(frodo_pke_pk pk, uchar m[L],
                   uchar seedSE[1 + LEN_BYTES_SEED_SE], frodo_pke_cipher *c);

void frodo_pke_dec(frodo_pke_sk sk, frodo_pke_cipher c, uchar m[L]);

#endif // FRODO_PKE_H
