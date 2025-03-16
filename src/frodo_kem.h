#ifndef FRODO_KEM_H
#define FRODO_KEM_H

#include "frodo_pke.h"

#define LEN_PKH 16
#define LEN_S 16
#define LEN_MU 16
#define LEN_K 16
#define LEN_SS 16

#define SIZE_KEM_PK (LEN_BYTES_SEED_A + D_PARAM * N * NBAR)
#define SIZE_CIPHER (D_PARAM * MBAR * (N + NBAR))

typedef uchar frodo_kem_pk[SIZE_KEM_PK];

typedef struct frodo_kem_sk {
    uchar s[LEN_S];
    uchar *pk;    // requires manual heap allocation
    uint16_t *tS; // idem - NBAR * N
    uchar pkh[LEN_PKH];
} frodo_kem_sk;

frodo_kem_sk new_frodo_kem_sk(void);
void free_frodo_kem_sk(frodo_kem_sk *ptr);

typedef uchar frodo_kem_cipher[SIZE_CIPHER];

/**
 * @param pk Output generated publick key
 * @param sk Output generated private key
 */
void frodo_kem_keygen(frodo_kem_pk pk, frodo_kem_sk *sk);

/**
 * @param pk Input public key
 * @param c Output ciphertext
 * @param ss Output symmetric key
 */
void frodo_kem_encaps(frodo_kem_pk pk, frodo_kem_cipher c, uchar ss[LEN_SS]);

/**
 * @param sk Input private key
 * @param c Input ciphertext
 * @param ss Output symmetric key
 */
void frodo_kem_decaps(frodo_kem_sk sk, frodo_kem_cipher c, uchar ss[LEN_SS]);

/************/
/* IO UTILS */
/************/

void load_pk_from_file(char *filename, frodo_kem_pk pk);
frodo_kem_sk load_sk_from_file(char *filename);
void load_cipher_from_file(char *filename, frodo_kem_cipher c);

void save_pk_to_file(char *filename, frodo_kem_pk pk);
void save_sk_to_file(char *filename, frodo_kem_sk sk);
void save_cipher_to_file(char *filename, frodo_kem_cipher c);
void save_shared_to_file(char *filename, uchar ss[LEN_SS]);

#endif // FRODO_KEM_H
