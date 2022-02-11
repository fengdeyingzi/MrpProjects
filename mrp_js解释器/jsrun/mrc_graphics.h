#ifndef _MRC_GRAPHICS_H_
#define _MRC_GRAPHICS_H_


#include "xl_bmp.h"
enum
{
_JPG=0,
_PNG=1,
_BMP16=2
};


typedef struct
{
uint32 width; //宽度
uint32 height; //高度
uint32 stride;
int32 format; //格式
uint32 flags; // 0 for now
void *ptr; //缓存
} BITMAPINFO;

extern BITMAP_565* readBitmap565(char *filename);
extern int32 drawBitmap565Flip(BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);
extern void drawBitmap565(BITMAP_565 *b,int32 x,int32 y);
extern int32 bitmap565Free(BITMAP_565 *b);
extern BITMAP_565 *readBitmap565FromAssets(char *filename);

void gl_drawRect(int32 x,int32 y,int32 w,int32 h,uint32 color);
void gl_drawCir(int32 x,int32 y,int32 r,uint32 color);
void drawBitmap565Ex(BITMAP_565* bmp, int32 x,int32 y,int32 w,int32 h, int32 tx, int32 ty,int32 tw,int32 th);
int32 bitmap565getInfo(BITMAP_565* bmp, BITMAPINFO *info);

#define readBitmap(name) (int32)readBitmap565(name)
#define drawBitmapFlip(buf,x,y,w,h,sx,sy) drawBitmap565Flip((BITMAP_565*)buf,x,y,w,h,sx,sy)
#define drawBitmap(b,x,y) drawBitmap565((BITMAP_565*)b,x,y)
#define bitmapFree(b) bitmap565Free((BITMAP_565*)b)
#define readBitmapFromAssets(filename) (int32)readBitmap565FromAssets(filename)
#define drawRect gl_drawRect
// #define drawCir gl_drawCir
void drawCircle(int x, int y, int radius, unsigned int color);
#define drawBitmapEx(bmp,x,y,w,h,tx,ty,tw,th) drawBitmap565Ex((BITMAP_565*)bmp,x,y,w,h,tx,ty,tw,th)
#define bitmapGetInfo(bmp,info) bitmap565getInfo((BITMAP_565*)bmp,info)

#endif