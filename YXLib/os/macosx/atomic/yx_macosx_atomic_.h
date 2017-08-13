//
//  yx_os_atomic_drawin.h
//  YXLib
//
//  Created by Yuxi Liu on 7/19/13.
//  Copyright (c) 2013 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_ATOMIC_H_
#define _YX_MACOSX_ATOMIC_H_

#include "../yx_macosx_global.h"

//:~ TODO support barrier here !

typedef volatile yx_int32  yx_atomic32;
typedef volatile yx_int64  yx_atomic64;
//typedef volatile void*     yx_atomicPtr;

#define yx_atomic32_max YX_MAX_INT32
#define yx_atomic32_min YX_MIN_INT32

#define yx_atomic64_max YX_MAX_INT64
#define yx_atomic64_min YX_MIN_INT64

#define yx_atomic32_len YX_INT32_LEN
#define yx_atomic64_len YX_INT64_LEN



/*****************************************************************************************************/
/*Add*/

/*Adds two integer values together and stores the result in one of the specified variables.*/
/*****************************************************************************************************/


/*! @abstract Atomically adds two 32-bit values.
 @discussion
 This function adds the value given by <code>__theAmount</code> to the
 value in the memory location referenced by <code>__theValue</code>,
 storing the result back to that memory location atomically.
 @result Returns the new value.
 */
//int32_t	OSAtomicAdd32( int32_t __theAmount, volatile int32_t *__theValue );

#define yx_atomic_add32(val, atomic_ptr)  OSAtomicAdd32(val, atomic_ptr)



/*! @abstract Atomically adds two 64-bit values.
 @discussion
 This function adds the value given by <code>__theAmount</code> to the
 value in the memory location referenced by <code>__theValue</code>,
 storing the result back to that memory location atomically.
 */
//int64_t	OSAtomicAdd64( int64_t __theAmount, volatile int64_t *__theValue );
#define yx_atomic_add64(val, atomic_ptr) OSAtomicAdd64(val, atomic_ptr)




/*****************************************************************************************************/
/*Increament*/

/*Increments the specified integer value by 1.*/
/*****************************************************************************************************/

/*! @abstract Atomically increments a 32-bit value. */
//__inline static int32_t	OSAtomicIncrement32( volatile int32_t *__theValue ) { return OSAtomicAdd32(  1, __theValue); }
#define yx_atomic_increment32(atomic_ptr) OSAtomicIncrement32(atomic_ptr)

/*! @abstract Atomically increments a 64-bit value. */
//__inline static int64_t	OSAtomicIncrement64( volatile int64_t *__theValue ) { return OSAtomicAdd64(  1, __theValue); }
#define yx_atomic_increment64(atomic_ptr) OSAtomicIncrement64(atomic_ptr)





/*****************************************************************************************************/
/*Decrement*/

/*Decrements the specified integer value by 1.*/
/*****************************************************************************************************/


/*! @abstract Atomically decrements a 32-bit value. */
//__inline static int32_t	OSAtomicDecrement32( volatile int32_t *__theValue ) { return OSAtomicAdd32( -1, __theValue); }
#define yx_atomic_decrement32(atomic_ptr) OSAtomicDecrement32(atomic_ptr)

/*! @abstract Atomically decrements a 64-bit value. */
//__inline static int64_t	OSAtomicDecrement64( volatile int64_t *__theValue ) { return OSAtomicAdd64( -1, __theValue); }
#define yx_atomic_decrement64(atomic_ptr) OSAtomicDecrement64(atomic_ptr)



/*****************************************************************************************************/
/*Logical OR*/

/*Performs a logical OR between the specified 32-bit value and a 32-bit mask.*/
/*****************************************************************************************************/


/*! @abstract Atomic bitwise OR of two 32-bit values.
 @discussion
 This function performs the bitwise OR of the value given by <code>__theMask</code>
 with the value in the memory location referenced by <code>__theValue</code>,
 storing the result back to that memory location atomically.
 @result Returns the new value.
 */
//int32_t	OSAtomicOr32( uint32_t __theMask, volatile uint32_t *__theValue );
#define yx_atomic_or32(theMask, atomic_ptr) OSAtomicOr32(theMask, atomic_ptr)





/*****************************************************************************************************/
/*Logical AND*/

/*Performs a logical AND between the specified 32-bit value and a 32-bit mask.*/
/*****************************************************************************************************/

/*! @abstract Atomic bitwise AND of two 32-bit values.
 @discussion
 This function performs the bitwise AND of the value given by <code>__theMask</code>
 with the value in the memory location referenced by <code>__theValue</code>,
 storing the result back to that memory location atomically.
 @result Returns the new value.
 */
//int32_t	OSAtomicAnd32( uint32_t __theMask, volatile uint32_t *__theValue );
#define yx_atomic_and32(theMask, atomic_ptr) OSAtomicAnd32(theMask, atomic_ptr)




/*****************************************************************************************************/
/*Logical XOR*/

/*Performs a logical XOR between the specified 32-bit value and a 32-bit mask.*/
/*****************************************************************************************************/


/*! @abstract Atomic bitwise XOR of two 32-bit values.
 @discussion
 This function performs the bitwise XOR of the value given by <code>__theMask</code>
 with the value in the memory location referenced by <code>__theValue</code>,
 storing the result back to that memory location atomically.
 @result Returns the new value.
 */
//int32_t	OSAtomicXor32( uint32_t __theMask, volatile uint32_t *__theValue );

#define yx_atomic_xor32(theMask, atomic_ptr) OSAtomicXor32(theMask, atomic_ptr)



/*****************************************************************************************************/
/*Compare and swap*/

/*
 Compares a variable against the specified old value. If the two values are equal, this function assigns the specified new value to the variable; otherwise, it does nothing. The comparison and assignment are done as one atomic operation and the function returns a Boolean value indicating whether the swap actually occurred.
 */
/*****************************************************************************************************/

/*! @abstract Compare and swap for 32-bit values.
 @discussion
 This function compares the value in <code>__oldValue</code> to the value
 in the memory location referenced by <code>__theValue</code>.  If the values
 match, this function stores the value from <code>__newValue</code> into
 that memory location atomically.
 @result Returns TRUE on a match, FALSE otherwise.
 */
//bool    OSAtomicCompareAndSwap32( int32_t __oldValue, int32_t __newValue, volatile int32_t *__theValue );
#define yx_atomic_compareAndSwap32(oldVal, newVal, atomic_ptr) OSAtomicCompareAndSwap32(oldVal, newVal, atomic_ptr)


/*! @abstract Compare and swap for <code>uint64_t</code> values.
 @discussion
 This function compares the value in <code>__oldValue</code> to the value
 in the memory location referenced by <code>__theValue</code>.  If the values
 match, this function stores the value from <code>__newValue</code> into
 that memory location atomically.
 @result Returns TRUE on a match, FALSE otherwise.
 */
//bool    OSAtomicCompareAndSwap64( int64_t __oldValue, int64_t __newValue, volatile int64_t *__theValue );
#define yx_atomic_compareAndSwap64(oldVal, newVal, atomic_ptr) OSAtomicCompareAndSwap64(oldVal, newVal, atomic_ptr)




/*! @abstract Compare and swap pointers.
 @discussion
 This function compares the pointer stored in <code>__oldValue</code> to the pointer
 in the memory location referenced by <code>__theValue</code>.  If the pointers
 match, this function stores the pointer from <code>__newValue</code> into
 that memory location atomically.
 @result Returns TRUE on a match, FALSE otherwise.
 */
//bool	OSAtomicCompareAndSwapPtr( void *__oldValue, void *__newValue, void * volatile *__theValue ) __OSX_AVAILABLE_STARTING(__MAC_10_5, __IPHONE_2_0);

#define yx_atomic_compareAnsSwapPtr(oldPtr, newPtr, atomic_ptr)  OSAtomicCompareAndSwapPtr(oldPtr, newPtr, atomic_ptr)



/*****************************************************************************************************/
/*Test and set*/

/*
Tests a bit in the specified variable, sets that bit to 1, and returns the value of the old bit as a Boolean value. Bits are tested according to the formula (0×80 >> (n & 7)) of byte((char*)address + (n >> 3)) where n is the bit number and address is a pointer to the variable. This formula effectively breaks up the variable into 8-bit sized chunks and orders the bits in each chunk in reverse. For example, to test the lowest-order bit (bit 0) of a 32-bit integer, you would actually specify 7 for the bit number; similarly, to test the highest order bit (bit 32), you would specify 24 for the bit number.
 */
/*****************************************************************************************************/

/* Test and set.  They return the original value of the bit, and operate on bit (0x80>>(n&7))
 * in byte ((char*)theAddress + (n>>3)).
 */
/*! @abstract Atomic test and set
 @discussion
 This function tests a bit in the value referenced by <code>__theAddress</code>
 and if it is not set, sets it.  The bit is chosen by the value of <code>__n</code>.
 The bits are numbered in order beginning with bit 1 as the lowest order bit.
 
 For example, if <code>__theAddress</code> points to a 64-bit value,
 to compare the value of the highest bit, you would specify <code>64</code> for
 <code>__n</code>.
 @result
 Returns the original value of the bit being tested.
 */
//bool    OSAtomicTestAndSet( uint32_t __n, volatile void *__theAddress );

#define yx_atomic_testAndSet(bitNum, atomic_ptr) OSAtomicTestAndSet(bitNum, atomic_ptr)



/*****************************************************************************************************/
/*Test and clear*/

/*
Tests a bit in the specified variable, sets that bit to 0, and returns the value of the old bit as a Boolean value. Bits are tested according to the formula (0×80 >> (n & 7)) of byte((char*)address + (n >> 3)) where n is the bit number and address is a pointer to the variable. This formula effectively breaks up the variable into 8-bit sized chunks and orders the bits in each chunk in reverse. For example, to test the lowest-order bit (bit 0) of a 32-bit integer, you would actually specify 7 for the bit number; similarly, to test the highest order bit (bit 32), you would specify 24 for the bit number.
 */
/*****************************************************************************************************/


/*! @abstract Atomic test and clear
 @discussion
 This function tests a bit in the value referenced by <code>__theAddress</code>
 and if it is not cleared, clears it.  The bit is chosen by the value of <code>__n</code>.
 The bits are numbered in order beginning with bit 1 as the lowest order bit.
 
 For example, if <code>__theAddress</code> points to a 64-bit value,
 to compare the value of the highest bit, you would specify <code>64</code> for
 <code>__n</code>.
 @result
 Returns the original value of the bit being tested.
 */
//bool    OSAtomicTestAndClear( uint32_t __n, volatile void *__theAddress );

#define yx_atomic_testAndClear(bitNum, atomic_ptr) OSAtomicTestAndClear(bitNum, atomic_ptr)




#endif






