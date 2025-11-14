#include <mrc_base.h>
#include "mrc_graphics.h"



void drawBitmapFormAlpha(BITMAP_565 *dst, BITMAP_565 *src, int alpha) {
    uint16 srcColor;
    uint16 dstColor;
    int index;


    // 检查图片宽高是否一致
    if (dst->width != src->width || dst->height != src->height) {
        return; // 返回或处理错误
    }

    // 遍历每一个像素进行绘制
    for ( index = 0; index < dst->width * dst->height; index++) {
             srcColor = src->bitmap[index];
            // 检查透明色
            if (srcColor != src->transcolor) {
                 dstColor = dst->bitmap[index];
                // 进行混合
                dst->bitmap[index] = blendColor(dstColor, srcColor, alpha);
            }
        
    }
}