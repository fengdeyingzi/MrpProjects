#ifndef _BITMAP_H_
#define _BITMAP_H_
#include <graphics.h>

//获取bitmap宽度
int bitmap_getw(int bitmap)
{
 int w=0;
 BITMAPINFO *info= malloc(sizeof(BITMAPINFO));
 bitmapGetInfo(bitmap,info);
 w=info->width;
 return w;
}

//获取bitmap高度
int bitmap_geth(int bitmap)
{
 int h=0;
 BITMAPINFO *info= malloc(sizeof(BITMAPINFO));
 bitmapGetInfo(bitmap,info);
 h=info->height;
 return h;
}



#endif
