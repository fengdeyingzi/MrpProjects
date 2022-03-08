#include "topwnd.h"
#include "smp.h"
#include "mainwnd.h"
#include "palette.h"
#include "opendlg.h"
#include "keypad.h"

//macro to map a top window entry
#define MAP_TOPWND_ENTRY(id, proc) \
	topwindows[id - TOPWND_ID_START].wndproc = proc

//top window pool
static struct {
	HWND hWnd;
	SGL_WndProc wndproc;
}topwindows[MAX_TOPWND_COUNT - TOPWND_ID_START];


////////////////////////////////////////////////////////////////////////////////

VOID InitTopWindow(VOID)
{
	//add the map entry here
	MAP_TOPWND_ENTRY(TOPWND_MAINWND, MAINWND_WndProc);
	MAP_TOPWND_ENTRY(TOPWND_GUANYU, MAINWND_GuanYu);
	MAP_TOPWND_ENTRY(TOPWND_BANGZHU, MAINWND_BangZhu);
	MAP_TOPWND_ENTRY(TOPWND_SET, MAINWND_Set);
	MAP_TOPWND_ENTRY(TOPWND_OPENDLG, OpenDlg_WndProc);
	MAP_TOPWND_ENTRY(TOPWND_PALETTE, Palette_WndProc);
	MAP_TOPWND_ENTRY(TOPWND_KEYPAD, Keypad_WndProc);
}


HWND GetTopWindow(WID wid)
{
	wid -= TOPWND_ID_START;
	return topwindows[wid].hWnd;
}


HWND ShowTopWindow(WID wid, HWND listener, DWORD userdata)
{
	wid -= TOPWND_ID_START;
	
	if(!topwindows[wid].hWnd)
	{	
		if(NULL == (topwindows[wid].hWnd = SGL_CreateWindow(topwindows[wid].wndproc, 
				0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (WID)(wid + TOPWND_ID_START), 0, userdata)))
		{
			SGL_TRACE("%s, %d\n", __FILE__, __LINE__);
			return NULL;
		}
	}else{
		_SET_USERDATA(topwindows[wid].hWnd, userdata);
	}

	_LISTENER(topwindows[wid].hWnd) = listener;
	if(_PARENT(topwindows[wid].hWnd) != NULL)
	{
		if(SGL_GetNextChild(HWND_DESKTOP, NULL) == topwindows[wid].hWnd)
			SGL_UpdateWindow(topwindows[wid].hWnd);
		else
			SGL_BringWindowToTop(topwindows[wid].hWnd);
	}else{
		SGL_AddChildWindow(HWND_DESKTOP, topwindows[wid].hWnd);	
	}

	return topwindows[wid].hWnd;
}


VOID HideTopWindow(WID wid, BOOL destroy, BOOL redraw)
{
	wid -= TOPWND_ID_START;
	
	if(!topwindows[wid].hWnd) return;

	if(!redraw)
		SGL_SuspendDrawing();
	
	SGL_RemoveChildWindow(topwindows[wid].hWnd);
	
	if(destroy)
	{
		SGL_DestroyWindow(topwindows[wid].hWnd);
		topwindows[wid].hWnd = NULL;
	}

	if(!redraw)
		SGL_UnsuspendDrawing();
}

