#include "frodo_kem.h"

#include <stdio.h>

int main(int argc, char **argv) {
    // Retrieving args
    if (argc < 4) {
        printf("[Usage] %s pk-file cipher-file shared-secret-file\n", argv[0]);
        return 1;
    }
    uchar *pk = (uchar *)malloc(SIZE_KEM_PK);
    load_pk_from_file(argv[1], pk);
    char *cipher_filename = argv[2];
    char *shared_filename = argv[3];

    // Encapsulating public key
    uchar *c = (uchar *)malloc(SIZE_CIPHER);
    uchar ss[LEN_SS];
    frodo_kem_encaps(pk, c, ss);

    // Storing cipher and shared secret
    save_cipher_to_file(cipher_filename, c);
    save_shared_to_file(shared_filename, ss);

    free(pk);
    free(c);
}
