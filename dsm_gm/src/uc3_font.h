
#ifndef UC3_FONT_H
#define UC3_FONT_H

#include <mrc_base.h>
//初始化uc3字体
int32 uc3_init(void);

// 画uc3字体
int32 uc3_drawText(const char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode);

// 在矩形区域内绘制多行uc3字体 y坐标可为负数
int32 uc3_drawTextInRect(const char *pcText, int16 dx, int16 dy, int16 x, int16 y, int16 width, int16 height, uint8 r, uint8 g, uint8 b, int is_unicode);

// 返回待显示字符串若显示在宽为w的区间里，需要的行数；
int32 uc3_textRow(const char* pcText, int32 w, int is_unicode);

// 绘制上下渐变文字效果（单行绘制）
int32 uc3_drawGradientText(const char *pcText, int16 x, int16 y, uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, int is_unicode);

//横屏绘制文字
int32 uc3_drawTextHor(const char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode);

// 获取文字宽度
int uc3_getWidth(const char *pcText, int is_unicode);

// 获取文字高度
int uc3_getHeight(const char *pcText, int is_unicode);

// 释放uc3字体
void uc3_free(void);

#endif