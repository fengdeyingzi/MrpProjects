#ifndef _OBJ_ECT_H_
#define _OBJ_ECT_H_

//封装sky的sprite功能
#include "mrc_base.h"
extern int32 BBOX_DEBUG;
//碰撞盒
typedef struct bbox
{
    int32 x;
    int32 y;
    int32 w;
    int32 h;
} Bbox;

//精灵
typedef struct sprite
{
    int32 bmp_id;
    int32 width;
    int32 height;
    int32 image_number;
    int32 start; //可以一个图片多个动作, 默认是0

    int32 posx; //中心点相对于图片位置，用来升级绘制功能
    int32 posy;
    int32 bmmode;

    Bbox box; //相对于posx和posy的矩形
} Sprite;

//对象
typedef struct object
{
    Sprite *sprite_index;

    int32 image_index;
    int32 image_speed;
    int32 _image_speed;
    int32 image_xscale; //object变换，只支持-1翻转
    int32 image_yscale;

    int32 state; //辅助变量
    int32 state_next;
    Sprite *sprite_next; //用于切换动画

    int16 x;
    int16 y;

    mrc_timerCB imageOverCb;
    int32 data[2]; //object通用数据
} Object;

typedef struct camera
{
    int32 x;
    int32 y;
    int32 w;
    int32 h;
}Camera;

extern Sprite *initSprite(Sprite *spr, int32 bmp_id, int32 width, int32 height, int32 number);
extern Sprite *setSprPos(Sprite *spr, int32 x, int32 y);
extern Sprite *setSprBbox(Sprite *spr, int32 x, int32 y, int32 w, int32 h);
extern Sprite *drawSprite(Sprite *spr, int32 index, int32 x, int32 y);

extern Object *initObject(Object *obj, Sprite *spr, int32 index, int16 x, int16 y);
extern Object *setObjSpr(Object *obj, Sprite *spr);
extern Object *setObjImageOverCb(Object *obj, mrc_timerCB call);
extern Object *setObjPos(Object *obj, int16 x, int16 y);

//获取碰撞盒真实位置
extern Object *getObjRealBbox(Object *obj, Bbox *real);

extern Object *objPlaySpr(Object *obj, Sprite *spr, int32 state, Sprite *next_spr, int32 next_state);
extern Object *runObject(Object *obj);

extern Object *drawObject(Object *obj);
extern Object *drawObjectEx(Object *obj, int32 dx, int32 dy);

#endif