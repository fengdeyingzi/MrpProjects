#ifndef _TEXTVIEW_H_
#define _TEXTVIEW_H_
#include "base.h"
#include "graphics.h"
#include "ex_math.h"
#include "coding.h"

/*
文本框控件

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
int padding; //边距间隔
int tx;
int ty;//文字xy坐标
int textColor; //文字颜色
int backgroundColor; //背景色
char *text;
int draw_mode; //刷新模式
int timer; //定时器句柄
int font; // 字体大小 取值：0 1 2
char *onclick; //点击事件回调函数
} TextView;


//创建控件
void *textview_create(int x,int y,int w,int h)
{
 TextView *view = malloc(sizeof(TextView));
 memset(view,0,sizeof(TextView));
 view->x = x;
 view->y = y;
 view->w = w;
 view->h = h;
 view->tx=x;
 view->ty=y;
 view->isshow=1;
 view->backgroundColor = 0x00000000;
 view->font = 1;
 view->timer = timercreate();
 return view;
}

//设置文字 gb2312编码
void textview_setText(TextView *view, char *text)
{
 if(view->text!=NULL)
 {
 free(view->text);
 view->text = NULL;
 }
 
 view->text = gbToUn(text);
 
}

//设置文本颜色 argb格式
void textview_setTextColor(TextView *view, int color)
{
 view->textColor = color;
}

//设置边距
void textview_setpadding(TextView *view, int padding)
{
 view->padding = padding;
 view->tx = view->x+padding;
 view->ty = view->y+padding;
}

//设置id
void textview_setid(TextView *view,int id)
{
 view->id=id;
}

//获取id
int textview_getid(TextView *view)
{
 return view->id;
}

//绘制控件
void textview_draw(TextView *view)
{
 rectst rect;
colorst color;
rect.x= view->x+view->padding;
rect.y= view->y+view->padding;
rect.w= view->w-view->padding*2;
rect.h= view->h-view->padding*2;
color.r=(view->textColor>>16)&0xff;
color.g=(view->textColor>>8)&0xff;
color.b=(view->textColor)&0xff;

 if(view->isshow)
 {
  drawRect(view->x, view->y, view->w, view->h, view->backgroundColor);
  if(view->text!=NULL)
  dtextex(view->text,view->tx,view->ty,&rect,&color,1|2,view->font);
 }
}

//显示
void textview_show(TextView *view)
{
 view->isshow = 1;
 if(view->draw_mode==0)
 {
  view_draw(view);
  ref(view->x,view->y,view->w,view->h);
 }
}

//隐藏
void textview_hide(TextView *view)
{
 view->isshow = 0;
}

//判断是否显示
int textview_isshow(TextView *view)
{
 return view->isshow;
}

//设置背景色 argb格式
void textview_setBackgroundColor(TextView *view, int color)
{
 view->backgroundColor = color;
}

//设置点击事件
void textview_setonclick(TextView *view, char *click)
{
 view->onclick = click;
}

//控件event事件
int textview_event(TextView *view, int type, int p1, int p2)
{
 if(view->isshow)
 {
  if(type == MS_DOWN)
  {
   if(isPointCollRect(p1,p2,view->x,view->y,view->w,view->h))
   {
    view->isdown = 1;
   }
   
  }
  else if(type == MS_UP)
  {
   if(view->isdown)
   {
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
void textview_free(TextView *view)
{
 if(view->timer)
 {
  timerstop(view->timer);
  timerdel(view->timer);
  view->timer = 0;
 }
 if(view->text!=NULL)
 free(view->text);
 free(view);
 
}












#endif

