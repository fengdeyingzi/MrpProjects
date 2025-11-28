#ifndef __MRC_GRAPHICS_HOR_H__
#define __MRC_GRAPHICS_HOR_H__

#include <mrc_base.h>
#include "mrc_graphics.h"

/*
横屏绘图库
实现伪横屏绘制，通过坐标转换将横屏坐标映射到竖屏

坐标转换规则（顺时针90度旋转）：
横屏坐标 (x_hor, y_hor) -> 竖屏坐标 (x_ver, y_ver)
x_ver = SCRH - 1 - y_hor
y_ver = x_hor

横屏时屏幕尺寸：SCRH x SCRW（宽x高）

风的影子
*/

/* 坐标转换宏 */
#define HOR_TO_VER_X(hor_x, hor_y) (SCRH - 1 - (hor_y))
#define HOR_TO_VER_Y(hor_x, hor_y) (hor_x)

/* 横屏尺寸宏 */
#define SCRW_HOR SCRH
#define SCRH_HOR SCRW

/* 基础绘图函数 */

/* 清屏 */
void gl_clearScreen_hor(int32 r, int32 g, int32 b);

/* 画点 */
void gl_drawPoint_hor(int x, int y, uint32 color);

/* 画线 */
void gl_drawLine_hor(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);

/* 画水平线 */
void gl_drawHLine_hor(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);

/* 画矩形 */
void gl_drawRect_hor(int32 x, int32 y, int32 w, int32 h, uint32 color);

/* 画空心矩形 */
void gl_drawHollowRect_hor(int x, int y, int width, int height, uint32 color);

/* 画三角形 */
void gl_drawTriangle_hor(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);

/* 画空心三角形 */
void gl_drawHollowTriangle_hor(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);

/* 画圆 */
void gl_drawCir_hor(int32 x, int32 y, int32 r, uint32 color);

/* 画空心圆 */
void gl_drawHollowCir_hor(int x0, int y0, int r, uint32 color);

/* 绘制填充多边形 */
void gl_drawPolygon_hor(int *points, int count, uint32 color);

/* 绘制空心多边形 */
void gl_drawHollowPolygon_hor(int *points, int count, uint32 color);

/* 绘制旋转的矩形 */
void gl_drawRotatedRect_hor(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color);

/* 绘制旋转的空心矩形 */
void gl_drawRotatedHollowRect_hor(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color);

/* 全屏颜色叠加 */
void gl_drawColor_hor(uint32 color);

/* Bitmap操作函数 */

/* 绘制bitmap */
void drawBitmap_hor(BITMAP_565 *bmp, int32 x, int32 y);

/* 绘制bitmap（带透明度） */
void drawBitmapAlpha_hor(BITMAP_565 *b, int32 x, int32 y, int32 alpha);

/* 绘制bitmap区域 */
void drawBitmapRegion_hor(BITMAP_565 *src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor);

/* 绘制bitmap（缩放） */
void drawBitmapEx_hor(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th);

/* 绘制旋转的bitmap */
void drawBitmapRotate_hor(BITMAP_565 *b, int32 centerX, int32 centerY, int32 bx, int32 by, int32 r);

/* 创建横屏旋转的bitmap（将原始bitmap旋转90度生成新bitmap用于横屏显示） */
BITMAP_565 *createBitmapForHor(BITMAP_565 *bmp);

/* 刷新横屏区域到屏幕 */
void mrc_refreshScreen_hor(int16 x, int16 y, uint16 w, uint16 h);

#endif /* __MRC_GRAPHICS_HOR_H__ */
