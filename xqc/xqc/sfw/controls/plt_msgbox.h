#ifndef _PLT_MSGBOX_H
#define _PLT_MSGBOX_H

#include "window.h"

/*
  ƽ̨MsgBox����
  ��Ҫʹ��ƽ̨����Ϣ��
   - ����PLT_MsgBox����ʾ��Ϣ��
   - ��ӦWM_COMMAND��Ϣ

   \code
   //��ʾһ��ƽ̨��Ϣ��
  PLT_MsgBox(0, hParent, SGL_LoadString(STR_MESSENGER), SGL_LoadString(STR_EXITCONFIRM), MR_DIALOG_OK_CANCEL, NULL);
   //��ӦWM_COMMAND
  case WM_COMMAND:
  	WID id = LOWORD(wParam);
  	WORD code = HIWORD(wParam);
  	if(id == "ƽ̨��Ϣ���ID")
  	{
  		if(code == MR_DIALOG_KEY_OK)
  		{
  			//��Ĵ���
  			......
  		}else if(code == MR_DIALOG_KEY_CANCEL){
			//��Ĵ���
			......
  		}
  	}
	\endcode

 * \��ʾһ����Ϣ��
 * \����id					����ID
 * \����hParent				���㴰�ھ��
 * \����title				�����ַ���
 * \����content				�����ַ���
 * \����style
 * - MR_DIALOG_OK			//ֻ��һ��ȷ����ť
 * - MR_DIALOG_OK_CANCEL 	//��ȷ����ȡ����ť
 * - MR_DIALOG_CANCEL		//ֻ��һ��ȡ����ť
 * \����listener			����֪ͨ��Ϣ������Ϊ��ʱ�����ڽ��õ���Ϣ
 * \����MsgBox�ľ��
 **/
HWND PLT_MsgBox(WID id, HWND hParent, PCWSTR title, PCWSTR content, DWORD style, HWND listener);

/*
���ڳ���
 * \��Ϣ�򴰿ڣ���ƽ̨�ĳ���
 * \����hDlg		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT PLT_MsgBox_WndProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of pltmsgbox */

#endif

