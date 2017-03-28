//
//  yx_core_hash_spookyHash.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/23.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_hash_spookyHash.h"


#define ALLOW_UNALIGNED_READS 1


// number of uint64's in internal state
static const yx_size sc_numVars = 12;

// size of the internal state
static const yx_size sc_blockSize = sc_numVars*8;

// size of buffer of unhashed data, in bytes
static const yx_size sc_bufSize = 2*sc_blockSize;

//
// sc_const: a constant which:
//  * is not zero
//  * is odd
//  * is a not-very-regular mix of 1's and 0's
//  * does not need any other special mathematical properties
//
static const yx_uint64 sc_const = 0xdeadbeefdeadbeefLL;





//
// left rotate a 64-bit value by k bytes
//
//yx_uint64 _spookyHash_rote64(yx_uint64 x, yx_int k)
#define _spookyHash_rote64(x, k) \
( ((x) << (k)) | ((x) >> (64 - (k))) )


//
// This is used if the input is 96 bytes long or longer.
//
// The internal state is fully overwritten every 96 bytes.
// Every input bit appears to cause at least 128 bits of entropy
// before 96 other bytes are combined, when run forward or backward
//   For every input bit,
//   Two inputs differing in just that input bit
//   Where "differ" means xor or subtraction
//   And the base value is random
//   When run forward or backwards one Mix
// I tried 3 pairs of each; they all differed by at least 212 bits.
//
/*
 static INLINE void _spookyHash_mix(
 const uint64 *data,
 uint64 &s0, uint64 &s1, uint64 &s2, uint64 &s3,
 uint64 &s4, uint64 &s5, uint64 &s6, uint64 &s7,
 uint64 &s8, uint64 &s9, uint64 &s10,uint64 &s11)
 */


#define _spookyHash_mix( \
data, \
s0, s1, s2, s3, \
s4, s5, s6, s7, \
s8, s9, s10,s11) \
do{ \
s0 += data[0];    s2 ^= s10;   s11 ^= s0;   s0 = _spookyHash_rote64(s0,11);    s11 += s1; \
s1 += data[1];    s3 ^= s11;   s0 ^= s1;    s1 = _spookyHash_rote64(s1,32);    s0 += s2; \
s2 += data[2];    s4 ^= s0;    s1 ^= s2;    s2 = _spookyHash_rote64(s2,43);    s1 += s3; \
s3 += data[3];    s5 ^= s1;    s2 ^= s3;    s3 = _spookyHash_rote64(s3,31);    s2 += s4; \
s4 += data[4];    s6 ^= s2;    s3 ^= s4;    s4 = _spookyHash_rote64(s4,17);    s3 += s5; \
s5 += data[5];    s7 ^= s3;    s4 ^= s5;    s5 = _spookyHash_rote64(s5,28);    s4 += s6; \
s6 += data[6];    s8 ^= s4;    s5 ^= s6;    s6 = _spookyHash_rote64(s6,39);    s5 += s7; \
s7 += data[7];    s9 ^= s5;    s6 ^= s7;    s7 = _spookyHash_rote64(s7,57);    s6 += s8; \
s8 += data[8];    s10 ^= s6;   s7 ^= s8;    s8 = _spookyHash_rote64(s8,55);    s7 += s9; \
s9 += data[9];    s11 ^= s7;   s8 ^= s9;    s9 = _spookyHash_rote64(s9,54);    s8 += s10; \
s10 += data[10];  s0 ^= s8;    s9 ^= s10;   s10 = _spookyHash_rote64(s10,22);  s9 += s11; \
s11 += data[11];  s1 ^= s9;    s10 ^= s11;  s11 = _spookyHash_rote64(s11,46);  s10 += s0; \
}while(0)


//
// Mix all 12 inputs together so that h0, h1 are a hash of them all.
//
// For two inputs differing in just the input bits
// Where "differ" means xor or subtraction
// And the base value is random, or a counting value starting at that bit
// The final result will have each bit of h0, h1 flip
// For every input bit,
// with probability 50 +- .3%
// For every pair of input bits,
// with probability 50 +- 3%
//
// This does not rely on the last Mix() call having already mixed some.
// Two iterations was almost good enough for a 64-bit result, but a
// 128-bit result is reported, so End() does three iterations.
//

/*
 static INLINE void EndPartial(
 uint64 &h0, uint64 &h1, uint64 &h2, uint64 &h3,
 uint64 &h4, uint64 &h5, uint64 &h6, uint64 &h7,
 uint64 &h8, uint64 &h9, uint64 &h10,uint64 &h11)
 
 */


#define _spookyHash_endPartial( \
h0, h1, h2, h3, \
h4, h5, h6, h7, \
h8, h9, h10,h11) \
do{ \
h11+= h1;    h2 ^= h11;   h1 = _spookyHash_rote64(h1,44); \
h0 += h2;    h3 ^= h0;    h2 = _spookyHash_rote64(h2,15); \
h1 += h3;    h4 ^= h1;    h3 = _spookyHash_rote64(h3,34); \
h2 += h4;    h5 ^= h2;    h4 = _spookyHash_rote64(h4,21); \
h3 += h5;    h6 ^= h3;    h5 = _spookyHash_rote64(h5,38); \
h4 += h6;    h7 ^= h4;    h6 = _spookyHash_rote64(h6,33); \
h5 += h7;    h8 ^= h5;    h7 = _spookyHash_rote64(h7,10); \
h6 += h8;    h9 ^= h6;    h8 = _spookyHash_rote64(h8,13); \
h7 += h9;    h10^= h7;    h9 = _spookyHash_rote64(h9,38); \
h8 += h10;   h11^= h8;    h10= _spookyHash_rote64(h10,53); \
h9 += h11;   h0 ^= h9;    h11= _spookyHash_rote64(h11,42); \
h10+= h0;    h1 ^= h10;   h0 = _spookyHash_rote64(h0,54); \
}while(0)



/*
 static INLINE void _spookyHash_end(
 const uint64 *data,
 uint64 &h0, uint64 &h1, uint64 &h2, uint64 &h3,
 uint64 &h4, uint64 &h5, uint64 &h6, uint64 &h7,
 uint64 &h8, uint64 &h9, uint64 &h10,uint64 &h11)
 */
#define _spookyHash_end( \
data, \
h0, h1, h2, h3, \
h4, h5, h6, h7, \
h8, h9, h10,h11) \
do{ \
h0 += data[0];   h1 += data[1];   h2 += data[2];   h3 += data[3]; \
h4 += data[4];   h5 += data[5];   h6 += data[6];   h7 += data[7]; \
h8 += data[8];   h9 += data[9];   h10 += data[10]; h11 += data[11]; \
_spookyHash_endPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11); \
_spookyHash_endPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11); \
_spookyHash_endPartial(h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11); \
}while(0)




//
// The goal is for each bit of the input to expand into 128 bits of
//   apparent entropy before it is fully overwritten.
// n trials both set and cleared at least m bits of h0 h1 h2 h3
//   n: 2   m: 29
//   n: 3   m: 46
//   n: 4   m: 57
//   n: 5   m: 107
//   n: 6   m: 146
//   n: 7   m: 152
// when run forwards or backwards
// for all 1-bit and 2-bit diffs
// with diffs defined by either xor or subtraction
// with a base of all zeros plus a counter, or plus another bit, or random
//
//#define ShortMix(uint64 &h0, uint64 &h1, uint64 &h2, uint64 &h3)
#define _spookyHash_shortMix(h0, h1, h2, h3) \
do{ \
h2 = _spookyHash_rote64(h2,50);  h2 += h3;  h0 ^= h2; \
h3 = _spookyHash_rote64(h3,52);  h3 += h0;  h1 ^= h3; \
h0 = _spookyHash_rote64(h0,30);  h0 += h1;  h2 ^= h0; \
h1 = _spookyHash_rote64(h1,41);  h1 += h2;  h3 ^= h1; \
h2 = _spookyHash_rote64(h2,54);  h2 += h3;  h0 ^= h2; \
h3 = _spookyHash_rote64(h3,48);  h3 += h0;  h1 ^= h3; \
h0 = _spookyHash_rote64(h0,38);  h0 += h1;  h2 ^= h0; \
h1 = _spookyHash_rote64(h1,37);  h1 += h2;  h3 ^= h1; \
h2 = _spookyHash_rote64(h2,62);  h2 += h3;  h0 ^= h2; \
h3 = _spookyHash_rote64(h3,34);  h3 += h0;  h1 ^= h3; \
h0 = _spookyHash_rote64(h0,5);   h0 += h1;  h2 ^= h0; \
h1 = _spookyHash_rote64(h1,36);  h1 += h2;  h3 ^= h1; \
}while(0)



//
// Mix all 4 inputs together so that h0, h1 are a hash of them all.
//
// For two inputs differing in just the input bits
// Where "differ" means xor or subtraction
// And the base value is random, or a counting value starting at that bit
// The final result will have each bit of h0, h1 flip
// For every input bit,
// with probability 50 +- .3% (it is probably better than that)
// For every pair of input bits,
// with probability 50 +- .75% (the worst case is approximately that)
//
//static INLINE void ShortEnd(uint64 &h0, uint64 &h1, uint64 &h2, uint64 &h3)
#define _spookyHash_shortEnd(h0, h1, h2, h3) \
do{ \
h3 ^= h2;  h2 = _spookyHash_rote64(h2,15);  h3 += h2; \
h0 ^= h3;  h3 = _spookyHash_rote64(h3,52);  h0 += h3; \
h1 ^= h0;  h0 = _spookyHash_rote64(h0,26);  h1 += h0; \
h2 ^= h1;  h1 = _spookyHash_rote64(h1,51);  h2 += h1; \
h3 ^= h2;  h2 = _spookyHash_rote64(h2,28);  h3 += h2; \
h0 ^= h3;  h3 = _spookyHash_rote64(h3,9);   h0 += h3; \
h1 ^= h0;  h0 = _spookyHash_rote64(h0,47);  h1 += h0; \
h2 ^= h1;  h1 = _spookyHash_rote64(h1,54);  h2 += h1; \
h3 ^= h2;  h2 = _spookyHash_rote64(h2,32);  h3 += h2; \
h0 ^= h3;  h3 = _spookyHash_rote64(h3,25);  h0 += h3; \
h1 ^= h0;  h0 = _spookyHash_rote64(h0,63);  h1 += h0; \
}while(0)


//
// Short is used for messages under 192 bytes in length
// Short has a low startup cost, the normal mode is good for long
// keys, the cost crossover is at about 192 bytes.  The two modes were
// held to the same quality bar.
//
static void _spookyHash_short(
                          const void *message,  // message (array of bytes, not necessarily aligned)
                          yx_size length,        // length of message (in bytes)
                          yx_uint64 *hash1,        // in/out: in the seed, out the hash value
                          yx_uint64 *hash2)       // in/out: in the seed, out the hash value
{
    yx_uint64 buf[2*sc_numVars];
    union
    {
        const yx_uint8 *p8;
        yx_uint32 *p32;
        yx_uint64 *p64;
        yx_size i;
    } u;
    
    u.p8 = (const yx_uint8 *)message;
    
    if (!ALLOW_UNALIGNED_READS && (u.i & 0x7))
    {
        memcpy(buf, message, length);
        u.p64 = buf;
    }
    
    yx_size remainder = length%32;
    yx_uint64 a=*hash1;
    yx_uint64 b=*hash2;
    yx_uint64 c=sc_const;
    yx_uint64 d=sc_const;
    
    if (length > 15)
    {
        const yx_uint64 *end = u.p64 + (length/32)*4;
        
        // handle all complete sets of 32 bytes
        for (; u.p64 < end; u.p64 += 4)
        {
            c += u.p64[0];
            d += u.p64[1];
            _spookyHash_shortMix(a,b,c,d);
            a += u.p64[2];
            b += u.p64[3];
        }
        
        //Handle the case of 16+ remaining bytes.
        if (remainder >= 16)
        {
            c += u.p64[0];
            d += u.p64[1];
            _spookyHash_shortMix(a,b,c,d);
            u.p64 += 2;
            remainder -= 16;
        }
    }
    
    // Handle the last 0..15 bytes, and its length
    d += ((yx_uint64)length) << 56;
    switch (remainder)
    {
        case 15:
            d += ((yx_uint64)u.p8[14]) << 48;
        case 14:
            d += ((yx_uint64)u.p8[13]) << 40;
        case 13:
            d += ((yx_uint64)u.p8[12]) << 32;
        case 12:
            d += u.p32[2];
            c += u.p64[0];
            break;
        case 11:
            d += ((yx_uint64)u.p8[10]) << 16;
        case 10:
            d += ((yx_uint64)u.p8[9]) << 8;
        case 9:
            d += (yx_uint64)u.p8[8];
        case 8:
            c += u.p64[0];
            break;
        case 7:
            c += ((yx_uint64)u.p8[6]) << 48;
        case 6:
            c += ((yx_uint64)u.p8[5]) << 40;
        case 5:
            c += ((yx_uint64)u.p8[4]) << 32;
        case 4:
            c += u.p32[0];
            break;
        case 3:
            c += ((yx_uint64)u.p8[2]) << 16;
        case 2:
            c += ((yx_uint64)u.p8[1]) << 8;
        case 1:
            c += (yx_uint64)u.p8[0];
            break;
        case 0:
            c += sc_const;
            d += sc_const;
    }
    _spookyHash_shortEnd(a,b,c,d);
    *hash1 = a;
    *hash2 = b;

}





// do the whole hash in one call
void yx_core_spookyhash128(
                         const void *message,
                         yx_size length,
                         yx_uint64 *hash1,
                         yx_uint64 *hash2)
{
    if (length < sc_bufSize)
    {
        _spookyHash_short(message, length, hash1, hash2);
        return;
    }
    
    yx_uint64 h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11;
    yx_uint64 buf[sc_numVars];
    yx_uint64 *end;
    union
    {
        const yx_uint8 *p8;
        yx_uint64 *p64;
        yx_size i;
    } u;
    yx_size remainder;
    
    h0=h3=h6=h9  = *hash1;
    h1=h4=h7=h10 = *hash2;
    h2=h5=h8=h11 = sc_const;
    
    u.p8 = (const yx_uint8 *)message;
    end = u.p64 + (length/sc_blockSize)*sc_numVars;
    
    // handle all whole sc_blockSize blocks of bytes
    if (ALLOW_UNALIGNED_READS || ((u.i & 0x7) == 0))
    {
        while (u.p64 < end)
        {
            _spookyHash_mix(u.p64, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
            u.p64 += sc_numVars;
        }
    }
    else
    {
        while (u.p64 < end)
        {
            memcpy(buf, u.p64, sc_blockSize);
            _spookyHash_mix(buf, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
            u.p64 += sc_numVars;
        }
    }
    
    // handle the last partial block of sc_blockSize bytes
    remainder = (length - (yx_size)((const yx_uint8 *)end-(const yx_uint8 *)message));
    memcpy(buf, end, remainder);
    memset(((yx_uint8 *)buf)+remainder, 0, sc_blockSize-remainder);
    ((yx_uint8 *)buf)[sc_blockSize-1] = (yx_uint8)remainder;
    
    // do some final mixing
    _spookyHash_end(buf, h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11);
    *hash1 = h0;
    *hash2 = h1;
}



yx_uint64 yx_core_spookyhash64(
                             const void *message,  // message to hash
                             yx_size length,        // length of message in bytes
                             yx_uint64 seed)          // seed
{
    yx_uint64 hash1 = seed;
    yx_core_spookyhash128(message, length, &hash1, &seed);
    return hash1;
}


//
// Hash32: hash a single message in one call, produce 32-bit output
//
yx_uint32 yx_core_spookyhash32(
                         const void *message,  // message to hash
                         yx_size length,        // length of message in bytes
                         yx_uint32 seed)          // seed
{
    yx_uint64 hash1 = seed, hash2 = seed;
    yx_core_spookyhash128(message, length, &hash1, &hash2);
    return (yx_uint32)hash1;
}



















