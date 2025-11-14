#include "mrc_graphics.h"

//将图片src绘制到图片dst中,并指定绘制的透明度
//两张图片的宽高数据必须一致
void drawBitmapFormAlpha(BITMAP_565 *dst, BITMAP_565 *src, int alpha);
//打开磁盘缓存
//打开模式0重置 1添加
void openFSBitmapCache();
//将bitmap保存到磁盘缓存中(只保存字节数据),并返回缓存位置offset
int32 saveBitmapCache(BITMAP_565 *bitmap);
//从磁盘缓存指定位置读取到bitmap
int32 readBitmapFormCache(BITMAP_565 *bitmap, int32 offset);
//关闭缓存
void closeFSBitmapCache();

