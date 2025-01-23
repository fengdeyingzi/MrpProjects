#ifndef _STAR_H_
#define _STAR_H_

#include "list.h"

#define STAR_ITEM_WIDTH 24
#define STAR_BITMAP_ID 0

typedef struct
{
    int x;       // x����
    int y;       //
    int type;    // ����
    int isDraw;  // �Ƿ���ʾ
    int isBeset; // �Ƿ�Ȧס
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

// ������������
// ������x���� y���� ��Ļ��� ��Ļ�߶�
extern _STARS *star_create_withscr(int x, int y, int width, int height);

// ������������
// ������x���� y���� ��� �߶�
extern _STARS *star_create(int x, int y, int width, int height);

// ��ʾ����
extern int star_draw(_STARS *stars);

// ����event�¼�
extern int star_event(_STARS *stars, int type, int p1, int p2);

#endif
