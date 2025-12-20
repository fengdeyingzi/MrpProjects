#ifndef _BRIDGE_H_
#define _BRIDGE_H_

#ifndef MRC_BASE_H
#include "mr_helper.h"
#endif

typedef struct {
#ifndef MRC_BASE_H
    mr_table_st *mr_table;
    mr_c_function_st *mr_c_function;
#else
    void *mr_table;
    void *mr_c_function;
#endif
    void *(*mrc_malloc)(int size);
    void (*mrc_free)(void *address);
    void (*mrc_clearScreen)(int32 r, int32 g, int32 b);
    int32 (*mrc_drawText)(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
    void (*mrc_refreshScreen)(int16 x, int16 y, uint16 w, uint16 h);

    int32 (*mrc_timerCreate)(void);
    void (*mrc_timerDelete)(int32 t);
    void (*mrc_timerStop)(int32 t);
    int32 (*mrc_timerStart)(int32 t, int32 time, int32 data, mrc_timerCB f, int32 loop);
    int32 (*mrc_timerSetTimeEx)(int32 t, int32 time);

    void (*mrc_exit)(void);
    int32 (*mrc_readFileFromMrpEx)(char *packname, const char *filename, uint8 **filebuf, int32 *filelen, int32 lookfor);
    int32 (*mrc_readFileFromMrpExA)(char *packname, const char *filename, uint8 **filebuf, int32 *filelen, int32 lookfor);
    int32 (*mrc_readFileFromMrpExPart)(char *packname, const char *filename, uint8 **filebuf, int32 *filelen, int32 offset, int32 read_len);
    void *(*mrc_readFileFromMrp)(const char *filename, int32 *filelen, int32 lookfor);
    void (*mrc_freeFileData)(void *data, int32 filelen);
    void (*mrc_freeOrigin)(void *add, int32 size);

    uint32 (*mrc_rand)(void);
    void (*mrc_sand)(uint32 seed);
} inFuncs_st;

typedef struct {
    int32 (*mrc_init)(void);
    int32 (*mrc_event)(int32 code, int32 param0, int32 param1);
    int32 (*mrc_pause)(void);
    int32 (*mrc_resume)(void);
    int32 (*mrc_exitApp)(void);
} outFuncs_st;

typedef outFuncs_st *(*_START)(inFuncs_st *in);

#endif
