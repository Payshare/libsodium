
#define TEST_NAME "box_easy2"
#include "cmptest.h"

unsigned char m[10000];
unsigned char m2[10000];
unsigned char c[crypto_box_MACBYTES + 10000];

int main(void)
{
    unsigned char *alicepk;
    unsigned char *alicesk;
    unsigned char *bobpk;
    unsigned char *bobsk;
    unsigned char *mac;
    unsigned char *nonce;
    size_t         mlen;
    size_t         i;

    alicepk = (unsigned char *) sodium_malloc(crypto_box_PUBLICKEYBYTES);
    alicesk = (unsigned char *) sodium_malloc(crypto_box_SECRETKEYBYTES);
    bobpk = (unsigned char *) sodium_malloc(crypto_box_PUBLICKEYBYTES);
    bobsk = (unsigned char *) sodium_malloc(crypto_box_SECRETKEYBYTES);
    mac = (unsigned char *) sodium_malloc(crypto_box_MACBYTES);
    nonce = (unsigned char *) sodium_malloc(crypto_box_NONCEBYTES);
    crypto_box_keypair(alicepk, alicesk);
    crypto_box_keypair(bobpk, bobsk);
    mlen = (size_t) randombytes_uniform((uint32_t)sizeof m);
    randombytes_buf(m, mlen);
    randombytes_buf(nonce, sizeof nonce);
    crypto_box_easy(c, m, mlen, nonce, bobpk, alicesk);
    if (crypto_box_open_easy(m2, c,
                             (unsigned long long) mlen + crypto_box_MACBYTES,
                             nonce, alicepk, bobsk) != 0) {
        printf("open() failed");
        return 1;
    }
    printf("%d\n", memcmp(m, m2, mlen));

    for (i = 0; i < mlen + crypto_box_MACBYTES - 1; i++) {
        if (crypto_box_open_easy(m2, c, (unsigned long long) i,
                                 nonce, alicepk, bobsk) == 0) {
            printf("short open() should have failed");
            return 1;
        }
    }

    memcpy(c, m, mlen);
    crypto_box_easy(c, c, (unsigned long long) mlen, nonce, bobpk, alicesk);
    printf("%d\n", memcmp(m, c, mlen) == 0);
    printf("%d\n", memcmp(m, c + crypto_box_MACBYTES, mlen) == 0);
    if (crypto_box_open_easy(c, c,
                             (unsigned long long) mlen + crypto_box_MACBYTES,
                             nonce, alicepk, bobsk) != 0) {
        printf("crypto_box_open_easy() failed\n");
    }

    crypto_box_detached(c, mac, m, (unsigned long long) mlen,
                        nonce, bobsk, alicepk);
    crypto_box_open_detached(m2, c, mac, (unsigned long long) mlen,
                             nonce, alicepk, bobsk);
    printf("%d\n", memcmp(m, m2, mlen));
    sodium_free(alicepk);
    sodium_free(alicesk);
    sodium_free(bobpk);
    sodium_free(bobsk);
    sodium_free(mac);
    sodium_free(nonce);
    printf("OK\n");

    return 0;
}
