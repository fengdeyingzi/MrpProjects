#include <mrc_base.h>
#ifndef UC3_FONT_H
#define UC3_FONT_H
//初始化uc3字体
int32 uc3_init(void);

// 画uc3字体
int32 uc3_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode);

// 获取文字宽度
int uc3_getWidth(char *pcText, int is_unicode);

// 释放uc3字体
void uc3_free(void);

#endif