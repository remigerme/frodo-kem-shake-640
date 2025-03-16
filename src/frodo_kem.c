#include "frodo_kem.h"

#include "sha3.h"

#include <stdio.h>
#include <string.h>

void frodo_pack(uint16_t *C, size_t m, size_t n, uchar *b) {
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            int idx = i * n + j;
            uint16_t Cij = C[idx];
            for (int l = 0; l < D_PARAM; ++l)
                set_bit_buffer(b, idx * D_PARAM + l,
                               get_bit_u16(Cij, D_PARAM - 1 - l));
        }
    }
}

void frodo_unpack(uchar *b, size_t m, size_t n, uint16_t *C) {
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            int idx = i * n + j;
            C[idx] = 0;
            for (int l = 0; l < D_PARAM; ++l)
                set_bit_u16(&C[idx], D_PARAM - 1 - l,
                            get_bit_buffer(b, idx * D_PARAM + l));
        }
    }
}

frodo_kem_sk new_frodo_kem_sk() {
    frodo_kem_sk sk;
    sk.pk = (uchar *)malloc(SIZE_KEM_PK);
    sk.tS = (uint16_t *)malloc(sizeof(uint16_t) * NBAR * N);
    return sk;
}

void free_frodo_kem_sk(frodo_kem_sk *ptr) {
    free(ptr->pk);
    free(ptr->tS);
}

void frodo_kem_keygen(frodo_kem_pk pk, frodo_kem_sk *sk) {
    // Using PKE.Keygen
    frodo_pke_pk pke_pk;
    frodo_pke_sk pke_sk;
    DEBUG_LOG("Calling PKE.Keygen\n");
    frodo_pke_keygen(&pke_pk, pke_sk);

    // Setting pk
    DEBUG_LOG("Calling Frodo.Pack\n");
    frodo_pack(pke_pk.B, N, NBAR, &pk[LEN_BYTES_SEED_A]);
    memcpy(pk, pke_pk.seedA, LEN_BYTES_SEED_A);
    memcpy(sk->pk, pk, SIZE_KEM_PK);

    // Computing pkh
    DEBUG_LOG("Computing PKH\n");
    shake128(pk, SIZE_KEM_PK, LEN_PKH, sk->pkh);

    // Generating s
    DEBUG_LOG("Generating s\n");
    random_bytes(sk->s, LEN_S);
}

void frodo_kem_encaps(frodo_kem_pk pk, frodo_kem_cipher c, uchar ss[LEN_SS]) {
    uchar pkh_mu[LEN_PKH + LEN_MU];
    uchar *pkh = pkh_mu;
    uchar *mu = &pkh_mu[LEN_PKH];

    random_bytes(mu, LEN_MU);
    shake128(pk, SIZE_KEM_PK, LEN_PKH, pkh);
    uchar seedSE_k[LEN_BYTES_SEED_SE + LEN_K];
    uchar *seedSE = seedSE_k;
    uchar *k = &seedSE[LEN_BYTES_SEED_SE];
    shake128(pkh_mu, LEN_PKH + LEN_MU, LEN_BYTES_SEED_SE + LEN_K, seedSE_k);

    frodo_pke_pk pke_pk;
    memcpy(pke_pk.seedA, pk, LEN_BYTES_SEED_A);
    frodo_unpack(&pk[LEN_BYTES_SEED_A], N, NBAR, pke_pk.B);

    frodo_pke_cipher pke_c;
    int len_c1 = D_PARAM * MBAR * N;
    int len_c2 = D_PARAM * MBAR * NBAR;
    frodo_pke_enc(pke_pk, mu, seedSE, &pke_c);
    frodo_pack(pke_c.C1, MBAR, N, c);
    frodo_pack(pke_c.C2, MBAR, NBAR, &c[len_c1]);

    uchar *buf = (uchar *)malloc(len_c1 + len_c2 + LEN_K);
    memcpy(buf, c, len_c1);
    memcpy(&buf[len_c1], &c[len_c1], len_c2);
    memcpy(&buf[len_c1 + len_c2], k, LEN_K);

    shake128(buf, len_c1 + len_c2 + LEN_K, LEN_SS, ss);
}

int frodo_pke_cipher_eq(frodo_pke_cipher c1, frodo_pke_cipher c2) {
    int len_c1 = MBAR * N;
    int len_c2 = MBAR * NBAR;

    int eq = 1;
    for (int i = 0; i < len_c1; ++i)
        eq &= c1.C1[i] == c2.C1[i];

    for (int i = 0; i < len_c2; ++i)
        eq &= c1.C2[i] == c2.C2[i];

    return eq;
}

void frodo_kem_decaps(frodo_kem_sk sk, frodo_kem_cipher c, uchar ss[LEN_SS]) {
    int len_c1 = D_PARAM * MBAR * N;

    frodo_pke_cipher pke_c; // = (B', C)
    frodo_unpack(c, MBAR, N, pke_c.C1);
    frodo_unpack(&c[len_c1], MBAR, NBAR, pke_c.C2);

    uchar mu1[LEN_MU];
    frodo_pke_dec(sk.tS, pke_c, mu1);

    frodo_pke_pk pke_pk;
    memcpy(pke_pk.seedA, sk.pk, LEN_BYTES_SEED_A);
    frodo_unpack(&sk.pk[LEN_BYTES_SEED_A], N, NBAR, pke_pk.B);

    uchar seedSE_k[1 + LEN_BYTES_SEED_SE + LEN_K];
    seedSE_k[0] = 0x96;
    uchar pkhmu[LEN_PKH + LEN_PKH];
    memcpy(pkhmu, sk.pkh, LEN_PKH);
    memcpy(&pkhmu[LEN_PKH], mu1, LEN_MU);
    shake128(pkhmu, LEN_PKH + LEN_MU, LEN_BYTES_SEED_SE + LEN_K, &seedSE_k[1]);
    uchar *k1 = &seedSE_k[1 + LEN_BYTES_SEED_SE];

    frodo_pke_cipher c1; // = (B'', C')
    frodo_pke_enc(pke_pk, mu1, seedSE_k, &c1);

    // LEN_S = LEN_K
    uchar *buf = (uchar *)malloc(SIZE_CIPHER + LEN_S);
    memcpy(buf, c, SIZE_CIPHER);
    uchar *suffix = (frodo_pke_cipher_eq(pke_c, c1)) ? k1 : sk.s;
    memcpy(&buf[SIZE_CIPHER], suffix, LEN_S);
    shake128(buf, SIZE_CIPHER + LEN_S, LEN_SS, ss);

    free(buf);
}

/************/
/* IO UTILS */
/************/

FILE *checked_open(char *filename, char *mode) {
    FILE *fd = fopen(filename, mode);
    if (fd == NULL) {
        printf("Couldn't open file %s with mode %s.\n", filename, mode);
        exit(1);
    }
    return fd;
}

void checked_write(void *ptr, size_t size, size_t n_items, FILE *fd,
                   char *filename) {
    if (fwrite(ptr, size, n_items, fd) != n_items) {
        printf("Couldn't fully write buffer to %s.\n", filename);
        exit(1);
    }
}

void checked_read(void *ptr, size_t size, size_t n_items, FILE *fd,
                  char *filename) {
    if (fread(ptr, size, n_items, fd) != n_items) {
        printf("Couldn't fully read buffer from %s.\n", filename);
        exit(1);
    }
}

void load_pk_from_file(char *filename, frodo_kem_pk pk) {
    FILE *fd = checked_open(filename, "rb");
    checked_read(pk, 1, SIZE_KEM_PK, fd, filename);
    fclose(fd);
}

frodo_kem_sk load_sk_from_file(char *filename) {
    frodo_kem_sk sk = new_frodo_kem_sk();

    FILE *fd = checked_open(filename, "rb");
    checked_read(sk.s, 1, LEN_S, fd, filename);
    checked_read(sk.pk, 1, SIZE_KEM_PK, fd, filename);
    checked_read(sk.tS, sizeof(uint16_t), NBAR * N, fd, filename);
    checked_read(sk.pkh, 1, LEN_PKH, fd, filename);
    fclose(fd);

    return sk;
}

void load_cipher_from_file(char *filename, frodo_kem_cipher c) {
    FILE *fd = checked_open(filename, "rb");
    checked_read(c, 1, SIZE_CIPHER, fd, filename);
    fclose(fd);
}

void save_pk_to_file(char *filename, frodo_kem_pk pk) {
    FILE *fd = checked_open(filename, "wb");
    checked_write(pk, 1, SIZE_KEM_PK, fd, filename);
    fclose(fd);
}

void save_sk_to_file(char *filename, frodo_kem_sk sk) {
    FILE *fd = checked_open(filename, "wb");
    checked_write(sk.s, 1, LEN_S, fd, filename);
    checked_write(sk.pk, 1, SIZE_KEM_PK, fd, filename);
    checked_write(sk.tS, sizeof(uint16_t), NBAR * N, fd, filename);
    checked_write(sk.pkh, 1, LEN_PKH, fd, filename);
    fclose(fd);
}

void save_cipher_to_file(char *filename, frodo_kem_cipher c) {
    FILE *fd = checked_open(filename, "wb");
    checked_write(c, 1, SIZE_CIPHER, fd, filename);
    fclose(fd);
}

void save_shared_to_file(char *filename, uchar ss[LEN_SS]) {
    FILE *fd = checked_open(filename, "wb");
    checked_write(ss, 1, LEN_SS, fd, filename);
    fclose(fd);
}
