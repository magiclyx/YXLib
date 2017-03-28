//
//  hashmain.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/20.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#include "yx_core_headers.h"
#include "unittest/yx_unittest_headers.h"
#include "unittest/yx_unittest_unittest.h"

#include "testcase/core/yx_testcase_core_headers.h"

//#define YX_SWAP_UINT64(a, b) \
//do{ \
//yx_uint64 c = a; \
//a = b; \
//b = c; \
//}while(0)
//
//#define YX_SWAP_UINT32(a, b) \
//do{ \
//yx_uint32 c = a; \
//a = b; \
//b = c; \
//}while(0)
//
//
//// Mac OS X / Darwin features
//#include <libkern/OSByteOrder.h>
//#define bswap_32(x) OSSwapInt32(x)
//#define bswap_64(x) OSSwapInt64(x)
//
//#define PERMUTE3(a, b, c) do { YX_SWAP_UINT32(a, b); YX_SWAP_UINT32(a, c); } while (0)
//
//
//
//typedef struct uint128
//{
//    yx_uint64 first;
//    yx_uint64 second;
//}uint128;
//
//#define uint128(a, b) {a, b}
//
//inline yx_uint64 Uint128Low64(uint128* x) { return x->first; }
//inline yx_uint64 Uint128High64(uint128* x) { return x->second; }
//
//
//static yx_uint64 UNALIGNED_LOAD64(const char *p) {
//    yx_uint64 result;
//    memcpy(&result, p, sizeof(result));
//    return result;
//}
//
//
//static yx_uint32 UNALIGNED_LOAD32(const char *p) {
//    yx_uint32 result;
//    memcpy(&result, p, sizeof(result));
//    return result;
//}
//
//
//
//
//#ifdef WORDS_BIGENDIAN
//#define uint32_in_expected_order(x) (bswap_32(x))
//#define uint64_in_expected_order(x) (bswap_64(x))
//#else
//#define uint32_in_expected_order(x) (x)
//#define uint64_in_expected_order(x) (x)
//#endif
//
//
//static yx_uint64 Fetch64(const char *p) {
//    return uint64_in_expected_order(UNALIGNED_LOAD64(p));
//}
//
//static yx_uint64 Fetch32(const char *p) {
//    return uint32_in_expected_order(UNALIGNED_LOAD32(p));
//}
//
//// Some primes between 2^63 and 2^64 for various uses.
//static const yx_uint64 k0 = 0xc3a5c85c97cb3127ULL;
//static const yx_uint64 k1 = 0xb492b66fbe98f273ULL;
//static const yx_uint64 k2 = 0x9ae16a3b2f90404fULL;
//
//// Magic numbers for 32-bit hashing.  Copied from Murmur3.
//static const yx_uint32 c1 = 0xcc9e2d51;
//static const yx_uint32 c2 = 0x1b873593;
//
//// A 32-bit to 32-bit integer hash copied from Murmur3.
//static yx_uint32 fmix(yx_uint32 h)
//{
//    h ^= h >> 16;
//    h *= 0x85ebca6b;
//    h ^= h >> 13;
//    h *= 0xc2b2ae35;
//    h ^= h >> 16;
//    return h;
//}
//
//static yx_uint32 Rotate32(yx_uint32 val, int shift) {
//    // Avoid shifting by 32: doing so yields an undefined result.
//    return shift == 0 ? val : ((val >> shift) | (val << (32 - shift)));
//}
//
//// Bitwise right rotate.  Normally this will compile to a single
//// instruction, especially if the shift is a manifest constant.
//static yx_uint64 Rotate(yx_uint64 val, int shift) {
//    // Avoid shifting by 64: doing so yields an undefined result.
//    return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
//}
//
//static yx_uint64 ShiftMix(yx_uint64 val) {
//    return val ^ (val >> 47);
//}
//
//
//
//// Hash 128 input bits down to 64 bits of output.
//// This is intended to be a reasonably good hash function.
//inline yx_uint64 Hash128to64(uint128* x) {
//    // Murmur-inspired hashing.
//    const yx_uint64 kMul = 0x9ddfea08eb382d69ULL;
//    yx_uint64 a = (Uint128Low64(x) ^ Uint128High64(x)) * kMul;
//    a ^= (a >> 47);
//    yx_uint64 b = (Uint128High64(x) ^ a) * kMul;
//    b ^= (b >> 47);
//    b *= kMul;
//    return b;
//}
//
//static yx_uint32 Mur(yx_uint32 a, yx_uint32 h) {
//    // Helper from Murmur3 for combining two 32-bit values.
//    a *= c1;
//    a = Rotate32(a, 17);
//    a *= c2;
//    h ^= a;
//    h = Rotate32(h, 19);
//    return h * 5 + 0xe6546b64;
//}
//
//static yx_uint32 Hash32Len0to4(const char *s, size_t len) {
//    yx_uint32 b = 0;
//    yx_uint32 c = 9;
//    for (size_t i = 0; i < len; i++) {
//        signed char v = s[i];
//        b = b * c1 + (yx_uint8)v;
//        c ^= b;
//    }
//    return fmix(Mur(b, Mur((yx_uint32)len, c)));
//}
//
//
//static yx_uint32 Hash32Len5to12(const char *s, size_t len) {
//    yx_uint32 a = (yx_uint32)len, b = (yx_uint32)len * 5, c = 9, d = b;
//    a += Fetch32(s);
//    b += Fetch32(s + len - 4);
//    c += Fetch32(s + ((len >> 1) & 4));
//    return fmix(Mur(c, Mur(b, Mur(a, d))));
//}
//
//static yx_uint32 Hash32Len13to24(const char *s, size_t len) {
//    yx_uint32 a = (yx_uint32)Fetch32(s - 4 + (len >> 1));
//    yx_uint32 b = (yx_uint32)Fetch32(s + 4);
//    yx_uint32 c = (yx_uint32)Fetch32(s + len - 8);
//    yx_uint32 d = (yx_uint32)Fetch32(s + (len >> 1));
//    yx_uint32 e = (yx_uint32)Fetch32(s);
//    yx_uint32 f = (yx_uint32)Fetch32(s + len - 4);
//    yx_uint32 h = (yx_uint32)len;
//    
//    return fmix(Mur(f, Mur(e, Mur(d, Mur(c, Mur(b, Mur(a, h)))))));
//}
//
//
//static yx_uint64 HashLen16(yx_uint64 u, yx_uint64 v) {
//    uint128 x;
//    x.first = u;
//    x.second = v;
//    return Hash128to64(&x);
//}
//
//static yx_uint64 HashLen16_3(yx_uint64 u, yx_uint64 v, yx_uint64 mul) {
//    // Murmur-inspired hashing.
//    yx_uint64 a = (u ^ v) * mul;
//    a ^= (a >> 47);
//    yx_uint64 b = (v ^ a) * mul;
//    b ^= (b >> 47);
//    b *= mul;
//    return b;
//}
//
//// Return a 16-byte hash for 48 bytes.  Quick and dirty.
//// Callers do best to use "random-looking" values for a and b.
//static void WeakHashLen32WithSeeds_7(uint128* result,  yx_uint64 w, yx_uint64 x, yx_uint64 y, yx_uint64 z, yx_uint64 a, yx_uint64 b) {
//    a += w;
//    b = Rotate(b + a + z, 21);
//    yx_uint64 c = a;
//    a += x;
//    a += y;
//    b += Rotate(a, 44);
//    
//    result->first = a+z;
//    result->second = b+c;
//}
//// Return a 16-byte hash for s[0] ... s[31], a, and b.  Quick and dirty.
//static void WeakHashLen32WithSeeds(uint128* result, const char* s, yx_uint64 a, yx_uint64 b) {
//    WeakHashLen32WithSeeds_7(result,
//                           Fetch64(s),
//                           Fetch64(s + 8),
//                           Fetch64(s + 16),
//                           Fetch64(s + 24),
//                           a,
//                           b);
//}
//
//static yx_uint64 HashLen0to16(const char *s, size_t len) {
//    if (len >= 8) {
//        yx_uint64 mul = k2 + len * 2;
//        yx_uint64 a = Fetch64(s) + k2;
//        yx_uint64 b = Fetch64(s + len - 8);
//        yx_uint64 c = Rotate(b, 37) * mul + a;
//        yx_uint64 d = (Rotate(a, 25) + b) * mul;
//        return HashLen16_3(c, d, mul);
//    }
//    if (len >= 4) {
//        yx_uint64 mul = k2 + len * 2;
//        yx_uint64 a = Fetch32(s);
//        return HashLen16_3(len + (a << 3), Fetch32(s + len - 4), mul);
//    }
//    if (len > 0) {
//        yx_uint8 a = (yx_uint8)s[0];
//        yx_uint8 b = (yx_uint8)s[len >> 1];
//        yx_uint8 c = (yx_uint8)s[len - 1];
//        yx_uint32 y = (yx_uint32)a + ( ((yx_uint32)b) << 8) ;
//        yx_uint32 z = (yx_uint32)len + ( ((yx_uint32)c ) << 2 );
//        return ShiftMix(y * k2 ^ z * k0) * k2;
//    }
//    return k2;
//}
//
//// This probably works well for 16-byte strings as well, but it may be overkill
//// in that case.
//static yx_uint64 HashLen17to32(const char *s, size_t len) {
//    yx_uint64 mul = k2 + len * 2;
//    yx_uint64 a = Fetch64(s) * k1;
//    yx_uint64 b = Fetch64(s + 8);
//    yx_uint64 c = Fetch64(s + len - 8) * mul;
//    yx_uint64 d = Fetch64(s + len - 16) * k2;
//    return HashLen16_3(Rotate(a + b, 43) + Rotate(c, 30) + d,
//                     a + Rotate(b + k2, 18) + c, mul);
//}
//
//// Return an 8-byte hash for 33 to 64 bytes.
//static yx_uint64 HashLen33to64(const char *s, size_t len) {
//    yx_uint64 mul = k2 + len * 2;
//    yx_uint64 a = Fetch64(s) * k2;
//    yx_uint64 b = Fetch64(s + 8);
//    yx_uint64 c = Fetch64(s + len - 24);
//    yx_uint64 d = Fetch64(s + len - 32);
//    yx_uint64 e = Fetch64(s + 16) * k2;
//    yx_uint64 f = Fetch64(s + 24) * 9;
//    yx_uint64 g = Fetch64(s + len - 8);
//    yx_uint64 h = Fetch64(s + len - 16) * mul;
//    yx_uint64 u = Rotate(a + g, 43) + (Rotate(b, 30) + c) * 9;
//    yx_uint64 v = ((a + g) ^ d) + f + 1;
//    yx_uint64 w = bswap_64((u + v) * mul) + h;
//    yx_uint64 x = Rotate(e + f, 42) + c;
//    yx_uint64 y = (bswap_64((v + w) * mul) + g) * mul;
//    yx_uint64 z = e + f + c;
//    a = bswap_64((x + z) * mul + y) + b;
//    b = ShiftMix((z + a) * mul + d + h) * mul;
//    return b + x;
//}
//
//
//yx_uint64 CityHash64(const char *s, size_t len)
//{
//    if (len <= 32) {
//        if (len <= 16) {
//            return HashLen0to16(s, len);
//        } else {
//            return HashLen17to32(s, len);
//        }
//    } else if (len <= 64) {
//        return HashLen33to64(s, len);
//    }
//
//    // For strings over 64 bytes we hash the end first, and then as we
//    // loop we keep 56 bytes of state: v, w, x, y, and z.
//    yx_uint64 x = Fetch64(s + len - 40);
//    yx_uint64 y = Fetch64(s + len - 16) + Fetch64(s + len - 56);
//    yx_uint64 z = HashLen16(Fetch64(s + len - 48) + len, Fetch64(s + len - 24));
//    
//    
//    uint128 v, w;
//    WeakHashLen32WithSeeds(&v, s + len - 64, len, z);
//    WeakHashLen32WithSeeds(&w, s + len - 32, y + k1, x);
//    x = x * k1 + Fetch64(s);
//    
//    // Decrease len to the nearest multiple of 64, and operate on 64-byte chunks.
//    len = (len - 1) & ~((size_t)63);
//    do {
//        x = Rotate(x + y + v.first + Fetch64(s + 8), 37) * k1;
//        y = Rotate(y + v.second + Fetch64(s + 48), 42) * k1;
//        x ^= w.second;
//        y += v.first + Fetch64(s + 40);
//        z = Rotate(z + w.first, 33) * k1;
//        WeakHashLen32WithSeeds(&v, s, v.second * k1, x + w.first);
//        WeakHashLen32WithSeeds(&w, s + 32, z + w.second, y + Fetch64(s + 16));
//        YX_SWAP_UINT64(z, x);
//        s += 64;
//        len -= 64;
//    } while (len != 0);
//    return HashLen16(HashLen16(v.first, w.first) + ShiftMix(y) * k1 + z,
//                     HashLen16(v.second, w.second) + x);
//
//}
//
//
//
//yx_uint64 CityHash64WithSeeds(const char *s, size_t len,
//                           yx_uint64 seed0, yx_uint64 seed1) {
//    return HashLen16(CityHash64(s, len) - seed0, seed1);
//}
//
//
//yx_uint64 CityHash64WithSeed(const char *s, size_t len, yx_uint64 seed) {
//    return CityHash64WithSeeds(s, len, k2, seed);
//}
//
//
//
//yx_uint32 CityHash32(const char *s, size_t len) {
//    if (len <= 24) {
//        return len <= 12 ?
//        (len <= 4 ? Hash32Len0to4(s, len) : Hash32Len5to12(s, len)) :
//        Hash32Len13to24(s, len);
//    }
//    
//    // len > 24
//    yx_uint32 h = (yx_uint32)len, g = c1 * (yx_uint32)len, f = g;
//    yx_uint32 a0 = Rotate32((yx_uint32)Fetch32(s + (yx_uint32)len - 4) * c1, 17) * c2;
//    yx_uint32 a1 = Rotate32((yx_uint32)Fetch32(s + (yx_uint32)len - 8) * c1, 17) * c2;
//    yx_uint32 a2 = Rotate32((yx_uint32)Fetch32(s + (yx_uint32)len - 16) * c1, 17) * c2;
//    yx_uint32 a3 = Rotate32((yx_uint32)Fetch32(s + (yx_uint32)len - 12) * c1, 17) * c2;
//    yx_uint32 a4 = Rotate32((yx_uint32)Fetch32(s + (yx_uint32)len - 20) * c1, 17) * c2;
//    h ^= a0;
//    h = Rotate32(h, 19);
//    h = h * 5 + 0xe6546b64;
//    h ^= a2;
//    h = Rotate32(h, 19);
//    h = h * 5 + 0xe6546b64;
//    g ^= a1;
//    g = Rotate32(g, 19);
//    g = g * 5 + 0xe6546b64;
//    g ^= a3;
//    g = Rotate32(g, 19);
//    g = g * 5 + 0xe6546b64;
//    f += a4;
//    f = Rotate32(f, 19);
//    f = f * 5 + 0xe6546b64;
//    size_t iters = (len - 1) / 20;
//    do {
//        a0 = Rotate32((yx_uint32)Fetch32(s) * c1, 17) * c2;
//        a1 = (yx_uint32)Fetch32(s + 4);
//        a2 = (yx_uint32)Rotate32((yx_uint32)Fetch32(s + 8) * c1, 17) * c2;
//        a3 = (yx_uint32)Rotate32((yx_uint32)Fetch32(s + 12) * c1, 17) * c2;
//        a4 = (yx_uint32)Fetch32(s + 16);
//        h ^= a0;
//        h = Rotate32(h, 18);
//        h = h * 5 + 0xe6546b64;
//        f += a1;
//        f = Rotate32(f, 19);
//        f = f * c1;
//        g += a2;
//        g = Rotate32(g, 18);
//        g = g * 5 + 0xe6546b64;
//        h ^= a3 + a1;
//        h = Rotate32(h, 19);
//        h = h * 5 + 0xe6546b64;
//        g ^= a4;
//        g = bswap_32(g) * 5;
//        h += a4 * 5;
//        h = bswap_32(h);
//        f += a0;
//        PERMUTE3(f, h, g);
//        s += 20;
//    } while (--iters != 0);
//    g = Rotate32(g, 11) * c1;
//    g = Rotate32(g, 17) * c1;
//    f = Rotate32(f, 11) * c1;
//    f = Rotate32(f, 17) * c1;
//    h = Rotate32(h + g, 19);
//    h = h * 5 + 0xe6546b64;
//    h = Rotate32(h, 17) * c1;
//    h = Rotate32(h + f, 19);
//    h = h * 5 + 0xe6546b64;
//    h = Rotate32(h, 17) * c1;
//    return h;
//}







//int main(int argc, const char * argv[])
//{
//    
//    yx_unused(argc);
//    yx_unused(argv);
//    
//    
//    
//    return 0;
//}