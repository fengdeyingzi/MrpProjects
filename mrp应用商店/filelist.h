
#ifndef __FILELIST__
#define __FILELIST__
/*
XGUI
基础的View控件
支持设置颜色 设置点击事件

影子俱乐部网站：www.yzjlb.net
手机C论坛：bbs.yzjlb.net
手机C交流QQ群：606757263

*/
#include <mrc_base.h>
#include "mpc.h"
typedef struct{
    char *path;
    char DisplayName[25]; //28:52
    char Desc[65]; //128:192
} MRPHEADER;

typedef struct {
  char *name;           //控件名字
  int id;               //控件id
  int isshow;           //是否显示 不显示时将不会执行点击事件
  int isdown;           //是否按下
  int x;                //位置x
  int y;                //位置y
  int w;                //宽度
  int h;                //高度
  int backgroundColor;  //背景色
  int draw_mode;        //刷新模式
  int32 timer;          //定时器句柄
  void *onclick;        //点击事件回调函数
    MRPHEADER **list; //list列表
    int list_windowsize; //一个窗口容纳的item数
    int list_max; //列表最大长度
    int list_len; //当前列表长度
    int list_index; //光标位置
    int list_progress; //list显示进度 非百分比 根据len来显示进度

    //以下是mrp信息
    
} ListView;

void *list_create(int x, int y, int w, int h);
void list_setid(ListView *view, int id);
int list_getid(ListView *view);
void list_draw(ListView *view);
void list_setMode(ListView *view, int mode) ;
void list_show(ListView *view);
void list_hide(ListView *view);
int list_isshow(ListView *view);
void list_setBackgroundColor(ListView *view, int color);
void list_setonclick(ListView *view, void *click);
void list_add(ListView *view, char *filename);
void list_find_txt(ListView *view, char *file);
int list_event(ListView *view, int32 type, int32 p1, int32 p2);
void list_free(ListView *view);
MRPHEADER *list_getCurItem(ListView *list);

#endif
