
#include <mrc_base.h>
#include "mrc_android.h"
#include "mrc_graphics.h"
#include "xl_debug.h"
#include "mpc.h"
#include <math.h>

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
extern int pointInTriangle(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3);
extern uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha);
BITMAP_565 *createBitmap565(int width, int height)
{
  BITMAP_565 *bmp = (BITMAP_565 *)mrc_malloc(sizeof(BITMAP_565));
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

BITMAP_565 *readBitmap565FromAssets(char *filename)
{
  int32 len = 0;
  void *buf;
  BITMAP_565 *re = NULL;
  char *endName = NULL;
  char *head = NULL;
  //  debug_printf("开始读取文件");

  buf = mrc_readFileFromAssets(filename, &len);
  head = (char *)buf;
  if (len > 0)
  {
    //  debug_printf("解析图片");
    endName = mrc_strrchr(filename, '.');

    if (head[0] == 'M' && head[1] == 'A' && head[2] == 'P' && head[3] == '5')
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

BITMAP_565 *readBitmap565(char *filename)
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
int32 drawBitmap565Flip(BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
{
  bmp_drawflip(buf, x, y, w, h, sx, sy);
  return 0;
}
void drawBitmap565(BITMAP_565 *b, int32 x, int32 y)
{
  bmp_draw(b, x, y);
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
  int32 minX, minY;
  int32 maxX, maxY;
  uint16 *buffer = w_getScreenBuffer();
  // 限制 alpha 的范围在 0 到 255 之间
  if (alpha < 0)
    alpha = 0;
  if (alpha > 255)
    alpha = 255;
  minX = MAX(0, x);
  minY = MAX(0, y);
  maxX = MIN(SCRW, x + b->width);
  maxY = MIN(SCRH, y + b->height);
  // 遍历位图的每个像素
  if (b->mode == BM_TRANSPARENT)
  {
    for (j = 0; j < b->height; j++)
    {
      for (i = 0; i < b->width; i++)
      {
        // 计算当前像素的位图的索引
        color = b->bitmap[j * b->width + i];

        // 如果是透明色，则跳过该像素
        if (color == b->transcolor)
        {
          continue;
        }

        // 计算目标点的位置
        targetX = x + i;
        targetY = y + j;
        *(buffer + SCRW * targetY + targetX) = blendColor(*(buffer + SCRW * targetY + targetX), color, alpha);
      }
    }
  }
  else
  {
    for (j = 0; j < b->height; j++)
    {
      for (i = 0; i < b->width; i++)
      {
        // 计算当前像素的位图的索引
        color = b->bitmap[j * b->width + i];

        // 计算目标点的位置
        targetX = x + i;
        targetY = y + j;
        *(buffer + SCRW * targetY + targetX) = blendColor(*(buffer + SCRW * targetY + targetX), color, alpha);
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
void drawBitmap565Rotate(BITMAP_565 *b, int32 centerX, int32 centerY, int32 bx, int32 by, int32 r)
{
  int32 x, y;
  int32 newX, newY;
  int32 x1, y1, x2, y2;
  // uint16 color11, color12, color21, color22;
  // double dx, dy;
  uint16 color;

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
  void mrc_drawRotatedRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint8 r, uint8 g, uint8 b)
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
    vertices[1][1] = centerY - (height - by);

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
      mrc_drawLine(x1, y1, x2, y2, r, g, b);
    }
  }

  /*
  将bitmap绘制到另一个bitmap上
  将buf中的bitmap图片, 从sx,sy开始的宽高为w,h的区域,绘制到图片di从x,y开始的地方
  */
  void drawBitmap565Old(BITMAP_565 * di, BITMAP_565 * buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy)
  {
    int32 j;
    int32 i;

    // 检查目标位图的边界
    if (x < 0 || y < 0 || w <= 0 || h <= 0 || sx < 0 || sy < 0 ||
        sx + w > buf->width || sy + h > buf->height)
    {
      mrc_printf("drawBitmap565Old error\n");
      return; // 无效参数，直接返回
    }
    if (x + w > di->width)
    {
      w = di->width - x;
    }
    if (y + h > di->height)
    {
      h = di->height - y;
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
            mrc_printf("...");
            di->bitmap[dest_y * di->width + dest_x] = color; // 绘制到目标位图
          }
        }
      }
      else
      {
        mrc_memcpy(di->bitmap, buf->bitmap, w * 2);
      }
    }
  }

  /*
  将bitmap缩放,生成一个新的BITMAP
  width 生成的图片宽度
  height 生成的图片高度
  */
  BITMAP_565 *createBitmapFromBitmap(BITMAP_565 * bmp, int32 width, int32 height)
  {
    int32 x_ratio;
    int32 y_ratio;
    int32 src_x, src_y;
    int32 x, y;
    // 变量声明
    BITMAP_565 *new_bmp = (BITMAP_565 *)malloc(sizeof(BITMAP_565)); // 分配内存用于新位图
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
    new_bmp->bitmap = (uint16 *)malloc(width * height * sizeof(uint16));
    if (!new_bmp->bitmap)
    {
      free(new_bmp); // 释放已分配的内存
      return NULL;   // 分配失败
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
        new_bmp->bitmap[y * width + x] = bmp->bitmap[src_y * bmp->width + src_x];
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
  void drawBitmap565Ex(BITMAP_565 * bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th)
  {
    int px, py; // 屏幕区域坐标(相对)
    int32 pindex;
    if (w == tw && h == th)
    {
      mrc_bitmapShowExTrans(bmp->bitmap, (int16)x, (int16)y, (uint16)bmp->width, (uint16)tw, (uint16)th, (uint16)bmp->mode, tx, ty, (uint16)bmp->transcolor);
    }
    else
    {
      // 根据屏幕坐标计算出图片上的点
      for (px = 0; px < w; px++)
      {
        for (py = 0; py < h; py++)
        {
          pindex = bmp->width * (tx + py * th / h) + (ty + px * tw / w);
          if (pindex < bmp->width * bmp->height)
            mrc_drawPoint(px + x, py + y, *(bmp->bitmap + pindex));
          //*(getscrbuf() + (SCRW*(x+py)+(px+x))) = *(bmp->bitmap + bmp->width * (tx+py*th/h) + (ty+px*tw/w));
        }
      }
    }
  }

  int32 bitmap565Free(BITMAP_565 * b)
  {
    bmp_free(b);
    return 0;
  }

  // 获取两点之间的长度 的平方
  static int gl_getLineSize(int x, int y, int x2, int y2)
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

    // 进行 alpha 混合
    uint8 r = (r1 * (255 - alpha) + r2 * alpha) / 255;
    uint8 g = (g1 * (255 - alpha) + g2 * alpha) / 255;
    uint8 b = (b1 * (255 - alpha) + b2 * alpha) / 255;

    // 重新组合为 16 位 RGB 565 格式
    return (r << 11) | (g << 5) | b;
  }

  // 画点 参数：x, y, argb
  void gl_drawPoint(int x, int y, uint32 color)
  {
    uint16 scr_color;
    uint16 color565;
    int32 alpha;
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
    scr_color = *(buffer + SCRW * y + x);
    color565 = MAKECOLOR565(color);
    alpha = color >> 24;

    *(buffer + SCRW * y + x) = blendColor(scr_color, color565, alpha);
    // 生成rgb565
    //*(getscrbuf()+ (SCRW*y+x)) = MAKECOLOR565(o_color);
    // mrc_drawPoint(x, y, MAKECOLOR565(o_color));
    //*(((uint16*)getscrbuf())+ (SCRW*y+x)*2) = 0xffff;
    // printf("setzcolor %x\n",MAKECOLOR565(o_color));
  }

  // 画矩形
  void gl_drawRect(int32 x, int32 y, int32 w, int32 h, uint32 color)
  {
    int ix, iy;
    uint16 color565 = MAKECOLOR565(color);
    uint16 *buffer = w_getScreenBuffer();
    int32 maxX, maxY;
    int32 alpha;
    uint16 upcolor;
    uint16 upscrcolor = 0;
    maxX = MIN(SCRW, x + w);
    maxY = MIN(SCRH, y + h);
    x = MAX(0, x);
    y = MAX(0, y);
    alpha = color >> 24;
    upcolor = blendColor(upscrcolor, color565, alpha);
    if (alpha == 0xff)
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
            upcolor = *(buffer + SCRW * iy + ix) = blendColor(upscrcolor = *(buffer + SCRW * iy + ix), color565, alpha);
          }

          // gl_drawPoint(ix, iy, color);
        }
      }
    }
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
    upcolor - blendColor(upsrccolor, color565, alpha);
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
            upcolor = *(buffer + SCRW * y + x) = blendColor(upsrccolor = *(buffer + SCRW * y + x), color565, alpha);
            // gl_drawPoint(x, y, color);
          }
        }
      }
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

    while (1)
    {
      gl_drawPoint(x1, y1, color); // 绘制当前点

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

  // 在bitmap内绘制矩形
  void bmp_drawRect(BITMAP_565 * bmp, int32 x, int32 y, int32 w, int32 h, uint32 color)
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
  void bmp_drawPoint(BITMAP_565 * bmp, int32 x, int32 y, uint32 color)
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
  void bmp_drawLine(BITMAP_565 * bmp, int32 x1, int32 y1, int32 x2, int32 y2, uint32 color)
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
  void bmp_drawCircle(BITMAP_565 * bmp, int32 cx, int32 cy, int32 radius, uint32 color)
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
  void bmp_drawHollowCircle(BITMAP_565 * bmp, int32 centerX, int32 centerY, int32 radius, uint32 color)
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

  // int pointInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3)
  // {
  //   // 计算重心坐标法
  //   float area = 0.5 * (-y2 * x3 + y1 * (-x2 + x3) + x1 * (y2 - y3) + x2 * y3);
  //   float s = 1 / (2 * area) * (y1 * x3 - x1 * y3 + (x3 - x1) * py + (x1 - x2) * px);
  //   float t = 1 / (2 * area) * (x1 * y2 - y1 * x2 + (x1 - x2) * py + (y1 - y2) * px);
  //   return (s > 0) && (t > 0) && (1 - s - t > 0);
  // }

  void bmp_drawTriangle(BITMAP_565 * bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
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

  void bmp_drawHollowTriangle(BITMAP_565 * bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color)
  {
    // 仅画三条边
    bmp_drawLine(bmp, x1, y1, x2, y2, color);
    bmp_drawLine(bmp, x2, y2, x3, y3, color);
    bmp_drawLine(bmp, x3, y3, x1, y1, color);
  }

  // 画圆
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
          *(buffer + SCRW * iy + ix) = blendColor(*(buffer + SCRW * iy + ix), color565, alpha);
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
          *(buffer + SCRW * iy + ix) = blendColor(*(buffer + SCRW * iy + ix), color565, alpha);
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
          *(buffer + SCRW * iy + ix) = blendColor(*(buffer + SCRW * iy + ix), color565, alpha);
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
          *(buffer + SCRW * iy + ix) = blendColor(*(buffer + SCRW * iy + ix), color565, alpha);
        }
      }
      return;
    }
  }

  int32 bitmap565getInfo(BITMAP_565 * bmp, BITMAPINFO * info)
  {
    mrc_memset(info, 0, sizeof(BITMAPINFO));
    info->width = bmp->width;
    info->height = bmp->height;
    info->format = 16;
    info->ptr = bmp->bitmap;
    return 0;
  }
  // 保存图片为bma格式
  void saveBitmap(BITMAP_565 * bmp, const char *filename)
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
