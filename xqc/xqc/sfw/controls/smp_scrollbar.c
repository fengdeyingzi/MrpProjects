#include "smp.h"
#include "smp_scrollbar.h"

#define SCRBAR_MIN_DEFAULT 	0
#define SCRBAR_MAX_DEFAULT	99
#define SCRBAR_STEP_DEFAULT	1
#define SCRBAR_PAGE_DEFAULT	(SCRBAR_MAX_DEFAULT - SCRBAR_MIN_DEFAULT + 1) / 10

#define SCRBAR_SMP_HEADER_HEIGHT	12
#define MIN_CURSOR_HEIGHT		8

//internal use
#define GET_SCRBAR_DATA(hWnd) \
	_GET_WINDATA(hWnd, PSBDATA)

#define SET_SCRBAR_DATA(hWnd, data) \
	_SET_WINDATA(hWnd, data)


// DATA TYPES
///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct SMP_ScrBarData
{
	int min; //minimum value
	int max; //maximum value
	int cur; //current position
	int page; //page size
	int step; //the line step
}SBDATA, * PSBDATA;


// FUNCTION IMPLEMENTS
/////////////////////////////////////////////////////////////////////////////////////////////


//function to get cursor information
static VOID SMP_Scrollbar_GetCursorInfo(PSBDATA pScrbar, int h, int* cursor_y, int* cursor_h)
{
	//compute cursor position
	int total = MAX(pScrbar->max - pScrbar->min + 1, pScrbar->page);
	int maxSteps = pScrbar->max - pScrbar->min + 1 - pScrbar->page;

	*cursor_h = (h*pScrbar->page)/total; 
	if(*cursor_h < MIN_CURSOR_HEIGHT) *cursor_h = MIN_CURSOR_HEIGHT;
	
	h -= *cursor_h;
	if(maxSteps < 1) maxSteps = 1; 	
	*cursor_y = (h* (pScrbar->cur - pScrbar->min)) / maxSteps;

	//when hight the last item in the menu, set cursor to the scrollbar bottom
	if(pScrbar->cur == pScrbar->max) 
		*cursor_y = h;
}


BOOL SMP_Scrollbar_SetInfo(HWND hWnd, int min, int max, int pagesize)
{
	PSBDATA pScrbar;

	if(!IS_NORMAL_WINDOW(hWnd) 
		|| pagesize < 0 || min > max)
		return FALSE;

	pScrbar = GET_SCRBAR_DATA(hWnd);
	pScrbar->min = min;
	pScrbar->max = max;
	pScrbar->page = pagesize;
	return TRUE;
}


BOOL SMP_Scrollbar_GetInfo(HWND hWnd, int* min, int* max, int* pagesize)
{
	PSBDATA pScrbar;

	if(!IS_NORMAL_WINDOW(hWnd) 
		|| !min || !max || !pagesize)
		return FALSE;

	pScrbar = GET_SCRBAR_DATA(hWnd);
	if(min) *min = pScrbar->min;
	if(max) *max = pScrbar->max;
	if(pagesize) *pagesize = pScrbar->page;
	return TRUE;
}


VOID SMP_Scrollbar_SetStep(HWND hWnd, int step)
{
	PSBDATA pScrbar = GET_SCRBAR_DATA(hWnd);
	pScrbar->step = step;
}


BOOL SMP_Scrollbar_SetValue(HWND hWnd, int index, BOOL redraw)
{
	PSBDATA pScrbar;

	if(!IS_NORMAL_WINDOW(hWnd))
		return FALSE;

	pScrbar = GET_SCRBAR_DATA(hWnd);
	if(index == pScrbar->cur || index < pScrbar->min || index > MAX(pScrbar->max - pScrbar->page + 1, 0))
		return FALSE;

	pScrbar->cur = index;
	if(redraw)
	{
		SGL_UpdateWindow(hWnd);
		
		//Notify Parent that position changed
		SGL_NotifyParent(hWnd, SMP_SCRBARN_VALUECHANGED, index);
	}
	return TRUE;
}


int SMP_Scrollbar_GetValue(HWND hWnd)
{
	PSBDATA pScrbar;

	if(!IS_NORMAL_WINDOW(hWnd))
		return 0;
	
	pScrbar = GET_SCRBAR_DATA(hWnd);
	return pScrbar->cur;
}


BOOL SMP_Scrollbar_PageUp(HWND hWnd)
{
	PSBDATA pScrbar;

	if(!IS_NORMAL_WINDOW(hWnd))
		return FALSE;

	pScrbar = GET_SCRBAR_DATA(hWnd);
	if(pScrbar->cur > pScrbar->min)
		SMP_Scrollbar_SetValue(hWnd, MAX(pScrbar->cur - pScrbar->page, pScrbar->min), TRUE);

	return TRUE;
}


BOOL SMP_Scrollbar_PageDown(HWND hWnd)
{
	PSBDATA pScrbar;

	if(!IS_NORMAL_WINDOW(hWnd))
		return FALSE;

	pScrbar = GET_SCRBAR_DATA(hWnd);
	if(pScrbar->cur < pScrbar->max - pScrbar->page + 1)
		SMP_Scrollbar_SetValue(hWnd, MIN( pScrbar->cur + pScrbar->page, pScrbar->max - pScrbar->page + 1), TRUE);

	return TRUE;
}


BOOL SMP_Scrollbar_LineUp(HWND hWnd)
{
	PSBDATA pScrbar;

	if(!IS_NORMAL_WINDOW(hWnd))
		return FALSE;

	pScrbar = GET_SCRBAR_DATA(hWnd);
	if(pScrbar->cur > pScrbar->min)
		SMP_Scrollbar_SetValue(hWnd, MAX(pScrbar->cur - pScrbar->step, pScrbar->min), TRUE);

	return TRUE;
}


BOOL SMP_Scrollbar_LineDown(HWND hWnd)
{
	PSBDATA pScrbar;

	if(!IS_NORMAL_WINDOW(hWnd))
		return FALSE;

	pScrbar = GET_SCRBAR_DATA(hWnd);
	if(pScrbar->cur < pScrbar->max - pScrbar->page + 1)
		SMP_Scrollbar_SetValue(hWnd, MIN( pScrbar->cur + pScrbar->step, pScrbar->max - pScrbar->page + 1), TRUE);

	return TRUE;
}


LRESULT SMP_Scrollbar_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{

	case WM_CREATE:
	{
		PSBDATA pScrbar = SGL_MALLOC(sizeof(SBDATA));
		if(!pScrbar){
			SGL_TRACE("%s, %d: memory out\n", __FILE__, __LINE__);
			return 1;
		}
		
		SGL_MEMSET(pScrbar, 0, sizeof(SBDATA));
		pScrbar->cur = pScrbar->min = SCRBAR_MIN_DEFAULT;
		pScrbar->max = SCRBAR_MAX_DEFAULT;
		pScrbar->page = SCRBAR_PAGE_DEFAULT;
		pScrbar->step = SCRBAR_STEP_DEFAULT; // the default step
		SET_SCRBAR_DATA(hWnd, pScrbar);
		_BGCOLOR(hWnd) = 0xefefef;
		_FGCOLOR(hWnd) = COLOR_controlhili;
		return 0;
	}

	case WM_DESTROY:
	{
		PSBDATA pScrbar = GET_SCRBAR_DATA(hWnd);
		if(pScrbar) SGL_FREE(pScrbar);
		return 0;
	}

	case WM_PAINT:
	{
		int x=0, y=0, cursor_y=0, cursor_h=0, i;
		PSBDATA pScrbar = GET_SCRBAR_DATA(hWnd);

		SGL_WindowToScreen(hWnd, &x, &y);
		SMP_Scrollbar_GetCursorInfo(pScrbar, _HEIGHT(hWnd) - 2 * SCRBAR_SMP_HEADER_HEIGHT, &cursor_y, &cursor_h);

		//draw the scrollbar
		GAL_FillBox(PHYSICALGC, x + 1, y + 1, _WIDTH(hWnd)-2, SCRBAR_SMP_HEADER_HEIGHT-2, 0xbdd7ef);
		GAL_Rectangle(PHYSICALGC, x, y, _WIDTH(hWnd), SCRBAR_SMP_HEADER_HEIGHT, _FGCOLOR(hWnd));
		GAL_FillBox(PHYSICALGC, x+1, y + _HEIGHT(hWnd) - SCRBAR_SMP_HEADER_HEIGHT +1, _WIDTH(hWnd)-2, SCRBAR_SMP_HEADER_HEIGHT-2, 0xbdd7ef);	
		GAL_Rectangle(PHYSICALGC, x, y + _HEIGHT(hWnd) - SCRBAR_SMP_HEADER_HEIGHT, _WIDTH(hWnd), SCRBAR_SMP_HEADER_HEIGHT, _FGCOLOR(hWnd));	

		for(i=0; i<2; ++i)
		{
			GAL_Line(PHYSICALGC, x + 2, i+y + SCRBAR_SMP_HEADER_HEIGHT - 5, x + DIV(_WIDTH(hWnd), 2) - 1, i+y + 3, COLOR_darkgray);
			GAL_Line(PHYSICALGC, x + _WIDTH(hWnd) - 4, i+y + SCRBAR_SMP_HEADER_HEIGHT - 5, x + DIV(_WIDTH(hWnd), 2) - 1, i+y + 3, COLOR_darkgray);	
		}
		for(i=0; i<2; ++i)
		{
			GAL_Line(PHYSICALGC, x + 2, i+y + _HEIGHT(hWnd) - SCRBAR_SMP_HEADER_HEIGHT + 3, x + DIV(_WIDTH(hWnd), 2) - 1, i+y + _HEIGHT(hWnd) - 5, COLOR_darkgray);
			GAL_Line(PHYSICALGC, x + _WIDTH(hWnd) - 4, i+y + _HEIGHT(hWnd) - SCRBAR_SMP_HEADER_HEIGHT + 3, x + DIV(_WIDTH(hWnd), 2) - 1, i+y + _HEIGHT(hWnd) - 5, COLOR_darkgray);		
		}
		//drawf cursor
		GAL_FillBox(PHYSICALGC, x + 1, y + SCRBAR_SMP_HEADER_HEIGHT + cursor_y, _WIDTH(hWnd) - 2, cursor_h, 0xbdd7ef);
		GAL_Rectangle(PHYSICALGC, x, y + SCRBAR_SMP_HEADER_HEIGHT + cursor_y, _WIDTH(hWnd), cursor_h, _FGCOLOR(hWnd));

		//画中间标示
		//GAL_DrawHLine(PHYSICALGC, x + 2, y + SCRBAR_SMP_HEADER_HEIGHT + cursor_y + cursor_h/2 - 3, _WIDTH(hWnd) - 4, 0x94aebd);//COLOR_bgNight);
		//GAL_DrawHLine(PHYSICALGC, x + 2, y + SCRBAR_SMP_HEADER_HEIGHT + cursor_y + cursor_h/2 - 1, _WIDTH(hWnd) - 4, 0x94aebd);//COLOR_bgNight);
		//GAL_DrawHLine(PHYSICALGC, x + 2, y + SCRBAR_SMP_HEADER_HEIGHT + cursor_y + cursor_h/2 + 1, _WIDTH(hWnd) - 4, 0x94aebd);//COLOR_bgNight);
		return 0;
	}

	case WM_MOUSEDOWN:
	{
		int cursor_y, cursor_h;
		PSBDATA pScrbar = GET_SCRBAR_DATA(hWnd);

		if((int)lParam < SCRBAR_SMP_HEADER_HEIGHT)
			SMP_Scrollbar_LineUp(hWnd);
		else if((int)lParam > _HEIGHT(hWnd) - SCRBAR_SMP_HEADER_HEIGHT)
			SMP_Scrollbar_LineDown(hWnd);
		else{
			SMP_Scrollbar_GetCursorInfo(pScrbar, _HEIGHT(hWnd) - 2 * SCRBAR_SMP_HEADER_HEIGHT, &cursor_y, &cursor_h);
			if((int)lParam < SCRBAR_SMP_HEADER_HEIGHT + cursor_y)
				SMP_Scrollbar_PageUp(hWnd);
			else if((int)lParam > SCRBAR_SMP_HEADER_HEIGHT + cursor_y + cursor_h)
				SMP_Scrollbar_PageDown(hWnd);
		}
		return 0;
	}

	case WM_KEYDOWN:
	case WM_KEYDOWNREPEAT:
	{
		UINT key = (UINT) wParam;		
		if(key == MR_KEY_DOWN)
		{
			SMP_Scrollbar_LineDown(hWnd);
			return 1;
		}else if(key == MR_KEY_UP){
			SMP_Scrollbar_LineUp(hWnd);
			return 1;
		}
		break;
	}
	
	}

	return 0;
}


