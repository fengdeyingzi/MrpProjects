#include "palette.h"
#include "topwnd.h"
#include "mainwnd.h"
#include "smp.h"
#include "smp_menu.h"
#include "smp_toolbar.h"
#include "smp_titlebar.h"
#include "smp_menuwnd.h"
#include "smp_msgbox.h"
#include "bmp.h"
#include "smp_list.h"
#include "smp_titlebar.h "
#include "string.h"
#include "i18n.h"
#include "mrc_exb.h"
#include "smp_inputbox.h"

LRESULT Palette_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND hControl;
	switch(Msg)
	{
	case WM_CREATE:    //来自创建的消息，从此处创建各个控件
		{   
			//创建标题栏
			hControl = SGL_CreateWindow(SMP_Titlebar_WndProc,
				0, 0, SCREEN_WIDTH, SMP_ITEM_HEIGHT,
				0, SMP_TITLEBAR_STATIC,0);
			SMP_Titlebar_SetContent(hControl, RESID_INVALID, SGL_LoadString(STR_SELECT));
			_FGCOLOR(hControl)=COLOR_lightwhite;
			SGL_AddChildWindow(hWnd, hControl);

			//创建工具栏
			hControl = SGL_CreateWindow(
				SMP_Toolbar_WndProc, 
				0, SCREEN_HEIGHT - SMP_ITEM_HEIGHT, 
				SCREEN_WIDTH,SMP_ITEM_HEIGHT,
				0, 0, 0);
			SMP_Toolbar_SetStrings(hControl, STR_MENU, RESID_INVALID, STR_BACK, FALSE);
			SGL_AddChildWindow(hWnd, hControl);
			break;  
		}
	case WM_INITFOCUS:
		{
			SGL_FocusNext(hWnd, TRUE);
			break;
		}
	case WM_SHOW:
		{
			int temp=1;
			hControl=SGL_FindChildWindow(hWnd,MAINWND_SETZHK);


		}
		break;

	case WM_COMMAND://来自子窗口的通知消息
		{
			WID id = LOWORD(wParam);//菜单事件
			WORD code = HIWORD(wParam);//菜单数据

			switch(id)
			{
			case MAINWND_OPTMENU:
				switch(code)
				{
				case STR_OK:
					break;
				}
				break;
			}
		}
	case WM_KEYDOWN:
	case WM_KEYDOWNREPEAT:
		{
			switch(wParam)
			{
			case MR_KEY_UP:
				SGL_FocusNext(hWnd, FALSE);
				return 1;
			case MR_KEY_DOWN:
				SGL_FocusNext(hWnd, TRUE);
				return 1;
			}
			break;
		}	
	case WM_KEYUP://用户释放了一个按键
		{
			switch(wParam)//p1
			{
			case MR_KEY_SOFTLEFT://左软键
				{
					int temp=1;
					hControl=SGL_FindChildWindow(hWnd,MAINWND_SETZHK);


				
				}
			case MR_KEY_SOFTRIGHT://右软键
				HideTopWindow(TOPWND_BANGZHU, TRUE, FALSE);
				ShowTopWindow(TOPWND_MAINWND, NULL, (DWORD)TOPWND_MAINWND);
				return 1;

			}
			break;
		}
	}
	return SMP_MenuWnd_WndProc(hWnd, Msg, wParam, lParam);
}

