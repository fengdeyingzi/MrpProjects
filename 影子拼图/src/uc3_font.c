#include <mrc_base.h>

uint8 *uc3_buf = NULL;
int32 uc3_buf_len = 0;

extern void drawBitmap(
    uint8 *font, int x, int y, uint8 r, uint8 g, uint8 b);
extern int getUC3Width(int id);
extern uint8 *getCharUC3(uint16 id, uint32 fontw, uint32 fonth, uint32 *out_size);
extern int getFontPixel(uint8 *buf, int n);

int tempfontindex;

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
    if (is_unicode)
    {
        while (*ucptr)
        {
            code = *ucptr;
            tempchar = getCharUC3((uint16)((code << 8) & 0xff00 | code >> 8), 16, 16, &out_size);
            drawBitmap(tempchar, ix, iy, r, g, b);
            ix += getUC3Width((uint16)((code << 8) & 0xff00 | code >> 8));
            ucptr++;
        }
    }
    else
    {
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
            // mrc_printf("draw %x\n",code);
            if (tempchar != NULL)
            {
                drawBitmap(tempchar, ix, iy, r, g, b);
            }

            ix += getUC3Width((uint16)((code << 8) & 0xff00 | code >> 8));
            ucptr++;
            csize -= 2;
        }
        // mrc_printf("free buf");
        mrc_free(buf);
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
    }
    return iw;
}

void drawBitmap(
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

int getFontPixel(uint8 *buf, int n)
{
    // Calculate which byte and bit position
    int byteIndex = n / 8;
    return (128 >> (n % 8)) & buf[byteIndex];
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
    fontCount = *(uint32*)(uc3_buf + 12);
    // mrc_memcpy(&fontCount, uc3_buf + 12, sizeof(uint32));

    endPos = startPos + fontCount * 4;
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
        mrc_free(uc3_buf);
        uc3_buf = NULL;
        uc3_buf_len = 0;
    }
}