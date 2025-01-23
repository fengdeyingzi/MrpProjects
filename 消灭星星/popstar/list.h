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
    float x; // ����
    float y; // ����
    int r;
    float speed_x; // �����˶�ֵ
    float speed_y; // �����˶�ֵ
    int hp;        // ����
    int color;
    int alpha; // ͸����
} _POINT;

typedef struct
{
    _POINT *point;
    int32 bitmap; // ����ͼƬ
    int x;
    int y;           // ���ӷ����� ����
    int r;           // ���
    int v;           // �ٶ�
    int min_v;       // ��С�ٶ�
    int size;        // ��������
    int birth;       // ������
    int speed;       // ���ӵݼ���
    int hp;          // ����
    int color;       // ��ɫ rgbֵ
    int coreColor;   // ������ɫ
    int isRandColor; // �Ƿ���������ɫ
    int isDirection; // �Ƿ��з���
    int direction;   // ����
    int isFirst;     // �Ƿ�ֻ����һ��
    int time;        // ��������ʱ��
} _LI;

// ��������
extern _LI *li_create(int x, int y);
extern int li_start(_LI *li);
// �����˶�
extern int li_run(_LI *li);
// ���������ٶ�
extern int li_setV(_LI *li, int v);
// ��������ֻ����һ��
extern int li_setFirst(_LI *li);
// ��������ͼƬ
extern int li_setBitmap(_LI *li, int32 bitmap);
// ��������͸���ȵݼ�����
extern int li_setSpeed(_LI *li, int speed);
// ������������
extern int li_setHp(_LI *li, int hp);
// ������������
extern int li_setSize(_LI *li, int size);
// ����������ɫ
extern int li_setColor(_LI *li, int color);
// ������������
extern int li_setxy(_LI *li, int x, int y);
// ��ʾ����
extern int li_draw(_LI *li);
// ɾ������
extern int li_free(_LI *li);

#endif
