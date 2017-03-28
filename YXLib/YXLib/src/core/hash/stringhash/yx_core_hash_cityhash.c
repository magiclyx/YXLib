
#include "yx_core_hash_cityhash.h"


static yx_uint64 UNALIGNED_LOAD64(const yx_char *p) {
    yx_uint64 result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static yx_uint32 UNALIGNED_LOAD32(const yx_char *p) {
    yx_uint32 result;
    memcpy(&result, p, sizeof(result));
    return result;
}


// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)


#ifdef YX_ENV_WORDS_BIGENDIAN
#define yx_uint32_in_expected_order(x) (bswap_32(x))
#define yx_uint64_in_expected_order(x) (bswap_64(x))
#else
#define yx_uint32_in_expected_order(x) (x)
#define yx_uint64_in_expected_order(x) (x)
#endif


static yx_uint64 Fetch64(const yx_char *p) {
    return yx_uint64_in_expected_order(UNALIGNED_LOAD64(p));
}

static yx_uint32 Fetch32(const yx_char *p) {
    return yx_uint32_in_expected_order(UNALIGNED_LOAD32(p));
}



// Some primes between 2^63 and 2^64 for various uses.
static const yx_uint64 k0 = 0xc3a5c85c97cb3127ULL;
static const yx_uint64 k1 = 0xb492b66fbe98f273ULL;
static const yx_uint64 k2 = 0x9ae16a3b2f90404fULL;

// Magic numbers for 32-bit hashing.  Copied from Murmur3.
static const yx_uint32 c1 = 0xcc9e2d51;
static const yx_uint32 c2 = 0x1b873593;

// A 32-bit to 32-bit integer hash copied from Murmur3.
static yx_uint32 fmix(yx_uint32 h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

static yx_uint32 Rotate32(yx_uint32 val, yx_int shift) {
    // Avoid shifting by 32: doing so yields an undefined result.
    return shift == 0 ? val : ((val >> shift) | (val << (32 - shift)));
}

#undef PERMUTE3
#define PERMUTE3(a, b, c) do { YX_SWAP(a, b); YX_SWAP(a, c); } while (0)

static yx_uint32 Mur(yx_uint32 a, yx_uint32 h) {
    // Helper from Murmur3 for combining two 32-bit values.
    a *= c1;
    a = Rotate32(a, 17);
    a *= c2;
    h ^= a;
    h = Rotate32(h, 19);
    return h * 5 + 0xe6546b64;
}

static yx_uint32 Hash32Len13to24(const yx_char *s, yx_size len) {
    yx_uint32 a = Fetch32(s - 4 + (len >> 1));
    yx_uint32 b = Fetch32(s + 4);
    yx_uint32 c = Fetch32(s + len - 8);
    yx_uint32 d = Fetch32(s + (len >> 1));
    yx_uint32 e = Fetch32(s);
    yx_uint32 f = Fetch32(s + len - 4);
    yx_uint32 h = (yx_uint32)len;
    
    return fmix(Mur(f, Mur(e, Mur(d, Mur(c, Mur(b, Mur(a, h)))))));
}

static yx_uint32 Hash32Len0to4(const yx_char *s, yx_size len) {
    yx_uint32 b = 0;
    yx_uint32 c = 9;
    for (yx_size i = 0; i < len; i++) {
        yx_char v = s[i];
        b = b * c1 + (yx_uint32)v;
        c ^= b;
    }
    return fmix(Mur(b, Mur((yx_uint32)len, c)));
}

static yx_uint32 Hash32Len5to12(const yx_char *s, yx_size len) {
    yx_uint32 a = (yx_uint32)len, b = (yx_uint32)len * 5, c = 9, d = b;
    a += Fetch32(s);
    b += Fetch32(s + len - 4);
    c += Fetch32(s + ((len >> 1) & 4));
    return fmix(Mur(c, Mur(b, Mur(a, d))));
}


// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
static yx_uint64 Hash128to64(uint128* x) {
    // Murmur-inspired hashing.
    const yx_uint64 kMul = 0x9ddfea08eb382d69ULL;
    yx_uint64 a = (yx_core_hash_cityhash128Low64(x) ^ yx_core_hash_cityhash128High64(x)) * kMul;
    a ^= (a >> 47);
    yx_uint64 b = (yx_core_hash_cityhash128High64(x) ^ a) * kMul;
    b ^= (b >> 47);
    b *= kMul;
    return b;
}



yx_uint32 yx_core_cityhash32(const yx_char *s, yx_size len) {
    if (len <= 24) {
        return len <= 12 ?
        (len <= 4 ? Hash32Len0to4(s, len) : Hash32Len5to12(s, len)) :
        Hash32Len13to24(s, len);
    }
    
    // len > 24
    yx_uint32 h = (yx_uint32)len, g = c1 * (yx_uint32)len, f = g;
    yx_uint32 a0 = Rotate32(Fetch32(s + len - 4) * c1, 17) * c2;
    yx_uint32 a1 = Rotate32(Fetch32(s + len - 8) * c1, 17) * c2;
    yx_uint32 a2 = Rotate32(Fetch32(s + len - 16) * c1, 17) * c2;
    yx_uint32 a3 = Rotate32(Fetch32(s + len - 12) * c1, 17) * c2;
    yx_uint32 a4 = Rotate32(Fetch32(s + len - 20) * c1, 17) * c2;
    h ^= a0;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    h ^= a2;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    g ^= a1;
    g = Rotate32(g, 19);
    g = g * 5 + 0xe6546b64;
    g ^= a3;
    g = Rotate32(g, 19);
    g = g * 5 + 0xe6546b64;
    f += a4;
    f = Rotate32(f, 19);
    f = f * 5 + 0xe6546b64;
    yx_size iters = (len - 1) / 20;
    do {
        a0 = Rotate32(Fetch32(s) * c1, 17) * c2;
        a1 = Fetch32(s + 4);
        a2 = Rotate32(Fetch32(s + 8) * c1, 17) * c2;
        a3 = Rotate32(Fetch32(s + 12) * c1, 17) * c2;
        a4 = Fetch32(s + 16);
        h ^= a0;
        h = Rotate32(h, 18);
        h = h * 5 + 0xe6546b64;
        f += a1;
        f = Rotate32(f, 19);
        f = f * c1;
        g += a2;
        g = Rotate32(g, 18);
        g = g * 5 + 0xe6546b64;
        h ^= a3 + a1;
        h = Rotate32(h, 19);
        h = h * 5 + 0xe6546b64;
        g ^= a4;
        g = bswap_32(g) * 5;
        h += a4 * 5;
        h = bswap_32(h);
        f += a0;
        PERMUTE3(f, h, g);
        s += 20;
    } while (--iters != 0);
    g = Rotate32(g, 11) * c1;
    g = Rotate32(g, 17) * c1;
    f = Rotate32(f, 11) * c1;
    f = Rotate32(f, 17) * c1;
    h = Rotate32(h + g, 19);
    h = h * 5 + 0xe6546b64;
    h = Rotate32(h, 17) * c1;
    h = Rotate32(h + f, 19);
    h = h * 5 + 0xe6546b64;
    h = Rotate32(h, 17) * c1;
    return h;
}

// Bitwise right rotate.  Normally this will compile to a single
// instruction, especially if the shift is a manifest constant.
static yx_uint64 Rotate(yx_uint64 val, yx_int shift) {
    // Avoid shifting by 64: doing so yields an undefined result.
    return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

static yx_uint64 ShiftMix(yx_uint64 val) {
    return val ^ (val >> 47);
}

static yx_uint64 HashLen16(yx_uint64 u, yx_uint64 v) {
    uint128 x;
    x.first = u;
    x.second = v;
    return Hash128to64(&x);
}

static yx_uint64 HashLen16WithMul(yx_uint64 u, yx_uint64 v, yx_uint64 mul) {
    // Murmur-inspired hashing.
    yx_uint64 a = (u ^ v) * mul;
    a ^= (a >> 47);
    yx_uint64 b = (v ^ a) * mul;
    b ^= (b >> 47);
    b *= mul;
    return b;
}

static yx_uint64 HashLen0to16(const yx_char *s, yx_size len) {
    if (len >= 8) {
        yx_uint64 mul = k2 + len * 2;
        yx_uint64 a = Fetch64(s) + k2;
        yx_uint64 b = Fetch64(s + len - 8);
        yx_uint64 c = Rotate(b, 37) * mul + a;
        yx_uint64 d = (Rotate(a, 25) + b) * mul;
        return HashLen16WithMul(c, d, mul);
    }
    if (len >= 4) {
        yx_uint64 mul = k2 + len * 2;
        yx_uint64 a = Fetch32(s);
        return HashLen16WithMul(len + (a << 3), Fetch32(s + len - 4), mul);
    }
    if (len > 0) {
        yx_uint8 a = (yx_uint8)s[0];
        yx_uint8 b = (yx_uint8)s[len >> 1];
        yx_uint8 c = (yx_uint8)s[len - 1];
        yx_uint32 y = (yx_uint32)(a) + ((yx_uint32)(b) << 8);
        yx_uint32 z = (yx_uint32)len + ((yx_uint32)(c) << 2);
        return ShiftMix(y * k2 ^ z * k0) * k2;
    }
    return k2;
}

// This probably works well for 16-byte strings as well, but it may be overkill
// in that case.
static yx_uint64 HashLen17to32(const yx_char *s, yx_size len) {
    yx_uint64 mul = k2 + len * 2;
    yx_uint64 a = Fetch64(s) * k1;
    yx_uint64 b = Fetch64(s + 8);
    yx_uint64 c = Fetch64(s + len - 8) * mul;
    yx_uint64 d = Fetch64(s + len - 16) * k2;
    return HashLen16WithMul(Rotate(a + b, 43) + Rotate(c, 30) + d,
                            a + Rotate(b + k2, 18) + c, mul);
}

// Return a 16-byte hash for 48 bytes.  Quick and dirty.
// Callers do best to use "random-looking" values for a and b.
static void WeakHashLen32WithSeeds(
                                   uint128* result, yx_uint64 w, yx_uint64 x, yx_uint64 y, yx_uint64 z, yx_uint64 a, yx_uint64 b) {
    a += w;
    b = Rotate(b + a + z, 21);
    yx_uint64 c = a;
    a += x;
    a += y;
    b += Rotate(a, 44);
    
    result->first = a + z;
    result->second = b + c;
}

// Return a 16-byte hash for s[0] ... s[31], a, and b.  Quick and dirty.
static void WeakHashLen32StringWithSeeds(
                                         uint128* result, const yx_char* s, yx_uint64 a, yx_uint64 b) {
    return WeakHashLen32WithSeeds(
                                  result,
                                  Fetch64(s),
                                  Fetch64(s + 8),
                                  Fetch64(s + 16),
                                  Fetch64(s + 24),
                                  a,
                                  b);
}

// Return an 8-byte hash for 33 to 64 bytes.
static yx_uint64 HashLen33to64(const yx_char *s, yx_size len) {
    yx_uint64 mul = k2 + len * 2;
    yx_uint64 a = Fetch64(s) * k2;
    yx_uint64 b = Fetch64(s + 8);
    yx_uint64 c = Fetch64(s + len - 24);
    yx_uint64 d = Fetch64(s + len - 32);
    yx_uint64 e = Fetch64(s + 16) * k2;
    yx_uint64 f = Fetch64(s + 24) * 9;
    yx_uint64 g = Fetch64(s + len - 8);
    yx_uint64 h = Fetch64(s + len - 16) * mul;
    yx_uint64 u = Rotate(a + g, 43) + (Rotate(b, 30) + c) * 9;
    yx_uint64 v = ((a + g) ^ d) + f + 1;
    yx_uint64 w = bswap_64((u + v) * mul) + h;
    yx_uint64 x = Rotate(e + f, 42) + c;
    yx_uint64 y = (bswap_64((v + w) * mul) + g) * mul;
    yx_uint64 z = e + f + c;
    a = bswap_64((x + z) * mul + y) + b;
    b = ShiftMix((z + a) * mul + d + h) * mul;
    return b + x;
}

yx_uint64 yx_core_cityhash64(const yx_char *s, yx_size len) {
    if (len <= 32) {
        if (len <= 16) {
            return HashLen0to16(s, len);
        } else {
            return HashLen17to32(s, len);
        }
    } else if (len <= 64) {
        return HashLen33to64(s, len);
    }
    
    // For strings over 64 bytes we hash the end first, and then as we
    // loop we keep 56 bytes of state: v, w, x, y, and z.
    yx_uint64 x = Fetch64(s + len - 40);
    yx_uint64 y = Fetch64(s + len - 16) + Fetch64(s + len - 56);
    yx_uint64 z = HashLen16(Fetch64(s + len - 48) + len, Fetch64(s + len - 24));
    uint128 v;
    uint128 w;
    WeakHashLen32StringWithSeeds(&v, s + len - 64, len, z);
    WeakHashLen32StringWithSeeds(&w, s + len - 32, y + k1, x);
    x = x * k1 + Fetch64(s);
    
    // Decrease len to the nearest multiple of 64, and operate on 64-byte chunks.
    len = (len - 1) & ~( (yx_size)(63) );
    do {
        x = Rotate(x + y + v.first + Fetch64(s + 8), 37) * k1;
        y = Rotate(y + v.second + Fetch64(s + 48), 42) * k1;
        x ^= w.second;
        y += v.first + Fetch64(s + 40);
        z = Rotate(z + w.first, 33) * k1;
        WeakHashLen32StringWithSeeds(&v, s, v.second * k1, x + w.first);
        WeakHashLen32StringWithSeeds(&w, s + 32, z + w.second, y + Fetch64(s + 16));
        YX_SWAP(z, x);
        s += 64;
        len -= 64;
    } while (len != 0);
    return HashLen16(HashLen16(v.first, w.first) + ShiftMix(y) * k1 + z,
                     HashLen16(v.second, w.second) + x);
}

yx_uint64 yx_core_cityhash64WithSeed(const yx_char *s, yx_size len, yx_uint64 seed) {
    return yx_core_cityhash64WithSeeds(s, len, k2, seed);
}

yx_uint64 yx_core_cityhash64WithSeeds(const yx_char *s, yx_size len,
                                      yx_uint64 seed0, yx_uint64 seed1) {
    return HashLen16(yx_core_cityhash64(s, len) - seed0, seed1);
}

// A subroutine for yx_core_cityhash128().  Returns a decent 128-bit hash for strings
// of any length representable in signed long.  Based on City and Murmur.
static void CityMurmur(uint128* result, const yx_char *s, yx_size len, uint128* seed) {
    yx_uint64 a = yx_core_hash_cityhash128Low64(seed);
    yx_uint64 b = yx_core_hash_cityhash128High64(seed);
    yx_uint64 c = 0;
    yx_uint64 d = 0;
    signed long l = (signed long)len - 16;
    if (l <= 0) {  // len <= 16
        a = ShiftMix(a * k1) * k1;
        c = b * k1 + HashLen0to16(s, len);
        d = ShiftMix(a + (len >= 8 ? Fetch64(s) : c));
    } else {  // len > 16
        c = HashLen16(Fetch64(s + len - 8) + k1, a);
        d = HashLen16(b + len, c + Fetch64(s + len - 16));
        a += d;
        do {
            a ^= ShiftMix(Fetch64(s) * k1) * k1;
            a *= k1;
            b ^= a;
            c ^= ShiftMix(Fetch64(s + 8) * k1) * k1;
            c *= k1;
            d ^= c;
            s += 16;
            l -= 16;
        } while (l > 0);
    }
    a = HashLen16(a, c);
    b = HashLen16(d, b);
    
    result->first = a ^ b;
    result->second = HashLen16(b, a);
    //  return uint128(a ^ b, HashLen16(b, a));
}

void yx_core_cityhash128WithSeed(uint128* result, const yx_char *s, yx_size len, uint128* seed) {
    if (len < 128) {
        CityMurmur(result, s, len, seed);
        return;
    }
    
    // We expect len >= 128 to be the common case.  Keep 56 bytes of state:
    // v, w, x, y, and z.
    uint128 v, w;
    yx_uint64 x = yx_core_hash_cityhash128Low64(seed);
    yx_uint64 y = yx_core_hash_cityhash128High64(seed);
    yx_uint64 z = len * k1;
    v.first = Rotate(y ^ k1, 49) * k1 + Fetch64(s);
    v.second = Rotate(v.first, 42) * k1 + Fetch64(s + 8);
    w.first = Rotate(y + z, 35) * k1 + x;
    w.second = Rotate(x + Fetch64(s + 88), 53) * k1;
    
    // This is the same inner loop as yx_core_cityhash64(), manually unrolled.
    do {
        x = Rotate(x + y + v.first + Fetch64(s + 8), 37) * k1;
        y = Rotate(y + v.second + Fetch64(s + 48), 42) * k1;
        x ^= w.second;
        y += v.first + Fetch64(s + 40);
        z = Rotate(z + w.first, 33) * k1;
        WeakHashLen32StringWithSeeds(&v, s, v.second * k1, x + w.first);
        WeakHashLen32StringWithSeeds(&w, s + 32, z + w.second, y + Fetch64(s + 16));
        YX_SWAP(z, x);
        s += 64;
        x = Rotate(x + y + v.first + Fetch64(s + 8), 37) * k1;
        y = Rotate(y + v.second + Fetch64(s + 48), 42) * k1;
        x ^= w.second;
        y += v.first + Fetch64(s + 40);
        z = Rotate(z + w.first, 33) * k1;
        WeakHashLen32StringWithSeeds(&v, s, v.second * k1, x + w.first);
        WeakHashLen32StringWithSeeds(&w, s + 32, z + w.second, y + Fetch64(s + 16));
        YX_SWAP(z, x);
        s += 64;
        len -= 128;
    } while (yx_likely(len >= 128));
    x += Rotate(v.first + z, 49) * k0;
    y = y * k0 + Rotate(w.second, 37);
    z = z * k0 + Rotate(w.first, 27);
    w.first *= 9;
    v.first *= k0;
    // If 0 < len < 128, hash up to 4 chunks of 32 bytes each from the end of s.
    for (yx_size tail_done = 0; tail_done < len; ) {
        tail_done += 32;
        y = Rotate(x + y, 42) * k0 + v.second;
        w.first += Fetch64(s + len - tail_done + 16);
        x = x * k0 + w.first;
        z += w.second + Fetch64(s + len - tail_done);
        w.second += v.first;
        WeakHashLen32StringWithSeeds(&v, s + len - tail_done, v.first + z, v.second);
        v.first *= k0;
    }
    // At this point our 56 bytes of state should contain more than
    // enough information for a strong 128-bit hash.  We use two
    // different 56-byte-to-8-byte hashes to get a 16-byte final result.
    x = HashLen16(x, v.first);
    y = HashLen16(y + z, w.first);
    
    result->first = HashLen16(x + v.second, w.second) + y;
    result->second =  HashLen16(x + w.second, y + v.second);
    //  return uint128(HashLen16(x + v.second, w.second) + y,
    //                 HashLen16(x + w.second, y + v.second));
}

void yx_core_cityhash128(uint128* result, const yx_char *s, yx_size len) {
    
    if (len >= 16)
    {
        uint128 sx;
        sx.first = Fetch64(s);
        sx.second = Fetch64(s + 8) + k0;
        yx_core_cityhash128WithSeed(result, s + 16, len - 16, &sx);
    }
    else
    {
        uint128 sx;
        sx.first = k0;
        sx.second = k1;
        
        yx_core_cityhash128WithSeed(result, s, len, &sx);
    }
    //  return len >= 16 ?
    //      yx_core_cityhash128WithSeed(s + 16, len - 16,
    //                          uint128(Fetch64(s), Fetch64(s + 8) + k0)) :
    //      yx_core_cityhash128WithSeed(s, len, uint128(k0, k1));
}



#ifdef YX_OS_CPUCRC_SUPPORT

// Requires len >= 240.
static void CityHashCrc256Long(const yx_char *s, yx_size len,
                               yx_uint32 seed, yx_uint64 *result) {
    yx_uint64 a = Fetch64(s + 56) + k0;
    yx_uint64 b = Fetch64(s + 96) + k0;
    yx_uint64 c = result[0] = HashLen16(b, len);
    yx_uint64 d = result[1] = Fetch64(s + 120) * k0 + len;
    yx_uint64 e = Fetch64(s + 184) + seed;
    yx_uint64 f = 0;
    yx_uint64 g = 0;
    yx_uint64 h = c + d;
    yx_uint64 x = seed;
    yx_uint64 y = 0;
    yx_uint64 z = 0;
    
    // 240 bytes of input per iter.
    yx_size iters = len / 240;
    len -= iters * 240;
    do {
#undef CHUNK
#define CHUNK(r)                                \
PERMUTE3(x, z, y);                          \
b += Fetch64(s);                            \
c += Fetch64(s + 8);                        \
d += Fetch64(s + 16);                       \
e += Fetch64(s + 24);                       \
f += Fetch64(s + 32);                       \
a += b;                                     \
h += f;                                     \
b += c;                                     \
f += d;                                     \
g += e;                                     \
e += z;                                     \
g += x;                                     \
z = _mm_crc32_u64(z, b + g);                \
y = _mm_crc32_u64(y, e + h);                \
x = _mm_crc32_u64(x, f + a);                \
e = Rotate(e, (yx_int)r);                   \
c += e;                                     \
s += 40
        
        CHUNK(0); PERMUTE3(a, h, c);
        CHUNK(33); PERMUTE3(a, h, f);
        CHUNK(0); PERMUTE3(b, h, f);
        CHUNK(42); PERMUTE3(b, h, d);
        CHUNK(0); PERMUTE3(b, h, e);
        CHUNK(33); PERMUTE3(a, h, e);
    } while (--iters > 0);
    
    while (len >= 40) {
        CHUNK(29);
        e ^= Rotate(a, 20);
        h += Rotate(b, 30);
        g ^= Rotate(c, 40);
        f += Rotate(d, 34);
        PERMUTE3(c, h, g);
        len -= 40;
    }
    if (len > 0) {
        s = s + len - 40;
        CHUNK(33);
        e ^= Rotate(a, 43);
        h += Rotate(b, 42);
        g ^= Rotate(c, 41);
        f += Rotate(d, 40);
    }
    result[0] ^= h;
    result[1] ^= g;
    g += h;
    a = HashLen16(a, g + z);
    x += y << 32;
    b += x;
    c = HashLen16(c, z) + h;
    d = HashLen16(d, e + result[0]);
    g += e;
    h += HashLen16(x, f);
    e = HashLen16(a, d) + g;
    z = HashLen16(b, c) + a;
    y = HashLen16(g, h) + c;
    result[0] = e + z + y + x;
    a = ShiftMix((a + y) * k0) * k0 + b;
    result[1] += a + result[0];
    a = ShiftMix(a * k0) * k0 + c;
    result[2] = a + result[1];
    a = ShiftMix((a + e) * k0) * k0;
    result[3] = a + result[2];
}

// Requires len < 240.
static void CityHashCrc256Short(const yx_char *s, yx_size len, yx_uint64 *result) {
    char buf[240];
    memcpy(buf, s, len);
    memset(buf + len, 0, 240 - len);
    CityHashCrc256Long(buf, 240, ~((yx_uint32)(len)), result);
}

void yx_core_hash256_crc(const yx_char* s, yx_size len, yx_uint64* result)
{
    if (yx_likely(len >= 240))
    {
        CityHashCrc256Long(s, len, 0, result);
    }
    else
    {
        CityHashCrc256Short(s, len, result);
    }
}

void yx_core_cityhash128WithSeed_crc(uint128* result, const yx_char* s, size_t len, uint128* seed)
{
    if (len <= 900) {
        return yx_core_cityhash128WithSeed(result, s, len, seed);
    } else {
        yx_uint64 resultArr[4];
        yx_core_hash256_crc(s, len, resultArr);
        yx_uint64 u = yx_core_hash_cityhash128High64(seed) + resultArr[0];
        yx_uint64 v = yx_core_hash_cityhash128Low64(seed) + resultArr[1];
        
        result->first = HashLen16(u, v + resultArr[2]);
        result->second = HashLen16(Rotate(v, 32), u * k0 + resultArr[3]);
        
//        return uint128(HashLen16(u, v + resultArr[2]),
//                       HashLen16(Rotate(v, 32), u * k0 + resultArr[3]));
    }
}

void yx_core_cityhash128_crc(uint128* result, const yx_char* s, yx_size len)
{
//uint128 CityHashCrc128(const yx_char *s, yx_size len) {
    if (len <= 900)
    {
        yx_core_cityhash128(result, s, len);
    }
    else
    {
        yx_uint64 resultArr[4];
        yx_core_hash256_crc(s, len, resultArr);
        
        result->first = resultArr[2];
        result->second = resultArr[3];
        
//        return uint128(resultArr[2], resultArr[3]);
    }
}

#endif




