#ifndef _SMP_MSGBOX_H
#define _SMP_MSGBOX_H

#include "window.h"

/**
  ��MsgBox

  ��Ҫʹ�ü�msgbox
  - ��ʾMsgBox
  - ��Ӧ��Ϣ

  \code
  	//ֻ��Ҫ���������������ʾ��MsgBox
  	SMP_MsgBox(...);

  	//��Ӧ��Ϣ��֪ͨ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the msgbox id")
		{
			HWND hMsgbox = (HWND)lParam;
			if(code == ID_OK)
			{
				//����ȷ��

			}else{
				//��ȡ��
			}
		}	
  \endcode

  @ingroup controls
  @{
*/

	/**
	* \name Window Styles
	* @{
	*/

/**
 * \MsgBoxû�а�ť
 */
#define ID_NON		0 //when modal window close by the window manangement system/�ɴ���manangementϵͳģʽ���ڹر�

/**
 * \MsgBox��ȷ����ť
 */
#define ID_OK		0x0001L

/**
 * \MsgBox��ȡ����ť
 */
#define ID_CANCEL	0x0002L

/**
 * \MsgBox��ť�С���/�񡱰�ť
 */
#define ID_YESNO	0x0004L

/**
 * \MsgBox������һ��ʱ���ر�
 * ����ʽֻ��ʹ���˲���һ����ťʱ��Ч��
 */
#define SMP_MSGBOXS_AUTOCLOSE		0x0008L


/**
 * \MsgBox�Զ��ر�ʱ��
 *
 * ����MsgBoxֻ��һ����ťʱ
 * MsgBox����SMP_MSGBOX_AUTOCLOSE_INTERVAL������Զ��ر�
 */
#define SMP_MSGBOX_AUTOCLOSE_INTERVAL	1500 //ms

/**
���ڳ�Ա����

 * \��ʾһ����Ϣ��
 *
 * \����id			��Ϣ��ı��
 * \����hParent		���㴰�ھ��
 * \����hBmp		ͼ��
 * \����title		�����ַ���
 * \����content		�����ַ���
 * \����style		���ڵİ�ť��ʽ������ID_OK��ID_CANCEL��ID_YESNO
 * \����listener	��������֪ͨ��Ϣ������ΪNULLʱ�����ڽ��õ���Ϣ
 */
HWND SMP_MsgBox(WID id, HWND hParent, HBITMAP hBmp, PCWSTR title, PCWSTR content, DWORD style, HWND listener);



//�ر���Ϣ��
VOID SMP_Close_MsgBox(HWND hMsgBox);

/**
���ڳ���

 * \MsgBox���ڹ���
 *
 * \����hDlg	���ھ��
 * \����Msg		������Ϣ
 * \����wParam	��һ������
 * \����lParam	�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_MsgBox_WndProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	/** @} */

/** @} end of smp_msgbox */

#endif /* _SMP_MSGBOX_H */

