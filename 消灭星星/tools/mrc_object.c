//封装sky的sprite功能
#include "mrc_object.h"
int32 BBOX_DEBUG;

Sprite *initSprite(Sprite *spr, int32 bmp_id, int32 width, int32 height, int32 number)
{
    mrc_spriteSet(bmp_id, height);
    spr->bmp_id = bmp_id;
    spr->height = height;
    spr->width = width;
    spr->image_number = number;

    spr->bmmode = BM_TRANSPARENT;
    spr->start = 0;

    spr->posx = 0;
    spr->posy = 0;

    setSprBbox(spr, 0, 0, 0, 0);
    return spr;
}

Sprite *setSprPos(Sprite *spr, int32 x, int32 y)
{
    spr->posx = x;
    spr->posy = y;
    return spr;
}

Sprite *setSprBbox(Sprite *spr, int32 x, int32 y, int32 w, int32 h)
{
    spr->box.x = x;
    spr->box.y = y;
    spr->box.w = w;
    spr->box.h = h;
    return spr;
}

Object *initObject(Object *obj, Sprite *spr, int32 index, int16 x, int16 y)
{
    obj->sprite_index = spr;

    obj->sprite_next = NULL;
    obj->state_next = -1;

    obj->image_index = index;
    obj->image_speed = 5;
    obj->_image_speed = 0;

    obj->image_xscale = 1;
    obj->image_yscale = 1;

    obj->x = x;
    obj->y = y;

    obj->data[0] = 1;
    obj->data[1] = 1;
    return obj;
}

Object *setObjSpr(Object *obj, Sprite *spr)
{
    obj->sprite_index = spr;
    obj->image_index %= obj->sprite_index->image_number;
    return obj;
}

Object *setObjImageOverCb(Object *obj, mrc_timerCB call)
{
    obj->imageOverCb = call;
    return obj;
}

Object *setObjPos(Object *obj, int16 x, int16 y)
{
    obj->x = x;
    obj->y = y;
    return obj;
}

Object *objPlaySpr(Object *obj, Sprite *spr, int32 state, Sprite *next_spr, int32 next_state)
{
    obj->state = state;
    setObjSpr(obj, spr);

    obj->sprite_next = next_spr;
    obj->state_next = next_state;
    return obj;
}

#define IMAGE_SPEED 10
//image_speed 越大速度越快
Object *runObject(Object *obj)
{
    obj->_image_speed++;

    if (obj->image_speed == 0)
    {
        obj->_image_speed = 0;
        return obj;
    }

    if ((IMAGE_SPEED - obj->image_speed) / obj->_image_speed <= 1)
    {
        obj->_image_speed = 0;
        obj->image_index++;

        obj->image_index %= obj->sprite_index->image_number;

        if (obj->imageOverCb != NULL)
            if (obj->image_index == 0)
            {
                obj->imageOverCb((int32)obj);
                if (obj->sprite_next != NULL)
                {
                    obj->sprite_index = obj->sprite_next;
                    obj->state = obj->state_next;

                    obj->sprite_next = NULL;
                    obj->state_next = -1;
                }
            }
    }
    return obj;
}

//获取碰撞盒真实位置
Object *getObjRealBbox(Object *obj, Bbox *real)
{
    int32 boxx, boxy;
    Bbox *box = &(obj->sprite_index->box);

    //碰撞盒也要变换
    boxx = box->x;
    boxy = box->y;
    if (obj->image_xscale == -1)
        boxx = -(box->w + box->x);
    if (obj->image_yscale == -1)
        boxy = -(box->h + box->y);

    real->x = obj->x + boxx;
    real->y = obj->y + boxy;
    real->w = box->w;
    real->h = box->h;
    return obj;
}

Sprite *drawSpriteEx(Sprite *spr, int32 index, int32 x, int32 y, int32 image_xscale, int32 image_yscale)
{
    int32 xx, yy, posx, posy, boxx, boxy;
    Bbox *box = &spr->box;

    posx = spr->posx;
    posy = spr->posy;
    xx = x - posx;
    yy = y - posy;

    //根据image_xcale和image_yscale
    //变换sprite的pos坐标
    if (image_xscale == -1)
    {
        posx = (spr->width - posx);
        xx = x - posx;
    }
    if (image_yscale == -1)
    {
        posy = (spr->height - posy);
        yy = y - posy;
    }
    mrc_spriteDrawEx(spr->bmp_id, spr->start + index, xx, yy, image_xscale * 256, 0, 0, image_yscale * 256);

    //碰撞盒也要变换
    boxx = box->x;
    boxy = box->y;
    if (image_xscale == -1)
        boxx = -(box->w + box->x);
    if (image_yscale == -1)
        boxy = -(box->h + box->y);
    if (BBOX_DEBUG)
    {
        mrc_drawLine(x + boxx, y + boxy, x + boxx, y + boxy + box->h, 0xff, 0, 0);
        mrc_drawLine(x + boxx, y + boxy, x + boxx + box->w, y + boxy, 0xff, 0, 0);
        mrc_drawLine(x + boxx + box->w, y + boxy, x + boxx + box->w, y + boxy + box->h, 0xff, 0, 0);
        mrc_drawLine(x + boxx, y + boxy + box->h, x + boxx + box->w, y + boxy + box->h, 0xff, 0, 0);
        //mrc_drawRect(x + boxx, y + boxy, box->w, box->h, 0xff, 0, 0);
    }
    return spr;
}

//无视image_xcale和image_yscale
Sprite *drawSprite(Sprite *spr, int32 index, int32 x, int32 y)
{
    int32 xx, yy;
    Bbox *box = &spr->box;

    xx = x - spr->posx;
    yy = y - spr->posy;

    mrc_spriteDraw(spr->bmp_id, spr->start + index, xx, yy, spr->bmmode);
    if (BBOX_DEBUG)
    {
        mrc_drawLine(x + box->x, y + box->y, x + box->x, y + box->y + box->h, 0xff, 0, 0);
        mrc_drawLine(x + box->x, y + box->y, x + box->x + box->w, y + box->y, 0xff, 0, 0);
        mrc_drawLine(x + box->x + box->w, y + box->y, x + box->x + box->w, y + box->y + box->h, 0xff, 0, 0);
        mrc_drawLine(x + box->x, y + box->y + box->h, x + box->x + box->w, y + box->y + box->h, 0xff, 0, 0);
        //mrc_drawRect(x + box->x, y + box->y, box->w, box->h, 0xff, 0, 0);
    }
    return spr;
}

Object *drawObject(Object *obj)
{
    drawSpriteEx(obj->sprite_index, obj->image_index, obj->x, obj->y, obj->image_xscale, obj->image_yscale);
    return obj;
}

Object *drawObjectEx(Object *obj, int32 dx, int32 dy)
{
    drawSpriteEx(obj->sprite_index, obj->image_index, obj->x + dx, obj->y + dy, obj->image_xscale, obj->image_yscale);
    return obj;
}
