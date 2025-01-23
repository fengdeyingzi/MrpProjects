#include "mrc_tool.h"

mr_colourSt text_color;
static int DSIN[91], DCOS[91];
static int init_cossin_ok;

void init_cossin()
{
    int i;
    int DSIN_TABLE[91] = {0, 71, 142, 214, 285, 356, 428, 499, 570, 640, 711, 781, 851, 921, 990, 1060, 1128, 1197, 1265, 1333, 1400, 1467, 1534, 1600, 1665, 1731, 1795, 1859, 1922, 1985, 2047, 2109, 2170, 2230, 2290, 2349, 2407, 2465, 2521, 2577, 2632, 2687, 2740, 2793, 2845, 2896, 2946, 2995, 3043, 3091, 3137, 3183, 3227, 3271, 3313, 3355, 3395, 3435, 3473, 3510, 3547, 3582, 3616, 3649, 3681, 3712, 3741, 3770, 3797, 3823, 3848, 3872, 3895, 3917, 3937, 3956, 3974, 3991, 4006, 4020, 4033, 4045, 4056, 4065, 4073, 4080, 4086, 4090, 4093, 4095, 4096};
    int DCOS_TABLE[91] = {4096, 4095, 4093, 4090, 4086, 4080, 4073, 4065, 4056, 4045, 4033, 4020, 4006, 3991, 3974, 3956, 3937, 3917, 3895, 3872, 3848, 3823, 3797, 3770, 3741, 3712, 3681, 3649, 3616, 3582, 3547, 3510, 3473, 3435, 3395, 3355, 3313, 3271, 3227, 3183, 3137, 3091, 3043, 2995, 2946, 2896, 2845, 2793, 2740, 2687, 2632, 2577, 2521, 2465, 2407, 2349, 2290, 2230, 2170, 2109, 2048, 1985, 1922, 1859, 1795, 1731, 1666, 1600, 1534, 1467, 1400, 1333, 1265, 1197, 1129, 1060, 990, 921, 851, 781, 711, 640, 570, 499, 428, 357, 285, 214, 143, 71, 0};

    if(init_cossin_ok) return ;

    for (i = 0; i < 91; i++)
        DSIN[i] = DSIN_TABLE[i];
    for (i = 0; i < 91; i++)
        DCOS[i] = DCOS_TABLE[i];
    init_cossin_ok = TRUE;
}

static int mycos(int x)
{
    int ret;
    init_cossin();

    if (x >= 0 && x <= 90)
        ret = DCOS[x];
    else if (x > 90 && x <= 180)
        ret = -DSIN[x - 90];
    else if (x > 180 && x <= 270)
        ret = -DCOS[x - 180];
    else if (x > 270 && x <= 360)
        ret = DSIN[x - 270];
    return ret;
}

static int mysin(int x)
{
    int ret;
    init_cossin();

    if (x >= 0 && x <= 90)
        ret = DSIN[x];
    else if (x > 90 && x <= 180)
        ret = DCOS[x - 90];
    else if (x > 180 && x <= 270)
        ret = -DSIN[x - 180];
    else if (x > 270 && x <= 360)
        ret = -DCOS[x - 270];
    return ret;
}

// 参数：旋转中心点(px,py)，旋转横向半径rx，旋转纵向半径ry， 旋转后坐标指针(*x,*y)
void toSpin(int px, int py, int rx, int ry, int r, int *x, int *y)
{
    int cx, cy, rr;
    init_cossin();
    
    if (r >= 0)
        rr = r % 361;
    else
        rr = -r % 361;

    cx = mycos(rr);
    cy = mysin(rr);
    *x = rx * cx / 4096 + px;
    *y = ry * cy / 4096 + py;
}

int32 get_str_width(char *str)
{
    int32 w = 0;
    int32 h = 0;

    if (str != NULL)
    {
        mrc_textWidthHeight(str, 0, MR_FONT_MEDIUM, &w, &h);
        return w;
    }
    return 0;
}

int32 get_str_height(char *str)
{
    int32 w = 0;
    int32 h = 0;

    if (str != NULL)
    {
        mrc_textWidthHeight(str, 0, MR_FONT_MEDIUM, &w, &h);
        return h;
    }
    return 0;
}

int32 collRect(int32 x, int32 y, int32 w, int32 h, int32 xx, int32 yy, int32 ww, int32 hh)
{
    return (abs(x - xx) < (w + ww) / 2) && (abs(y - yy) < (h + hh) / 2);
}

int32 fps_start, fps_prev, fps_frame, fps_speed;
int32 initFpsTool(int32 delta)
{
    fps_start = mrc_getUptime();
    fps_frame = 0;
    fps_speed = delta;
    return fps_start;
}

// 获取当前fps
int32 getRealFps(void)
{
    return 0;
}

// 获取平均fps
int32 getMainFps(void)
{
    return fps_frame * 1000 / (mrc_getUptime() - fps_start);
}

// 绘制结束时定时器调用
int32 runFpsTool(void)
{
    fps_prev = mrc_getUptime();
    fps_frame++;
    return fps_frame;
}
