
#ifndef __SCREEN_ADAPTER__
#define __SCREEN_ADAPTER__

#include <mrc_base.h>
#include "mrc_graphics.h"

//设置屏幕宽高
void sc_setScreenSize(int32 width, int32 height);

//读取图片 并自动调整图片宽高到适配屏幕的大小
BITMAP_565* sc_readBitmapFromAssets(const char* filename);

//绘制一张图片
void sc_drawBitmap(BITMAP_565 *bmp, int32 x, int32 y);

//绘制文字
int32 sc_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode);

//绘制矩形
void sc_drawRect(int32 x,int32 y,int32 w,int32 h,uint32 color);

//画线
void sc_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);

int32 sc_getX(int32 x);
int32 sc_getY(int32 y);

#endif