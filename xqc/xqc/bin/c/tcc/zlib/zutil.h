/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2010 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* @(#) $Id$ */

#ifndef ZUTIL_H
#define ZUTIL_H

#define ZLIB_INTERNAL

#include "zlib.h"

#ifndef local
#  define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

typedef unsigned char  uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long  ulg;

#define ERR_RETURN(strm,err) \
  return (strm->msg = NULL, (err))
/* To be used only when the state is known to be valid */

        /* common constants */

#ifndef DEF_WBITS
#  define DEF_WBITS MAX_WBITS
#endif
/* default windowBits for decompression. MAX_WBITS is for compression only */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

        /* target dependencies */

#if defined(MSDOS) || (defined(WINDOWS) && !defined(WIN32))
#  define OS_CODE  0x00
#  if defined(__TURBOC__) || defined(__BORLANDC__)
#    if (__STDC__ == 1) && (defined(__LARGE__) || defined(__COMPACT__))
       /* Allow compilation with ANSI keywords only enabled */
       void _Cdecl farfree( void *block );
       void *_Cdecl farmalloc( unsigned long nbytes );
#    else
#      include <alloc.h>
#    endif
#  else /* MSC or DJGPP */
#    include <malloc.h>
#  endif
#endif

#ifdef WIN32
#  ifndef __CYGWIN__  /* Cygwin is Unix, not Win32 */
#    define OS_CODE  0x0b
#  endif
#endif

#if (defined(_MSC_VER) && (_MSC_VER > 600)) && !defined __INTERIX
#  if defined(_WIN32_WCE)
#    define fdopen(fd,mode) NULL /* No fdopen() */
#    ifndef _PTRDIFF_T_DEFINED
       typedef int ptrdiff_t;
#      define _PTRDIFF_T_DEFINED
#    endif
#  else
#    define fdopen(fd,type) _fdopen(fd,type)
#  endif
#endif

/* provide prototypes for these when building zlib without LFS */
#if !defined(_LARGEFILE64_SOURCE) || _LFS64_LARGEFILE-0 == 0
    extern uLong adler32_combine64(uLong, uLong, z_off_t);
    extern uLong crc32_combine64(uLong, uLong, z_off_t);
#endif

        /* common defaults */

#ifndef OS_CODE
#  define OS_CODE  0x0b  /* ��Ӧmrp��gz��ʽ */
#endif

//#ifndef OS_CODE
//#  define OS_CODE  0x03  /* assume Unix */
//#endif

#ifndef F_OPEN
#  define F_OPEN(name, mode) fopen((name), (mode))
#endif

         /* functions */

#if defined(STDC99) || (defined(__TURBOC__) && __TURBOC__ >= 0x550)
#  ifndef HAVE_VSNPRINTF
#    define HAVE_VSNPRINTF
#  endif
#endif
#if defined(__CYGWIN__)
#  ifndef HAVE_VSNPRINTF
#    define HAVE_VSNPRINTF
#  endif
#endif
#ifndef HAVE_VSNPRINTF
#  ifdef MSDOS
     /* vsnprintf may exist on some MS-DOS compilers (DJGPP?),
        but for now we just assume it doesn't. */
#    define NO_vsnprintf
#  endif
#  ifdef WIN32
     /* In Win32, vsnprintf is available as the "non-ANSI" _vsnprintf. */
#    if !defined(vsnprintf) && !defined(NO_vsnprintf)
#      if !defined(_MSC_VER) || ( defined(_MSC_VER) && _MSC_VER < 1500 )
#         define vsnprintf _vsnprintf
#      endif
#    endif
#  endif
#  ifdef __SASC
#    define NO_vsnprintf
#  endif
#endif

#if defined(pyr)
#  define NO_MEMCPY
#endif
#if defined(SMALL_MEDIUM) && !defined(_MSC_VER) && !defined(__SC__)
 /* Use our own functions for small and medium model with MSC <= 5.0.
  * You may have to use the same strategy for Borland C (untested).
  * The __SC__ check is for Symantec.
  */
#  define NO_MEMCPY
#endif
#if defined(STDC) && !defined(HAVE_MEMCPY) && !defined(NO_MEMCPY)
#  define HAVE_MEMCPY
#endif
#ifdef HAVE_MEMCPY
#  ifdef SMALL_MEDIUM /* MSDOS small or medium model */
#    define zmemcpy _fmemcpy
#    define zmemcmp _fmemcmp
#    define zmemzero(dest, len) _fmemset(dest, 0, len)
#  else
#    define zmemcpy memcpy
#    define zmemcmp memcmp
#    define zmemzero(dest, len) memset(dest, 0, len)
#  endif
#else
   void ZLIB_INTERNAL zmemcpy(Bytef* dest, const Bytef* source, uInt len);
   int ZLIB_INTERNAL zmemcmp(const Bytef* s1, const Bytef* s2, uInt len);
   void ZLIB_INTERNAL zmemzero(Bytef* dest, uInt len);
#endif

/* Diagnostic functions */
#ifdef DEBUG
#  include <stdio.h>
   extern int ZLIB_INTERNAL z_verbose;
   extern void ZLIB_INTERNAL z_error OF((char *m));
#  define Assert(cond,msg) {if(!(cond)) z_error(msg);}
#  define Trace(x) {if (z_verbose>=0) fprintf x ;}
#  define Tracev(x) {if (z_verbose>0) fprintf x ;}
#  define Tracevv(x) {if (z_verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (z_verbose>0 && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (z_verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond, msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

voidpf zcalloc(voidpf opaque, unsigned items, unsigned size);
void zcfree(voidpf opaque, voidpf ptr);

#define ZALLOC(strm, items, size) (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr) (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

#endif /* ZUTIL_H */
