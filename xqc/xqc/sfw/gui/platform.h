/*
** paltform.h: the platform header file.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#ifndef _SGL_PLATFORM_H
#define _SGL_PLATFORM_H

#include "config.h"   //include the system configuration
#include "mrc_base.h"
//include mrc_plat
//#include "mrc_base_i.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _EXT_RELEASE
#ifndef DISABLE_SGL_TRACE
#define DISABLE_SGL_TRACE
#endif

#ifndef DISABLE_SGL_LOG
#define DISABLE_SGL_LOG
#endif

#ifndef DISABLE_SGL_ASSERT
#define DISABLE_SGL_ASSERT
#endif

#ifdef ENABLE_TRACE_TO_FILE
#undef ENABLE_TRACE_TO_FILE
#endif
#endif

#ifdef ENABLE_TRACE_TO_FILE

//global data
#define FILE_TRACE_BUFFER_SIZE		512
extern char g_TraceBuffer[FILE_TRACE_BUFFER_SIZE];
extern int32 g_TraceFD;

//call this function in mrc_init before any other code
#define SGL_FILE_TRACE_BEGIN(filename) \
	g_TraceFD = mrc_open(filename, MR_FILE_WRONLY | MR_FILE_CREATE)

//call this function in mrc_exitApp after all other code
#define SGL_FILE_TRACE_END() \
	mrc_close(g_TraceFD)

//replace SGL_TRACE with this macro, just #define SGL_TRACE SGL_FILE_TRACE.
#define SGL_FILE_TRACE(...) \
do{\
	int len = sprintf(g_TraceBuffer, __VA_ARGS__);\
	mrc_write(g_TraceFD, g_TraceBuffer, len);\
}while(0)

#endif

#ifndef DISABLE_SGL_TRACE
#ifdef ENABLE_TRACE_TO_FILE
#define SGL_TRACE		SGL_FILE_TRACE
#else
#define SGL_TRACE		mrc_printf
#endif
#else
#define SGL_TRACE(...)
#endif

#ifndef DISABLE_SGL_LOG
void sgl_log(const char* file, void* buffer, int size);
#define SGL_LOG(f, b, s)		sgl_log(f, b, s)
#else
#define SGL_LOG(f, b, s)
#endif

#ifndef DISABLE_SGL_ASSERT
void sgl_assert(const char* file, int line, int exp);		
#define SGL_ASSERT(exp)		sgl_assert(__FILE__, __LINE__, (exp))
#else
#define SGL_ASSERT(exp)
#endif

#define SGL_MALLOC		mrc_malloc
#define SGL_FREE		mrc_free
#define SGL_MEMSET		MR_MEMSET
#define SGL_MEMCPY		MR_MEMCPY
#define SGL_MEMMOVE		MR_MEMMOVE
#define SGL_STRCPY		MR_STRCPY
#define SGL_STRNCPY		MR_STRNCPY
#define SGL_STRCAT		MR_STRCAT
#define SGL_STRNCAT		MR_STRNCAT
#define SGL_MEMCMP		MR_MEMCMP
#define SGL_STRCMP		MR_STRCMP
#define SGL_STRNCMP		MR_STRNCMP
#define SGL_STRCOLL		MR_STRCOLL
#define SGL_MEMCHR		MR_MEMCHR
#define SGL_STRLEN		MR_STRLEN
#define SGL_STRSTR		MR_STRSTR
#define SGL_SPRINTF		MR_SPRINTF
#define SGL_ATOI		MR_ATOI
#define SGL_STROUL		MR_STRTOUL

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SGL_PLATFORM_H */

