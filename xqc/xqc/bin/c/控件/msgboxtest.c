#include <base.h>
#define FONT 1//如果定义，请注释type.h的字体定义
#include "c/控件/msgbox.h"
int init(void)
{
cls(250,250,250);   drawTitleBar(0,0,240,25,0xff0000,0x000000,0xffffff,"测试标题") ;
drawToolBar(0,295,240,25,0xff0000,0x000000,0xffffff,"菜单","确定","退出");
ref(0,0,240,320);
showMsgBox(50,50,"测试");
    return 0;
}

int event(int type, int p1, int p2)
{
    if(KY_DOWN == type)
    {
        switch(p1)
        {
        case _SRIGHT:
            exit();
            break;
        case _1:
            break;
        }
    }

    return 0;
}

int pause(void)
{
    return 0;
}

int resume(void)
{
    return 0;
}