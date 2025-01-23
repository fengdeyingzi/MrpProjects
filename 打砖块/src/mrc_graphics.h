#ifndef _MRC_GRAPHICS_H_
#define _MRC_GRAPHICS_H_


#include "xl_bmp.h"
enum
{
_JPG=0,
_PNG=1,
_BMP16=2
};


typedef struct
{
uint32 width; //���
uint32 height; //�߶�
uint32 stride;
int32 format; //��ʽ
uint32 flags; // 0 for now
void *ptr; //����
} BITMAPINFO;

typedef struct{
    int16 A; // A, B, C, and D are fixed point values with an 8-bit integer part
    int16 B; // and an 8-bit fractional part.
    int16 C;
    int16 D;
    uint16 rop;
}mr_transMatrixSt;

enum
{
	SHADE_UPDOWN,		//���ϵ���
	SHADE_LEFTRIGHT,	//������
	SHADE_DOWNUP,		//���µ���
	SHADE_RIGHTLEFT		//���ҵ���
};
//���������ɫ
uint16 blendColor(uint16 dstColor, uint16 srcColor, int alpha);
//���ڴ濨��ȡbitmap
extern BITMAP_565* readBitmap565(char *filename);
//����һ��bitmap
BITMAP_565 *createBitmap565(int width, int height);
//����bitmapָ������
extern int32 drawBitmap565Flip(BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);
//����bitmap
extern void drawBitmap565(BITMAP_565 *b,int32 x,int32 y);
//�ͷ�bitmap
extern int32 bitmap565Free(BITMAP_565 *b);
//��mrp�ж�ȡbitmap
extern BITMAP_565 *readBitmap565FromAssets(char *filename);
// ������
void gl_drawRect(int32 x,int32 y,int32 w,int32 h,uint32 color);
//����
void gl_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);
//���ƿ���������
void gl_drawHollowTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
//��Բ
void gl_drawCir(int32 x,int32 y,int32 r,uint32 color);
//������ת�Ŀ��ľ���
void mrc_drawRotatedRect(int16 centerX, int16 centerY, int16 width, int16 height, int32 bx, int32 by, float angle, uint8 r, uint8 g, uint8 b);
//���ƽ������
void drawShadeRect(int32 x, int32 y, int32 w, int32 h, uint32 colorA, uint32 colorB, int mode);
//����������
void gl_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
//��չ����bitmap
void drawBitmap565Ex(BITMAP_565* bmp, int32 x,int32 y,int32 w,int32 h, int32 tx, int32 ty,int32 tw,int32 th);
//��bitmap����,����һ���µ�BITMAP
BITMAP_565 *createBitmapFromBitmap(BITMAP_565 *bmp, int32 width, int32 height);
//��ȡbitmap��Ϣ
int32 bitmap565getInfo(BITMAP_565* bmp, BITMAPINFO *info);
//ͼƬ��ת����
void drawBitmap565Rotate(BITMAP_565 *b, int32 scrx, int32 scry, int32 bx, int32 by, int32 r);
//��bufͼƬָ��������Ƶ�di��
void drawBitmap565Old(BITMAP_565 *di, BITMAP_565 *buf, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);
//����bitmap���ļ�
void saveBitmap(BITMAP_565 *bmp, const char *filename);
//͸���Ȼ���bitmap
void drawBitmapAlpha(BITMAP_565 *b, int32 x, int32 y, int32 alpha);
//��ת/��תbitmap
void bitmap565Rotate(BITMAP_565 *b, int type, int flip);
//��bitmap�ڻ��ƿ���������
void bmp_drawHollowTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
//��bitmap�ڻ���������
void bmp_drawTriangle(BITMAP_565 *bmp, int x1, int y1, int x2, int y2, int x3, int y3, uint32 color);
// ��bitmap�ڻ��ƿ���Բ
void bmp_drawHollowCircle(BITMAP_565 *bmp, int32 centerX, int32 centerY, int32 radius, uint32 color);
// ��bitmap�ڻ���Բ��
void bmp_drawCircle(BITMAP_565 *bmp, int32 cx, int32 cy, int32 radius, uint32 color);
// ��bitmap�ڻ����߶�
void bmp_drawLine(BITMAP_565 *bmp, int32 x1, int32 y1, int32 x2, int32 y2, uint32 color);
// ��bitmap�ڻ��Ƶ�
void bmp_drawPoint(BITMAP_565 *bmp, int32 x, int32 y, uint32 color);
// ��bitmap�ڻ��ƾ���
void bmp_drawRect(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, uint32 color);
#endif