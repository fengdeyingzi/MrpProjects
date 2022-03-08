#include <base.h>
#include "c/控件/edit.h"
PEdit a;
int init(void)
{
a= editCreate(10,20,220,220,"测试","的示范围绕道路上述的话说明显示范围",0x00ffffff,0x00ffff00,0x00ff0000,255,0);
setEditFocus(a);
//editDraw(a);
ref(0,0,240,320);
//editDel(a);
    return 0;
}

int event(int type, int p1, int p2)
{//注意返回的是1，为退出本地编辑框
if(1==editEvent(type,p1,p2))
{cls(0,0,0);setEditFocus(a);ref(0,0,240,320);}
    if(KY_DOWN == type)
    {
        switch(p1)
        {
        case _SRIGHT:
            editDel(a);exit();
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