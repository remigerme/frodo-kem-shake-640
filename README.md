# FrodoKEM-shake-640

> [!warning]
> FOR NOW, THE IMPLEMENTATION IS BUGGY AND DOESN'T WORK PROPERLY (shared secret differs from parties). The implemen is supposed to be complete (all components from paper below have been implemented - some of them in a buggy way apparently...).
> I didn't succeed to fix it after some very time consuming work. There is at least a problem in `frodo_pke.c` in `frodo_pke_dec`. When trying to `encode` a `decoded` value, the initial value is not retrieved. `pack` and `unpack` seems to work fine, and it doesn't seem there is a problem when `decoding` an `encoded` value.  
> It is possible to compile with flag `-DFRODO_DEBUG` in Makefile to see various logs.

Implemented according to [this paper](https://frodokem.org/files/FrodoKEM-specification-20200325.pdf) (**old version of the specification**).

## Install

```shell
make
```

Will build three targets : `frodokeygen`, `frodoencaps` and `frododecaps`.

Unfortunately for now, it doesn't seem to work (fails to establish same shared secret).

## Tests

todo : provide some tests using [these known values](https://github.com/microsoft/PQCrypto-LWEKE/blob/master/FrodoKEM/KAT/PQCkemKAT_19888_shake.rsp).

Code implemented for the "Advanced Cryptology" course at École polytechnique by Rémi Germe.
