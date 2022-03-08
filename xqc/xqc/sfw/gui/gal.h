/*
** gal.h: graphic absract layer header file.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#ifndef _SGL_GAL_H
#define _SGL_GAL_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _screendevice
{
	mr_screeninfo scrInfo;
}* GAL_GC;

/*ϵͳ����*/
enum SysThem
{
	THEME_CLASSIC = 0, //0.����ģʽ
	THEME_NIGHT        //1.ҹ��ģʽ
};


extern struct _screendevice scrdev;
extern uint8 System_Theme;  //ϵͳ����

/**
 * \brief Normal colors.
 */
#define COLOR_transparent	0x00000000
#define COLOR_darkred		0x00800000
#define COLOR_darkgreen		0x00008000
#define COLOR_darkyellow	0x00808000
#define COLOR_darkblue		0x00000080
#define COLOR_darkmagenta	0x00800080
#define COLOR_darkcyan		0x00008080
#define COLOR_lightgray		0x00c0c0c0
#define COLOR_darkgray		0x00808080
#define COLOR_red			0x00ff0000
#define COLOR_green			0x0000ff00
#define COLOR_yellow		0x00ffff00
#define COLOR_blue			0x000000f0
#define COLOR_magenta		0x008080ff
#define COLOR_cyan			0x0000ffff
#define COLOR_lightwhite	0x00ffffff
#define COLOR_black			0x00000000

//�Զ�����ɫ
#define COLOR_MAIN_BG		0x00333333	//�����ڱ���
#define COLOR_Listface	    0x00171a1d   //�б�Ƥ��
#define COLOR_FONT_night    0x004A6D8C  //QQ�����ҹ������ɫ
#define COLOR_Combox_bg     0x00202020  //combox����
#define COLOR_user3        0x008000ff  //����������ɫ


/**
 * \brief The phisical graphic context handle.
 */
#define PHYSICALGC	(&scrdev)

/**
 * \brief Intialize the graphic abstract layer.
 */
void GAL_Initialize (VOID);

/**
 * \brief Terminate the graphic abstract layer.
 */
void GAL_Terminate (VOID);

/**
 * \brief Get the bytes per pixel.
 */
#define GAL_BytesPerPixel(gc) \
	DIV((gc)->scrInfo.bit + 7 , 8)

/**
 * \brief Get the bits per pixel.
 */
#define GAL_BitsPerPixel(gc) \
	((gc)->scrInfo.bit)

/**
 * \brief Get the screen width.
 * ��ȡ��Ļ��
 */
#define GAL_Width(gc) \
	((gc)->scrInfo.width)

/**
 * \brief Get the screen height.
 * ��ȡ��Ļ��
 */
#define GAL_Height(gc) \
	((gc)->scrInfo.height)

/**
 * \brief Get supported colors.
 * �õ�֧�ֵ���ɫ
 */
#define GAL_Colors(gc) \
	(((gc)->scrInfo.bit >= 24)? (1 << 24): (1 << (gc)->scrInfo.bit))

/**
 * \brief Fill the box with a specific color.
 * ��ָ����ɫ���հ���
 */
#define GAL_FillBox(gc, x, y, w, h, pixel) \
	mrc_drawRect((int16)(x), (int16)(y), (int16)(w), (int16)(h), PIXEL888RED((pixel)), PIXEL888GREEN((pixel)), PIXEL888BLUE((pixel)))

/**
 * \brief Draw a horizontal line.
 * ��ˮƽ��
 */
#define GAL_DrawHLine(gc, x, y, w, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)((x)+(w)-1), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * \brief Draw a vertical line.
 * ����ֱ��
 */
#define GAL_DrawVLine(gc, x, y, h, pixel) \
	mrc_drawLine((int16)(x), (int16)(y), (int16)(x), (int16)((y)+(h)-1), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * \brief Draw a pixel.
 * ��һ�����ص�
 */
#define GAL_DrawPixel(gc, x, y, pixel) \
	mrc_drawPointEx((int16)(x), (int16)(y), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * \brief Daw a normal line.
 * ������
 */
#define GAL_Line(gc, x1, y1, x2, y2, pixel) \
	mrc_drawLine((int16)(x1), (int16)(y1), (int16)(x2), (int16)(y2), PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel))

/**
 * \brief Draw a rectangle.
 * ������
 */
void GAL_Rectangle(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel);

/**
 * \brief Draw a rectangle with a specific width.
 * ��һ���ض���ȵľ���
 */
void GAL_Rectangle2(GAL_GC gc, int x, int y, int w, int h, int line, Uint32 pixel);

//4�����㲻����
void GAL_Rectangle3(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel);

//����ɫ����������
void GAL_FillBox2(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel);

//����ɫ�����������ޱ߿�
void GAL_FillBox3(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel);

//��������������1 (�����߿�)
void FillShadeRange1(int16 x, int16 y, int16 w, int16 h, uint32 top, uint32 btm);

//��������������2 (���߿�)
void FillShadeRange2(int16 x, int16 y, int16 w, int16 h, uint32 top, uint32 btm);

/**
 * \brief Draw a rectangle with a specific width.
 * ��һ����Բ�Ǿ���
 */
void GAL_DrawRoundRange(int16 x, int16 y, int16 w, int16 h, uint32 pixel);

/**
 * \brief Draw a rectangle with a specific width.
 * ��һ����Բ�Ǿ���
 */
void GAL_FillRoundRrct(int x, int y, int w, int h, uint32 pixel);

/**
 * \brief Flush all the screen.
 * ˢ��������Ļ
 */
#define GAL_Flush(gc) \
	mrc_refreshScreen(0, 0, (uint16)(gc->scrInfo.width), (uint16)(gc->scrInfo.height))

/**
 * \brief Flush a region of the screen.
 * ˢ��ָ����Χ��Ļ
 */
#define GAL_FlushRegion(gc,  x,  y,  w, h) \
	mrc_refreshScreen((int16)(x), (int16)(y), (uint16)(w), (uint16)(h))

/**
 * \brief clear all the screen.
 * ��ָ����ɫ���������Ļ
 */
#define GAL_ClearScreen(pixel) \
	mrc_clearScreen(PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));

/**
 * \brief mrc_drawTextEx �ķ�װ
 */
VOID GAL_DrawTextEx(PSTR pText, mr_screenRectSt r, uint32 pixel, int flag, uint16 font);

/**
 * \brief mrc_drawText�ķ�װ
 */
VOID GAL_DrawTextRight(PSTR pText, mr_screenRectSt r, uint32 pixel, BOOL isUnicode, uint16 font);

/**
 * \brief mrc_drawText�ķ�װ
 * \��������ʾ����
 */
VOID GAL_DrawTextA(PSTR pText, int x, int y, uint32 pixel, BOOL isUnicode, uint16 font);


/**
 * \��Ļ������ʾ����
 */
void GAL_DrawTextMidScn(char* text, uint32 pixel, int is_unicode);

/**
 * ���һ�������Ƿ���ָ��������
 */
#define POINT_IS_INRECT(pr, x, y) \
	(x >= pr.left && x < pr.left + pr.width \
	&& y >= pr.top && y < pr.top + pr.height)

/**
 * ��������ֵ 
 * mr_screenRectSt r;
 */
#define SET_RECT(r, ax, ay, aw, ah)	\
	do{	\
		r.x = (uint16)(ax);	\
		r.y = (uint16)(ay); 	\
		r.w = (uint16)(aw); 	\
		r.h = (uint16)(ah);	\
	}while(0)

/* 
 * ������ɫֵ 
 * mr_colourSt color;
 */
#define SET_COLOR(color, pixel)	\
	do{	\
		color.r = PIXEL888RED(pixel);	\
		color.g = PIXEL888GREEN(pixel);	\
		color.b = PIXEL888BLUE(pixel);	\
	}while(0)


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _SGL_GAL_H */

