//
//  yx_core_hash_murmurHash.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/23.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_hash_murmurHash.h"


static yx_uint32 rotl32 ( yx_uint32 x, yx_int8 r )
{
    return (x << r) | (x >> (32 - r));
}

static yx_uint64 rotl64 ( yx_uint64 x, yx_int8 r )
{
    return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)


//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

yx_forceinline
yx_uint32 getblock32 ( const yx_uint32* p, yx_int i )
{
    return p[i];
}

yx_forceinline
yx_uint64 getblock64 ( const yx_uint64* p, yx_int i )
{
    return p[i];
}



//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

yx_forceinline
yx_uint32 fmix32 ( yx_uint32 h )
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    
    return h;
}

//----------

yx_forceinline
yx_uint64 fmix64 ( yx_uint64 k )
{
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;
    
    return k;
}


yx_uint32 yx_core_murmurhash32WithSeed ( const void * key, yx_size len, yx_uint32 seed)
{
    const yx_uint8 * data = (const yx_uint8*)key;
    const int nblocks = (yx_int)len / 4;
    
    yx_uint32 h1 = seed;
    
    const yx_uint32 c1 = 0xcc9e2d51;
    const yx_uint32 c2 = 0x1b873593;
    
    //----------
    // body
    
    const yx_uint32 * blocks = (const yx_uint32 *)(data + nblocks*4);
    
    for(int i = -nblocks; i; i++)
    {
        yx_uint32 k1 = getblock32(blocks,i);
        
        k1 *= c1;
        k1 = ROTL32(k1,15);
        k1 *= c2;
        
        h1 ^= k1;
        h1 = ROTL32(h1,13);
        h1 = h1*5+0xe6546b64;
    }
    
    //----------
    // tail
    
    const yx_uint8 * tail = (const yx_uint8*)(data + nblocks*4);
    
    yx_uint32 k1 = 0;
    
    switch(len & 3)
    {
        case 3: k1 ^= (yx_uint32)(tail[2] << 16);
        case 2: k1 ^= (yx_uint32)(tail[1] << 8);
        case 1: k1 ^= tail[0];
            k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };
    
    //----------
    // finalization
    
    h1 ^= len;
    
    h1 = fmix32(h1);
    
    return h1;
} 





//-----------------------------------------------------------------------------
// MurmurHash2, 64-bit versions, by Austin Appleby

// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment
// and endian-ness issues if used across multiple platforms.



#if YX_DATAMODE_64

// 64-bit hash for 64-bit platforms

typedef yx_uint64 yx_uint64;

// 64-bit hash for 64-bit platforms

yx_uint64 yx_core_murmurhash64WithSeed ( const void * key, yx_size len, yx_uint seed )
{
    const yx_uint64 m = 0xc6a4a7935bd1e995;
    const int r = 47;
    
    yx_uint64 h = seed ^ (len * m);
    
    const yx_uint64 * data = (const yx_uint64 *)key;
    const yx_uint64 * end = data + (len/8);
    
    while(data != end)
    {
        yx_uint64 k = *data++;
        
        k *= m;
        k ^= k >> r;
        k *= m;
        
        h ^= k;
        h *= m;
    }
    
    const yx_uchar * data2 = (const yx_uchar*)data;
    
    switch(len & 7)
    {
        case 7: h ^= ((yx_uint64)(data2[6])) << 48;
        case 6: h ^= ((yx_uint64)(data2[5])) << 40;
        case 5: h ^= ((yx_uint64)(data2[4])) << 32;
        case 4: h ^= ((yx_uint64)(data2[3])) << 24;
        case 3: h ^= ((yx_uint64)(data2[2])) << 16;
        case 2: h ^= ((yx_uint64)(data2[1])) << 8;
        case 1: h ^= ((yx_uint64)(data2[0]));
            h *= m;
    };
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
}


void yx_core_murmurhash128WithSeed ( const void * key, const yx_size len, const yx_uint32 seed, void * out )
{
    const yx_uint8 * data = (const yx_uint8*)key;
    const yx_int nblocks = (yx_int)len / 16;
    
    yx_uint64 h1 = seed;
    yx_uint64 h2 = seed;
    
    const yx_uint64 c1 = BIG_CONSTANT(0x87c37b91114253d5);
    const yx_uint64 c2 = BIG_CONSTANT(0x4cf5ad432745937f);
    
    //----------
    // body
    
    const yx_uint64 * blocks = (const yx_uint64 *)(data);
    
    for(yx_int i = 0; i < nblocks; i++)
    {
        yx_uint64 k1 = getblock64(blocks,i*2+0);
        yx_uint64 k2 = getblock64(blocks,i*2+1);
        
        k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
        
        h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;
        
        k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;
        
        h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
    }
    
    //----------
    // tail
    
    const yx_uint8 * tail = (const yx_uint8*)(data + nblocks*16);
    
    yx_uint64 k1 = 0;
    yx_uint64 k2 = 0;
    
    switch(len & 15)
    {
        case 15: k2 ^= ((yx_uint64)tail[14]) << 48;
        case 14: k2 ^= ((yx_uint64)tail[13]) << 40;
        case 13: k2 ^= ((yx_uint64)tail[12]) << 32;
        case 12: k2 ^= ((yx_uint64)tail[11]) << 24;
        case 11: k2 ^= ((yx_uint64)tail[10]) << 16;
        case 10: k2 ^= ((yx_uint64)tail[ 9]) << 8;
        case  9: k2 ^= ((yx_uint64)tail[ 8]) << 0;
            k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;
            
        case  8: k1 ^= ((yx_uint64)tail[ 7]) << 56;
        case  7: k1 ^= ((yx_uint64)tail[ 6]) << 48;
        case  6: k1 ^= ((yx_uint64)tail[ 5]) << 40;
        case  5: k1 ^= ((yx_uint64)tail[ 4]) << 32;
        case  4: k1 ^= ((yx_uint64)tail[ 3]) << 24;
        case  3: k1 ^= ((yx_uint64)tail[ 2]) << 16;
        case  2: k1 ^= ((yx_uint64)tail[ 1]) << 8;
        case  1: k1 ^= ((yx_uint64)tail[ 0]) << 0;
            k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
    };
    
    //----------
    // finalization
    
    h1 ^= len; h2 ^= len;
    
    h1 += h2;
    h2 += h1;
    
    h1 = fmix64(h1);
    h2 = fmix64(h2);
    
    h1 += h2;
    h2 += h1;
    
    ((yx_uint64*)out)[0] = h1;
    ((yx_uint64*)out)[1] = h2;
}



/*
yx_uint64 yx_core_murmurhashWithSeed( const void * key, yx_size len, yx_uint seed )
{
    const yx_uint64 m = 0xc6a4a7935bd1e995;
    const yx_int r = 47;
    
    yx_uint64 h = seed ^ (len * m);
    
    const yx_uint64 * data = (const yx_uint64 *)key;
    const yx_uint64 * end = data + (len/8);
    
    while(data != end)
    {
        yx_uint64 k = *data++;
        
        k *= m;
        k ^= k >> r;
        k *= m;
        
        h ^= k;
        h *= m;
    }
    
    const yx_uchar* data2 = (const unsigned char*)data;
    
    switch(len & 7)
    {
        case 7: h ^= ((yx_uint64)(data2[6])) << 48;
        case 6: h ^= ((yx_uint64)(data2[5])) << 40;
        case 5: h ^= ((yx_uint64)(data2[4])) << 32;
        case 4: h ^= ((yx_uint64)(data2[3])) << 24;
        case 3: h ^= ((yx_uint64)(data2[2])) << 16;
        case 2: h ^= ((yx_uint64)(data2[1])) << 8;
        case 1: h ^= (yx_uint64)(data2[0]);
            h *= m;
    };
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
}
*/

#elif YX_DATAMODE_32

// 64-bit hash for 32-bit platforms
yx_uint64 yx_core_murmurhash64WithSeed ( const void * key, yx_size len, yx_uint seed )
{
    const yx_uint m = 0x5bd1e995;
    const yx_int r = 24;
    
    yx_uint h1 = seed ^ len;
    yx_uint h2 = 0;
    
    const yx_uint* data = (const yx_uint*)key;
    
    while(len >= 8)
    {
        yx_uint k1 = *data++;
        k1 *= m; k1 ^= k1 >> r; k1 *= m;
        h1 *= m; h1 ^= k1;
        len -= 4;
        
        yx_uint k2 = *data++;
        k2 *= m; k2 ^= k2 >> r; k2 *= m;
        h2 *= m; h2 ^= k2;
        len -= 4;
    }
    
    if(len >= 4)
    {
        yx_uint k1 = *data++;
        k1 *= m; k1 ^= k1 >> r; k1 *= m;
        h1 *= m; h1 ^= k1;
        len -= 4;
    }
    
    switch(len)
    {
        case 3: h2 ^= ((yx_uchar*)data)[2] << 16;
        case 2: h2 ^= ((yx_uchar*)data)[1] << 8;
        case 1: h2 ^= ((yx_uchar*)data)[0];
            h2 *= m;
    };
    
    h1 ^= h2 >> 18; h1 *= m;
    h2 ^= h1 >> 22; h2 *= m;
    h1 ^= h2 >> 17; h1 *= m;
    h2 ^= h1 >> 19; h2 *= m;
    
    yx_uint64 h = h1;
    
    h = (h << 32) | h2;
    
    return h;
}


void yx_core_murmurhash128WithSeed ( const void * key, const yx_size len, const yx_uint32 seed, void * out )
{
    const yx_uint8 * data = (const yx_uint8*)key;
    const yx_int nblocks = len / 16;
    
    yx_uint32 h1 = seed;
    yx_uint32 h2 = seed;
    yx_uint32 h3 = seed;
    yx_uint32 h4 = seed;
    
    const yx_uint32 c1 = 0x239b961b;
    const yx_uint32 c2 = 0xab0e9789;
    const yx_uint32 c3 = 0x38b34ae5;
    const yx_uint32 c4 = 0xa1e38b93;
    
    //----------
    // body
    
    const yx_uint32 * blocks = (const yx_uint32 *)(data + nblocks*16);
    
    for(yx_int i = -nblocks; i; i++)
    {
        yx_uint32 k1 = getblock32(blocks,i*4+0);
        yx_uint32 k2 = getblock32(blocks,i*4+1);
        yx_uint32 k3 = getblock32(blocks,i*4+2);
        yx_uint32 k4 = getblock32(blocks,i*4+3);
        
        k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
        
        h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;
        
        k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
        
        h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;
        
        k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
        
        h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;
        
        k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
        
        h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
    }
    
    //----------
    // tail
    
    const yx_uint8 * tail = (const yx_uint8*)(data + nblocks*16);
    
    yx_uint32 k1 = 0;
    yx_uint32 k2 = 0;
    yx_uint32 k3 = 0;
    yx_uint32 k4 = 0;
    
    switch(len & 15)
    {
        case 15: k4 ^= tail[14] << 16;
        case 14: k4 ^= tail[13] << 8;
        case 13: k4 ^= tail[12] << 0;
            k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
            
        case 12: k3 ^= tail[11] << 24;
        case 11: k3 ^= tail[10] << 16;
        case 10: k3 ^= tail[ 9] << 8;
        case  9: k3 ^= tail[ 8] << 0;
            k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
            
        case  8: k2 ^= tail[ 7] << 24;
        case  7: k2 ^= tail[ 6] << 16;
        case  6: k2 ^= tail[ 5] << 8;
        case  5: k2 ^= tail[ 4] << 0;
            k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
            
        case  4: k1 ^= tail[ 3] << 24;
        case  3: k1 ^= tail[ 2] << 16;
        case  2: k1 ^= tail[ 1] << 8;
        case  1: k1 ^= tail[ 0] << 0;
            k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };
    
    //----------
    // finalization
    
    h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;
    
    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;
    
    h1 = fmix32(h1);
    h2 = fmix32(h2);
    h3 = fmix32(h3);
    h4 = fmix32(h4);
    
    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;
    
    ((yx_uint32*)out)[0] = h1;
    ((yx_uint32*)out)[1] = h2;
    ((yx_uint32*)out)[2] = h3;
    ((yx_uint32*)out)[3] = h4;
}


#else

#error "unknown data mode"

#endif



