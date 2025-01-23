#ifndef _OBJ_ECT_H_
#define _OBJ_ECT_H_

//��װsky��sprite����
#include "mrc_base.h"
extern int32 BBOX_DEBUG;
//��ײ��
typedef struct bbox
{
    int32 x;
    int32 y;
    int32 w;
    int32 h;
} Bbox;

//����
typedef struct sprite
{
    int32 bmp_id;
    int32 width;
    int32 height;
    int32 image_number;
    int32 start; //����һ��ͼƬ�������, Ĭ����0

    int32 posx; //���ĵ������ͼƬλ�ã������������ƹ���
    int32 posy;
    int32 bmmode;

    Bbox box; //�����posx��posy�ľ���
} Sprite;

//����
typedef struct object
{
    Sprite *sprite_index;

    int32 image_index;
    int32 image_speed;
    int32 _image_speed;
    int32 image_xscale; //object�任��ֻ֧��-1��ת
    int32 image_yscale;

    int32 state; //��������
    int32 state_next;
    Sprite *sprite_next; //�����л�����

    int16 x;
    int16 y;

    mrc_timerCB imageOverCb;
    int32 data[2]; //objectͨ������
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

//��ȡ��ײ����ʵλ��
extern Object *getObjRealBbox(Object *obj, Bbox *real);

extern Object *objPlaySpr(Object *obj, Sprite *spr, int32 state, Sprite *next_spr, int32 next_state);
extern Object *runObject(Object *obj);

extern Object *drawObject(Object *obj);
extern Object *drawObjectEx(Object *obj, int32 dx, int32 dy);

#endif