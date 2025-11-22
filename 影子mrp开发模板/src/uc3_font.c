#include <mrc_base.h>

uint8 *uc3_buf = NULL;
int32 uc3_buf_len = 0;

// 是否使用UTF-8编码
#define USE_UTF8 1

// 是否横屏绘制
#define USE_HOR_DRAW 0

// 开启位置交�?
#define SWAP_FONT 0

extern void drawBitmapBit(
uint8 *font, int x, int y, uint8 r, uint8 g, uint8 b);
extern  void drawBitmapBitRotate(uint8 *font, int x, int y, uint8 r, uint8 g, uint8 b, int type);
extern int getUC3Width(int id);
extern uint8 *getCharUC3(uint16 id, uint32 fontw, uint32 fonth, uint32 *out_size);
extern int getFontPixel(uint8 *buf, int n);

int tempfontindex;
/*
uc3字体文件结构说明
0-3字节前缀字符�? "FT16"
4-7字节字体图片宽度 fontw
8-11字节字体图片高度 fonth
12-15字节文字数量
16字节开�? 记录文字id以及对应的offset,id�?2字节,offset�?4字节
读取单个字的字节大小dataSize计算�?: isAddBit = (fontw * fonth % 8) > 0; dataSize = ((fontw * fonth / 8) + (isAddBit ? 1 : 0));

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
    return MR_SUCCESS;
}

int32 uc3_drawText(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode)
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
    #if USE_HOR_DRAW
    return uc3_drawTextHor(pcText, x, y, r, g, b, is_unicode);
    #else
    if (is_unicode)
    {
        while (*ucptr)
        {
            code = *ucptr;
            tempchar = getCharUC3((uint16)((code << 8) & 0xff00 | code >> 8), 16, 16, &out_size);
            if(tempchar!=NULL)
                drawBitmapBit(tempchar, ix, iy, r, g, b);
            ix += getUC3Width((uint16)((code << 8) & 0xff00 | code >> 8));
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
                code = (uint16)((unicode << 8) & 0xff00 | unicode >> 8);
                tempchar = getCharUC3(code, 16, 16, &out_size);
                if (tempchar != NULL)
                    drawBitmapBit(tempchar, ix, iy, r, g, b);
                ix += getUC3Width(code);
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
        // mrc_printf("draw text %d\n",csize);
        while (*ucptr && csize > 0)
        {
            uint16 code = *ucptr;

            uint8 *tempchar = getCharUC3((uint16)((code << 8) & 0xff00 | code >> 8), 16, 16, &out_size);
            // mrc_printf("draw %x ptr=%p\n", (uint16)((code << 8) & 0xff00 | code >> 8), tempchar);
            if (tempchar != NULL)
            {
                drawBitmapBit(tempchar, ix, iy, r, g, b);
            }

            ix += getUC3Width((uint16)((code << 8) & 0xff00 | code >> 8));
            ucptr++;
            csize -= 2;
        }
        // mrc_printf("free buf");
        mrc_free(buf);
        #endif
    }
    
    return 0;
    #endif
}


//横屏绘制文字
int32 uc3_drawTextHor(char *pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode)
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
            tempchar = getCharUC3((uint16)((code << 8) & 0xff00 | code >> 8), 16, 16, &out_size);
            drawBitmapBitRotate(tempchar, ix, iy, r, g, b, 1);
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
                code = (uint16)((unicode << 8) & 0xff00 | unicode >> 8);
                tempchar = getCharUC3(unicode, 16, 16, &out_size);
                if (tempchar != NULL)
                    drawBitmapBitRotate(tempchar, ix, iy, r, g, b, 1);
                iy += getUC3Width(unicode);
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
        // mrc_printf("draw text %d\n",csize);
        while (*ucptr && csize > 0)
        {
            code = *ucptr;

            tempchar = getCharUC3((uint16)((code << 8) & 0xff00 | code >> 8), 16, 16, &out_size);
            // mrc_printf("draw %x\n",code);
            if (tempchar != NULL)
            {
                drawBitmapBitRotate(tempchar, ix, iy, r, g, b, 1);
            }

            iy += getUC3Width((uint16)((code << 8) & 0xff00 | code >> 8));
            ucptr++;
            csize -= 2;
        }
        // mrc_printf("free buf");
        mrc_free(buf);
        #endif
    }
    return 0;
}

int getUC3Width(int id)
{
    return id < 128 ? 8 : 16;
}

int uc3_getWidth(char *pcText, int is_unicode)
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
                iw += getUC3Width((uint16)((unicode << 8) & 0xff00 | unicode >> 8));
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
        // mrc_printf("draw text %d\n",csize);
        while (*ucptr && csize > 0)
        {
            uint16 code = *ucptr;

            iw += getUC3Width((uint16)(code << 8 & 0xff00 | code >> 8));
            ucptr++;
            csize -= 2;
        }
        // mrc_printf("free buf");
        mrc_free(buf);
        #endif
    }
    return iw;
}

// 旋转绘制二进制位�?
// type取�? 1:90 2:180 3:270
void drawBitmapBitRotate(uint8 *font, int x, int y, uint8 r, uint8 g, uint8 b, int type) {
    
    int iy = 0;
    int bit = 0;

    switch(type) {
        case 1: // 90度旋�?
            for (iy = 0; iy < 16; iy++) {
                for (bit = 0; bit < 16; bit++) {
                    if (getFontPixel(font, iy * 16 + bit) >= 1) {
                        mrc_drawPointEx(x + (15-iy), y + (bit), r, g, b);
                    }
                }
            }
            break;
        case 2: // 180度旋�?
            for (iy = 0; iy < 16; iy++) {
                for (bit = 0; bit < 16; bit++) {
                    if (getFontPixel(font, (15 - iy) * 16 + (15 - bit)) >= 1) {
                        mrc_drawPointEx(x + bit, y + iy, r, g, b);
                    }
                }
            }
            break;
        case 3: // 270度旋�?
            for (iy = 0; iy < 16; iy++) {
                for (bit = 0; bit < 16; bit++) {
                    if (getFontPixel(font, iy * 16 + bit) >= 1) {
                        mrc_drawPointEx(x + iy, y + (15-bit), r, g, b);
                    }
                }
            }
            break;
        default:
            // 处理无效的type�?
            break;
    }
}

// 绘制二进制位图数�?
// x,y 绘制在屏幕上的坐标点
// r,g,b 绘制的颜�?
void drawBitmapBit(
    uint8 *font, int x, int y, uint8 r, uint8 g, uint8 b)
{
    int n = 0;
    int iy = 0;
    int bit = 0;
    for (iy = y; iy < y + 16; iy++)
    {
        int ix = x;

        for (bit = 0; bit < 16; bit++)
        {
            if (getFontPixel(font, n++) >= 1)
            {
                mrc_drawPointEx(ix, iy, r, g, b);
            }
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

    // Read the font count from the buffer (little-endian)
    uint32 fontCount = 0;
    int startPos = 16;
    int endPos = 0;
    int isAddBit = 0;
    uint32 dataSize = 0;
    uint16 tempid;
    uint32 tempoffset;
    uint8 tempsw[6];
    fontCount = *(uint32 *)(uc3_buf + 12);
    // mrc_memcpy(&fontCount, uc3_buf + 12, sizeof(uint32));

    endPos = startPos + fontCount * 6;
    isAddBit = (fontw * fonth % 8) > 0;

    // fontCount = (fontCount & 0xFFFFFFFF); // Ensure it's properly interpreted

    // mrc_printf("文字数量: %u\n", fontCount);

    // Calculate start and end positions

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