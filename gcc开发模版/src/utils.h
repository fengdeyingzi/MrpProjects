// #include "base.h"


#define MAKERGB565(r, g, b) (uint16)(((uint32)(r >> 3) << 11) | ((uint32)(g >> 2) << 5) | ((uint32)(b >> 3)))
#define PIXEL565R(v) ((((uint32)v >> 11) << 3) & 0xff)
#define PIXEL565G(v) ((((uint32)v >> 5) << 2) & 0xff)
#define PIXEL565B(v) (((uint32)v << 3) & 0xff)
#define MAKERGB(r, g, b) ( ((uint16)(r>>3) << 11) | ((uint16)(g>>2) << 5) | ((uint16)(b>>3)) )
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
