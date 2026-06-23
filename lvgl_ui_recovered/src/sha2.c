/* SHA-256 and HMAC-SHA256 — public domain (based on RFC 6234 reference).
 * No external dependencies. */
#include "sha2.h"
#include <string.h>

#define ROR32(x, n)  (((x) >> (n)) | ((x) << (32-(n))))
#define CH(x,y,z)    (((x)&(y))^(~(x)&(z)))
#define MAJ(x,y,z)   (((x)&(y))^((x)&(z))^((y)&(z)))
#define EP0(x)       (ROR32(x,2)^ROR32(x,13)^ROR32(x,22))
#define EP1(x)       (ROR32(x,6)^ROR32(x,11)^ROR32(x,25))
#define SIG0(x)      (ROR32(x,7)^ROR32(x,18)^((x)>>3))
#define SIG1(x)      (ROR32(x,17)^ROR32(x,19)^((x)>>10))

static const uint32_t K[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static void sha256_compress(sha256_ctx_t *ctx, const uint8_t block[64]) {
    uint32_t w[64], a,b,c,d,e,f,g,h,t1,t2;
    int i;
    for (i = 0; i < 16; i++)
        w[i] = ((uint32_t)block[i*4]<<24)|((uint32_t)block[i*4+1]<<16)|
               ((uint32_t)block[i*4+2]<<8)|(uint32_t)block[i*4+3];
    for (; i < 64; i++)
        w[i] = SIG1(w[i-2]) + w[i-7] + SIG0(w[i-15]) + w[i-16];
    a=ctx->s[0]; b=ctx->s[1]; c=ctx->s[2]; d=ctx->s[3];
    e=ctx->s[4]; f=ctx->s[5]; g=ctx->s[6]; h=ctx->s[7];
    for (i = 0; i < 64; i++) {
        t1 = h + EP1(e) + CH(e,f,g) + K[i] + w[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    ctx->s[0]+=a; ctx->s[1]+=b; ctx->s[2]+=c; ctx->s[3]+=d;
    ctx->s[4]+=e; ctx->s[5]+=f; ctx->s[6]+=g; ctx->s[7]+=h;
}

void sha256_init(sha256_ctx_t *ctx) {
    ctx->len = 0; ctx->buflen = 0;
    ctx->s[0]=0x6a09e667; ctx->s[1]=0xbb67ae85;
    ctx->s[2]=0x3c6ef372; ctx->s[3]=0xa54ff53a;
    ctx->s[4]=0x510e527f; ctx->s[5]=0x9b05688c;
    ctx->s[6]=0x1f83d9ab; ctx->s[7]=0x5be0cd19;
}

void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len) {
    while (len > 0) {
        int space = 64 - ctx->buflen;
        int take  = (len < (size_t)space) ? (int)len : space;
        memcpy(ctx->buf + ctx->buflen, data, (size_t)take);
        ctx->buflen += take; data += take; len -= (size_t)take;
        ctx->len += (uint64_t)take;
        if (ctx->buflen == 64) { sha256_compress(ctx, ctx->buf); ctx->buflen = 0; }
    }
}

void sha256_final(sha256_ctx_t *ctx, uint8_t hash[32]) {
    uint64_t bits = ctx->len * 8;
    uint8_t pad = 0x80;
    sha256_update(ctx, &pad, 1);
    while (ctx->buflen != 56) { pad = 0; sha256_update(ctx, &pad, 1); }
    uint8_t bl[8];
    for (int i = 7; i >= 0; i--) { bl[i] = (uint8_t)(bits & 0xFF); bits >>= 8; }
    sha256_update(ctx, bl, 8);
    for (int i = 0; i < 8; i++) {
        hash[i*4+0] = (uint8_t)(ctx->s[i]>>24);
        hash[i*4+1] = (uint8_t)(ctx->s[i]>>16);
        hash[i*4+2] = (uint8_t)(ctx->s[i]>>8);
        hash[i*4+3] = (uint8_t)(ctx->s[i]);
    }
}

void sha256_once(const uint8_t *data, size_t len, uint8_t hash[32]) {
    sha256_ctx_t ctx; sha256_init(&ctx); sha256_update(&ctx, data, len); sha256_final(&ctx, hash);
}

void hmac_sha256(const uint8_t *key, size_t klen,
                 const uint8_t *msg, size_t mlen,
                 uint8_t mac[32]) {
    uint8_t k[64], ipad[64], opad[64], inner[32];
    int i;
    memset(k, 0, 64);
    if (klen > 64) sha256_once(key, klen, k);
    else            memcpy(k, key, klen);
    for (i = 0; i < 64; i++) { ipad[i] = k[i] ^ 0x36; opad[i] = k[i] ^ 0x5c; }
    sha256_ctx_t ctx;
    sha256_init(&ctx); sha256_update(&ctx, ipad, 64); sha256_update(&ctx, msg, mlen);
    sha256_final(&ctx, inner);
    sha256_init(&ctx); sha256_update(&ctx, opad, 64); sha256_update(&ctx, inner, 32);
    sha256_final(&ctx, mac);
}
