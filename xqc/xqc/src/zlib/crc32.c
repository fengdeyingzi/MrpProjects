/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-2006, 2010 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Thanks to Rodney Brown <rbrown64@csc.com.au> for his contribution of faster
 * CRC methods: exclusive-oring 32 bits of data at a time, and pre-computing
 * tables for updating the shift register in one step with three exclusive-ors
 * instead of four steps with four exclusive-ors.  This results in about a
 * factor of two increase in speed on a Power PC G4 (PPC7455) using gcc -O3.
 */

#include "zutil.h" /* for STDC and FAR definitions */

#define local static

/* Find a four-byte integer type for crc32_little() and crc32_big(). */
#ifndef NOBYFOUR
#  define BYFOUR
   typedef unsigned long int u4;
   typedef long int ptrdiff_t;
#endif /* !NOBYFOUR */

/* Definitions for doing the crc four data bytes at a time. */
#ifdef BYFOUR
#  define REV(w) ((((w)>>24)&0xff)+(((w)>>8)&0xff00)+ \
                (((w)&0xff00)<<8)+(((w)&0xff)<<24))
   local unsigned long crc32_little(unsigned long, const unsigned char*, unsigned);
   local unsigned long crc32_big(unsigned long, const unsigned char*, unsigned);
#  define TBLS 8
#else
#  define TBLS 1
#endif /* BYFOUR */

#include "crc32.h"

/* Local functions for crc concatenation */
local unsigned long gf2_matrix_times(unsigned long *mat, unsigned long vec);
local void gf2_matrix_square(unsigned long *square, unsigned long *mat);
local uLong crc32_combine_(uLong crc1, uLong crc2, z_off64_t len2);

/* =========================================================================
 * This function can be used by asm versions of crc32()
 */
const unsigned long *get_crc_table()
{
    return (const unsigned long *)crc_table;
}

/* ========================================================================= */
#define DO1 crc = crc_table[0][((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8)
#define DO8 DO1; DO1; DO1; DO1; DO1; DO1; DO1; DO1

/* ========================================================================= */
unsigned long  crc32(unsigned long crc, const unsigned char *buf, uInt len)
{
    if (buf == Z_NULL) 
        return 0UL;

#ifdef BYFOUR
    if (sizeof(void *) == sizeof(ptrdiff_t)) 
    {
        u4 endian;

        endian = 1;
        if (*((unsigned char *)(&endian)))
            return crc32_little(crc, buf, len);
        else
            return crc32_big(crc, buf, len);
    }
#endif /* BYFOUR */

    crc = crc ^ 0xffffffffUL;
    while (len >= 8)
    {
        DO8;
        len -= 8;
    }

    if (len)
    {
        do {
            DO1;
        } while (--len);
    }
    return crc ^ 0xffffffffUL;
}

#ifdef BYFOUR

/* ========================================================================= */
#define DOLIT4 c ^= *buf4++; \
        c = crc_table[3][c & 0xff] ^ crc_table[2][(c >> 8) & 0xff] ^ \
            crc_table[1][(c >> 16) & 0xff] ^ crc_table[0][c >> 24]
#define DOLIT32 DOLIT4; DOLIT4; DOLIT4; DOLIT4; DOLIT4; DOLIT4; DOLIT4; DOLIT4

/* ========================================================================= */
local unsigned long crc32_little(unsigned long crc, const unsigned char *buf, unsigned len)
{
    register u4 c;
    register const u4 *buf4;

    c = (u4)crc;
    c = ~c;
    while (len && ((ptrdiff_t)buf & 3))
    {
        c = crc_table[0][(c ^ *buf++) & 0xff] ^ (c >> 8);
        len--;
    }

    buf4 = (const u4*)(const void*)buf;
    while (len >= 32) 
    {
        DOLIT32;
        len -= 32;
    }

    while (len >= 4) 
    {
        DOLIT4;
        len -= 4;
    }
    buf = (const unsigned char *)buf4;

    if (len)
    {
        do {
            c = crc_table[0][(c ^ *buf++) & 0xff] ^ (c >> 8);
        } while (--len);
    }
    c = ~c;
    return (unsigned long)c;
}

/* ========================================================================= */
#define DOBIG4 c ^= *++buf4; \
        c = crc_table[4][c & 0xff] ^ crc_table[5][(c >> 8) & 0xff] ^ \
            crc_table[6][(c >> 16) & 0xff] ^ crc_table[7][c >> 24]
#define DOBIG32 DOBIG4; DOBIG4; DOBIG4; DOBIG4; DOBIG4; DOBIG4; DOBIG4; DOBIG4

/* ========================================================================= */
local unsigned long crc32_big(unsigned long crc, const unsigned char *buf, unsigned len)
{
    register u4 c;
    register const u4 *buf4;

    c = REV((u4)crc);
    c = ~c;
    while (len && ((ptrdiff_t)buf & 3)) 
    {
        c = crc_table[4][(c >> 24) ^ *buf++] ^ (c << 8);
        len--;
    }

    buf4 = (const u4*)(const void*)buf;
    buf4--;
    while (len >= 32)
    {
        DOBIG32;
        len -= 32;
    }

    while (len >= 4)
    {
        DOBIG4;
        len -= 4;
    }
    buf4++;
    buf = (const unsigned char*)buf4;

    if (len) 
    {
        do {
            c = crc_table[4][(c >> 24) ^ *buf++] ^ (c << 8);
        } while (--len);
    }
    c = ~c;
    return (unsigned long)(REV(c));
}

#endif /* BYFOUR */

#define GF2_DIM 32      /* dimension of GF(2) vectors (length of CRC) */

/* ========================================================================= */
local unsigned long gf2_matrix_times(unsigned long *mat, unsigned long vec)
{
    unsigned long sum;

    sum = 0;
    while (vec) 
    {
        if (vec & 1)
            sum ^= *mat;
        vec >>= 1;
        mat++;
    }
    return sum;
}

/* ========================================================================= */
local void gf2_matrix_square(unsigned long *square, unsigned long *mat)
{
    int n;

    for (n = 0; n < GF2_DIM; n++)
        square[n] = gf2_matrix_times(mat, mat[n]);
}

/* ========================================================================= */
local uLong crc32_combine_(uLong crc1, uLong crc2, z_off64_t len2)
{
    int n;
    unsigned long row;
    unsigned long even[GF2_DIM];    /* even-power-of-two zeros operator */
    unsigned long odd[GF2_DIM];     /* odd-power-of-two zeros operator */

    /* degenerate case (also disallow negative lengths) */
    if (len2 <= 0)
        return crc1;

    /* put operator for one zero bit in odd */
    odd[0] = 0xedb88320UL;          /* CRC-32 polynomial */
    row = 1;
    for (n = 1; n < GF2_DIM; n++) 
    {
        odd[n] = row;
        row <<= 1;
    }

    /* put operator for two zero bits in even */
    gf2_matrix_square(even, odd);

    /* put operator for four zero bits in odd */
    gf2_matrix_square(odd, even);

    /* apply len2 zeros to crc1 (first square will put the operator for one
       zero byte, eight zero bits, in even) */
    do {
        /* apply zeros operator for this bit of len2 */
        gf2_matrix_square(even, odd);
        if (len2 & 1)
            crc1 = gf2_matrix_times(even, crc1);
        len2 >>= 1;

        /* if no more bits set, then done */
        if (len2 == 0)
            break;

        /* another iteration of the loop with odd and even swapped */
        gf2_matrix_square(odd, even);
        if (len2 & 1)
            crc1 = gf2_matrix_times(odd, crc1);
        len2 >>= 1;

        /* if no more bits set, then done */
    } while (len2 != 0);

    /* return combined crc */
    crc1 ^= crc2;
    return crc1;
}

/* ========================================================================= */
uLong  crc32_combine(uLong crc1, uLong crc2, z_off_t len2)
{
    return crc32_combine_(crc1, crc2, len2);
}

uLong  crc32_combine64(uLong crc1, uLong crc2, z_off64_t len2)
{
    return crc32_combine_(crc1, crc2, len2);
}
