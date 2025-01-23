#include "mrc_base.h"
#include "mrc_exb.h"

#include "mrc_win.h"
#include "mrc_text.h"
#include "mrc_menu.h"
#include "mrc_aux.h"

#include "win_game.h"
#include "win_home.h"


int32 SCRW, SCRH;
int32 gameWinPtr, homeWinPtr, infoWinPtr, scoreWinPtr;
mr_screeninfo screenInfo;
int32 startMem, runMem;
int i, j, k, m, n;

//入口函数
int32 mrc_init(void)
{
    startMem = mrc_getMemoryRemain();
    
    mrc_getScreenInfo(&screenInfo);
    SCRW = screenInfo.width;
    SCRH = screenInfo.height;

    mrc_winInit();

    homeWinPtr = mrc_winNew(0, homeWinEvent, homeKeyEvent);
    return MR_SUCCESS;
}

//退出函数
int32 mrc_exitApp(void)
{
    mrc_printf("MRC_EXT_EXIT();\r\n");
    return MR_SUCCESS;
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    mrc_printf("mrc_appEvent(%d, %d, %d)\r\n", code, param0, param1);
    mrc_winEvent(code, param0, param1);
    return MR_SUCCESS;
}

int32 mrc_pause()
{
    mrc_printf("mrc_appPause();\r\n");
    return MR_SUCCESS;
}

int32 mrc_resume()
{
    mrc_printf("mrc_appResume();\r\n");
    return MR_SUCCESS;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 param0, int32 param1)
{
    return MR_SUCCESS;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
    return MR_SUCCESS;
}

