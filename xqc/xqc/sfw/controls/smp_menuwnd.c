#include "i18n.h"
#include "smp.h"
#include "smp_menuwnd.h"
#include "smp_toolbar.h"
#include "smp_menuwnd.h"
#include "smp_edit.h"


LRESULT SMP_MenuWnd_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static DWORD sSavedIDs[SMP_TOOLBAR_ITEMCOUNT];
	HWND hControl;
	
	switch(Msg)
	{

	case WM_MODALSHOW:
	{
		//stop the edit blink when the focus is a edit
		if(_FOCUS(hWnd) && _WNDPROC(_FOCUS(hWnd)) == SMP_Edit_WndProc)
			SMP_Edit_StopBlink(_FOCUS(hWnd), FALSE);
		break;
	}

	case WM_MODALHIDE:
	{	
		//enable the edit blink when the focus is a edit
		if(_FOCUS(hWnd) && _WNDPROC(_FOCUS(hWnd)) == SMP_Edit_WndProc)
			SMP_Edit_StartBlink(_FOCUS(hWnd), FALSE);		
		break;
	}

	case WM_MENUSHOW:
	{
		//stop the edit blink when the focus is a edit
		if(_FOCUS(hWnd) && _WNDPROC(_FOCUS(hWnd)) == SMP_Edit_WndProc)
			SMP_Edit_StopBlink(_FOCUS(hWnd), FALSE);

		//set the toolbar strings to "SELECT" and "CANCEL"
		hControl = SGL_FindChildWindowByPosition(hWnd, 0, _HEIGHT(hWnd) - SMP_TOOLBAR_HEIGHT);
		if(!hControl && _WNDPROC(hControl) != SMP_Toolbar_WndProc)
			return 0;
		
		sSavedIDs[SMP_TOOLBARLOC_LEFT] = SMP_Toolbar_GetString(hControl, SMP_TOOLBARLOC_LEFT);
		sSavedIDs[SMP_TOOLBARLOC_MID] = SMP_Toolbar_GetString(hControl, SMP_TOOLBARLOC_MID);
		sSavedIDs[SMP_TOOLBARLOC_RIGHT] = SMP_Toolbar_GetString(hControl, SMP_TOOLBARLOC_RIGHT);
		SMP_Toolbar_SetStrings(hControl, STR_SELECT, RESID_INVALID, STR_CANCEL, TRUE);
		break;
	}

	case WM_MENUHIDE:
	{	
		//enable the edit blink when the focus is a edit
		if(_FOCUS(hWnd) && _WNDPROC(_FOCUS(hWnd)) == SMP_Edit_WndProc)
			SMP_Edit_StartBlink(_FOCUS(hWnd), FALSE);

		//restore the toolbar strings
		hControl = SGL_FindChildWindowByPosition(hWnd, 0, _HEIGHT(hWnd) - SMP_TOOLBAR_HEIGHT);
		if(!hControl && _WNDPROC(hControl) != SMP_Toolbar_WndProc)
			return 0;
		
		SMP_Toolbar_SetStrings(hControl, sSavedIDs[SMP_TOOLBARLOC_LEFT], sSavedIDs[SMP_TOOLBARLOC_MID], sSavedIDs[SMP_TOOLBARLOC_RIGHT], FALSE);
		break;
	}

	case WM_MENUMOUSEUP:
	{
		if((int)lParam < _HEIGHT(hWnd) - SMP_TOOLBAR_HEIGHT)
			return 0;
		
		hControl = SGL_FindChildWindowByPosition(hWnd, 0, _HEIGHT(hWnd) - SMP_TOOLBAR_HEIGHT);
		if(!hControl && _WNDPROC(hControl) != SMP_Toolbar_WndProc)
			return 0;

		//the scrollbar will set as the mouse tracker on MOUSE DOWN, so we should reset the mouse tracker
		SGL_SendMessage(hControl, WM_MOUSEDOWN, wParam - _LEFT(hControl), lParam - _TOP(hControl));
		SGL_SendMessage(hControl, WM_MOUSEUP, wParam - _LEFT(hControl), lParam - _TOP(hControl));
		SGL_SetMouseTracker(NULL);
		return 1;
	}

	case WM_MENUMOUSEDOWN:
	{
		//NOT IMPLEMENTED YET
		break;
	}
	
	}

	return 0;
}

