#ifndef _SMP_BUTTON_H
#define _SMP_BUTTON_H

#include "window.h"


/**
	��ť

  ��Ҫʹ�ð�ť��
 - ������ť�Ĵ���
 - ���ð�ť�ı����������Ҫ����Ϣ
 - ��ӵ�������
 - ��Ӧ��ť����Ϣ֪ͨ
  \code
	//����һ����ť
	hBtn = SGL_CreateWindow(SMP_Button_WndProc, ...);
	SMP_Button_SetTitle(hBtn, SGL_LoadString(STR_OK));
	SGL_AddChildWindow(hWnd, hBtn);

	//��Ӧ��ť����Ϣ֪ͨ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the button id")
		{
			HWND hBtn = (HWND)lParam;
			switch(code)
			{
			case SMP_BUTTONN_CLICKED:
				//����֪ͨ�¼���
				break;
			}
		}	
  \endcode

*/

/*
������ʽ
��Ҫ˵����ť��һ����ѡ��ť��
 */
#define SMP_BUTTONS_CHECKBOX		0x0001L

/**
 * \��Ҫ˵���İ�ť��һ��radiobox��ť��
 */
#define SMP_BUTTONS_RADIOBOX		0x0002L

/**
 * \�ı������Ҷ������ʽ��
 */
#define SMP_BUTTONS_HRIGHT			0x0004L

/**
 * \���������Ķ������ʽ��
 */
#define SMP_BUTTONS_HCENTER			0x0008L

/**
 * \�������ڴ�ֱ���Ķ��롣
 */
#define SMP_BUTTONS_VCENTER			0x0010L

/**
 * \�������ڴ�ֱ����ײ���
 */
#define SMP_BUTTONS_VBOTTOM			0x0020L

/**
* 
* ָʾ��ť��ѡ�С�
 */
#define SMP_BUTTONS_CHECKED			0x0040L	

/*
����NOTIFY��Ϣ
������Ͱ�ťʱ
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the button id" && code == SMP_BUTTONN_CLICKED)
 *		{
 *			HWND hBtn = (HWND)lParam;
 * 			//����ť���֪ͨ��Ϣ
 *		}
 * \endcode
 *
 * ����hBtn��ť����֪ͨ��Ϣ
*/
#define SMP_BUTTONN_CLICKED			0x0001
/*
���ڳ�Ա����
* \���ð�ť���ڵı��⡣
* \����hBtn	��ť���ھ��
* \����title	�±���
*/
VOID SMP_Button_SetTitle(HWND hBtn, PCWSTR title);

/**
 * \��ȡ��ť�ı��⡣
 * \����hBtn	��ť���ھ��
 * \���ذ�ť���ڵı���
 */
PCWSTR SMP_Button_GetTitle(HWND hBtn);

/**
 * \���ð�ť�ı���λͼ��
 * \����hBtn	��ť���ھ��
 * \����bmp		����״̬�ı���λͼ��ID
 * \����bmp_p	����״̬�ı���λͼ��ID
 */
VOID SMP_Button_SetBitmapGroup(HWND hBtn, DWORD bmp, DWORD bmp_p);

/**
 * \ѡ�а�ť��
 * \����hBtn	��ť���ھ��
 * \����checked
 *	- TRUE		ѡ�а�ť
 *	- FALSE		ȡ��ѡ��
 */
VOID SMP_Button_SetChecked(HWND hBtn, BOOL checked);
/**
���ڳ���
 * \��ť�Ĵ��ڹ��̡�
 * \����hBtn		��ť���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_Button_WndProc(HWND hBtn, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of smp_button */

#endif /* _SMP_BUTTON_H */

