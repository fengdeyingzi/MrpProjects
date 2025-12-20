#include "mrc_base.h"
#include "mrc_graphics.h"
#include "xl_bmp.h"
#include <math.h>
#include <stdlib.h>
#include "mpc.h"

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

// 声明外部函数
extern int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3);

// 混合两个rgb565颜色 返回新的颜色
// dstColor 颜色1
// srcColor 颜色2
// alpha: 透明度 (0-255)
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
    
    // 重新组合为16位RGB565格式
    return (r << 11) | (g << 5) | b;
}

// 混合rgb565和rgb8888颜色 返回新的颜色
// dstColor 颜色1 (rgb565)
// srcColor 颜色2 (argb8888)
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

  buf = mrc_readFileFromAssets(filename, &len);
  if (buf == NULL)
    return NULL;
  head = (char *)buf;
  if (len > 0)
  {
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
  len = mrc_getLen(filename);

  if (len > 0)
  {
    f = mrc_open(filename, 1);
    buf = mr_malloc(len);
    if (buf == NULL)
    {
      return NULL;
    }
    if (f > 0)
    {
      mrc_read(f, buf, len);
      mrc_close(f);
      endName = mrc_strrchr(filename, '.');

      re = bmp_read(buf, len);
    }
    mrc_freeFileData(buf, len);
  }
  return re;
}

BITMAP_565 *readBitmap565(char *filename)
{
  return readBitmap(filename);
}

int32 drawBitmap565Flip(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
{
  if(bmp->color_bit == 16){
    return gl_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)w,(uint16)h, (uint16)bmp->mode, (int16)sx,(int16)sy,(uint16)bmp->transcolor);
  }else if(bmp->color_bit == 32){
    return gl_bitmapShowExTrans8888((uint32*)bmp->bitmap, (int16)x, (int16)y, (int16)bmp->width, (int16)w, (int16)h, (int16)sx, (int16)sy);
  }
  
  return MR_FAILED;
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
  // 检查输入参数的有效性
  if (p == NULL || screenBuffer == NULL || w <= 0 || h <= 0 ||
      sx < 0 || sy < 0 || sx + w > mw)
  {
    return MR_FAILED; // 参数无效，返回失败
  }
  switch (rop)
  {
  case BM_COPY:
    sourceX = sx; // 重置源图像的X坐标
    for (i = startY; i < endY; i++)
    {
      // 计算源图像的有效位移
      pindex = (sourceY * mw + sourceX);
      // 计算目标屏幕中的位置
      sindex = (i * SCRW + startX);
      mrc_memcpy(screenBuffer + sindex, p + pindex, (endX - startX) * 2);
      sourceY++; // 移动到源图像的下一个行
    }
    break;
  case BM_TRANSPARENT:
    // 遍历要绘制的区域
    for (i = startY; i < endY; i++)
    {
      pindex = (sourceY * mw + sourceX);
      // sindex = (i * SCRW);
      sindex = (i * SCRW + startX);
      for (j = startX; j < endX; j++)
      {
        // 计算源图像的有效位移

        pixelColor = p[pindex]; // 获取当前像素颜色

        // 判断是否为透明色
        if (pixelColor != transcolor)
        {
          screenBuffer[sindex] = pixelColor;
        }
        sindex++;
        pindex++;
      }
      sourceX = sx; // 重置源图像的X坐标
      sourceY++;    // 移动到源图像的下一个行
    }
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

  // 检查输入参数的有效性
  if (p == NULL || screenBuffer == NULL || w <= 0 || h <= 0 ||
      sx < 0 || sy < 0 || sx + w > mw)
  {
    return MR_FAILED; // 参数无效，返回失败
  }
  // 遍历要绘制的区域
  for (i = startY; i < endY; i++)
  {
    for (j = startX; j < endX; j++)
    {
      pixel = p[(sourceY * mw + sourceX)]; // 从图像缓冲区获取当前32位像素
      alpha = pixel >> 24;
      if (alpha != 0) // 判断是否为透明色
      {
        pixelColor = blendColor888(screenBuffer[i * SCRW + j], pixel); // B

        screenBuffer[i * SCRW + j] = pixelColor; // 写入屏幕缓冲区
      }
      sourceX++; // 移动到源图像的下一个像素
    }
    sourceX = sx; // 重置源图像的X坐标
    sourceY++;    // 移动到源图像的下一个行
  }

  return MR_SUCCESS; // 成功完成绘制
}

void drawBitmap565(BITMAP_565 *bmp, int32 x, int32 y)
{
  gl_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)bmp->width, (uint16)bmp->height, (uint16)bmp->mode, 0, 0, (uint16)bmp->transcolor);
}

void drawBitmap8888(BITMAP_565 *bmp, int32 x, int32 y)
{
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
  return *(w_getScreenBuffer() + (SCRW * y + x));
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
  BITMAP_565 *new_bmp = (BITMAP_565 *)mrc_malloc(sizeof(BITMAP_565)); // 分配内存用于新位图
  mrc_memset(new_bmp, 0, sizeof(BITMAP_565));
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
    return NULL;
  }

  if (!new_bmp->bitmap)
  {
    mrc_free(new_bmp); // 释放已分配的内存
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
void drawBitmap565Ex(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th)
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
  int r1 = (color1 >> 16) & 0xff;
  int g1 = (color1 >> 8) & 0xff;
  int b1 = color1 & 0xff;
  
  int r2 = (color2 >> 16) & 0xff;
  int g2 = (color2 >> 8) & 0xff;
  int b2 = color2 & 0xff;
  
  int draw_a = (color2 >> 24) & 0xff;
  
  int r = r1 * (255 - draw_a) / 255 + r2 * draw_a / 255;
  int g = g1 * (255 - draw_a) / 255 + g2 * draw_a / 255;
  int b = b1 * (255 - draw_a) / 255 + b2 * draw_a / 255;
  
  return (0xff000000) | (r << 16) | (g << 8) | b;
}

// 画点 参数：x, y, argb
void gl_drawPoint(int x, int y, uint32 color)
{
  uint16 *scr_color;
  uint16 *buffer;
  if (x < 0 || y < 0 || x >= SCRW || y >= SCRH)
    return;
  buffer = w_getScreenBuffer();
  scr_color = buffer + SCRW * y + x;
  *scr_color = blendColor888(*scr_color, color);
}

void gl_clearScreen(int32 r, int32 g, int32 b)
{
  int i;
  uint16 color = MAKERGB(r, g, b);
  uint16 *buffer = w_getScreenBuffer();
  if ((color >> 8) == (color & 0xff))
  {
    mrc_memset(buffer, color, SCRW * SCRH * 2);
  }
  else
  {
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
  mrc_printf("drawRect ....");
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

void gl_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;
  int err2;
  uint16 *buffer = w_getScreenBuffer();
  while (1)
  {
    if (x1 >= 0 && y1 >= 0 && x1 < SCRW && y1 < SCRH)
    {
      *(buffer + SCRW * y1 + x1) = blendColor888(*(buffer + SCRW * y1 + x1), color);
    }

    if (x1 == x2 && y1 == y2)
      break;
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

// 画圆
void gl_drawCir(int x0,int y0,int r, uint32 color)
{
  //使用正负法画圆
  int x, y, f;//(x,y)为当前坐标 f为误差值
  x = x0;
  y = y0 + r;
  f = 0;

  while(y >= y0){//从圆周的右上部分开始
    gl_drawLine( x, y, 2 * x0 - x, y,color);
    gl_drawLine( x, 2 * y0 -y, 2 * x0 - x, 2 * y0 -y,color);

    if(f > 0){
      f = f - 2 * (y - y0) + 1;
      y--;//向圆内走
    }
    else{
      f = f + 2 * (x - x0) + 1;
      x++;//向圆外走
    }
  }

  if(y == y0)
    gl_drawPoint( x, y,color);
}

// 绘制圆形
void drawCircle(int x, int y, int radius, unsigned int color)
{
    int i = 0;
    int tx = 0, ty = radius, d = 3 - (radius << 1);

    while (tx < ty)
    {
        for (i = x - ty; i <= x + ty; ++i)
        {
           gl_drawPoint( i, y - tx, color);
            if (tx){
               gl_drawPoint( i, y + tx, color);
            }
               
        }
        if (d < 0)
            d += (tx << 2) + 6;
        else
        {
            for (i = x - tx; i <= x + tx; ++i)
            {
               gl_drawPoint( i, y - ty, color);
               gl_drawPoint( i, y + ty, color);
            }
            d += ((tx - ty) << 2) + 10, ty--;
        }
        tx++;
    }
    if (tx == ty)
        for (i = x - ty; i <= x + ty; ++i)
        {
           gl_drawPoint( i, y - tx, color);
           gl_drawPoint( i, y + tx, color);
        }
}

int32 bitmap565getInfo(BITMAP_565* bmp, BITMAPINFO *info)
{
  mrc_memset(info,0, sizeof(BITMAPINFO));
  info->width = bmp->width;
  info->height = bmp->height;
  info->format = bmp->color_bit;
  info->ptr = bmp->bitmap;
  return 0;
}
