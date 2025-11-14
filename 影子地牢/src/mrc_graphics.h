#ifndef _MRC_GRAPHICS_H_
#define _MRC_GRAPHICS_H_

#include "xl_bmp.h"
enum
{
    _JPG = 0,
    _PNG = 1,
    _BMP16 = 2
};

typedef struct
{
    uint32 width;  // 宽度
    uint32 height; // 高度
    uint32 stride;
    int32 format; // 格式
    uint32 flags; // 0 for now
    void *ptr;    // 缓存
} BITMAPINFO;

typedef struct
{
    int16 A; // A, B, C, and D are fixed point values with an 8-bit integer part
    int16 B; // and an 8-bit fractional part.
    int16 C;
    int16 D;
    uint16 rop;
} mr_transMatrixSt;


enum
{
    SHADE_UPDOWN,    // 从上到下
    SHADE_LEFTRIGHT, // 从左到右
    SHADE_DOWNUP,    // 从下到上
    SHADE_RIGHTLEFT  // 从右到左
};


#define GRAPHICS_LEFT 3
#define GRAPHICS_TOP 48
#define GRAPHICS_RIGHT 5
#define GRAPHICS_CENTER 17
#define GRAPHICS_BOTTOM 80

// 混合两个颜色
uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha);

int gl_bitmapShowExTrans8888(uint32 *p, int16 x, int16 y, int16 mw, int16 w, int16 h, int16 sx, int16 sy);
int gl_bitmapShowExTrans(uint16 *p, int16 x, int16 y, int16 mw, int16 w, int16 h, uint16 rop, int16 sx, int16 sy, uint16 transcolor);
    // 从内存卡读取bitmap
    extern BITMAP_565 *readBitmap(char *filename);
// 创建一个bitmap
BITMAP_565 *createBitmap565(int width, int height);
// 绘制bitmap指定区域
extern int32 drawBitmapFlip(BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);
// 绘制bitmap
/*
src: 源图像对象，即要从中提取区域的图像。
x_src: 源图像中要提取区域的左上角的x坐标。
y_src: 源图像中要提取区域的左上角的y坐标。
width: 要提取区域的宽度。
height:要提取区域的高度。
transform:变换类型
x_dest: 目标位置的x坐标，即绘制提取区域的目标位置的左上角的x坐标。
y_dest: 目标位置的y坐标，即绘制提取区域的目标位置的左上角的y坐标。
anchor: 锚点，指定目标位置的参考点。常见的锚点包括：GRAPHICS_LEFT,GRAPHICS_TOP,GRAPHICS_RIGHT,GRAPHICS_BOTTOM,GRAPHICS_CENTER
*/
extern void drawBitmapRegion(BITMAP_565 *src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor);

extern void drawBitmap565(BITMAP_565 *b, int32 x, int32 y);
void drawBitmap(BITMAP_565 *bmp, int32 x, int32 y);
// memcpy绘图优化
extern int32 bitmapAutoMemcpy(BITMAP_565 *b);
// 扩展绘制bitmap
void drawBitmapEx(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th);
// 绘制argb颜色值的bitmap
void drawBitmap8888(BITMAP_565 *bmp, int32 x, int32 y);
// 释放bitmap
extern int32 bitmapFree(BITMAP_565 *b);
// 从mrp中读取bitmap
extern BITMAP_565 *readBitmap565FromAssets(const char *filename);
void gl_drawColor(uint32 color);
void gl_clearScreen(int32 r, int32 g, int32 b);
// 画矩形
void gl_drawRect(int32 x, int32 y, int32 w, int32 h, uint32 color);
void gl_drawHollowRect(int x, int y, int width, int height, uint32 color);
// 画线
void gl_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);
// 绘制三角形
void gl_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
// 绘制空心三角形
void gl_drawHollowTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
// 画圆
void gl_drawCir(int32 x, int32 y, int32 r, uint32 color);
// 绘制空心圆
void gl_drawHollowCir(int x0, int y0, int r, uint32 color);
// 绘制旋转的矩形
void gl_drawRotatedRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color);
// 绘制旋转的空心矩形
void gl_drawRotatedHollowRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color);
// 绘制渐变矩形
void drawShadeRect(int32 x, int32 y, int32 w, int32 h, uint32 colorA, uint32 colorB, int mode);

// 将bitmap缩放,生成一个新的BITMAP
BITMAP_565 *createBitmapFromBitmap(BITMAP_565 *bmp, int32 width, int32 height);
// 获取bitmap信息
int32 bitmap565getInfo(BITMAP_565 *bmp, BITMAPINFO *info);
// 图片旋转绘制
void drawBitmapRotate(BITMAP_565 *b, int32 scrx, int32 scry, int32 bx, int32 by, int32 r);
// 将buf图片指定区域绘制到di中
void drawBitmap565Old(BITMAP_565 *di, BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);
// 保存bitmap到文件
void saveBitmap(BITMAP_565 *bmp, const char *filename);
// 透明度绘制bitmap
void drawBitmapAlpha(BITMAP_565 *b, int32 x, int32 y, int32 alpha);
// 旋转/翻转bitmap
void bitmap565Rotate(BITMAP_565 *b, int type, int flip);
// 在bitmap内绘制空心三角形
void bmp_drawHollowTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
// 在bitmap内绘制三角形
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