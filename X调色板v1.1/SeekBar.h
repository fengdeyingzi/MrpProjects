#ifndef _SEEKBAR_H_
#define _SEEKBAR_H_

#include "base.h"
#include "graphics.h"
#include "ex_math.h"

/*
拖动条控件 SeekBar


*/

typedef struct
{
  char *name; //控件名字
int id; //控件id
int isshow; //是否显示 不显示时将不会执行点击事件
int isdown; //是否按下
int x; //按钮位置x
int y; //按钮位置y
int w; //按钮宽度
int h; //按钮高度
int load; //进度
int h_load; //进度条显示的高度
int max; //seekbar最大值
int r; //拖动按钮半径
int color; //控件颜色
int backgroundColor; //背景色
int timer_onstart; //定时器
int timer_onpos;
int timer_onstop;
int draw_mode; //刷新模式
char *onstart; //函数回调
char *onpos;
char *onstop;
} SeekBar;

//创建拖动条
void *seek_create(int x,int y,int w,int h)
{
 SeekBar *seekbar = malloc(sizeof(SeekBar));
 memset(seekbar, 0, sizeof(SeekBar));
 seekbar->name = "SeekBar";
 seekbar->x=x;
 seekbar->y=y;
 seekbar->w=w;
 seekbar->h=h;
 seekbar->color= 0xff60aef0;
 seekbar->isshow = 1;
 seekbar->max = 100;
 seekbar->backgroundColor = 0xffffffff;
 seekbar->timer_onstart = timercreate();
 seekbar->timer_onpos = timercreate();
 seekbar->timer_onstop = timercreate();
 int dev_w = 0;
 if(SCRW<SCRH)
 dev_w = SCRW;
 else
 dev_w = SCRH;
 seekbar->r = dev_w /20;
 seekbar->h_load = dev_w /100;
 if(seekbar->r>h) seekbar->r=h;
 
 return seekbar;
}

//设置控件id
void seek_setid(SeekBar *seekbar,int id)
{
 seekbar->id=id;
}

//显示控件
void seek_show(SeekBar *seekbar)
{
 seekbar->isshow = 1;
 if(seekbar->draw_mode==0)
 {
 seek_draw(seekbar);
 ref(seekbar->x, seekbar->y, seekbar->w, seekbar->h);
 }
}
//隐藏控件
void seek_hide(SeekBar *seekbar)
{
 seekbar->isshow = 0;
}

//绘制控件
void seek_draw(SeekBar *seekbar)
{
 if(seekbar->isshow)
 {
 //画背景
 if(seekbar->draw_mode==0)
 drawRect(seekbar->x, seekbar->y, seekbar->w, seekbar->h, seekbar->backgroundColor);
 //画进度
 drawRect(seekbar->x+seekbar->r, seekbar->y+(seekbar->h-seekbar->h_load)/2, seekbar->w-seekbar->r*2, seekbar->h_load, 0xa0a0a0a0);
 drawRect(seekbar->x+seekbar->r, seekbar->y+(seekbar->h-seekbar->h_load)/2, (seekbar->w-seekbar->r*2)*seekbar->load/seekbar->max, seekbar->h_load, seekbar->color);
 //画圆形按钮
 int cir_x,cir_y;
 cir_x = seekbar->x+seekbar->r+ (seekbar->w-seekbar->r*2) * seekbar->load / seekbar->max;
 cir_y = seekbar->y + seekbar->h/2;
 if(seekbar->isdown)
 {
 
 drawCir(cir_x,cir_y,seekbar->r, seekbar->color&0xffffff | 0x40000000);
 drawCir(cir_x,cir_y,seekbar->r*7/12, seekbar->color);
 }
 else
 drawCir(cir_x,cir_y,seekbar->r*13/24, seekbar->color);
 }
}

//设置定时器模式
void seek_setref(SeekBar *seekbar, int mode)
{
 seekbar->draw_mode = mode;
}

//设置控件颜色
void seek_setColor(SeekBar *seekbar,int color)
{
 seekbar->color = color;
}

//设置背景色 默认白色
void seek_setBackgroundColor(SeekBar *seekbar, int color)
{
 seekbar->backgroundColor = color;
}

//判断控件是否显示
int seek_isshow(SeekBar *seekbar)
{
 return seekbar->isshow;
}

//设置进度
void seek_setProgress(SeekBar *seekbar, int pos)
{
 seekbar->load = pos;
 if(seekbar->draw_mode==0)
 {
  seek_draw(seekbar);
  ref(seekbar->x,seekbar->y, seekbar->w,seekbar->h);
  
 }
}

//获取进度
int seek_getProgress(SeekBar *seekbar)
{
 return seekbar->load;
}

//设置最大值
void seek_setMax(SeekBar *seekbar, int max)
{
 seekbar->max = max;
}


//监听滑动开始事件 定时器回调函数
void seek_onStartTrackingTouch(SeekBar *seekbar, char *onstart)
{
 seekbar->onstart = onstart;
}

//监听滑动事件 定时器回调函数
void seek_onProgressChanged(SeekBar *seekbar, char *onpos)
{
 seekbar->onpos = onpos;
}
//监听滑动结束事件 定时器回调函数
void seek_onStopTrackingTouch(SeekBar *seekbar, char *onstop)
{
 seekbar->onstop = onstop;
}

//event事件
int seek_event(SeekBar *seekbar, int type, int p1, int p2)
{
 //点击坐标点
 int px,py;
 int loadx = seekbar->load;
 if(seekbar->isshow)
 {
  if(type==MS_DOWN)
  {
   //根据进度算出圆形按钮坐标
   px = seekbar->x+seekbar->r+ (seekbar->w-seekbar->r*2)* seekbar->load/seekbar->max;
   py = seekbar->y+ seekbar->h/2;
   printf("距离：%d\n", getLineSize(p1,p2,px,py));
   if(getLineSize(p1,p2, px,py)<=seekbar->r)
   {
    seekbar->isdown = 1;
    if(seekbar->onstart!=NULL)
    timerstart(seekbar->timer_onstart, 10, seekbar->id, seekbar->onstart,0);
    if(seekbar->draw_mode == 0)
    {
     seek_draw(seekbar);
     ref(seekbar->x, seekbar->y, seekbar->w, seekbar->h);
    }
   }
   
  }
  
  if(type == MS_MOVE)
  {
   if(seekbar->isdown)
   {
    //根据屏幕坐标算出seekbar进度
    int load = seekbar->max * (p1 - (seekbar->x+seekbar->r)) / (seekbar->w-seekbar->r*2);
    //设置进度
    if(load>seekbar->max)load = seekbar->max;
    if(load<0)load=0;
    seek_setProgress(seekbar, load);
    if(seekbar->onpos!=NULL)
    timerstart(seekbar->timer_onpos, 10, seekbar->id, seekbar->onpos,0);
    if(seekbar->draw_mode == 0)
    {
     if(seekbar->load!=loadx)
     {
     seek_draw(seekbar);
     ref(seekbar->x, seekbar->y, seekbar->w, seekbar->h);
     }
    }
   }
   
  }
  if(type == MS_UP)
  {
   if(seekbar->isdown)
   {
    seekbar->isdown = 0;
    //根据屏幕坐标算出seekbar进度
    int load = seekbar->max * (p1 - (seekbar->x+seekbar->r)) /(seekbar->w-seekbar->r*2);
    if(load>seekbar->max)load = seekbar->max;
    if(load<0)load=0;
    //设置进度
    seek_setProgress(seekbar, load);
    if(seekbar->onstop!=NULL)
    timerstart(seekbar->timer_onstop, 10, seekbar->id, seekbar->onstop,0);
    if(seekbar->draw_mode == 0)
    {
     seek_draw(seekbar);
     ref(seekbar->x, seekbar->y, seekbar->w, seekbar->h);
    }
   }
  }
 }
 
 return 0;
}

//释放控件
void seek_free(SeekBar *seekbar)
{
 if(seekbar->timer_onstart)
 {
 timerstop(seekbar->timer_onstart);
 timerdel(seekbar->timer_onstart);
 seekbar->timer_onstart=0;
 }
 
 if(seekbar->timer_onpos)
 {
 timerstop(seekbar->timer_onpos);
 timerdel(seekbar->timer_onpos);
 seekbar->timer_onpos=0;
 }
 if(seekbar->timer_onstop)
 {
 timerstop(seekbar->timer_onstop);
 timerdel(seekbar->timer_onstop);
 seekbar->timer_onstop=0;
 }
 
 free(seekbar);
}










#endif