// 游戏进行中画面
#include "win_game.h"
#include "win_info.h"

#include "mrc_object.h"
#include "mrc_tool.h"

#include "mrc_spritex.h"

#include "popstar\list.h"
#include "popstar\star.h"

int32 gameTimer;
#define GAME_SPEED 30
#define STEP_SPEED 3
#define IMAGE_SPEED 3

_STARS *star;

void gameTimerCb(int32 data)
{
    star_run(star);

    mrc_sendEventTo(gameWinPtr, WIN_EVENT_UPDATE);
}

// 游戏消息
void gameWinEvent(int32 data, int32 eventId)
{
    switch (eventId)
    {
    case WIN_EVENT_SHOW:
        initGame();
        showGame();
        break;

    // 显示子页面，暂停游戏
    case WIN_EVENT_PAUSE:
        mrc_timerStop(gameTimer);
        break;

    // 子页面关闭，继续游戏
    case WIN_EVENT_REFRESH:
        mrc_timerStart(gameTimer, GAME_SPEED, 0, gameTimerCb, TRUE);

        break;

    // 更新页面
    case WIN_EVENT_UPDATE:
        showGame();
        break;

    // 关闭页面
    case WIN_EVENT_EXIT:
        mrc_timerStop(gameTimer);
        mrc_timerDelete(gameTimer);

        break;
    }
}

// 按键消息
void gameKeyEvent(int32 data, int32 code, int32 param0, int32 param1)
{
    star_event(star, code, param0, param1);

    if (code == MR_KEY_PRESS)
    {
        switch (param0)
        {
        case MR_KEY_SOFTRIGHT:
            mrc_winCloseById(gameWinPtr);
            break;

        case MR_KEY_STAR:
        case MR_KEY_9:
            BBOX_DEBUG = !BBOX_DEBUG;
            break;
        }
    }

    mrc_sendEventTo(gameWinPtr, WIN_EVENT_UPDATE);
}

void showGame()
{
    int32 fps, blood, win, px, py;
    char temp[100];
    mrc_clearScreen(0x80, 0x80, 0x80);

    win = mrc_getActiveWinId();
    if (win != gameWinPtr)
        return;
    runFpsTool();

    star_draw(star);
    mrc_drawText(star->view_fen, 0, 0, 255, 0, 0, FALSE, MR_FONT_SMALL);

    fps = getMainFps();
    runMem = mrc_getMemoryRemain();
    mrc_sprintf(temp, "fps:%d, mem:%dk", fps, (startMem - runMem) / 1024);
    mrc_drawText(temp, 0, 240, 255, 0, 0, FALSE, MR_FONT_SMALL);
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

void initGame()
{
    initFpsTool(GAME_SPEED);

    star = star_create_withscr(32, 32, SCRW - 64, SCRH - 64);

    gameTimer = mrc_timerCreate();
    mrc_timerStart(gameTimer, GAME_SPEED, 0, gameTimerCb, TRUE);

    mrc_printf("initGame Over!");
}
