//游戏中显示下一关/失败/胜利画面
#include "win_game.h"
#include "win_info.h"

char *infoStr[4];
int infoIndex;
void infoWinEvent(int32 data, int32 eventId)
{
    mrc_printf("infoWinEvent(%d, %d)\n", data, eventId);
    if (data > 3)
    {
        mrc_winCloseById(infoWinPtr);
        return;
    }

    switch (eventId)
    {
    case WIN_EVENT_SHOW:
        infoIndex = data;
        infoStr[0] = "游戏失败,任意键退出";
        infoStr[1] = "游戏过关,任意键下一关";
        infoStr[2] = "游戏暂停,任意键继续";
        infoStr[3] = "小蟀作品";
        drawInfo();
        break;

    case WIN_EVENT_UPDATE:
    case WIN_EVENT_REFRESH:
        drawInfo();
        break;
    }
}

void infoKeyEvent(int32 data, int32 code, int32 param0, int32 param1)
{
    if (code == MR_KEY_PRESS)
    {
        mrc_winCloseById(infoWinPtr);
        switch (infoIndex)
        {
        case 0:
            mrc_winCloseById(gameWinPtr);
            break;
        }
    }
    mrc_sendEventTo(infoWinPtr, WIN_EVENT_UPDATE);
}

void drawInfo()
{
    int32 th, tw;
    mrc_drawRect(20 - 1, 20 - 1, SCRW - 40 + 2, SCRH - 40 + 2, 238, 232, 170);
    mrc_drawRect(20, 20, SCRW - 40, SCRH - 40, 80, 80, 80);
    mrc_textWidthHeight(infoStr[infoIndex], FALSE, MR_FONT_BIG, &tw, &th);
    mrc_drawText(infoStr[infoIndex], SCRW / 2 - tw / 2, SCRH / 2 - th / 2, 255, 255, 255, FALSE, MR_FONT_BIG);
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

