#include <mrc_base.h>
#include "mrc_android.h"
#include "mrc_graphics.h"
#include "mrc_graphics_hor.h"
#include "mpc.h"
#include <math.h>

/*
横屏绘图库实现
实现伪横屏绘制，通过坐标转换将横屏坐标映射到竖屏

坐标转换规则（顺时针90度旋转）：
横屏坐标 (x_hor, y_hor) -> 竖屏坐标 (x_ver, y_ver)
x_ver = SCRH - 1 - y_hor
y_ver = x_hor

风的影子
*/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ==================== 基础绘图函数 ==================== */

/* 清屏 */
void gl_clearScreen_hor(int32 r, int32 g, int32 b)
{
    gl_clearScreen(r, g, b);
}

/* 画点 */
void gl_drawPoint_hor(int x, int y, uint32 color)
{
    int ver_x, ver_y;

    /* 检查横屏坐标范围 */
    if (x < 0 || x >= SCRW_HOR || y < 0 || y >= SCRH_HOR)
        return;

    /* 坐标转换 */
    ver_x = HOR_TO_VER_X(x, y);
    ver_y = HOR_TO_VER_Y(x, y);

    /* 调用竖屏绘图函数 */
    gl_drawPoint(ver_x, ver_y, color);
}

/* 画线 */
void gl_drawLine_hor(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{
    int ver_x1, ver_y1, ver_x2, ver_y2;

    /* 坐标转换 */
    ver_x1 = HOR_TO_VER_X(x1, y1);
    ver_y1 = HOR_TO_VER_Y(x1, y1);
    ver_x2 = HOR_TO_VER_X(x2, y2);
    ver_y2 = HOR_TO_VER_Y(x2, y2);

    /* 调用竖屏绘图函数 */
    gl_drawLine(ver_x1, ver_y1, ver_x2, ver_y2, color);
}

/* 画水平线 */
void gl_drawHLine_hor(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{
    int ver_x1, ver_y1, ver_x2, ver_y2;

    /* 坐标转换 */
    ver_x1 = HOR_TO_VER_X(x1, y1);
    ver_y1 = HOR_TO_VER_Y(x1, y1);
    ver_x2 = HOR_TO_VER_X(x2, y2);
    ver_y2 = HOR_TO_VER_Y(x2, y2);

    /* 调用竖屏绘图函数 */
    gl_drawLine(ver_x1, ver_y1, ver_x2, ver_y2, color);
}

/* 画矩形 */
void gl_drawRect_hor(int32 x, int32 y, int32 w, int32 h, uint32 color)
{
    int ver_x, ver_y;
    int32 ver_w, ver_h;

    /* 检查参数 */
    if (w <= 0 || h <= 0)
        return;

    /* 坐标转换：横屏矩形左上角转换为竖屏坐标 */
    /* 横屏矩形左上角 (x, y)，宽w，高h */
    /* 转换后竖屏矩形左上角 (SCRH - 1 - y - h + 1, x) = (SCRH - y - h, x) */
    ver_x = SCRH - y - h;
    ver_y = x;
    /* 宽高互换 */
    ver_w = h;
    ver_h = w;

    /* 调用竖屏绘图函数 */
    gl_drawRect(ver_x, ver_y, ver_w, ver_h, color);
}

/* 画空心矩形 */
void gl_drawHollowRect_hor(int x, int y, int width, int height, uint32 color)
{
    int ver_x, ver_y;
    int32 ver_w, ver_h;

    /* 检查参数 */
    if (width <= 0 || height <= 0)
        return;

    /* 坐标转换 */
    ver_x = SCRH - y - height;
    ver_y = x;
    ver_w = height;
    ver_h = width;

    /* 调用竖屏绘图函数 */
    gl_drawHollowRect(ver_x, ver_y, ver_w, ver_h, color);
}

/* 画三角形 */
void gl_drawTriangle_hor(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
    int ver_x1, ver_y1, ver_x2, ver_y2, ver_x3, ver_y3;

    /* 坐标转换 */
    ver_x1 = HOR_TO_VER_X(x1, y1);
    ver_y1 = HOR_TO_VER_Y(x1, y1);
    ver_x2 = HOR_TO_VER_X(x2, y2);
    ver_y2 = HOR_TO_VER_Y(x2, y2);
    ver_x3 = HOR_TO_VER_X(x3, y3);
    ver_y3 = HOR_TO_VER_Y(x3, y3);

    /* 调用竖屏绘图函数 */
    gl_drawTriangle(ver_x1, ver_y1, ver_x2, ver_y2, ver_x3, ver_y3, color);
}

/* 画空心三角形 */
void gl_drawHollowTriangle_hor(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
    int ver_x1, ver_y1, ver_x2, ver_y2, ver_x3, ver_y3;

    /* 坐标转换 */
    ver_x1 = HOR_TO_VER_X(x1, y1);
    ver_y1 = HOR_TO_VER_Y(x1, y1);
    ver_x2 = HOR_TO_VER_X(x2, y2);
    ver_y2 = HOR_TO_VER_Y(x2, y2);
    ver_x3 = HOR_TO_VER_X(x3, y3);
    ver_y3 = HOR_TO_VER_Y(x3, y3);

    /* 调用竖屏绘图函数 */
    gl_drawHollowTriangle(ver_x1, ver_y1, ver_x2, ver_y2, ver_x3, ver_y3, color);
}

/* 画圆 */
void gl_drawCir_hor(int32 x, int32 y, int32 r, uint32 color)
{
    int ver_x, ver_y;

    /* 坐标转换：圆心坐标转换 */
    ver_x = HOR_TO_VER_X(x, y);
    ver_y = HOR_TO_VER_Y(x, y);

    /* 调用竖屏绘图函数 */
    gl_drawCir(ver_x, ver_y, r, color);
}

/* 画空心圆 */
void gl_drawHollowCir_hor(int x0, int y0, int r, uint32 color)
{
    int ver_x, ver_y;

    /* 坐标转换：圆心坐标转换 */
    ver_x = HOR_TO_VER_X(x0, y0);
    ver_y = HOR_TO_VER_Y(x0, y0);

    /* 调用竖屏绘图函数 */
    gl_drawHollowCir(ver_x, ver_y, r, color);
}

/* 绘制填充多边形 */
void gl_drawPolygon_hor(int *points, int count, uint32 color)
{
    int i;
    int *ver_points;

    if (points == NULL || count < 3)
        return;

    /* 分配竖屏坐标数组 */
    ver_points = (int *)mrc_malloc(count * 2 * sizeof(int));
    if (ver_points == NULL)
        return;

    /* 坐标转换 */
    for (i = 0; i < count; i++)
    {
        int hor_x = points[i * 2];
        int hor_y = points[i * 2 + 1];
        ver_points[i * 2] = HOR_TO_VER_X(hor_x, hor_y);
        ver_points[i * 2 + 1] = HOR_TO_VER_Y(hor_x, hor_y);
    }

    /* 调用竖屏绘图函数 */
    gl_drawPolygon(ver_points, count, color);

    /* 释放内存 */
    mrc_free(ver_points);
}

/* 绘制空心多边形 */
void gl_drawHollowPolygon_hor(int *points, int count, uint32 color)
{
    int i;
    int *ver_points;

    if (points == NULL || count < 3)
        return;

    /* 分配竖屏坐标数组 */
    ver_points = (int *)mrc_malloc(count * 2 * sizeof(int));
    if (ver_points == NULL)
        return;

    /* 坐标转换 */
    for (i = 0; i < count; i++)
    {
        int hor_x = points[i * 2];
        int hor_y = points[i * 2 + 1];
        ver_points[i * 2] = HOR_TO_VER_X(hor_x, hor_y);
        ver_points[i * 2 + 1] = HOR_TO_VER_Y(hor_x, hor_y);
    }

    /* 调用竖屏绘图函数 */
    gl_drawHollowPolygon(ver_points, count, color);

    /* 释放内存 */
    mrc_free(ver_points);
}

/* 绘制旋转的矩形 */
void gl_drawRotatedRect_hor(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color)
{
    int ver_centerX, ver_centerY;
    int16 ver_width, ver_height;
    int32 ver_bx, ver_by;

    /* 坐标转换：中心点坐标转换 */
    ver_centerX = HOR_TO_VER_X(centerX, centerY);
    ver_centerY = HOR_TO_VER_Y(centerX, centerY);

    /* 宽高互换 */
    ver_width = height;
    ver_height = width;

    /* 旋转中心点转换：相对位置也需要转换 */
    ver_bx = by;
    ver_by = bx;

    /* 角度调整：横屏旋转需要加90度 */
    angle = angle + 90.0f;

    /* 调用竖屏绘图函数 */
    gl_drawRotatedRect(ver_centerX, ver_centerY, ver_width, ver_height, ver_bx, ver_by, angle, color);
}

/* 绘制旋转的空心矩形 */
void gl_drawRotatedHollowRect_hor(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color)
{
    int ver_centerX, ver_centerY;
    int16 ver_width, ver_height;
    int32 ver_bx, ver_by;

    /* 坐标转换：中心点坐标转换 */
    ver_centerX = HOR_TO_VER_X(centerX, centerY);
    ver_centerY = HOR_TO_VER_Y(centerX, centerY);

    /* 宽高互换 */
    ver_width = height;
    ver_height = width;

    /* 旋转中心点转换 */
    ver_bx = by;
    ver_by = bx;

    /* 角度调整 */
    angle = angle + 90.0f;

    /* 调用竖屏绘图函数 */
    gl_drawRotatedHollowRect(ver_centerX, ver_centerY, ver_width, ver_height, ver_bx, ver_by, angle, color);
}

/* 全屏颜色叠加 */
void gl_drawColor_hor(uint32 color)
{
    gl_drawColor(color);
}

/* ==================== Bitmap操作函数 ==================== */

/* 创建横屏旋转的bitmap（将原始bitmap旋转90度生成新bitmap用于横屏显示） */
BITMAP_565 *createBitmapForHor(BITMAP_565 *bmp)
{
    BITMAP_565 *new_bmp;
    int x, y;
    uint16 *src16, *dst16;
    uint32 *src32, *dst32;

    if (bmp == NULL)
        return NULL;

    /* 创建新bitmap，宽高互换 */
    new_bmp = (BITMAP_565 *)mrc_malloc(sizeof(BITMAP_565));
    if (new_bmp == NULL)
        return NULL;

    mrc_memset(new_bmp, 0, sizeof(BITMAP_565));

    new_bmp->width = bmp->height;
    new_bmp->height = bmp->width;
    new_bmp->color_bit = bmp->color_bit;
    new_bmp->transcolor = bmp->transcolor;
    new_bmp->mode = bmp->mode;

    if (bmp->color_bit == 16)
    {
        new_bmp->buflen = new_bmp->width * new_bmp->height * 2;
        new_bmp->bitmap = (uint16 *)mr_malloc(new_bmp->buflen);
        if (new_bmp->bitmap == NULL)
        {
            mrc_free(new_bmp);
            return NULL;
        }

        src16 = bmp->bitmap;
        dst16 = new_bmp->bitmap;

        /* 顺时针旋转90度 */
        for (y = 0; y < bmp->height; y++)
        {
            for (x = 0; x < bmp->width; x++)
            {
                /* 原坐标 (x, y) -> 新坐标 (bmp->height - 1 - y, x) */
                dst16[(bmp->height - 1 - y) + x * new_bmp->width] = src16[y * bmp->width + x];
            }
        }
    }
    else if (bmp->color_bit == 32)
    {
        new_bmp->buflen = new_bmp->width * new_bmp->height * 4;
        new_bmp->bitmap = (uint16 *)mr_malloc(new_bmp->buflen);
        if (new_bmp->bitmap == NULL)
        {
            mrc_free(new_bmp);
            return NULL;
        }

        src32 = (uint32 *)bmp->bitmap;
        dst32 = (uint32 *)new_bmp->bitmap;

        /* 顺时针旋转90度 */
        for (y = 0; y < bmp->height; y++)
        {
            for (x = 0; x < bmp->width; x++)
            {
                dst32[(bmp->height - 1 - y) + x * new_bmp->width] = src32[y * bmp->width + x];
            }
        }
    }
    else
    {
        mrc_free(new_bmp);
        return NULL;
    }

    return new_bmp;
}

/* 绘制bitmap */
void drawBitmap_hor(BITMAP_565 *bmp, int32 x, int32 y)
{
    BITMAP_565 *rotated_bmp;
    int ver_x, ver_y;

    if (bmp == NULL)
        return;

    /* 创建旋转后的bitmap */
    rotated_bmp = createBitmapForHor(bmp);
    if (rotated_bmp == NULL)
        return;

    /* 坐标转换：横屏左上角 (x, y) 转换为竖屏坐标 */
    ver_x = SCRH - y - rotated_bmp->height;
    ver_y = x;

    /* 绘制 */
    drawBitmap(rotated_bmp, ver_x, ver_y);

    /* 释放旋转后的bitmap */
    bitmapFree(rotated_bmp);
}

/* 绘制bitmap（带透明度） */
void drawBitmapAlpha_hor(BITMAP_565 *b, int32 x, int32 y, int32 alpha)
{
    BITMAP_565 *rotated_bmp;
    int ver_x, ver_y;

    if (b == NULL)
        return;

    /* 创建旋转后的bitmap */
    rotated_bmp = createBitmapForHor(b);
    if (rotated_bmp == NULL)
        return;

    /* 坐标转换 */
    ver_x = SCRH - y - rotated_bmp->height;
    ver_y = x;

    /* 绘制 */
    drawBitmapAlpha(rotated_bmp, ver_x, ver_y, alpha);

    /* 释放旋转后的bitmap */
    bitmapFree(rotated_bmp);
}

/* 绘制bitmap区域 */
void drawBitmapRegion_hor(BITMAP_565 *src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor)
{
    BITMAP_565 *rotated_bmp;
    int ver_x_src, ver_y_src;
    int ver_width, ver_height;
    int ver_x_dest, ver_y_dest;
    int ver_anchor;

    if (src == NULL)
        return;

    /* 创建旋转后的bitmap */
    rotated_bmp = createBitmapForHor(src);
    if (rotated_bmp == NULL)
        return;

    /* 源区域坐标转换：需要根据旋转调整 */
    /* 原图 (x_src, y_src, width, height) */
    /* 旋转后 (src->height - y_src - height, x_src, height, width) */
    ver_x_src = src->height - y_src - height;
    ver_y_src = x_src;
    ver_width = height;
    ver_height = width;

    /* 目标坐标转换 */
    ver_x_dest = HOR_TO_VER_X(x_dest, y_dest);
    ver_y_dest = HOR_TO_VER_Y(x_dest, y_dest);

    /* 锚点转换 */
    ver_anchor = anchor; /* 锚点暂时保持不变，可能需要调整 */

    /* 绘制 */
    drawBitmapRegion(rotated_bmp, ver_x_src, ver_y_src, ver_width, ver_height, transform, ver_x_dest, ver_y_dest, ver_anchor);

    /* 释放旋转后的bitmap */
    bitmapFree(rotated_bmp);
}

/* 绘制bitmap（缩放） */
void drawBitmapEx_hor(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th)
{
    BITMAP_565 *rotated_bmp;
    int ver_x, ver_y;
    int32 ver_w, ver_h;
    int32 ver_tx, ver_ty, ver_tw, ver_th;

    if (bmp == NULL)
        return;

    /* 创建旋转后的bitmap */
    rotated_bmp = createBitmapForHor(bmp);
    if (rotated_bmp == NULL)
        return;

    /* 目标坐标和尺寸转换 */
    ver_x = SCRH - y - h;
    ver_y = x;
    ver_w = h;
    ver_h = w;

    /* 源区域转换 */
    ver_tx = bmp->height - ty - th;
    ver_ty = tx;
    ver_tw = th;
    ver_th = tw;

    /* 绘制 */
    drawBitmapEx(rotated_bmp, ver_x, ver_y, ver_w, ver_h, ver_tx, ver_ty, ver_tw, ver_th);

    /* 释放旋转后的bitmap */
    bitmapFree(rotated_bmp);
}

/* 绘制旋转的bitmap */
void drawBitmapRotate_hor(BITMAP_565 *b, int32 centerX, int32 centerY, int32 bx, int32 by, int32 r)
{
    BITMAP_565 *rotated_bmp;
    int ver_centerX, ver_centerY;
    int32 ver_bx, ver_by;
    int32 ver_r;

    if (b == NULL)
        return;

    /* 创建旋转后的bitmap */
    rotated_bmp = createBitmapForHor(b);
    if (rotated_bmp == NULL)
        return;

    /* 中心点坐标转换 */
    ver_centerX = HOR_TO_VER_X(centerX, centerY);
    ver_centerY = HOR_TO_VER_Y(centerX, centerY);

    /* 旋转中心点转换 */
    ver_bx = by;
    ver_by = bx;

    /* 角度调整 */
    ver_r = r + 90;

    /* 绘制 */
    drawBitmapRotate(rotated_bmp, ver_centerX, ver_centerY, ver_bx, ver_by, ver_r);

    /* 释放旋转后的bitmap */
    bitmapFree(rotated_bmp);
}

/* 刷新横屏区域到屏幕 */
void mrc_refreshScreen_hor(int16 x, int16 y, uint16 w, uint16 h)
{
    int16 ver_x, ver_y;
    uint16 ver_w, ver_h;

    /* 坐标转换 */
    ver_x = SCRH - y - h;
    ver_y = x;
    ver_w = h;
    ver_h = w;

    /* 调用竖屏刷新函数 */
    mrc_refreshScreen(ver_x, ver_y, ver_w, ver_h);
}
