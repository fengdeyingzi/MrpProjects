#ifndef __UNI_SPRINTF_H__
#define __UNI_SPRINTF_H__

#include <stdarg.h>

/*
 * To format the string.  Currently %s(unicode string) and %d (decimal integer)
 * are supported.
 *
 * Parameters:
 *        dst - the destination buffer,  make sure not to overflow the buffer.
 *
 *        fmt - the format string also in utf16be encoding, 
 *              That is to say "%s" "%d" are also encoded in utf16be.      
 */

int32 uni_sprintf(char * dst, const char* fmt, ...);

#endif /* __UNI_SPRINTF_H__ */