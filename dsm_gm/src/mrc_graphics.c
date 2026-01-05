#include "mrc_base.h"
#include "mrc_graphics.h"
#include "xl_bmp.h"
#include "ex_math.h"
#include "mpc.h"
#include "uc3_mfont.h"
#include "emoji.h"

/*
实现一些绘图函数
主要是实现bmp图片绘制

风的影子

*/

// 是否使用mrc绘制函数
#define USE_MRC_TEXT 1

// 字体管理：支持 0, 1, 2 三种字体大小
static UC3_FONT *g_fonts[3] = {NULL, NULL, NULL};

// 获取字体实例，如果未加载则自动加载
static UC3_FONT* get_font(uint16 font) {
    char font_name[32];

    // 检查 font 参数范围
    if (font > 2) {
        mrc_printf("get_font: invalid font index: %d", font);
        return NULL;
    }

    // 如果字体已加载，直接返回
    if (g_fonts[font] != NULL) {
        return g_fonts[font];
    }

    // 构造字体文件名并加载
    mrc_sprintf(font_name, "font_%d.uc3", font);
    g_fonts[font] = mfont_load(font_name);

    if (g_fonts[font] == NULL) {
        mrc_printf("get_font: failed to load font: %s", font_name);
    }

    return g_fonts[font];
}
// 将rgb888转rgb565
#define MAKECOLOR565(color) ((uint16)((color >> 8) & 0xf800) | (uint16)((color >> 5) & 0x07e0) | (uint16)((color >> 3) & 0x1f))
#ifndef MAKERGB
#define MAKERGB(r, g, b) (((uint16)(r >> 3) << 11) | ((uint16)(g >> 2) << 5) | ((uint16)(b >> 3)))
#endif

#define BLEND_COLOR(dstColor, srcColor, alpha)                                                               \
  (((((((dstColor) >> 11) & 0x1F) * (255 - (alpha)) + (((srcColor) >> 11) & 0x1F) * (alpha)) / 255) << 11) | \
   (((((dstColor) >> 5) & 0x3F) * (255 - (alpha)) + (((srcColor) >> 5) & 0x3F) * (alpha)) / 255) << 5) |     \
      ((((dstColor) & 0x1F) * (255 - (alpha)) + ((srcColor) & 0x1F) * (alpha)) / 255)

// 声明外部函数
extern int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3);
extern void gl_drawHLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);
extern void gl_drawVLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);
extern void drawBitmap8888(BITMAP_565 *bmp, int32 x, int32 y);
extern void drawBitmap565(BITMAP_565 *bmp, int32 x, int32 y);
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

BITMAP_565 *readBitmapFromAssets(const char *filename)
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
      mrc_freeFileData(buf, len);
    }
  }
  return re;
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

int32 drawBitmapFlip(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
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

  // 如果绘制区域完全在屏幕外，直接返回
  if (startX >= endX || startY >= endY) {
    return MR_SUCCESS;
  }

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
    for (i = startY; i < endY; i++)
    {
      // 计算源图像的有效位移 - 修正：sourceX应该基于当前行和偏移计算
      pindex = (sourceY * mw + sourceX);
      // 计算目标屏幕中的位置
      sindex = (i * SCRW + startX);
      // 复制一行像素
      mrc_memcpy(screenBuffer + sindex, p + pindex, (endX - startX) * 2);
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

  // 如果绘制区域完全在屏幕外，直接返回
  if (startX >= endX || startY >= endY) {
    return MR_SUCCESS;
  }

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

// 辅助函数：用于翻转32位图像数据
void flipBitmap32(uint32 *bitmap, int width, int height, int flip)
{
  uint32 temp;
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

  dst_max_y = (h * ex_abs(tD) + w * ex_abs(tC)) >> 9;
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


// 绘制旋转矩形
// centerx centery 绘制在屏幕上的中心位置
// bx by 相对于矩形的旋转中心
void gl_drawRotatedHollowRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, int32 angle, uint32 color)
{
  int16 x1, y1, x2, y2;
  // 使用整数三角函数（256倍缩放）
  int32 cosR = INT_COS(angle);  // 已经缩放256倍
  int32 sinR = INT_SIN(angle);  // 已经缩放256倍
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
    vertices[i][0] = centerX + (x * cosR - y * sinR) / TRIG_SCALE;
    vertices[i][1] = centerY + (x * sinR + y * cosR) / TRIG_SCALE;
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
/*
  绘制旋转的矩形
  centerx centery 绘制在屏幕上的中心位置
  bx by 相对于矩形的旋转中心
  angle 旋转角度，0-359度
*/
void gl_drawRotatedRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, int32 angle, uint32 color)
{
  // 使用整数三角函数（256倍缩放）
  int32 cosR = INT_COS(angle);  // 已经缩放256倍
  int32 sinR = INT_SIN(angle);  // 已经缩放256倍
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
    vertices[i][0] = centerX + (x * cosR - y * sinR) / TRIG_SCALE;
    vertices[i][1] = centerY + (x * sinR + y * cosR) / TRIG_SCALE;
  }

  // 绘制旋转后的矩形
  gl_drawTriangle(vertices[0][0], vertices[0][1], vertices[1][0], vertices[1][1], vertices[2][0], vertices[2][1], color);
  gl_drawTriangle(vertices[0][0], vertices[0][1], vertices[3][0], vertices[3][1], vertices[2][0], vertices[2][1], color);
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

//绘制空心矩形
void gl_drawHollowRect(int x, int y, int width, int height, uint32 color){
  if (width <= 0 || height <= 0) {
    return;
  }

  // 使用HLine和VLine加速绘制
  // 上边
  gl_drawHLine(x, y, x + width - 1, y, color);
  // 下边
  gl_drawHLine(x, y + height - 1, x + width - 1, y + height - 1, color);
  // 左边
  gl_drawVLine(x, y, x, y + height - 1, color);
  // 右边
  gl_drawVLine(x + width - 1, y, x + width - 1, y + height - 1, color);
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

void gl_drawVLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{
  int32 i;
  uint16 *buffer = w_getScreenBuffer();
  int32 minY;
  int32 maxY;
  uint16 upsrccolor = 0;
  uint16 upcolor = blendColor888(upsrccolor, color);

  if (y1 < y2)
  {
    minY = MIN(MAX(0, y1), SCRH);
    maxY = MIN(MAX(0, y2), SCRH);
  }
  else
  {
    minY = MIN(MAX(0, y2), SCRH);
    maxY = MIN(MAX(0, y1), SCRH);
  }
  if (x1 < 0 || x1 >= SCRW)
    return;

  for (i = minY; i < maxY; i++)
  {
    if (upsrccolor == buffer[SCRW * i + x1])
    {
      buffer[SCRW * i + x1] = upcolor;
    }
    else
    {
      upcolor = buffer[SCRW * i + x1] = blendColor888(upsrccolor = buffer[SCRW * i + x1], color);
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


void gl_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
{
  int dx = ex_abs(x2 - x1);
  int dy = ex_abs(y2 - y1);
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

// 在bitmap内绘制线段

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

void gl_drawRoundRect(int32 x, int32 y, int32 w, int32 h, int32 radius, uint32 color)
{
    int32 ix, iy;
    int32 dx, dy;

    

    if (w <= 0 || h <= 0 || radius < 0) {
        return;
    }

    // 限制圆角半径不超过矩形的一半
    if (radius > w / 2) radius = w / 2;
    if (radius > h / 2) radius = h / 2;

    // 如果圆角半径为0，直接绘制普通矩形
    if (radius == 0) {
        gl_drawRect(x, y, w, h, color);
        return;
    }

    // 绘制中间部分的矩形（不包含圆角）
    // 上中矩形
    gl_drawRect( x + radius, y, w - 2 * radius, radius, color);
    // 中间矩形
    gl_drawRect( x, y + radius, w, h - 2 * radius, color);
    // 下中矩形
    gl_drawRect( x + radius, y + h - radius, w - 2 * radius, radius, color);

    // 绘制四个圆角
    for (iy = 0; iy < radius; iy++) {
        for (ix = 0; ix < radius; ix++) {
            dx = radius - ix;
            dy = radius - iy;

            // 判断点是否在圆内
            if (dx * dx + dy * dy <= radius * radius) {
                // 左上角
                gl_drawPoint( x + ix, y + iy, color);
                // 右上角
                gl_drawPoint( x + w - 1 - ix, y + iy, color);
                // 左下角
                gl_drawPoint( x + ix, y + h - 1 - iy, color);
                // 右下角
                gl_drawPoint( x + w - 1 - ix, y + h - 1 - iy, color);
            }
        }
    }
}

// 画文字 font表示字体大小，可选0/1/2三种大小
int32 gl_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font){
  #if USE_MRC_TEXT  
  return mrc_drawText(pcText, x, y, r, g, b, is_unicode, font);
  #else
  UC3_FONT *fontObj = get_font(font);

    if (fontObj == NULL) {
        mrc_printf("gl_drawText: failed to get font %d", font);
        return MR_FAILED;
    }

    return mfont_drawText(fontObj, pcText, x, y, r, g, b, is_unicode);
  #endif
}

// 在矩形区域内绘制多行uc3字体 y坐标可为负数
int32 gl_drawTextInRect(const char *pcText, int16 dx, int16 dy, int16 x, int16 y, int16 width, int16 height, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font){
  #if USE_MRC_TEXT  
  mr_screenRectSt rect = {dx, dy, width, height};
  mr_colourSt colour = {r, g, b};
  return mrc_drawTextEx(pcText, x, y, rect, colour, is_unicode | DRAW_TEXT_EX_IS_AUTO_NEWLINE, font);
  #else
  UC3_FONT *fontObj = get_font(font);

    if (fontObj == NULL) {
        mrc_printf("gl_drawTextInRect: failed to get font %d", font);
        return MR_FAILED;
    }

    return mfont_drawTextInRect(fontObj, pcText, dx, dy, x, y, width, height, r, g, b, is_unicode);
  #endif
}

// 返回待显示字符串若显示在宽为w的区间里，需要的行数
int32 gl_textRow(const char* pcText, int32 w, int is_unicode, uint16 font){
  #if USE_MRC_TEXT
  int len = mrc_strlen(pcText)*2+2;
  if(is_unicode == 0){
    char *temp = mrc_c2u(pcText, NULL, len);
    int row = mrc_unicodeTextRow(temp, font, w);
    mrc_free(temp);
    return row;
  }
  return mrc_unicodeTextRow(pcText, font, w);
  #else
    UC3_FONT *fontObj = get_font(font);
    int line_count = 1;
    int current_width = 0;
    int line_height;
    uint16 *ucptr;
    uint8 *utf8;
    uint16 unicode;
    int char_width;

    if (fontObj == NULL) {
        mrc_printf("gl_textRow: failed to get font %d", font);
        return 1;
    }

    line_height = fontObj->font_height + 2;

    if (is_unicode) {
        ucptr = (uint16 *)pcText;
        while (*ucptr) {
            char_width = mfont_getCharWidth(fontObj, *ucptr);
            if (current_width + char_width > w) {
                line_count++;
                current_width = char_width;
            } else {
                current_width += char_width;
            }
            ucptr++;
        }
    } else {
        // UTF-8编码处理
        utf8 = (uint8 *)pcText;
        while (*utf8) {
            // UTF-8解码
            if ((*utf8 & 0x80) == 0) {
                unicode = *utf8++;
            } else if ((*utf8 & 0x20) == 0) {
                unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                utf8 += 2;
            } else {
                unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                utf8 += 3;
            }

            char_width = mfont_getCharWidth(fontObj, unicode);
            if (current_width + char_width > w) {
                line_count++;
                current_width = char_width;
            } else {
                current_width += char_width;
            }
        }
    }

    return line_count;
    #endif
}

// 获取文字宽高 保存到w和h中
int32 gl_textWidthHeight(const char* pcText, int is_unicode, uint16 font, int32* w, int32* h){
  #if USE_MRC_TEXT
  return mrc_textWidthHeight(pcText, is_unicode, font, w, h);
  #else
    UC3_FONT *fontObj = get_font(font);

    if (fontObj == NULL) {
        mrc_printf("gl_textWidthHeight: failed to get font %d", font);
        return MR_FAILED;
    }

    *w = mfont_getWidth(fontObj, pcText, is_unicode);
    *h = mfont_getHeight(fontObj, pcText, is_unicode);

    return MR_SUCCESS;
    #endif
}

int32 bitmapGetInfo(BITMAP_565* bmp, BITMAPINFO *info)
{
  mrc_memset(info,0, sizeof(BITMAPINFO));
  info->width = bmp->width;
  info->height = bmp->height;
  info->format = bmp->color_bit;
  info->ptr = bmp->bitmap;
  return 0;
}


// 绘制emoji封装, size表示emoji图标的宽高
int32 gl_emoji_draw(char *emoji, int x, int y, int size){
    return emoji_draw(emoji, x, y, size);
}

// 释放资源（font资源需要释放）
int32 gl_free(){
    int i;
    for (i = 0; i < 3; i++) {
        if (g_fonts[i] != NULL) {
            mfont_free(g_fonts[i]);
            g_fonts[i] = NULL;
        }
    }

    /* 释放emoji资源 */
    emoji_free();

    return MR_SUCCESS;
}