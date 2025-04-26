// MKALGAM_ON
// RAW_ON
// Copyright https://github.com/DOSAYGO-Research/rain
// Released under the Apache-2.0 license
// RAW_OFF

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define RAINBOW1_ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static const bool BSWAP = false;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static const bool BSWAP = true;
#else
    #error "Endianness not supported!"
#endif

//------------------------------------------------------------------------------
static inline uint64_t GET_U64(bool bswap, const uint8_t* data, size_t index) {
    uint64_t result;
    memcpy(&result, data + index, sizeof(result));
    if(bswap) {
        result = ((result & 0x00000000000000FF) << 56) |
                 ((result & 0x000000000000FF00) << 40) |
                 ((result & 0x0000000000FF0000) << 24) |
                 ((result & 0x00000000FF000000) << 8) |
                 ((result & 0x000000FF00000000) >> 8) |
                 ((result & 0x0000FF0000000000) >> 24) |
                 ((result & 0x00FF000000000000) >> 40) |
                 ((result & 0xFF00000000000000) >> 56);
    }

    return result;
}

//------------------------------------------------------------------------------
static inline void PUT_U64(bool bswap, uint64_t value, uint8_t* data, size_t index) {
    if(bswap) {
        value = ((value & 0x00000000000000FF) << 56) |
                ((value & 0x000000000000FF00) << 40) |
                ((value & 0x0000000000FF0000) << 24) |
                ((value & 0x00000000FF000000) << 8) |
                ((value & 0x000000FF00000000) >> 8) |
                ((value & 0x0000FF0000000000) >> 24) |
                ((value & 0x00FF000000000000) >> 40) |
                ((value & 0xFF00000000000000) >> 56);
    }

    memcpy(data + index, &value, sizeof(value));
}

static const uint64_t P = UINT64_C(0xFFFFFFFFFFFFFFFF) - 58;
static const uint64_t Q = UINT64_C(13166748625691186689);
static const uint64_t R = UINT64_C(1573836600196043749);
static const uint64_t S = UINT64_C(1478582680485693857);
static const uint64_t T = UINT64_C(1584163446043636637);
static const uint64_t U = UINT64_C(1358537349836140151);
static const uint64_t V = UINT64_C(2849285319520710901);
static const uint64_t W = UINT64_C(2366157163652459183);

//------------------------------------------------------------------------------
static inline void mixA(uint64_t* s) {
    uint64_t a = s[0], b = s[1], c = s[2], d = s[3];

    a *= P;
    a = RAINBOW1_ROTR64(a, 23);
    a *= Q;

    b ^= a;

    b *= R;
    b = RAINBOW1_ROTR64(b, 29);
    b *= S;

    c *= T;
    c = RAINBOW1_ROTR64(c, 31);
    c *= U;

    d ^= c;

    d *= V;
    d = RAINBOW1_ROTR64(d, 37);
    d *= W;

    s[0] = a; s[1] = b; s[2] = c; s[3] = d;
}

//------------------------------------------------------------------------------
static inline void mixB(uint64_t* s, uint64_t iv) {
    uint64_t a = s[1], b = s[2];

    a *= V;
    a = RAINBOW1_ROTR64(a, 23);
    a *= W;

    b ^= a + iv;

    b *= R;
    b = RAINBOW1_ROTR64(b, 23);
    b *= S;

    s[1] = a; s[2] = b;
}

//------------------------------------------------------------------------------
uint32_t rainbow(unsigned hashsize, const void* in, const size_t in_len, uint8_t out[32], const uint64_t seed) {
    memset(out, 0, 32);

    uint64_t h[4] = {
        seed + in_len + 1,
        seed + in_len + 3,
        seed + in_len + 5,
        seed + in_len + 7
    };
    
    const uint8_t* data  = (const uint8_t*) in;
    size_t         len   = in_len;
    uint64_t       g     = 0;
    bool           inner = false;

    while (len >= 16) {
        g = GET_U64(BSWAP, data, 0);

        h[0] -= g;
        h[1] += g;
        data += 8;

        g = GET_U64(BSWAP, data, 0);

        h[2] += g;
        h[3] -= g;

        if (inner) {
            mixB(h, seed);
        }
        else {
            mixA(h);
        }

        inner  = !inner;
        data  += 8;
        len   -= 16;
    }

    mixB(h, seed);

    switch (len) {
        case 15: h[0] += (uint64_t)(data[14]) << 56; [[fallthrough]];
        case 14: h[1] += (uint64_t)(data[13]) << 48; [[fallthrough]];
        case 13: h[2] += (uint64_t)(data[12]) << 40; [[fallthrough]];
        case 12: h[3] += (uint64_t)(data[11]) << 32; [[fallthrough]];
        case 11: h[0] += (uint64_t)(data[10]) << 24; [[fallthrough]];
        case 10: h[1] += (uint64_t)(data[9])  << 16; [[fallthrough]];
        case  9: h[2] += (uint64_t)(data[8])  <<  8; [[fallthrough]];
        case  8: h[3] += data[7]; [[fallthrough]];
        case  7: h[0] += (uint64_t)(data[6])  << 48; [[fallthrough]];
        case  6: h[1] += (uint64_t)(data[5])  << 40; [[fallthrough]];
        case  5: h[2] += (uint64_t)(data[4])  << 32; [[fallthrough]];
        case  4: h[3] += (uint64_t)(data[3])  << 24; [[fallthrough]];
        case  3: h[0] += (uint64_t)(data[2])  << 16; [[fallthrough]];
        case  2: h[1] += (uint64_t)(data[1])  <<  8; [[fallthrough]];
        case  1: h[2] += (uint64_t)(data[0]);
    }

    mixA(h);
    mixB(h, seed);
    mixA(h);

    g  = 0;
    g -= h[2];
    g -= h[3];

    PUT_U64(BSWAP, g, (uint8_t*) out, 0);

    if (hashsize == 128) {
        mixA(h);

        g  = 0;
        g -= h[3];
        g -= h[2];

        PUT_U64(BSWAP, g, (uint8_t*) out, 8);
    }
    else if (hashsize == 256) {
        mixA(h);

        g  = 0;
        g -= h[3];
        g -= h[2];

        PUT_U64(BSWAP, g, (uint8_t*) out, 8);
        mixA(h);
        mixB(h, seed);
        mixA(h);

        g  = 0;
        g -= h[3];
        g -= h[2];

        PUT_U64(BSWAP, g, (uint8_t*) out, 16);
        mixA(h);

        g  = 0;
        g -= h[3];
        g -= h[2];

        PUT_U64(BSWAP, g, (uint8_t*) out, 24);
    }
    
    uint32_t res;
    memcpy(&res, out, 4);
    return res;
}

// MKALGAM_OFF
