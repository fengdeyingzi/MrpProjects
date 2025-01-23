#ifndef _GAME_H_
#define _GAME_H_

#include "mrc_base.h"
#include "mrc_exb.h"

#include "mrc_win.h"
#include "mrc_text.h"

extern int32 SCRW, SCRH;
extern int32 gameWinPtr, homeWinPtr, infoWinPtr;

extern int i, j, k, m, n;
extern int32 startMem, runMem;

#define IW (SCRW / 3 - 3)
#define IH (SCRH / 4 - 4)
#define TW (SCRW - IW * 3) / 2
#define TH (SCRH - IH * 4) / 2
#define IN (i + j * 3)

//game.c
extern void showGame(void);
extern void initGame(void);

extern void gameWinEvent(int32 data,int32 eventId);
extern void gameKeyEvent(int32 data, int32 code,int32 param0,int32 param1);

#endif
