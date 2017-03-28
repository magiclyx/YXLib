//
//  yx_macosx_string.c
//  YXLib
//
//  Created by LiuYuxi on 15/7/29.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_string.h"

/*
 string
 */

yx_char* yx_os_strlow(yx_char *dst, yx_char *src, yx_strSize len)
{
    while (len) {
        *dst = yx_os_tolower(*src);
        dst++;
        src++;
        len--;
    }

    return dst;
}
yx_char* yx_os_strup(yx_char *dst, yx_char *src, yx_strSize len)
{
    while (len) {
        *dst = yx_os_toupper(*src);
        dst++;
        src++;
        len--;
    }

    return dst;
}

/*
 * We use yx_os_strcasecmp()/yx_os_strncasecmp() for 7-bit ASCII strings only,
 * and implement our own ngx_strcasecmp()/ngx_strncasecmp()
 * to avoid libc locale overhead.
 */
yx_int yx_os_strncasecmp(yx_char* str1, yx_char* str2, yx_strSize len)
{
    yx_char c1, c2;
    
    while (len) {
        c1 = (yx_char) *str1++;
        c2 = (yx_char) *str2++;
        
        c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
        c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;
        
        if (c1 == c2) {
            
            if (c1) {
                len--;
                continue;
            }
            
            return 0;
        }
        
        return c1 - c2;
    }
    
    return 0;

}

yx_int yx_os_strcasecmp(yx_char* str1, yx_char* str2)
{
    yx_char  c1, c2;
    
    for ( ;; ) {
        c1 = (yx_char) *str1++;
        c2 = (yx_char) *str2++;
        
        c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
        c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;
        
        if (c1 == c2) {
            
            if (c1) {
                continue;
            }
            
            return 0;
        }
        
        return c1 - c2;
    }

}

yx_int yx_os_rstrncmp(yx_char *str1, yx_char *str2, yx_strSize len)
{
    if (len == 0) {
        return 0;
    }
    
    len--;
    
    for ( ;; ) {
        if (str1[len] != str2[len]) {
            return str1[len] - str2[len];
        }
        
        if (len == 0) {
            return 0;
        }
        
        len--;
    }
}

yx_int yx_os_rstrncasecmp(yx_char *str1, yx_char *str2, yx_strSize len)
{
    yx_char  c1, c2;
    
    if (len == 0) {
        return 0;
    }
    
    len--;
    
    for ( ;; ) {
        c1 = str1[len];
        if (c1 >= 'a' && c1 <= 'z') {
            c1 -= 'a' - 'A';
        }
        
        c2 = str2[len];
        if (c2 >= 'a' && c2 <= 'z') {
            c2 -= 'a' - 'A';
        }
        
        if (c1 != c2) {
            return c1 - c2;
        }
        
        if (len == 0) {
            return 0;
        }
        
        len--;
    }
}



yx_char* yx_os_strstrn(yx_char* string, yx_char* search, yx_strSize len)
{
    yx_char  c1, c2;
    
    c2 = *search++;
    
    do {
        do {
            c1 = *string++;
            
            if (c1 == 0) {
                return NULL;
            }
            
        } while (c1 != c2);
        
    } while (yx_os_strncmp(string, search, len) != 0);
    
    return --string;

}

yx_int yx_os_filename_cmp(yx_char *path1, yx_char *path2, yx_strSize len)
{
    yx_uint c1, c2;
    
    while (len) {
        c1 = (yx_uint) *path1++;
        c2 = (yx_uint) *path2++;
        
#if (YX_ENV_CASELES_FILESYSTEM)
        c1 = yx_os_tolower(c1);
        c2 = yx_os_tolower(c2);
#endif
        
        if (c1 == c2) {
            
            if (c1) {
                len--;
                continue;
            }
            
            return 0;
        }
        
        /*
         we need '/' to be the lowest character
         这应该和文件名排序相关
         */
        if (c1 == 0 || c2 == 0) {
            return (yx_int)(c1 - c2);
        }
        
        c1 = (c1 == '/') ? 0 : c1;
        c2 = (c2 == '/') ? 0 : c2;
        
        return (yx_int)(c1 - c2);
    }
    
    return 0;
}



yx_char* yx_os_strnstr(yx_char* string, yx_char* search, yx_strSize len) //search must a null-terminal string
{
    yx_char c1, c2;
    yx_size n;
    
    c2 = *(yx_char *) search++;
    
    n = yx_os_strlen(search);
    
    do {
        do {
            if (len-- == 0) {
                return NULL;
            }
            
            c1 = *string++;
            
            if (c1 == 0) {
                return NULL;
            }
            
        } while (c1 != c2);
        
        if (n > len) {
            return NULL;
        }
        
    } while (yx_os_strncmp(string, (yx_char *) search, n) != 0);
    
    return --string;
}


yx_char *yx_os_strcasestrn(yx_char *string, char *search, yx_strSize len)  //string must a null-terminal string
{
    yx_uint c1, c2;
    
    c2 = (yx_uint) *search++;
    c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;
    
    do {
        do {
            c1 = (yx_uint) *string++;
            
            if (c1 == 0) {
                return NULL;
            }
            
            c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
            
        } while (c1 != c2);
        
    } while (yx_os_strncasecmp(string, (yx_char *) search, len) != 0);
    
    return --string;

}

yx_char *yx_os_strlcasestrn(yx_char *string, yx_char *string_last, yx_char *search, yx_strSize len)
//yx_char *yx_os_strlcasestrn(yx_char *s1, yx_char *last, yx_char *s2, yx_strSize n)
{
    yx_uint c1, c2;
    
    c2 = (yx_uint) *search++;
    c2 = (c2 >= 'A' && c2 <= 'Z') ? (c2 | 0x20) : c2;
    string_last -= len;
    
    do {
        do {
            if (string >= string_last) {
                return NULL;
            }
            
            c1 = (yx_uint) *string++;
            
            c1 = (c1 >= 'A' && c1 <= 'Z') ? (c1 | 0x20) : c1;
            
        } while (c1 != c2);
        
    } while (yx_os_strncasecmp(string, search, len) != 0);
    
    return --string;
}





yx_char* yx_os_strfillstr(yx_char* dst, yx_char* src, yx_strSize len)
{
    if (len == 0) {
        return dst;
    }
    
    while (--len) {
        *dst = *src;
        
        if (*dst == '\0') {
            return dst;
        }
        
        dst++;
        src++;
    }
    
    *dst = '\0';
    
    return dst;
}


yx_char* yx_os_strlchr(yx_char *p, yx_char *last, yx_char c)
{
    while (p < last) {
        
        if (*p == c) {
            return p;
        }
        
        p++;
    }
    
    return NULL;
}


yx_int yx_os_atoi(const yx_char* str)
{
    register yx_int total;
    register yx_int c;
    register yx_int sign;
    register const yx_char* s = str;
    
    while (yx_os_isspace((int)(unsigned char)*s))
        ++s;
    
    c = (int)(unsigned char)*s++;
    sign = c;
    
    if ('-'  ==  c  ||  c == '+')
        c = (int)(unsigned char)*s++;
    
    total = 0;
    while ( yx_os_isdigit(c) )
    {
        total = 10 * total + (c - '0');
        c = (int)(unsigned char)*s++;
    }
    
    
    if ('-' == sign)
        return -total;
    else
        return total;
}

yx_int yx_os_atoin(const yx_char* str, register yx_strSize len)
{
    register yx_strSize index;
    register yx_int total = 0;
    register yx_int c;
    register yx_int sign;
    register const yx_char* s = str;
    
    if (0 == len)
        return total;
    
    index = 0;
    while (yx_os_isspace((int)(unsigned char)*s))
    {
        ++s;
        
        if (index >= len)
            return 0;
        
        index++;
    }
    
    c = (int)(unsigned char)*s++;
    sign = c;
    
    if ('-'  ==  c  ||  c == '+')
    {
        c = (int)(unsigned char)*s++;
        index++;
        
        if (index >= len)
            return 0;
    }
    
    while (yx_os_isdigit(c)  &&  index < len)
    {
        total = 10 * total + (c - '0');
        c = (int)(unsigned char)*s++;
        
        index++;
    }
    
    
    if ('-' == sign)
        return -total;
    else
        return total;

}

yx_int yx_os_atoin_fast(const yx_char* str, register yx_strSize len) //weak but faster
{
    register yx_int total = 0;
    register const yx_char* s = str;
    register yx_int c;
    
    if (len == 0)
        return -1;
    
    for (; len--; s++) {
        
        c = (int)(unsigned char)*s++;
        
        if (!yx_os_isdigit(c))
            return -1;
        
        total = total * 10 + (c - '0');
    }
    
    
    return total;
}

yx_long yx_os_atol_base(const yx_char* str, yx_char** endPtr, register yx_int base) //base 0, 2~36
{
    register const yx_char *s = str;
    register yx_ulong acc;
    register yx_int c;
    register yx_ulong cutoff;
    register yx_int neg = 0, any, cutlim;
    
    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    do {
        c = *s++;
    } while (yx_os_isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if ((base == 0 || base == 2) &&
               c == '0' && (*s == 'b' || *s == 'B')) {
        c = s[1];
        s += 2;
        base = 2;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;
    
    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? -(yx_ulong)LONG_MIN : LONG_MAX;
    cutlim = (yx_int)(cutoff % (yx_ulong)base);
    cutoff /= (yx_ulong)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (yx_os_isdigit(c))
            c -= '0';
        else if (yx_os_isalpha(c))
            c -= yx_os_isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc *= (yx_ulong)base;
            acc += (yx_ulong)c;
        }
    }
    if (any < 0) {
        acc = (yx_ulong)(neg ? YX_MIN_LONG : YX_MAX_LONG);
        //		errno = ERANGE;
    } else if (neg)
        acc = -acc;
    if (endPtr != 0)
        *endPtr = (yx_char *)(any ? s - 1 : str);
    
    return (yx_long)(acc);

}

yx_ulong yx_os_atoul_base(const yx_char *str, yx_char **endPtr, register yx_int base)
{
    register const yx_char *s = str;
    register yx_ulong acc;
    register yx_int c;
    register yx_ulong cutoff;
    register yx_int neg = 0, any, cutlim;
    
    /*
     * See strtol for comments as to the logic used.
     */
    do {
        c = *s++;
    } while (yx_os_isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if ((base == 0 || base == 2) &&
               c == '0' && (*s == 'b' || *s == 'B')) {
        c = s[1];
        s += 2;
        base = 2;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;
    cutoff = (yx_ulong)YX_MAX_ULONG / (yx_ulong)base;
    cutlim = (yx_int)((yx_ulong)YX_MAX_ULONG % (yx_ulong)base);
    for (acc = 0, any = 0;; c = *s++) {
        if (yx_os_isdigit(c))
            c -= '0';
        else if (yx_os_isalpha(c))
            c -= yx_os_isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc *= (yx_ulong)base;
            acc += (yx_ulong)c;
        }
    }
    if (any < 0) {
        acc = ULONG_MAX;
        //		errno = ERANGE;
    } else if (neg)
        acc = -acc;
    if (endPtr != 0)
        *endPtr = (yx_char *)(any ? s - 1 : str);
    return (acc);
}

yx_int yx_os_atofp_fast(const yx_char* str, register yx_strSize n, register yx_strSize point)
{
    const register yx_char* s = str;
    register yx_int value;
    register yx_uint dot;
    
    if (n == 0) {
        return -1;
    }
    
    dot = 0;
    
    for (value = 0; n--; s++) {
        
        if (point == 0) {
            return -1;
        }
        
        if (*s == '.') {
            if (dot) {
                return -1;
            }
            
            dot = 1;
            continue;
        }
        
        if (*s < '0' || *s > '9') {
            return -1;
        }
        
        value = value * 10 + (*s - '0');
        point -= dot;
    }
    
    while (point--) {
        value = value * 10;
    }
    
    if (value < 0) {
        return -1;
        
    } else {
        return value;
    }
}



yx_int yx_os_hextoi_fast(const yx_char* str, register yx_strSize len)
{
    const register yx_char* s = str;
    yx_uchar c, ch;
    yx_int value;
    
    if (len == 0) {
        return -1;
    }
    
    for (value = 0; len--; s++) {
        ch = (yx_uchar)(*s);
        
        if (ch >= '0' && ch <= '9') {
            value = value * 16 + (ch - '0');
            continue;
        }
        
        c = (yx_uchar) (ch | 0x20);
        
        if (c >= 'a' && c <= 'f') {
            value = value * 16 + (c - 'a' + 10);
            continue;
        }
        
        return -1;
    }
    
    if (value < 0) {
        return -1;
        
    } else {
        return value;
    }
}


#define YX_INT_DIGITS_64 19		/* enough for 64 bit integer */
#define YX_INT_DIGITS_32 9         /* enough for 32 bit integer */

yx_char* yx_ltoa(yx_long i)
{
    /* Room for INT_DIGITS_32 digits, - and '\0' */
    static yx_char buf[YX_INT_DIGITS_64 + 2];
    yx_char *p = buf + YX_INT_DIGITS_64 + 1;	/* points to terminating '\0' */
    if (i >= 0) {
        do {
            *--p = '0' + (i % 10);
            i /= 10;
        } while (i != 0);
        return p;
    }
    else {			/* i < 0 */
        do {
            *--p = '0' - (i % 10);
            i /= 10;
        } while (i != 0);
        *--p = '-';
    }
    return p;
}


#define YX_MAX_PRECISION	(10)
static const yx_real rounders[YX_MAX_PRECISION + 1] =
{
    0.5,				// 0
    0.05,				// 1
    0.005,				// 2
    0.0005,				// 3
    0.00005,			// 4
    0.000005,			// 5
    0.0000005,			// 6
    0.00000005,			// 7
    0.000000005,		// 8
    0.0000000005,		// 9
    0.00000000005		// 10
};
yx_char* yx_ftoa_ex(yx_real f, yx_char* buf, yx_int precision)
{
    yx_char* ptr = buf;
    yx_char* p = ptr;
    yx_char* p1;
    yx_char c;
    yx_long intPart;
    // check precision bounds
    if (precision > YX_MAX_PRECISION)
        precision = YX_MAX_PRECISION;
    // sign stuff
    if (f < 0)
    {
        f = -f;
        *ptr++ = '-';
    }
    if (precision < 0)  // negative precision == automatic precision guess
    {
        if (f < 1.0) precision = 6;
        else if (f < 10.0) precision = 5;
        else if (f < 100.0) precision = 4;
        else if (f < 1000.0) precision = 3;
        else if (f < 10000.0) precision = 2;
        else if (f < 100000.0) precision = 1;
        else precision = 0;
    }
    // round value according the precision
    if (precision)
        f += rounders[precision];
    // integer part...
    intPart = (yx_long)f;
    f -= intPart;
    if (!intPart)
        *ptr++ = '0';
    else
    {
        // save start pointer
        p = ptr;
        // convert (reverse order)
        while (intPart)
        {
            *p++ = '0' + intPart % 10;
            intPart /= 10;
        }
        // save end pos
        p1 = p;
        // reverse result
        while (p > ptr)
        {
            c = *--p;
            *p = *ptr;
            *ptr++ = c;
        }
        // restore end pos
        ptr = p1;
    }
    // decimal part
    if (precision)
    {
        // place decimal point
        *ptr++ = '.';
        // convert
        while (precision--)
        {
            f *= 10.0;
            c = (yx_char)f;
            *ptr++ = '0' + c;
            f -= c;
        }
    }
    // terminating zero
    *ptr = 0;
    return buf;
}

yx_char* yx_ftoa(yx_real f)
{
    static yx_char buf[30];
    return yx_ftoa_ex(f, buf, -1);
}





yx_char* yx_os_hex2str(yx_char* dst, const yx_char* src, yx_strSize len)
{
    static yx_char hex[] = "0123456789abcdef";
    
    while (len--) {
        *dst++ = hex[*src >> 4];
        *dst++ = hex[*src++ & 0xf];
    }
    
    return dst;
}

yx_char* yx_os_num2str(yx_char* dst, yx_strSize len, yx_int num, yx_int base)
{
    yx_char *p;
    
    if(base<2 || base>16) return NULL;
    
    p = &dst[len - 1];
    *p = '\0';
    
    do{
        *--p = "0123456789abcdef"[num % base];
        num /= base;
    }while(num != 0  &&  p != dst);
    
    return p;
}







