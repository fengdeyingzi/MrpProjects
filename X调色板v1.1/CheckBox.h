#ifndef _VIEW_H_
#define _VIEW_H_
#include "base.h"
#include "graphics.h"
#include "bitmap.h"
#include "ex_math.h"
/*
CheckBox控件

风的影子
影子俱乐部网站：www.yzjlb.net
手机C论坛：bbs.yzjlb.net
手机C交流QQ群：370468001

*/

typedef struct
{
  char *name; //控件名字
int id; //控件id
int isshow; //是否显示 不显示时将不会执行点击事件
int isdown; //是否按下
int x; //位置x
int y; //位置y
int w; //宽度
int h; //高度
int tx; //图片坐标
int ty;
int isCheck;
int bitmap_unCheck; //未选中的图片
int bitmap_isCheck; //选中的图片
int backgroundColor; //背景色
int draw_mode; //刷新模式
int timer; //定时器句柄
char *onclick; //点击事件回调函数
} CheckBox;


//创建控件
void *check_create(int x,int y,int w,int h)
{
 CheckBox *view = malloc(sizeof(CheckBox));
 memset(view,0,sizeof(CheckBox));
 view->x = x;
 view->y = y;
 view->w = w;
 view->h = h;
 view->isshow=1;
 view->backgroundColor = 0x00000000;
 view->bitmap_unCheck= readBitmapFromAssets("check/Box-0.png");
 view->bitmap_isCheck= readBitmapFromAssets("check/Box-1.png");
 view->tx = view->x + (view->w-bitmap_getw(view->bitmap_unCheck))/2;
 view->ty = view->y + (view->h-bitmap_geth(view->bitmap_unCheck))/2;
 view->timer = timercreate();
 return view;
}

//设置id
void check_setid(CheckBox *view,int id)
{
 view->id=id;
}

//获取id
int check_getid(CheckBox *view)
{
 return view->id;
}

//绘制控件
void check_draw(CheckBox *view)
{
 int tw,th;
 tw = bitmap_getw(view->bitmap_unCheck);
 th = bitmap_geth(view->bitmap_isCheck);
 if(view->isshow)
 {
  drawRect(view->x, view->y, view->w, view->h, view->backgroundColor);
  if(view->isdown)
  {
   drawRect(view->tx, view->ty, tw, th, 0x40cecece);
  }
  drawBitmap(view->bitmap_unCheck, view->tx, view->ty);
  if(view->isCheck)
  {
   drawBitmap(view->bitmap_isCheck, view->tx, view->ty);
  }
  else
  {
   
  }
  
  
 }
}

//显示
void check_show(CheckBox *view)
{
 view->isshow = 1;
 if(view->draw_mode==0)
 {
  view_draw(view);
  ref(view->x,view->y,view->w,view->h);
 }
}

//隐藏
void check_hide(CheckBox *view)
{
 view->isshow = 0;
}

//判断是否显示
int check_isshow(CheckBox *view)
{
 return view->isshow;
}

//设置背景色 argb格式
void check_setBackgroundColor(CheckBox *view, int color)
{
 view->backgroundColor = color;
}

//切换
void check_switch(CheckBox *check)
{
 if(check->isCheck)
   check->isCheck = 0;
 else
   check->isCheck = 1;
 if(check->draw_mode==0)
 {
  check_draw(check);
  ref(check->x,check->y,check->w,check->h);
 }
}

//设置点击事件
void check_setonclick(CheckBox *view, char *click)
{
 view->onclick = click;
}

//控件event事件
int check_event(CheckBox *view, int type, int p1, int p2)
{
 if(view->isshow)
 {
  if(type == MS_DOWN)
  {
   if(isPointCollRect(p1,p2,view->x,view->y,view->w,view->h))
   {
    view->isdown = 1;
    if(view->draw_mode==0)
    {
     
     check_draw(view);
     ref(view->x,view->y,view->w,view->h);
    }
   }
   
  }
  else if(type == MS_UP)
  {
   if(view->isdown)
   {
    check_switch(view);
    view->isdown = 0;
    if(view->onclick != NULL)
    {
     timerstart(view->timer, 10, view->id, view->onclick, 0);
    }
   }
  }
  
 }
 
 return 0;
}




//释放内存
void check_free(CheckBox *view)
{
 if(view->timer)
 {
  timerstop(view->timer);
  timerdel(view->timer);
  view->timer = 0;
 }
 
 if(view->bitmap_unCheck)
 {
  bitmapFree(view->bitmap_unCheck);
  view->bitmap_unCheck = 0;
 }
 if(view->bitmap_isCheck)
 {
  bitmapFree(view->bitmap_isCheck);
  view->bitmap_isCheck = 0;
 }
 
 
 free(view);
 
}












#endif
