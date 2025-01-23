
#include "star.h"

void star_upset(_STARS *stars);

// 创建星星数组
// 参数：x坐标 y坐标 屏幕宽度 屏幕高度
_STARS *star_create_withscr(int x, int y, int width, int height)
{
    int32 xitem = width / STAR_ITEM_WIDTH;
    int32 yitem = height / STAR_ITEM_WIDTH;
    int xx = width - xitem * STAR_ITEM_WIDTH;
    int yy = height - yitem * STAR_ITEM_WIDTH;
    return star_create(x + xx / 2, y + yy / 2, xitem, yitem);
}

// 创建星星数组
// 参数：x坐标 y坐标 横项 竖项
_STARS *star_create(int x, int y, int width, int height)
{
    char text[50];
    int32 i, j;
    int irand = 1;
    _STARS *stars = mrc_malloc(sizeof(_STARS));
    // 小星星粒子
    int color[6] = {0xffffff, 0xff90aa, 0x30f000, 0x8dc0f0, 0xf060f0, 0xffff00};

    mrc_memset(stars, 0, sizeof(_STARS));
    stars->star = mrc_malloc(sizeof(STAR) * width * height);
    stars->x = x;
    stars->y = y;
    stars->width = width;
    stars->height = height;
    stars->itemw = STAR_ITEM_WIDTH;
    stars->itemh = STAR_ITEM_WIDTH;

    for (i = 0; i <= 5; i++)
    {
        mrc_sprintf(text, "block_%d.bmp", i);
        stars->bitmap[i] = i + STAR_BITMAP_ID;
        mrc_bitmapLoad(i + STAR_BITMAP_ID, text, 0, 0, STAR_ITEM_WIDTH, STAR_ITEM_WIDTH, STAR_ITEM_WIDTH);
    }
    mrc_bitmapLoad(i + STAR_BITMAP_ID, "particle.bmp", 0, 0, LI_ITEM_WIDTH, LI_ITEM_WIDTH, LI_ITEM_WIDTH);
    for (j = 0; j <= 5; j++)
    {
        stars->par_bitmap[j] = i + STAR_BITMAP_ID;
    }

    stars->li = li_create(SCRW / 2, SCRH / 2);
    li_setColor(stars->li, 0xff6420);
    li_setFirst(stars->li);
    li_setBitmap(stars->li, stars->par_bitmap[0]);

    // 循环生成星星
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            stars->star[i * width + j].type = irand;
            stars->star[i * width + j].isDraw = TRUE;
            stars->star[i * width + j].x = j * stars->itemw;
            stars->star[i * width + j].y = i * stars->itemh;
            stars->star[i * width + j].isBeset = 0;
        }
        irand++;
        if (irand > 5)
            irand = 1;
    }
    star_upset(stars);
    return stars;
}

// 打乱星星
void star_upset(_STARS *stars)
{
    int i, j, ri;
    STAR temp;
    int len = stars->width * stars->height;
    for (i = 0; i < len; i++)
    {
        temp = stars->star[i];
        ri = i + mrc_rand() % (len - i);
        stars->star[i] = stars->star[ri];
        stars->star[ri] = temp;
    }
}

int star_getType(_STARS *stars, int x, int y)
{
    return stars->star[y * stars->width + x].type;
}

// 消灭指定坐标内的星星

// 判断指定位置是否有星星
int star_isDraw(_STARS *stars, int x, int y)
{
    if (x >= 0 && x < stars->width && y >= 0 && y < stars->height && stars->star[y * stars->width + x].isDraw)
        return TRUE;
    return FALSE;
}

int star_isBeset(_STARS *stars, int x, int y)
{
    if (x >= 0 && x < stars->width && y >= 0 && y < stars->height && (!stars->star[y * stars->width + x].isBeset))
        return FALSE;
    return TRUE;
}

// 获取指定位置的星星
STAR *star_get(_STARS *stars, int x, int y)
{
    if (x >= 0 && x < stars->width && y >= 0 && y < stars->height)
        return &(stars->star[y * stars->width + x]);
    return NULL;
}

// 获取星星粒子播放时间
int star_getTime(_STARS *stars)
{
    return stars->li->time;
}

// 判断星星是否可以点击
int star_isImpact(_STARS *stars, int x, int y)
{
    int ix = x;
    int iy = y;
    int type;
    int isimpact = 0;
    // 判断当前是否有星星
    if (star_isDraw(stars, ix, iy))
    {
        type = star_getType(stars, ix, iy);

        // 判断四个方向
        if (star_isDraw(stars, ix, iy - 1) && star_getType(stars, ix, iy - 1) == type)
        {
            return TRUE;
        }
        if (star_isDraw(stars, ix, iy + 1) && star_getType(stars, ix, iy + 1) == type)
        {
            return TRUE;
        }
        if (star_isDraw(stars, ix - 1, iy) && star_getType(stars, ix - 1, iy) == type)
        {
            return TRUE;
        }
        if (star_isDraw(stars, ix + 1, iy) && star_getType(stars, ix + 1, iy) == type)
        {
            return TRUE;
        }
    }
    return FALSE;
}

// 设置待点击的星星(圈住星星)
int star_beset(_STARS *stars, int x, int y)
{
    int ix = x;
    int iy = y;
    int type;
    int isimpact = 0;
    // 判断当前是否有星星
    if (star_isDraw(stars, ix, iy))
    {
        type = star_getType(stars, ix, iy);
        stars->star[iy * stars->width + ix].isBeset = TRUE;

        // 判断四个方向
        if (!star_isBeset(stars, ix, iy - 1) && star_getType(stars, ix, iy - 1) == type)
        {
            star_beset(stars, ix, iy - 1);
            isimpact = 1;
        }
        if (!star_isBeset(stars, ix, iy + 1) && star_getType(stars, ix, iy + 1) == type)
        {
            star_beset(stars, ix, iy + 1);
            isimpact = 1;
        }
        if (!star_isBeset(stars, ix - 1, iy) && star_getType(stars, ix - 1, iy) == type)
        {
            star_beset(stars, ix - 1, iy);
            isimpact = 1;
        }
        if (!star_isBeset(stars, ix + 1, iy) && star_getType(stars, ix + 1, iy) == type)
        {
            star_beset(stars, ix + 1, iy);
            isimpact = 1;
        }
    }
    return 0;
}

// 将圈住的星星销毁
int star_burst(_STARS *stars)
{
    int ix, iy;
    int i = 0;
    STAR *star;
    char text[300];

    for (iy = 0; iy < stars->height; iy++)
    {
        for (ix = 0; ix < stars->width; ix++)
        {
            star = &(stars->star[iy * stars->width + ix]);
            if (star->isBeset)
            {
                star->isDraw = FALSE;
                star->isBeset = FALSE;
                li_setBitmap(stars->li, stars->par_bitmap[star->type]);
                i++;
            }
        }
    }
    // 分数增加
    stars->fen += i * i * 10;
    // 更新分数
    if (stars->view_fen)
    {
        mrc_sprintf(stars->view_fen, "分数：%d", stars->fen);
    }
    if (i > 0)
        li_start(stars->li);

    return 0;
}

// 点击一颗星星
int star_impact(_STARS *stars, int x, int y)
{
    int ix = x;
    int iy = y;
    int type;
    int isimpact = 0;
    // 判断当前是否有星星
    if (star_isDraw(stars, ix, iy))
    {
        type = star_getType(stars, ix, iy);
        stars->star[iy * stars->width + ix].isDraw = 0;

        // 判断四个方向
        if (star_isDraw(stars, ix, iy - 1) && star_getType(stars, ix, iy - 1) == type)
        {
            star_impact(stars, ix, iy - 1);
            isimpact = 1;
        }
        if (star_isDraw(stars, ix, iy + 1) && star_getType(stars, ix, iy + 1) == type)
        {
            star_impact(stars, ix, iy + 1);
            isimpact = 1;
        }
        if (star_isDraw(stars, ix - 1, iy) && star_getType(stars, ix - 1, iy) == type)
        {
            star_impact(stars, ix - 1, iy);
            isimpact = 1;
        }
        if (star_isDraw(stars, ix + 1, iy) && star_getType(stars, ix + 1, iy) == type)
        {
            star_impact(stars, ix + 1, iy);
            isimpact = 1;
        }
    }
    return 0;
}

// 星星event事件
int star_event(_STARS *stars, int type, int p1, int p2)
{
    // 判断点中了哪个星星
    if (type == MR_MOUSE_DOWN)
    {
        int ix = (p1 - stars->x) / stars->itemw;
        int iy = (p2 - stars->y) / stars->itemh;
        if (ix >= 0 && ix < stars->width && iy >= 0 && iy < stars->height)
        {
            // 将星星隐藏
            if (star_isImpact(stars, ix, iy))
                star_beset(stars, ix, iy);
            li_setxy(stars->li, p1, p2);
        }
    }
    if (type == MR_MOUSE_UP)
    {
        star_burst(stars);
    }

    return 0;
}

// 判断星星是否可以点击，(如果找不到可以点击的星星就进入下一关)
int star_isNoImpact(_STARS *stars)
{
    int ix, iy;
    for (iy = 0; iy < stars->height; iy++)
    {
        for (ix = 0; ix < stars->width; ix++)
        {
            if (star_isImpact(stars, ix, iy))
                return FALSE;
        }
    }
    return TRUE;
}

// 星星整体右移
int star_moveRight(_STARS *stars)
{
    int i, j, ix, iy = 0;
    int type = 0;
    STAR *temp;
    STAR *star;
    STAR *starleft;

    iy = stars->height - 1;
    for (ix = stars->width - 1; ix > 0; ix--)
    {
        // 检测一纵列都木有星星
        for (iy = 0; iy < stars->height; iy++)
        {
            temp = star_get(stars, ix, iy);

            if (temp->isDraw)
                type = 1;
        }
        if (type == 1)
        {
            type = 0;
            continue;
        }
        star = star_get(stars, ix, stars->height - 1);
        starleft = star_get(stars, ix - 1, stars->height - 1);

        // 右移
        if (!star->isDraw && starleft->isDraw)
        {
            for (i = 0; i < stars->height; i++)
            {
                star = star_get(stars, ix, i);
                starleft = star_get(stars, ix - 1, i);
                temp = star;

                star->y = starleft->y;
                star->x = starleft->x;
                star->type = starleft->type;
                star->isDraw = starleft->isDraw;
                starleft->isDraw = FALSE;
            }
        }
    }

    return 0;
}

// 星星运动事件
// 星星下落
int star_run(_STARS *stars)
{
    int len = stars->width * stars->height;
    int ix, iy;
    // 屏幕坐标
    int px, py, tx, ty;
    STAR *temp;
    STAR *star;
    STAR *starup;

    for (iy = stars->height - 1; iy >= 0; iy--)
    {
        for (ix = 0; ix < stars->width; ix++)
        {
            star = star_get(stars, ix, iy);
            starup = star_get(stars, ix, iy - 1);

            // 下落
            // 判断下方是否为空
            if (starup != NULL)
                if (!star->isDraw && starup->isDraw)
                {
                    // 交换位置
                    temp = star;

                    star->y = starup->y;
                    star->x = starup->x;
                    star->type = starup->type;
                    star->isDraw = starup->isDraw;
                    starup->isDraw = FALSE;
                }

            star = star_get(stars, ix, iy);
            px = stars->itemw * ix;
            py = stars->itemh * iy;
            tx = star->x;
            ty = star->y;
            if (tx < px)
                tx += 8;
            else if (tx > px)
                tx -= 8;
            if (ty < py)
                ty += 8;
            else if (ty > py)
                ty -= 8;
            star->x = tx;
            star->y = ty;
        }
    }
    star_moveRight(stars);
    li_run(stars->li);

    return 0;
}

// 显示星星
int star_draw(_STARS *stars)
{
    int ix, iy;
    STAR star;

    for (iy = 0; iy < stars->height; iy++)
    {
        for (ix = 0; ix < stars->width; ix++)
        {
            star = stars->star[iy * stars->width + ix];
            if (star.isBeset)
                mrc_bitmapShow(stars->bitmap[star.type], stars->x + star.x, stars->y + star.y, BM_GRAY, 0, 0, STAR_ITEM_WIDTH, STAR_ITEM_WIDTH);
            else if (star.isDraw)
                mrc_bitmapShow(stars->bitmap[star.type], stars->x + star.x, stars->y + star.y, BM_TRANSPARENT, 0, 0, STAR_ITEM_WIDTH, STAR_ITEM_WIDTH);
            
        }
    }
    li_draw(stars->li);
    return 0;
}

// 销毁星星
int star_mrc_free(_STARS *stars)
{
    int i;
    // 释放bitmap
    for (i = 0; i <= 6; i++)
    {
        mrc_bitmapLoad(i + STAR_BITMAP_ID, "*", 0, 0, STAR_ITEM_WIDTH, STAR_ITEM_WIDTH, STAR_ITEM_WIDTH);
    }
    // 释放star
    mrc_free(stars->star);
    li_free(stars->li);
    // 释放全部
    mrc_free(stars);

    return 0;
}
