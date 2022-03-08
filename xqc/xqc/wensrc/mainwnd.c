#include "i18n.h"

#include "mainwnd.h"

#include "smp.h"
#include "smp_menu.h"
#include "smp_toolbar.h"
#include "smp_menuwnd.h"
#include "smp_edit.h"
#include "smp_titlebar.h "
#include "smp_msgbox.h"
//#include "smp_flashbox.h"
#include "bmp.h"
#include "smp_label.h"
//#include "plt_msgbox.h"
//#include "smp_progbar.h"
#include "smp_combobox.h"
//#include "smp_list.h"
#include "topwnd.h"
#include "string.h"
//#include "smp_datepicker.h"
#include "smp_textinfo.h"
#include "smp_inputbox.h"
#include "opendlg.h "
//#include <math.h>
#include "wenexp.h"
#include "editormain.h"
#include "editor.h"
#include "palette.h"
#include "Keyboard.H "


void logostop();


extern int32 EditEditHand,//�༭����
EditFunction;
extern int PACKGEDMRP,SET_HEAP_SIZE;


extern enum Windows MainWindow;


static UCHAR tmp[512];


static const DWORD miOptions[] = 
{
	STR_CHARU,
	STR_DAOHANG,
	STR_WENJIAN,
	STR_QITA,
	STR_YUNXING,
	STR_EXIT
};

static const DWORD miDaohang[] =
{
	STR_TIAOZHUAN,
	STR_SETQI,
	STR_SETZHONG,
	STR_FUZHI,
	STR_ZHANTIE,
	STR_QUXIAOXUANZE
};
static const DWORD miWenjian[] =
{
	STR_NEW,
	STR_DAKAI,
	STR_BAOCUN,
	STR_LINGCUNWEI,
	STR_PRINTLOG,
	STR_CLOSE,
	STR_CLOSES,
	STR_DABAO
};
static const DWORD miQiTa[] =
{
	STR_SET,
	STR_SETCOL,
	STR_ABOUT,
	STR_HELP
};
int32 mrc_unicodeToGb2312(uint8* input, int32 input_len, uint8** output, int32* output_len);

int GotoLine(int line);
int32 EditInsertText(char *text);//�ڹ�괦����һ������y


void SetFont(int font)
{
	openfont(FONT);
	SGL_SetSystemFont(FONT);
	SetEditorListShow(WORDOBG,5,SMP_ITEM_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT-SMP_ITEM_HEIGHT-SMP_ITEM_HEIGHT);
}

static VOID ShowOptMenu(HWND hWnd)
{
	SMP_Menu_ClearMenuItems();//����˵���
	SMP_Menu_SetMenuItem2(0, miOptions, TABLESIZE(miOptions));//���ò˵����ַ�����Դ

	SMP_Menu_SetMenuItem2(TABLESIZE(miOptions), miDaohang, TABLESIZE(miDaohang));//���ò˵����ַ�����Դ//����
	SMP_Menu_SetSubMenu(STR_DAOHANG, TABLESIZE(miOptions));//����һ���Ӳ˵�

	SMP_Menu_SetMenuItem2(TABLESIZE(miOptions)+TABLESIZE(miDaohang), miWenjian, TABLESIZE(miWenjian));//���ò˵����ַ�����Դ//�ļ�
	SMP_Menu_SetSubMenu(STR_WENJIAN,TABLESIZE(miOptions)+TABLESIZE(miDaohang));//����һ���Ӳ˵�

	SMP_Menu_SetMenuItem2(TABLESIZE(miOptions)+TABLESIZE(miDaohang)+TABLESIZE(miWenjian), miQiTa, TABLESIZE(miQiTa));//���ò˵����ַ�����Դ//����
	SMP_Menu_SetSubMenu(STR_QITA,TABLESIZE(miOptions)+TABLESIZE(miDaohang)+TABLESIZE(miWenjian));//����һ���Ӳ˵�

	SMP_Menu_Popup(MAINWND_OPTMENU, SMP_MENUS_BOTTOMLEFT, hWnd, 0, _HEIGHT(hWnd) - SMP_TOOLBAR_HEIGHT, NULL);//����ȫ�ֲ˵�
}
static const DWORD miNow[] =
{
	STR_NEWNULL,
	STR_NEWFORM,
	STR_NEWPRO
};
static const DWORD miNowPro[] =
{
	STR_NEWHELLO,
	STR_NEWMODE
};
static VOID ShowMenuNow(HWND hWnd)
{
	SMP_Menu_ClearMenuItems();//����˵���
	SMP_Menu_SetMenuItem2(0, miNow, TABLESIZE(miNow));//���ò˵����ַ�����Դ

	SMP_Menu_SetMenuItem2(TABLESIZE(miNow), miNowPro, TABLESIZE(miNowPro));//���ò˵����ַ�����Դ//����
	SMP_Menu_SetSubMenu(STR_NEWPRO, TABLESIZE(miNow));//����һ���Ӳ˵�

	SMP_Menu_Popup(MAINWND_OPTMENU, SMP_MENUS_BOTTOMLEFT, hWnd, 0, _HEIGHT(hWnd) - SMP_TOOLBAR_HEIGHT, NULL);//����ȫ�ֲ˵�
}



LRESULT MAINWND_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND hControl;

	switch(Msg)
	{
	case WM_CREATE://��ʼ��
	{
		editorinit();
		SGL_SetSystemFont(FONT);
		RectValue(WORDING->EditShow,0,SMP_ITEM_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT-SMP_ITEM_HEIGHT-SMP_ITEM_HEIGHT);
		if(WORDOBG)
			SetEditorListShow(WORDOBG,0,SMP_ITEM_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT-SMP_ITEM_HEIGHT-SMP_ITEM_HEIGHT);


		//���ڵı�����ɫ
		_BGCOLOR(hWnd) = COLOR_darkgray;//��ɫ

		//������
		hControl = SGL_CreateWindow(SMP_Label_WndProc,
			0, 0, SCREEN_WIDTH, SMP_ITEM_HEIGHT,
			MAINWND_ZBT, 
			 SMP_LABELS_LINK | SMP_LABELS_AUTOSCROLL, 0);
		_BGCOLOR(hControl) = COLOR_lightgray;
			SMP_Label_SetContent(hControl, RESID_INVALID, SGL_LoadString(STR_ZBT),300);
		SGL_AddChildWindow(hWnd, hControl);


		//x=0,y=320-26;w=240;h=26,��������ʼ��
		hControl = SGL_CreateWindow(SMP_Toolbar_WndProc, 
			0, SCREEN_HEIGHT - SMP_TOOLBAR_HEIGHT, SCREEN_WIDTH, SMP_TOOLBAR_HEIGHT,
			MAINWND_TOOLBAR, SMP_TOOLBAR_ITEMCOUNT, 0);
		//���ù��������ַ���
		SMP_Toolbar_SetStrings(hControl, STR_OPTIONS, RESID_INVALID, STR_REMOVE, FALSE);
		//����������Ϊ�Ӵ��ڿؼ�
		SGL_AddChildWindow(hWnd, hControl);

		break;
	}

	case WM_INITFOCUS://���͵������ģ̬��������ʼ��Ĭ�Ͻ�����Ӵ���
	{
		SGL_FocusNext(hWnd, TRUE);//���õ�һ���ؼ�Ϊ����
		break;
	}

	case WM_KEYDOWN://�û�����һ����
		EditEvent(MR_KEY_PRESS,wParam,0);
		break;
	

	case WM_KEYUP://�û��ͷ���һ������
	{
		switch(wParam)//p1
		{
		case MR_KEY_SOFTLEFT://�����
			ShowOptMenu(hWnd);//���ز˵�
			return 1;
		case MR_KEY_SELECT:
			{
				char **wtt;
				GetObgText(&wtt);
				SMP_InputBox(MAINWND_EDITORU,hWnd,SGL_LoadString(STR_CHARU),(PCWSTR)CharToUnicode((PSTR)*wtt),512,ES_ALPHA,NULL);
			}
			return 1;
		case MR_KEY_2:
			{
				char tt[15],*wtt;
				sprintf(tt,"%d",GetFocusLine()+1);
				wtt=(char *)CharToUnicode((PSTR)tt);
				SMP_InputBox(MAINWND_TIAOZHUAN,hWnd,SGL_LoadString(STR_TIAOZHUAN),(PCWSTR)wtt,12,ES_NUM,NULL);
			}
			return 1;
		case MR_KEY_3:
			EditSetCopyStart();
			return 1;

		case MR_KEY_6: //�����յ�
			EditSetCopyEnd();
			return 1;
		case MR_KEY_8:
			CopyStrText();
			return 1;
		case MR_KEY_0:
			PasteStrText();
			return 1;
		case MR_KEY_7:
			if(SaveFile()==-1)
				SMP_InputBox(MAINWND_LINGCUN,hWnd,SGL_LoadString(STR_LINGCUNWEI),SGL_LoadString(STR_CNOWNAME),128,ES_ALPHA,NULL);
			else
				SMP_MsgBox(0, hWnd, NULL, SGL_LoadString(STR_MSG), SGL_LoadString(STR_SAVESUC), ID_OK | SMP_MSGBOXS_AUTOCLOSE, NULL);
			return 1;
		case MR_KEY_9:
			PACKGEDMRP=0;
			mrc_pause();
			EditPause();
			Debugging();
			return 1;


//			/**
//* \\��̵����صĶ���
//
//*
//
//* \\����WID��������ID
//
//* \\�����ƻ�������𴰿�
//
//* \\�����ػ����ˢ�¶���
// */
//			HideTopWindow(_WID(hWnd), FALSE, TRUE);
		}
		EditEvent(MR_KEY_RELEASE,wParam,0);
		break;
	}
	case WM_MOUSEDOWN:
		EditEvent(MR_MOUSE_DOWN,wParam,lParam);
		break;
	case WM_MOUSEUP:
		EditEvent(MR_MOUSE_UP,wParam,lParam);
	/*	if(wParam==1)
			{
				GetTopWindow(TOPWND_KEYPAD);
		}*/

		break;
	case WM_MOUSEMOVE:
		EditEvent(MR_MOUSE_MOVE,wParam,lParam);
		break;

	case WM_COMMAND://�����Ӵ��ڵ�֪ͨ��Ϣ
	{
		WID id = LOWORD(wParam);//�˵��¼�
		WORD code = HIWORD(wParam);//�˵�����

		switch(id)
		{
		case MAINWND_CHARU:
			if(code == INPUT_OK)
			{
				PSTR tt=NULL;
				tt = (PSTR)UnicodeToChar((PWSTR)lParam);
				EditInsertText(tt);
				SGL_FREE(tt);
			}
			break;
		case MAINWND_LINGCUN:
		   if(code == INPUT_OK)
		   {
			   PSTR tt=NULL;
			   tt = (PSTR)UnicodeToChar((PWSTR)lParam);
			   SMP_MsgBox(0, hWnd, NULL, SGL_LoadString(STR_MSG), SGL_LoadString(EditSaveToFile(tt,1)?STR_SAVEFAI:STR_SAVESUC), ID_OK | SMP_MSGBOXS_AUTOCLOSE, NULL);
			   SGL_FREE(tt);
		   }
		   break;
		case MAINWND_TIAOZHUAN:
			if(code == INPUT_OK)
			{
				int32 num;
				PSTR tt=NULL;
				tt = (PSTR)UnicodeToChar((PWSTR)lParam);
				num=mrc_atoi(tt);
				SGL_FREE(tt);
				num=num<1?0:num-1;
				GotoLine(num);
			}
			break;
		case MAINWND_NEWJ:
			if(code == INPUT_OK)
			{
				int32 num;
				PSTR tt=NULL;
				tt = (PSTR)UnicodeToChar((PWSTR)lParam);
				SMP_MsgBox(0, hWnd, NULL, SGL_LoadString(STR_MSG), SGL_LoadString(memdir((char*)tt,(char*)tmp)?STR_FOUNDNO:STR_FOUNDOK), ID_OK | SMP_MSGBOXS_AUTOCLOSE, NULL);
			}
			break;
		case  MAINWND_EDITORU:
			if(code == INPUT_OK)
			{
				if(!lParam)
					EditInsertText("");
				else
				{
					char **wtt,*tts;
					GetObgText(&wtt);
					tts= (char *)UnicodeToChar((PWSTR)lParam);
					free(*wtt);
					*wtt=tts;
				}
			}
			break;
		case MAINWND_LABEL://û��
			SMP_MsgBox(0, hWnd, NULL, SGL_LoadString(STR_YES), SGL_LoadString(STR_NO), ID_OK | SMP_MSGBOXS_AUTOCLOSE, NULL);
			break;
		case MAINWND_OPTMENU://����ȷ��
			switch(code)//p1
			{

			case STR_CANCEL://ȡ��//û������
				SMP_MsgBox(0, hWnd, NULL, SGL_LoadString(STR_YES), SGL_LoadString(STR_NO), ID_OK | SMP_MSGBOXS_AUTOCLOSE, NULL);
				break;

	
			case STR_CHARU: //����
				SMP_InputBox(MAINWND_CHARU,hWnd,SGL_LoadString(STR_CHARU),(PCWSTR)"",512,ES_ALPHA,NULL);
				break;
			case STR_TIAOZHUAN: //��ת
				{
					char tt[15],*wtt;
					sprintf(tt,"%d",GetFocusLine()+1);
					wtt=(char *)CharToUnicode(tt);
					SMP_InputBox(MAINWND_TIAOZHUAN,hWnd,SGL_LoadString(STR_TIAOZHUAN),(PCWSTR)wtt,12,ES_NUM,NULL);
				}
				break;

			case STR_SETQI: //�������
				EditSetCopyStart();
				break;

			case STR_SETZHONG: //�����յ�
				EditSetCopyEnd();
				break;

			case STR_FUZHI: //����
				CopyStrText();
				break;

			case STR_ZHANTIE: //ճ��
				PasteStrText();
				break;

			case STR_QUXIAOXUANZE: //ȡ��ѡ��
				CleanSelect();
				break;
			case STR_NEW: //�½�
				ShowMenuNow(hWnd);
				break;
			case STR_NEWNULL: //�½����ļ�
				EditLoadFile(NULL,ED_FILE_RDWR);
				SGL_UpdateWindow(hWnd);
				break;
			case STR_NEWFORM: //�½�ģ��
					if(!EditLoadFile("c/ģ��.c",ED_FILE_RDWR))
					{
						WORDOBG->WorkFile[0]=0;
						SGL_UpdateWindow(hWnd);
					}else
						SMP_MsgBox(0, hWnd, NULL, SGL_LoadString(STR_MSG), SGL_LoadString(STR_NOTFILE), ID_OK | SMP_MSGBOXS_AUTOCLOSE, NULL);
				break;
				case STR_NEWHELLO: //�½���Ŀ
					{
						char tt[128],*wtt;
						sprintf(tt,"%s","c/�ҵ���Ŀ");
						wtt=(char *)CharToUnicode(tt);
						SMP_InputBox(MAINWND_NEWJ,hWnd,SGL_LoadString(STR_PRONAMEPUT),SGL_LoadString(STR_PRONAME),128,ES_ALPHA,NULL);

						strcpy((char *)&tmp[0],"xqc/project/helloworld");
					}
				break;
			case STR_DAKAI: //��
				HideTopWindow(TOPWND_MAINWND, FALSE, FALSE);
				ShowTopWindow(TOPWND_OPENDLG, hWnd, (DWORD)TOPWND_MAINWND);
			break;

			case STR_BAOCUN: //����
				if(SaveFile()==-1)
					SMP_InputBox(MAINWND_LINGCUN,hWnd,SGL_LoadString(STR_LINGCUNWEI),SGL_LoadString(STR_CNOWNAME),128,ES_ALPHA,NULL);
				else
					SMP_MsgBox(0, hWnd, NULL, SGL_LoadString(STR_MSG), SGL_LoadString(STR_SAVESUC), ID_OK | SMP_MSGBOXS_AUTOCLOSE, NULL);
			break;

			case STR_LINGCUNWEI: //���Ϊ
				{
					char tt[128],*wtt;
					sprintf(tt,"%s",WORDOBG->WorkFile);
					wtt=(char *)CharToUnicode(tt);
					SMP_InputBox(MAINWND_LINGCUN,hWnd,SGL_LoadString(STR_LINGCUNWEI),(PCWSTR)wtt,128,ES_ALPHA,NULL);
				}
			break;
			case STR_PRINTLOG: //�����־

				if(!EditLoadFile("c/print.txt",ED_FILE_RDWR))
				{
					WORDOBG->WorkFile[0]=0;
					SGL_UpdateWindow(hWnd);
				}else
					SMP_MsgBox(0, hWnd, NULL, SGL_LoadString(STR_MSG), SGL_LoadString(STR_NOTFILE), ID_OK | SMP_MSGBOXS_AUTOCLOSE, NULL);
				break;
			case STR_CLOSE: //�رյ�ǰ
				WORDOBG=DelWode(WORDOBG);
				SGL_UpdateWindow(hWnd);
				break;
			case STR_CLOSES: //�ر�ȫ��
				DelAllEdit(WORDING->docListHead);
				SGL_UpdateWindow(hWnd);
				break;
			case STR_DABAO: //���
				PACKGEDMRP=1;
				mrc_pause();
				EditPause();
				Debugging();
				break;

			case STR_YUNXING://����
				PACKGEDMRP=0;
				mrc_pause();
				EditPause();
				Debugging();
				break;

			case STR_SET: //����
				HideTopWindow(TOPWND_MAINWND, FALSE, FALSE);
				ShowTopWindow(TOPWND_SET, NULL, (DWORD)TOPWND_MAINWND);
				break;
			case STR_SETCOL: //��ɫ����
				HideTopWindow(TOPWND_MAINWND, FALSE, FALSE);
				ShowTopWindow(TOPWND_PALETTE, NULL, (DWORD)TOPWND_MAINWND);
				break;
			case STR_ABOUT: //����
				HideTopWindow(TOPWND_MAINWND, FALSE, FALSE);
				ShowTopWindow(TOPWND_GUANYU, NULL, (DWORD)TOPWND_MAINWND);
				break;
			case STR_HELP://����
				HideTopWindow(TOPWND_MAINWND, FALSE, FALSE);
				ShowTopWindow(TOPWND_BANGZHU, NULL, (DWORD)TOPWND_MAINWND);
				break;
			case STR_EXIT:
				_SET_STYLE(hWnd, WS_KEEPFOCUS);//�ָ���������ʾ
				EditFreeAll();//�˳�
				mrc_exit();
				break;
			}
			break;
		}
		
		break;
	}
	case WM_SHOW:

	case WM_MODALSHOW://ģ̬������ʾʱ���͵�������
	case WM_MENUSHOW://�˵���ʾʱ���͵�������
	{
		//�ڸ������а���ʶ�������Ӵ���
		SMP_Label_StartAutoScroll( SGL_FindChildWindow(hWnd, MAINWND_ZBT),TRUE);
		EditPause();

		break;
	}

	case WM_MODALHIDE://ģ̬���ڹر�ʱ���͵�������
	case WM_MENUHIDE://�˵��ر�ʱ�����͵�������
	{
		SMP_Label_StartAutoScroll( SGL_FindChildWindow(hWnd, MAINWND_ZBT),FALSE);
		EditResume();
		break;
	}
	case WM_PAINT:
		{
			PCWSTR file=NULL;
			if(SGL_GetPopupMenu()==NULL)
				EditResume();
			if(WORDOBG)
				if(WORDOBG->WorkFile[0])
					file=(PCWSTR)CharToUnicode((PSTR)WORDOBG->WorkFile);
				else
					file=(PCWSTR)CharToUnicode((PSTR)"��ʱ�ļ�");
			else
					file=SGL_LoadString(STR_ZBT);
				SMP_Label_SetContent( SGL_FindChildWindow(hWnd, MAINWND_ZBT), BMP_FILE, file,300);
			DrawEditor(1);
		}
		break;
	case WM_HIDE:
		EditPause();
		break;

	case WM_DESTROY:
		closefont();
		break;
	}

	return SMP_MenuWnd_WndProc(hWnd, Msg, wParam, lParam);
}

LRESULT MAINWND_GuanYu(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

	HWND hControl;
	static  HWND htext;

	switch(Msg)
	{
	case WM_CREATE://��ʼ��
		{

			//���ڵı�����ɫ
			_BGCOLOR(hWnd) = COLOR_darkyellow;//��ɫ



			//����������
			hControl = SGL_CreateWindow(SMP_Titlebar_WndProc,
				0, 0, _WIDTH(hWnd), SMP_ITEM_HEIGHT,
				0, SMP_TITLEBAR_STATIC,0);
			SMP_Titlebar_SetContent(hControl, RESID_INVALID, SGL_LoadString(STR_ABOUT));
			SGL_AddChildWindow(hWnd, hControl);



			//����������
			hControl = SGL_CreateWindow(SMP_Toolbar_WndProc, 
				0,  _HEIGHT(hWnd) - SMP_ITEM_HEIGHT, 
				_WIDTH(hWnd), SMP_ITEM_HEIGHT,
				MAINWND_GUANYUGJ,
				SMP_TOOLBARLOC_RIGHT, 0);
			SMP_Toolbar_SetStrings(hControl,  RESID_INVALID, RESID_INVALID, STR_BACK, FALSE);
			SGL_AddChildWindow(hWnd, hControl);
			//�ı���
			htext=hControl = SGL_CreateWindow(SMP_TextInfo_WndProc,
				0,SMP_ITEM_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT - SMP_ITEM_HEIGHT*2,
				MAINWND_GUANYUDT,SMP_TEXTINFOS_HASBOARD,0);
			SMP_TextInfo_SetContent(hControl,SGL_LoadString(STR_GUANYUDT));
			SGL_AddChildWindow(hWnd, hControl);
			break;
		}
	case WM_INITFOCUS:
		{
			SGL_SetFocusWindow(hWnd, htext);
			break;
		}

	case WM_COMMAND://�����Ӵ��ڵ�֪ͨ��Ϣ
		{
			break;
		}
	case WM_KEYUP://�û��ͷ���һ������
	{
		switch(wParam)//p1
		{
		case MR_KEY_SOFTLEFT://�����
			return 1;
		case MR_KEY_SOFTRIGHT://�����
			HideTopWindow(TOPWND_GUANYU, TRUE, FALSE);
			ShowTopWindow(TOPWND_MAINWND, NULL, (DWORD)TOPWND_MAINWND);
			return 1;
		}
	}
	break;
	}
	return SMP_MenuWnd_WndProc(hWnd, Msg, wParam, lParam);
}

LRESULT MAINWND_BangZhu(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

	HWND hControl;
	static  HWND htext;

	switch(Msg)
	{
	case WM_CREATE://��ʼ��
		{

			//���ڵı�����ɫ
			_BGCOLOR(hWnd) = COLOR_darkyellow;//��ɫ

			//����������
			hControl = SGL_CreateWindow(SMP_Titlebar_WndProc,
				0, 0, _WIDTH(hWnd), SMP_ITEM_HEIGHT,
				0, SMP_TITLEBAR_STATIC,0);
			SMP_Titlebar_SetContent(hControl, RESID_INVALID, SGL_LoadString(STR_HELP));
			SGL_AddChildWindow(hWnd, hControl);


		
			//����������
			hControl = SGL_CreateWindow(SMP_Toolbar_WndProc, 
				0,  _HEIGHT(hWnd) - SMP_ITEM_HEIGHT, 
				_WIDTH(hWnd), SMP_ITEM_HEIGHT,
				MAINWND_BANGZHUGJ,
				SMP_TOOLBARLOC_RIGHT, 0);
			SMP_Toolbar_SetStrings(hControl,  RESID_INVALID, RESID_INVALID, STR_BACK, FALSE);
			SGL_AddChildWindow(hWnd, hControl);
			//�ı���
			htext=hControl = SGL_CreateWindow(SMP_TextInfo_WndProc,
				0,SMP_ITEM_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT - SMP_ITEM_HEIGHT*2,
				MAINWND_BANGZHUDT,SMP_TEXTINFOS_HASBOARD,0);
			SMP_TextInfo_SetContent(hControl,SGL_LoadString(STR_BANGZHUDT));
			SGL_AddChildWindow(hWnd, hControl);
			break;
		}
	case WM_INITFOCUS:
		{
			SGL_SetFocusWindow(hWnd, htext);
			break;
		}

	case WM_COMMAND://�����Ӵ��ڵ�֪ͨ��Ϣ
		{

		}
	case WM_KEYUP://�û��ͷ���һ������
		{
			switch(wParam)//p1
			{
			case MR_KEY_SOFTLEFT://�����
				return 1;
			case MR_KEY_SOFTRIGHT://�����
				HideTopWindow(TOPWND_BANGZHU, TRUE, FALSE);
				ShowTopWindow(TOPWND_MAINWND, NULL, (DWORD)TOPWND_MAINWND);
				return 1;
			}
			break;
		}
	}
	return SMP_MenuWnd_WndProc(hWnd, Msg, wParam, lParam);
}

static const DWORD miFONT[] =
{
	STR_FONT0,
	STR_FONT1,
	STR_FONT2,
	STR_FONT12,
	STR_FONT16
};
PWSTR Set_text_HEAP[30];

LRESULT MAINWND_Set(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

	HWND hControl,hControl2;

	switch(Msg)
	{
	case WM_CREATE://��ʼ��
		{

			int yy=0;
			//���ڵı�����ɫ
			_BGCOLOR(hWnd) = COLOR_darkgray;//��ɫ

			//����������
			hControl = SGL_CreateWindow(SMP_Titlebar_WndProc,
				0, 0, _WIDTH(hWnd), SMP_HEADER_HEIGHT,
				0, SMP_TITLEBAR_STATIC,0);
			SMP_Titlebar_SetContent(hControl, RESID_INVALID, SGL_LoadString(STR_SET));
			SGL_AddChildWindow(hWnd, hControl);


	
			//����������
			hControl = SGL_CreateWindow(SMP_Toolbar_WndProc, 
				0,  _HEIGHT(hWnd) - SMP_ITEM_HEIGHT, 
				_WIDTH(hWnd), SMP_ITEM_HEIGHT,
				0,SMP_TOOLBAR_ITEMCOUNT, 0);
			SMP_Toolbar_SetStrings(hControl,  STR_OK, STR_SELECT, STR_BACK, FALSE);
			SGL_AddChildWindow(hWnd, hControl);

			//SGL���ڵĹ��̣�����ͨ�õ�SGL����'x=0,y=29+180;w=240;h=26;��ʾ���ڿ���ֹͣ���㡣����ʽ������WS_TABSTOP��ʹ�õ�ʱ��WS_FOCUSCHILD setted �Ӵ��ڻ�ý��㡣
			hControl2 = SGL_CreateWindow(SGL_Window_WndProc,
				5, SMP_HEADER_HEIGHT , SCREEN_WIDTH-10, SCREEN_HEIGHT,
				MAINWND_SETZHK, WS_TABSTOP | WS_FOCUSCHILD|WS_TRANSPARENT, 0);
			SGL_AddChildWindow(hWnd, hControl2);


			//��Ͽ����x=y=0,w=70,h=26��ʾ���ڿ���ֹͣ����
			hControl = SGL_CreateWindow(SMP_ComboBox_WndProc,
				0, yy, _WIDTH(hControl2), SMP_TOOLBAR_HEIGHT,
				MAINWND_SETZT, WS_TABSTOP, 0);
			yy+=SMP_TOOLBAR_HEIGHT;
			//������Ͽ��б���
			SMP_ComboBox_SetItems(hControl, miFONT, TABLESIZE(miFONT));
			SGL_AddChildWindow(hControl2, hControl);

			//���ڴ��С(kb)
			hControl = SGL_CreateWindow(SMP_Label_WndProc,
				0, yy, _WIDTH(hControl2), SMP_TOOLBAR_HEIGHT,
				0, WS_TRANSPARENT, 0);
			yy+=SMP_TOOLBAR_HEIGHT;
			//���ñ�ǩ����
			SMP_Label_SetContent(hControl, RESID_INVALID, SGL_LoadString(STR_HEAP_SIZE), 0);
			SGL_AddChildWindow(hControl2, hControl);
			//���ڴ��С ����
			hControl = SGL_CreateWindow(SMP_Edit_WndProc,
				0, yy, _WIDTH(hControl2), SMP_TOOLBAR_HEIGHT,
				MAINWND_SETHEAP, WS_TABSTOP, 0);
			yy+=SMP_TOOLBAR_HEIGHT;
			//���ñ༭��Ϣ
			SMP_Edit_SetInfo(hControl, (PCWSTR)SGL_LoadString(STR_HEAP_SIZE), (PWSTR)Set_text_HEAP, 15);
			SGL_AddChildWindow(hControl2, hControl);


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
			switch(FONT)
			{
			case 0:	case 1:	case 2:
				temp=FONT;
				break;
			case 12:
				temp=3;
				break;
			case 16:
				temp=4;
				break;
			}
			SMP_ComboBox_SetSelectedItem(SGL_FindChildWindow(hControl,MAINWND_SETZT),temp,1,1);
			sprintf((char *)tmp,"%d",SET_HEAP_SIZE);
			wstrcpy((PWSTR)Set_text_HEAP,(PCWSTR)CharToUnicode((PSTR)tmp));

		}
		break;

	case WM_COMMAND://�����Ӵ��ڵ�֪ͨ��Ϣ
		{
			WID id = LOWORD(wParam);//�˵��¼�
			WORD code = HIWORD(wParam);//�˵�����

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
	case WM_KEYUP://�û��ͷ���һ������
		{
			switch(wParam)//p1
			{
			case MR_KEY_SOFTLEFT://�����
				{
					int temp=1;
					hControl=SGL_FindChildWindow(hWnd,MAINWND_SETZHK);

					switch(SMP_ComboBox_GetSelectedItem(SGL_FindChildWindow(hControl,MAINWND_SETZT)))
					{
					case 0:
					case 1:
					case 2:
						FONT=temp;
						break;
					case 3:
						FONT=12;
						break;
					case 4:
						FONT=16;
						break;
					}
					SetFont(FONT);
					SET_HEAP_SIZE=atoi((const char *)UnicodeToChar((PWSTR)Set_text_HEAP));
					IniSetSave();
				
				}
			case MR_KEY_SOFTRIGHT://�����
				HideTopWindow(TOPWND_BANGZHU, TRUE, FALSE);
				ShowTopWindow(TOPWND_MAINWND, NULL, (DWORD)TOPWND_MAINWND);
				return 1;

			}
			break;
		}
	}
	return SMP_MenuWnd_WndProc(hWnd, Msg, wParam, lParam);
}


