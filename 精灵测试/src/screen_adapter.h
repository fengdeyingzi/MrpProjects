
#ifndef __SCREEN_ADAPTER__
#define __SCREEN_ADAPTER__

#include <mrc_base.h>
#include "mrc_graphics.h"

//������Ļ���
void sc_setScreenSize(int32 width, int32 height);

//��ȡͼƬ ���Զ�����ͼƬ��ߵ�������Ļ�Ĵ�С
BITMAP_565* sc_readBitmapFromAssets(const char* filename);

//����һ��ͼƬ
void sc_drawBitmap(BITMAP_565 *bmp, int32 x, int32 y);

//��������
int32 sc_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode);

//���ƾ���
void sc_drawRect(int32 x,int32 y,int32 w,int32 h,uint32 color);

//����
void sc_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);

int32 sc_getX(int32 x);
int32 sc_getY(int32 y);

#endif