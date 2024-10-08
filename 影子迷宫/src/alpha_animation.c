#include <mrc_base.h>
#include "mrc_graphics.h"



void drawBitmapFormAlpha(BITMAP_565 *dst, BITMAP_565 *src, int alpha) {
    uint16 srcColor;
    uint16 dstColor;
    int index;


    // ���ͼƬ����Ƿ�һ��
    if (dst->width != src->width || dst->height != src->height) {
        return; // ���ػ������
    }

    // ����ÿһ�����ؽ��л���
    for ( index = 0; index < dst->width * dst->height; index++) {
             srcColor = src->bitmap[index];
            // ���͸��ɫ
            if (srcColor != src->transcolor) {
                 dstColor = dst->bitmap[index];
                // ���л��
                dst->bitmap[index] = blendColor(dstColor, srcColor, alpha);
            }
        
    }
}