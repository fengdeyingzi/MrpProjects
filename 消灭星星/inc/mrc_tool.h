#ifndef MRC_TOOL_H
#define MRC_TOOL_H

#include "mrc_base.h"
#include "types.h"
extern int32 SCRW, SCRH;

// ��(x,y)��(px,py)��תָ������r���õ���ת�������
// ��������ת���ĵ�(px,py)����ת����뾶rx����ת����뾶ry�� ��ת����r, ��ת������ָ��(*x,*y)
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

//fps����
extern int32 initFpsTool(int32 delta);
extern int32 getMainFps(void);
//���ƽ���ʱ��ʱ������
extern int32 runFpsTool(void);

#endif
