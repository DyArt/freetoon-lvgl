#ifndef SHA2_H
#define SHA2_H
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t len;
    uint32_t s[8];
    uint8_t  buf[64];
    int      buflen;
} sha256_ctx_t;

void sha256_init  (sha256_ctx_t *ctx);
void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len);
void sha256_final (sha256_ctx_t *ctx, uint8_t hash[32]);
void sha256_once  (const uint8_t *data, size_t len, uint8_t hash[32]);
void hmac_sha256  (const uint8_t *key, size_t klen,
                   const uint8_t *msg, size_t mlen,
                   uint8_t mac[32]);
#endif
