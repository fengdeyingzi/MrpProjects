#ifndef _INFO_H_
#define _INFO_H_

#include "mrc_base.h"
#include "mrc_exb.h"

#include "mrc_win.h"
#include "mrc_menu.h"

extern int32 SCRW, SCRH;
//游戏中显示下一关/失败/胜利画面
#include "win_game.h"
#include "win_info.h"

extern char* infoStr[4];
extern int infoIndex;

extern void infoWinEvent(int32 data, int32 eventId);
extern void infoKeyEvent(int32 data, int32 code, int32 param0, int32 param1);
extern void drawInfo(void);


#endif
