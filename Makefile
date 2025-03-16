CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -g -DFRODO_DEBUG
INCLUDES = -lm -Isrc -Isrc/sha3

TARGETS = frodokeygen frodoencaps frododecaps
SHA3 = src/sha3/sha3_utils.c src/sha3/keccak_rho.c src/sha3/sponge.c src/sha3/keccak_c.c src/sha3/sha3.c

all: $(TARGETS)

tests:
	./test.sh

frodokeygen: $(SHA3) src/frodo_utils.c src/mat.c src/frodo_pke.c src/frodo_kem.c src/keygen.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

frodoencaps: $(SHA3) src/frodo_utils.c src/mat.c src/frodo_pke.c src/frodo_kem.c src/encaps.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

frododecaps: $(SHA3) src/frodo_utils.c src/mat.c src/frodo_pke.c src/frodo_kem.c src/decaps.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

clean:
	rm -f $(TARGETS)
