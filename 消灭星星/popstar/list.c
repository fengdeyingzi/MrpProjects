#include "list.h"

// 创建粒子
_LI *li_create(int x, int y)
{
    _LI *li = (_LI*)mrc_malloc(sizeof(_LI));
    li->x = x;
    li->y = y;
    li->r = 5;
    li->v = 4;
    li->min_v = 1;
    li->bitmap = 0;
    li->isRandColor = FALSE;
    li->speed = 10;
    li->isDirection = FALSE;
    li->direction = 270;
    li->hp = 20;
    li->birth = 5;
    li->size = 50;
    li->time = 1;
    li->point = (_POINT*)mrc_malloc(sizeof(_POINT) * li->size);
    mrc_memset(li->point, 0, sizeof(_POINT) * li->size);
    li->color = 0xffffff;
    li->coreColor = 0xffffff;

    return li;
}

int li_start(_LI *li)
{
    mrc_memset(li->point, 0, sizeof(_POINT) * li->size);
    return 0;
}

// 粒子运动
int li_run(_LI *li)
{
    // 生成粒子
    int i, alpha;
    int size = 0;
    int x, y, v;
    _POINT *point;

    for (i = 0; i < li->size; i++)
    {
        if (li->point[i].hp <= 0)
        {
            point = &(li->point[i]);
            // 判断是否为第一次生成
            if ((point->x != 0) && (point->y != 0) && li->isFirst)
                break;

            point->hp = li->hp;
            point->x = li->x;
            point->y = li->y;
            point->alpha = 255;
            alpha = point->alpha;
            point->color = li->color;
            if (li->isRandColor)
                point->color |= mrc_rand() % 0xffffff;
            point->r = li->r;
            point->speed_x = -3 + mrc_rand() % 6;
            point->speed_y = -3 + mrc_rand() % 6;

            v = ((mrc_rand() % (li->v - li->min_v + 1)) + li->min_v) * 100 + 2;
            mrc_printf("%d\n", v);
            toSpin(0, 0, v, v, mrc_rand() % 360, &x, &y);
            point->speed_x = ((float)x) / 100;
            point->speed_y = ((float)y) / 100;
            size++;
            if (size >= li->birth)
                break;
        }
    }

    // 运动
    for (i = 0; i < li->size; i++)
    {
        point = &(li->point[i]);
        point->x += point->speed_x;
        point->y += point->speed_y;
        v = 200;
        if (li->isDirection)
        {
            toSpin(0, 0, v, v, li->direction, &x, &y);
            point->speed_x += ((float)x) / 100;
            point->speed_y += ((float)y) / 100;
        }
        point->hp--;
        point->alpha -= li->speed;
        if (point->alpha < 0)
            point->alpha = 0;
    }

    li->time++;

    return 0;
}
// 设置粒子速度
int li_setV(_LI *li, int v)
{
    li->v = v;
    return 0;
}

// 设置粒子只生成一次
int li_setFirst(_LI *li)
{
    li->isFirst = TRUE;
    return 0;
}

// 设置粒子图片
int li_setBitmap(_LI *li, int32 bitmap)
{
    li->bitmap = bitmap;
    return 0;
}

// 设置粒子只生成一次

// 获取粒子播放时间
int li_getTime(_LI *li)
{
    return li->time;
}

// 设置粒子透明度递减量量
int li_setSpeed(_LI *li, int speed)
{
    li->speed = speed;
    return 0;
}

// 设置粒子生成角度
int li_setDirection(_LI *li, int direction)
{
    li->isDirection = TRUE;
    li->direction = direction;
    return 0;
}

// 设置粒子生命
int li_setHp(_LI *li, int hp)
{
    li->hp = hp;
    return 0;
}

// 设置粒子数量
int li_setSize(_LI *li, int size)
{
    li->size = size;
    if (li->point)
        mrc_free(li->point);
    li->point = mrc_malloc(sizeof(_POINT) * li->size);
    mrc_memset(li->point, 0, sizeof(_POINT) * li->size);
    return 0;
}

// 设置粒子坐标
int li_setxy(_LI *li, int x, int y)
{
    li->x = x;
    li->y = y;
    return 0;
}

// 设置粒子颜色
int li_setColor(_LI *li, int color)
{
    li->color = color;
    return 0;
}

// 显示粒子
int li_draw(_LI *li)
{
    int i;
    int r, g, b;
    _POINT *point;

    // 如果是图片
    if (li->bitmap)
    {
        for (i = 0; i < li->size; i++)
        {
            point = &(li->point[i]);
            if (point->hp > 0)
                mrc_bitmapShow(li->bitmap, point->x, point->y, BM_TRANSPARENT, 0, 0, LI_ITEM_WIDTH, LI_ITEM_WIDTH);
        }
    }
    else
    {
        for (i = 0; i < li->size; i++)
        {
            point = &(li->point[i]);
            if (point->hp > 0 && point->x >= 0 && point->x < SCRW && point->y >= 0 && point->y < SCRH)
            {
                r = (point->color >> 16) & 0xff;
                g = (point->color >> 8) & 0xff;
                b = (point->color) & 0xff;
                r = r + (255 - r) * point->alpha / 255;
                g = g + (255 - g) * point->alpha / 255;
                b = b + (255 - b) * point->alpha / 255;
                //drawCir(point->x, point->y, point->r, (point->alpha << 24) | (r << 16) | (g << 8) | b);
            }
        }
    }

    return 0;
}

// 删除粒子
int li_free(_LI *li)
{
    mrc_free(li->point);
    mrc_free(li);
    return 0;
}

