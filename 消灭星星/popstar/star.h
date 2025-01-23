#ifndef _STAR_H_
#define _STAR_H_

#include "list.h"

#define STAR_ITEM_WIDTH 24
#define STAR_BITMAP_ID 0

typedef struct
{
    int x;       // x坐标
    int y;       //
    int type;    // 类型
    int isDraw;  // 是否显示
    int isBeset; // 是否被圈住
} STAR;

typedef struct
{
    int32 bitmap[6];
    _LI *li;
    int32 par_bitmap[6];
    STAR *star;
    int x;
    int y;
    int itemw;
    int itemh;
    int width;
    int height;
    int fen;
    char view_fen[50];
} _STARS;

// 创建星星数组
// 参数：x坐标 y坐标 屏幕宽度 屏幕高度
extern _STARS *star_create_withscr(int x, int y, int width, int height);

// 创建星星数组
// 参数：x坐标 y坐标 宽度 高度
extern _STARS *star_create(int x, int y, int width, int height);

// 显示星星
extern int star_draw(_STARS *stars);

// 星星event事件
extern int star_event(_STARS *stars, int type, int p1, int p2);

#endif
