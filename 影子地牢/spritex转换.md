我需要将j2me平台的java代码转换为C语言代码,使用c99标准,支持以下绘图函数(mrc_graphics.h):
```c
#ifndef _MRC_GRAPHICS_H_
#define _MRC_GRAPHICS_H_

typedef struct {
 uint16* bitmap;
 int width; //
 int height;
 int color_bit; //颜色位数 默认16
 int transcolor; //透明色 默认0xffff
 int mode; //绘制模式 默认BM_COPY
 int32 buflen; //缓存区宽高
 int32 *memcache; //memcpy绘图优化缓存 存放dst src count(共12字节)
 int32 memlen; //memcpy绘图优化缓存
} BITMAP_565;

#define TRANS_NONE          0 //无变换
#define TRANS_ROT90        5 //顺时针旋转90度
#define TRANS_ROT180       3 //顺时针旋转180度
#define TRANS_ROT270       6 //顺时针旋转270度
#define TRANS_MIRROR       2 //水平镜像
#define TRANS_MIRROR_ROT90 7 //水平镜像后再顺时针旋转90度
#define TRANS_MIRROR_ROT180 1 //水平镜像后再顺时针旋转180度
#define TRANS_MIRROR_ROT270 4 //水平镜像后再顺时针旋转270度


#define GRAPHICS_LEFT 3
#define GRAPHICS_TOP 48
#define GRAPHICS_RIGHT 5
#define GRAPHICS_CENTER 17
#define GRAPHICS_BOTTOM 80

// 从内存卡读取bitmap
extern BITMAP_565 *readBitmap(char *filename);

// 绘制bitmap 使用方法等同于j2me的drawRegion
extern void drawBitmapRegion(BITMAP_565 *src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor);

// 释放bitmap
extern int32 bitmapFree(BITMAP_565 *b);

#endif
```
支持以下获取时间函数:
```
/*
获取系统时间，单位毫秒。系统时间可以以Mythroad平
台引擎启动之前的任意时刻为起始时间，返回从起
始时间到目前经过的毫秒数。例如这个函数可能返
回的是系统启动后经过的毫秒数。
返回:
     单位毫秒的系统时间
*/
int32 mrc_getUptime(void);
```
下面我给出代码,请帮我转换