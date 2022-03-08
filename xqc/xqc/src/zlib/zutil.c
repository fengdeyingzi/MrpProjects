/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2005, 2010 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include "zutil.h"

#ifndef NO_DUMMY_DECL
struct internal_state {int dummy;}; /* for buggy compilers */
#endif

#ifndef HAVE_MEMCPY

void ZLIB_INTERNAL zmemcpy(dest, source, len)
    Bytef* dest;
    const Bytef* source;
    uInt  len;
{
    if (len == 0) return;
    do {
        *dest++ = *source++; /* ??? to be unrolled */
    } while (--len != 0);
}

int ZLIB_INTERNAL zmemcmp(s1, s2, len)
    const Bytef* s1;
    const Bytef* s2;
    uInt  len;
{
    uInt j;

    for (j = 0; j < len; j++) {
        if (s1[j] != s2[j]) return 2*(s1[j] > s2[j])-1;
    }
    return 0;
}

void ZLIB_INTERNAL zmemzero(dest, len)
    Bytef* dest;
    uInt  len;
{
    if (len == 0) return;
    do {
        *dest++ = 0;  /* ??? to be unrolled */
    } while (--len != 0);
}
#endif

#ifndef MY_ZCALLOC /* Any system without a special alloc function */

#ifndef STDC
extern voidp malloc(uInt size);
//extern voidp calloc(uInt items, uInt size);
extern void free(voidpf ptr);
#endif

voidpf zcalloc (voidpf opaque, unsigned items, unsigned size)
{
    if (opaque)
        items += size - size; /* make compiler happy */
    return (voidpf)malloc(items * size);
}

void zcfree (voidpf opaque, voidpf ptr)
{
    free(ptr);
    if (opaque) 
        return; /* make compiler happy */
}

#endif /* MY_ZCALLOC */
