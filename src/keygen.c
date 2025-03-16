#include "frodo_kem.h"

#include <stdio.h>

int main(int argc, char **argv) {
    // Retrieving args
    if (argc < 3) {
        printf("[Usage] %s pk-file sk-file\n", argv[0]);
        return 1;
    }
    char *pk_filename = argv[1];
    char *sk_filename = argv[2];

    // Generating keys
    uchar *pk = (uchar *)malloc(SIZE_KEM_PK);
    frodo_kem_sk sk = new_frodo_kem_sk();
    DEBUG_LOG("Calling KEM.Keygen\n");
    frodo_kem_keygen(pk, &sk);

    // Store the keys in file
    save_pk_to_file(pk_filename, pk);
    save_sk_to_file(sk_filename, sk);
}
