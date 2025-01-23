

#ifndef _HOME_H_
#define _HOME_H_

#include "mrc_base.h"
#include "mrc_exb.h"

#include "mrc_win.h"
#include "mrc_menu.h"

extern void drawHome(void);
extern void homeMenuClick(int32 data);
extern void homeWinEvent(int32 data,int32 eventId);
extern void homeKeyEvent(int32 data, int32 code,int32 param0,int32 param1);

#endif
