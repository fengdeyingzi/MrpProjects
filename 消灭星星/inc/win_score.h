#ifndef _SCORE_H_
#define _SCORE_H_

#include "mrc_base.h"
#include "mrc_exb.h"
#include "mrc_tool.h"

#include "mrc_win.h"
#include "mrc_menu.h"

extern int32 scoreWinPtr;
extern void scoreWinEvent(int32 data, int32 eventId);
extern void scoreKeyEvent(int32 data, int32 code, int32 param0, int32 param1);
extern void showScoreWin(void);
extern void initScoreWin(void);

#endif
