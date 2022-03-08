#include "i18n.h"
#include "bmp.h"
#include "smp.h"
#include "smp_toolbar.h"
int anxia,anp1;
//toolbar private data
typedef struct SMP_ToolbarData
{
	DWORD ids[SMP_TOOLBAR_ITEMCOUNT];
	PCWSTR str[SMP_TOOLBAR_ITEMCOUNT];
	int32 width[SMP_TOOLBAR_ITEMCOUNT];
	int32 height;
}TBDATA, *PTBDATA;

//////////////////////////////////////////////////////////////////////////////////////////////

VOID SMP_Toolbar_SetStrings(HWND hWnd, DWORD left, DWORD mid, DWORD right, BOOL redraw)
{
	int i; int32 h = 0;
	HFONT font = SGL_GetSystemFont();
	PTBDATA	pTbData = _GET_WINDATA(hWnd, PTBDATA);
	pTbData->ids[SMP_TOOLBARLOC_LEFT] = left;
	pTbData->ids[SMP_TOOLBARLOC_MID] = mid;
	pTbData->ids[SMP_TOOLBARLOC_RIGHT] = right;
	pTbData->str[SMP_TOOLBARLOC_LEFT] = SGL_LoadString(left);
	pTbData->str[SMP_TOOLBARLOC_MID] = SGL_LoadString(mid);	
	pTbData->str[SMP_TOOLBARLOC_RIGHT] = SGL_LoadString(right);
	pTbData->height = 0;

	for(i = 0; i < SMP_TOOLBAR_ITEMCOUNT; i++)
	{
		if(pTbData->str[i])
			fontwh((PSTR)pTbData->str[i], TRUE, (uint16)font, &pTbData->width[i], &h);
		if(h > pTbData->height) pTbData->height = h;
	}

	if(redraw && SGL_IsWindowVisible(hWnd))
		SGL_UpdateWindow(hWnd);
}


DWORD SMP_Toolbar_GetString(HWND hWnd, int location)
{
	PTBDATA	pTbData = _GET_WINDATA(hWnd, PTBDATA);
	return pTbData->ids[location];
}


LRESULT SMP_Toolbar_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PTBDATA pTbData = _GET_WINDATA(hWnd, PTBDATA);
	
	switch(Msg) 
	{
	
	case WM_CREATE:
	{
		pTbData = (PTBDATA)SGL_MALLOC(sizeof(TBDATA));
		if(!pTbData){
			SGL_TRACE("%s, %d: memory out\n", __FILE__, __LINE__);
			return 1;
		}
		
		SGL_MEMSET(pTbData, 0, sizeof(TBDATA));
		_SET_WINDATA(hWnd, pTbData);
		_SET_STYLE(hWnd, WS_TRANSPARENT);
		_BGCOLOR(hWnd) = COLOR_controlhili; //0x3383cc;
		_FGCOLOR(hWnd) = COLOR_black;		
		return 0;
	}

	case WM_DESTROY:
	{
		//have get the pTbData
		if(pTbData) SGL_FREE(pTbData);
		return 0;
	}

	case WM_SHOW:
	{
		_CLR_STYLE(hWnd, WS_PRESSED);
		break;
	}

	case WM_PAINT:
	{
		int i, mid;
		int x = _LEFT(hWnd);
		int y = _TOP(hWnd);
		HFONT font = SGL_GetSystemFont();

		SGL_WindowToScreen(_PARENT(hWnd), &x, &y);	

		//»æÖÆ
		mid = DIV(_HEIGHT(hWnd),2);
		GAL_DrawHLine(PHYSICALGC, x, y,  _WIDTH(hWnd), COLOR_black);
		FillShadeRange1(x, y+1,  _WIDTH(hWnd), _HEIGHT(hWnd)-1, 0x00B4B4B4, 0x00181818);

		for(i = 0; i < SMP_TOOLBAR_ITEMCOUNT; i++)
		{
			if(pTbData->str[i])
			{
				gbsez((PSTR)pTbData->str[i], 
					(int16)(x + (DIV((_WIDTH(hWnd) - pTbData->width[i])*i, 2))), 
					(int16)(y + DIV(_HEIGHT(hWnd) - pTbData->height, 2)), 
					PIXEL888RED(_FGCOLOR(hWnd)), PIXEL888GREEN(_FGCOLOR(hWnd)), PIXEL888BLUE(_FGCOLOR(hWnd)), TRUE, (uint16)font);
			}
		}
		return 0;
	}

	case WM_MOUSEDOWN:
	{
		int x, i;
		_SET_STYLE(hWnd, WS_PRESSED);
		SGL_SetMouseTracker(hWnd);
	

			if(!_IS_SET_ANY(hWnd, WS_PRESSED))
				return 0;

			_CLR_STYLE(hWnd, WS_PRESSED);
			for(i = 0; i < SMP_TOOLBAR_ITEMCOUNT; i++)
			{
				x = DIV((_WIDTH(hWnd) - pTbData->width[i])*i, 2);
				if(pTbData->str[i] && *pTbData->str[i] && (int)wParam >= x && (int)wParam <= (x + pTbData->width[i]))
				{
					SGL_DispatchEvents(WM_KEYDOWN, anp1=(i == SMP_TOOLBARLOC_LEFT? MR_KEY_SOFTLEFT : (i==SMP_TOOLBARLOC_MID? MR_KEY_SELECT : MR_KEY_SOFTRIGHT)), 0);
					anxia=1;
					break;
				}
			}		
			return 0;
		}
	case WM_MOUSEUP:
		{
			int x, i;

			if(!anxia)
				return 0;
			SGL_DispatchEvents(WM_KEYUP, anp1, 0);
			return 0;
		}
	
	}

	return 0;
}

