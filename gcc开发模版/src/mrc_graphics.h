#ifndef _MRC_GRAPHICS_H_
#define _MRC_GRAPHICS_H_

#include "mrc_base.h"
#include "xl_bmp.h"
enum
{
_JPG=0,
_PNG=1,
_BMP16=2
};

// 图形锚点常量
typedef enum {
    GRAPHICS_LEFT = 0,    // 左对齐
    GRAPHICS_RIGHT = 1,   // 右对齐
    GRAPHICS_TOP = 2,     // 上对齐
    GRAPHICS_BOTTOM = 3,  // 下对齐
    GRAPHICS_CENTER = 4   // 居中对齐
} GraphicsAnchor;


typedef struct
{
uint32 width; //瀹藉害
uint32 height; //楂樺害
uint32 stride;
int32 format; //鏍煎紡
uint32 flags; // 0 for now
void *ptr; //缂撳瓨
} BITMAPINFO;




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
// 从mrp中读取bitmap
extern BITMAP_565 *readBitmapFromAssets(const char *filename);

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


void drawBitmap(BITMAP_565 *bmp, int32 x, int32 y);

// 扩展绘制bitmap
void drawBitmapEx(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th);

// 释放bitmap
extern int32 bitmapFree(BITMAP_565 *b);

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
// 绘制多边形(填充)
void gl_drawPolygon(int *points, int count, uint32 color);
// 绘制空心多边形
void gl_drawHollowPolygon(int *points, int count, uint32 color);
// 画圆
void gl_drawCir(int32 x, int32 y, int32 r, uint32 color);
// 绘制空心圆
void gl_drawHollowCir(int x0, int y0, int r, uint32 color);
// 绘制旋转的矩形
void gl_drawRotatedRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, int32 angle, uint32 color);
// 绘制旋转的空心矩形
void gl_drawRotatedHollowRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, int32 angle, uint32 color);

// 画文字 font表示字体大小，可选0/1/2三种大小
int32 gl_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);

// 在矩形区域内绘制多行uc3字体 y坐标可为负数
int32 gl_drawTextInRect(const char *pcText, int16 dx, int16 dy, int16 x, int16 y, int16 width, int16 height, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);

// 返回待显示字符串若显示在宽为w的区间里，需要的行数；
int32 gl_textRow(const char* pcText, int32 w, int is_unicode, uint16 font);

// 获取文字宽高 保存到w和h中
int32 gl_textWidthHeight(const char* pcText, int is_unicode, uint16 font, int32* w, int32* h);

// 获取bitmap信息
int32 bitmapGetInfo(BITMAP_565 *bmp, BITMAPINFO *info);

// 绘制emoji, size表示emoji图标的宽高
int32 emoji_draw(char *emoji, int x, int y, int size);

// 绘制emoji封装, size表示emoji图标的宽高
int32 gl_emoji_draw(char *emoji, int x, int y, int size);

// 释放资源（font资源需要释放）
int32 gl_free();


#endif