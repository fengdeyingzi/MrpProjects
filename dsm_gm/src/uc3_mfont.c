#include <mrc_base.h>
#include "xl_debug.h"
#include "uc3_mfont.h"

// 是否使用UTF-8编码
#define USE_UTF8 1

// 是否横屏绘制
#define USE_HOR_DRAW 0

// 开启位置交换
#define SWAP_FONT 0

// 外部绘制函数声明
extern void drawBitmapBit(uint8 *font, int x, int y, int w, int h, uint8 r, uint8 g, uint8 b);
extern void drawBitmapBitInRect(uint8 *font, int x, int y, int w, int h,
    int rect_x, int rect_y, int rect_w, int rect_h,
    uint8 r, uint8 g, uint8 b);
extern void drawBitmapBitRotate(uint8 *font, int x, int y, int w, int h, uint8 r, uint8 g, uint8 b, int type);

// 字节序交换
uint16 mfont_swap_bytes_shift(uint16 value) {
    return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
}

// 加载字体文件
UC3_FONT* mfont_load(const char *font_name) {
    UC3_FONT *font = (UC3_FONT*)mrc_malloc(sizeof(UC3_FONT));
    if (font == NULL) {
        mrc_printf("mfont_load: malloc failed");
        return NULL;
    }
    
    // 初始化结构体
    font->buffer = NULL;
    font->buffer_len = 0;
    font->font_width = 0;
    font->font_height = 0;
    font->char_count = 0;
    font->is_loaded = 0;
    
    // 读取字体文件
    // font->buffer = mrc_readFileFromMrpEx(NULL, font_name, &font->buffer_len, 0);
    font->buffer = mrc_readFileFromMrp(font_name, &font->buffer_len, 0);
    if (font->buffer == NULL) {
        mrc_printf("mfont_load: failed to load font file: %s", font_name);
        mrc_free(font);
        return NULL;
    }
    
    // 检查文件大小是否足够
    if (font->buffer_len < 16) {
        mrc_printf("mfont_load: font file too small: %s", font_name);
        mrc_freeFileData(font->buffer, font->buffer_len);
        mrc_free(font);
        return NULL;
    }
    
    // 解析字体头信息
    // 检查文件头标识 "FT16"
    if (font->buffer[0] != 'F' || font->buffer[1] != 'T' || 
        font->buffer[2] != '1' || font->buffer[3] != '6') {
        mrc_printf("mfont_load: invalid font format: %s", font_name);
        mrc_freeFileData(font->buffer, font->buffer_len);
        mrc_free(font);
        return NULL;
    }
    
    // 读取字体信息
    font->font_width = *(uint32*)(font->buffer + 4);
    font->font_height = *(uint32*)(font->buffer + 8);
    font->char_count = *(uint32*)(font->buffer + 12);
    font->is_loaded = 1;
    
    mrc_printf("mfont_load: font loaded successfully: %s (w=%d, h=%d, count=%d)", 
               font_name, font->font_width, font->font_height, font->char_count);
    
    return font;
}

// 释放字体资源
void mfont_free(UC3_FONT *font) {
    if (font == NULL) return;
    
    if (font->buffer != NULL) {
        mrc_freeFileData(font->buffer, font->buffer_len);
        font->buffer = NULL;
        font->buffer_len = 0;
    }
    
    mrc_free(font);
}

// 获取字符数据
uint8 *mfont_getCharData(UC3_FONT *font, uint16 id, uint32 *out_size) {
    if (font == NULL || !font->is_loaded || font->buffer == NULL) {
        *out_size = 0;
        return NULL;
    }
    
    static int tempfontindex = 0; // SWAP_FONT使用的静态变量
    int i;
    int startPos = 16;
    int endPos = startPos + font->char_count * 6;
    int isAddBit = (font->font_width * font->font_height % 8) > 0;
    uint32 dataSize = 0;
    uint16 tempid;
    uint32 tempoffset;
    uint8 tempsw[6];
    
    // 遍历字符表查找目标字符
    for (i = startPos; i < endPos; i += 6) {
        tempid = *(uint16*)(font->buffer + i);
        
        if (tempid == id) {
            // tempoffset = *(uint32 *)(font->buffer + i + 2);
            mrc_memcpy(&tempoffset, font->buffer + i + 2, sizeof(uint32));
            
            #if SWAP_FONT
            if (i - startPos > 600) {
                tempfontindex++;
                if (tempfontindex >= 100) {
                    tempfontindex = 0;
                }
                // 交换位置
                mrc_memcpy(tempsw, font->buffer + i, 6);
                mrc_memcpy(font->buffer + i, font->buffer + tempfontindex * 6 + startPos, 6);
                mrc_memcpy(font->buffer + tempfontindex * 6 + startPos, tempsw, 6);
            }
            #endif
            
            dataSize = (uint32)((font->font_width * font->font_height / 8) + (isAddBit ? 1 : 0));
            if (tempoffset + dataSize <= font->buffer_len) {
                *out_size = dataSize;
                return font->buffer + tempoffset;
            } else {
                *out_size = 0;
                return NULL;
            }
        }
    }
    
    // 字符未找到
    dataSize = (font->font_width * font->font_height / 8) + (isAddBit ? 1 : 0);
    *out_size = dataSize;
    return NULL;
}

// 获取位图像素值
int mfont_getPixel(uint8 *buf, int n) {
    int byteIndex = n >> 3;
    return (128 >> (n & 7)) & buf[byteIndex];
}

// 获取字符宽度
int mfont_getCharWidth(UC3_FONT *font, uint16 char_id) {
    if (font == NULL) return 8; // 默认宽度
    
    return char_id < 128 ? font->font_width * 6/11 : font->font_width;
}

// 使用指定字体绘制文本
int32 mfont_drawText(UC3_FONT *font, const char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode) {
    if (font == NULL || !font->is_loaded) {
        mrc_printf("mfont_drawText: font not loaded");
        return MR_FAILED;
    }
    
    #if USE_HOR_DRAW
    return mfont_drawTextHor(font, pcText, x, y, r, g, b, is_unicode);
    #else
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
    
    if (is_unicode) {
        while (*ucptr) {
            code = mfont_swap_bytes_shift(*ucptr);
            tempchar = mfont_getCharData(font, code, &out_size);
            drawW = font->font_width;
            if (font->font_width == 12 && code < 128) {
                drawW = 8;
            }
            if (tempchar != NULL)
                drawBitmapBit(tempchar, ix, iy, drawW, font->font_height, r, g, b);
            ix += mfont_getCharWidth(font, code);
            ucptr++;
        }
    } else {
        #if USE_UTF8
        {
            uint8 *utf8 = (uint8 *)pcText;
            while (*utf8) {
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
                code = unicode;
                tempchar = mfont_getCharData(font, code, &out_size);
                drawW = font->font_width;
                if (font->font_width == 12 && code < 128) {
                    drawW = 8;
                }
                if (tempchar != NULL)
                    drawBitmapBit(tempchar, ix, iy, drawW, font->font_height, r, g, b);
                ix += mfont_getCharWidth(font, code);
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
            code = mfont_swap_bytes_shift(*ucptr);
            tempchar = mfont_getCharData(font, code, &out_size);
            drawW = font->font_width;
            if (font->font_width == 12 && code < 128) {
                drawW = 8;
            }
            if (tempchar != NULL) {
                drawBitmapBit(tempchar, ix, iy, drawW, font->font_height, r, g, b);
            }
            ix += mfont_getCharWidth(font, code);
            ucptr++;
            csize -= 2;
        }
        mrc_free(buf);
        #endif
    }
    
    return MR_SUCCESS;
    #endif
}

// 在矩形区域内绘制多行文本
int32 mfont_drawTextInRect(UC3_FONT *font, const char *pcText, int16 dx, int16 dy, int16 x, int16 y, 
                         int16 width, int16 height, uint8 r, uint8 g, uint8 b, int is_unicode) {
    if (font == NULL || !font->is_loaded) {
        mrc_printf("mfont_drawTextInRect: font not loaded");
        return MR_FAILED;
    }
    
    int16 ix = x + dx;
    int16 iy = y + dy;
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
    
    line_height = font->font_height + 2;
    
    // 调整起始位置
    if (iy < 0) {
        skip_lines = (-iy) / line_height;
        iy += skip_lines * line_height;
    }
    
    if (is_unicode) {
        ucbuf = (uint16 *)pcText;
        ucptr = ucbuf;
        
        while (*ucptr && iy < y + height) {
            char_width = mfont_getCharWidth(font, *ucptr);
            
            // 换行检查
            if (ix + char_width > x + width) {
                ix = x + dx;
                iy += line_height;
                
                if (iy >= y + height) {
                    break;
                }
            }
            
            code = mfont_swap_bytes_shift(*ucptr);
            tempchar = mfont_getCharData(font, code, &out_size);
            drawW = font->font_width;
            if (font->font_width == 12 && code < 128) {
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                if (iy >= y && iy + font->font_height <= y + height) {
                    drawBitmapBit(tempchar, ix, iy, drawW, font->font_height, r, g, b);
                } else if (iy < y && iy + font->font_height > y) {
                    drawBitmapBitInRect(tempchar, ix, iy, drawW, font->font_height, x, y, width, height, r, g, b);
                } else if (iy < y + height && iy + font->font_height > y + height) {
                    drawBitmapBitInRect(tempchar, ix, iy, drawW, font->font_height, x, y, width, height, r, g, b);
                }
            }
            
            ix += char_width;
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
                
                char_width = mfont_getCharWidth(font, unicode);
                
                // 换行检查
                if (ix + char_width > x + width) {
                    ix = x + dx;
                    iy += line_height;
                    
                    if (iy >= y + height) {
                        break;
                    }
                }
                
                tempchar = mfont_getCharData(font, unicode, &out_size);
                drawW = font->font_width;
                if (font->font_width == 12 && unicode < 128) {
                    drawW = 8;
                }
                
                if (tempchar != NULL) {
                    if (iy >= y && iy + font->font_height <= y + height) {
                        drawBitmapBit(tempchar, ix, iy, drawW, font->font_height, r, g, b);
                    } else if (iy < y && iy + font->font_height > y) {
                        drawBitmapBitInRect(tempchar, ix, iy, drawW, font->font_height, x, y, width, height, r, g, b);
                    } else if (iy < y + height && iy + font->font_height > y + height) {
                        drawBitmapBitInRect(tempchar, ix, iy, drawW, font->font_height, x, y, width, height, r, g, b);
                    }
                }
                
                ix += char_width;
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
        while (*ucptr && csize > 0 && iy < y + height) {
            char_width = mfont_getCharWidth(font, *ucptr);
            
            // 换行检查
            if (ix + char_width > x + width) {
                ix = x + dx;
                iy += line_height;
                
                if (iy >= y + height) {
                    break;
                }
            }
            
            code = mfont_swap_bytes_shift(*ucptr);
            tempchar = mfont_getCharData(font, code, &out_size);
            drawW = font->font_width;
            if (font->font_width == 12 && code < 128) {
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                if (iy >= y && iy + font->font_height <= y + height) {
                    drawBitmapBit(tempchar, ix, iy, drawW, font->font_height, r, g, b);
                } else if (iy < y && iy + font->font_height > y) {
                    drawBitmapBitInRect(tempchar, ix, iy, drawW, font->font_height, x, y, width, height, r, g, b);
                } else if (iy < y + height && iy + font->font_height > y + height) {
                    drawBitmapBitInRect(tempchar, ix, iy, drawW, font->font_height, x, y, width, height, r, g, b);
                }
            }
            
            ix += char_width;
            ucptr++;
            csize -= 2;
        }
        
        mrc_free(buf);
        #endif
    }
    
    return MR_SUCCESS;
}

// 绘制渐变文字
int32 mfont_drawGradientText(UC3_FONT *font, const char *pcText, int16 x, int16 y, 
                           uint8 r1, uint8 g1, uint8 b1, uint8 r2, uint8 g2, uint8 b2, int is_unicode) {
    if (font == NULL || !font->is_loaded) {
        mrc_printf("mfont_drawGradientText: font not loaded");
        return MR_FAILED;
    }
    
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
    int drawW = 0;
    
    // 计算颜色渐变参数（使用整数运算）
    int16 r_diff = (int16)r2 - (int16)r1;
    int16 g_diff = (int16)g2 - (int16)g1;
    int16 b_diff = (int16)b2 - (int16)b1;
    
    if (is_unicode) {
        ucbuf = (uint16 *)pcText;
        ucptr = ucbuf;
        
        while (*ucptr) {
            code = mfont_swap_bytes_shift(*ucptr);
            tempchar = mfont_getCharData(font, code, &out_size);
            drawW = font->font_width;
            if (font->font_width == 12 && code < 128) {
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                n = 0;
                for (row = 0; row < font->font_height; row++) {
                    // 计算当前行的颜色
                    r_cur = r1 + (uint8)((r_diff * row) / font->font_height);
                    g_cur = g1 + (uint8)((g_diff * row) / font->font_height);
                    b_cur = b1 + (uint8)((b_diff * row) / font->font_height);
                    
                    ix_temp = ix;
                    
                    for (col = 0; col < drawW; col++) {
                        if (mfont_getPixel(tempchar, n++) >= 1) {
                            mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                        }
                        ix_temp++;
                    }
                }
            }
            
            ix += mfont_getCharWidth(font, code);
            ucptr++;
        }
    } else {
        #if USE_UTF8
        {
            utf8 = (uint8 *)pcText;
            
            while (*utf8) {
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
                
                tempchar = mfont_getCharData(font, unicode, &out_size);
                drawW = font->font_width;
                if (font->font_width == 12 && unicode < 128) {
                    drawW = 8;
                }
                
                if (tempchar != NULL) {
                    n = 0;
                    for (row = 0; row < font->font_height; row++) {
                        // 计算当前行的颜色
                        r_cur = r1 + (uint8)((r_diff * row) / font->font_height);
                        g_cur = g1 + (uint8)((g_diff * row) / font->font_height);
                        b_cur = b1 + (uint8)((b_diff * row) / font->font_height);
                        
                        ix_temp = ix;
                        
                        for (col = 0; col < drawW; col++) {
                            if (mfont_getPixel(tempchar, n++) >= 1) {
                                mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                            }
                            ix_temp++;
                        }
                    }
                }
                
                ix += mfont_getCharWidth(font, unicode);
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
            code = mfont_swap_bytes_shift(*ucptr);
            tempchar = mfont_getCharData(font, code, &out_size);
            drawW = font->font_width;
            if (font->font_width == 12 && code < 128) {
                drawW = 8;
            }
            
            if (tempchar != NULL) {
                n = 0;
                for (row = 0; row < font->font_height; row++) {
                    // 计算当前行的颜色
                    r_cur = r1 + (uint8)((r_diff * row) / font->font_height);
                    g_cur = g1 + (uint8)((g_diff * row) / font->font_height);
                    b_cur = b1 + (uint8)((b_diff * row) / font->font_height);
                    
                    ix_temp = ix;
                    
                    for (col = 0; col < drawW; col++) {
                        if (mfont_getPixel(tempchar, n++) >= 1) {
                            mrc_drawPointEx(ix_temp, iy + row, r_cur, g_cur, b_cur);
                        }
                        ix_temp++;
                    }
                }
            }
            
            ix += mfont_getCharWidth(font, code);
            ucptr++;
            csize -= 2;
        }
        
        mrc_free(buf);
        #endif
    }
    
    return MR_SUCCESS;
}

// 横屏绘制文字
int32 mfont_drawTextHor(UC3_FONT *font, const char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode) {
    if (font == NULL || !font->is_loaded) {
        mrc_printf("mfont_drawTextHor: font not loaded");
        return MR_FAILED;
    }
    
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
    
    if (is_unicode) {
        while (*ucptr) {
            code = *ucptr;
            tempchar = mfont_getCharData(font, mfont_swap_bytes_shift(code), &out_size);
            drawBitmapBitRotate(tempchar, ix, iy, font->font_width, font->font_height, r, g, b, 1);
            iy += mfont_getCharWidth(font, mfont_swap_bytes_shift(code));
            ucptr++;
        }
    } else {
        #if USE_UTF8
        {
            uint8 *utf8 = (uint8 *)pcText;
            while (*utf8) {
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
                tempchar = mfont_getCharData(font, unicode, &out_size);
                if (tempchar != NULL)
                    drawBitmapBitRotate(tempchar, ix, iy, font->font_width, font->font_height, r, g, b, 1);
                iy += mfont_getCharWidth(font, unicode);
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
            code = *ucptr;
            tempchar = mfont_getCharData(font, mfont_swap_bytes_shift(code), &out_size);
            if (tempchar != NULL) {
                drawBitmapBitRotate(tempchar, ix, iy, font->font_width, font->font_height, r, g, b, 1);
            }
            iy += mfont_getCharWidth(font, mfont_swap_bytes_shift(code));
            ucptr++;
            csize -= 2;
        }
        mrc_free(buf);
        #endif
    }
    return MR_SUCCESS;
}

// 获取文本宽度
int mfont_getWidth(UC3_FONT *font, const char *pcText, int is_unicode) {
    if (font == NULL || !font->is_loaded) {
        return 0;
    }
    
    uint16 *ucbuf = (uint16 *)pcText;
    uint16 *ucptr = ucbuf;
    int32 err = 0;
    int32 csize = 0;
    int16 iw = 0;
    uint16 *buf = NULL;
    uint16 code = 0;
    
    if (is_unicode) {
        while (*ucptr) {
            code = *ucptr;
            iw += mfont_getCharWidth(font, mfont_swap_bytes_shift(code));
            ucptr++;
        }
    } else {
        #if USE_UTF8
        {
            uint8 *utf8 = (uint8 *)pcText;
            while (*utf8) {
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
                iw += mfont_getCharWidth(font, unicode);
            }
        }
        #else
        buf = mrc_c2u(pcText, &err, &csize);
        if (buf == NULL) {
            mrc_printf("mrc_c2u error");
            return 0;
        }
        ucptr = buf;
        while (*ucptr && csize > 0) {
            code = *ucptr;
            iw += mfont_getCharWidth(font, mfont_swap_bytes_shift(code));
            ucptr++;
            csize -= 2;
        }
        mrc_free(buf);
        #endif
    }
    return iw;
}

// 获取文本高度
int mfont_getHeight(UC3_FONT *font, const char *pcText, int is_unicode) {
    if (font == NULL || !font->is_loaded) {
        return 0;
    }
    return font->font_height;
}

// 为兼容性提供的别名函数（可选）
UC3_FONT* font_load(const char *font_name) {
    return mfont_load(font_name);
}

int32 font_drawText(UC3_FONT* font, const char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode) {
    return mfont_drawText(font, pcText, x, y, r, g, b, is_unicode);
}