#include "frodo_kem.h"

#include <stdio.h>

int main(int argc, char **argv) {
    // Retrieving args
    if (argc < 4) {
        printf("[Usage] %s sk-file cipher-file shared-secret-file\n", argv[0]);
        return 1;
    }
    frodo_kem_sk sk = load_sk_from_file(argv[1]);
    uchar *c = (uchar *)malloc(SIZE_CIPHER);
    load_cipher_from_file(argv[2], c);
    char *shared_filename = argv[3];

    // Decapsulating key
    uchar ss[LEN_SS];
    frodo_kem_decaps(sk, c, ss);

    save_shared_to_file(shared_filename, ss);

    free_frodo_kem_sk(&sk);
    free(c);
}
