/*
** gal.c: implements of gal.h
** 
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** Create date: 2007-12-27 by wandonglin
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#include "gal.h"
#include "window.h"


struct _screendevice scrdev;


uint8 System_Theme;  /*系统主题模式*/
uint8 System_Font;   //字体字体


void GAL_Initialize (VOID)
{	
	mrc_getScreenInfo(&(scrdev.scrInfo));
}

void GAL_Terminate (VOID)
{
	SGL_TRACE("%s, %d: Terminate GAL\n", __FILE__, __LINE__);
}

void GAL_Rectangle(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel)
{
	int16 r = x + w - 1;
	int16 b = y + h - 1;
	mrc_drawLine((int16)x, (int16)y, r, (int16)y, PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));
	mrc_drawLine((int16)x, b, r, b, PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));
	mrc_drawLine((int16)x, (int16)y, (int16)x, b, PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));
	mrc_drawLine(r, (int16)y, r, b, PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));
}

void GAL_Rectangle2(GAL_GC gc, int x, int y, int w, int h, int line, Uint32 pixel)
{
	int i;
	for(i = 0; i < line; i++)
		GAL_Rectangle(gc, x+i, y+i, w-2*i, h-2*i, pixel);
}

void GAL_Rectangle3(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel)
{
	int16 r = x + w -1;
	int16 b = y + h -1;

	//2横线
	mrc_drawLine((int16)x+1, (int16)y, r-1, (int16)y, PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));
	mrc_drawLine((int16)x+1, b, r-1, b, PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));
	//2竖线
	mrc_drawLine((int16)x, (int16)y+1, (int16)x, b-1, PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));
	mrc_drawLine(r, (int16)y+1, r, b-1, PIXEL888RED(pixel), PIXEL888GREEN(pixel), PIXEL888BLUE(pixel));
}

void GAL_Rectangle4(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel)
{
}

//过渡色填充矩形区域
void GAL_FillBox2(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel)
{
	int i;
	mr_colourSt clr;
	mr_screenRectSt r;

	SET_COLOR(clr, pixel);
	r.x = x, r.y = y, r.w = w, r.h = h;

	//两横线
	mrc_drawLine(r.x+1, r.y, r.x+r.w-2, r.y, clr.r,clr.g,clr.b);
	mrc_drawLine(r.x+1, r.y+r.h-1, r.x+r.w-2, r.y+r.h-1, clr.r,clr.g,clr.b);
	//两竖线
	mrc_drawLine(r.x, r.y+1, r.x, r.y+r.h-2, clr.r,clr.g,clr.b);
	mrc_drawLine(r.x+r.w-1, r.y+1, r.x+r.w-1, r.y+r.h-2, clr.r,clr.g,clr.b); 
	r.x += 1, r.y += 1, r.w -= 2, r.h -= 2;
	//绘制按钮面
	for(i=0; i<r.h; i++)
	{
		mrc_drawLine(r.x, r.y+r.h-i-1, r.x+r.w-1, r.y+r.h-i-1, clr.r+4*(i+1),clr.g+3*(i+1),clr.b+2*(i+1));
	}
}
//过渡色填充矩形区域无边框
void GAL_FillBox3(GAL_GC gc, int x, int y, int w, int h, Uint32 pixel)
{
	int i;
	mr_colourSt clr;
	mr_screenRectSt r;

	SET_COLOR(clr, pixel);
	r.x = x, r.y = y, r.w = w, r.h = h;
	//过渡色
	for(i=0; i<r.h; i++)
	{
		mrc_drawLine(r.x, r.y+r.h-i-1, r.x+r.w-1, r.y+r.h-i-1, clr.r+4*(i+1),clr.g+3*(i+1),clr.b+2*(i+1));
	}
}

//过渡填充矩形区域1 (不带边框)
void FillShadeRange1(int16 x, int16 y, int16 w, int16 h, uint32 top, uint32 btm)
{
	int i;
	mr_colourSt topClr, clr;
	mr_screenRectSt r;

	SET_COLOR(topClr, top);
	clr.r = topClr.r - PIXEL888RED(btm), clr.g = topClr.g - PIXEL888GREEN(btm), clr.b = topClr.b - PIXEL888BLUE(btm);
	r.x = x, r.y = y, r.w = w, r.h = h;

	for(i=0; i<h; i++)
	{
		mrc_drawLine(r.x, r.y+i, r.x+r.w-1, r.y+i, 
			topClr.r-clr.r*i/h, topClr.g-clr.g*i/h, topClr.b-clr.b*i/h);
	}
}

//过渡填充矩形区域2 (带边框)
void FillShadeRange2(int16 x, int16 y, int16 w, int16 h, uint32 top, uint32 btm)
{
	mr_colourSt clr;
	mr_screenRectSt r;

	SET_COLOR(clr, top);
	r.x = x, r.y = y, r.w = w, r.h = h;

	//两横线
	mrc_drawLine(r.x+1, r.y, r.x+r.w-2, r.y, clr.r,clr.g,clr.b);
	mrc_drawLine(r.x+1, r.y+r.h-1, r.x+r.w-2, r.y+r.h-1, clr.r,clr.g,clr.b);
	//两竖线
	mrc_drawLine(r.x, r.y+1, r.x, r.y+r.h-2, clr.r,clr.g,clr.b);
	mrc_drawLine(r.x+r.w-1, r.y+1, r.x+r.w-1, r.y+r.h-2, clr.r,clr.g,clr.b); 

	r.x += 1, r.y += 1, r.w -= 2, r.h -= 2;
	FillShadeRange1(r.x, r.y, r.w, r.h, top, btm);
}


void GAL_DrawLine(POINT start, POINT end, mr_colourSt color)
{
	mrc_drawLine(start.x, start.y, end.x, end.y, color.r,color.g,color.b);
}

void GAL_DrawRoundRange(int16 x, int16 y, int16 w, int16 h, uint32 pixel)
{
	mr_colourSt color;
	POINT topL, topR, btmL, btmR;

	color.r = PIXEL888RED(pixel);
	color.g = PIXEL888GREEN(pixel);
	color.b = PIXEL888BLUE(pixel);

	topL.x = x, topL.y = y; topR.x = x + w - 1, topR.y = y;
	btmL.x = x, btmL.y = y + h - 1; btmR.x = x + w - 1, btmR.y = y + h - 1;

	GAL_DrawLine(topL, topR, color);
	GAL_DrawLine(btmL, btmR, color);
	GAL_DrawLine(topL, btmL, color);
	GAL_DrawLine(topR, btmR, color);

	mrc_drawPointEx(topL.x+1, topL.y+1, color.r,color.g,color.b);
	mrc_drawPointEx(topL.x+2, topL.y+1, color.r,color.g,color.b);
	mrc_drawPointEx(topL.x+1, topL.y+2, color.r,color.g,color.b);

	mrc_drawPointEx(topR.x-1, topR.y+1, color.r,color.g,color.b);
	mrc_drawPointEx(topR.x-2, topR.y+1, color.r,color.g,color.b);
	mrc_drawPointEx(topR.x-1, topR.y+2, color.r,color.g,color.b);

	mrc_drawPointEx(btmL.x+1, btmL.y-1, color.r,color.g,color.b);
	mrc_drawPointEx(btmL.x+2, btmL.y-1, color.r,color.g,color.b);
	mrc_drawPointEx(btmL.x+1, btmL.y-2, color.r,color.g,color.b);

	mrc_drawPointEx(btmR.x-1, btmR.y-1, color.r,color.g,color.b);
	mrc_drawPointEx(btmR.x-2, btmR.y-1, color.r,color.g,color.b);
	mrc_drawPointEx(btmR.x-1, btmR.y-2, color.r,color.g,color.b);
}

void GAL_FillRoundRrct(int x, int y, int w, int h, uint32 pixel)
{
	int i;
	uint8 r, g, b;

	r = PIXEL888RED(pixel);
	g = PIXEL888GREEN(pixel);
	b = PIXEL888BLUE(pixel);

	x += 1, y += 1, w -= 2, h -= 2; 
	mrc_drawRect((int16)x, (int16)y, (int16)w, (int16)h, r, g, b);
	for(i=0; i<1; i++)
	{
		GAL_DrawHLine(PHYSICALGC, x+i+1, y-(i+1), w-2*(i+1), pixel);
		GAL_DrawHLine(PHYSICALGC, x+i+1, y+h+i, w-2*(i+1), pixel);
	}
	for(i=0; i<1; i++)
	{
		GAL_DrawVLine(PHYSICALGC, x-(i+1), y+(i+1), h-2*(i+1), pixel);
		GAL_DrawVLine(PHYSICALGC, x+w+i, y+(i+1), h-2*(i+1), pixel);
	}
}

//屏幕中央显示文字函数
void GAL_DrawTextMidScn(char* text, uint32 pixel, int is_unicode) 
{ 
    int16 x,y; 
    int32 w,h;
	uint8 r, g, b;

	r = PIXEL888RED(pixel);
	g = PIXEL888GREEN(pixel);
	b = PIXEL888BLUE(pixel);

    fontwh(text, is_unicode, MR_FONT_MEDIUM, &w, &h); 
    x = (int16)(GAL_Width(PHYSICALGC) - w )/2; 
    y =  (int16)(GAL_Height(PHYSICALGC)- h )/2; 
    gbsez(text, x, y, r, g, b, is_unicode, MR_FONT_MEDIUM); 
}

//封装文本显示函数
VOID GAL_DrawTextEx(PSTR pText, mr_screenRectSt r, uint32 pixel, int flag, uint16 font)
{
	mr_colourSt color;

	color.r = PIXEL888RED(pixel);
	color.g = PIXEL888GREEN(pixel);
	color.b = PIXEL888BLUE(pixel);
	gbsez2(pText, r.x, r.y, r, color, flag, font);
}

VOID GAL_DrawTextRight(PSTR pText, mr_screenRectSt rt, uint32 pixel, BOOL isUnicode, uint16 font)
{
	int32 w, h;
	uint8 r, g, b;

	r = PIXEL888RED(pixel);
	g = PIXEL888GREEN(pixel);
	b = PIXEL888BLUE(pixel);
	fontwh(pText, isUnicode, font, &w, &h);
	gbsez(pText, (int16)(rt.x + (rt.w - w - 2)), (int16)(rt.y + (rt.h - h)/2), r, g, b, isUnicode, font);
}

VOID GAL_DrawTextLeft(PSTR pText, mr_screenRectSt rt, uint32 pixel, BOOL isUnicode, uint16 font)
{
	int32 w, h;
	uint8 r, g, b;

	r = PIXEL888RED(pixel);
	g = PIXEL888GREEN(pixel);
	b = PIXEL888BLUE(pixel);
	fontwh(pText, isUnicode, font, &w, &h);
	gbsez(pText, (int16)(rt.x + 2), (int16)(rt.y + (rt.h - h)/2), r, g, b, isUnicode, font);
}

VOID GAL_DrawTextA(PSTR pText, int x, int y, uint32 pixel, BOOL isUnicode, uint16 font)
{
	int32 w, h;
	uint8 r, g, b;

	r = PIXEL888RED(pixel);
	g = PIXEL888GREEN(pixel);
	b = PIXEL888BLUE(pixel);

	fontwh(pText, isUnicode, font, &w, &h);
	gbsez(pText, (int16)(x), (int16)(y), r, g, b, isUnicode, font);
	gbsez(pText, (int16)(x+1), (int16)(y), r+0, g+0, b+0, isUnicode, font);
}