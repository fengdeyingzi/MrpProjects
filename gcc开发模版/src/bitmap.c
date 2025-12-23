#include "mrc_base.h"
#include "mrc_graphics.h"
#include "xl_bmp.h"
#include "ex_math.h"
#include <math.h>


/*
实现bitmap绘图函数
参考mrc_graphics.c的实现方式

风的影子
*/

// 将rgb888转rgb565
#define MAKECOLOR565(color) ((uint16)((color >> 8) & 0xf800) | (uint16)((color >> 5) & 0x07e0) | (uint16)((color >> 3) & 0x1f))
#ifndef MAKERGB
#define MAKERGB(r, g, b) (((uint16)(r >> 3) << 11) | ((uint16)(g >> 2) << 5) | ((uint16)(b >> 3)))
#endif

// 声明外部函数
extern int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3);
extern uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha);
extern uint16 blendColor888(uint16 dstColor, uint32 srcColor);
extern void bmp_drawHLine(BITMAP_565 *bmp, int32 x1, int32 y, int32 x2, uint32 color);
extern void bmp_drawVLine(BITMAP_565 *bmp, int32 x, int32 y1, int32 y2, uint32 color);
/**
 * 创建一张bitmap
 */
BITMAP_565 *bmp_createBitmap(int width, int height, int bit)
{
    BITMAP_565 *bmp = (BITMAP_565 *)mrc_malloc(sizeof(BITMAP_565));
    if (bmp == NULL) {
        return NULL;
    }

    mrc_memset(bmp, 0, sizeof(BITMAP_565));

    if (bit == 16) {
        bmp->bitmap = (uint16 *)mr_malloc(width * height * 2);
        if (bmp->bitmap == NULL) {
            mrc_free(bmp);
            return NULL;
        }
        mrc_memset(bmp->bitmap, 0, width * height * 2);
        bmp->buflen = width * height * 2;
    } else if (bit == 32) {
        bmp->bitmap = (uint16 *)mr_malloc(width * height * 4);
        if (bmp->bitmap == NULL) {
            mrc_free(bmp);
            return NULL;
        }
        mrc_memset(bmp->bitmap, 0, width * height * 4);
        bmp->buflen = width * height * 4;
    } else {
        mrc_free(bmp);
        return NULL;
    }

    bmp->color_bit = bit;
    bmp->transcolor = 0;
    bmp->width = width;
    bmp->height = height;
    bmp->mode = BM_COPY;

    return bmp;
}

/**
 * 复制一张bitmap
 */
BITMAP_565 *bmp_copyBitmap(BITMAP_565 *src)
{
    BITMAP_565 *dst;

    if (src == NULL || src->bitmap == NULL) {
        return NULL;
    }

    dst = (BITMAP_565 *)mrc_malloc(sizeof(BITMAP_565));
    if (dst == NULL) {
        return NULL;
    }

    mrc_memcpy(dst, src, sizeof(BITMAP_565));

    dst->bitmap = (uint16 *)mr_malloc(src->buflen);
    if (dst->bitmap == NULL) {
        mrc_free(dst);
        return NULL;
    }

    mrc_memcpy(dst->bitmap, src->bitmap, src->buflen);
    dst->memcache = NULL;
    dst->memlen = 0;

    return dst;
}

/**
 * 在bitmap内绘制点
 */
static void bmp_drawPoint(BITMAP_565 *bmp, int32 x, int32 y, uint32 color)
{
    int index;
    uint16 dstColor, color565;
    uint32 *bitmap32;
    int alpha;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    if (x < 0 || x >= bmp->width || y < 0 || y >= bmp->height) {
        return;
    }

    index = y * bmp->width + x;

    if (bmp->color_bit == 16) {
        dstColor = bmp->bitmap[index];
        color565 = MAKECOLOR565(color);
        alpha = (color >> 24) & 0xff;

        if (dstColor != bmp->transcolor) {
            bmp->bitmap[index] = blendColor(dstColor, color565, alpha);
        }
    } else if (bmp->color_bit == 32) {
        bitmap32 = (uint32 *)bmp->bitmap;

        // 对于32位色，直接使用ARGB格式
        if ((bitmap32[index] >> 24) != 0 || (color >> 24) != 0) {
            bitmap32[index] = color;
        }
    }
}

/**
 * 在bitmap上绘制渐变矩形
 */
void bmp_drawShadeRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 colorA, uint32 colorB, int mode)
{
    int32 ix, iy;
    uint8 rA, gA, bA, rB, gB, bB;
    uint8 r, g, b, a;
    uint16 color565;
    uint32 color32;
    int32 ratio;
    uint32 *bitmap32;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    // 提取颜色分量
    rA = (colorA >> 16) & 0xFF;
    gA = (colorA >> 8) & 0xFF;
    bA = colorA & 0xFF;

    rB = (colorB >> 16) & 0xFF;
    gB = (colorB >> 8) & 0xFF;
    bB = colorB & 0xFF;

    if (bmp->color_bit == 16) {
        for (iy = 0; iy < h; iy++) {
            for (ix = 0; ix < w; ix++) {
                if (x + ix < 0 || x + ix >= bmp->width || y + iy < 0 || y + iy >= bmp->height) {
                    continue;
                }

                switch (mode) {
                    case SHADE_UPDOWN:
                        ratio = (iy * 255) / h;
                        break;
                    case SHADE_DOWNUP:
                        ratio = ((h - iy) * 255) / h;
                        break;
                    case SHADE_LEFTRIGHT:
                        ratio = (ix * 255) / w;
                        break;
                    case SHADE_RIGHTLEFT:
                        ratio = ((w - ix) * 255) / w;
                        break;
                    default:
                        ratio = 0;
                        break;
                }

                r = (rA * (255 - ratio) + rB * ratio) / 255;
                g = (gA * (255 - ratio) + gB * ratio) / 255;
                b = (bA * (255 - ratio) + bB * ratio) / 255;

                color565 = MAKERGB(r, g, b);
                bmp->bitmap[(y + iy) * bmp->width + (x + ix)] = color565;
            }
        }
    } else if (bmp->color_bit == 32) {
        bitmap32 = (uint32 *)bmp->bitmap;
        for (iy = 0; iy < h; iy++) {
            for (ix = 0; ix < w; ix++) {
                if (x + ix < 0 || x + ix >= bmp->width || y + iy < 0 || y + iy >= bmp->height) {
                    continue;
                }

                switch (mode) {
                    case SHADE_UPDOWN:
                        ratio = (iy * 255) / h;
                        break;
                    case SHADE_DOWNUP:
                        ratio = ((h - iy) * 255) / h;
                        break;
                    case SHADE_LEFTRIGHT:
                        ratio = (ix * 255) / w;
                        break;
                    case SHADE_RIGHTLEFT:
                        ratio = ((w - ix) * 255) / w;
                        break;
                    default:
                        ratio = 0;
                        break;
                }

                r = (rA * (255 - ratio) + rB * ratio) / 255;
                g = (gA * (255 - ratio) + gB * ratio) / 255;
                b = (bA * (255 - ratio) + bB * ratio) / 255;
                a = 0xFF; // 不透明

                color32 = (a << 24) | (r << 16) | (g << 8) | b;
                bitmap32[(y + iy) * bmp->width + (x + ix)] = color32;
            }
        }
    }
}

/**
 * 在bitmap上绘制矩形
 */
void bmp_drawRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color)
{
    int32 ix, iy;
    int32 startX, startY, endX, endY;
    uint16 color565;
    uint32 *bitmap32;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    startX = (x < 0) ? 0 : x;
    startY = (y < 0) ? 0 : y;
    endX = (x + w > bmp->width) ? bmp->width : (x + w);
    endY = (y + h > bmp->height) ? bmp->height : (y + h);

    if (bmp->color_bit == 16) {
        color565 = MAKECOLOR565(color);
        for (iy = startY; iy < endY; iy++) {
            for (ix = startX; ix < endX; ix++) {
                bmp->bitmap[iy * bmp->width + ix] = color565;
            }
        }
    } else if (bmp->color_bit == 32) {
        bitmap32 = (uint32 *)bmp->bitmap;
        for (iy = startY; iy < endY; iy++) {
            for (ix = startX; ix < endX; ix++) {
                bitmap32[iy * bmp->width + ix] = color;
            }
        }
    }
}

/**
 * 在bitmap上绘制空心矩形
 */
void bmp_drawHollowRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color)
{
    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    if (w <= 0 || h <= 0) {
        return;
    }

    // 使用HLine和VLine加速绘制
    // 上边
    bmp_drawHLine(bmp, x, y, x + w - 1, color);
    // 下边
    bmp_drawHLine(bmp, x, y + h - 1, x + w - 1, color);
    // 左边
    bmp_drawVLine(bmp, x, y, y + h - 1, color);
    // 右边
    bmp_drawVLine(bmp, x + w - 1, y, y + h - 1, color);
}

/**
 * 在bitmap上绘制圆角矩形
 */
void bmp_drawRoundRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 radius, uint32 color)
{
    int32 ix, iy;
    int32 dx, dy;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    if (w <= 0 || h <= 0 || radius < 0) {
        return;
    }

    // 限制圆角半径不超过矩形的一半
    if (radius > w / 2) radius = w / 2;
    if (radius > h / 2) radius = h / 2;

    // 如果圆角半径为0，直接绘制普通矩形
    if (radius == 0) {
        bmp_drawRect(bmp, x, y, w, h, color);
        return;
    }

    // 绘制中间部分的矩形（不包含圆角）
    // 上中矩形
    bmp_drawRect(bmp, x + radius, y, w - 2 * radius, radius, color);
    // 中间矩形
    bmp_drawRect(bmp, x, y + radius, w, h - 2 * radius, color);
    // 下中矩形
    bmp_drawRect(bmp, x + radius, y + h - radius, w - 2 * radius, radius, color);

    // 绘制四个圆角
    for (iy = 0; iy < radius; iy++) {
        for (ix = 0; ix < radius; ix++) {
            dx = radius - ix;
            dy = radius - iy;

            // 判断点是否在圆内
            if (dx * dx + dy * dy <= radius * radius) {
                // 左上角
                bmp_drawPoint(bmp, x + ix, y + iy, color);
                // 右上角
                bmp_drawPoint(bmp, x + w - 1 - ix, y + iy, color);
                // 左下角
                bmp_drawPoint(bmp, x + ix, y + h - 1 - iy, color);
                // 右下角
                bmp_drawPoint(bmp, x + w - 1 - ix, y + h - 1 - iy, color);
            }
        }
    }
}

/**
 * 在bitmap上绘制空心圆角矩形
 */
void bmp_drawHollowRoundRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 radius, uint32 color)
{
    int32 angle;
    int32 px, py;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    if (w <= 0 || h <= 0 || radius < 0) {
        return;
    }

    // 限制圆角半径不超过矩形的一半
    if (radius > w / 2) radius = w / 2;
    if (radius > h / 2) radius = h / 2;

    // 如果圆角半径为0，直接绘制空心矩形
    if (radius == 0) {
        bmp_drawHollowRect(bmp, x, y, w, h, color);
        return;
    }

    // 绘制四条直边
    // 上边（不包含圆角部分）
    bmp_drawHLine(bmp, x + radius, y, x + w - 1 - radius, color);
    // 下边（不包含圆角部分）
    bmp_drawHLine(bmp, x + radius, y + h - 1, x + w - 1 - radius, color);
    // 左边（不包含圆角部分）
    bmp_drawVLine(bmp, x, y + radius, y + h - 1 - radius, color);
    // 右边（不包含圆角部分）
    bmp_drawVLine(bmp, x + w - 1, y + radius, y + h - 1 - radius, color);

    // 绘制四个圆角（使用三角函数）
    // 左上角 (180-270度)
    for (angle = 180; angle < 270; angle++) {
        px = x + radius + (radius * INT_COS(angle)) / TRIG_SCALE;
        py = y + radius + (radius * INT_SIN(angle)) / TRIG_SCALE;
        if (px >= 0 && px < bmp->width && py >= 0 && py < bmp->height) {
            bmp_drawPoint(bmp, px, py, color);
        }
    }

    // 右上角 (270-360度)
    for (angle = 270; angle < 360; angle++) {
        px = x + w - 1 - radius + (radius * INT_COS(angle)) / TRIG_SCALE;
        py = y + radius + (radius * INT_SIN(angle)) / TRIG_SCALE;
        if (px >= 0 && px < bmp->width && py >= 0 && py < bmp->height) {
            bmp_drawPoint(bmp, px, py, color);
        }
    }

    // 右下角 (0-90度)
    for (angle = 0; angle < 90; angle++) {
        px = x + w - 1 - radius + (radius * INT_COS(angle)) / TRIG_SCALE;
        py = y + h - 1 - radius + (radius * INT_SIN(angle)) / TRIG_SCALE;
        if (px >= 0 && px < bmp->width && py >= 0 && py < bmp->height) {
            bmp_drawPoint(bmp, px, py, color);
        }
    }

    // 左下角 (90-180度)
    for (angle = 90; angle < 180; angle++) {
        px = x + radius + (radius * INT_COS(angle)) / TRIG_SCALE;
        py = y + h - 1 - radius + (radius * INT_SIN(angle)) / TRIG_SCALE;
        if (px >= 0 && px < bmp->width && py >= 0 && py < bmp->height) {
            bmp_drawPoint(bmp, px, py, color);
        }
    }
}

/**
 * 在bitmap上画圆
 */
void bmp_drawCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 color)
{
    int16 x, y;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                bmp_drawPoint(bmp, cx + x, cy + y, color);
            }
        }
    }
}

/**
 * 在bitmap上画空心圆
 */
void bmp_drawHollowCircle(BITMAP_565 *bmp, int32 centerX, int32 centerY, int32 radius, uint32 color)
{
    int32 x, y;
    int32 angle;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    for (angle = 0; angle < 360; angle++) {
        x = centerX + (radius * INT_COS(angle)) / TRIG_SCALE;
        y = centerY + (radius * INT_SIN(angle)) / TRIG_SCALE;

        if (x >= 0 && x < bmp->width && y >= 0 && y < bmp->height) {
            bmp_drawPoint(bmp, x, y, color);
        }
    }
}

/**
 * 在bitmap上画线
 */
void bmp_drawLine(BITMAP_565 *bmp, int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{
    int dx = ex_abs(x2 - x1);
    int dy = ex_abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int err2;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    while (1) {
        bmp_drawPoint(bmp, x1, y1, color);

        if (x1 == x2 && y1 == y2)
            break;

        err2 = err * 2;

        if (err2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (err2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/**
 * 在bitmap上画水平线(优化版本)
 */
void bmp_drawHLine(BITMAP_565 *bmp, int32 x1, int32 y, int32 x2, uint32 color)
{
    int32 i;
    int32 minX, maxX;
    uint16 color565;
    uint32 *bitmap32;
    int alpha;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    if (y < 0 || y >= bmp->height) {
        return;
    }

    minX = (x1 < x2) ? x1 : x2;
    maxX = (x1 < x2) ? x2 : x1;
    minX = (minX < 0) ? 0 : minX;
    maxX = (maxX >= bmp->width) ? bmp->width - 1 : maxX;

    if (bmp->color_bit == 16) {
        color565 = MAKECOLOR565(color);
        alpha = (color >> 24) & 0xff;

        for (i = minX; i <= maxX; i++) {
            bmp_drawPoint(bmp, i, y, color);
        }
    } else if (bmp->color_bit == 32) {
        bitmap32 = (uint32 *)bmp->bitmap;
        for (i = minX; i <= maxX; i++) {
            bmp_drawPoint(bmp, i, y, color);
        }
    }
}

/**
 * 在bitmap上画垂直线(优化版本)
 */
void bmp_drawVLine(BITMAP_565 *bmp, int32 x, int32 y1, int32 y2, uint32 color)
{
    int32 i;
    int32 minY, maxY;
    uint16 color565;
    uint32 *bitmap32;
    int alpha;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    if (x < 0 || x >= bmp->width) {
        return;
    }

    minY = (y1 < y2) ? y1 : y2;
    maxY = (y1 < y2) ? y2 : y1;
    minY = (minY < 0) ? 0 : minY;
    maxY = (maxY >= bmp->height) ? bmp->height - 1 : maxY;

    if (bmp->color_bit == 16) {
        color565 = MAKECOLOR565(color);
        alpha = (color >> 24) & 0xff;

        for (i = minY; i <= maxY; i++) {
            bmp_drawPoint(bmp, x, i, color);
        }
    } else if (bmp->color_bit == 32) {
        bitmap32 = (uint32 *)bmp->bitmap;
        for (i = minY; i <= maxY; i++) {
            bmp_drawPoint(bmp, x, i, color);
        }
    }
}

/**
 * 在bitmap上画三角形
 */
void bmp_drawTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
    int minX, maxX, minY, maxY;
    int x, y;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    minX = MIN(MIN(x1, x2), x3);
    maxX = MAX(MAX(x1, x2), x3);
    minY = MIN(MIN(y1, y2), y3);
    maxY = MAX(MAX(y1, y2), y3);

    for (y = minY; y < maxY; y++) {
        for (x = minX; x < maxX; x++) {
            if (pointInTriangle(x, y, x1, y1, x2, y2, x3, y3)) {
                bmp_drawPoint(bmp, x, y, color);
            }
        }
    }
}

/**
 * 在bitmap上画空心三角形
 */
void bmp_drawHollowTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
    bmp_drawLine(bmp, x1, y1, x2, y2, color);
    bmp_drawLine(bmp, x2, y2, x3, y3, color);
    bmp_drawLine(bmp, x3, y3, x1, y1, color);
}

/**
 * 在bitmap上画椭圆
 */
void bmp_drawEllipse(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color)
{
    int32 ix, iy;
    int32 centerX = x + w / 2;
    int32 centerY = y + h / 2;
    int32 a = w / 2;
    int32 b = h / 2;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    for (iy = y; iy < y + h; iy++) {
        for (ix = x; ix < x + w; ix++) {
            int32 dx = ix - centerX;
            int32 dy = iy - centerY;
            if ((dx * dx * b * b + dy * dy * a * a) <= (a * a * b * b)) {
                bmp_drawPoint(bmp, ix, iy, color);
            }
        }
    }
}

/**
 * 在bitmap上画空心椭圆
 */
void bmp_drawHollowEllipse(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color)
{
    int32 angle;
    int32 centerX = x + w / 2;
    int32 centerY = y + h / 2;
    int32 a = w / 2;
    int32 b = h / 2;
    int32 px, py;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    for (angle = 0; angle < 360; angle++) {
        px = centerX + (a * INT_COS(angle)) / TRIG_SCALE;
        py = centerY + (b * INT_SIN(angle)) / TRIG_SCALE;

        if (px >= 0 && px < bmp->width && py >= 0 && py < bmp->height) {
            bmp_drawPoint(bmp, px, py, color);
        }
    }
}

/**
 * 在bitmap上画渐变圆形
 */
void bmp_drawShadeCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 colorA, uint32 colorB)
{
    int16 x, y;
    int32 distance;
    uint8 rA, gA, bA, rB, gB, bB;
    uint8 r, g, b, a;
    uint16 color565;
    uint32 color32;
    int32 ratio;
    uint32 *bitmap32;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    rA = (colorA >> 16) & 0xFF;
    gA = (colorA >> 8) & 0xFF;
    bA = colorA & 0xFF;

    rB = (colorB >> 16) & 0xFF;
    gB = (colorB >> 8) & 0xFF;
    bB = colorB & 0xFF;

    if (bmp->color_bit == 16) {
        for (y = -radius; y <= radius; y++) {
            for (x = -radius; x <= radius; x++) {
                distance = x * x + y * y;
                if (distance <= radius * radius) {
                    ratio = (distance * 255) / (radius * radius);

                    r = (rA * (255 - ratio) + rB * ratio) / 255;
                    g = (gA * (255 - ratio) + gB * ratio) / 255;
                    b = (bA * (255 - ratio) + bB * ratio) / 255;

                    color565 = MAKERGB(r, g, b);

                    if (cx + x >= 0 && cx + x < bmp->width && cy + y >= 0 && cy + y < bmp->height) {
                        bmp->bitmap[(cy + y) * bmp->width + (cx + x)] = color565;
                    }
                }
            }
        }
    } else if (bmp->color_bit == 32) {
        bitmap32 = (uint32 *)bmp->bitmap;
        for (y = -radius; y <= radius; y++) {
            for (x = -radius; x <= radius; x++) {
                distance = x * x + y * y;
                if (distance <= radius * radius) {
                    ratio = (distance * 255) / (radius * radius);

                    r = (rA * (255 - ratio) + rB * ratio) / 255;
                    g = (gA * (255 - ratio) + gB * ratio) / 255;
                    b = (bA * (255 - ratio) + bB * ratio) / 255;
                    a = 0xFF;

                    color32 = (a << 24) | (r << 16) | (g << 8) | b;

                    if (cx + x >= 0 && cx + x < bmp->width && cy + y >= 0 && cy + y < bmp->height) {
                        bitmap32[(cy + y) * bmp->width + (cx + x)] = color32;
                    }
                }
            }
        }
    }
}

/**
 * 将bitmap中不透明像素处理成指定颜色
 */
void bmp_fillColor(BITMAP_565 *bmp, uint32 color)
{
    int32 i;
    uint16 color565;
    uint32 *bitmap32;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    if (bmp->color_bit == 16) {
        color565 = MAKECOLOR565(color);
        for (i = 0; i < bmp->width * bmp->height; i++) {
            if (bmp->bitmap[i] != bmp->transcolor) {
                bmp->bitmap[i] = color565;
            }
        }
    } else if (bmp->color_bit == 32) {
        bitmap32 = (uint32 *)bmp->bitmap;
        for (i = 0; i < bmp->width * bmp->height; i++) {
            if ((bitmap32[i] >> 24) != 0) { // 不是完全透明
                bitmap32[i] = color;
            }
        }
    }
}

/**
 * 在bitmap上画环形
 */
void bmp_drawHollowRing(BITMAP_565 *bmp, int32 cx, int32 cy, int32 innerRadius, int32 outerRadius, uint32 color)
{
    int16 x, y;
    int32 distance;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    for (y = -outerRadius; y <= outerRadius; y++) {
        for (x = -outerRadius; x <= outerRadius; x++) {
            distance = x * x + y * y;
            if (distance <= outerRadius * outerRadius && distance >= innerRadius * innerRadius) {
                bmp_drawPoint(bmp, cx + x, cy + y, color);
            }
        }
    }
}

/**
 * 在bitmap上画扇形
 */
void bmp_drawFanShape(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, int32 startAngle, int32 endAngle, uint32 color)
{
    int16 x, y;
    int32 angle;
    int32 distance;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    // 确保角度在0-360范围内
    startAngle = startAngle % 360;
    endAngle = endAngle % 360;
    if (startAngle < 0) startAngle += 360;
    if (endAngle < 0) endAngle += 360;

    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            distance = x * x + y * y;
            if (distance <= radius * radius) {
                // 计算角度
                angle = (int32)(atan2((double)y, (double)x) * 180.0 / 3.14159265) + 90;
                if (angle < 0) angle += 360;

                // 判断是否在扇形范围内
                if (startAngle <= endAngle) {
                    if (angle >= startAngle && angle <= endAngle) {
                        bmp_drawPoint(bmp, cx + x, cy + y, color);
                    }
                } else {
                    if (angle >= startAngle || angle <= endAngle) {
                        bmp_drawPoint(bmp, cx + x, cy + y, color);
                    }
                }
            }
        }
    }
}

/**
 * 在bitmap上画空心扇形
 */
void bmp_drawHollowFanShape(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, int32 startAngle, int32 endAngle, uint32 color)
{
    int32 angle;
    int32 x, y;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    // 确保角度在0-360范围内
    startAngle = startAngle % 360;
    endAngle = endAngle % 360;
    if (startAngle < 0) startAngle += 360;
    if (endAngle < 0) endAngle += 360;

    // 画两条边
    for (angle = startAngle; angle != endAngle; angle = (angle + 1) % 360) {
        x = cx + (radius * INT_COS(angle)) / TRIG_SCALE;
        y = cy + (radius * INT_SIN(angle)) / TRIG_SCALE;
        bmp_drawPoint(bmp, x, y, color);
    }

    // 画圆弧
    bmp_drawLine(bmp, cx, cy, cx + (radius * INT_COS(startAngle)) / TRIG_SCALE,
                 cy + (radius * INT_SIN(startAngle)) / TRIG_SCALE, color);
    bmp_drawLine(bmp, cx, cy, cx + (radius * INT_COS(endAngle)) / TRIG_SCALE,
                 cy + (radius * INT_SIN(endAngle)) / TRIG_SCALE, color);
}

/**
 * 在bitmap上画镜像圆形渐变
 */
void bmp_drawShadeMirrorCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 colorA, uint32 colorB)
{
    int16 x, y;
    int32 distance;
    uint8 rA, gA, bA, rB, gB, bB;
    uint8 r, g, b, a;
    uint16 color;
    uint32 color32;
    int32 ratio;
    uint32 *bitmap32;

    if (!bmp || bmp->bitmap == NULL) {
        return;
    }

    rA = (colorA >> 16) & 0xFF;
    gA = (colorA >> 8) & 0xFF;
    bA = colorA & 0xFF;

    rB = (colorB >> 16) & 0xFF;
    gB = (colorB >> 8) & 0xFF;
    bB = colorB & 0xFF;

    if (bmp->color_bit == 16) {
        for (y = -radius; y <= radius; y++) {
            for (x = -radius; x <= radius; x++) {
                distance = x * x + y * y;
                if (distance <= radius * radius) {
                    // 镜像渐变：从边缘到中心再到边缘
                    int32 distSqrt = (int32)sqrt((double)distance);
                    if (distSqrt <= radius / 2) {
                        ratio = (distSqrt * 255 * 2) / radius;
                    } else {
                        ratio = ((radius - distSqrt) * 255 * 2) / radius;
                    }

                    r = (rA * (255 - ratio) + rB * ratio) / 255;
                    g = (gA * (255 - ratio) + gB * ratio) / 255;
                    b = (bA * (255 - ratio) + bB * ratio) / 255;

                    color = MAKERGB(r, g, b);

                    if (cx + x >= 0 && cx + x < bmp->width && cy + y >= 0 && cy + y < bmp->height) {
                        bmp->bitmap[(cy + y) * bmp->width + (cx + x)] = color;
                    }
                }
            }
        }
    } else if (bmp->color_bit == 32) {
        bitmap32 = (uint32 *)bmp->bitmap;
        for (y = -radius; y <= radius; y++) {
            for (x = -radius; x <= radius; x++) {
                distance = x * x + y * y;
                if (distance <= radius * radius) {
                    // 镜像渐变：从边缘到中心再到边缘
                    int32 distSqrt = (int32)sqrt((double)distance);
                    if (distSqrt <= radius / 2) {
                        ratio = (distSqrt * 255 * 2) / radius;
                    } else {
                        ratio = ((radius - distSqrt) * 255 * 2) / radius;
                    }

                    r = (rA * (255 - ratio) + rB * ratio) / 255;
                    g = (gA * (255 - ratio) + gB * ratio) / 255;
                    b = (bA * (255 - ratio) + bB * ratio) / 255;
                    a = 0xFF;

                    color32 = (a << 24) | (r << 16) | (g << 8) | b;

                    if (cx + x >= 0 && cx + x < bmp->width && cy + y >= 0 && cy + y < bmp->height) {
                        bitmap32[(cy + y) * bmp->width + (cx + x)] = color32;
                    }
                }
            }
        }
    }
}

/**
 * 将buf图片指定区域绘制到di中
 */
void drawBitmap565Old(BITMAP_565 *di, BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
{
    int32 j, i;

    if (!di || !buf || !di->bitmap || !buf->bitmap) {
        return;
    }

    // 处理目标坐标超出边界的情况
    if (x < 0) {
        w += x;
        sx -= x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        sy -= y;
        y = 0;
    }
    if (x + w > di->width) {
        w = di->width - x;
    }
    if (y + h > di->height) {
        h = di->height - y;
    }

    // 处理源图片超出边界的情况
    if (sx < 0) {
        w += sx;
        sx = 0;
    }
    if (sy < 0) {
        h += sy;
        sy = 0;
    }
    if (sx + w > buf->width) {
        w = buf->width - sx;
    }
    if (sy + h > buf->height) {
        h = buf->height - sy;
    }

    if (w <= 0 || h <= 0) {
        return;
    }

    // 进行像素拷贝
    if (buf->color_bit == 16 && di->color_bit == 16) {
        for (j = 0; j < h; j++) {
            if (buf->mode == BM_TRANSPARENT) {
                for (i = 0; i < w; i++) {
                    int32 src_x = sx + i;
                    int32 src_y = sy + j;
                    uint16 color = buf->bitmap[src_y * buf->width + src_x];

                    if (color != buf->transcolor) {
                        int32 dest_x = x + i;
                        int32 dest_y = y + j;
                        di->bitmap[dest_y * di->width + dest_x] = color;
                    }
                }
            } else {
                mrc_memcpy(di->bitmap + (y + j) * di->width + x,
                          buf->bitmap + (sy + j) * buf->width + sx,
                          w * 2);
            }
        }
    } else if (buf->color_bit == 32 && di->color_bit == 32) {
        uint32 *buf32 = (uint32 *)buf->bitmap;
        uint32 *di32 = (uint32 *)di->bitmap;

        for (j = 0; j < h; j++) {
            if (buf->mode == BM_TRANSPARENT) {
                for (i = 0; i < w; i++) {
                    int32 src_x = sx + i;
                    int32 src_y = sy + j;
                    uint32 color = buf32[src_y * buf->width + src_x];

                    if ((color >> 24) != 0) {
                        int32 dest_x = x + i;
                        int32 dest_y = y + j;
                        di32[dest_y * di->width + dest_x] = color;
                    }
                }
            } else {
                mrc_memcpy(di32 + (y + j) * di->width + x,
                          buf32 + (sy + j) * buf->width + sx,
                          w * 4);
            }
        }
    }
}

/**
 * 将bitmap缩放到指定大小（修改原bitmap）
 */
void bmp_scaleBitmap(BITMAP_565 *src, int32 width, int32 height)
{
    int32 x, y;
    int32 x_ratio, y_ratio;
    int32 src_x, src_y;
    uint16 *new_bitmap;
    uint32 *bitmap32, *new_bitmap32;

    if (!src || !src->bitmap || width <= 0 || height <= 0) {
        return;
    }

    // 分配新的位图内存
    if (src->color_bit == 16) {
        new_bitmap = (uint16 *)mr_malloc(width * height * sizeof(uint16));
        if (!new_bitmap) {
            return;
        }

        // 使用最近邻插值进行缩放
        x_ratio = (src->width << 16) / width + 1;
        y_ratio = (src->height << 16) / height + 1;

        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                src_x = (x * x_ratio) >> 16;
                src_y = (y * y_ratio) >> 16;

                if (src_x >= 0 && src_y >= 0 && src_x < src->width && src_y < src->height) {
                    new_bitmap[y * width + x] = src->bitmap[src_y * src->width + src_x];
                }
            }
        }

        // 释放旧位图，更新为新位图
        mrc_freeFileData(src->bitmap, src->buflen);
        src->bitmap = new_bitmap;
        src->buflen = width * height * sizeof(uint16);
    } else if (src->color_bit == 32) {
        new_bitmap32 = (uint32 *)mr_malloc(width * height * sizeof(uint32));
        if (!new_bitmap32) {
            return;
        }

        bitmap32 = (uint32 *)src->bitmap;
        x_ratio = (src->width << 16) / width + 1;
        y_ratio = (src->height << 16) / height + 1;

        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                src_x = (x * x_ratio) >> 16;
                src_y = (y * y_ratio) >> 16;

                if (src_x >= 0 && src_y >= 0 && src_x < src->width && src_y < src->height) {
                    new_bitmap32[y * width + x] = bitmap32[src_y * src->width + src_x];
                }
            }
        }

        // 释放旧位图，更新为新位图
        mrc_freeFileData(src->bitmap, src->buflen);
        src->bitmap = (uint16 *)new_bitmap32;
        src->buflen = width * height * sizeof(uint32);
    }

    src->width = width;
    src->height = height;
}

/**
 * 辅助函数：翻转位图数据
 */
static void flipBitmapData(uint16 *bitmap, int width, int height, int flip)
{
    uint16 temp;
    int x, y;

    if (flip == 1) {
        // 水平翻转
        for (y = 0; y < height; y++) {
            for (x = 0; x < width / 2; x++) {
                temp = bitmap[y * width + x];
                bitmap[y * width + x] = bitmap[y * width + (width - 1 - x)];
                bitmap[y * width + (width - 1 - x)] = temp;
            }
        }
    } else if (flip == 2) {
        // 垂直翻转
        for (y = 0; y < height / 2; y++) {
            for (x = 0; x < width; x++) {
                temp = bitmap[y * width + x];
                bitmap[y * width + x] = bitmap[(height - 1 - y) * width + x];
                bitmap[(height - 1 - y) * width + x] = temp;
            }
        }
    }
}

/**
 * 辅助函数：翻转32位位图数据
 */
static void flipBitmapData32(uint32 *bitmap, int width, int height, int flip)
{
    uint32 temp;
    int x, y;

    if (flip == 1) {
        // 水平翻转
        for (y = 0; y < height; y++) {
            for (x = 0; x < width / 2; x++) {
                temp = bitmap[y * width + x];
                bitmap[y * width + x] = bitmap[y * width + (width - 1 - x)];
                bitmap[y * width + (width - 1 - x)] = temp;
            }
        }
    } else if (flip == 2) {
        // 垂直翻转
        for (y = 0; y < height / 2; y++) {
            for (x = 0; x < width; x++) {
                temp = bitmap[y * width + x];
                bitmap[y * width + x] = bitmap[(height - 1 - y) * width + x];
                bitmap[(height - 1 - y) * width + x] = temp;
            }
        }
    }
}

/**
 * 旋转/翻转bitmap（修改原bitmap）
 */
void bmpRotate(BITMAP_565 *b, int type, int flip)
{
    int newWidth, newHeight;
    uint16 *newBitmap;
    uint32 *newBitmap32, *bitmap32;
    int x, y;

    if (!b || !b->bitmap || type < 0 || type > 3) {
        return;
    }

    if (type == 0) {
        // 只翻转，不旋转
        if (flip == 1 || flip == 2) {
            if (b->color_bit == 16) {
                flipBitmapData(b->bitmap, b->width, b->height, flip);
            } else if (b->color_bit == 32) {
                flipBitmapData32((uint32 *)b->bitmap, b->width, b->height, flip);
            }
        }
        return;
    }

    if (b->color_bit == 16) {
        switch (type) {
            case 1: // 旋转90度
                newWidth = b->height;
                newHeight = b->width;
                newBitmap = (uint16 *)mr_malloc(newWidth * newHeight * sizeof(uint16));
                if (!newBitmap) return;

                for (y = 0; y < b->height; y++) {
                    for (x = 0; x < b->width; x++) {
                        newBitmap[x * newWidth + (newWidth - 1 - y)] = b->bitmap[y * b->width + x];
                    }
                }
                break;

            case 2: // 旋转180度
                newWidth = b->width;
                newHeight = b->height;
                newBitmap = (uint16 *)mr_malloc(newWidth * newHeight * sizeof(uint16));
                if (!newBitmap) return;

                for (y = 0; y < b->height; y++) {
                    for (x = 0; x < b->width; x++) {
                        newBitmap[(newHeight - 1 - y) * newWidth + (newWidth - 1 - x)] = b->bitmap[y * b->width + x];
                    }
                }
                break;

            case 3: // 旋转270度
                newWidth = b->height;
                newHeight = b->width;
                newBitmap = (uint16 *)mr_malloc(newWidth * newHeight * sizeof(uint16));
                if (!newBitmap) return;

                for (y = 0; y < b->height; y++) {
                    for (x = 0; x < b->width; x++) {
                        newBitmap[(newHeight - 1 - x) * newWidth + y] = b->bitmap[y * b->width + x];
                    }
                }
                break;

            default:
                return;
        }

        // 处理翻转
        if (flip == 1 || flip == 2) {
            flipBitmapData(newBitmap, newWidth, newHeight, flip);
        }

        // 更新位图
        mrc_freeFileData(b->bitmap, b->buflen);
        b->bitmap = newBitmap;
        b->width = newWidth;
        b->height = newHeight;
        b->buflen = newWidth * newHeight * sizeof(uint16);

    } else if (b->color_bit == 32) {
        bitmap32 = (uint32 *)b->bitmap;

        switch (type) {
            case 1: // 旋转90度
                newWidth = b->height;
                newHeight = b->width;
                newBitmap32 = (uint32 *)mr_malloc(newWidth * newHeight * sizeof(uint32));
                if (!newBitmap32) return;

                for (y = 0; y < b->height; y++) {
                    for (x = 0; x < b->width; x++) {
                        newBitmap32[x * newWidth + (newWidth - 1 - y)] = bitmap32[y * b->width + x];
                    }
                }
                break;

            case 2: // 旋转180度
                newWidth = b->width;
                newHeight = b->height;
                newBitmap32 = (uint32 *)mr_malloc(newWidth * newHeight * sizeof(uint32));
                if (!newBitmap32) return;

                for (y = 0; y < b->height; y++) {
                    for (x = 0; x < b->width; x++) {
                        newBitmap32[(newHeight - 1 - y) * newWidth + (newWidth - 1 - x)] = bitmap32[y * b->width + x];
                    }
                }
                break;

            case 3: // 旋转270度
                newWidth = b->height;
                newHeight = b->width;
                newBitmap32 = (uint32 *)mr_malloc(newWidth * newHeight * sizeof(uint32));
                if (!newBitmap32) return;

                for (y = 0; y < b->height; y++) {
                    for (x = 0; x < b->width; x++) {
                        newBitmap32[(newHeight - 1 - x) * newWidth + y] = bitmap32[y * b->width + x];
                    }
                }
                break;

            default:
                return;
        }

        // 处理翻转
        if (flip == 1 || flip == 2) {
            flipBitmapData32(newBitmap32, newWidth, newHeight, flip);
        }

        // 更新位图
        mrc_freeFileData(b->bitmap, b->buflen);
        b->bitmap = (uint16 *)newBitmap32;
        b->width = newWidth;
        b->height = newHeight;
        b->buflen = newWidth * newHeight * sizeof(uint32);
    }
}

/**
 * 按中心点旋转bitmap，返回旋转后的bitmap（创建新bitmap）
 */
BITMAP_565 *bmp_rotateBitmap(BITMAP_565 *src, int32 angle)
{
    BITMAP_565 *dst;
    int32 x, y;
    int32 newX, newY;
    int32 centerX, centerY;
    int32 cosA, sinA;
    int32 maxSize;

    if (!src || !src->bitmap) {
        return NULL;
    }

    // 标准化角度到0-359范围
    angle = angle % 360;
    if (angle < 0) angle += 360;

    // 计算旋转中心
    centerX = src->width / 2;
    centerY = src->height / 2;

    // 使用整数三角函数
    cosA = INT_COS(angle);
    sinA = INT_SIN(angle);

    // 计算旋转后的边界（使用对角线长度作为新图大小）
    maxSize = (int32)sqrt((double)(src->width * src->width + src->height * src->height)) + 1;

    // 创建新位图
    dst = bmp_createBitmap(maxSize, maxSize, src->color_bit);
    if (!dst) {
        return NULL;
    }

    dst->mode = src->mode;
    dst->transcolor = src->transcolor;

    // 填充透明色
    if (src->color_bit == 16) {
        for (y = 0; y < maxSize; y++) {
            for (x = 0; x < maxSize; x++) {
                dst->bitmap[y * maxSize + x] = src->transcolor;
            }
        }
    } else if (src->color_bit == 32) {
        uint32 *dst32 = (uint32 *)dst->bitmap;
        for (y = 0; y < maxSize; y++) {
            for (x = 0; x < maxSize; x++) {
                dst32[y * maxSize + x] = 0; // 透明
            }
        }
    }

    // 旋转映射
    if (src->color_bit == 16) {
        for (y = 0; y < maxSize; y++) {
            for (x = 0; x < maxSize; x++) {
                // 计算相对于新图中心的坐标
                int32 dx = x - maxSize / 2;
                int32 dy = y - maxSize / 2;

                // 反向旋转获取源图坐标
                newX = centerX + (dx * cosA + dy * sinA) / TRIG_SCALE;
                newY = centerY + (-dx * sinA + dy * cosA) / TRIG_SCALE;

                // 检查是否在源图范围内
                if (newX >= 0 && newX < src->width && newY >= 0 && newY < src->height) {
                    dst->bitmap[y * maxSize + x] = src->bitmap[newY * src->width + newX];
                }
            }
        }
    } else if (src->color_bit == 32) {
        uint32 *src32 = (uint32 *)src->bitmap;
        uint32 *dst32 = (uint32 *)dst->bitmap;

        for (y = 0; y < maxSize; y++) {
            for (x = 0; x < maxSize; x++) {
                int32 dx = x - maxSize / 2;
                int32 dy = y - maxSize / 2;

                newX = centerX + (dx * cosA + dy * sinA) / TRIG_SCALE;
                newY = centerY + (-dx * sinA + dy * cosA) / TRIG_SCALE;

                if (newX >= 0 && newX < src->width && newY >= 0 && newY < src->height) {
                    dst32[y * maxSize + x] = src32[newY * src->width + newX];
                }
            }
        }
    }

    return dst;
}


// 保存图片为bma格式
void bmp_saveBitmap(BITMAP_565 *bmp, const char *filename)
{
  int32 f;
  f = mrc_open(filename, MR_FILE_CREATE | MR_FILE_WRONLY);
  if(bmp->color_bit == 32){
    mrc_write(f, "MAP8", 4);
  }
  else{
    mrc_write(f, "MAP5", 4);
  }
  mrc_write(f, (void *)&bmp->width, 4);
  mrc_write(f, (void *)&bmp->height, 4);
  mrc_write(f, (void *)((bmp->mode == BM_COPY) ? 0 : bmp->transcolor), 2);
  mrc_write(f, bmp->bitmap, bmp->width * bmp->height * 2);
  mrc_close(f);
}