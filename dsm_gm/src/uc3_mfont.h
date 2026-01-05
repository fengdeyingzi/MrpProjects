#ifndef UC3_MFONT_H
#define UC3_MFONT_H

#include <mrc_base.h>

// 字体结构体，封装单个字体实例的数据
typedef struct UC3_FONT {
    uint8 *buffer;          // 字体文件数据缓冲区
    int32 buffer_len;       // 缓冲区长度
    int32 font_width;       // 字体图片宽度
    int32 font_height;      // 字体图片高度
    int32 char_count;       // 字符数量
    int is_loaded;          // 是否已加载成功
} UC3_FONT;

// 加载字体文件
UC3_FONT* mfont_load(const char *font_name);

// 释放字体资源
void mfont_free(UC3_FONT *font);

// 使用指定字体绘制文本
int32 mfont_drawText(UC3_FONT *font, const char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode);

// 在矩形区域内绘制多行文本
int32 mfont_drawTextInRect(UC3_FONT *font, const char *pcText, int16 dx, int16 dy, int16 x, int16 y, 
                         int16 width, int16 height, uint8 r, uint8 g, uint8 b, int is_unicode);

// 绘制渐变文字
int32 mfont_drawGradientText(UC3_FONT *font, const char *pcText, int16 x, int16 y, 
                           uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, int is_unicode);

// 横屏绘制文字
int32 mfont_drawTextHor(UC3_FONT *font, const char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode);

// 获取文本宽度
int mfont_getWidth(UC3_FONT *font, const char *pcText, int is_unicode);

// 获取文本高度
int mfont_getHeight(UC3_FONT *font, const char *pcText, int is_unicode);

// 获取字符宽度
int mfont_getCharWidth(UC3_FONT *font, uint16 char_id);

// 内部辅助函数声明
uint8 *mfont_getCharData(UC3_FONT *font, uint16 id, uint32 *out_size);
int mfont_getPixel(uint8 *buf, int n);
uint16 mfont_swap_bytes_shift(uint16 value);

#endif // UC3_MFONT_H