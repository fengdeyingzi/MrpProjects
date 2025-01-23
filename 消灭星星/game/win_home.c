///��Ϸ��ҳ/�˵�/LOGO

#include "win_home.h"
#include "win_game.h"
#include "win_info.h"
#include "win_score.h"

char *homeMenu[5];
#define MENU_NUM 4
int32 menuIndex = 0;

void homeWinEvent(int32 data, int32 eventId)
{
    mrc_printf("homeWinEvent:%d", eventId);

    homeMenu[0] = "��ʼ��Ϸ";
    homeMenu[1] = "��Ϸ����";
    homeMenu[2] = "������Ϸ";
    homeMenu[3] = "�˳���Ϸ";

    switch (eventId)
    {
    case WIN_EVENT_SHOW:
        drawHome();
        break;

    case WIN_EVENT_PAUSE:
        mrc_drawRect(0, 0, SCRW, SCRH, 0xff, 0xff, 0xff);
        mrc_refreshScreen(0, 0, SCRW, SCRH);
        break;

    case WIN_EVENT_UPDATE:
    case WIN_EVENT_REFRESH:
        drawHome();
        break;
    }
}

void homeKeyEvent(int32 data, int32 code, int32 param0, int32 param1)
{
    if (code == MR_KEY_PRESS)
    {
        switch (param0)
        {

        case MR_KEY_8:
        case MR_KEY_VOLUME_DOWN:
        case MR_KEY_DOWN:
            menuIndex++;
            menuIndex %= MENU_NUM;
            break;

        case MR_KEY_2:
        case MR_KEY_VOLUME_UP:
        case MR_KEY_UP:
            menuIndex--;
            menuIndex += MENU_NUM;
            menuIndex %= MENU_NUM;
            break;

        case MR_KEY_SOFTRIGHT:
            mrc_winCloseById(homeWinPtr);
            mrc_exit();
            break;

        case MR_KEY_SOFTLEFT:
        case MR_KEY_SELECT:
        case MR_KEY_5:
            homeMenuClick(menuIndex);
            return;
            break;
        }
        mrc_sendEventTo(homeWinPtr, WIN_EVENT_UPDATE);
    }
}

void homeMenuClick(int32 data)
{
    mrc_printf("�˵���%d\n", data);
    switch (data)
    {
    case 0:
        gameWinPtr = mrc_winNew(0, gameWinEvent, gameKeyEvent);
        return;
        break;
    
    case 1:
        scoreWinPtr = mrc_winNew(0, scoreWinEvent, scoreKeyEvent);
        break;

    case 2:
        infoWinPtr = mrc_winNew(3, infoWinEvent, infoKeyEvent);
        break;

    case 3:
        mrc_winCloseById(homeWinPtr);
        mrc_exit();
        break;
    }
}

void drawHome()
{
    int32 xx, yy, ww, hh, tw, th;
    xx = 1 * (IW + 1) + TW;
    yy = 3 * (IH + 1) + TH;
    ww = IW;
    hh = 30;

    mrc_drawRect(0, 0, SCRW, SCRH, 0xff, 0xff, 0xff);

    for (i = 0; i < MENU_NUM; i++)
    {

        mrc_textWidthHeight(homeMenu[MENU_NUM - 1 - i], FALSE, MR_FONT_BIG, &tw, &th);

        if (menuIndex != MENU_NUM - 1 - i)
        {
            mrc_drawRect(xx, yy - (hh + 4) * i, ww, hh, 80, 80, 80);
        }
        else //ѡ����Ŀ
        {
            mrc_drawRect(xx - 1, yy - (hh + 4) * i - 1, ww + 2, hh + 2, 238, 232, 170);
            mrc_drawRect(xx, yy - (hh + 4) * i, ww, hh, 180, 80, 180);
        }

        mrc_drawText(homeMenu[MENU_NUM - 1 - i], xx + (ww - tw) / 2, yy - (hh + 4) * i + (hh - th) / 2, 255, 255, 255, FALSE, MR_FONT_BIG);
    }
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}
