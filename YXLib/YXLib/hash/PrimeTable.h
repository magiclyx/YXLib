/*!
@header PrimeTable.h 
@abstract This file is used to get the max hash size.
@author Yuxi Liu
@version 1.00 January 5, 2012 Creation
*/


#ifndef TestHas_PrimeTable_h
#define TestHas_PrimeTable_h


#ifdef __cplusplus
extern "C"{
#endif



/*!
@method
@abstract
    Under the suggested value to calculate the size of the hash value
@discussion
    if the suggested value larger than the 65534. This function will run for a long time.
@param
    size - the suggested value
@result
    return the hash size
*/
size_t getPrimeSize(size_t size);

#ifdef __cplusplus
}
#endif

    
    
#endif //include once
