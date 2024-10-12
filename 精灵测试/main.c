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
int transform;
BITMAP_565 *bitmap;

// ��������
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

// Ӧ����ں���
int32 mrc_init()
{

    mpc_init();
    
    uc3_init();

    spx = createSpriteXFromAssets("a.sprite", "a.bma");
    bitmap = spx->image;
    transform = TRANS_NONE;
    
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
    int32 dd;
    int i;
    cls(240, 240, 240);
    uc3_drawText("�������,������0-9�л�����", 10, 10, 20, 20, 20, 0);
    setPosition(spx, SCRW / 2, SCRH / 2);
    
    dd = getuptime();

    paint(spx, spx->image, SCRW / 2, SCRH / 2);
    paint(spx, spx->image, SCRW / 2, SCRH / 2+100);
    paint(spx, spx->image, SCRW / 2, SCRH / 2-100);
    // drawBitmapRegion(bitmap, 0, 0, 100, 30, transform, SCRW/2, SCRW/2,GRAPHICS_LEFT|GRAPHICS_TOP);
    update(spx, getuptime());
    
    mrc_printf("time = %d", getuptime() - dd);

    ref(0, 0, SCRW, SCRH);


}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    
    int cur_action = 0;
    if(code == MS_UP){
        cur_action = spx->actionIndex;
        cur_action++;
        if(cur_action >= spx->actionCount){
            cur_action = 0;
        }
        setAction(spx, cur_action);
        
    }
    if (code == KY_UP)
    { // ����Ƿ��а���������
        switch (param0)
        {
        case _0:
            setAction(spx, 0);
            mrc_printf("setAction 0");
            transform = TRANS_NONE;
            break;
        case _1:
            setAction(spx, 1);
            mrc_printf("setAction 1");
            transform = TRANS_ROT90;
            break;
        case _2:
            setAction(spx, 2);
            mrc_printf("setAction 2");
            transform = TRANS_ROT180;
            break;
        case _3:
            setAction(spx, 3);
            mrc_printf("setAction 3");
            transform = TRANS_ROT270;
            break;
        case _4:
            setAction(spx, 4);
            mrc_printf("setAction 4");
            transform = TRANS_MIRROR;
            break;
        case _5:
            setAction(spx, 5);
            mrc_printf("setAction 5");
            transform = TRANS_MIRROR_ROT90;
            break;
        case _6:
            setAction(spx, 6);
            mrc_printf("setAction 6");
            transform = TRANS_MIRROR_ROT180;
            break;
        case _7:
        transform = TRANS_MIRROR_ROT270;
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
    mrc_printf("�ͷ�uc3����");
    uc3_free();
    mrc_printf("�ͷ�spriteX");
    removeAllSpx();
    mrc_printf("�ͷ�timer");
    mrc_timerStop(timer);
    mrc_timerDelete(timer);
    mrc_printf("�ͷ����");
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