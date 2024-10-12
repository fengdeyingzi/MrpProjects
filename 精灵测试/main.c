#include <mrc_base.h>
#include "mrc_graphics.h"
#include "mpc.h"
#include "uc3_font.h"
#include "logo.h"
#include "spritex.h"

#define MAX_ROW 20
#define MAX_COL 20
#define true 1
#define false 0
typedef int bool;
SpriteX *spx;
int32 timer;

// 函数声明
void drawGame(void);

void logoc(int32 data)
{
    drawGame();
}
int32 game_init(void)
{

    drawGame();
    return 0;
}

// 应用入口函数
int32 mrc_init()
{

    mpc_init();
    uc3_init();

    spx = createSpriteXFromAssets("a.sprite", "a.bma");
    addSpxToManage(spx);
    setVisible(spx, 1);
    update(spx, 100);
    setAction(spx, 0);
    setPosition(spx, SCRW / 2, SCRH / 2);
    timer = timercreate();
    mrc_timerStart(timer, 50, 1, logoc, 1);
    mrc_event(LOGO_EVENT, 0, 0);
    return 0;
}

//
void drawGame(void)
{
    cls(240, 240, 240);
    uc3_drawText("精灵测试,按数字0-9切换动作", 10, 10, 20, 20, 20, 0);
    setPosition(spx, SCRW / 2, SCRH / 2);
    

    paint(spx, spx->image, SCRW / 2, SCRH / 2);
    update(spx, getuptime());
    ref(0, 0, SCRW, SCRH);
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    if (code == KY_UP)
    { // 检测是否有按键被按下
        switch (param0)
        {
        case _0:
            setAction(spx, 0);
            mrc_printf("setAction 0");
            break;
        case _1:
            setAction(spx, 1);
            mrc_printf("setAction 1");
            break;
        case _2:
            setAction(spx, 2);
            mrc_printf("setAction 2");
            break;
        case _3:
            setAction(spx, 3);
            mrc_printf("setAction 3");
            break;
        case _4:
            setAction(spx, 4);
            mrc_printf("setAction 4");
            break;
        case _5:
            setAction(spx, 5);
            mrc_printf("setAction 5");
            break;
        case _6:
            setAction(spx, 6);
            mrc_printf("setAction 6");
            break;

        case _SRIGHT: // Quit game
            mrc_exit();
        }
    }

    return 0;
}

int32 mrc_pause(void)
{
    return 0;
}

int32 mrc_resume(void)
{
    return 0;
}

int32 mrc_exitApp()
{
    mrc_printf("释放uc3字体");
    uc3_free();
    mrc_printf("释放spriteX");
    removeAllSpx();
    mrc_printf("释放timer");
    mrc_timerStop(timer);
    mrc_timerDelete(timer);
    mrc_printf("释放完成");
    return 0;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5)
{
    return 0;
}