#ifndef _PROGRESSBAR_H_
#define _PROGRESSBAR_H_

#include "base.h"
#include "graphics.h"
#include "ex_math.h"

/*
拖动条控件 ProgressBar

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
int x; //按钮位置x
int y; //按钮位置y
int w; //按钮宽度
int h; //按钮高度
int load; //进度
int h_load; //进度条显示的高度
int max; //seekbar最大值
int r; //拖动按钮半径
int backgroundColor; //背景色
int timer_onstart; //定时器
int timer_onpos;
int timer_onstop;
int draw_mode; //刷新模式
char *onstart; //函数回调
char *onpos;
char *onstop;
} ProgressBar;

//创建拖动条
void* pos_create(int x,int y,int w,int h)
{
 ProgressBar *seekbar = malloc(sizeof(ProgressBar));
 memset(seekbar, 0, sizeof(ProgressBar));
 seekbar->name = "ProgressBar";
 seekbar->x=x;
 seekbar->y=y;
 seekbar->w=w;
 seekbar->h=h;
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
void pos_setid(ProgressBar *seekbar,int id)
{
 seekbar->id=id;
}

//显示控件
void pos_show(ProgressBar *seekbar)
{
 seekbar->isshow = 1;
 if(seekbar->draw_mode==0)
 {
 pos_draw(seekbar);
 ref(seekbar->x, seekbar->y, seekbar->w, seekbar->h);
 }
}
//隐藏控件
void pos_hide(ProgressBar *seekbar)
{
 seekbar->isshow = 0;
}

//绘制控件
void pos_draw(ProgressBar *seekbar)
{
 if(seekbar->isshow)
 {
 //画背景
 if(seekbar->draw_mode==0)
 drawRect(seekbar->x, seekbar->y, seekbar->w, seekbar->h, seekbar->backgroundColor);
 //画进度
 drawRect(seekbar->x, seekbar->y+(seekbar->h-seekbar->h_load)/2, seekbar->w, seekbar->h_load, 0xb0808080);
 //进度
 drawRect(seekbar->x, seekbar->y+(seekbar->h-seekbar->h_load)/2, seekbar->w*seekbar->load/seekbar->max, seekbar->h_load, 0xff7ec0f0);
 }
}

//设置定时器模式
void pos_setref(ProgressBar *seekbar, int mode)
{
 seekbar->draw_mode = mode;
}

//设置背景色 默认白色
void pos_setBackgroundColor(ProgressBar *seekbar, int color)
{
 seekbar->backgroundColor = color;
}

//判断控件是否显示
int pos_isshow(ProgressBar *seekbar)
{
 return seekbar->isshow;
}

//设置进度
void pos_setProgress(ProgressBar *seekbar, int pos)
{
 seekbar->load = pos;
 if(seekbar->draw_mode==0)
 {
  pos_draw(seekbar);
  ref(seekbar->x,seekbar->y, seekbar->w,seekbar->h);
  
 }
}

//获取进度
int pos_getProgress(ProgressBar *seekbar)
{
 return seekbar->load;
}

//设置最大值
void pos_setMax(ProgressBar *seekbar, int max)
{
 seekbar->max = max;
}


//监听滑动开始事件 定时器回调函数
void pos_onStartTrackingTouch(ProgressBar *seekbar, char *onstart)
{
 seekbar->onstart = onstart;
}

//监听滑动事件 定时器回调函数
void pos_onProgressChanged(ProgressBar *seekbar, char *onpos)
{
 seekbar->onpos = onpos;
}
//监听滑动结束事件 定时器回调函数
void pos_onStopTrackingTouch(ProgressBar *seekbar, char *onstop)
{
 seekbar->onstop = onstop;
}

//event事件
int pos_event(ProgressBar *seekbar, int type, int p1, int p2)
{
 //点击坐标点
 int px,py;
 if(seekbar->isshow)
 {
  
 }
 
 return 0;
}

//释放控件
void pos_free(ProgressBar *seekbar)
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