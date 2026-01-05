
#include <mrc_base.h>
#include "mrc_android.h"
#include "mrc_graphics.h"
#include "xl_debug.h"
#include "mpc.h"
#include <math.h>
#include <armdsp.h>

/*
实现一些绘图函数
主要是实现bmp图片绘制

风的影子

*/
// 将rgb888转rgb565
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

// 通过屏幕缓存创建BITMAP
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
  //  debug_printf("开始读取文件");

  buf = mrc_readFileFromAssets(filename, &len);
  if (buf == NULL)
    return NULL;
  head = (char *)buf;
  if (len > 0)
  {
    //  debug_printf("解析图片");
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
  //  debug_printf("获取文件长度");
  len = mrc_getLen(filename);

  if (len > 0)
  {
    //  debug_printf("open");
    f = mrc_open(filename, 1);
    //  debug_log("malloc %d", len);
    buf = mr_malloc(len);
    if (buf == NULL)
    {
      mrc_printf("申请内存失败");
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
    //  debug_printf("释放内存");
    mrc_freeFileData(buf, len);
  }
  return re;
}

int32 drawBitmapFlip(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
{
  if(bmp->color_bit == 16){
    return gl_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)w,(uint16)h, (uint16)bmp->mode, (int16)sx,(int16)sy,(uint16)bmp->transcolor);
  }else if(bmp->color_bit == 32){
    return gl_bitmapShowExTrans8888((uint32*)bmp->bitmap, (int16)x, (int16)y, (int16)bmp->width, (int16)w, (int16)h, (int16)sx, (int16)sy);
  }
  
  return MR_FAILED;
}

void drawBitmap(BITMAP_565 *bmp, int32 x, int32 y)
{
  // 判断位数
  if (bmp->color_bit == 16)
  {
    drawBitmap565(bmp, x, y);
  }
  else if (bmp->color_bit == 32)
  {
    drawBitmap8888(bmp, x, y);
  }
}
void drawBitmapRegion(BITMAP_565 *src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor) {
  int16 sourceX, sourceY;
  uint16 *screenBuffer;
  uint16 pixelColor;
  int16 startX, startY, endX, endY;
  int16 i,j;
    if (src == NULL || src->bitmap == NULL || width <= 0 || height <= 0 ||
        x_src < 0 || y_src < 0 || x_src + width > src->width || y_src + height > src->height) {
        return; // 无效参数处理
    }

    screenBuffer = w_getScreenBuffer(); // 获取屏幕缓冲

    // 根据锚点计算目标绘制区域的起始位置
    startX = x_dest;
    startY = y_dest;
    if (anchor == GRAPHICS_RIGHT) {
        startX = x_dest - width;
    } else if (anchor == GRAPHICS_BOTTOM) {
        startY = y_dest - height;
    } else if (anchor == GRAPHICS_CENTER) {
        startX = x_dest - width / 2;
        startY = y_dest - height / 2;
    }

    // 计算绘制区域的实际边界
    endX = (startX + width > SCRW) ? SCRW : startX + width;
    endY = (startY + height > SCRH) ? SCRH : startY + height;
    startX = (startX < 0) ? 0 : startX;
    startY = (startY < 0) ? 0 : startY;

    // 计算源图像在绘制区域中的对应位置
     sourceX = x_src;
     sourceY = y_src;

    // 根据绘制模式执行绘制
    if (src->mode == BM_COPY) {
        for ( i = startY; i < endY; i++) {
            for ( j = startX; j < endX; j++) {
                pixelColor = src->bitmap[((sourceY + (i - startY)) * src->width) + (sourceX + (j - startX))];
                screenBuffer[i * SCRW + j] = pixelColor; // 直接绘制
            }
        }
    } else if (src->mode == BM_TRANSPARENT) {
        for ( i = startY; i < endY; i++) {
            for ( j = startX; j < endX; j++) {
                pixelColor = src->bitmap[((sourceY + (i - startY)) * src->width) + (sourceX + (j - startX))];
                if (pixelColor != src->transcolor) { // 判断是否为透明色
                    screenBuffer[i * SCRW + j] = pixelColor; // 绘制非透明像素
                }
            }
        }
    }
}
/*
    将p缓冲中的图片(rgb565格式)，从缓冲中的图片的(sx, sy)
    开始的宽高为w, h的区域，绘制到(x, y)开始的屏幕缓冲中。

输入:
i                  图片缓冲序号
x,y              屏幕位置
rop              选择如下：
   BM_COPY,       //DST = SRC*      覆盖
   BM_TRANSPARENT,//透明色不显示

sx,sy              源图片的起始位置
w,h             欲加载图片的宽高
transcolor 透明色

返回:
      MR_SUCCESS     成功
      MR_FAILED         失败
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
  // 定义局部变量
  int16 i, j;                                 // 用于循环迭代
  uint16 *screenBuffer = w_getScreenBuffer(); // 获取屏幕缓冲区
  uint16 pixelColor;                          // 当前处理的像素颜色

  // 计算绘制区域实际边界
  int16 startX = (x < 0) ? 0 : x;             // 截取开始绘制的X边界
  int16 startY = (y < 0) ? 0 : y;             // 截取开始绘制的Y边界
  int16 endX = (x + w > SCRW) ? SCRW : x + w; // 截取结束绘制的X边界
  int16 endY = (y + h > SCRH) ? SCRH : y + h; // 截取结束绘制的Y边界

  

  // 计算源图片在绘制区域中的对应位置
  int16 sourceX = (startX - x) + sx; // 映射源图像X坐标
  int16 sourceY = (startY - y) + sy; // 映射源图像Y坐标
  int32 sindex, pindex;
  
  // 如果绘制区域完全在屏幕外，直接返回
  if (startX >= endX || startY >= endY) {
    return MR_SUCCESS;
  }

  // 检查输入参数的有效性
  if (p == NULL || screenBuffer == NULL || w <= 0 || h <= 0 ||
      sx < 0 || sy < 0 || sx + w > mw)
  {
    return MR_FAILED; // 参数无效，返回失败
  }
  
  switch (rop)
  {
  case BM_COPY:
    for (i = startY; i < endY; i++)
    {
      // 计算源图像的有效位移 - 修正：sourceX应该基于当前行和偏移计算
      pindex = (sourceY * mw + sourceX);
      // 计算目标屏幕中的位置
      sindex = (i * SCRW + startX);
      // 复制一行像素
      memcpy(screenBuffer + sindex, p + pindex, (endX - startX) * 2);
      sourceY++; // 移动到源图像的下一个行
    }
    break;
  case BM_TRANSPARENT:
    // 遍历要绘制的区域
    for (i = startY; i < endY; i++)
    {
      // 计算当前行的起始索引
      pindex = (sourceY * mw + sourceX);
      sindex = (i * SCRW + startX);
      for (j = startX; j < endX; j++)
      {
        pixelColor = p[pindex]; // 获取当前像素颜色

        // 判断是否为透明色
        if (pixelColor != transcolor)
        {
          screenBuffer[sindex] = pixelColor;
        }
        sindex++;
        pindex++;
      }
      sourceY++; // 移动到源图像的下一个行
    }
    break;
  default:
    return MR_FAILED; // 无效的绘制模式
  }

  return MR_SUCCESS; // 成功完成绘制
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
  // 定义局部变量
  int16 i, j;                                 // 用于循环迭代
  uint16 *screenBuffer = w_getScreenBuffer(); // 获取屏幕缓冲区
  uint16 pixelColor;                          // 当前处理的像素颜色
  uint32 pixel;                               // 当前处理的32位像素
  uint32 alpha;

  // 计算绘制区域实际边界
  int16 startX = (x < 0) ? 0 : x;             // 截取开始绘制的X边界
  int16 startY = (y < 0) ? 0 : y;             // 截取开始绘制的Y边界
  int16 endX = (x + w > SCRW) ? SCRW : x + w; // 截取结束绘制的X边界
  int16 endY = (y + h > SCRH) ? SCRH : y + h; // 截取结束绘制的Y边界

  

  // 计算源图片在绘制区域中的对应位置
  int16 sourceX = (startX - x) + sx; // 映射源图像X坐标
  int16 sourceY = (startY - y) + sy; // 映射源图像Y坐标

  // 如果绘制区域完全在屏幕外，直接返回
  if (startX >= endX || startY >= endY) {
    return MR_SUCCESS;
  }
  
  // 检查输入参数的有效性
  if (p == NULL || screenBuffer == NULL || w <= 0 || h <= 0 ||
      sx < 0 || sy < 0 || sx + w > mw)
  {
    return MR_FAILED; // 参数无效，返回失败
  }
  
  // 遍历要绘制的区域
  for (i = startY; i < endY; i++)
  {
    // 重置当前行的源图像X坐标
    int16 currentSourceX = sourceX;
    for (j = startX; j < endX; j++)
    {
      pixel = p[(sourceY * mw + currentSourceX)]; // 从图像缓冲区获取当前32位像素
      alpha = pixel >> 24;
      if (alpha != 0) // 判断是否为透明色
      {
        pixelColor = blendColor888(screenBuffer[i * SCRW + j], pixel);
        screenBuffer[i * SCRW + j] = pixelColor; // 写入屏幕缓冲区
      }
      currentSourceX++; // 移动到源图像的下一个像素
    }
    sourceY++; // 移动到源图像的下一个行
  }

  return MR_SUCCESS; // 成功完成绘制
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
  //     // 屏幕点位置 图片点位置 复制长度
  //     mrc_memcpy(buffer + screenIndex + bmp->memcache[i >> 2], bmp->bitmap + bmp->memcache[(i >> 2) + 1], bmp->memcache[(i >> 2) + 2]);
  //   }
  // }
  // else
  // {
  gl_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)bmp->width, (uint16)bmp->height, (uint16)bmp->mode, 0, 0, (uint16)bmp->transcolor);
  // }

  // bmp_draw(bmp, x, y);
}

// 自动分析memcpy优化
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
    // 如果mem_count数量小于图片面积/10,则进行优化
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

// 透明度,取值0到255
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

  // 限制 alpha 的范围在 0 到 255 之间
  if (alpha < 0)
    alpha = 0;
  if (alpha > 255)
    alpha = 255;

  if (b == NULL)
    return;

  // 计算有效绘制区域
  minX = MAX(0, x);
  minY = MAX(0, y);
  maxX = MIN(SCRW, x + b->width);
  maxY = MIN(SCRH, y + b->height);

  // 决定遍历的起始和结束坐标
  startX = MIN(x, 0) + b->width + x;
  startY = MIN(y, 0) + b->height + y;

  if (maxX <= minX || maxY <= minY)
    return; // 没有有效的绘制区域

  if (b->color_bit == 16)
  {
    // 遍历位图的每个像素
    if (b->mode == BM_TRANSPARENT)
    {
      for (j = minY; j < maxY; j++)
      {
        for (i = minX; i < maxX; i++)
        {
          // 计算当前像素的位图的索引
          color = b->bitmap[(j - y) * b->width + (i - x)];

          // 如果是透明色，则跳过该像素
          if (color == b->transcolor)
          {
            continue;
          }

          // 计算目标点的位置
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
          // 计算当前像素的位图的索引
          color = b->bitmap[(j - y) * b->width + (i - x)];

          // 计算目标点的位置
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
        // 计算当前像素的位图的索引
        color32 = buf32[(j - y) * b->width + (i - x)];
        if ((color32 >> 24) != 0)
        {
          alpha32 = ((color32 >> 24) * alpha) >> 8;
          // 计算目标点的位置
          targetX = i;
          targetY = j;
          buffer[SCRW * targetY + targetX] = blendColor888(buffer[SCRW * targetY + targetX], (color32 & 0xffffff) | (alpha32 << 24));
        }
      }
    }
  }
}

// 辅助函数：用于翻转图像数据
void flipBitmap(uint16 *bitmap, int width, int height, int flip)
{
  uint16 temp;
  int x, y;
  if (flip == 1)
  { // 横向翻转
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
  { // 纵向翻转
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

// 将bitmap旋转90度/180度/270度,旋转之后修改bitmap内bitmap缓存数据
// type取值 1:90 2:180 3:270
// flip取值 1横向翻转 2纵向翻转
void bitmap565Rotate(BITMAP_565 *b, int type, int flip)
{
  int newWidth, newHeight;
  uint16 *newBitmap;
  int x, y;
  if (type < 1 || type > 3)
  {
    return; // 无效的旋转类型
  }

  switch (type)
  {
  case 1: // 旋转90度
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

  case 2: // 旋转180度
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

  case 3: // 旋转270度
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
    return; // 不应到达此行
  }

  // 处理翻转
  if (flip == 1 || flip == 2)
  {
    flipBitmap(newBitmap, newWidth, newHeight, flip);
  }

  // 更新原始位图数据
  mrc_freeFileData(b->bitmap, b->buflen); // 释放原 bitmap 的内存

  b->bitmap = newBitmap;
  b->width = newWidth;
  b->height = newHeight;
  b->buflen = newWidth * newHeight * sizeof(uint16);
}
/*
图片旋转缩放绘制
rop [IN] BM_COPY, //DST = SRC* 覆盖

BM_TRANSPARENT, //透明色不 显示，图片(0，0)象素是透明色

用增强的方式将bmp图片绘制于指定图片中。
将srcbmp 的bmp缓冲中的图片，从缓冲中的图片的(sx, sy)开
始的宽高为w, h的区域，绘制到dstbmp从(dx,dy)开始的bmp
缓冲中。

模式rop选择如下：
   BM_COPY,               //DST = SRC*      覆盖
   BM_TRANSPARENT,  //透明色不显示

A、B、C、D用于图像变化，用于该变化的变换矩阵为：
x = A0*x0 + B0*y0
y = C0*x0 + D0*y0
这里为了表示小数，A, B, C, D均被乘以了256，即：
A = A0*256
B = B0*256
C = C0*256
D = D0*256
根据变换公式，可以绘出不同效果的图像，比如：
旋转图像：
A = 256 * cos（角度）
B = 256 * sin（角度）
C = 256 * -sin（角度）
D = 256 * cos（角度）
放大、缩小图像：
A = 256 * 放大倍数
B = 0
C = 0
D = 256 * 放大倍数

返回:
      MR_SUCCESS     成功
      MR_FAILED         失败
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
将bitmap旋转绘制到屏幕上
scrx scry 绘制到屏幕上的中心位置
bx by 图片旋转中心

*/
void drawBitmapRotate(BITMAP_565 *b, int32 centerX, int32 centerY, int32 bx, int32 by, int32 r)
{
  int32 x, y;
  int32 newX, newY;
  int32 x1, y1, x2, y2;
  // uint16 color11, color12, color21, color22;
  // double dx, dy;
  uint16 color;
  uint32 color32;
  float radian = r * M_PI / 180.0; // 将角度转换为弧度
  float cosR = cos(radian);
  float sinR = sin(radian);
  int32 minX = SCRW;
  int32 maxX = 0;
  int32 minY = SCRH;
  int32 maxY = 0;
  int i;
  // 计算矩形的四个顶点
  int16 vertices[4][2]; // 先声明不初始化
  uint16 *screenBuffer = w_getScreenBuffer();
  uint32 *bitmap32;

  // 计算对角线的长度
  // int32 diagonal = (int)sqrt(b->width * b->width + b->height * b->height);

  // 使用计算的常量值填充 vertices
  vertices[0][0] = centerX - bx; // 左上角
  vertices[0][1] = centerY - by;

  vertices[1][0] = centerX + b->width - bx; // 右上角
  vertices[1][1] = centerY - (b->height - by);

  vertices[2][0] = centerX + (b->width - bx); // 右下角
  vertices[2][1] = centerY + (b->height - by);

  vertices[3][0] = centerX - bx; // 左下角
  vertices[3][1] = centerY + (b->height - by);

  // 旋转每个顶点
  for (i = 0; i < 4; i++)
  {
    x = vertices[i][0] - centerX;
    y = vertices[i][1] - centerY;
    vertices[i][0] = centerX + x * cosR - y * sinR;
    vertices[i][1] = centerY + x * sinR + y * cosR;
  }

  for (i = 0; i < 4; i++)
  {
    // 更新最小和最大 x 值
    if (vertices[i][0] < minX)
    {
      minX = vertices[i][0];
    }
    if (vertices[i][0] > maxX)
    {
      maxX = vertices[i][0];
    }

    // 更新最小和最大 y 值
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
  // 遍历屏幕的每一个像素
  if(b->color_bit == 16){
    if (b->mode == BM_TRANSPARENT)
  {
    for (y = minY; y < maxY; y++)
    {
      for (x = minX; x < maxX; x++)
      {
        // 计算旋转后的坐标
        newX = (int32)(bx + (x - centerX) * cosR + (y - centerY) * sinR);
        newY = (int32)(by - (x - centerX) * sinR + (y - centerY) * cosR);

        // 检查坐标是否在图片范围内
        if (newX >= 0 && newX < b->width && newY >= 0 && newY < b->height)
        {
          // 使用双线性插值计算颜色值
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
          // 将颜色写入屏幕缓冲区
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
        // 计算旋转后的坐标
        newX = (int32)(bx + (x - centerX) * cosR + (y - centerY) * sinR);
        newY = (int32)(by - (x - centerX) * sinR + (y - centerY) * cosR);

        // 检查坐标是否在图片范围内
        if (newX >= 0 && newX < b->width && newY >= 0 && newY < b->height)
        {
          // 使用双线性插值计算颜色值
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
          // 将颜色写入屏幕缓冲区
          screenBuffer[y * SCRW + x] = color;
        }
      }
    }
  }
  
  }else if(b->color_bit == 32){
    bitmap32 = (uint32*)b->bitmap;
     for (y = minY; y < maxY; y++)
    {
      for (x = minX; x < maxX; x++)
      {
        // 计算旋转后的坐标
        newX = (int32)(bx + (x - centerX) * cosR + (y - centerY) * sinR);
        newY = (int32)(by - (x - centerX) * sinR + (y - centerY) * cosR);

        // 检查坐标是否在图片范围内
        if (newX >= 0 && newX < b->width && newY >= 0 && newY < b->height)
        {
          // 使用双线性插值计算颜色值
          x1 = newX;
          y1 = newY;
          
          color32 = bitmap32[y1 * b->width + x1];
          // 将颜色写入屏幕缓冲区
          screenBuffer[y * SCRW + x] = blendColor888(screenBuffer[y * SCRW + x], color32);
        }
      }
    }

  }
  
  
  // 绘制旋转后的矩形
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
    // 计算旋转角度的弧度值
    double radian = r * M_PI / 180.0; // 将角度转换为弧度
    double cosR = cos(radian);
    double sinR = sin(radian);

    // 遍历图片的每一个像素
    for (y = 0; y < b->height; y++) {
        for (x = 0; x < b->width; x++) {
            // 原始图像的颜色值
            uint16 color = b->bitmap[y * b->width + x];

            // 如果当前像素是透明色，则跳过
            if (color == b->transcolor) {
                continue;
            }

            // 计算旋转后的坐标
            newX = (int32)(bx + (x - bx) * cosR - (y - by) * sinR);
            newY = (int32)(by + (x - bx) * sinR + (y - by) * cosR);

            // 计算屏幕上的位置
             screenX = scrx + newX;
             screenY = scry + newY;

            // 检查坐标是否在屏幕范围内
            if (screenX >= 0 && screenX < SCRW && screenY >= 0 && screenY < SCRH) {
                // 将颜色写入屏幕缓冲区
                screenBuffer[screenY * SCRW + screenX] = color;
            }
        }
    }
}
*/

// 绘制旋转矩形
// bx by 相对于矩形的旋转中心
void gl_drawRotatedHollowRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint32 color)
{
  int16 x1, y1, x2, y2;
  float radian = angle * M_PI / 180.0; // 将角度转换为弧度
  float cosR = cos(radian);
  float sinR = sin(radian);
  int16 x, y;
  int i;
  // 计算矩形的四个顶点
  int16 vertices[4][2]; // 先声明不初始化

  // 使用计算的常量值填充 vertices
  vertices[0][0] = centerX - bx; // 左上角
  vertices[0][1] = centerY - by;

  vertices[1][0] = centerX + width - bx; // 右上角
  vertices[1][1] = centerY - by;

  vertices[2][0] = centerX + (width - bx); // 右下角
  vertices[2][1] = centerY + (height - by);

  vertices[3][0] = centerX - bx; // 左下角
  vertices[3][1] = centerY + (height - by);

  // 旋转每个顶点
  for (i = 0; i < 4; i++)
  {
    x = vertices[i][0] - centerX;
    y = vertices[i][1] - centerY;
    vertices[i][0] = centerX + x * cosR - y * sinR;
    vertices[i][1] = centerY + x * sinR + y * cosR;
  }

  // 绘制旋转后的矩形
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
  float radian = angle * M_PI / 180.0; // 将角度转换为弧度
  float cosR = cos(radian);
  float sinR = sin(radian);
  int16 x, y;
  int i;
  // 计算矩形的四个顶点
  int16 vertices[4][2]; // 先声明不初始化

  // 使用计算的常量值填充 vertices
  vertices[0][0] = centerX - bx; // 左上角
  vertices[0][1] = centerY - by;

  vertices[1][0] = centerX + width - bx; // 右上角
  vertices[1][1] = centerY - by;

  vertices[2][0] = centerX + (width - bx); // 右下角
  vertices[2][1] = centerY + (height - by);

  vertices[3][0] = centerX - bx; // 左下角
  vertices[3][1] = centerY + (height - by);

  // 旋转每个顶点
  for (i = 0; i < 4; i++)
  {
    x = vertices[i][0] - centerX;
    y = vertices[i][1] - centerY;
    vertices[i][0] = centerX + x * cosR - y * sinR;
    vertices[i][1] = centerY + x * sinR + y * cosR;
  }

  // 绘制旋转后的矩形
  gl_drawTriangle(vertices[0][0], vertices[0][1], vertices[1][0], vertices[1][1], vertices[2][0], vertices[2][1], color);
  gl_drawTriangle(vertices[0][0], vertices[0][1], vertices[3][0], vertices[3][1], vertices[2][0], vertices[2][1], color);
}

/*
将bitmap绘制到另一个bitmap上
将buf中的bitmap图片, 从sx,sy开始的宽高为w,h的区域,绘制到图片di从x,y开始的地方
*/
void drawBitmap565Old(BITMAP_565 *di, BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
{
    int32 j;
    int32 i;

    // 处理目标坐标超出边界的情况
    if (x < 0)
    {
        w += x; // 对宽度进行调整
        sx -= x; // 左移源图片的起始x坐标
        x = 0; // 设置目标x为0
    }
    if (y < 0)
    {
        h += y; // 对高度进行调整
        sy -= y; // 上移源图片的起始y坐标
        y = 0; // 设置目标y为0
    }
    if (x + w > di->width)
    {
        w = di->width - x; // 确保宽度不超出目标位图
    }
    if (y + h > di->height)
    {
        h = di->height - y; // 确保高度不超出目标位图
    }

    // 处理源图片超出边界的情况
    if (sx < 0)
    {
        w += sx; // 对宽度进行调整
        sx = 0; // 设置源图片的起始坐标为0
    }
    if (sy < 0)
    {
        h += sy; // 对高度进行调整
        sy = 0; // 设置源图片的起始坐标为0
    }
    if (sx + w > buf->width)
    {
        w = buf->width - sx; // 确保宽度不超出源位图
    }
    if (sy + h > buf->height)
    {
        h = buf->height - sy; // 确保高度不超出源位图
    }

    // 进行像素拷贝
    for (j = 0; j < h; ++j)
    {
        if (buf->mode == BM_TRANSPARENT)
        {
            for (i = 0; i < w; ++i)
            {
                int32 src_x = sx + i;
                int32 src_y = sy + j;
                uint16 color = buf->bitmap[src_y * buf->width + src_x]; // 从buf中获取颜色

                // 仅当颜色不等于透明色时，才进行绘制
                if (color != buf->transcolor)
                {
                    int32 dest_x = x + i;
                    int32 dest_y = y + j;
                    di->bitmap[dest_y * di->width + dest_x] = color; // 绘制到目标位图
                }
            }
        }
        else
        {
            // 如果不需要透明处理，直接使用内存复制
            mrc_memcpy(di->bitmap + (y + j) * di->width + x, 
                        buf->bitmap + (sy + j) * buf->width + sx, 
                        w * 2);
        }
    }
}

/*
将bitmap缩放,生成一个新的BITMAP
width 生成的图片宽度
height 生成的图片高度
*/
BITMAP_565 *createBitmapFromBitmap(BITMAP_565 *bmp, int32 width, int32 height)
{
  int32 x_ratio;
  int32 y_ratio;
  int32 src_x, src_y;
  int32 x, y;
  uint32 *bitmap32;
  uint32 *bitmap32_new;
  // 变量声明
  BITMAP_565 *new_bmp = (BITMAP_565 *)malloc(sizeof(BITMAP_565)); // 分配内存用于新位图
  memset(new_bmp, 0, sizeof(BITMAP_565));
  if (!new_bmp)
  {
    return NULL; // 分配失败
  }

  new_bmp->width = width;
  new_bmp->height = height;
  new_bmp->color_bit = bmp->color_bit;   // 默认颜色位数
  new_bmp->transcolor = bmp->transcolor; // 默认透明色
  new_bmp->mode = bmp->mode;             // 默认模式（这里用0代替BM_COPY）

  // 分配内存用于新位图数据
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
    free(new_bmp); // 释放已分配的内存
    mrc_printf("Failed to allocate memory for new bitmap\n");
    return NULL; // 分配失败
  }

  // 进行缩放（简单的最近邻插值）
  x_ratio = (bmp->width << 16) / width + 1;   // 使用固定点数进行计算
  y_ratio = (bmp->height << 16) / height + 1; // 使用固定点数进行计算

  for (y = 0; y < height; y++)
  {
    for (x = 0; x < width; x++)
    {
      src_x = (x * x_ratio) >> 16; // 使用位移运算获取源图x坐标
      src_y = (y * y_ratio) >> 16; // 使用位移运算获取源图y坐标
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

  return new_bmp; // 返回新生成的位图
}
/*
将bitmap上(tx,ty,tw,th)区域缩放绘制到屏幕(x,y,w,h)区域上
参数：
x 绘制到屏幕上的x坐标
y 绘制到屏幕上的y坐标
w 绘制宽度 h 绘制高度 tx ty tw th 裁剪区域
*/
void drawBitmapEx(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th)
{
  int px, py; // 屏幕区域坐标(相对)
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
        // 根据屏幕坐标计算出图片上的点
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
        // 根据屏幕坐标计算出图片上的点
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
      // 根据屏幕坐标计算出图片上的点
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

// 获取两点之间的长度 的平方
int gl_getLineSize(int x, int y, int x2, int y2)
{
  return (x2 - x) * (x2 - x) + (y2 - y) * (y2 - y);
}

// 混合两个颜色
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
// 混合两个rgb565颜色 返回新的颜色
// dstColor 颜色1
// srcColor 颜色2
// alpha: 5位透明度 (0-31) 或者 6位透明度 (0-63)
uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha)
{
    uint8 a = (uint8)alpha;
    // 提取目标颜色分量
    uint8 r1 = (dstColor >> 11) & 0x1F; // 5位
    uint8 g1 = (dstColor >> 5) & 0x3F;   // 6位
    uint8 b1 = dstColor & 0x1F;         // 5位
    
    // 提取源颜色分量
    uint8 r2 = (srcColor >> 11) & 0x1F; // 5位
    uint8 g2 = (srcColor >> 5) & 0x3F;   // 6位
    uint8 b2 = srcColor & 0x1F;         // 5位
    
    // 方法1：使用6位alpha (0-63)
    // 对于5位分量，需要调整alpha的权重
    uint8 inv_alpha = 63 - a; // 6位alpha
    
    // 混合计算 - 使用6位alpha
    uint8 r = ((r1 * inv_alpha) + (r2 * a)) >> 6;
    uint8 g = ((g1 * inv_alpha) + (g2 * a)) >> 6;
    uint8 b = ((b1 * inv_alpha) + (b2 * a)) >> 6;
    
    // 方法2：使用5位alpha (0-31) - 绿色分量需要特殊处理
    // uint8_t inv_alpha5 = 31 - alpha; // 5位alpha
    // uint8_t r = ((r1 * inv_alpha5) + (r2 * alpha)) >> 5;
    // uint8_t g = ((g1 * inv_alpha5 * 2) + (g2 * alpha * 2)) >> 6; // 绿色需要6位精度
    // uint8_t b = ((b1 * inv_alpha5) + (b2 * alpha)) >> 5;
    
    // 重新组合为16位RGB565格式
    return (r << 11) | (g << 5) | b;
}
/*
uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha)
{
  uint8 r1 = (dstColor >> 11) & 0x1F; // 提取目标颜色的红色分量
  uint8 g1 = (dstColor >> 5) & 0x3F;  // 提取目标颜色的绿色分量
  uint8 b1 = dstColor & 0x1F;         // 提取目标颜色的蓝色分量

  uint8 r2 = (srcColor >> 11) & 0x1F; // 提取源颜色的红色分量
  uint8 g2 = (srcColor >> 5) & 0x3F;  // 提取源颜色的绿色分量
  uint8 b2 = srcColor & 0x1F;         // 提取源颜色的蓝色分量

  // 进行 alpha 混合
  // uint8 r = (r1 * (255 - alpha) + r2 * alpha) / 255;
  // uint8 g = (g1 * (255 - alpha) + g2 * alpha) / 255;
  // uint8 b = (b1 * (255 - alpha) + b2 * alpha) / 255;
  int inv_alpha = 255 - alpha;
  uint8 r = (r1 * (inv_alpha) + r2 * alpha) >> 8;
  uint8 g = (g1 * (inv_alpha) + g2 * alpha) >> 8;
  uint8 b = (b1 * (inv_alpha) + b2 * alpha) >> 8;

  // 重新组合为 16 位 RGB 565 格式
  return (r << 11) | (g << 5) | b;
}
*/
#else
// 混合两个rgb565颜色 返回新的颜色
// datColor 颜色1
// srcColor 颜色2
// srcColor的透明度
uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha)
{
  uint8 r1 = (dstColor >> 11) & 0x1F; // 提取目标颜色的红色分量
  uint8 g1 = (dstColor >> 5) & 0x3F;  // 提取目标颜色的绿色分量
  uint8 b1 = dstColor & 0x1F;         // 提取目标颜色的蓝色分量

  uint8 r2 = (srcColor >> 11) & 0x1F; // 提取源颜色的红色分量
  uint8 g2 = (srcColor >> 5) & 0x3F;  // 提取源颜色的绿色分量
  uint8 b2 = srcColor & 0x1F;         // 提取源颜色的蓝色分量

  // 计算混合的红色分量
  Word32 tempR = smlabb(r1, (Word32)(r2 * alpha), (Word32)(r1 * (255 - alpha)));
  uint8 r = tempR >> 8; // 由于使用了128位的加法，所以需要右移8位

  // 计算混合的绿色分量
  Word32 tempG = smlabb(g1, (Word32)(g2 * alpha), (Word32)(g1 * (255 - alpha)));
  uint8 g = tempG >> 8; // 右移8位

  // 计算混合的蓝色分量
  Word32 tempB = smlabb(b1, (Word32)(b2 * alpha), (Word32)(b1 * (255 - alpha)));
  uint8 b = tempB >> 8; // 右移8位

  // 重新组合为16位 RGB 565格式
  return (r << 11) | (g << 5) | b;
}
#endif

#ifndef USE_DSP
// 混合两个rgb565 rgb8888颜色 返回新的颜色
// datColor 颜色1
// srcColor 颜色2
uint16 blendColor888(uint16 dstColor, uint32 srcColor)
{
  uint8 r1 = (dstColor >> 11) & 0x1F; // 提取目标颜色的红色分量
  uint8 g1 = (dstColor >> 5) & 0x3F;  // 提取目标颜色的绿色分量
  uint8 b1 = dstColor & 0x1F;         // 提取目标颜色的蓝色分量

  uint8 r2 = (srcColor >> 19) & 0x1F; // 提取源颜色的红色分量
  uint8 g2 = (srcColor >> 10) & 0x3F; // 提取源颜色的绿色分量
  uint8 b2 = (srcColor >> 3) & 0x1F;  // 提取源颜色的蓝色分量
  int32 alpha = srcColor >> 24;

  // 进行 alpha 混合
  uint8 r = (r1 * (255 - alpha) + r2 * alpha) >> 8;
  uint8 g = (g1 * (255 - alpha) + g2 * alpha) >> 8;
  uint8 b = (b1 * (255 - alpha) + b2 * alpha) >> 8;

  // 重新组合为 16 位 RGB 565 格式
  return (r << 11) | (g << 5) | b;
}
#else

uint16 blendColor888(uint16 dstColor, uint32 srcColor)
{
  uint8 r1 = (dstColor >> 11) & 0x1F; // 提取目标颜色的红色分量
  uint8 g1 = (dstColor >> 5) & 0x3F;  // 提取目标颜色的绿色分量
  uint8 b1 = dstColor & 0x1F;         // 提取目标颜色的蓝色分量

  uint8 r2 = (srcColor >> 19) & 0x1F; // 提取源颜色的红色分量
  uint8 g2 = (srcColor >> 10) & 0x3F; // 提取源颜色的绿色分量
  uint8 b2 = (srcColor >> 3) & 0x1F;  // 提取源颜色的蓝色分量
  int32 alpha = srcColor >> 24;

  // 进行 alpha 混合使用 armdsp.h 提供的函数
  // 在这里，255 是为了避免溢出，在进行该项计算的时候并不需要遇到负数
  uint32 r = smlabb(r1, 255 - alpha, r2 * alpha);
  uint32 g = smlabb(g1, 255 - alpha, g2 * alpha);
  uint32 b = smlabb(b1, 255 - alpha, b2 * alpha);

  // 结果需要先右移以缩放到正确范围
  r = (r >> 8) & 0x1F; // 保证结果范围在 0-31
  g = (g >> 8) & 0x3F; // 保证结果范围在 0-63
  b = (b >> 8) & 0x1F; // 保证结果范围在 0-31

  // 重新组合为 16 位 RGB 565 格式
  return (r << 11) | (g << 5) | b;
}
#endif

// 画点 参数：x, y, argb
void gl_drawPoint(int x, int y, uint32 color)
{
  uint16 *scr_color;
  uint16 *buffer;
  // int32 scr_color = 0;
  // int32 o_color = 0;
  // uint16 scr_color16 = 0;
  // // 获取屏幕颜色
  // if (x < 0 || x >= SCRW)
  //   return;
  // if (y < 0 || y >= SCRH)
  //   return;
  // scr_color16 = *(getscrbuf() + (SCRW * y + x));
  // scr_color = ((scr_color16 << 8) & 0xf80000) | ((scr_color16 << 5) & 0xfc00) | (scr_color16 << 3) & 0xff;
  // // 混合
  // o_color = gl_getColor(scr_color, color);
  if (x < 0 || y < 0 || x >= SCRW || y >= SCRH)
    return;
  buffer = w_getScreenBuffer();
  scr_color = buffer + SCRW * y + x;

  *scr_color = blendColor888(*scr_color, color);
  // 生成rgb565
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

// 画矩形
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

//绘制空心矩形
void gl_drawHollowRect(int x, int y, int width, int height, uint32 color){
  // int ix, iy;
  // uint16 *buffer = w_getScreenBuffer();
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
  uint16 upcolor = 0;
  uint16 upsrccolor = 0;
  color565 = MAKECOLOR565(color);
  alpha = color >> 24;
  upcolor = blendColor888(upsrccolor, color);
  // 画三条边
  // bmp_drawLine(bmp, x1, y1, x2, y2, color);
  // bmp_drawLine(bmp, x2, y2, x3, y3, color);
  // bmp_drawLine(bmp, x3, y3, x1, y1, color);

  // 填充三角形
  // 使用边界填充算法
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
      // 检查点是否在三角形内部
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
    // gl_drawPoint(x1, y1, color); // 绘制当前点
    if (x1 >= 0 && y1 >= 0 && x1 < SCRW && y1 < SCRH)
    {
      *(buffer + SCRW * y1 + x1) = blendColor888(*(buffer + SCRW * y1 + x1), color);
    }

    if (x1 == x2 && y1 == y2)
      break; // 如果到达终点，跳出循环
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
  // 仅画三条边
  gl_drawLine(x1, y1, x2, y2, color);
  gl_drawLine(x2, y2, x3, y3, color);
  gl_drawLine(x3, y3, x1, y1, color);
}

// 绘制填充多边形
// points: 顶点数组，格式为[x0, y0, x1, y1, x2, y2, ...]
// count: 顶点数量
// color: 颜色值(ARGB格式)
void gl_drawPolygon(int *points, int count, uint32 color)
{
  int i;

  if (points == NULL || count < 3)
  {
    return; // 多边形至少需要3个顶点
  }

  // 使用扇形三角剖分算法，将多边形分解为多个三角形
  // 以第一个顶点为中心，依次连接相邻顶点形成三角形
  for (i = 1; i < count - 1; i++)
  {
    gl_drawTriangle(
      points[0], points[1],           // 第一个顶点
      points[i * 2], points[i * 2 + 1],     // 第i个顶点
      points[(i + 1) * 2], points[(i + 1) * 2 + 1], // 第i+1个顶点
      color
    );
  }
}

// 绘制空心多边形
// points: 顶点数组，格式为[x0, y0, x1, y1, x2, y2, ...]
// count: 顶点数量
// color: 颜色值(ARGB格式)
void gl_drawHollowPolygon(int *points, int count, uint32 color)
{
  int i;

  if (points == NULL || count < 3)
  {
    return; // 多边形至少需要3个顶点
  }

  // 绘制多边形的所有边
  for (i = 0; i < count; i++)
  {
    int next = (i + 1) % count; // 下一个顶点的索引（循环到第一个）
    gl_drawLine(
      points[i * 2], points[i * 2 + 1],           // 当前顶点
      points[next * 2], points[next * 2 + 1],     // 下一个顶点
      color
    );
  }
}

// 在bitmap内绘制矩形
void bmp_drawRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color)
{
  int ix, iy;
  // 确保坐标在有效范围内
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
    return; // 检查指针有效性
  }
  // 循环遍历矩形的每一个像素并进行绘制
  for (ix = startX; ix < endX; ix++)
  {
    for (iy = startY; iy < endY; iy++)
    {
      // 计算在 bitmap 中的位置
      index = iy * bmp->width + ix;

      // 取出当前像素颜色
      dstColor = bmp->bitmap[index];

      // 如果当前颜色是透明色，则跳过
      if (dstColor == bmp->transcolor)
      {
        continue;
      }

      // 混合颜色，并更新到 bitmap 中
      newColor = blendColor(dstColor, (uint16)color565, alpha); // 透明度可以根据需要调整
      bmp->bitmap[index] = newColor;
    }
  }
}

// 在bitmap内绘制点
void bmp_drawPoint(BITMAP_565 *bmp, int32 x, int32 y, uint32 color)
{
  int index;
  uint16 dstColor, color565;
  int alpha;
  if (!bmp || bmp->bitmap == NULL)
  {
    return; // 检查指针有效性
  }

  // 计算在 bitmap 中的位置
  index = y * bmp->width + x;

  // 确保坐标在有效范围内
  if (x >= 0 && x < bmp->width && y >= 0 && y < bmp->height)
  {
    dstColor = bmp->bitmap[index];
    color565 = MAKECOLOR565(color);
    alpha = (color >> 24) & 0xff;

    // 如果当前颜色是透明色，则跳过
    if (dstColor != bmp->transcolor)
    {
      // 混合颜色，并更新到 bitmap 中
      bmp->bitmap[index] = blendColor(dstColor, color565, alpha);
    }
  }
}

// 在bitmap内绘制线段
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
    return; // 检查指针有效性
  }

  while (1)
  {
    bmp_drawPoint(bmp, x1, y1, color); // 绘制当前点

    if (x1 == x2 && y1 == y2)
      break; // 如果到达终点，跳出循环
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

// 在bitmap内绘制圆形
void bmp_drawCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 color)
{

  int16 x, y;
  // uint16 color565 = MAKECOLOR565(color);
  // int alpha = (color >> 24) & 0xff;
  if (!bmp || bmp->bitmap == NULL)
  {
    return; // 检查指针有效性
  }

  for (y = -radius; y <= radius; y++)
  {
    for (x = -radius; x <= radius; x++)
    {
      if (x * x + y * y <= radius * radius)
      {                                            // 判断是否在圆内
        bmp_drawPoint(bmp, cx + x, cy + y, color); // 绘制点
      }
    }
  }
}

// 在bitmap内绘制空心圆
void bmp_drawHollowCircle(BITMAP_565 *bmp, int32 centerX, int32 centerY, int32 radius, uint32 color)
{
  int32 x, y;
  int32 angle;

  if (!bmp || bmp->bitmap == NULL)
  {
    return; // 检查指针有效性
  }

  // uint16 color565 = MAKECOLOR565(color);
  // int alpha = (color >> 24) & 0xff;

  // 计算圆的周围的点
  for (angle = 0; angle < 360; angle++)
  {
    // 使用极坐标计算圆周上的点
    x = (int32)(centerX + radius * cos(angle * M_PI / 180.0));
    y = (int32)(centerY + radius * sin(angle * M_PI / 180.0));

    // 确保点在合法范围内
    if (x >= 0 && x < bmp->width && y >= 0 && y < bmp->height)
    {
      bmp_drawPoint(bmp, x, y, color);
    }
  }
}

#ifndef USE_DSP
// 求一个三角形面积
int tri_area(int x1, int y1, int x2, int y2, int x3, int y3)
{
  // return (1/2)*(x1*y2+x2*y3+x3*y1-x1*y3-x2*y1-x3*y2);
  int m = ((x2 - x1) * (y3 - y2)) - ((x3 - x2) * (y2 - y1));
  return (m > 0) ? m : -m;
}

// 判断点是否在三角形內
int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3)
{
  if (tri_area(x, y, x1, y1, x2, y2) + tri_area(x, y, x1, y1, x3, y3) + tri_area(x, y, x2, y2, x3, y3) == tri_area(x1, y1, x2, y2, x3, y3))
    return TRUE;
  return FALSE;
}
#else

// 求一个三角形面积
int tri_area(int x1, int y1, int x2, int y2, int x3, int y3)
{
  // 使用 DSP 指令优化计算
  // m = (x2 - x1) * (y3 - y2) - (x3 - x2) * (y2 - y1)
  Word32 area1, area2, m;

  area1 = smulbb(x2 - x1, y3 - y2); // (x2-x1)*(y3-y2)
  area2 = smulbb(x3 - x2, y2 - y1); // (x3-x2)*(y2-y1)

  m = qsub(area1, area2); // m = area1 - area2

  return (m > 0) ? m : -m; // 无需改变此行，保持返回正值
}

// 判断点是否在三角形内
int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3)
{
  // 使用 DSP 指令优化面积计算
  Word32 area1 = tri_area(x, y, x1, y1, x2, y2);
  Word32 area2 = tri_area(x, y, x1, y1, x3, y3);
  Word32 area3 = tri_area(x, y, x2, y2, x3, y3);
  Word32 total_area = tri_area(x1, y1, x2, y2, x3, y3);

  return (qadd(area1, qadd(area2, area3)) == total_area) ? TRUE : FALSE; // 使用 qadd 进行加法
}

#endif

// int pointInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3)
// {
//   // 计算重心坐标法
//   float area = 0.5 * (-y2 * x3 + y1 * (-x2 + x3) + x1 * (y2 - y3) + x2 * y3);
//   float s = 1 / (2 * area) * (y1 * x3 - x1 * y3 + (x3 - x1) * py + (x1 - x2) * px);
//   float t = 1 / (2 * area) * (x1 * y2 - y1 * x2 + (x1 - x2) * py + (y1 - y2) * px);
//   return (s > 0) && (t > 0) && (1 - s - t > 0);
// }

void bmp_drawTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
  int minX, maxX, minY, maxY;
  int x, y;
  // 画三条边
  // bmp_drawLine(bmp, x1, y1, x2, y2, color);
  // bmp_drawLine(bmp, x2, y2, x3, y3, color);
  // bmp_drawLine(bmp, x3, y3, x1, y1, color);

  // 填充三角形
  // 使用边界填充算法
  minX = MIN(MIN(x1, x2), x3);
  maxX = MAX(MAX(x1, x2), x3);
  minY = MIN(MIN(y1, y2), y3);
  maxY = MAX(MAX(y1, y2), y3);

  for (y = minY; y < maxY; y++)
  {
    for (x = minX; x < maxX; x++)
    {
      // 检查点是否在三角形内部
      if (pointInTriangle(x, y, x1, y1, x2, y2, x3, y3))
      {
        bmp_drawPoint(bmp, x, y, color);
      }
    }
  }
}

void bmp_drawHollowTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
{
  // 仅画三条边
  bmp_drawLine(bmp, x1, y1, x2, y2, color);
  bmp_drawLine(bmp, x2, y2, x3, y3, color);
  bmp_drawLine(bmp, x3, y3, x1, y1, color);
}

// 画圆
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
        // 考虑效率问题，不透明的圆单独处理

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
   //考虑效率问题，不透明的圆单独处理

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
  // 使用正负法画圆
  int x, y, f; //(x,y)为当前坐标 f为误差值
  int upy;
  x = x0;
  y = y0 + r;
  f = 0;
  upy = -1;

  while (y >= y0)
  { // 从圆周的右上部分开始
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
      y--; // 向圆内走
    }
    else
    {
      f = f + 2 * (x - x0) + 1;
      x++; // 向圆外走
    }
  }

  if (y == y0)
    gl_drawPoint(x, y, color);
}

void gl_drawHollowCir(int x0, int y0, int r, uint32 color)
{
  // 使用正负法画圆
  int x, y, f; //(x,y)为当前坐标 f为误差值
  x = x0;
  y = y0 + r;
  f = 0;

  while (y >= y0)
  {                            // 从圆周的右上部分开始
    gl_drawPoint(x, y, color); // 对称地画出四个象限内的坐标点
    gl_drawPoint(2 * x0 - x, y, color);
    gl_drawPoint(x, 2 * y0 - y, color);
    gl_drawPoint(2 * x0 - x, 2 * y0 - y, color);

    if (f > 0)
    {
      f = f - 2 * (y - y0) + 1;
      y--; // 向圆内走
    }
    else
    {
      f = f + 2 * (x - x0) + 1;
      x++; // 向圆外走
    }
  }

  if (y == y0)
    gl_drawPoint(x, y, color);
}

// 获取两个颜色之间的中间值
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
// 保存图片为bma格式
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
