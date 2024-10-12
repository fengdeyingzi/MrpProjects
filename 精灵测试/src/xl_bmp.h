#ifndef _XL_BMP_H_
#define _XL_BMP_H_

/*
��װ��ȡbmpͼƬ
���Ӱ��
����mrp�������ὫbmpͼƬ���д�������mrp�ڲ���bmpͼƬ�����÷�bmp��׺��ֹ�����룬���罫bmp�ĳ�jpg
*/
#ifdef TCC
#include "base.h"
enum{
BM_OR,         //SRC .OR. DST*   ��͸��Ч��
   BM_XOR,        //SRC .XOR. DST*
   BM_COPY,       //DST = SRC*      ����
   BM_NOT,        //DST = (!SRC)*
   BM_MERGENOT,   //DST .OR. (!SRC)
   BM_ANDNOT,     //DST .AND. (!SRC)
   BM_TRANSPARENT,//͸��ɫ����ʾ��ͼƬ�ĵ�һ�����أ����Ͻǵ����أ���͸��ɫ
   BM_AND,        //DST AND SRC
   BM_GRAY,        //�ҶȻ�ͼ�� �൱��BM_TRANSPARENT���ҶȻ�ͼ DST�Ҷ� �� 30%R + 59%G + 11%BͼƬ���
   BM_REVERSE     //�����ͼ���൱��BM_TRANSPARENT�������ͼ��V1939��
   };
   #define mrc_bitmapShowFlip bmpshowflip
   #define mrc_bitmapShowExTrans bmpshowex
#else
#include <mrc_base.h>
#endif



typedef struct {
 uint16* bitmap;
 int width; //
 int height;
 int color_bit; //��ɫλ�� Ĭ��16
 int transcolor; //͸��ɫ Ĭ��0xffff
 int mode; //����ģʽ Ĭ��BM_COPY
 int32 buflen; //���������
 int32 *memcache; //memcpy��ͼ�Ż����� ���dst src count(��12�ֽ�)
 int32 memlen; //memcpy��ͼ�Ż�����
} BITMAP_565;


BITMAP_565* bmp_create(uint16 *bitmap, int width, int height, int transcolor);
int get_int(char *buf,int ptr);
BITMAP_565* bmp_read(void *buf, int len);
BITMAP_565* bma_read(void *buf, int len);
void bmp_settranscolor(BITMAP_565* bmp, int color);
void bmp_draw(BITMAP_565* bmp, int x,int y);
void bmp_drawflip(BITMAP_565* bmp, int x,int y,int w,int h,int tx,int ty);
void bmp_free(BITMAP_565* bmp);
void* mr_malloc(int32 len);



#endif