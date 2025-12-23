#include "mrc_graphics.h"



/**
 * 创建一张bitmap
 * @param width 宽度
 * @param height 高度
 * @param bit 位数 16/32
 */
BITMAP_565 *bmp_createBitmap(int width, int height, int bit);

/**
 * 复制一张bitmap
 * @param src 源bitmap
 */
BITMAP_565 *bmp_copyBitmap(BITMAP_565 *src);

/**
 * 在bitmap内绘制点
 */
static void bmp_drawPoint(BITMAP_565 *bmp, int32 x, int32 y, uint32 color);

/**
 * 在bitmap上绘制渐变矩形
 * @param bmp 目标bitmap
 * @param x 矩形左上角x坐标
 * @param y 矩形左上角y坐标
 * @param w 矩形宽度
 * @param h 矩形高度
 * @param colorA 颜色A
 * @param colorB 颜色B
 * @param mode 渐变模式
enum
{
    SHADE_UPDOWN,    // 从上到下
    SHADE_LEFTRIGHT, // 从左到右
    SHADE_DOWNUP,    // 从下到上
    SHADE_RIGHTLEFT  // 从右到左
};
 */
void bmp_drawShadeRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 colorA, uint32 colorB, int mode);

/**
 * 在bitmap上绘制矩形
 */
void bmp_drawRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color);

/**
 * 在bitmap上绘制空心矩形
 */
void bmp_drawHollowRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color);

/**
 * 在bitmap上绘制圆角矩形
 * @param bmp 目标bitmap
 * @param x 矩形左上角x坐标
 * @param y 矩形左上角y坐标
 * @param w 矩形宽度
 * @param h 矩形高度
 * @param radius 圆角半径
 * @param color 填充颜色
 */
void bmp_drawRoundRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 radius, uint32 color);

/**
 * 在bitmap上绘制空心圆角矩形
 * @param bmp 目标bitmap
 * @param x 矩形左上角x坐标
 * @param y 矩形左上角y坐标
 * @param w 矩形宽度
 * @param h 矩形高度
 * @param radius 圆角半径
 * @param color 边框颜色
 */
void bmp_drawHollowRoundRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 radius, uint32 color);

/**
 * 在bitmap上画圆
 */
void bmp_drawCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 color);

/**
 * 在bitmap上画空心圆
 */
void bmp_drawHollowCircle(BITMAP_565 *bmp, int32 centerX, int32 centerY, int32 radius, uint32 color);

/**
 * 在bitmap上画三角形
 */
void bmp_drawTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);

/**
 * 在bitmap上画空心三角形
 */
void bmp_drawHollowTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);

/**
 * 在bitmap上画椭圆
 */
void bmp_drawEllipse(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color);

/**
 * 在bitmap上画空心椭圆
 */
void bmp_drawHollowEllipse(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color);

/**
 * 在bitmap上画线
 */
void bmp_drawLine(BITMAP_565 *bmp, int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);

/**
 * 在bitmap上画水平线(优化版本)
 */
void bmp_drawHLine(BITMAP_565 *bmp, int32 x1, int32 y, int32 x2, uint32 color);

/**
 * 在bitmap上画垂直线(优化版本)
 */
void bmp_drawVLine(BITMAP_565 *bmp, int32 x, int32 y1, int32 y2, uint32 color);

/**
 * 在bitmap上画渐变圆形
 */
void bmp_drawShadeCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 colorA, uint32 colorB);

/**
 * 将bitmap中不透明像素处理成指定颜色
 */
void bmp_fillColor(BITMAP_565 *bmp, uint32 color);

/**
 * 在bitmap上画环形
 */
void bmp_drawHollowRing(BITMAP_565 *bmp, int32 cx, int32 cy, int32 innerRadius, int32 outerRadius, uint32 color);

/**
 * 在bitmap上画扇形
 */
void bmp_drawFanShape(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, int32 startAngle, int32 endAngle, uint32 color);

/**
 * 在bitmap上画空心扇形
 */
void bmp_drawHollowFanShape(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, int32 startAngle, int32 endAngle, uint32 color);

/**
 * 在bitmap上画镜像圆形渐变
 */
void bmp_drawShadeMirrorCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 colorA, uint32 colorB);

/**
 * 将buf图片指定区域绘制到di中
 */
void drawBitmap565Old(BITMAP_565 *di, BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);

/**
 * 将bitmap缩放到指定大小
 */
void bmp_scaleBitmap(BITMAP_565 *src, int32 width, int32 height);

/**
 * 旋转/翻转bitmap
 * @param b 目标bitmap
 * @param type 旋转类型 0:不旋转 1:90度 2:180度 3:270度
 * @param flip 翻转类型 0:不翻转 1:水平翻转 2:垂直翻转
 */
void bmpRotate(BITMAP_565 *b, int type, int flip);

/**
 * 按中心点旋转bitmap，返回旋转后的bitmap
 */
BITMAP_565 *bmp_rotateBitmap(BITMAP_565 *src, int32 angle);

// 保存图片为bma格式
void bmp_saveBitmap(BITMAP_565 *bmp, const char *filename);