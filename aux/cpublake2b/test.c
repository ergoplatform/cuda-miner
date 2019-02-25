#include <stdio.h>
#include <inttypes.h>

#include "blake2b.h"

// Deterministic sequences (Fibonacci generator).
static void selftest_seq(uint8_t *out, size_t len, uint32_t seed)
{
    size_t i;
    uint32_t t, a , b;

    a = 0xDEAD4BAD * seed;              // prime
    b = 1;

    for (i = 0; i < len; i++) {         // fill the buf
        t = a + b;
        a = b;
        b = t;
        out[i] = (t >> 24) & 0xFF;
    }
}

// BLAKE2b self-test validation. Return 0 when OK.
int blake2b_selftest()
{
    // grand hash of hash results
    const uint8_t blake2b_res[32] = {
        0xC2, 0x3A, 0x78, 0x00, 0xD9, 0x81, 0x23, 0xBD,
        0x10, 0xF5, 0x06, 0xC6, 0x1E, 0x29, 0xDA, 0x56,
        0x03, 0xD7, 0x63, 0xB8, 0xBB, 0xAD, 0x2E, 0x73,
        0x7F, 0x5E, 0x76, 0x5A, 0x7B, 0xCC, 0xD4, 0x75
    };

    // parameter sets
    const size_t b2b_md_len[4] = { 20, 32, 48, 64 };
    const size_t b2b_in_len[6] = { 0, 3, 128, 129, 255, 1024 };

    size_t i, j, outlen, inlen;
    uint8_t in[1024], md[64], key[64];
    blake2b_ctx ctx;

    // 256-bit hash for testing
    if (blake2b_init(&ctx, 32, NULL, 0))
    {
        return -1;
    }

    for (i = 0; i < 4; i++)
    {
        outlen = b2b_md_len[i];

        for (j = 0; j < 6; j++)
        {
            inlen = b2b_in_len[j];

            selftest_seq(in, inlen, inlen);     // unkeyed hash
            blake2b(md, outlen, NULL, 0, in, inlen);
            blake2b_update(&ctx, md, outlen);   // hash the hash

            selftest_seq(key, outlen, outlen);  // keyed hash
            blake2b(md, outlen, key, outlen, in, inlen);
            blake2b_update(&ctx, md, outlen);   // hash the hash
        }
    }

    // compute and compare the hash of hashes
    blake2b_final(&ctx, md);

    for (i = 0; i < 32; i++)
    {
        if (md[i] != blake2b_res[i])
        {
            return -1;
        }
    }

    return 0;
}

// Test driver.
int main(int argc, char **argv)
{
    // for original hash
    // printf("blake2b_selftest() = %s\n", blake2b_selftest() ? "FAIL" : "OK");

    /// blake2b_ctx ctx;
    /// uint8_t in[3] = { 0xFF, 0xFF, 0xFF };
    uint8_t in[3] = { 'a', 'b', 'c' };
    uint32_t out[8];
    blake2b(out, 32, NULL, 0, in, 3);
    // blake2b(out, 32, NULL, 0, NULL, 0);

    printf(
        "blake2b-256 = 0x%016lX %016lX %016lX %016lX\n",
        ((uint64_t *)out)[3],
        ((uint64_t *)out)[2],
        ((uint64_t *)out)[1],
        ((uint64_t *)out)[0]
    );

    uint32_t a[1] = { 0x100 };

    printf("%"PRIx8"\n", ((uint8_t *)a)[0]);

    fflush(stdout);

    return 0;
}
