#include "smp_label.h"
#include "string.h"
#include "smp.h"
#include "bmp.h"

//Label data
typedef struct SMP_LabelData
{
	DWORD bmp; //bitmap id
	PCWSTR str; // the content string
	int32 timer; // the auto scroll timer
	Uint32 interval; // the auto scroll timer interval
	Sint32 cur; // current index for auto scroll
	int32 len; //the string length in bytes
	int32 width; // the total width of the string
	int32 height; // the content height
	BOOL playing; //current auto scroll status
}LABELDATA, *PLABELDATA;


//////////////////////////////////////////////////////////////////////////////////////

static VOID SMP_Label_Scroll(int32 data)
{
	HWND hWnd = (HWND)data;
	PLABELDATA pData = _GET_WINDATA(hWnd, PLABELDATA);

	pData->cur += 2;
	if(pData->cur >= pData->len)
		pData->cur = 0;

	SGL_UpdateWindow(hWnd);
}


VOID SMP_Label_SetContent(HWND hWnd, DWORD bmpID, PCWSTR content, Uint32 interval)
{
	PLABELDATA pData = _GET_WINDATA(hWnd, PLABELDATA);

	SMP_Label_StopAutoScroll(hWnd);	

	pData->str = content;
	pData->interval = interval;
	pData->cur = 0;
	pData->bmp = bmpID;
	
	fontwh((PSTR)pData->str, TRUE, (uint16)SGL_GetSystemFont(), &pData->width, &pData->height);
	pData->len = wstrlen(pData->str);
}


VOID SMP_Label_StartAutoScroll(HWND hWnd, BOOL reset)
{
	PLABELDATA pData;

    if(0 == hWnd)
	    return;

	pData = _GET_WINDATA(hWnd, PLABELDATA);

	if(pData->playing || !_IS_SET_ANY(hWnd, SMP_LABELS_AUTOSCROLL)) 
		return;

	pData->playing = TRUE;
	if(pData->width >= _WIDTH(hWnd))
	{
		if(!pData->timer)
			pData->timer = mrc_timerCreate();
		
		mrc_timerStart(pData->timer, pData->interval, (int32)hWnd, SMP_Label_Scroll, TRUE);
	}
}


VOID SMP_Label_StopAutoScroll(HWND hWnd)
{
	PLABELDATA pData;

   if((HWND)0 == hWnd)
	   return;

	pData = _GET_WINDATA(hWnd, PLABELDATA);

	if(!pData->playing) 
		return;
	
	pData->playing = FALSE;
	if(pData->timer)
		mrc_timerStop(pData->timer);
}


LRESULT SMP_Label_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PLABELDATA pData = _GET_WINDATA(hWnd, PLABELDATA);
	
	switch(Msg)
	{
	
	case WM_CREATE:
	{
		pData = (PLABELDATA)SGL_MALLOC(sizeof(LABELDATA));
		if(NULL == pData){
			SGL_TRACE("%s, %d: memory out\n", __FILE__, __LINE__);
			return 1;
		}
		
		SGL_MEMSET(pData, 0, sizeof(LABELDATA));
		_SET_WINDATA(hWnd, pData);
		break;
	}

	case WM_DESTROY:
	{
		if(pData)
		{
			if(pData->timer)
				mrc_timerDelete(pData->timer);
			
			SGL_FREE(pData);
		}
		break;
	}

	case WM_SHOW:
	{
		SMP_Label_StartAutoScroll(hWnd, FALSE);
		break;
	}

	case WM_HIDE:
	{
		SMP_Label_StopAutoScroll(hWnd);
		break;
	}

	case WM_PAINT:
	{
		int x=0, y=0, mid;
		mr_colourSt color = {0,};
		mr_screenRectSt rect;
		HFONT font = SGL_GetSystemFont();
		PCWSTR pContent = pData->str + pData->cur;

		color.r = PIXEL888RED(_FGCOLOR(hWnd));
		color.g = PIXEL888GREEN(_FGCOLOR(hWnd));
		color.b = PIXEL888BLUE(_FGCOLOR(hWnd));

		SGL_WindowToScreen(hWnd, &x, &y);
		rect.x = (uint16)(x + SMP_ITEM_CONTENT_MARGIN); rect.y = (uint16)y;
		rect.w = (uint16)(_WIDTH(hWnd) - 2*SMP_ITEM_CONTENT_MARGIN); rect.h = (uint16)_HEIGHT(hWnd);

		//GAL_FillBox(PHYSICALGC, x, y, _WIDTH(hWnd), _HEIGHT(hWnd), COLOR_controlbg);
        if(_IS_SET_ANY(hWnd, SMP_LABELS_AUTOSCROLL))
        {
            GAL_FillBox(PHYSICALGC, x, y, _WIDTH(hWnd), _HEIGHT(hWnd), COLOR_controlbg);
        }
        
		if(_IS_SET_ANY(hWnd, WS_FOCUSED))
			GAL_Rectangle(PHYSICALGC, x, y, _WIDTH(hWnd), _HEIGHT(hWnd), COLOR_focus);

		if(RESID_INVALID != pData->bmp)
		{
			int w, h;
			HBITMAP bmp = SGL_LoadBitmap(pData->bmp, &w, &h);
			if(bmp)
			{
				mrc_bitmapShowEx((uint16 *)bmp, (int16)rect.x, (int16)(y + DIV(_HEIGHT(hWnd)-h, 2)), (int16)w, (int16)w, (int16)h, BM_TRANSPARENT, 0, 0);
				x += w + SMP_ITEM_CONTENT_MARGIN;
				rect.w -= w + SMP_ITEM_CONTENT_MARGIN;
			}
		}

		x += SMP_ITEM_CONTENT_MARGIN;
		rect.x = (int16)x;
		mid = DIV(_HEIGHT(hWnd)-pData->height, 2);
		if(_IS_SET_ANY(hWnd, SMP_LABELS_HCENTER) && pData->width < rect.w)
			gbsez2((PSTR)pContent, (int16)(x + DIV(rect.w - pData->width, 2)), (int16)(y + mid), rect, color, 1, (uint16)font);
		else
			gbsez2((PSTR)pContent, (int16)x, (int16)(y + mid), rect, color, 1, (uint16)font);

		if(_IS_SET_ANY(hWnd, SMP_LABELS_LINK))
			GAL_DrawHLine(PHYSICALGC, x, y+mid+pData->height, pData->width > rect.w? rect.w : pData->width, COLOR_blue);
		break;
	}

	case WM_MOUSEUP:
	{
		if(!_IS_SET_ANY(hWnd, SMP_LABELS_STATIC))
			SGL_NotifyParent(hWnd, SMP_LABELN_CLICKED, (DWORD)hWnd);
		break;
	}

	case WM_KEYUP:
	{
		if(wParam == MR_KEY_SELECT && !_IS_SET_ANY(hWnd, SMP_LABELS_STATIC))
		{
			SGL_NotifyParent(hWnd, SMP_LABELN_CLICKED, (DWORD)hWnd);
			return 1;
		}
		break;
	}

	case WM_SETFOCUS:
	case WM_KILLFOCUS:
	{
		SGL_UpdateWindow(hWnd);
		break;
	}	
	
	}

	return 0;
}

