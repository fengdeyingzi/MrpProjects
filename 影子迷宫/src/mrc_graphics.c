
#include <mrc_base.h>
#include "mrc_android.h"
#include "mrc_graphics.h"
#include "xl_debug.h"
#include "mpc.h"
#include <math.h>
#include <armdsp.h>

/*
ʵ��һЩ��ͼ����
��Ҫ��ʵ��bmpͼƬ����

���Ӱ��

*/
// ��rgb888תrgb565
#define MAKECOLOR565(color) ((uint16)((color >> 8) & 0xf800) | (uint16)((color >> 5) & 0x07e0) | (uint16)((color >> 3) & 0x1f))
#ifndef MAKERGB
#define MAKERGB(r, g, b) (((uint16)(r >> 3) << 11) | ((uint16)(g >> 2) << 5) | ((uint16)(b >> 3)))
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define BLEND_COLOR(dstColor, srcColor, alpha)                                                               \
  (((((((dstColor) >> 11) & 0x1F) * (255 - (alpha)) + (((srcColor) >> 11) & 0x1F) * (alpha)) / 255) << 11) | \
   (((((dstColor) >> 5) & 0x3F) * (255 - (alpha)) + (((srcColor) >> 5) & 0x3F) * (alpha)) / 255) << 5) |     \
      ((((dstColor) & 0x1F) * (255 - (alpha)) + ((srcColor) & 0x1F) * (alpha)) / 255)

extern int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3);
extern uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha);
extern uint16 blendColor888(uint16 dstColor, uint32 srcColor);
BITMAP_565 *createBitmap565(int width, int height)
{
  BITMAP_565 *bmp = (BITMAP_565 *)mrc_malloc(sizeof(BITMAP_565));
  mrc_memset(bmp, 0, sizeof(BITMAP_565));
  bmp->bitmap = (uint16 *)mr_malloc(width * height * 2);
  mrc_memset(bmp->bitmap, 0, width * height * 2);
  bmp->color_bit = 16;
  bmp->transcolor = 0;
  bmp->width = width;
  bmp->height = height;
  bmp->mode = BM_COPY;
  bmp->buflen = width * height * 2;
  return bmp;
}

// ͨ����Ļ���洴��BITMAP
BITMAP_565 *createBitmapFromScreen()
{
  BITMAP_565 *bmp;

  bmp = createBitmap565(SCRW, SCRH);

  mrc_memcpy(bmp->bitmap, w_getScreenBuffer(), SCRW * SCRH * 2);

  return bmp;
}

BITMAP_565 *readBitmap565FromAssets(const char *filename)
{
  int32 len = 0;
  void *buf;
  BITMAP_565 *re = NULL;
  char *endName = NULL;
  char *head = NULL;
  //  debug_printf("��ʼ��ȡ�ļ�");

  buf = mrc_readFileFromAssets(filename, &len);
  if (buf == NULL)
    return NULL;
  head = (char *)buf;
  if (len > 0)
  {
    //  debug_printf("����ͼƬ");
    endName = mrc_strrchr(filename, '.');

    if (head[0] == 'M' && head[1] == 'A' && head[2] == 'P' && (head[3] == '5' || head[3] == '8'))
    {
      re = bma_read(buf, len);
    }
    else
    {
      re = bmp_read(buf, len);
      mrc_free(buf);
    }
  }
  return re;
}

BITMAP_565 *readBitmap(char *filename)
{
  int32 f;
  void *buf;
  int32 len = 0;
  BITMAP_565 *re = NULL;
  char *endName = NULL;
  //  debug_printf("��ȡ�ļ�����");
  len = mrc_getLen(filename);

  if (len > 0)
  {
    //  debug_printf("open");
    f = mrc_open(filename, 1);
    //  debug_log("malloc %d", len);
    buf = mr_malloc(len);
    if (buf == NULL)
    {
      mrc_printf("�����ڴ�ʧ��");
    }
    if (f > 0)
    {
      //  debug_printf("read");
      mrc_read(f, buf, len);
      mrc_close(f);
      //  debug_printf("bmp_read");
      endName = mrc_strrchr(filename, '.');

      re = bmp_read(buf, len);
    }
    //  debug_printf("�ͷ��ڴ�");
    mrc_freeFileData(buf, len);
  }
  return re;
}

int32 drawBitmapFlip(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
{
  if(bmp->color_bit == 16){
    return mrc_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)w,(uint16)h, (uint16)bmp->mode, (int16)sx,(int16)sy,(uint16)bmp->transcolor);
  }else if(bmp->color_bit == 32){
    return gl_bitmapShowExTrans8888((uint32*)bmp->bitmap, (int16)x, (int16)y, (int16)bmp->width, (int16)w, (int16)h, (int16)sx, (int16)sy);
  }
  
  return MR_FAILED;
}

void drawBitmap(BITMAP_565 *bmp, int32 x, int32 y)
{
  // �ж�λ��
  if (bmp->color_bit == 16)
  {
    drawBitmap565(bmp, x, y);
  }
  else if (bmp->color_bit == 32)
  {
    drawBitmap8888(bmp, x, y);
  }
}

/*
    ��p�����е�ͼƬ(rgb565��ʽ)���ӻ����е�ͼƬ��(sx, sy)
    ��ʼ�Ŀ��Ϊw, h�����򣬻��Ƶ�(x, y)��ʼ����Ļ�����С�

����:
i                  ͼƬ�������
x,y              ��Ļλ��
rop              ѡ�����£�
   BM_COPY,       //DST = SRC*      ����
   BM_TRANSPARENT,//͸��ɫ����ʾ

sx,sy              ԴͼƬ����ʼλ��
w,h             ������ͼƬ�Ŀ��
transcolor ͸��ɫ

����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ��
*/
int gl_bitmapShowExTrans(uint16 *p,
                         int16 x,
                         int16 y,
                         int16 mw,
                         int16 w,
                         int16 h,
                         uint16 rop,
                         int16 sx,
                         int16 sy,
                         uint16 transcolor)
{
  // ����ֲ�����
  int16 i, j;                                 // ����ѭ������
  uint16 *screenBuffer = w_getScreenBuffer(); // ��ȡ��Ļ������
  uint16 pixelColor;                          // ��ǰ�����������ɫ

  // �����������ʵ�ʱ߽�
  int16 startX = (x < 0) ? 0 : x;             // ��ȡ��ʼ���Ƶ�X�߽�
  int16 startY = (y < 0) ? 0 : y;             // ��ȡ��ʼ���Ƶ�Y�߽�
  int16 endX = (x + w > SCRW) ? SCRW : x + w; // ��ȡ�������Ƶ�X�߽�
  int16 endY = (y + h > SCRH) ? SCRH : y + h; // ��ȡ�������Ƶ�Y�߽�

  // ����ԴͼƬ�ڻ��������еĶ�Ӧλ��
  int16 sourceX = (startX - x) + sx; // ӳ��Դͼ��X����
  int16 sourceY = (startY - y) + sy; // ӳ��Դͼ��Y����
  int32 sindex, pindex;
  // ��������������Ч��
  if (p == NULL || screenBuffer == NULL || w <= 0 || h <= 0 ||
      sx < 0 || sy < 0 || sx + w > mw)
  {
    return MR_FAILED; // ������Ч������ʧ��
  }
  switch (rop)
  {
  case BM_COPY:
    sourceX = sx; // ����Դͼ���X����
    for (i = startY; i < endY; i++)
    {
      // ����Դͼ�����Чλ��
      pindex = (sourceY * mw + sourceX);
      // ����Ŀ����Ļ�е�λ��
      sindex = (i * SCRW + startX);
      memcpy(screenBuffer + sindex, p + pindex, (endX - startX) * 2);
      sourceY++; // �ƶ���Դͼ�����һ����
    }
    break;
  case BM_TRANSPARENT:
    // ����Ҫ���Ƶ�����
    for (i = startY; i < endY; i++)
    {
      pindex = (sourceY * mw + sourceX);
      sindex = (i * SCRW);
      for (j = startX; j < endX; j++)
      {
        // ����Դͼ�����Чλ��

        pixelColor = p[pindex]; // ��ȡ��ǰ������ɫ

        // �ж��Ƿ�Ϊ͸��ɫ
        if (pixelColor != transcolor)
        {
          screenBuffer[sindex] = pixelColor;
        }
        sindex++;
        pindex++;
      }
      sourceX = sx; // ����Դͼ���X����
      sourceY++;    // �ƶ���Դͼ�����һ����
    }
  }

  return MR_SUCCESS; // �ɹ���ɻ���
}

int gl_bitmapShowExTrans8888(uint32 *p,
                             int16 x,
                             int16 y,
                             int16 mw,
                             int16 w,
                             int16 h,
                             int16 sx,
                             int16 sy)
{
  // ����ֲ�����
  int16 i, j;                                 // ����ѭ������
  uint16 *screenBuffer = w_getScreenBuffer(); // ��ȡ��Ļ������
  uint16 pixelColor;                          // ��ǰ�����������ɫ
  uint32 pixel;                               // ��ǰ�����32λ����
  uint32 alpha;

  // �����������ʵ�ʱ߽�
  int16 startX = (x < 0) ? 0 : x;             // ��ȡ��ʼ���Ƶ�X�߽�
  int16 startY = (y < 0) ? 0 : y;             // ��ȡ��ʼ���Ƶ�Y�߽�
  int16 endX = (x + w > SCRW) ? SCRW : x + w; // ��ȡ�������Ƶ�X�߽�
  int16 endY = (y + h > SCRH) ? SCRH : y + h; // ��ȡ�������Ƶ�Y�߽�

  // ����ԴͼƬ�ڻ��������еĶ�Ӧλ��
  int16 sourceX = (startX - x) + sx; // ӳ��Դͼ��X����
  int16 sourceY = (startY - y) + sy; // ӳ��Դͼ��Y����

  // ��������������Ч��
  if (p == NULL || screenBuffer == NULL || w <= 0 || h <= 0 ||
      sx < 0 || sy < 0 || sx + w > mw)
  {
    return MR_FAILED; // ������Ч������ʧ��
  }
  // ����Ҫ���Ƶ�����
  for (i = startY; i < endY; i++)
  {
    for (j = startX; j < endX; j++)
    {
      pixel = p[(sourceY * mw + sourceX)]; // ��ͼ�񻺳�����ȡ��ǰ32λ����
      alpha = pixel >> 24;
      if (alpha != 0) // �ж��Ƿ�Ϊ͸��ɫ
      {
        pixelColor = blendColor888(screenBuffer[i * SCRW + j], pixel); // B

        screenBuffer[i * SCRW + j] = pixelColor; // д����Ļ������
      }
      sourceX++; // �ƶ���Դͼ�����һ������
    }
    sourceX = sx; // ����Դͼ���X����
    sourceY++;    // �ƶ���Դͼ�����һ����
  }

  return MR_SUCCESS; // �ɹ���ɻ���
}

void drawBitmap565(BITMAP_565 *bmp, int32 x, int32 y)
{
  // int i;
  // int32 screenIndex;
  // uint16 *buffer;
  // int by;
  // if (bmp->memlen > 0 && x > 0 && y > 0 && bmp->width + x < SCRW && bmp->height + y < SCRH)
  // {
  //   buffer = w_getScreenBuffer();
  //   screenIndex = y * SCRW + x;
  //   for (i = 0; i < bmp->memlen; i += 12)
  //   {
  //     // mrc_printf("inde %d %d", i/3, bmp->memlen);
  //     // mrc_printf("memcpy %d %d %d ", screenIndex + bmp->memcache[i/4], bmp->memcache[i/4+1], bmp->memcache[i/4+2]);
  //     // ��Ļ��λ�� ͼƬ��λ�� ���Ƴ���
  //     mrc_memcpy(buffer + screenIndex + bmp->memcache[i >> 2], bmp->bitmap + bmp->memcache[(i >> 2) + 1], bmp->memcache[(i >> 2) + 2]);
  //   }
  // }
  // else
  // {
  gl_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)bmp->width, (uint16)bmp->height, (uint16)bmp->mode, 0, 0, (uint16)bmp->transcolor);
  // }

  // bmp_draw(bmp, x, y);
}

// �Զ�����memcpy�Ż�
int32 bitmapAutoMemcpy(BITMAP_565 *b)
{
  int32 mem_count = 0;
  int32 ix, iy;
  int32 m_start = -1, m_end = 0;
  int32 bx, by;
  int32 screenIndex;

  if (b->mode == BM_TRANSPARENT)
  {
    for (iy = 0; iy < b->height; iy++)
    {
      m_start = -1;
      for (ix = 0; ix < b->width; ix++)
      {
        if (b->bitmap[iy * b->width + ix] != b->transcolor)
        {
          if (ix == b->width - 1 && m_start >= 0)
          {
            goto COUNT_ADD;
          }
          if (m_start < 0)
          {
            m_start = iy * b->width + ix;
          }
        }
        else
        {
          if (m_start >= 0)
          {
          COUNT_ADD:
            m_end = iy * b->width + ix;
            mem_count++;
          }
          m_start = -1;
        }
      }
    }
    // ���mem_count����С��ͼƬ���/10,������Ż�
    if (mem_count < b->width * b->height / 10)
    {
      b->memcache = mrc_malloc(mem_count * sizeof(int32) * 3);
      b->memlen = mem_count * sizeof(int32) * 3;
      mrc_printf("mem_count: %d\n", mem_count);
      mem_count = 0;
      for (iy = 0; iy < b->height; iy++)
      {
        m_start = -1;
        for (ix = 0; ix < b->width; ix++)
        {
          if (b->bitmap[iy * b->width + ix] != b->transcolor)
          {
            if (ix == b->width - 1 && m_start >= 0)
            {
              goto MEM_SET;
            }
            if (m_start < 0)
            {
              m_start = iy * b->width + ix;
            }
          }
          else
          {
            if (m_start >= 0)
            {
            MEM_SET:
              m_end = iy * b->width + ix;
              by = m_start / b->width;
              bx = m_start % b->width;
              screenIndex = SCRW * by + bx;
              b->memcache[mem_count * 3] = screenIndex;
              b->memcache[mem_count * 3 + 1] = m_start;
              b->memcache[mem_count * 3 + 2] = (m_end - m_start) * sizeof(uint16);
              mrc_printf("%d %d %d", screenIndex, m_start, m_end - m_start);
              mem_count++;
            }
            m_start = -1;
          }
        }
      }
      mrc_printf("mem_count2: %d\n", mem_count);
      return MR_SUCCESS;
    }
  }
  return MR_FAILED;
}

void drawBitmap8888(BITMAP_565 *bmp, int32 x, int32 y)
{
  // int32 px, py;
  int32 sx, sy;
  int32 w = bmp->width;
  int32 h = bmp->height;
  int32 pindex, sindex;
  int32 minX, maxX, minY, maxY;

  uint32 *buf = (uint32 *)bmp->bitmap;
  uint16 *buffer = w_getScreenBuffer();
  int32 ii = 0;
  minX = MAX(0, x);
  minY = MAX(0, y);
  maxY = MIN(y + h, SCRH);
  maxX = MIN(x + w, SCRW);
  if (maxX < 0 || maxY < 0)
    return;
  for (sy = minY; sy < maxY; sy++)
  {
    sindex = SCRW * sy + minX;
    pindex = y < 0 ? bmp->width * (sy - y) : bmp->width * (sy - y);
    if (x < 0)
    {
      pindex -= x;
    }
    for (sx = minX; sx < maxX; sx++)
    {
      if (buf[pindex] >> 24 != 0)
      {
        buffer[sindex] = blendColor888(buffer[sindex], (buf[pindex]));
        ii++;
      }
      sindex++;
      pindex++;
    }
  }
}

uint16 getPixelColor(int32 x, int32 y)
{
  return *(getscrbuf() + (SCRW * y + x));
}

// ͸����,ȡֵ0��255
void drawBitmapAlpha(BITMAP_565 *b, int32 x, int32 y, int32 alpha)
{
  int32 j, i;
  uint16 color;
  int32 targetX, targetY;
  int32 minX, minY, maxX, maxY;
  uint16 *buffer = w_getScreenBuffer();
  uint32 *buf32;
  uint32 color32;
  int32 alpha32;
  int32 startX, startY;

  // ���� alpha �ķ�Χ�� 0 �� 255 ֮��
  if (alpha < 0)
    alpha = 0;
  if (alpha > 255)
    alpha = 255;

  if (b == NULL)
    return;

  // ������Ч��������
  minX = MAX(0, x);
  minY = MAX(0, y);
  maxX = MIN(SCRW, x + b->width);
  maxY = MIN(SCRH, y + b->height);

  // ������������ʼ�ͽ�������
  startX = MIN(x, 0) + b->width + x;
  startY = MIN(y, 0) + b->height + y;

  if (maxX <= minX || maxY <= minY)
    return; // û����Ч�Ļ�������

  if (b->color_bit == 16)
  {
    // ����λͼ��ÿ������
    if (b->mode == BM_TRANSPARENT)
    {
      for (j = minY; j < maxY; j++)
      {
        for (i = minX; i < maxX; i++)
        {
          // ���㵱ǰ���ص�λͼ������
          color = b->bitmap[(j - y) * b->width + (i - x)];

          // �����͸��ɫ��������������
          if (color == b->transcolor)
          {
            continue;
          }

          // ����Ŀ����λ��
          targetX = i;
          targetY = j;
          *(buffer + SCRW * targetY + targetX) = blendColor(*(buffer + SCRW * targetY + targetX), color, alpha);
        }
      }
    }
    else
    {
      for (j = minY; j < maxY; j++)
      {
        for (i = minX; i < maxX; i++)
        {
          // ���㵱ǰ���ص�λͼ������
          color = b->bitmap[(j - y) * b->width + (i - x)];

          // ����Ŀ����λ��
          targetX = i;
          targetY = j;
          *(buffer + SCRW * targetY + targetX) = blendColor(*(buffer + SCRW * targetY + targetX), color, alpha);
        }
      }
    }
  }
  else if (b->color_bit == 32)
  {
    buf32 = (uint32 *)(b->bitmap);
    for (j = minY; j < maxY; j++)
    {
      for (i = minX; i < maxX; i++)
      {
        // ���㵱ǰ���ص�λͼ������
        color32 = buf32[(j - y) * b->width + (i - x)];
        if ((color32 >> 24) != 0)
        {
          alpha32 = ((color32 >> 24) * alpha) >> 8;
          // ����Ŀ����λ��
          targetX = i;
          targetY = j;
          buffer[SCRW * targetY + targetX] = blendColor888(buffer[SCRW * targetY + targetX], (color32 & 0xffffff) | (alpha32 << 24));
        }
      }
    }
  }
}

// �������������ڷ�תͼ������
void flipBitmap(uint16 *bitmap, int width, int height, int flip)
{
  uint16 temp;
  int x, y;
  if (flip == 1)
  { // ����ת
    for (y = 0; y < height; y++)
    {
      for (x = 0; x < width / 2; x++)
      {
        temp = bitmap[y * width + x];
        bitmap[y * width + x] = bitmap[y * width + (width - 1 - x)];
        bitmap[y * width + (width - 1 - x)] = temp;
      }
    }
  }
  else if (flip == 2)
  { // ����ת
    for (y = 0; y < height / 2; y++)
    {
      for (x = 0; x < width; x++)
      {
        temp = bitmap[y * width + x];
        bitmap[y * width + x] = bitmap[(height - 1 - y) * width + x];
        bitmap[(height - 1 - y) * width + x] = temp;
      }
    }
  }
}

// ��bitmap��ת90��/180��/270��,��ת֮���޸�bitmap��bitmap��������
// typeȡֵ 1:90 2:180 3:270
// flipȡֵ 1����ת 2����ת
void bitmap565Rotate(BITMAP_565 *b, int type, int flip)
{
  int newWidth, newHeight;
  uint16 *newBitmap;
  int x, y;
  if (type < 1 || type > 3)
  {
    return; // ��Ч����ת����
  }

  switch (type)
  {
  case 1: // ��ת90��
    newWidth = b->height;
    newHeight = b->width;
    newBitmap = (uint16 *)mr_malloc(newWidth * newHeight * sizeof(uint16));
    for (y = 0; y < b->height; y++)
    {
      for (x = 0; x < b->width; x++)
      {
        newBitmap[x * newWidth + (newWidth - 1 - y)] = b->bitmap[y * b->width + x];
      }
    }
    break;

  case 2: // ��ת180��
    newWidth = b->width;
    newHeight = b->height;
    newBitmap = (uint16 *)mr_malloc(newWidth * newHeight * sizeof(uint16));
    for (y = 0; y < b->height; y++)
    {
      for (x = 0; x < b->width; x++)
      {
        newBitmap[(newHeight - 1 - y) * newWidth + (newWidth - 1 - x)] = b->bitmap[y * b->width + x];
      }
    }
    break;

  case 3: // ��ת270��
    newWidth = b->height;
    newHeight = b->width;
    newBitmap = (uint16 *)mr_malloc(newWidth * newHeight * sizeof(uint16));
    for (y = 0; y < b->height; y++)
    {
      for (x = 0; x < b->width; x++)
      {
        newBitmap[(newHeight - 1 - x) * newWidth + y] = b->bitmap[y * b->width + x];
      }
    }
    break;

  default:
    return; // ��Ӧ�������
  }

  // ����ת
  if (flip == 1 || flip == 2)
  {
    flipBitmap(newBitmap, newWidth, newHeight, flip);
  }

  // ����ԭʼλͼ����
  mrc_freeFileData(b->bitmap, b->buflen); // �ͷ�ԭ bitmap ���ڴ�

  b->bitmap = newBitmap;
  b->width = newWidth;
  b->height = newHeight;
  b->buflen = newWidth * newHeight * sizeof(uint16);
}
/*
ͼƬ��ת���Ż���
rop [IN] BM_COPY, //DST = SRC* ����

BM_TRANSPARENT, //͸��ɫ�� ��ʾ��ͼƬ(0��0)������͸��ɫ

����ǿ�ķ�ʽ��bmpͼƬ������ָ��ͼƬ�С�
��srcbmp ��bmp�����е�ͼƬ���ӻ����е�ͼƬ��(sx, sy)��
ʼ�Ŀ��Ϊw, h�����򣬻��Ƶ�dstbmp��(dx,dy)��ʼ��bmp
�����С�

ģʽropѡ�����£�
   BM_COPY,               //DST = SRC*      ����
   BM_TRANSPARENT,  //͸��ɫ����ʾ

A��B��C��D����ͼ��仯�����ڸñ仯�ı任����Ϊ��
x = A0*x0 + B0*y0
y = C0*x0 + D0*y0
����Ϊ�˱�ʾС����A, B, C, D����������256������
A = A0*256
B = B0*256
C = C0*256
D = D0*256
���ݱ任��ʽ�����Ի����ͬЧ����ͼ�񣬱��磺
��תͼ��
A = 256 * cos���Ƕȣ�
B = 256 * sin���Ƕȣ�
C = 256 * -sin���Ƕȣ�
D = 256 * cos���Ƕȣ�
�Ŵ���Сͼ��
A = 256 * �Ŵ���
B = 0
C = 0
D = 256 * �Ŵ���

����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ��
*/
void _DrawBitmapEx(BITMAP_565 *srcbmp, uint16 sx, uint16 sy, BITMAP_565 *dstbmp, uint16 dx, uint16 dy,
                   uint16 w, uint16 h, mr_transMatrixSt *trans)
{
  int dy1, dy2;
  int dx1, dx2;
  int dy_max1, dy_max2;
  int dy_min1, dy_min2;

  uint16 *src;
  uint16 *dst;
  int src_tx, src_ty;
  int16 dst_cx, dst_cy;
  int16 dst_min_x, dst_min_y;
  int16 dst_max_x, dst_max_y;
  int16 d_x, d_y;
  int tA, tB, tC, tD;
  int transMod;
  int rop;
  uint16 transcoler = (uint16)(srcbmp->transcolor & 0xffff);

#ifdef LOG_DRAW
  LOGI("_DrawBitmapEx(...)");
#endif

  tA = trans->A;
  tB = trans->B;
  tC = trans->C;
  tD = trans->D;
  dst_cx = (w >> 1) + dx;
  dst_cy = (h >> 1) + dy;
  transMod = tD * tA - tC * tB;

  dst_max_y = (h * abs(tD) + w * abs(tC)) >> 9;
  if (dst_max_y >= dstbmp->height - dst_cy)
    dst_max_y = dstbmp->height - dst_cy;

  dst_min_y = -dst_max_y;
  if (dst_min_y <= -dst_cy)
    dst_min_y = -dst_cy;

  for (d_y = dst_min_y; d_y < dst_max_y; d_y++)
  {
    if (tD)
    {
      dy1 = (d_y * tB + (transMod * w >> 9)) / tD;
      dy2 = (d_y * tB - (transMod * w >> 9)) / tD;
      dy_max1 = MAX(dy1, dy2);
    }
    else
    {
      dy_max1 = 999;
    }

    if (tC)
    {
      dy1 = (d_y * tA + (transMod * h >> 9)) / tC;
      dy2 = (d_y * tA - (transMod * h >> 9)) / tC;
      dy_max2 = MAX(dy1, dy2);
    }
    else
    {
      dy_max2 = 999;
    }

    if (dy_max1 >= dy_max2)
    {
      if (tC)
      {
        dx1 = (d_y * tA + (transMod * h >> 9)) / tC;
        dx2 = (d_y * tA - (transMod * h >> 9)) / tC;
        dst_max_x = MAX(dx1, dx2);
      }
      else
      {
        dst_max_x = 999;
      }
    }
    else
    {
      if (tD)
      {
        dx1 = (d_y * tB + (transMod * w >> 9)) / tD;
        dx2 = (d_y * tB - (transMod * w >> 9)) / tD;
        dst_max_x = MAX(dx1, dx2);
      }
      else
      {
        dst_max_x = 999;
      }
    }

    if (tD)
    {
      dy1 = (d_y * tB - (transMod * w >> 9)) / tD;
      dy2 = (d_y * tB + (transMod * w >> 9)) / tD;
      dy_min1 = MIN(dy1, dy2);
    }
    else
    {
      dy_min1 = -999;
    }

    if (tC)
    {
      dy1 = (d_y * tA - (transMod * h >> 9)) / tC;
      dy2 = (d_y * tA + (transMod * h >> 9)) / tC;
      dy_min2 = MIN(dy1, dy2);
    }
    else
    {
      dy_min2 = -999;
    }

    if (dy_min1 <= dy_min2)
    {
      if (tC)
      {
        dx1 = (d_y * tA - (transMod * h >> 9)) / tC;
        dx2 = (d_y * tA + (transMod * h >> 9)) / tC;
        dst_min_x = MIN(dx1, dx2);
      }
      else
      {
        dst_min_x = -999;
      }
    }
    else
    {
      if (tD)
      {
        dx1 = (d_y * tB - (transMod * w >> 9)) / tD;
        dx2 = (d_y * tB + (transMod * w >> 9)) / tD;
        dst_min_x = MIN(dx1, dx2);
      }
      else
      {
        dst_min_x = -999;
      }
    }

    if (dst_max_x >= dstbmp->width - dst_cx)
      dst_max_x = dstbmp->width - dst_cx;
    if (dst_min_x <= -dst_cx)
      dst_min_x = -dst_cx;

    dst = dstbmp->bitmap + dstbmp->width * (dst_cy + d_y) + dst_cx + dst_min_x;
    rop = trans->rop;
    if (rop == BM_COPY)
    {
      for (d_x = dst_min_x; d_x < dst_max_x; d_x++)
      {
        src_ty = (h >> 1) + ((d_y * tA - d_x * tC) << 8) / transMod;
        src_tx = (w >> 1) + ((d_x * tD - d_y * tB) << 8) / transMod;
        if (src_ty < h && src_ty >= 0 && src_tx < w && src_tx >= 0)
        {
          src = srcbmp->bitmap + srcbmp->width * (sy + src_ty) + sx + src_tx;
          *dst = *src;
        }
        dst++;
      }
    }
    else if (rop == BM_TRANSPARENT)
    {
      for (d_x = dst_min_x; d_x < dst_max_x; d_x++)
      {
        src_ty = (h >> 1) + ((d_y * tA - d_x * tC) << 8) / transMod;
        src_tx = (w >> 1) + ((d_x * tD - d_y * tB) << 8) / transMod;
        if (src_ty < h && src_ty >= 0 && src_tx < w && src_tx >= 0)
        {
          src = srcbmp->bitmap + srcbmp->width * (sy + src_ty) + sx + src_tx;
          if (*src != transcoler)
            *dst = *src;
        }
        dst++;
      }
    }
  }
}

// void drawBitmap565Flip(BITMAP_565 *b, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty)
// {
//   drawBitmap565Ex(b, x, y, w, h, tx, ty, w, h);
// }
/*
��bitmap��ת���Ƶ���Ļ��
scrx scry ���Ƶ���Ļ�ϵ�����λ��
bx by ͼƬ��ת����

*/
void drawBitmap565Rotate(BITMAP_565 *b, int32 centerX, int32 centerY, int32 bx, int32 by, int32 r)
{
  int32 x, y;
  int32 newX, newY;
  int32 x1, y1, x2, y2;
  // uint16 color11, color12, color21, color22;
  // double dx, dy;
  uint16 color;

  float radian = r * M_PI / 180.0; // ���Ƕ�ת��Ϊ����
  float cosR = cos(radian);
  float sinR = sin(radian);
  int32 minX = SCRW;
  int32 maxX = 0;
  int32 minY = SCRH;
  int32 maxY = 0;
  int i;
  // ������ε��ĸ�����
  int16 vertices[4][2]; // ����������ʼ��
  uint16 *screenBuffer = w_getScreenBuffer();

  // ����Խ��ߵĳ���
  // int32 diagonal = (int)sqrt(b->width * b->width + b->height * b->height);

  // ʹ�ü���ĳ���ֵ��� vertices
  vertices[0][0] = centerX - bx; // ���Ͻ�
  vertices[0][1] = centerY - by;

  vertices[1][0] = centerX + b->width - bx; // ���Ͻ�
  vertices[1][1] = centerY - (b->height - by);

  vertices[2][0] = centerX + (b->width - bx); // ���½�
  vertices[2][1] = centerY + (b->height - by);

  vertices[3][0] = centerX - bx; // ���½�
  vertices[3][1] = centerY + (b->height - by);

  // ��תÿ������
  for (i = 0; i < 4; i++)
  {
    x = vertices[i][0] - centerX;
    y = vertices[i][1] - centerY;
    vertices[i][0] = centerX + x * cosR - y * sinR;
    vertices[i][1] = centerY + x * sinR + y * cosR;
  }

  for (i = 0; i < 4; i++)
  {
    // ������С����� x ֵ
    if (vertices[i][0] < minX)
    {
      minX = vertices[i][0];
    }
    if (vertices[i][0] > maxX)
    {
      maxX = vertices[i][0];
    }

    // ������С����� y ֵ
    if (vertices[i][1] < minY)
    {
      minY = vertices[i][1];
    }
    if (vertices[i][1] > maxY)
    {
      maxY = vertices[i][1];
    }
  }

  minX = MAX(0, minX);
  minY = MAX(0, minY);
  maxX = MIN(maxX, SCRW);
  maxY = MIN(maxY, SCRH);
  mrc_printf("minX = %d, minY = %d, maxX = %d, maxY = %d", minX, minY, maxX, maxY);
  // mrc_drawRect(minX, minY, maxX-minX, maxY-minY, 240, 20, 20);
  // ������Ļ��ÿһ������
  if (b->mode == BM_TRANSPARENT)
  {
    for (y = minY; y < maxY; y++)
    {
      for (x = minX; x < maxX; x++)
      {
        // ������ת�������
        newX = (int32)(bx + (x - centerX) * cosR + (y - centerY) * sinR);
        newY = (int32)(by - (x - centerX) * sinR + (y - centerY) * cosR);

        // ��������Ƿ���ͼƬ��Χ��
        if (newX >= 0 && newX < b->width && newY >= 0 && newY < b->height)
        {
          // ʹ��˫���Բ�ֵ������ɫֵ
          x1 = newX;
          y1 = newY;
          /*
          x2 = (x1 + 1) < b->width ? (x1 + 1) : x1;
          y2 = (y1 + 1) < b->height ? (y1 + 1) : y1;

          color11 = b->bitmap[y1 * b->width + x1];
          color12 = b->bitmap[y2 * b->width + x1];
          color21 = b->bitmap[y1 * b->width + x2];
          color22 = b->bitmap[y2 * b->width + x2];

          dx = newX - x1;
          dy = newY - y1;

          color = (uint16)((1 - dx) * (1 - dy) * color11 +
                           dx * (1 - dy) * color21 +
                           (1 - dx) * dy * color12 +
                           dx * dy * color22);
                           */
          color = b->bitmap[y1 * b->width + x1];
          if (color == b->transcolor)
          {
            continue;
          }
          // ����ɫд����Ļ������
          screenBuffer[y * SCRW + x] = color;
        }
      }
    }
  }
  else
  {
    for (y = minY; y < maxY; y++)
    {
      for (x = minX; x < maxX; x++)
      {
        // ������ת�������
        newX = (int32)(bx + (x - centerX) * cosR + (y - centerY) * sinR);
        newY = (int32)(by - (x - centerX) * sinR + (y - centerY) * cosR);

        // ��������Ƿ���ͼƬ��Χ��
        if (newX >= 0 && newX < b->width && newY >= 0 && newY < b->height)
        {
          // ʹ��˫���Բ�ֵ������ɫֵ
          x1 = newX;
          y1 = newY;
          /*
          x2 = (x1 + 1) < b->width ? (x1 + 1) : x1;
          y2 = (y1 + 1) < b->height ? (y1 + 1) : y1;

          color11 = b->bitmap[y1 * b->width + x1];
          color12 = b->bitmap[y2 * b->width + x1];
          color21 = b->bitmap[y1 * b->width + x2];
          color22 = b->bitmap[y2 * b->width + x2];

          dx = newX - x1;
          dy = newY - y1;

          color = (uint16)((1 - dx) * (1 - dy) * color11 +
                           dx * (1 - dy) * color21 +
                           (1 - dx) * dy * color12 +
                           dx * dy * color22);
          */
          color = b->bitmap[y1 * b->width + x1];
          // ����ɫд����Ļ������
          screenBuffer[y * SCRW + x] = color;
        }
      }
    }
  }
  // ������ת��ľ���
  for (i = 1; i < 4; i++)
  {
    x1 = vertices[i][0];
    y1 = vertices[i][1];
    x2 = vertices[(i + 1) % 4][0];
    y2 = vertices[(i + 1) % 4][1];
    mrc_drawLine(x1, y1, x2, y2, 23, 22, 22);
  }
}

/*
 {
    int32 x, y, newX, newY;
    int32 screenX, screenY;
    uint16 *screenBuffer = w_getScreenBuffer();
    // #define M_PI		3.14159265358979323846
    // ������ת�ǶȵĻ���ֵ
    double radian = r * M_PI / 180.0; // ���Ƕ�ת��Ϊ����
    double cosR = cos(radian);
    double sinR = sin(radian);

    // ����ͼƬ��ÿһ������
    for (y = 0; y < b->height; y++) {
        for (x = 0; x < b->width; x++) {
            // ԭʼͼ�����ɫֵ
            uint16 color = b->bitmap[y * b->width + x];

            // �����ǰ������͸��ɫ��������
            if (color == b->transcolor) {
                continue;
            }

            // ������ת�������
            newX = (int32)(bx + (x - bx) * cosR - (y - by) * sinR);
            newY = (int32)(by + (x - bx) * sinR + (y - by) * cosR);

            // ������Ļ�ϵ�λ��
             screenX = scrx + newX;
             screenY = scry + newY;

            // ��������Ƿ�����Ļ��Χ��
            if (screenX >= 0 && screenX < SCRW && screenY >= 0 && screenY < SCRH) {
                // ����ɫд����Ļ������
                screenBuffer[screenY * SCRW + screenX] = color;
            }
        }
    }
}
*/

// ������ת����
// bx by ����ھ��ε���ת����
void gl_drawRotatedHollowRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color)
{
  int16 x1, y1, x2, y2;
  float radian = angle * M_PI / 180.0; // ���Ƕ�ת��Ϊ����
  float cosR = cos(radian);
  float sinR = sin(radian);
  int16 x, y;
  int i;
  // ������ε��ĸ�����
  int16 vertices[4][2]; // ����������ʼ��

  // ʹ�ü���ĳ���ֵ��� vertices
  vertices[0][0] = centerX - bx; // ���Ͻ�
  vertices[0][1] = centerY - by;

  vertices[1][0] = centerX + width - bx; // ���Ͻ�
  vertices[1][1] = centerY - by;

  vertices[2][0] = centerX + (width - bx); // ���½�
  vertices[2][1] = centerY + (height - by);

  vertices[3][0] = centerX - bx; // ���½�
  vertices[3][1] = centerY + (height - by);

  // ��תÿ������
  for (i = 0; i < 4; i++)
  {
    x = vertices[i][0] - centerX;
    y = vertices[i][1] - centerY;
    vertices[i][0] = centerX + x * cosR - y * sinR;
    vertices[i][1] = centerY + x * sinR + y * cosR;
  }

  // ������ת��ľ���
  for (i = 0; i < 4; i++)
  {
    x1 = vertices[i][0];
    y1 = vertices[i][1];
    x2 = vertices[(i + 1) % 4][0];
    y2 = vertices[(i + 1) % 4][1];
    gl_drawLine(x1, y1, x2, y2, color);
  }
}

void gl_drawRotatedRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color)
{
  float radian = angle * M_PI / 180.0; // ���Ƕ�ת��Ϊ����
  float cosR = cos(radian);
  float sinR = sin(radian);
  int16 x, y;
  int i;
  // ������ε��ĸ�����
  int16 vertices[4][2]; // ����������ʼ��

  // ʹ�ü���ĳ���ֵ��� vertices
  vertices[0][0] = centerX - bx; // ���Ͻ�
  vertices[0][1] = centerY - by;

  vertices[1][0] = centerX + width - bx; // ���Ͻ�
  vertices[1][1] = centerY - by;

  vertices[2][0] = centerX + (width - bx); // ���½�
  vertices[2][1] = centerY + (height - by);

  vertices[3][0] = centerX - bx; // ���½�
  vertices[3][1] = centerY + (height - by);

  // ��תÿ������
  for (i = 0; i < 4; i++)
  {
    x = vertices[i][0] - centerX;
    y = vertices[i][1] - centerY;
    vertices[i][0] = centerX + x * cosR - y * sinR;
    vertices[i][1] = centerY + x * sinR + y * cosR;
  }

  // ������ת��ľ���
  gl_drawTriangle(vertices[0][0], vertices[0][1], vertices[1][0], vertices[1][1], vertices[2][0], vertices[2][1], color);
  gl_drawTriangle(vertices[0][0], vertices[0][1], vertices[3][0], vertices[3][1], vertices[2][0], vertices[2][1], color);
}

/*
��bitmap���Ƶ���һ��bitmap��
��buf�е�bitmapͼƬ, ��sx,sy��ʼ�Ŀ��Ϊw,h������,���Ƶ�ͼƬdi��x,y��ʼ�ĵط�
*/
void drawBitmap565Old(BITMAP_565 *di, BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
{
    int32 j;
    int32 i;

    // ����Ŀ�����곬���߽�����
    if (x < 0)
    {
        w += x; // �Կ�Ƚ��е���
        sx -= x; // ����ԴͼƬ����ʼx����
        x = 0; // ����Ŀ��xΪ0
    }
    if (y < 0)
    {
        h += y; // �Ը߶Ƚ��е���
        sy -= y; // ����ԴͼƬ����ʼy����
        y = 0; // ����Ŀ��yΪ0
    }
    if (x + w > di->width)
    {
        w = di->width - x; // ȷ����Ȳ�����Ŀ��λͼ
    }
    if (y + h > di->height)
    {
        h = di->height - y; // ȷ���߶Ȳ�����Ŀ��λͼ
    }

    // ����ԴͼƬ�����߽�����
    if (sx < 0)
    {
        w += sx; // �Կ�Ƚ��е���
        sx = 0; // ����ԴͼƬ����ʼ����Ϊ0
    }
    if (sy < 0)
    {
        h += sy; // �Ը߶Ƚ��е���
        sy = 0; // ����ԴͼƬ����ʼ����Ϊ0
    }
    if (sx + w > buf->width)
    {
        w = buf->width - sx; // ȷ����Ȳ�����Դλͼ
    }
    if (sy + h > buf->height)
    {
        h = buf->height - sy; // ȷ���߶Ȳ�����Դλͼ
    }

    // �������ؿ���
    for (j = 0; j < h; ++j)
    {
        if (buf->mode == BM_TRANSPARENT)
        {
            for (i = 0; i < w; ++i)
            {
                int32 src_x = sx + i;
                int32 src_y = sy + j;
                uint16 color = buf->bitmap[src_y * buf->width + src_x]; // ��buf�л�ȡ��ɫ

                // ������ɫ������͸��ɫʱ���Ž��л���
                if (color != buf->transcolor)
                {
                    int32 dest_x = x + i;
                    int32 dest_y = y + j;
                    di->bitmap[dest_y * di->width + dest_x] = color; // ���Ƶ�Ŀ��λͼ
                }
            }
        }
        else
        {
            // �������Ҫ͸������ֱ��ʹ���ڴ渴��
            mrc_memcpy(di->bitmap + (y + j) * di->width + x, 
                        buf->bitmap + (sy + j) * buf->width + sx, 
                        w * 2);
        }
    }
}

/*
��bitmap����,����һ���µ�BITMAP
width ���ɵ�ͼƬ���
height ���ɵ�ͼƬ�߶�
*/
BITMAP_565 *createBitmapFromBitmap(BITMAP_565 *bmp, int32 width, int32 height)
{
  int32 x_ratio;
  int32 y_ratio;
  int32 src_x, src_y;
  int32 x, y;
  uint32 *bitmap32;
  uint32 *bitmap32_new;
  // ��������
  BITMAP_565 *new_bmp = (BITMAP_565 *)malloc(sizeof(BITMAP_565)); // �����ڴ�������λͼ
  memset(new_bmp, 0, sizeof(BITMAP_565));
  if (!new_bmp)
  {
    return NULL; // ����ʧ��
  }

  new_bmp->width = width;
  new_bmp->height = height;
  new_bmp->color_bit = bmp->color_bit;   // Ĭ����ɫλ��
  new_bmp->transcolor = bmp->transcolor; // Ĭ��͸��ɫ
  new_bmp->mode = bmp->mode;             // Ĭ��ģʽ��������0����BM_COPY��

  // �����ڴ�������λͼ����
  if (bmp->color_bit == 16)
  {
    new_bmp->bitmap = (uint16 *)mr_malloc(width * height * sizeof(uint16));
    new_bmp->buflen = width * height * sizeof(uint16);
  }
  else if (bmp->color_bit == 32)
  {
    new_bmp->bitmap = (uint16 *)mr_malloc(width * height * sizeof(uint32));
    new_bmp->buflen = width * height * sizeof(uint32);
  }
  else
  {
    mrc_printf("Unsupported color depth %d\n", bmp->color_bit);
    return NULL;
  }

  if (!new_bmp->bitmap)
  {
    free(new_bmp); // �ͷ��ѷ�����ڴ�
    mrc_printf("Failed to allocate memory for new bitmap\n");
    return NULL; // ����ʧ��
  }

  // �������ţ��򵥵�����ڲ�ֵ��
  x_ratio = (bmp->width << 16) / width + 1;   // ʹ�ù̶��������м���
  y_ratio = (bmp->height << 16) / height + 1; // ʹ�ù̶��������м���

  for (y = 0; y < height; y++)
  {
    for (x = 0; x < width; x++)
    {
      src_x = (x * x_ratio) >> 16; // ʹ��λ�������ȡԴͼx����
      src_y = (y * y_ratio) >> 16; // ʹ��λ�������ȡԴͼy����
      if (src_x >= 0 && src_y >= 0 && src_x < bmp->width && src_y < bmp->height)
      {
        if (bmp->color_bit == 16)
        {
          new_bmp->bitmap[y * width + x] = bmp->bitmap[src_y * bmp->width + src_x];
        }
        else if (bmp->color_bit == 32)
        {
          bitmap32 = (uint32 *)bmp->bitmap;
          bitmap32_new = (uint32 *)new_bmp->bitmap;
          bitmap32_new[y * width + x] = bitmap32[src_y * bmp->width + src_x];
        }
      }
    }
  }

  return new_bmp; // ���������ɵ�λͼ
}
/*
��bitmap��(tx,ty,tw,th)�������Ż��Ƶ���Ļ(x,y,w,h)������
������
x ���Ƶ���Ļ�ϵ�x����
y ���Ƶ���Ļ�ϵ�y����
w ���ƿ�� h ���Ƹ߶� tx ty tw th �ü�����
*/
void drawBitmapEx(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th)
{
  int px, py; // ��Ļ��������(���)
  int32 pindex;
  uint16 *screenBuffer;
  int32 temp;
  uint32 *bitmap32;

  if (w == tw && h == th)
  {
    if (bmp->color_bit == 16)
    {
      gl_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)tw, (uint16)th, (uint16)bmp->mode, tx, ty, (uint16)bmp->transcolor);
    }
    else if (bmp->color_bit == 32)
    {
      gl_bitmapShowExTrans8888((uint32 *)bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)tw, (uint16)th, tx, ty);
    }
  }
  else
  {
    if (x + w > SCRW)
    {
      temp = w;
      w = SCRW - x;
      tw = tw * w / temp;
    }
    if (y + h > SCRH)
    {
      temp = h;
      h = SCRH - y;
      th = th * h / temp;
    }
    if (tx + tw > bmp->width || ty + th > bmp->height)
    {
      mrc_printf("drawBitmapEx error, tw or th err");
      return;
    }
    if (bmp->color_bit == 16)
    {
      screenBuffer = w_getScreenBuffer();
      switch (bmp->mode)
      {
      case BM_COPY:
        // ������Ļ��������ͼƬ�ϵĵ�
        for (px = 0; px < w; px++)
        {
          for (py = 0; py < h; py++)
          {
            pindex = bmp->width * (tx + py * th / h) + (ty + px * tw / w);
            if (pindex >= 0 && pindex < bmp->width * bmp->height && py + y >= 0 && px + x >= 0)
            {
              // mrc_drawPoint(px + x, py + y, *(bmp->bitmap + pindex));
              screenBuffer[SCRW * (py + y) + px + x] = bmp->bitmap[pindex];
            }
          }
        }
        break;
      case BM_TRANSPARENT:
        // ������Ļ��������ͼƬ�ϵĵ�
        for (px = 0; px < w; px++)
        {
          for (py = 0; py < h; py++)
          {
            pindex = bmp->width * (tx + py * th / h) + (ty + px * tw / w);
            if (pindex >= 0 && pindex < bmp->width * bmp->height && py + y >= 0 && px + x >= 0)
            {
              // mrc_drawPoint(px + x, py + y, *(bmp->bitmap + pindex));
              if (bmp->bitmap[pindex] != bmp->transcolor)
                screenBuffer[SCRW * (py + y) + px + x] = bmp->bitmap[pindex];
            }
          }
        }
      default:
        break;
      }
    }
    else if (bmp->color_bit == 32)
    {
      screenBuffer = w_getScreenBuffer();
      bitmap32 = (uint32 *)bmp->bitmap;
      // ������Ļ��������ͼƬ�ϵĵ�
      for (px = 0; px < w; px++)
      {
        for (py = 0; py < h; py++)
        {
          pindex = bmp->width * (tx + py * th / h) + (ty + px * tw / w);
          if (pindex >= 0 && pindex < bmp->width * bmp->height && py + y >= 0 && px + x >= 0)
          {
            // mrc_drawPoint(px + x, py + y, *(bmp->bitmap + pindex));
            if (bitmap32[pindex] >> 24 != 0)
              screenBuffer[SCRW * (py + y) + px + x] = blendColor888(screenBuffer[SCRW * (py + y) + px + x], bitmap32[pindex]);
          }
        }
      }
    }
  }
}

int32 bitmapFree(BITMAP_565 *b)
{
  bmp_free(b);
  return 0;
}

// ��ȡ����֮��ĳ��� ��ƽ��
int gl_getLineSize(int x, int y, int x2, int y2)
{
  return (x2 - x) * (x2 - x) + (y2 - y) * (y2 - y);
}

// ���������ɫ
int32 gl_getColor(int32 color1, uint32 color2)
{
  // printf("getColor %x %x\n",color1,color2);
  // int a1 = (color1>>24)&0xff;
  int r1 = (color1 >> 16) & 0xff;
  int g1 = (color1 >> 8) & 0xff;
  int b1 = color1 & 0xff;
  // printf("a1=%d r1=%d g1=%d b1=%d\n",a1,r1,g1,b1);
  // int a2 = (color2>>24)&0xff;
  int r2 = (color2 >> 16) & 0xff;
  int g2 = (color2 >> 8) & 0xff;
  int b2 = color2 & 0xff;
  // printf("a2=%d r2=%d g2=%d b2=%d\n",a2,r2,g2,b2);
  int draw_a = (color2 >> 24) & 0xff;
  // int a = a1 * (255-draw_a)/255 + a2 * draw_a/255;
  int r = r1 * (255 - draw_a) / 255 + r2 * draw_a / 255;
  // printf("drawa = %d\nr = %d + %d\n",draw_a, r1 * (255-draw_a)/255 , draw_a/255);
  int g = g1 * (255 - draw_a) / 255 + g2 * draw_a / 255;
  int b = b1 * (255 - draw_a) / 255 + b2 * draw_a / 255;
  // printf("a=%d,r=%d,g=%d,b=%d\n",a,r,g,b);
  return (0xff000000) | (r << 16) | (g << 8) | b;
}

#ifndef USE_DSP
uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha)
{
  uint8 r1 = (dstColor >> 11) & 0x1F; // ��ȡĿ����ɫ�ĺ�ɫ����
  uint8 g1 = (dstColor >> 5) & 0x3F;  // ��ȡĿ����ɫ����ɫ����
  uint8 b1 = dstColor & 0x1F;         // ��ȡĿ����ɫ����ɫ����

  uint8 r2 = (srcColor >> 11) & 0x1F; // ��ȡԴ��ɫ�ĺ�ɫ����
  uint8 g2 = (srcColor >> 5) & 0x3F;  // ��ȡԴ��ɫ����ɫ����
  uint8 b2 = srcColor & 0x1F;         // ��ȡԴ��ɫ����ɫ����

  // ���� alpha ���
  // uint8 r = (r1 * (255 - alpha) + r2 * alpha) / 255;
  // uint8 g = (g1 * (255 - alpha) + g2 * alpha) / 255;
  // uint8 b = (b1 * (255 - alpha) + b2 * alpha) / 255;
  int inv_alpha = 255 - alpha;
  uint8 r = (r1 * (inv_alpha) + r2 * alpha) >> 8;
  uint8 g = (g1 * (inv_alpha) + g2 * alpha) >> 8;
  uint8 b = (b1 * (inv_alpha) + b2 * alpha) >> 8;

  // �������Ϊ 16 λ RGB 565 ��ʽ
  return (r << 11) | (g << 5) | b;
}
#else
// �������rgb565��ɫ �����µ���ɫ
// datColor ��ɫ1
// srcColor ��ɫ2
// srcColor��͸����
uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha)
{
  uint8 r1 = (dstColor >> 11) & 0x1F; // ��ȡĿ����ɫ�ĺ�ɫ����
  uint8 g1 = (dstColor >> 5) & 0x3F;  // ��ȡĿ����ɫ����ɫ����
  uint8 b1 = dstColor & 0x1F;         // ��ȡĿ����ɫ����ɫ����

  uint8 r2 = (srcColor >> 11) & 0x1F; // ��ȡԴ��ɫ�ĺ�ɫ����
  uint8 g2 = (srcColor >> 5) & 0x3F;  // ��ȡԴ��ɫ����ɫ����
  uint8 b2 = srcColor & 0x1F;         // ��ȡԴ��ɫ����ɫ����

  // �����ϵĺ�ɫ����
  Word32 tempR = smlabb(r1, (Word32)(r2 * alpha), (Word32)(r1 * (255 - alpha)));
  uint8 r = tempR >> 8; // ����ʹ����128λ�ļӷ���������Ҫ����8λ

  // �����ϵ���ɫ����
  Word32 tempG = smlabb(g1, (Word32)(g2 * alpha), (Word32)(g1 * (255 - alpha)));
  uint8 g = tempG >> 8; // ����8λ

  // �����ϵ���ɫ����
  Word32 tempB = smlabb(b1, (Word32)(b2 * alpha), (Word32)(b1 * (255 - alpha)));
  uint8 b = tempB >> 8; // ����8λ

  // �������Ϊ16λ RGB 565��ʽ
  return (r << 11) | (g << 5) | b;
}
#endif

#ifndef USE_DSP
// �������rgb565 rgb8888��ɫ �����µ���ɫ
// datColor ��ɫ1
// srcColor ��ɫ2
uint16 blendColor888(uint16 dstColor, uint32 srcColor)
{
  uint8 r1 = (dstColor >> 11) & 0x1F; // ��ȡĿ����ɫ�ĺ�ɫ����
  uint8 g1 = (dstColor >> 5) & 0x3F;  // ��ȡĿ����ɫ����ɫ����
  uint8 b1 = dstColor & 0x1F;         // ��ȡĿ����ɫ����ɫ����

  uint8 r2 = (srcColor >> 19) & 0x1F; // ��ȡԴ��ɫ�ĺ�ɫ����
  uint8 g2 = (srcColor >> 10) & 0x3F; // ��ȡԴ��ɫ����ɫ����
  uint8 b2 = (srcColor >> 3) & 0x1F;  // ��ȡԴ��ɫ����ɫ����
  int32 alpha = srcColor >> 24;

  // ���� alpha ���
  uint8 r = (r1 * (255 - alpha) + r2 * alpha) >> 8;
  uint8 g = (g1 * (255 - alpha) + g2 * alpha) >> 8;
  uint8 b = (b1 * (255 - alpha) + b2 * alpha) >> 8;

  // �������Ϊ 16 λ RGB 565 ��ʽ
  return (r << 11) | (g << 5) | b;
}
#else

uint16 blendColor888(uint16 dstColor, uint32 srcColor)
{
  uint8 r1 = (dstColor >> 11) & 0x1F; // ��ȡĿ����ɫ�ĺ�ɫ����
  uint8 g1 = (dstColor >> 5) & 0x3F;  // ��ȡĿ����ɫ����ɫ����
  uint8 b1 = dstColor & 0x1F;         // ��ȡĿ����ɫ����ɫ����

  uint8 r2 = (srcColor >> 19) & 0x1F; // ��ȡԴ��ɫ�ĺ�ɫ����
  uint8 g2 = (srcColor >> 10) & 0x3F; // ��ȡԴ��ɫ����ɫ����
  uint8 b2 = (srcColor >> 3) & 0x1F;  // ��ȡԴ��ɫ����ɫ����
  int32 alpha = srcColor >> 24;

  // ���� alpha ���ʹ�� armdsp.h �ṩ�ĺ���
  // �����255 ��Ϊ�˱���������ڽ��и�������ʱ�򲢲���Ҫ��������
  uint32 r = smlabb(r1, 255 - alpha, r2 * alpha);
  uint32 g = smlabb(g1, 255 - alpha, g2 * alpha);
  uint32 b = smlabb(b1, 255 - alpha, b2 * alpha);

  // �����Ҫ�����������ŵ���ȷ��Χ
  r = (r >> 8) & 0x1F; // ��֤�����Χ�� 0-31
  g = (g >> 8) & 0x3F; // ��֤�����Χ�� 0-63
  b = (b >> 8) & 0x1F; // ��֤�����Χ�� 0-31

  // �������Ϊ 16 λ RGB 565 ��ʽ
  return (r << 11) | (g << 5) | b;
}
#endif

// ���� ������x, y, argb
void gl_drawPoint(int x, int y, uint32 color)
{
  uint16 *scr_color;
  uint16 *buffer;
  // int32 scr_color = 0;
  // int32 o_color = 0;
  // uint16 scr_color16 = 0;
  // // ��ȡ��Ļ��ɫ
  // if (x < 0 || x >= SCRW)
  //   return;
  // if (y < 0 || y >= SCRH)
  //   return;
  // scr_color16 = *(getscrbuf() + (SCRW * y + x));
  // scr_color = ((scr_color16 << 8) & 0xf80000) | ((scr_color16 << 5) & 0xfc00) | (scr_color16 << 3) & 0xff;
  // // ���
  // o_color = gl_getColor(scr_color, color);
  if (x < 0 || y < 0 || x >= SCRW || y >= SCRH)
    return;
  buffer = w_getScreenBuffer();
  scr_color = buffer + SCRW * y + x;

  *scr_color = blendColor888(*scr_color, color);
  // ����rgb565
  //*(getscrbuf()+ (SCRW*y+x)) = MAKECOLOR565(o_color);
  // mrc_drawPoint(x, y, MAKECOLOR565(o_color));
  //*(((uint16*)getscrbuf())+ (SCRW*y+x)*2) = 0xffff;
  // printf("setzcolor %x\n",MAKECOLOR565(o_color));
}
void gl_drawColor(uint32 color)
{
  uint16 *buffer = w_getScreenBuffer();
  int32 len = SCRW * SCRH;
  uint16 upcolor;
  int i;
  uint16 upscrcolor = buffer[0];
  upcolor = blendColor888(upscrcolor, color);

  for (i = 0; i < len; i++)
  {
    if (upscrcolor == buffer[i])
    {
      buffer[i] = upcolor;
    }
    else
    {
      upcolor = buffer[i] = blendColor888(upscrcolor = buffer[i], color);
    }
  }
}

void gl_clearScreen(int32 r, int32 g, int32 b)
{
  int i;
  uint16 color = MAKERGB(r, g, b);
  uint16 *buffer = w_getScreenBuffer();
  if ((color >> 8) == (color & 0xff))
  {
    // mrc_printf("memset screen");
    mrc_memset(buffer, color, SCRW * SCRH * 2);
  }
  else
  {
    // mrc_printf("memcpy screen with color");
    for (i = 0; i < SCRW; i++)
    {
      buffer[i] = color;
    }
    for (i = 0; i < SCRH / 2; i++)
    {
      mrc_memcpy(buffer + i * SCRW, (const void *)buffer, SCRW * 2);
    }
    mrc_memcpy(buffer + SCRW * SCRH / 2, buffer, SCRW * SCRH);
  }
}

// ������
void gl_drawRect(int32 x, int32 y, int32 w, int32 h, uint32 color)
{
  int ix, iy;
  uint16 *buffer = w_getScreenBuffer();
  int32 maxX, maxY;

  uint16 upcolor;
  uint16 upscrcolor = 0;
  maxX = MIN(SCRW, x + w);
  maxY = MIN(SCRH, y + h);
  x = MAX(0, x);
  y = MAX(0, y);

  upcolor = blendColor888(upscrcolor, color);
  if ((color >> 24) == 0xff)
  {
    // mrc_printf("drawRect mrc");
    mrc_drawRect((int16)x, (int16)y, (int16)w, (int16)h, (uint8)(color >> 16) & 0xff, (uint8)(color >> 8) & 0xff, (uint8)color & 0xff);
  }
  else
  {
    for (ix = x; ix < maxX; ix++)
    {
      for (iy = y; iy < maxY; iy++)
      {
        if (upscrcolor == *(buffer + SCRW * iy + ix))
        {
          *(buffer + SCRW * iy + ix) = upcolor;
        }
        else
        {
          upcolor = *(buffer + SCRW * iy + ix) = blendColor888(upscrcolor = *(buffer + SCRW * iy + ix), color);
        }
      }
    }
  }
}

//���ƿ��ľ���
void gl_drawHollowRect(int x, int y, int width, int height, uint32 color){
  int ix, iy;
  uint16 *buffer = w_getScreenBuffer();
  int32 maxX, maxY;
  maxX = MIN(SCRW, x + width);
  maxY = MIN(SCRH, y + height);
  x = MAX(0, x);
  y = MAX(0, y);
  gl_drawLine(x, y, x + width - 1, y, color);
  gl_drawLine(x, y + height - 1, x + width - 1, y + height - 1, color);
  gl_drawLine(x, y, x, y + height - 1, color);
  gl_drawLine(x + width - 1, y, x + width - 1, y + height - 1, color);
}

void gl_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
  int minX, maxX, minY, maxY;
  int x, y;
  uint16 *buffer = w_getScreenBuffer();
  uint16 color565;
  int32 alpha;
  uint16 upcolor;
  uint16 upsrccolor = 0;
  color565 = MAKECOLOR565(color);
  alpha = color >> 24;
  upcolor - blendColor888(upsrccolor, color);
  // ��������
  // bmp_drawLine(bmp, x1, y1, x2, y2, color);
  // bmp_drawLine(bmp, x2, y2, x3, y3, color);
  // bmp_drawLine(bmp, x3, y3, x1, y1, color);

  // ���������
  // ʹ�ñ߽�����㷨
  minX = MIN(MIN(x1, x2), x3);
  maxX = MAX(MAX(x1, x2), x3);
  minY = MIN(MIN(y1, y2), y3);
  maxY = MAX(MAX(y1, y2), y3);
  minX = MAX(0, minX);
  maxX = MIN(SCRW, maxX);
  minY = MAX(0, minY);
  maxY = MIN(SCRH, maxY);

  for (y = minY; y < maxY; y++)
  {
    for (x = minX; x < maxX; x++)
    {
      // �����Ƿ����������ڲ�
      if (pointInTriangle(x, y, x1, y1, x2, y2, x3, y3))
      {
        if (upsrccolor == *(buffer + SCRW * y + x))
        {
          *(buffer + SCRW * y + x) = upcolor;
        }
        else
        {
          upcolor = *(buffer + SCRW * y + x) = blendColor888(upsrccolor = *(buffer + SCRW * y + x), color);
          // gl_drawPoint(x, y, color);
        }
      }
    }
  }
}

void gl_drawHLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{
  int32 i;
  uint16 *buffer = w_getScreenBuffer();
  int32 minX;
  int32 maxX;
  uint16 upsrccolor = 0;
  uint16 upcolor = blendColor888(upsrccolor, color);
  if (x1 < x2)
  {
    minX = MIN(MAX(0, x1), SCRW);
    maxX = MIN(MAX(0, x2), SCRW);
  }
  else
  {
    minX = MIN(MAX(0, x2), SCRW);
    maxX = MIN(MAX(0, x1), SCRW);
  }
  if (y1 < 0 || y1 >= SCRH)
    return;
  for (i = minX; i < maxX; i++)
  {
    if (upsrccolor == buffer[SCRW * y1 + i])
    {
      buffer[SCRW * y1 + i] = upcolor;
    }
    else
    {
      upcolor = buffer[SCRW * y1 + i] = blendColor888(upsrccolor = buffer[SCRW * y1 + i], color);
    }

    // mrc_drawPoint(x1,y1, MAKECOLOR565(color));
  }
}

void gl_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{

  // uint16 color565 = MAKECOLOR565(color);
  // int alpha = (color >> 24) & 0xff;

  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;
  int err2;
  // int32 alpha = (color >> 24);
  // uint16 color565 = MAKECOLOR565(color);
  uint16 *buffer = w_getScreenBuffer();
  while (1)
  {
    // gl_drawPoint(x1, y1, color); // ���Ƶ�ǰ��
    if (x1 >= 0 && y1 >= 0 && x1 < SCRW && y1 < SCRH)
    {
      *(buffer + SCRW * y1 + x1) = blendColor888(*(buffer + SCRW * y1 + x1), color);
    }

    if (x1 == x2 && y1 == y2)
      break; // ��������յ㣬����ѭ��
    err2 = err * 2;

    if (err2 > -dy)
    {
      err -= dy;
      x1 += sx;
    }
    if (err2 < dx)
    {
      err += dx;
      y1 += sy;
    }
  }
}

void gl_drawHollowTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
  // ����������
  gl_drawLine(x1, y1, x2, y2, color);
  gl_drawLine(x2, y2, x3, y3, color);
  gl_drawLine(x3, y3, x1, y1, color);
}

// ��bitmap�ڻ��ƾ���
void bmp_drawRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color)
{
  int ix, iy;
  // ȷ����������Ч��Χ��
  uint16 dstColor, newColor;
  int index;
  int startX = x < 0 ? 0 : x;
  int startY = y < 0 ? 0 : y;
  int endX = (x + w > bmp->width) ? bmp->width : (x + w);
  int endY = (y + h > bmp->height) ? bmp->height : (y + h);
  uint16 color565 = MAKECOLOR565(color);
  int alpha = (color >> 24) & 0xff;

  if (!bmp || bmp->bitmap == NULL)
  {
    return; // ���ָ����Ч��
  }
  // ѭ���������ε�ÿһ�����ز����л���
  for (ix = startX; ix < endX; ix++)
  {
    for (iy = startY; iy < endY; iy++)
    {
      // ������ bitmap �е�λ��
      index = iy * bmp->width + ix;

      // ȡ����ǰ������ɫ
      dstColor = bmp->bitmap[index];

      // �����ǰ��ɫ��͸��ɫ��������
      if (dstColor == bmp->transcolor)
      {
        continue;
      }

      // �����ɫ�������µ� bitmap ��
      newColor = blendColor(dstColor, (uint16)color565, alpha); // ͸���ȿ��Ը�����Ҫ����
      bmp->bitmap[index] = newColor;
    }
  }
}

// ��bitmap�ڻ��Ƶ�
void bmp_drawPoint(BITMAP_565 *bmp, int32 x, int32 y, uint32 color)
{
  int index;
  uint16 dstColor, color565;
  int alpha;
  if (!bmp || bmp->bitmap == NULL)
  {
    return; // ���ָ����Ч��
  }

  // ������ bitmap �е�λ��
  index = y * bmp->width + x;

  // ȷ����������Ч��Χ��
  if (x >= 0 && x < bmp->width && y >= 0 && y < bmp->height)
  {
    dstColor = bmp->bitmap[index];
    color565 = MAKECOLOR565(color);
    alpha = (color >> 24) & 0xff;

    // �����ǰ��ɫ��͸��ɫ��������
    if (dstColor != bmp->transcolor)
    {
      // �����ɫ�������µ� bitmap ��
      bmp->bitmap[index] = blendColor(dstColor, color565, alpha);
    }
  }
}

// ��bitmap�ڻ����߶�
void bmp_drawLine(BITMAP_565 *bmp, int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{

  // uint16 color565 = MAKECOLOR565(color);
  // int alpha = (color >> 24) & 0xff;

  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;
  int err2;
  if (!bmp || bmp->bitmap == NULL)
  {
    return; // ���ָ����Ч��
  }

  while (1)
  {
    bmp_drawPoint(bmp, x1, y1, color); // ���Ƶ�ǰ��

    if (x1 == x2 && y1 == y2)
      break; // ��������յ㣬����ѭ��
    err2 = err * 2;

    if (err2 > -dy)
    {
      err -= dy;
      x1 += sx;
    }
    if (err2 < dx)
    {
      err += dx;
      y1 += sy;
    }
  }
}

// ��bitmap�ڻ���Բ��
void bmp_drawCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 color)
{

  int16 x, y;
  // uint16 color565 = MAKECOLOR565(color);
  // int alpha = (color >> 24) & 0xff;
  if (!bmp || bmp->bitmap == NULL)
  {
    return; // ���ָ����Ч��
  }

  for (y = -radius; y <= radius; y++)
  {
    for (x = -radius; x <= radius; x++)
    {
      if (x * x + y * y <= radius * radius)
      {                                            // �ж��Ƿ���Բ��
        bmp_drawPoint(bmp, cx + x, cy + y, color); // ���Ƶ�
      }
    }
  }
}

// ��bitmap�ڻ��ƿ���Բ
void bmp_drawHollowCircle(BITMAP_565 *bmp, int32 centerX, int32 centerY, int32 radius, uint32 color)
{
  int32 x, y;
  int32 angle;

  if (!bmp || bmp->bitmap == NULL)
  {
    return; // ���ָ����Ч��
  }

  // uint16 color565 = MAKECOLOR565(color);
  // int alpha = (color >> 24) & 0xff;

  // ����Բ����Χ�ĵ�
  for (angle = 0; angle < 360; angle++)
  {
    // ʹ�ü��������Բ���ϵĵ�
    x = (int32)(centerX + radius * cos(angle * M_PI / 180.0));
    y = (int32)(centerY + radius * sin(angle * M_PI / 180.0));

    // ȷ�����ںϷ���Χ��
    if (x >= 0 && x < bmp->width && y >= 0 && y < bmp->height)
    {
      bmp_drawPoint(bmp, x, y, color);
    }
  }
}

#ifndef USE_DSP
// ��һ�����������
int tri_area(int x1, int y1, int x2, int y2, int x3, int y3)
{
  // return (1/2)*(x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2);
  int m = ((x2 - x1) * (y3 - y2)) - ((x3 - x2) * (y2 - y1));
  return (m > 0) ? m : -m;
}

// �жϵ��Ƿ��������΃�
int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3)
{
  if (tri_area(x, y, x1, y1, x2, y2) + tri_area(x, y, x1, y1, x3, y3) + tri_area(x, y, x2, y2, x3, y3) == tri_area(x1, y1, x2, y2, x3, y3))
    return TRUE;
  return FALSE;
}
#else

// ��һ�����������
int tri_area(int x1, int y1, int x2, int y2, int x3, int y3)
{
  // ʹ�� DSP ָ���Ż�����
  // m = (x2 - x1) * (y3 - y2) - (x3 - x2) * (y2 - y1)
  Word32 area1, area2, m;

  area1 = smulbb(x2 - x1, y3 - y2); // (x2-x1)*(y3-y2)
  area2 = smulbb(x3 - x2, y2 - y1); // (x3-x2)*(y2-y1)

  m = qsub(area1, area2); // m = area1 - area2

  return (m > 0) ? m : -m; // ����ı���У����ַ�����ֵ
}

// �жϵ��Ƿ�����������
int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3)
{
  // ʹ�� DSP ָ���Ż��������
  Word32 area1 = tri_area(x, y, x1, y1, x2, y2);
  Word32 area2 = tri_area(x, y, x1, y1, x3, y3);
  Word32 area3 = tri_area(x, y, x2, y2, x3, y3);
  Word32 total_area = tri_area(x1, y1, x2, y2, x3, y3);

  return (qadd(area1, qadd(area2, area3)) == total_area) ? TRUE : FALSE; // ʹ�� qadd ���мӷ�
}

#endif

// int pointInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3)
// {
//   // �����������귨
//   float area = 0.5 * (-y2 * x3 + y1 * (-x2 + x3) + x1 * (y2 - y3) + x2 * y3);
//   float s = 1 / (2 * area) * (y1 * x3 - x1 * y3 + (x3 - x1) * py + (x1 - x2) * px);
//   float t = 1 / (2 * area) * (x1 * y2 - y1 * x2 + (x1 - x2) * py + (y1 - y2) * px);
//   return (s > 0) && (t > 0) && (1 - s - t > 0);
// }

void bmp_drawTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
  int minX, maxX, minY, maxY;
  int x, y;
  // ��������
  // bmp_drawLine(bmp, x1, y1, x2, y2, color);
  // bmp_drawLine(bmp, x2, y2, x3, y3, color);
  // bmp_drawLine(bmp, x3, y3, x1, y1, color);

  // ���������
  // ʹ�ñ߽�����㷨
  minX = MIN(MIN(x1, x2), x3);
  maxX = MAX(MAX(x1, x2), x3);
  minY = MIN(MIN(y1, y2), y3);
  maxY = MAX(MAX(y1, y2), y3);

  for (y = minY; y < maxY; y++)
  {
    for (x = minX; x < maxX; x++)
    {
      // �����Ƿ����������ڲ�
      if (pointInTriangle(x, y, x1, y1, x2, y2, x3, y3))
      {
        bmp_drawPoint(bmp, x, y, color);
      }
    }
  }
}

void bmp_drawHollowTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
  // ����������
  bmp_drawLine(bmp, x1, y1, x2, y2, color);
  bmp_drawLine(bmp, x2, y2, x3, y3, color);
  bmp_drawLine(bmp, x3, y3, x1, y1, color);
}

// ��Բ
/*
void gl_drawCir(int32 x, int32 y, int32 r, uint32 color)
{
  int ix, iy;
  uint16 upcolor;
  uint16 upsrccolor = 0;
  int32 alpha = color >> 24;
  uint16 color565 = MAKECOLOR565(color);
  uint16 *buffer = w_getScreenBuffer();
  int32 minX = MAX(0, x - r);
  int32 minY = MAX(0, y - r);
  int32 maxX = MIN(SCRW, x + r);
  int32 maxY = MIN(SCRH, y + r);
  upcolor = blendColor(upsrccolor, color565, alpha);
  for (ix = minX; ix < maxX; ix++)
  {
    for (iy = minY; iy < maxY; iy++)
    {
      if (gl_getLineSize(ix, iy, x, y) <= r * r)
      {
        // ����Ч�����⣬��͸����Բ��������

        // if (alpha == 0xff)
        // {
        //   mrc_drawPoint(ix, iy, MAKECOLOR565(color));
        // }
        // else
        // {
        if (*(buffer + SCRW * iy + ix) == upsrccolor)
        {
          *(buffer + SCRW * iy + ix) = upcolor;
        }
        else
        {
          upcolor = *(buffer + SCRW * iy + ix) = blendColor(upsrccolor = *(buffer + SCRW * iy + ix), color565, alpha);
        }

        // gl_drawPoint(ix, iy, color);
        // }
      }
    }
  }
  // printf("color>>24 = %d\n",color>>24);
}
*/
/*
void gl_drawCir(int32 x,int32 y,int32 r,uint32 color){
int ix,iy;

for(ix=x-r;ix<x+r;ix++){
 for(iy=y-r;iy<y+r;iy++){
  if(gl_getLineSize(ix,iy, x,y)<= r*r){
   //����Ч�����⣬��͸����Բ��������

   if(color>>24==0xff){
    mrc_drawPoint(ix,iy, MAKECOLOR565(color));
   }
   else{
   gl_drawPoint(ix,iy, color);
   }
  }
 }
}
//printf("color>>24 = %d\n",color>>24);
}
*/

void gl_drawCir(int32 x0, int32 y0, int32 r, uint32 color)
{
  // ʹ����������Բ
  int x, y, f; //(x,y)Ϊ��ǰ���� fΪ���ֵ
  int upy;
  x = x0;
  y = y0 + r;
  f = 0;
  upy = -1;

  while (y >= y0)
  { // ��Բ�ܵ����ϲ��ֿ�ʼ
    if (upy != y)
    {
      gl_drawHLine(x, y, 2 * x0 - x, y, color);
      if (y0 != y)
      {
        gl_drawHLine(x, 2 * y0 - y, 2 * x0 - x, 2 * y0 - y, color);
      }

      upy = y;
    }

    if (f > 0)
    {
      f = f - 2 * (y - y0) + 1;
      y--; // ��Բ����
    }
    else
    {
      f = f + 2 * (x - x0) + 1;
      x++; // ��Բ����
    }
  }

  if (y == y0)
    gl_drawPoint(x, y, color);
}

void gl_drawHollowCir(int x0, int y0, int r, uint32 color)
{
  // ʹ����������Բ
  int x, y, f; //(x,y)Ϊ��ǰ���� fΪ���ֵ
  x = x0;
  y = y0 + r;
  f = 0;

  while (y >= y0)
  {                            // ��Բ�ܵ����ϲ��ֿ�ʼ
    gl_drawPoint(x, y, color); // �ԳƵػ����ĸ������ڵ������
    gl_drawPoint(2 * x0 - x, y, color);
    gl_drawPoint(x, 2 * y0 - y, color);
    gl_drawPoint(2 * x0 - x, 2 * y0 - y, color);

    if (f > 0)
    {
      f = f - 2 * (y - y0) + 1;
      y--; // ��Բ����
    }
    else
    {
      f = f + 2 * (x - x0) + 1;
      x++; // ��Բ����
    }
  }

  if (y == y0)
    gl_drawPoint(x, y, color);
}

// ��ȡ������ɫ֮����м�ֵ
uint32 getGraColor(uint32 color1, uint32 color2, int32 load, int32 max)
{
  int32 alpha1 = color1 >> 24;
  int32 r1 = (color1 >> 16) & 0xff;
  int32 g1 = (color1 >> 8) & 0xff;
  int32 b1 = color1 & 0xff;
  int32 alpha2 = color2 >> 24;
  int32 r2 = (color2 >> 16) & 0xff;
  int32 g2 = (color2 >> 8) & 0xff;
  int32 b2 = color2 & 0xff;
  int32 alpha3 = alpha1 + (alpha2 - alpha1) * load / max;
  int32 r3 = r1 + (r2 - r1) * load / max;
  int32 g3 = g1 + (g2 - g1) * load / max;
  int32 b3 = b1 + (b2 - b1) * load / max;
  return (alpha3 << 24) | (r3 << 16) | (g3 << 8) | (b3);
}

void drawShadeRect(int32 x, int32 y, int32 w, int32 h, uint32 colorA, uint32 colorB, int mode)
{
  int load;
  uint32 colortemp;
  uint16 color565;
  int32 ix, iy;
  int32 alpha;
  int32 ymax;
  int32 xmax;
  uint16 *buffer = w_getScreenBuffer();

  switch (mode)
  {
  case SHADE_UPDOWN:
    xmax = MIN(SCRW, x + w);
    x = MAX(0, x);
    y = MAX(0, y);
    ymax = MIN(SCRH, y + h);
    load = 0;
    for (iy = y; iy < ymax; iy++)
    {
      colortemp = getGraColor(colorA, colorB, load++, h);
      alpha = colortemp >> 24;
      color565 = MAKECOLOR565(colortemp);
      for (ix = x; ix < xmax; ix++)
      {
        buffer[SCRW * iy + ix] = blendColor(buffer[SCRW * iy + ix], color565, alpha);
      }
    }
    return;
  case SHADE_DOWNUP:
    xmax = MIN(SCRW, x + w);
    x = MAX(0, x);
    y = MAX(0, y);
    ymax = MIN(SCRH, y + h);
    load = 0;
    for (iy = y; iy < ymax; iy++)
    {
      colortemp = getGraColor(colorB, colorA, load++, h);
      alpha = colortemp >> 24;
      color565 = MAKECOLOR565(colortemp);
      for (ix = x; ix < xmax; ix++)
      {
        buffer[SCRW * iy + ix] = blendColor(buffer[SCRW * iy + ix], color565, alpha);
      }
    }
    return;
  case SHADE_LEFTRIGHT:
    xmax = MIN(SCRW, x + w);
    x = MAX(0, x);
    y = MAX(0, y);
    ymax = MIN(SCRH, y + h);
    load = 0;
    for (ix = x; ix < xmax; ix++)
    {
      colortemp = getGraColor(colorA, colorB, load++, w);
      alpha = colortemp >> 24;
      color565 = MAKECOLOR565(colortemp);
      for (iy = y; iy < ymax; iy++)
      {
        buffer[SCRW * iy + ix] = blendColor(buffer[SCRW * iy + ix], color565, alpha);
      }
    }
    return;
  case SHADE_RIGHTLEFT:
    xmax = MIN(SCRW, x + w);
    x = MAX(0, x);
    y = MAX(0, y);
    ymax = MIN(SCRH, y + h);
    load = 0;
    for (ix = x; ix < xmax; ix++)
    {
      colortemp = getGraColor(colorB, colorA, load++, w);
      alpha = colortemp >> 24;
      color565 = MAKECOLOR565(colortemp);
      for (iy = y; iy < ymax; iy++)
      {
        buffer[SCRW * iy + ix] = blendColor(buffer[SCRW * iy + ix], color565, alpha);
      }
    }
    return;
  }
}

int32 bitmap565getInfo(BITMAP_565 *bmp, BITMAPINFO *info)
{
  mrc_memset(info, 0, sizeof(BITMAPINFO));
  info->width = bmp->width;
  info->height = bmp->height;
  info->format = bmp->color_bit;
  info->ptr = bmp->bitmap;
  return 0;
}
// ����ͼƬΪbma��ʽ
void saveBitmap(BITMAP_565 *bmp, const char *filename)
{
  int32 f;
  f = mrc_open(filename, MR_FILE_CREATE | MR_FILE_WRONLY);
  mrc_write(f, "MAP5", 4);
  mrc_write(f, (void *)&bmp->width, 4);
  mrc_write(f, (void *)&bmp->height, 4);
  mrc_write(f, (void *)((bmp->mode == BM_COPY) ? 0 : bmp->transcolor), 2);
  mrc_write(f, bmp->bitmap, bmp->width * bmp->height * 2);
  mrc_close(f);
}
