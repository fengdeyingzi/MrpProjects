#ifndef _TOPWND_H
#define _TOPWND_H

#include "window.h"

/**
* \��̵Ķ��㴰��ID��ʼ
 */
#define TOPWND_ID_START		300


/**
* \��̵Ķ�������ID

*

*���㴰��IDӦ����һ����Ŀ

* inittopwindowʵ��
 */
enum {
	TOPWND_MAINWND = TOPWND_ID_START,
	//�����������Ķ��㴰��ID
	
	TOPWND_GUANYU, //����
	TOPWND_BANGZHU,
	TOPWND_CHARU,
	TOPWND_OPENDLG,
	TOPWND_SET,
	TOPWND_PALETTE,
	TOPWND_KEYPAD,

	MAX_TOPWND_COUNT
};

/**
* \\���ݵĳ�ʼ���Ķ���

*

*�������Ӧ�ñ���Ϊinitapplication��
 */
VOID InitTopWindow(VOID);

/**
* \\�򵥵ö���

*

* \\����WID��������ID

* \\����
 */
HWND GetTopWindow(WID wid);

/**
* \\������ʾǰ��

* \\����WID��������ID

* \\�����������ڼ�����������

* \\�����û������û��ض�������

* \\���ض����Ĵ��ھ������ʱʧ��
 */
HWND ShowTopWindow(WID wid, HWND listener, DWORD userdata);

/**
* \\��̵����صĶ���

*

* \\����WID��������ID

* \\�����ƻ�������𴰿�

* \\�����ػ����ˢ�¶���
 */
VOID HideTopWindow(WID wid, BOOL destroy, BOOL redraw);


#endif
