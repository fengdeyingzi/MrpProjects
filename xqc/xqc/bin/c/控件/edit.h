#ifndef _EDIT_H
#define _EDIT_H

/*龙魂噬心(6369)
2012-7-10*/

#include <base.h>
#include "c/控件/type.h"
#define TITMAX 20
#define SPEED 800


#define FOCUSBK 0x00ff0000
#define FOCUSBKW 2

typedef struct
{
int x; int y; int w; int h;
char *title;
char*str;
int32 titleclr;
int32 bkclr;
int32 bgclr;

int max;
int type;
int timeEdit;
int speed;
}Edit;
typedef (Edit*) PEdit;

PEdit EditFocus;
int x,_z;
int focusdia;
/*创建一个编辑框
参数：
@ x，y，w，h：编辑框尺寸
@ title 编辑框标题( <=20)
@ str 编辑框内容
@ titleclr 文字颜色
@ bkclr 边框颜色
@ bgclr 背景颜色
@ max 最大输入字数
@ type编辑框类型：
     0: 全部
     1：字符
     2：密码
返回值：
 0 创建失败
 >0 编辑框指针
*/
void* editCreate(int x, int y, int w, int h, char* title,char* str, int32 titleclr, int32 bkclr, int32 bgclr, int max, int type)
{
PEdit a;
a=malloc(sizeof(Edit)+2);
if(a==NULL) return 0;
a->title=malloc(TITMAX);
memset(a->title,0,TITMAX);
memcpy(a->title,title,TITMAX);
a->str=malloc(max);
if(a->str==NULL) return 0;
memcpy(a->str,str,max);
a->max=max;
a->type = type;
a->x = x;
a->y = y;
a->w =w;
a->h = h;
a->titleclr = titleclr;
a->bkclr = bkclr;
a->bgclr = bgclr;
a->timeEdit=timercreate();
a->speed=SPEED;
return a;

}
/*删除编辑框
参数：
 a：编辑框指针
返回值：无
*/
void editDel(PEdit a)
{
free(a->title);
timerdel(a->timeEdit);
free(a);
a=0;
}

/*设置焦点编辑框
@ a ：编辑框指针，
*当为NULL时则相当于隐藏编辑框
*注意：请保证指针合法性，否则可以引起程序崩溃
返回值：无效
 */

int setEditFocus(PEdit a)
{
int r,g,b;
editDraw(a);
r=MAKER(FOCUSBK);
g=MAKEG(FOCUSBK);
b=MAKEB(FOCUSBK);
EditFocus=a;
drect(a->x+1, a->y, a->w-1, FOCUSBKW,  r, g, b);
drect(a->x+1, a->y+a->h-FOCUSBKW+1,a->w-1, FOCUSBKW,r,g,b);
drect(a->x,a->y+1,FOCUSBKW,a->h-1,r,g,b);
drect(a->x+a->w-FOCUSBKW+1,a->y+1,FOCUSBKW,a->h-1,r,g,b);
/*
 timerstop(EditFocus -> timeEdit);
 EditFocus=a;
x=a->x+2;
if(a==NULL)return 0;
 timerstart(a->timeEdit, a->speed, 0, "_time",1);
*/
 return 0;
}


//得到编辑框内容
char* getEditStr(PEdit a)
{
return a->str;
}

/*绘制编辑框
*/
void editDraw(PEdit a)
{
 int r, g, b;
 int zw,zh;
 rectst rect;
 colorst clr;
 r=MAKER(a->bkclr);g=MAKEG(a->bkclr);b=MAKEB(a->bkclr);
 dline(a->x+1, a->y, a->x+a->w-1, a->y, r, g, b);
 dline(a->x+1, a->y+a->h, a->x+a->w-1, a->y+a->h, r, g, b);
 dline(a->x, a->y+1, a->x, a->y+a->h-1, r, g, b);
 dline(a->x+a->w, a->y+1, a->x+a->w,a->y+a->h-1,r, g, b);
 
 drect(a->x+1, a->y+1, a->w-1, a->h-1, MAKER(a->bgclr), MAKEG(a->bgclr), MAKEB(a->bgclr));

 rect.x=a->x+FOCUSBKW;rect.y=a->y+FOCUSBKW;
 rect.w=a->w-FOCUSBKW*2;rect.h=a->h-FOCUSBKW*2;
 clr.r=MAKER(a->titleclr);
 clr.g=MAKEG(a->titleclr);
 clr.b=MAKEB(a->titleclr);
dtextex(a->str, a->x+FOCUSBKW,a->y+FOCUSBKW, &rect, &clr, 02,FONT);
}

//焦点编辑框光标回调(函数暂时无用
void _editTime(int a)
{
_z=!_z;
if(_z)
dline(x,EditFocus->y+2,x,EditFocus->y+EditFocus->h-2,MAKER(EditFocus->bgclr),MAKEG(EditFocus->bgclr),MAKEB(EditFocus->bgclr));
else 
dline(x,EditFocus->y+2,x,EditFocus->y+EditFocus->h-2,MAKER(EditFocus->titleclr),MAKEG(EditFocus->titleclr),MAKEB(EditFocus->titleclr));
ref(x,EditFocus->y,1,EditFocus->h);
}


//焦点编辑框事件处理。
int editEvent(int msg,int p1,int p2)
{
 if(KY_UP==msg)
 {
  switch(p1)
  {
   case _SELECT:
   focusdia=editcreate(gb_uni(EditFocus->title,0),gb_uni(EditFocus->str,0),EditFocus->type,EditFocus->max);
    break;
   
  }  
 }
//对编辑事件处理
  if(msg == MR_DIALOG)
 {
  switch(p1)
  {
   case DLG_OK:
   char* p;
  p=  editget(focusdia);
memset(EditFocus->str,0,EditFocus->max);
memcpy(EditFocus->str,gb_uni(p,1),EditFocus->max);
    editdel(focusdia);
    break;
   case DLG_CANCEL:
    editdel(focusdia);
  }
 return 1;//请注意，返回值为1，可判断是否为1来重绘界面
 }
return 0;
}

#endif
