/*
** bmp.c: the implements of bmp.h
** 
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** Create date: 2007-12-27 by wandonglin
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#include "bmp.h"
#include "platform.h"


//include the bitmap informat datas
#include "res_bmp.rc"


//handles of bitmaps
static HBITMAP bmpDatas[RES_BITMAP_COUNT];


HBITMAP GUIAPI
SGL_LoadBitmap(DWORD bmp, int* width, int* height)
{
	int32 len;

	if(bmp >= RES_BITMAP_COUNT)
		return NULL;
	
	if(!bmpDatas[bmp])
	{
		 mrc_readFileFromMrpEx(NULL, bmpInfo[bmp].filename, (uint8**)&bmpDatas[bmp], &len, 0);
		
#ifndef _EXT_RELEASE		
		if(!bmpDatas[bmp])
		{
			SGL_TRACE("%s, %d: Failed to load bitmap: %s\n", __FILE__, __LINE__, bmpInfo[bmp].filename);
		}
#endif
	}
	
	if(width) *width = bmpInfo[bmp].width;
	if(height) *height = bmpInfo[bmp].height;
	return bmpDatas[bmp];
}


VOID GUIAPI
SGL_ReleaseBitmap(DWORD bmp)
{
	if(bmp >= RES_BITMAP_COUNT)
		return;

	if(bmpDatas[bmp])
	{
		SGL_FREE(bmpDatas[bmp]);
		bmpDatas[bmp] = NULL;
	}
}

