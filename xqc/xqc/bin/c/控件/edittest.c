#include <base.h>
#include "c/�ؼ�/edit.h"
PEdit a;
int init(void)
{
a= editCreate(10,20,220,220,"����","��ʾ��Χ�Ƶ�·�����Ļ�˵����ʾ��Χ",0x00ffffff,0x00ffff00,0x00ff0000,255,0);
setEditFocus(a);
//editDraw(a);
ref(0,0,240,320);
//editDel(a);
    return 0;
}

int event(int type, int p1, int p2)
{//ע�ⷵ�ص���1��Ϊ�˳����ر༭��
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