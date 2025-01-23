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

typedef struct{
    int16 A; // A, B, C, and D are fixed point values with an 8-bit integer part
    int16 B; // and an 8-bit fractional part.
    int16 C;
    int16 D;
    uint16 rop;
}mr_transMatrixSt;

enum
{
	SHADE_UPDOWN,		//从上到下
	SHADE_LEFTRIGHT,	//从左到右
	SHADE_DOWNUP,		//从下到上
	SHADE_RIGHTLEFT		//从右到左
};
//混合两个颜色
uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha);
//从内存卡读取bitmap
extern BITMAP_565* readBitmap565(char *filename);
//创建一个bitmap
BITMAP_565 *createBitmap565(int width, int height);
//绘制bitmap指定区域
extern int32 drawBitmap565Flip(BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);
//绘制bitmap
extern void drawBitmap565(BITMAP_565 *b,int32 x,int32 y);
//释放bitmap
extern int32 bitmap565Free(BITMAP_565 *b);
//从mrp中读取bitmap
extern BITMAP_565 *readBitmap565FromAssets(char *filename);
// 画矩形
void gl_drawRect(int32 x,int32 y,int32 w,int32 h,uint32 color);
//画线
void gl_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);
//绘制空心三角形
void gl_drawHollowTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
//画圆
void gl_drawCir(int32 x,int32 y,int32 r,uint32 color);
//绘制旋转的空心矩形
void mrc_drawRotatedRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint8 r, uint8 g, uint8 b);
//绘制渐变矩形
void drawShadeRect(int32 x, int32 y, int32 w, int32 h, uint32 colorA, uint32 colorB, int mode);
//绘制三角形
void gl_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
//扩展绘制bitmap
void drawBitmap565Ex(BITMAP_565* bmp, int32 x,int32 y,int32 w,int32 h, int32 tx, int32 ty,int32 tw,int32 th);
//将bitmap缩放,生成一个新的BITMAP
BITMAP_565 *createBitmapFromBitmap(BITMAP_565 *bmp, int32 width, int32 height);
//获取bitmap信息
int32 bitmap565getInfo(BITMAP_565* bmp, BITMAPINFO *info);
//图片旋转绘制
void drawBitmap565Rotate(BITMAP_565 *b, int32 scrx, int32 scry, int32 bx, int32 by, int32 r);
//将buf图片指定区域绘制到di中
void drawBitmap565Old(BITMAP_565 *di, BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);
//保存bitmap到文件
void saveBitmap(BITMAP_565 *bmp, const char *filename);
//透明度绘制bitmap
void drawBitmapAlpha(BITMAP_565 *b, int32 x, int32 y, int32 alpha);
//旋转/翻转bitmap
void bitmap565Rotate(BITMAP_565 *b, int type, int flip);
//在bitmap内绘制空心三角形
void bmp_drawHollowTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
//在bitmap内绘制三角形
void bmp_drawTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
// 在bitmap内绘制空心圆
void bmp_drawHollowCircle(BITMAP_565 *bmp, int32 centerX, int32 centerY, int32 radius, uint32 color);
// 在bitmap内绘制圆形
void bmp_drawCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 color);
// 在bitmap内绘制线段
void bmp_drawLine(BITMAP_565 *bmp, int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);
// 在bitmap内绘制点
void bmp_drawPoint(BITMAP_565 *bmp, int32 x, int32 y, uint32 color);
// 在bitmap内绘制矩形
void bmp_drawRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color);
#endif