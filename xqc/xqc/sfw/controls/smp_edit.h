#ifndef _SMP_EDIT_H
#define _SMP_EDIT_H

#include "window.h"

/**
  \�༭��

  ��Ҫʹ�ñ༭��
   - ����һ���༭��һ���༭����������е�һ�����
 - ES_NUM��֧�ֺ�������
 - ES_PWD��֧����������
 - ES_ALPHA��֧��alpha�����֣��ַ�������
   - ���ñ༭����Ϣ
   - ��ӱ༭�򵽸�����
   - ��Ӧ�༭����Ϣ

  \code
  	//�����༭��
  	HWND hEdit = SGL_CreateWindow(SMP_Edit_WndProc, ...);
  	SMP_Edit_SetInfo(hEdit, title, text, maxsize);
  	SGL_AddChildWindow(hWnd, hEdit);

  	//��Ӧ��Ϣ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the edit id")
		{
			HWND hEdit = (HWND)lParam;
			switch(code)
			{
			case SMP_EDITN_TEXTCHANGED:
				//�����¼���
				break;
			}
		}
  \endcode


������ʽ

��ʾ�������Ҷ������ʽ��
*/
#define SMP_EDITS_HRIGHT		0x0001L
/**
��ʾ��ɾ�����е��������ݺͻ��з�����
 */
#define SMP_EDITS_TRIM_CR_LF    0x0002L

#define SMP_EDITS_DISABLED    0x0004L

#define SMP_EDITS_TRIM_SPACE    0x0008L
/**
������Ϣ

 * \�༭�ı����ı�֪ͨ��Ϣ
 *
 * \code����
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the edit id" && code == SMP_EDITN_TEXTCHANGED)
 *		{
 *			HWND hEdit = (HWND)lParam;
 * 			//handle the button click notify message
 *		}
 * \endcode
 *
 * \����hEdit	�༭���ڷ��ʹ���Ϣ
 */
#define SMP_EDITN_TEXTCHANGED		0x0001

	/** @} */

/**
 * �����˸ʱ����(��λ:����)
 */
#define SMP_EDIT_BLINK_INTERVAL		300 //ms

	/**
	 * \name Window Member Functions
	 * @{
	 */

/**
 * \���ñ༭��Ϣ
 *
 * \ע�⣺���ô˺���֮ǰ������ӵ�������
 *
 * \����hEdit					�༭����Ʒ���
 * \����title					����ʾ�����봰�ڵı����ַ���
 * \����[���룬���]text		�����������ֵĻ�����
 * \����MAXSIZE					�ı���������С
 */
VOID SMP_Edit_SetInfo(HWND hEdit, PCWSTR title, PWSTR text, int maxsize);

/**
 * \��ȡ�༭��Ϣ
 *
 * \����hEdit				�༭����
 * \����[���]title			�����ַ���
 * \����[���]text			������
 * \����[���]MAXSIZE		�������Ĵ�С
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_Edit_GetInfo(HWND hEdit, PCWSTR* title, PWSTR* text, int* maxsize);

/**
 * \��ȡ�ı�
 *
 * \����hEdit	�༭���ھ��
 * \�����ı�����
 */
PWSTR SMP_Edit_GetText(HWND hEdit);

/**
 * \����һ���µ��ַ���
 *
 * \����hEdit		�༭���ھ��
 * \����pstr		���ַ���
 * \����redraw		�Ƿ��ػ洰��
 * \����notify		�Ƿ���SMP_EDITN_TEXTCHANGED֪ͨ��Ϣ��������
 */
VOID SMP_Edit_SetText(HWND hEdit, PCWSTR pstr, BOOL redraw, BOOL notify);

/**
 * \���������˸
 *
 * \����hEdit	���ھ��
 * \����update	��ɺ��Ƿ��ػ�
 */
VOID SMP_Edit_StartBlink(HWND hEdit, BOOL update);

/**
 * \ֹͣ�����˸
 *
 * \����hEdit	���ھ��
 * \����update	��ɺ��Ƿ��ػ�
 */
VOID SMP_Edit_StopBlink(HWND hEdit, BOOL update);

/**
 * \���ù���λ��
 *
 * \����hEdit	�༭���ھ��
 * \����POS		���λ��
 * \����update	��ɺ��Ƿ��ػ�
 */
VOID SMP_Edit_SetCursorPosition(HWND hEdit, int pos, BOOL update);

	/** @} */

	/**
	 * \name Window Procedure���ڳ���
	 * @{
	 */
VOID SMP_Edit_AdjustPosEnd(HWND hEdit);

/**
 * \�༭���ڹ���
 *
 * \����hEdit		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_Edit_WndProc(HWND hEdit, UINT Msg, WPARAM wParam, LPARAM lParam);

void SMP_Edit_SetIntroduceText(HWND hEdit, PCWSTR pIntroduce);

	/** @} */

/** @} end of smp_edit */

#endif /* _SMP_EDIT_H */

