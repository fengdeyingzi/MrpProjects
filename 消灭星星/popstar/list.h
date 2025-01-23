#ifndef _LI_H_
#define _LI_H_
#include "mrc_base.h"
#include "mrc_exb.h"

#include "mrc_tool.h"
#include "mrc_tpng.h"

#define float int32
#define LI_ITEM_WIDTH 8

typedef struct
{
    float x; // 坐标
    float y; // 坐标
    int r;
    float speed_x; // 横向运动值
    float speed_y; // 纵向运动值
    int hp;        // 生命
    int color;
    int alpha; // 透明度
} _POINT;

typedef struct
{
    _POINT *point;
    int32 bitmap; // 粒子图片
    int x;
    int y;           // 粒子发生点 坐标
    int r;           // 宽度
    int v;           // 速度
    int min_v;       // 最小速度
    int size;        // 粒子数量
    int birth;       // 出生量
    int speed;       // 粒子递减量
    int hp;          // 生命
    int color;       // 颜色 rgb值
    int coreColor;   // 核心颜色
    int isRandColor; // 是否产生随机颜色
    int isDirection; // 是否有方向
    int direction;   // 风向
    int isFirst;     // 是否只生成一次
    int time;        // 粒子运行时间
} _LI;

// 创建粒子
extern _LI *li_create(int x, int y);
extern int li_start(_LI *li);
// 粒子运动
extern int li_run(_LI *li);
// 设置粒子速度
extern int li_setV(_LI *li, int v);
// 设置粒子只生成一次
extern int li_setFirst(_LI *li);
// 设置粒子图片
extern int li_setBitmap(_LI *li, int32 bitmap);
// 设置粒子透明度递减量量
extern int li_setSpeed(_LI *li, int speed);
// 设置粒子生命
extern int li_setHp(_LI *li, int hp);
// 设置粒子数量
extern int li_setSize(_LI *li, int size);
// 设置粒子颜色
extern int li_setColor(_LI *li, int color);
// 设置粒子坐标
extern int li_setxy(_LI *li, int x, int y);
// 显示粒子
extern int li_draw(_LI *li);
// 删除粒子
extern int li_free(_LI *li);

#endif
