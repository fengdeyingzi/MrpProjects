#ifndef MRC_TOOL_H
#define MRC_TOOL_H

#include "mrc_base.h"
#include "types.h"
extern int32 SCRW, SCRH;

// 点(x,y)绕(px,py)旋转指定弧度r，得到旋转后的坐标
// 参数：旋转中心点(px,py)，旋转横向半径rx，旋转纵向半径ry， 旋转弧度r, 旋转后坐标指针(*x,*y)
extern void toSpin(int px, int py, int rx, int ry, int r, int *x, int *y);
extern void init_cossin();

#define CHAR_H get_str_height("A")
#define CHAR_W get_str_width("A")

extern mr_colourSt text_color;
#define set_text_color(new_color)                     \
    do                                           \
    {                                            \
        text_color.r = PIXEL888RED(new_color);   \
        text_color.g = PIXEL888GREEN(new_color); \
        text_color.b = PIXEL888BLUE(new_color);  \
    } while (0)
#define draw_text(x, y, str) mrc_drawText(str, x, y, text_color.r, text_color.g, text_color.b, 0, MR_FONT_MEDIUM)

extern int32 collRect(int32 x, int32 y, int32 w, int32 h, int32 xx, int32 yy, int32 ww, int32 hh);

//fps工具
extern int32 initFpsTool(int32 delta);
extern int32 getMainFps(void);
//绘制结束时定时器调用
extern int32 runFpsTool(void);

#endif
