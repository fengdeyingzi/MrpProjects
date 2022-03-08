#include <base.h>
#include "c/控件/menu.h"
PMenu a;
int init(void)
{
    rectst r;
    r.x=10;r.y=10;r.w=60;r.h=100;
    a=menuCreate(0,0,60,104,4);
    for(int i=0;i<4;i++)
      setMenuItem(a, i,"测试");
    setMenuFocus(a,0);
    menuShow(a);
    ref(0,0,240,320);
    return 0;
}

int event(int type, int p1, int p2)
{
    menuEvent(type,p1,p2);

    return 0;
}

int pause(void)
{
    return 0;
}

int resume(void)
{
    return 0;
}int menuEvent(int type, int p1, int p2)
{
  if(type==KY_UP)
  {
   switch(p1)
   {
     case _UP:
       menuFocusNext(a,0);
       ref(0,0,240,320);
       break;
     case _DOWN:
       menuFocusNext(a,1);
       ref(0,0,240,320);
       break;
     case _SLEFT:
       switch(getMenuFocus(a))
       {
         case 0:
         case 1:
         case 2:
         case 3:
           printf("被点击，焦点id: %d\n",getMenuFocus(a));
         break;
       }
     break;
     case _SRIGHT:
      menuDel(a);exit();
       break;
     }
  }
  return 0;
}