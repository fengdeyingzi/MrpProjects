/*
** window.h: the window header file, contains most simple gui library apis.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#ifndef _SGL_WINDOWS_H
#define _SGL_WINDOWS_H

#include "types.h"
#include "font.h"
#include "gal.h"


/**

 - GUI���ʼ��
 - �������㴰��
 - �����������ڵ�����

  \code
  	int InitApplication(VOID)
  	{
		HWND hWnd;
  		...

  		hWnd = SGL_CreateWindow(WND1_WndProc, ...);
  		SGL_AddChildWindow(HWND_DESKTOP, hWnd);
  		...
  	}
  \endcode

 �������㴰��
SGL��һ�����ڶ�Ӧһ�����ڹ�������Ӧ������Ϣ��
  \code
	LRESULT WND1_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch(Msg)
		{
		case WM_CREATE:
			//�����Ҫ���ڴ棬������һЩ�Ӵ���.
			break;			
		case WM_DESTROY:
			//�ͷ���Դ
			break;
		case WM_SHOW:
			//�ڴ�����£������Ը����Ӵ��ڵ�λ�ú͵�������.
			break;
		case WM_HIDE:
			break;
		case WM_MOUSEDOWN:
			//������갴���¼�
			break;
		case WM_MOUSEUP:
			//��������ͷ��¼�
			break;
		case WM_KEYDOWN:
			//�����������¼�
			break;			
		case WM_KEYUP:
			//�������ͷ��¼�
			break;		
		case WM_COMMAND:
		    //�����Ӵ��ڵ�֪ͨ��Ϣ
			break;
        //������Ϣ
		... 
		}
		return 0;
	}
  \endcode
*/


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


/**
 * ƽ̨���֧�ֵĴ��ھ������,�ؼ�Ҳ��һ������
 */
#ifndef SGL_MAX_WINDOW_COUNT
#define SGL_MAX_WINDOW_COUNT		60
#endif

/**
 * \��������ʱ��������ʱ������
 */
#ifndef SGL_KEYREPEAT_START_INTERVAL
#define SGL_KEYREPEAT_START_INTERVAL		300
#endif

/**
 * \��������ʱ����ʱ������
 */
#ifndef SGL_KEYREPEAT_INTERVAL
#define SGL_KEYREPEAT_INTERVAL			100
#endif 


/**
 * \defgroup windows Windows Manangement
 * @{
 */

	/**
	 * \defgroup wintypes Windows Data Types
	 * @ingroup Windows
	 * @{
	 */

/**
 * ���ڹ���SGL_WndProc���Ͷ���
 */
typedef LRESULT (*SGL_WndProc)(HWND, UINT, WPARAM, LPARAM);

/**
 * \brief A type definition for application events filter.
		һ��Ӧ���¼����������͵Ķ���
 */
typedef LRESULT (*SGL_AppEventFilter)(UINT, WPARAM, LPARAM);

/**
 * \brief A type definition for SGL window in the system.
 */
typedef struct SGL_Window
{
	WID id;							/* ���ڱ�ʶ��*/
	Uint16 reserve;					/* ����*/
	DWORD style;					/* ������ʽ,��WS_TRANSPARENT�ȴ�����ʽ*/
	int left, top, width, height;	/* �����ڸ������������*/
	Uint32 bgcolor, fgcolor;		/* ������ǰ����ɫ*/	
	SGL_WndProc wndproc;			/* ���ڹ���*/	
	HWND parent;					/* ���ָ����ڡ������Ƕ��㴰��ʱ����������HWND_DESKTOP */	
	HWND child;						/* ��һ���Ӵ���hanlde*/	
	HWND next;						/* ��һ��ͬ�����ھ��*/	
	HWND prev;						/* ��һ��ͬ�����ھ��*/	
	HWND focus;						/* �����Ӵ���*/	
	HWND listener;					/* һ���������ڣ�������SGL_NotifyParent��������Ϣ����*/
	DWORD userdata;					/* �û�����*/
	DWORD windata;					/* ��������*/
}SGL_Window;


/**
 * \һ�����ڵ�id
 */
#define _WID(hWnd) \
	(hWnd)->id

/**
 * \һ�����ڵ���ʽ
 */
#define _STYLE(hWnd) \
	(hWnd)->style

/**
 * \�����һ��������ʽ
 */
#define _SET_STYLE(hWnd, style) \
	(_STYLE(hWnd) |= (style))

/**
 * \�����һ��������ʽ
 */
#define _CLR_STYLE(hWnd, style) \
	(_STYLE(hWnd) &= ~(style))

/**
 * \����Դ����Ƿ���ĳ����ʽ
 */
#define _IS_SET_ANY(hWnd, style) \
	(_STYLE(hWnd) & (style))

/**
 * \����Դ����Ƿ��ж�����ʽ
 */
#define _IS_SET_ALL(hWnd, style) \
	((_STYLE(hWnd) & (style)) == (style))

/**
 * \��ĳ�ִ�����ʽ�������ã��б��ޣ��ޱ���
 */
#define _REV_STYLE(hWnd, style)		\
do{									\
if(_IS_SET_ANY(hWnd, style))		\
	_CLR_STYLE(hWnd, style);		\
else								\
	_SET_STYLE(hWnd, style);		\
}while(0)

/**
 * \�������λ��
 */
#define _LEFT(hWnd) \
	(hWnd)->left

/**
 * \���ڶ���λ��
 */
#define _TOP(hWnd) \
	(hWnd)->top

/**
 * \���ڿ��
 */
#define _WIDTH(hWnd) \
	(hWnd)->width

/**
 * \���ڸ߶�
 */
#define _HEIGHT(hWnd) \
	(hWnd)->height

/**
 * \���ھ�������
 */
#define _WINRECT(hWnd, r) \
	(r).left = _LEFT(hWnd), \
	(r).top=_TOP(hWnd), \
	(r).width=_WIDTH(hWnd), \
	(r).height=_HEIGHT(hWnd)

/**
 * \���ڵı�����ɫ
 */
#define _BGCOLOR(hWnd) \
	(hWnd)->bgcolor

/**
 * \����ǰ��ɫ
 */
#define _FGCOLOR(hWnd) \
	(hWnd)->fgcolor

/**
 * \���ڵĴ��ڹ���
 */
#define _WNDPROC(hWnd) \
	(hWnd)->wndproc

/**
 * \���ڵ��û�����
 */
#define _USERDATA(hWnd) \
	(hWnd)->userdata

/**
 * \���ô����û�����
 */
#define _SET_USERDATA(hWnd, data) \
	_USERDATA(hWnd) = (DWORD)(data)

/**
 * \һ���ض����͵Ĵ����û�����
 */
#define _GET_USERDATA(hWnd, type) \
	((type)_USERDATA(hWnd))

/**
 * \HWND�Ĵ�������
 */
#define _WINDATA(hWnd) \
	(hWnd)->windata

/**
 * \������HWND�Ĵ�������
 */
#define _SET_WINDATA(hWnd, data) \
	_WINDATA(hWnd) = (DWORD)(data)

/**
 * \һ���ض����ʹ��ڵĴ�������
 */
#define _GET_WINDATA(hWnd, type) \
	((type)_WINDATA(hWnd))

/* ������ */
#define _PARENT(hWnd) \
	(hWnd)->parent

/* ���ڵĺ�һ���ڵ� */
#define _NEXT(hWnd) \
	(hWnd)->next

/* ���ڵ�ǰһ���ڵ� */
#define _PREV(hWnd) \
	(hWnd)->prev

/* �Ӵ��� */
#define _CHILD(hWnd) \
	(hWnd)->child

/**
 * \����/ģ̬���ڵĽ����Ӵ���
 */
#define _FOCUS(hWnd) \
	(hWnd)->focus

/* ��ǰ���ڵļ������� */
#define _LISTENER(hWnd) \
	(hWnd)->listener

/**
 * ���洰��
 */
#define HWND_DESKTOP	(HWND)(0xFFFFFFFF)

/**				
 * \���hWnd�ǳ�̬����
 */
#define IS_NORMAL_WINDOW(hWnd) \
	(NULL != hWnd && HWND_DESKTOP != hWnd)

//���hWnd���ǳ�̬����
#define NOT_IS_NORMAL_WINDOW(hWnd) \
    (!IS_NORMAL_WINDOW(hWnd))

/**
 * \brief A type definition for edit control information.
 */
typedef struct SGL_EditInfo
{
	/** title displayed on ime window ��ʾ�����뷨���ڵı���*/
	PCWSTR title; 
	/** the edit content buffer�༭������������ */
	PWSTR buffer;
	/** the buffer size��������С */
	Uint32 size;
}SGL_EditInfo;

	/** @} end of wintypes */

	/**
	 * \defgroup winmsg Windows Messages
	 * @ingroup Windows
	 * @{
	 */

/**
 * \��갴����Ϣ
 *
 * \code
 * case WM_MOUSEDOWN:
 *	int x = (int)wParam;
 *	int y = (int)lParam;
 * \endcode
 *
 * \����x		����������xλ��
 * \����y		����������yλ��
 */
#define WM_MOUSEDOWN		0x3001

/**
 * \����ͷ���Ϣ
 *
 * \code
 * case WM_MOUSEUP:
 *	int x = (int)wParam;
 *	int y = (int)lParam;
 * \endcode
 *
 * \����x		����������xλ��
 * \����y		����������yλ��
 */
#define WM_MOUSEUP			0x3002

/**
 * \����ƶ�����Ϣ
 *
 * \code
 * case WM_MOUSEMOVE:
 *	int x = (int)wParam;
 *	int y = (int)lParam;
 * \endcode
 *
 * \����x		����������xλ��
 * \����y		����������yλ��
 */
#define WM_MOUSEMOVE		0x3003

/**
 * \�û�����һ����
 *
 * ���û�����һ����������Ϣ���͵���ǰ�����
 * \code
 * case WM_KEYDOWN:
 *	DWORD key = wParam;
 * \endcode
 *
 * \����key		���µļ��ļ���
 */
#define WM_KEYDOWN		0x3004

/**
 * \�û��ͷ���һ������
 *
 * ���û��ͷ�һ����ʱ������Ϣ���͵���ǰ����ڡ�
 * \code
 * case WM_KEYUP:
 *	DWORD key = wParam;
 * \endcode
 * ����key		�ͷŵļ��ļ���
 */
#define WM_KEYUP		0x3005

/**
 * \��ʾ�����Ѿ�������뽹��
 * ����Ϣ�����͵����ڹ��̺�Ĵ��ڻ�����뽹�㡣
 */
#define WM_SETFOCUS		0x3006

/**
 * \��ʾ�ô����Ѿ�ʧȥ���뽹��
 * ����Ϣ�����͵����ڹ��̺�Ĵ���ʧȥ���뽹�㡣
 */
#define WM_KILLFOCUS		0x3007

/**
 * \��ʾ�����Ѿ�������������һ����������ʼ������CODE��
 *
 * ����Ϣ�����͵����ں󣬴����Ѵ�����
 * �����Գ�ʼ���Լ��Ķ��󣬵����յ�����Ϣ��
 * �������㵽ϵͳ��ʾ���Գ�ʼ���ĳɹ���
 * ������ط��㵽ϵͳ�������Ϣ�󣬴����Ĵ��ڽ������١�
 */
#define WM_CREATE		0x3008

/**
 * \��ʾ�������ٸô��ڡ�
 *
 * ����SGL_DestroyWindowʱ,����Ϣ�����͵����ڡ�
 * ���յ�����Ϣ������������CODE��
 */
#define WM_DESTROY		0x3009

/**
 * \�ػ洰��
 * \code 
 * case WM_PAINT:
 *	PRECT pRgn = (PRECT) wParam;
 * \endcode
 *
 * \����pRgn    ��Ч���򣬵�NULL��ζ�����еĴ�������
 * \�Ӵ��ڹ��̷��ط���ָ�����Լ����������Ӵ��ڣ�SGL��������WM_PAINT��Ϣ�������Ӵ���
 */
#define WM_PAINT		0x300A

/**
 * \���͵�����ʱ��������ʾ
 * ����Ϣ���͵�����ʱ�����ڱ�ÿɼ���
 */
#define WM_SHOW		0x300B

/**
 * \���͵�����ʱ����������
 * 
 * ����Ϣ���͵����ڵ��Ӵ���ʱɾ�����ĸ����ڣ�
 * ��һ�����㴰�ڱ���һ���ص���
 */
#define WM_HIDE			0x300C

/**
 * \�����Ϣ��ʾ�������Ӵ��ڵ�֪ͨ��Ϣ
 * ����Ϣ���͵�����ʱһ���Ӵ����ѷ���֪ͨ��Ϣ�������ڡ�
 *
 * \code 
 * case WM_COMMAND:
 *	WID id = LOWORD(wParam);
 *	WORD code = HIWORD(wParam);
 * \endcode
 *
 * \����id		�Ӵ���ID
 * \����code	�Ӵ���֪ͨ��Ϣ����
 */
#define WM_COMMAND		0x300D

/**
 * \�˵���ʾʱ���͵�������
 * ��һ���˵�����ʱ������Ϣ���͵��������ø������л���ȥ�������顣
 * \code
 * case WM_MENUSHOW:
 	HMENU hMenu = (HMENU)lParam;
 * \endcode
 *
 * \����hMenu	�����Ĳ˵����
 * \��Ӧ WM_MENUHIDE
 */
#define WM_MENUSHOW		0x3100

/**
 * \�˵��ر�ʱ�����͵�������
 *
 * �˵��ر�ʱ����Ϣ���͵��������ø������л��ᴦ���¼���
 *
 * \code
 * case WM_MENUHIDE:
 *	BOOL redraw = (BOOL)wParam;
 *	HMENU hMenu = (hMenu)lParam;
 * \endcode
 *
 * \����redraw	���˴��ڽ��ػ�
 * \����hMenu	�˵����ھ��
 * \��Ӧ WM_MENUSHOW
 */
#define WM_MENUHIDE		0x3101

/**
 * \\���͵�������ʱ����갴���ڲ˵��������.
 *
 * NOT IMPLEMENTED YET��Ҫִ��
 */
#define WM_MENUMOUSEDOWN	0x3102

/**
 * \��������ڲ˵���ʱ�����͵���������
 * ����Ϣ���͵������ڵ��������Ĳ˵���
 * ������0���رղ˵����������˵���
 * \code 
 * case WM_MENUMOUSEUP:
 *	int x = (int)wParam;
 *	int y = (int)lParam;
 * \endcode
 *
 * \����x	�����ڵ����xλ��
 * \����y	�����ڵ����yλ��
 */
#define WM_MENUMOUSEUP		0x3103

/**
 * \brief GUI framework send this message to get the submenu handle
 * \GUI��ܷ��ʹ���Ϣ�õ��Ӳ˵����
 * ��һ�������˵���
 *
 * �˵����ڱ���ʵ��������˵���
 *
 * \code
 * case WM_GETSUBMENU:
 *	return hSubMenu;  //�����Ӳ˵����
 * \endcode
 *
 * \param[out] pMenu the pointer to the sub menu handle return to framework
 ����[��] pMenu	    �Ӳ˵�����ָ�봦����
 */
#define WM_GETSUBMENU		0x3104

/**
 * \ģ̬������ʾʱ���͵�������
 *
 * \code
 * case WM_MODALSHOW:
 *	HWND hDlg = (HWND)lParam;
 * \endcode
 *
 * \����hDlg	ģ̬���ھ��
 */
#define WM_MODALSHOW		0x3200

/**
 * \ģ̬���ڹر�ʱ���͵�������
 *
 * \code
 * case WM_MODALHIDE:
 *	HWND hDlg = (HWND)lParam;
 * \endcode
 *
 * \����hDlg	ģ̬���ھ��
 */
#define WM_MODALHIDE		0x3201

/**
 * \���͵�������ʱ��갴����ģ̬����
 * \code
 * case WM_MODALMOUSEDOWN:
 * 	int x = (int)wParam;
 *	int y = (int)lParam;
 * \endcode
 *
 * \����x   ��������������xλ��
 * \����y   ����yλ��SKY��ڹ��
 */
#define WM_MODALMOUSEDOWN	0x3202

/**
 * \brief Sent to parent window when mouse up out of the modal window.
 * \���͵������ڵ���������ģ̬����
 * \code
 * case WM_MODALMOUSEUP:
 * 	int x = (int)wParam;
 *	int y = (int)lParam;
 * \endcode
 * \����xΪ��������������xλ��
 * \����y������yλ��SKY_ ��ڹ��
 */
#define WM_MODALMOUSEUP		0x3203

/**
 * \GUI��ܰ������Ϣ���͵����ڣ����ڻ�ȡ��Ϣ��
 *
 * �ڱ༭���ڱ���ʵ�������Ϣ��
 *
 * \code 
 * case WM_GETEDITINFO:
 *		SGL_EditInfo* info = (SGL_EditInfo*)wParam;
 *		info->title = GET_TITLE(hEdit);
 *		info->buffer = GET_BUFFER(hEdit);
 *		info->size = GET_MAXSIZE(hEdit);
 * \endcode
 *
 * \����[���]info   ���ؿ�ܵı༭��Ϣ
 */
#define WM_GETEDITINFO		0x3300

/**
 * \���͵��༭���ڣ���������ɣ�ȡ�����ᷢ�ͣ�
 * �༭���ڱ���ʵ�ִ�ϵͳ�������뷨������
 * \code
 * case WM_SETEDITTEXT:
 *	PCWSTR pContent = (PCWSTR)wParam;
 * \endcode
 *
 * \����pContent	��ϵͳ���뷨���յ��ı�����
 */
#define WM_SETEDITTEXT		0x3301

/**
 * \���͵������ģ̬��������ʼ��Ĭ�Ͻ�����Ӵ���
 */
#define WM_INITFOCUS		0x3302

/**
 * \�������¼�
 * Ϊ��ʹ����Ӧ�ÿ���ENABLE_SGL_KEYREPEAT��
 */
#define WM_KEYDOWNREPEAT		0x3303

/**
 * \��һ���û��������Ϣ��
 * SGL�����ķ�Χ��0x3800��0xEFFF�û��������Ϣ��
 * WM_USER�ǵ�һ���û��������Ϣ�������ʹ������
 */
#define WM_USER			0x3800


#define WM_GETFILE      0x3801
/**
 * \���һ���û�����Ϣ.
 *
 * �û�����Ϣ���ܳ������ֵ
 */
#define WM_LASTUSERMSG		0xEFFF

/**
 * \��������һ�����㴰��
 *
 * \ע��������ʽ���Կ�ܡ�Ӧ�ù���ʦ��Ӧ�����û�������ִ�����ʽ��
 */
#define WS_OVERLAPPED		0x80000000L

/**
 * \��ʾ����һ���Ӵ���
 *
 * \ע��������ʽ���Կ�ܡ�Ӧ�ù���ʦ��Ӧ�����û�������ִ�����ʽ��
 */
#define WS_CHILD			0x40000000L

/**
 * \��ʾ����һ�������˵�
 *
 * ����ʽ����������ʾһ�������˵�����һ���˵�popuped�ڶ���/ģ̬���ڣ�
 * �����ڽ��õ���ʽ��WS_INACTIVE | WS_POPUP������ˣ�һ�������˵�������
 * ����ʽWS_POPUP������WS_INACTIVE��ʽҲӦ��顣
 *
 * \code
 *	if(!_IS_SET_ANY(hWnd, WS_INACTIVE) && _IS_SET_ANY(hWnd, WS_POPUP))
 *	{
 *		//��һ���˵�
 *	}
 * \endcode
 *
 * \ע��������ʽ���Կ�ܡ�Ӧ�ù���ʦ��Ӧ�����û�������ִ�����ʽ��
 */
#define WS_POPUP			0x20000000L

/**
 * \��ʾ����һ��ģ̬����
 *
 * ��ֻ����������������ʽģ̬���ڡ��������洰����ʾһ��ģ̬��
 * �����ڽ��õ���ʽ��WS_INACTIVE | WS_MODAL������ˣ�һ��ģ̬���ڲ���ֻ
 * ����ʽWS_MODAL������WS_INACTIVE��ʽҲӦ��顣
 *
 * \code
 *	if(!_IS_SET_ANY(hWnd, WS_INACTIVE) && _IS_SET_ANY(hWnd, WS_MODAL))
 *	{
 *		//��һ��ģ̬����
 *	}
 * \endcode
 *
 * \ע��������ʽ���Կ�ܡ�Ӧ�ù���ʦ��Ӧ�����û�������ִ�����ʽ��
 */
#define WS_MODAL			0x10000000L

/**
 * \��ʾ���ڿɼ�
 *
 * \ע��������ʽ���Կ�ܡ�Ӧ�ù���ʦ��Ӧ�����û�������ִ�����ʽ��
 */
#define WS_VISIBLE			0x08000000L

/**
 * ��ʾ�������ڻ�ģ̬���ڴ��ڷǻ״̬
 *
 * һ���ǻ������ָһ�������˵��Ķ������ڻ�ģ̬������ʾ��
 * ��ģ̬���ڣ��ڶ��㴰���С�
 * ��һ�������˵�����ʾ�����ڽ��õ���WS_INACTIVE | WS_POPUP����
 * ��һ��ģ̬��������ʾ�����ڽ��õ���WS_INACTIVE | WS_MODAL����
 *
 * \ע��������ʽ���Կ�ܡ�Ӧ�ù���ʦ��Ӧ�����û�������ִ�����ʽ��
 */
#define WS_INACTIVE			0x04000000L

/**
 * \���ڻ�����뽹�㡣
 *
 * \ע��������ʽ���Կ�ܡ�Ӧ�ù���ʦ��Ӧ�����û�������ִ�����ʽ��
 */
#define WS_FOCUSED			0x02000000L

/**
 * \������͸���ġ�
 *
 * \GUI�����ử�����ڱ�����
 */
#define WS_TRANSPARENT	0x01000000L

/**
 * \��ʾ���ڿ���ֹͣ���㡣
 *
 * ���Ӵ��ڴ�����������ʽ��SGL_FocusNext / SGL_FocusPrev
 * ����ͣ���Ӵ��ڵĽ��㡣
 */
#define WS_TABSTOP			0x00800000L

/**
 * \brief Indicates the sgl should focus the child window except the window itself
 *
 * This style must used with WS_TABSTOP, when WS_FOCUSCHILD setted the 
 * child window will get the focus.
 * \SGL�����Ӵ��ڿ��Եõ�����
 *
 * ����ʽ������WS_TABSTOP��ʹ�õ�ʱ��WS_FOCUSCHILD setted
 * �Ӵ��ڻ�ý��㡣
 */
#define WS_FOCUSCHILD		0x00400000L

/**
 * \brief Restore the focus when shown again.
 *
 * You can create a top-level window with this style.
 * The window will restore the focus window that has the focus before hiden.
 * \��ԭ����ʱ�ٴ���ʾ��
 *
 * �����Դ���һ������������ʽ�Ķ��㴰�ڡ�
 * �ô��ڽ��ָ����н���Ĵ���ǰ���صĽ��㡣
 */
#define WS_KEEPFOCUS		0x00200000L

/**
 * \��ʾ���Ӵ���ɾ��ʱ��ܲ����ػ涥�㴰�ڡ�
 * 
 * \ע��ֻ���ڲ˵���ģ̬������ʹ��������ʽ��
 */
#define WS_NOREDRAW		0x00100000L

/**
 * \��������������Ч��
 *
 * Ҳ����ĳЩ�ؼ�����괦����ʹ��������ʽ��
 */
#define WS_PRESSED			0x00080000L

/**
 * \��ʾ��������ڰ��¼���
 *
 * Ҳ����ĳЩ�ؼ��İ���������ʹ��������ʽ��
 */
#define WS_KEYDOWN			0x00040000L

/**
 * \���ڱ����á�
 *
 * ������ʽӦ���ɿ�����ִ�С�
 */
#define WS_DISABLED		0x00020000L

/**
 * \���ڱ߽硣
 *
 * ������ʽӦ���ɿ�����ִ�С�
 */
#define WS_BORDER			0x00010000L

/**
 * \ϵͳ��ʽλ
 */
#define WS_MASK_SYSTEM	0xffff0000L

/**
 * \�Ӵ���ʽ���û�λ
 */
#define WS_MASK_USER		0x0000ffffL

	/**
	 * \defgroup editstyle Edit input Styles
	 *
	 * For edit window style used by the framework, so their declaration here.
	 * These style should be used exclusively. So one edit can just set one or non of these style. 
	 *
	 * @ingroup winstyle
	 * @{
	 */

/**
 * \�༭������ʽ��������
 */
#define ES_NUM		0x00008000L

/**
 * \�༭������ʽ��������
 */
#define ES_PWD		0x00004000L

/**
 * \�༭������ʽ�����ַ�
 */
#define ES_ALPHA	0x00002000L

	/** @} end of editstyle */

	/** @} end of winstyle */

	/**
	 * \defgroup winapi Windows APIs
	 * @ingroup Windows
	 * @{
	 */

/**
 * \��ʼ��SKY GUI�⣬�����������������Ӧ�ó������ǰ����
 * \��Ӧ SGL_Terminate
 */
VOID GUIAPI SGL_Initialize(VOID);


/**
 * \��ֹSKY GUI�⣬�˳�Ӧ�ó���ʱ����
 * \��Ӧ SGL_Initialize
 */
VOID GUIAPI	SGL_Terminate(VOID);

/**
 * \��ͣ����GDI������
 * \��ֹSGLˢ��, ���ô˺���֮��������ˢ�´��ڲ������ᷴӳ����Ļ��	  
 * \ʹ������������Է�ֹ����ˢ����Ļ, ������Ļ��˸
 * \code 
 * 	SGL_SuspendDrawing();
 *	SGL_UpdateWindow(hWnd); //֮��hWnd����õ�WM_PAINT�¼�
 * \endcode
 * 
 * \��Ӧ SGL_UnsuspendingDrawing
 */
VOID GUIAPI SGL_SuspendDrawing(VOID);

/**
 * \ȡ����ͣGDI����
 *
 * ���ù�SGL_SuspendDrawing�����
 * 
 * \��Ӧ SGL_SuspendDrawing
 */
VOID GUIAPI SGL_UnsuspendDrawing(VOID);

/**
 * \brief Get the drawing lock value.
 *�жϵ�ǰ�����Ƿ��ֹˢ��
 * \return the value of drawing lock
 */
BOOL GUIAPI SGL_IsSuspendDrawing(VOID);

/**
 * \��ȡscreeen������Ĵ�С
 *
 * \����pSize	����������С
 */
VOID GUIAPI SGL_GetScreenSize(PSIZE pSize);

/**
 * \��ȡ��ǰϵͳ����
 *
 * \���ص�ǰϵͳ����
 */
HFONT GUIAPI SGL_GetSystemFont(VOID);

/**
 * \����ϵͳ���塣
 *
 * \����font	������
 */
VOID GUIAPI SGL_SetSystemFont(HFONT font);

/**
 * \�ڸ������а���ʶ�������Ӵ���
 * \����hWnd   �����ھ��
 * \����id	   �Ӵ���ID
 * \�����ҵ��ĵ�һ���Ӵ��ھ��
 */
HWND GUIAPI SGL_FindChildWindow(HWND hWnd, WID id);

/**
 * \���ҵ�x��y���ڵ��Ӵ���
 * \����hWnd	���ھ��
 * \����x	    ��ߵ�λ��
 * \����Y	    �ұߵ�λ��
 * \�����ҵ��Ĵ��ھ�������򷵻�null
 */
HWND GUIAPI SGL_FindChildWindowByPosition(HWND hWnd, int x, int y);

/**
 * \������һ���ؼ�����
 * �˹���retrivesָ����һ
 * ����\һ��HWND��������ݵĲ���ΪNULL
 * ��\һhChild���ú��������ظô��ڵĵ�һ���Ӵ��ڡ�

 * \����hWnd	���ھ��
 * \����hChild	�Ӵ��ڵľ��
 * \������һ���Ӵ��ڵľ��������㴫��NULL��
 * ��\һhChild�����������ᴫ�صĵ�һ���Ӵ���
 * ���⡣������Ӵ��ڴ��ݵ����һ����
 * �˺���������NULL�����hWnd�ǲ��Ǹ�
 * ��hChild���������������NULL��
 */
HWND GUIAPI SGL_GetNextChild (HWND hWnd, HWND hChild);

/** 
 * \��ȡ�Ӵ��ڵĶ��㴰�� (��ν���㴰�ھ���������Ļ��ʾ�Ĵ󴰿�, ��HWND_DESKTOP���Ӵ���, ���ǿؼ�����)
 * \����hWnd	�Ӵ��ھ��
 * \���ض��㴰��
 */
HWND GUIAPI SGL_GetTopWindow(HWND hWnd);

/**
 * �Ѵ�������ϵת��Ϊ��Ļ����ϵ
 * ���ڵ�λ����Ϣ������ڸ����ڵ����λ��, �ú������Ӵ��ڵ�λ������ת������Ļ�ľ�������
 * \����hWnd			���ھ��
 * \����[���룬���]	X����
 * \����[���룬���]	Y����
 * 
 * \��Ӧ SGl_ScreenToWindow
 */
VOID GUIAPI SGL_WindowToScreen (HWND hWnd, int* x, int* y);

/**
 * ����Ļ����ϵת��Ϊ��������ϵ
 * ���ڵ�λ����Ϣ������ڸ����ڵ����λ��, �ú�������Ļ�ľ�������ת���ɴ��ڵ�λ������
 *
 * \����hWnd			���ھ��
 * \����[���룬���]	X����
 * \����[���룬���]	Y����
 * 
 * \��Ӧ SGL_WindowToScreen
 */
VOID GUIAPI SGL_ScreenToWindow (HWND hWnd, int* x, int* y);

/**
 * \����SGL����
 *
 * ����:
 * WndProc		���ڻص�
 * X			���ڵ����λ��
 * y			���ڵĶ���λ��
 * w			���ڵĿ��
 * h			���ڵĸ߶�
 * id			����ID
 * style		������ʽ
 * userdata		�û�����
 *
 * ���أ� 
 *	- �ɹ�����	���ھ��
 *	- ʧ�ܷ���	NULL
 */
HWND GUIAPI SGL_CreateWindow(SGL_WndProc wndproc, int x, int y, int w, int h, WID id, DWORD style, DWORD userdata);

/**
 * \����SGL_CreateWindow�����Ĵ���
 *
 * \����hWnd	�����ٴ��ھ��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL GUIAPI SGL_DestroyWindow(HWND hWnd);

/**
 * \������Ϣ��һ������
 *
 * \����hWnd	���ھ��
 * \����Msg		������Ϣ
 * \����wParam	��һ������
 * \����lParam	�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT GUIAPI SGL_SendMessage(HWND hWnd, UINT Msg, DWORD wParam, DWORD lParam);

/**
 * \������Ϣ��HWND�������е��Ӵ���
 *
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \����hExcept		��������Ϣ�Ĵ���
 * \������Ϣ�Ĵ�����
 */
VOID GUIAPI SGL_BroadcastMessageEx(HWND hWnd, UINT Msg, DWORD wParam, DWORD lParam, HWND hExcept);

/**
 * \������Ϣ��HWND�������е��Ӵ���
 *
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
#define SGL_BroadcastMessage(hWnd, Msg, wParam, lParam) \
	SGL_BroadcastMessageEx(hWnd, Msg, wParam, lParam, NULL)

/**
 * \������Ϣ��һ������
 *
 * \����hWnd		���ھ��
 * \����id			֪ͨ���ڵ�ID
 * \����code		֪ͨ����
 * \����userdata	message��Ϣ
 * \���ظ���Ϣ�Ĵ�����
 */
#define SGL_SendNotifyMessage(hWnd, id, code, userdata) \
	SGL_SendMessage(hWnd, WM_COMMAND, (WPARAM)MAKELONG(id, code), (LPARAM)userdata)

/**
 * \������Ϣ��������
 *
 * \����hWnd		���ھ��
 * \����code		��Ϣ����
 * \����userdata	�û���Ϣ
 * \���ظ���Ϣ�Ĵ�����
 */
#define  SGL_NotifyParent(hWnd, code, userdata) \
	SGL_SendNotifyMessage((_LISTENER(hWnd)? _LISTENER(hWnd) : _PARENT(hWnd)), _WID(hWnd), code, userdata)

/**
 * \��鴰���Ƿ�ɼ�
 *
 * \����hWnd	���ھ��
 * \�ɼ�����TRUE�����򷵻�FALSE
 */
#define SGL_IsWindowVisible(hWnd) \
	(IS_NORMAL_WINDOW(hWnd) && _IS_SET_ANY(hWnd, WS_VISIBLE))

/**
 * \��鴰�ڴ��ڻ״̬
 *
 * \����TRUEʱ��û�е����˵���ģ̬�����ڶ��㴰����ʾ
 */
#define SGL_IsWindowActive(hWnd) \
	(!_IS_SET_ANY(hWnd, WS_INACTIVE))

/**
 * \�����ض��Ĵ���
 *
 * \����hWnd		Ҫ���µĴ��ھ��
 * \����pClipRect	��������
 */
VOID GUIAPI SGL_UpdateWindowEx(HWND hWnd, PRECT pClipRect);

/**
 * \ˢ��ָ������(ȫ��ˢ��)
 *
 * \����hWnd	Ҫ���µĴ��ھ��
 */
#define SGL_UpdateWindow(hWnd)	\
	SGL_UpdateWindowEx(hWnd, NULL)

/**
 * \brief Paint a window with a clip region. \�������������Ķ��ⴰ��
 *
 * \����hWnd		���ھ��
 * \����pClipRect	�ü�����
 * \ע��ͨ����Ӧ��ֻʹ��SGL_UpdateWindowEx��SGL_UpdateWindow 
 * ��SGL_PaintWindowEx�ոջ��ƴ��ڣ�����ˢ�µ���Ļ�ϣ�Ҳ����
 * ����ͼ���ʹ��ڿɼ���״̬��
 * \��Ӧ SGL_UpdateWindowEx, SGL_UpdateWindow
 */
VOID GUIAPI SGL_PaintWindowEx(HWND hWnd, PRECT pClipRect);

/**
 * \�����������
 *
 * \����hWnd	���ھ��
 * \ע����SGL_PaintWindowEx��ֻͬ��û�м�������
 *	
 * \��Ӧ SGL_PaintWindowEx, SGL_UpdateWindowEx, SGL_UpdateWindow
 */
VOID GUIAPI SGL_PaintWindow(HWND hWnd);

/**
 * \��Ӵ�����Ϊָ�������ڵ��Ӵ���
 *
 * \����hParent		�����ھ������HWND_DESKTOPʱhChild����Ϊ��������
 * \����hChild		�Ӵ��ھ��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL GUIAPI SGL_AddChildWindow(HWND hParent, HWND hChild);

/**
 * \�Ӹ����ڽڵ�ɾ�����Ӵ��ڽڵ�
 *
 * ����hChild	�Ӵ��ھ��
 * �ɹ�����TRUE�����򷵻�FALSE
 */
BOOL GUIAPI SGL_RemoveChildWindow(HWND hChild);


/**
 * \ƽ̨�����ô˺���������GUI��
 *
 * \����Msg		������Ϣ
 * \����wParam	��һ����Ϣ����
 * \����lParam	�ڶ�����Ϣ����
 */
VOID GUIAPI SGL_DispatchEvents(UINT Msg, WPARAM wParam, LPARAM lParam);

/**
 * \���ý��㴰�ڡ�
 * \����hParent		�����ڣ��˴��ڱ�����top-level/modal����
 * \����hChild		�Ӵ���
 */
VOID GUIAPI SGL_SetFocusWindow(HWND hParent, HWND hChild);

/**
 * \��ý��㴰�ڵľ����
 *
 * \������hWnd	top-level/modal����
 * \���ؽ����Ӵ��ھ��
 */
HWND GUIAPI SGL_GetFocusWindow(HWND hWnd);

/**
 * \hHead��hEnd֮���ҵ�һ�����Եõ�������Ӵ���
 *
 * \����hHead   ��һ�����ھ��
 * \����hEnd    ���ҵ����һ�����ھ��
 * \����next    TRUEѰ����һ����FALSEѰ����һ��
 */
HWND GUIAPI SGL_FindFocusNext(HWND hHead, HWND hEnd, BOOL next);

/**
 * \��һ����������Ѱ����һ�������Ӵ���
 *
 * \������hWnd���ھ����Ӧ���Ƕ������ڻ�ģ̬����
 * \����next
 * - TRUE	��һ������
 * - FALSE	ǰһ������
 * \���ؽ����Ӵ��ھ��
 */
HWND GUIAPI SGL_FindFocus4TopWnd(HWND hWnd, BOOL next);

/**
 * \�л�ָ�����ڵ��Ӵ��役��
 *
 * \���� hWnd ���ھ����Ӧ���Ƕ������ڻ�ģ̬����
 * \���� next
 * - TRUE	��һ������
 * - FALSE	ǰһ������
 */
VOID GUIAPI SGL_FocusNext(HWND hWnd, BOOL next);

/**
 * \���������ٴ��ڡ�
 * \������hWnd�������ٴ���
 */
VOID GUIAPI SGL_SetMouseTracker(HWND hWnd);

/**
 * \��ȡ��ǰ�����ٴ��ڡ�
 * \���������ٴ���
 */
HWND GUIAPI SGL_GetMouseTracker(VOID);

/**
 * \��ָ�������ƶ����丸���ڵĵ�һ���Ӵ��ڡ�
 *
 * \����hWnd	�Ӵ��ھ��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL GUIAPI SGL_BringWindowToTop(HWND hWnd);

/**
 * \�رյ����˵���
 *
 * \����hMenu	�˵����
 * \����redraw	ǿ��ˢ��
 */
VOID GUIAPI SGL_CloseMenu(HMENU hMenu, BOOL redraw);

/**
 * \�����˵����ڡ�
 *
 * \����hMenu		�˵����ھ��
 * \����X			�˵������λ��
 * \����y			�˵��Ķ���λ��
 * \����hParent		���յ��˵���Ϣ�ĸ����ھ��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL GUIAPI SGL_PopupMenu(HMENU hMenu, int x, int y, HWND hParent);

/**
 * \ѡ��һ���˵��
 *
 * \����hMenu		�˵����
 * \����item		ѡ��˵����ID
 * \����userdata	�û�����
 */
VOID GUIAPI SGL_SelectMenuItem(HMENU hMenu, WID item, DWORD userdata);

/**
 * \��ȡ��ǰ�����Ĳ˵����ھ����
 *
 * \���ص����˵������NULLʱû�е������ڡ�
 */
HMENU GUIAPI SGL_GetPopupMenu(VOID);

/**
 * \��ģ̬���ڻ��ơ�
 *
 * \����hDlg		���ھ��
 * \����hParent		�����ھ������Ӧ��ΪNULL
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL GUIAPI SGL_DoModal(HWND hDlg, HWND hParent);

/**
 * \�ر�ģ̬����
 *
 * \����hDlg	���ھ��
 * \����code	���ش���
 * \����lParam  �ڶ�������
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SGL_ExitModal(HWND hDlg, WORD code,LPARAM userdata);

/**
 * \��ȡ��ǰģ̬���ڡ�
 *
 * \����ģ̬���ھ������û��ģ̬����ʱ����NULL��
 */
HWND GUIAPI SGL_GetModalWindow(VOID);

/**
 * \����ƽ̨����༭��
 *
 * \����hEdit	�༭���ھ��
 */
VOID GUIAPI SGL_TextInput(HWND hEdit);

/**
 * \����һ��Ӧ�ó����¼���������
 *
 * \����filter	�������ص�
 * \���ؾɵľ��
 */
LRESULT GUIAPI SGL_SetAppEventFilter(SGL_AppEventFilter filter);

#ifdef ENABLE_SGL_KEYREPEAT		

/**
//ֹͣ������ĳ��
 */
VOID GUIAPI SGL_StopKeyRepeat(VOID);

#endif

/**
 * \SGL���ڵĹ��̣�����ͨ�õ�SGL����
 *
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT GUIAPI SGL_Window_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

/**
 * \Ӧ�ó�������,�ɿ�ܵ��á�
 *
 * ��GUI��һЩ����ģ���ʼ������ܵ��øú���
 * ������ʼ��Ӧ�ó��������������
 * �������û�ʵ�֡�
 *
 * \�ɹ�����MR_SUCCESS������MR_FAILED
 */
int InitApplication(VOID); 

/** 
 * \�˳�Ӧ�ó���,�ɿ�ܵ��á�
 *
 * ��Ӧ�ó����˳�����ܵ��øú������û�
 * ��һЩ�˳��������ͷ���Դ���ر�
 * ���д򿪵��ļ��ȣ��⹦�ܱ�����Ӧ�ó���ʵ�֡�
 *
 * \�ɹ�����MR_SUCCESS������MR_FAILED
 */
int ExitApplication(VOID);

/**
Ӧ����ͣ
 */
int PauseApplication(VOID);

/**
 Ӧ�ûָ�
 */
int ResumeApplication(VOID);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SGL_WINDOWS_H */

