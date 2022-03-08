#include "gal.h"
#include "smp.h"
#include "smp_msgbox.h"
#include "bmp.h"


VOID SMP_DrawRadioBox(int x, int y, BOOL checked)
{
	SMP_DrawCheckBox(x, y, checked);		
}


VOID SMP_DrawCheckBox(int x, int y, BOOL checked)
{
	GAL_Rectangle(PHYSICALGC, x, y, SMP_CHECKBOX_SIZE, SMP_CHECKBOX_SIZE, COLOR_border);	

	if(checked)
	{
		x += DIV(SMP_CHECKBOX_SIZE-7, 2);
		GAL_Line(PHYSICALGC, x, y + DIV(SMP_CHECKBOX_SIZE, 2), x + 3, y + DIV(SMP_CHECKBOX_SIZE*2, 3), COLOR_focus);
		GAL_Line(PHYSICALGC, x + 3, y + DIV(SMP_CHECKBOX_SIZE*2, 3), x + 7, y + DIV(SMP_CHECKBOX_SIZE, 3), COLOR_focus);
	}
}


VOID SMP_DrawMsgBoxFrame(int x, int y, int w, int h)
{
	GAL_Rectangle(PHYSICALGC, x, y, w, h, 0x3f62a2);
	GAL_Rectangle(PHYSICALGC, x+1, y+1, w-2, h-2, 0x8DBEFF); 
	GAL_FillBox(PHYSICALGC, x+2, y+2, w-4, SMP_MSGBOX_TBHEIHT+SMP_MSGBOX_MARGIN - 1, 0x5984d2);
	GAL_DrawHLine(PHYSICALGC, x+3, y + SMP_MSGBOX_TBHEIHT+SMP_MSGBOX_MARGIN - 1, w-6, 0x3959a2);
	GAL_Rectangle(PHYSICALGC, x+2, y+SMP_MSGBOX_TBHEIHT+SMP_MSGBOX_MARGIN, w-4, h-SMP_MSGBOX_TBHEIHT-SMP_MSGBOX_MARGIN-2, 0x3f62a2); 
}


VOID SMP_DrawWndHeader(int x, int y, int w, int h, Uint32 bgcolor, Uint32 fgcolor, DWORD bmpID, PCWSTR str)
{
	int wBmp = 0, hBmp = 0;
	int32 wText, hText;
	mr_screenRectSt rect;
	mr_colourSt color={0,};
	HBITMAP bmp;

	//draw the background
	GAL_FillBox(PHYSICALGC, x, y, w, h, bgcolor);

	//draw the bitmap
	wBmp = 0;
	if(bmpID < RES_BITMAP_COUNT)
	{
		bmp = SGL_LoadBitmap(bmpID, &wBmp, &hBmp);
		mrc_bitmapShowEx(bmp, SMP_ITEM_CONTENT_MARGIN, (int16)DIV(h - hBmp, 2), (int16)wBmp, (int16)wBmp, (int16)hBmp, BM_TRANSPARENT, 0, 0);
		wBmp += SMP_ITEM_CONTENT_MARGIN;
	}

	//draw the title
	if(str)
	{
		HFONT font = SGL_GetSystemFont();
		rect.x = (int16)(SMP_ITEM_CONTENT_MARGIN + wBmp);
		fontwh((PSTR)str, TRUE, (uint16)font, &wText, &hText);
		rect.y = (int16)DIV(h - hText, 2); rect.h = h; rect.w = (uint16)(SCREEN_WIDTH - rect.x - SMP_ITEM_CONTENT_MARGIN);

		color.r = PIXEL888RED(fgcolor);
		color.g = PIXEL888GREEN(fgcolor);
		color.b = PIXEL888BLUE(fgcolor);
		gbsez2((PSTR)str, rect.x, rect.y, rect, color, 1, (uint16)font);
	}
}


