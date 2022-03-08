#include "opendlg.h"
#include "topwnd.h"
#include "mainwnd.h"
#include "smp.h"
#include "smp_button.H"
#include "string.h"
#include "i18n.h"
#include "mrc_exb.h"
#include "smp_inputbox.h"
// #include "smp_advbar.h"
#include "application.h"
#include "editormain.h"
#include "editor.h"

//主窗口上的子窗口ID列表
enum
{
	KEYPAD_TOOLBAR = 1,        //工具条
	KEYPAD_OPTMENU,	    //主菜单
};


LRESULT Keypad_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_CREATE:    //来自创建的消息，从此处创建各个控件
	{   
        HWND hControl;
		//创建标题栏
		hControl = SGL_CreateWindow(SMP_Button_WndProc,
			0, 40, SCREEN_WIDTH, SMP_ITEM_HEIGHT,
			0, SMP_BUTTONS_VCENTER,0);
		SMP_Button_SetTitle(hControl,SGL_LoadString(STR_SELECT));
		//_FGCOLOR(hControl)=COLOR_lightwhite;
		SGL_AddChildWindow(hWnd, hControl);


        break;  
    }
    case WM_PAINT:

		break;
	case WM_SHOW:
	{
		break;
	}
	case WM_HIDE:
	{

		break;
	}
	case WM_INITFOCUS:  
	{
		//SGL_SetFocusWindow(hWnd, hList);
		break;
	}	
	case WM_KEYUP:  
	{
		switch(wParam)
		{
		case MR_KEY_SOFTLEFT: 
			return 1;
		case MR_KEY_SOFTRIGHT:  //返回
			return 1;

		}
		break;
	}
	case WM_COMMAND:   
	{
		WID id = LOWORD(wParam);  
		WORD code = HIWORD(wParam);  

		switch(id)    
		{
		}
		break;
	}//WM_COMMAND  结束括号
	}
	return SMP_MenuWnd_WndProc(hWnd, Msg, wParam, lParam);
}

