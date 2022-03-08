/************************************************************
  FileName: graphics.h
        设备无关的图形函数接口  
***********************************************************/
#ifndef _GRAPHICS
#define _GRAPHICS

void init_graph();            /* 初始化图形模式 */
void close_graph();             /* 关闭图形模型 */
void set_color(XM_COLOR color);      /* 设置颜色 */
void set_fillcolor(XM_COLOR color);       /* 设置填充颜色 */
void put_pixel(int x, int y, XM_COLOR color);        /* 绘一个点 */
void put_line(int x1, int y1, int x2, int y2);        /* 绘一条线 */
void put_polyline(XM_POINT2D *ps, int n);         /* 绘多边形的线 */
void put_polygon(XM_POINT2D *ps, int n);        /* 绘一个填充的多边形 */

void put_rectangle(int left, int top, int right, int bottom);    /* 绘一个矩形 */
void set_clip_rectangle(int left, int top, int right, int botton);/* 设置裁剪区域 */
void flush_graph();/*把缓冲区的内容显示在屏幕上*/

#endif