#include <mrc_base.h>
#include "xl_debug.h"

uint8 *uc3_buf = NULL;
int32 uc3_buf_len = 0;

// 是否使用UTF-8编码
#define USE_UTF8 1

// 是否横屏绘制
#define USE_HOR_DRAW 0

// 开启位置交换
#define SWAP_FONT 0

extern void drawBitmapBit(
uint8 *font, int x, int y, int w, int h, uint8 r, uint8 g, uint8 b);
extern void drawBitmapBitInRect(
    uint8 *font, int x, int y, int w, int h,
    int rect_x, int rect_y, int rect_w, int rect_h,
    uint8 r, uint8 g, uint8 b);
extern  void drawBitmapBitRotate(uint8 *font, int x, int y, int w, int h, uint8 r, uint8 g, uint8 b, int type);
extern int getUC3Width(int id);
extern uint8 *getCharUC3(uint16 id, uint32 fontw, uint32 fonth, uint32 *out_size);
extern int getFontPixel(uint8 *buf, int n);

int tempfontindex;
int fontWidth;
int fontHeight;
int fontCount;
/*
uc3字体文件结构说明
0-3字节前缀字符串 "FT16"
4-7字节字体图片宽度 fontw
8-11字节字体图片高度 fonth
12-15字节文字数量
16字节开始 记录文字id以及对应的offset,id占2字节,offset占4字节
读取单个字的字节大小dataSize计算为: isAddBit = (fontw * fonth % 8) > 0; dataSize = ((fontw * fonth / 8) + (isAddBit ? 1 : 0));

*/
// 初始化uc3字体
int32 uc3_init(void)
{
    int32 re = 0;
    re = mrc_readFileFromMrpEx(NULL, "font.uc3", &uc3_buf, &uc3_buf_len, 0);
    if (re != MR_SUCCESS)
    {
        mrc_printf("uc3 load error");
        return MR_FAILED;
    }
    mrc_printf("uc3 load success len = %d\n", uc3_buf_len);
    fontWidth = *(uint32 *)(uc3_buf + 4);
    fontHeight = *(uint32 *)(uc3_buf + 8);
    fontCount = *(uint32 *)(uc3_buf + 12);
    return MR_SUCCESS;
}

uint16 swap_bytes_shift(uint16 value) {
    return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
}

int32 uc3_drawText(const char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode)
{
    uint16 *ucbuf = (uint16 *)pcText;
    uint16 *ucptr = ucbuf;
    uint32 out_size = 0;
    int32 err = 0;
    int32 csize = 0;
    int16 ix = x;
    int16 iy = y;
    uint16 *buf = NULL;
    uint16 code = 0;
    uint8 *tempchar = NULL;
    int drawW = 0;
    #if USE_HOR_DRAW
    return uc3_drawTextHor(pcText, x, y, r, g, b, is_unicode);
    #else
    if (is_unicode)
    {
        while (*ucptr)
        {
            code = swap_bytes_shift(*ucptr);
            tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
            drawW = fontWidth;
                if(fontWidth == 12 && code < 128){
                    drawW = 8;
                }
            if(tempchar!=NULL)
                drawBitmapBit(tempchar, ix, iy, drawW, fontHeight, r, g, b);
            ix += getUC3Width(code);
            ucptr++;
        }
    }
    else
    {
        #if USE_UTF8
        {
            uint8 *utf8 = (uint8 *)pcText;
            while (*utf8)
            {
                uint16 unicode;
                if ((*utf8 & 0x80) == 0) {
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }
                code = unicode; //(uint16)((unicode << 8) & 0xff00 | unicode >> 8);
                tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
                drawW = fontWidth;
                if(fontWidth == 12 && code < 128){
                    drawW = 8;
                }
                if (tempchar != NULL)
                    drawBitmapBit(tempchar, ix, iy, drawW, fontHeight, r, g, b);
                ix += getUC3Width(code);
            }
        }
        #else
        buf = mrc_c2u(pcText, &err, &csize);
        if (buf == NULL)
        {
            mrc_printf("mrc_c2u error");
            return MR_FAILED;
        }
        ucptr = buf;
        while (*ucptr && csize > 0)
        {
            uint16 code = swap_bytes_shift(*ucptr);

            uint8 *tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
            drawW = fontWidth;
            if(fontWidth == 12 && code < 128){
                drawW = 8;
            }
            if (tempchar != NULL)
            {
                drawBitmapBit(tempchar, ix, iy, drawW, fontHeight, r, g, b);
            }

            ix += getUC3Width(code);
            ucptr++;
            csize -= 2;
        }
        mrc_free(buf);
        #endif
    }
    
    return 0;
    #endif
}

// 在矩形区域内绘制多行uc3字体，并可自动换行，
// dx, dy表示相对于矩形(x,y)的绘制偏移量，可以为负数
// 实际绘制起始位置为(x+dx, y+dy)
int32 uc3_drawTextInRect(const char *pcText, int16 dx, int16 dy, int16 x, int16 y, int16 width, int16 height, uint8 r, uint8 g, uint8 b, int is_unicode) {
    int16 ix = x + dx;  // 使用dx作为水平偏移
    int16 iy = y + dy;  // 使用dy作为垂直偏移
    int16 line_height;
    uint8 *utf8 = NULL;
    uint16 *ucbuf = NULL;
    uint16 *ucptr = NULL;
    uint32 out_size = 0;
    int32 err = 0;
    int32 csize = 0;
    uint16 code = 0;
    uint8 *tempchar = NULL;
    int char_width = 0;
    int drawW = 0;
    int skip_lines;
    line_height = fontHeight + 2;

    // 如果iy为负数，调整绘制起始位置（考虑dy偏移）
    if (iy < 0) {
        // 跳过前几行不可见的部分
        skip_lines = (-iy) / line_height;
        iy += skip_lines * line_height;
    }
    
    if (is_unicode) {
        ucbuf = (uint16 *)pcText;
        ucptr = ucbuf;
        
        while (*ucptr && iy < y + height) {
            // 换行检查
            if (ix + getUC3Width(*ucptr) > x + width) {
                ix = x + dx;  // 换行时重置到x+dx位置
                iy += line_height;

                // 检查是否超出矩形底部
                if (iy >= y + height) {
                    break;
                }
            }
            
            code = swap_bytes_shift(*ucptr);
            tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
            drawW = fontWidth;
            if(fontWidth == 12 && code < 128){
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                // 只绘制在矩形区域内的部分
                if (iy >= y && iy + fontHeight <= y + height) {
                    drawBitmapBit(tempchar, ix, iy, drawW, fontHeight, r, g, b);
                } else if (iy < y && iy + fontHeight > y) {
                    // 部分在矩形上方，部分在矩形内
                    drawBitmapBitInRect(tempchar, ix, iy, drawW, fontHeight, x, y, width, height, r, g, b);
                } else if (iy < y + height && iy + fontHeight > y + height) {
                    // 部分在矩形下方，部分在矩形内
                    drawBitmapBitInRect(tempchar, ix, iy, drawW, fontHeight, x, y, width, height, r, g, b);
                }
            }
            
            ix += getUC3Width(code);
            ucptr++;
        }
    } else {
        #if USE_UTF8
        {
            utf8 = (uint8 *)pcText;
            
            while (*utf8 && iy < y + height) {
                uint16 unicode;
                int bytes = 0;

                // UTF-8解码
                if ((*utf8 & 0x80) == 0) {
                    unicode = *utf8;
                    bytes = 1;
                } else if ((*utf8 & 0x20) == 0) {
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    bytes = 2;
                } else {
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    bytes = 3;
                }

                char_width = getUC3Width(unicode);

                // 换行检查
                if (ix + char_width > x + width) {
                    ix = x + dx;  // 换行时重置到x+dx位置
                    iy += line_height;

                    // 检查是否超出矩形底部
                    if (iy >= y + height) {
                        break;
                    }
                }
                
                tempchar = getCharUC3(unicode, fontWidth, fontHeight, &out_size);
                drawW = fontWidth;
                if(fontWidth == 12 && unicode < 128){
                    drawW = 8;
                }
                
                if (tempchar != NULL) {
                    // 只绘制在矩形区域内的部分
                    if (iy >= y && iy + fontHeight <= y + height) {
                        drawBitmapBit(tempchar, ix, iy, drawW, fontHeight, r, g, b);
                    } else if (iy < y && iy + fontHeight > y) {
                        // 部分在矩形上方，部分在矩形内
                        drawBitmapBitInRect(tempchar, ix, iy, drawW, fontHeight, x, y, width, height, r, g, b);
                    } else if (iy < y + height && iy + fontHeight > y + height) {
                        // 部分在矩形下方，部分在矩形内
                        drawBitmapBitInRect(tempchar, ix, iy, drawW, fontHeight, x, y, width, height, r, g, b);
                    }
                }
                
                ix += char_width;
                utf8 += bytes;
            }
        }
        #else
        // 非UTF-8编码的处理（使用原有的mrc_c2u转换）
        buf = mrc_c2u(pcText, &err, &csize);
        if (buf == NULL) {
            mrc_printf("mrc_c2u error");
            return MR_FAILED;
        }
        
        ucptr = buf;
        while (*ucptr && csize > 0 && iy < y + height) {
            // 换行检查
            if (ix + getUC3Width(*ucptr) > x + width) {
                ix = x + dx;  // 换行时重置到x+dx位置
                iy += line_height;

                // 检查是否超出矩形底部
                if (iy >= y + height) {
                    break;
                }
            }
            
            code = swap_bytes_shift(*ucptr);
            tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
            drawW = fontWidth;
            if(fontWidth == 12 && code < 128){
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                // 只绘制在矩形区域内的部分
                if (iy >= y && iy + fontHeight <= y + height) {
                    drawBitmapBit(tempchar, ix, iy, drawW, fontHeight, r, g, b);
                } else if (iy < y && iy + fontHeight > y) {
                    // 部分在矩形上方，部分在矩形内
                    drawBitmapBitInRect(tempchar, ix, iy, drawW, fontHeight, x, y, width, height, r, g, b);
                } else if (iy < y + height && iy + fontHeight > y + height) {
                    // 部分在矩形下方，部分在矩形内
                    drawBitmapBitInRect(tempchar, ix, iy, drawW, fontHeight, x, y, width, height, r, g, b);
                }
            }
            
            ix += getUC3Width(code);
            ucptr++;
            csize -= 2;
        }
        
        mrc_free(buf);
        #endif
    }
    
    return MR_SUCCESS;
}


// 绘制上下渐变文字效果（单行绘制）
int32 uc3_drawGradientText(const char *pcText, int16 x, int16 y, uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, int is_unicode) {
    char temp[30];
    uint16 *ucbuf = NULL;
    uint16 *ucptr = NULL;
    uint8 *utf8 = NULL;
    uint32 out_size = 0;
    int32 err = 0;
    int32 csize = 0;
    int16 ix = x;
    int16 iy = y;
    uint16 *buf = NULL;
    uint16 code = 0;
    uint8 *tempchar = NULL;
    uint8 r_cur,g_cur,b_cur;
    uint16 unicode;
    int bytes = 0;
    int char_width = 0;
    int drawW = 0;
    int ix_temp;
    float r_step, g_step, b_step;
    int n = 0;
                    int row, col;
                    float testa,testb;
    LOG_MSG("开始绘制渐变");
    // 计算颜色渐变参数
     r_step = (float)(r2 - r1) / fontHeight;
     toCharEx(r_step, temp);
     LOG_VAR("r_step = %s", temp);
     g_step = (float)(g2 - g1) / fontHeight;
        toCharEx(g_step, temp);
        LOG_VAR("g_step = %s", temp);
     b_step = (float)(b2 - b1) / fontHeight;
        toCharEx(b_step, temp);
        LOG_VAR("b_step = %s", temp);
testa = 0.0f;
testb = 0.0f;
    
    if (is_unicode) {
        ucbuf = (uint16 *)pcText;
        ucptr = ucbuf;
        
        while (*ucptr) {
            code = swap_bytes_shift(*ucptr);
            tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
            drawW = fontWidth;
            if(fontWidth == 12 && code < 128){
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                // 使用渐变颜色绘制字符

                
                for (row = 0; row < fontHeight; row++) {
                    // 计算当前行的颜色
                    toCharEx(r_step, temp);
                    LOG_VAR("r_step = %s", temp);
                    toCharEx(g_step, temp);
                    LOG_VAR("g_step = %s", temp);
                    toCharEx(b_step, temp);
                    LOG_VAR("b_step = %s", temp);
                     r_cur = r1 + (uint8)(r_step * row);
                     g_cur = g1 + (uint8)(g_step * row);
                     b_cur = b1 + (uint8)(b_step * row);
                    
                     ix_temp = ix;
                    
                    for (col = 0; col < drawW; col++) {
                        if (getFontPixel(tempchar, n++) >= 1) {
                            mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                        }
                        ix_temp++;
                    }
                }
            }
            
            ix += getUC3Width(code);
            ucptr++;
        }
    } else {
        #if USE_UTF8
        {
            utf8 = (uint8 *)pcText;
            
            while (*utf8) {
                
                
                // UTF-8解码
                if ((*utf8 & 0x80) == 0) {
                    unicode = *utf8;
                    bytes = 1;
                } else if ((*utf8 & 0x20) == 0) {
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    bytes = 2;
                } else {
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    bytes = 3;
                }
                
                tempchar = getCharUC3(unicode, fontWidth, fontHeight, &out_size);
                drawW = fontWidth;
                if(fontWidth == 12 && unicode < 128){
                    drawW = 8;
                }
                
                if (tempchar != NULL) {
                    // 使用渐变颜色绘制字符
                    
                    
                    for (row = 0; row < fontHeight; row++) {
                        // 计算当前行的颜色
                    toCharEx(r_step, temp);
                    LOG_VAR("r_step = %s", temp);
                    toCharEx(g_step, temp);
                    LOG_VAR("g_step = %s", temp);
                    toCharEx(b_step, temp);
                    LOG_VAR("b_step = %s", temp);
                    
                    LOG_VAR("%s", toCharEx(r_step * row, temp));
                    LOG_VAR("%s", toCharEx(g_step * row, temp));
                    LOG_VAR("%s", toCharEx(b_step * row, temp));
                    LOG_VAR("%d", (int)(r_step * row));
                    LOG_VAR("%c", ((uint8)(testa * testb), temp));
                    // LOG_VAR("%d", (uint8)(g_step * row));
                    // LOG_VAR("%d", (uint8)(b_step * row));
                        // 计算当前行的颜色
                         r_cur = r1 + (int)(r_step * row);
                         g_cur = g1 + (int)(g_step * row);
                         b_cur = b1 + (int)(b_step * row);
                         
                         ix_temp = ix;
                        
                        for (col = 0; col < drawW; col++) {
                            if (getFontPixel(tempchar, n++) >= 1) {
                                mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                            }
                            ix_temp++;
                        }
                    }
                }
                
                ix += getUC3Width(unicode);
                utf8 += bytes;
            }
        }
        #else
        buf = mrc_c2u(pcText, &err, &csize);
        if (buf == NULL) {
            mrc_printf("mrc_c2u error");
            return MR_FAILED;
        }
        
        ucptr = buf;
        while (*ucptr && csize > 0) {
            code = swap_bytes_shift(*ucptr);
            tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
            drawW = fontWidth;
            if(fontWidth == 12 && code < 128){
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                // 使用渐变颜色绘制字符
                int n = 0;
                int row, col;
                
                for (row = 0; row < fontHeight; row++) {
                    // 计算当前行的颜色
                    uint8 r_cur = r1 + (uint8)(r_step * row);
                    uint8 g_cur = g1 + (uint8)(g_step * row);
                    uint8 b_cur = b1 + (uint8)(b_step * row);
                    
                    int ix_temp = ix;
                    
                    for (col = 0; col < drawW; col++) {
                        if (getFontPixel(tempchar, n++) >= 1) {
                            mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                        }
                        ix_temp++;
                    }
                }
            }
            
            ix += getUC3Width(code);
            ucptr++;
            csize -= 2;
        }
        
        mrc_free(buf);
        #endif
    }
    
    return MR_SUCCESS;
}
/*
// 绘制上下渐变文字效果（单行绘制）
int32 uc3_drawGradientText(const char *pcText, int16 x, int16 y, uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, int is_unicode) {
    uint16 *ucbuf = NULL;
    uint16 *ucptr = NULL;
    uint8 *utf8 = NULL;
    uint32 out_size = 0;
    int32 err = 0;
    int32 csize = 0;
    int16 ix = x;
    int16 iy = y;
    uint16 *buf = NULL;
    uint16 code = 0;
    uint8 *tempchar = NULL;
    int r_cur, g_cur, b_cur;
    int row, col;
    int ix_temp;
    int n;
    int char_width = 0;
    int drawW = 0;
    
    // 计算颜色渐变参数（使用整数运算避免浮点）
    int16 r_diff = (int16)r2 - (int16)r1;
    int16 g_diff = (int16)g2 - (int16)g1;
    int16 b_diff = (int16)b2 - (int16)b1;
    
    if (is_unicode) {
        ucbuf = (uint16 *)pcText;
        ucptr = ucbuf;
        
        while (*ucptr) {
            code = swap_bytes_shift(*ucptr);
            tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
            drawW = fontWidth;
            if(fontWidth == 12 && code < 128){
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                // 使用渐变颜色绘制字符
                int n = 0;
                int row, col;
                
                for (row = 0; row < fontHeight; row++) {
                    // 计算当前行的颜色（使用整数运算）
                    uint8 r_cur = r1 + (uint8)((r_diff * row) / fontHeight);
                    uint8 g_cur = g1 + (uint8)((g_diff * row) / fontHeight);
                    uint8 b_cur = b1 + (uint8)((b_diff * row) / fontHeight);

                    int ix_temp = ix;

                    for (col = 0; col < drawW; col++) {
                        if (getFontPixel(tempchar, n++) >= 1) {
                            mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                        }
                        ix_temp++;
                    }
                }
            }
            
            ix += getUC3Width(code);
            ucptr++;
        }
    } else {
        #if USE_UTF8
        {
            utf8 = (uint8 *)pcText;
            LOG_MSG("");
            while (*utf8) {
                uint16 unicode;
                int bytes = 0;
                LOG_MSG("");
                // UTF-8解码
                if ((*utf8 & 0x80) == 0) {
                    unicode = *utf8;
                    bytes = 1;
                } else if ((*utf8 & 0x20) == 0) {
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    bytes = 2;
                } else {
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    bytes = 3;
                }
                LOG_MSG("");
                tempchar = getCharUC3(unicode, fontWidth, fontHeight, &out_size);
                drawW = fontWidth;
                if(fontWidth == 12 && unicode < 128){
                    drawW = 8;
                }
                LOG_MSG("");
                if (tempchar != NULL) {
                    // 使用渐变颜色绘制字符
                     n = 0;
                    
                    LOG_MSG("");
                    for (row = 0; row < fontHeight; row++) {
                        LOG_MSG("计算");
                        // 计算当前行的颜色（使用整数运算）
                         r_cur = r1 + (uint8)((r_diff * row) / fontHeight);
                         LOG_MSG("计算");
                         g_cur = g1 + (uint8)((g_diff * row) / fontHeight);
                         LOG_MSG("计算");
                         b_cur = b1 + (uint8)((b_diff * row) / fontHeight);

                        ix_temp = ix;
                        LOG_MSG("");
                        for (col = 0; col < drawW; col++) {
                            LOG_MSG("");
                            if (getFontPixel(tempchar, n++) >= 1) {
                                mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                            }
                            ix_temp++;
                        }
                        LOG_MSG("");
                    }
                    LOG_MSG("绘制完成");
                }
                LOG_MSG("success..........");
                ix += getUC3Width(unicode);
                utf8 += bytes;
            }
        }
        #else
        buf = mrc_c2u(pcText, &err, &csize);
        if (buf == NULL) {
            mrc_printf("mrc_c2u error");
            return MR_FAILED;
        }
        
        ucptr = buf;
        while (*ucptr && csize > 0) {
            code = swap_bytes_shift(*ucptr);
            tempchar = getCharUC3(code, fontWidth, fontHeight, &out_size);
            drawW = fontWidth;
            if(fontWidth == 12 && code < 128){
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                // 使用渐变颜色绘制字符
                int n = 0;
                int row, col;
                
                for (row = 0; row < fontHeight; row++) {
                    // 计算当前行的颜色（使用整数运算）
                    uint8 r_cur = r1 + (uint8)((r_diff * row) / fontHeight);
                    uint8 g_cur = g1 + (uint8)((g_diff * row) / fontHeight);
                    uint8 b_cur = b1 + (uint8)((b_diff * row) / fontHeight);

                    int ix_temp = ix;

                    for (col = 0; col < drawW; col++) {
                        if (getFontPixel(tempchar, n++) >= 1) {
                            mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                        }
                        ix_temp++;
                    }
                }
            }
            
            ix += getUC3Width(code);
            ucptr++;
            csize -= 2;
        }
        
        mrc_free(buf);
        #endif
    }
    
    return MR_SUCCESS;
}
*/

//横屏绘制文字
int32 uc3_drawTextHor(const char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode)
{
    mr_screeninfo screen;
    uint16 *ucbuf = (uint16 *)pcText;
    uint16 *ucptr = ucbuf;
    uint32 out_size = 0;
    int32 err = 0;
    int32 csize = 0;
    int16 ix = 0;
    int16 iy = x;
    uint16 *buf = NULL;
    uint16 code = 0;
    uint8 *tempchar = NULL;
    mrc_getScreenInfo(&screen);
    ix = screen.width - y - 16;
    if (is_unicode)
    {
        while (*ucptr)
        {
            code = *ucptr;
            tempchar = getCharUC3((uint16)((code << 8) & 0xff00 | code >> 8), fontWidth, fontHeight, &out_size);
            drawBitmapBitRotate(tempchar, ix, iy, fontWidth, fontHeight, r, g, b, 1);
            iy += getUC3Width((uint16)((code << 8) & 0xff00 | code >> 8));
            ucptr++;
        }
    }
    else
    {
        #if USE_UTF8
        {
            uint8 *utf8 = (uint8 *)pcText;
            while (*utf8)
            {
                uint16 unicode;
                if ((*utf8 & 0x80) == 0) {
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }
                code = unicode; //(uint16)((unicode << 8) & 0xff00 | unicode >> 8);
                tempchar = getCharUC3(unicode, fontWidth, fontHeight, &out_size);
                if (tempchar != NULL)
                    drawBitmapBitRotate(tempchar, ix, iy, fontWidth, fontHeight, r, g, b, 1);
                iy += getUC3Width(unicode);
            }
        }
        #else
        buf = mrc_c2u(pcText, &err, &csize);
        if (buf == NULL)
        {
            mrc_printf("mrc_c2u error");
            return MR_FAILED;
        }
        ucptr = buf;
        while (*ucptr && csize > 0)
        {
            code = *ucptr;

            tempchar = getCharUC3((uint16)((code << 8) & 0xff00 | code >> 8), fontWidth, fontHeight, &out_size);
            if (tempchar != NULL)
            {
                drawBitmapBitRotate(tempchar, ix, iy, fontWidth, fontHeight, r, g, b, 1);
            }

            iy += getUC3Width((uint16)((code << 8) & 0xff00 | code >> 8));
            ucptr++;
            csize -= 2;
        }
        mrc_free(buf);
        #endif
    }
    return 0;
}

int getUC3Width(int id)
{
    return id < 128 ? 8 : fontWidth;
}

int uc3_getWidth(const char *pcText, int is_unicode)
{
    uint16 *ucbuf = (uint16 *)pcText;
    uint16 *ucptr = ucbuf;
    int32 err = 0;
    int32 csize = 0;
    int16 iw = 0;
    uint16 *buf = NULL;
    uint16 code = 0;

    if (is_unicode)
    {
        while (*ucptr)
        {
            code = *ucptr;
            iw += getUC3Width((uint16)(code << 8 & 0xff00 | code >> 8));
            ucptr++;
        }
    }
    else
    {
        #if USE_UTF8
        {
            uint8 *utf8 = (uint8 *)pcText;
            while (*utf8)
            {
                uint16 unicode;
                if ((*utf8 & 0x80) == 0) {
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }
                iw += getUC3Width(unicode);
            }
        }
        #else
        // mrc_printf("start con");
        buf = mrc_c2u(pcText, &err, &csize);
        if (buf == NULL)
        {
            mrc_printf("mrc_c2u error");
            return MR_FAILED;
        }
        ucptr = buf;
        while (*ucptr && csize > 0)
        {
            uint16 code = *ucptr;

            iw += getUC3Width((uint16)(code << 8 & 0xff00 | code >> 8));
            ucptr++;
            csize -= 2;
        }
        mrc_free(buf);
        #endif
    }
    return iw;
}

// 旋转绘制二进制位图
// type取值 1:90 2:180 3:270
void drawBitmapBitRotate(uint8 *font, int x, int y, int fontWidth, int fontHeight, uint8 r, uint8 g, uint8 b, int type) {
    
    int iy = 0;
    int bit = 0;

    switch(type) {
        case 1: // 90度旋转
            for (iy = 0; iy < fontWidth; iy++) {
                for (bit = 0; bit < fontHeight; bit++) {
                    if (getFontPixel(font, iy * 16 + bit) >= 1) {
                        mrc_drawPointEx(x + (15-iy), y + (bit), r, g, b);
                    }
                }
            }
            break;
        case 2: // 180度旋转
            for (iy = 0; iy < fontHeight; iy++) {
                for (bit = 0; bit < fontWidth; bit++) {
                    if (getFontPixel(font, (15 - iy) * 16 + (15 - bit)) >= 1) {
                        mrc_drawPointEx(x + bit, y + iy, r, g, b);
                    }
                }
            }
            break;
        case 3: // 270度旋转
            for (iy = 0; iy < fontWidth; iy++) {
                for (bit = 0; bit < fontHeight; bit++) {
                    if (getFontPixel(font, iy * 16 + bit) >= 1) {
                        mrc_drawPointEx(x + iy, y + (15-bit), r, g, b);
                    }
                }
            }
            break;
        default:
            // 处理无效的type值
            break;
    }
}

// 绘制二进制位图数据
// x,y 绘制在屏幕上的坐标点
// r,g,b 绘制的颜色
void drawBitmapBit(uint8 *font, int x, int y, int w, int h, uint8 r, uint8 g, uint8 b)
{
    int n = 0;
    int iy = 0;
    int bit = 0;
    for (iy = y; iy < y + h; iy++)
    {
        int ix = x;

        for (bit = 0; bit < w; bit++)
        {
            if (getFontPixel(font, n++) >= 1)
            {
                mrc_drawPointEx(ix, iy, r, g, b);
            }
            ix++;
        }
    }
}

// 在指定区域内绘制二进制位图数据
void drawBitmapBitInRect(
    uint8 *font, int x, int y, int w, int h,
    int rect_x, int rect_y, int rect_w, int rect_h,
    uint8 r, uint8 g, uint8 b)
{
    int n = 0;
    int iy = 0;
    int bit = 0;
    int ix;
    for (iy = y; iy < y + h; iy++)
    {
        if (iy < rect_y || iy >= rect_y + rect_h)
        {
            n += w;
            continue;
        }

        ix = x;

        for (bit = 0; bit < w; bit++)
        {
            if (ix >= rect_x && ix < rect_x + rect_w)
            {
                if (getFontPixel(font, n) >= 1)
                {
                    mrc_drawPointEx(ix, iy, r, g, b);
                }
            }
            n++;
            ix++;
        }
    }
}


//获取二进制位图中第n个点信息
int getFontPixel(uint8 *buf, int n)
{
    // Calculate which byte and bit position
    int byteIndex = n >> 3; // n / 8;
    return (128 >> (n & 7)) & buf[byteIndex];
    // return (128 >> (n % 8)) & buf[byteIndex];
}

uint8 *getCharUC3(uint16 id, uint32 fontw, uint32 fonth, uint32 *out_size)
{
    int i = 0;

    int startPos = 16;
    int endPos = 0;
    int isAddBit = 0;
    uint32 dataSize = 0;
    uint16 tempid;
    uint32 tempoffset;
    uint8 tempsw[6];

    endPos = startPos + fontCount * 6;
    isAddBit = (fontw * fonth % 8) > 0;




    if (uc3_buf_len < 16)
    {
        *out_size = 0;
        return NULL; // Invalid buffer size
    }
    // Iterate through font entries
    for (i = startPos; i < endPos; i += 6)
    {

        // mrc_memcpy(&tempid, uc3_buf + i, sizeof(uint16));
        tempid = *(uint16 *)(uc3_buf + i);
        if (tempid == id)
        {

            // tempoffset = *(uint32 *)(uc3_buf + i + 2);
            mrc_memcpy(&tempoffset, uc3_buf + i + 2, sizeof(uint32));
            #if SWAP_FONT
            if (i - startPos > 600)
            {
                tempfontindex++;
                if (tempfontindex >= 100)
                {
                    tempfontindex = 0;
                }
                // 交换位置

                mrc_memcpy(tempsw, uc3_buf + i, 6);
                mrc_memcpy(uc3_buf + i, uc3_buf + tempfontindex * 6 + startPos, 6);
                mrc_memcpy(uc3_buf + tempfontindex * 6 + startPos, tempsw, 6);
            }
            #endif
            dataSize = (uint32)((fontw * fonth / 8) + (isAddBit ? 1 : 0));
            if (tempoffset + dataSize <= uc3_buf_len)
            {
                *out_size = dataSize;
                return uc3_buf + tempoffset;
            }
            else
            {
                *out_size = 0;
                return NULL; // Out of bounds
            }
        }
    }

    // If not found, return an empty buffer
    *out_size = (fontw * fonth / 8) + (isAddBit ? 1 : 0);

    return NULL;
}

// 释放uc3字体
void uc3_free(void)
{
    if (uc3_buf != NULL)
    {
        mrc_freeFileData(uc3_buf, uc3_buf_len);
        uc3_buf = NULL;
        uc3_buf_len = 0;
    }
}