  #ifndef _PLT_TEXT_H
#define _PLT_TEXT_H

#include "window.h"

/*
  �ı�����

  ��Ҫʹ�������Ӵ�ƽ̨��
   - �����ı�����
   - ����������Ϣ
   - ��ӵ�����

  \code
  //��������
  hWnd = SGL_CreateWindow(PLT_Text_WndProc, ..., MR_DIALOG_OK_CANCEL, ...);
  PLT_Text_SetInfo(hWnd, SGL_LoadString(STR_TITLE), SGL_LoadString(STR_CONTENT));

  _LISTENER(hWnd) = hYouWnd; //���ô��ڣ��õ���Ϣ������
  SGL_AddChildWindow(HWND_DESKTOP, hWnd);

  //��Ӧ��WM_COMMAND
  case WM_COMMAND:
  	WID id = LOWORD(wParam);
  	WORD code = HIWORD(wParam);
  	if(id == "the platform text id")
  	{
  		if(code == MR_DIALOG_KEY_OK)
  		{
  			//your code
  			......
  		}else if(code == MR_DIALOG_KEY_CANCEL){
			//your code
			......
  		}
  	}
  \endcode






 ���ڳ�Ա����




 * \�����ı����ڵ���Ϣ
 *
 * \����hWnd	���ھ��
 * \����title	�����ַ���
 * \����text	����
 */
VOID PLT_Text_SetInfo(HWND hWnd, PCWSTR title, PCWSTR text);

/*
 * \���ֿ򣬸�ƽ̨�Ĵ��ڹ��̡�
 *
 * ��ƽ̨�����ֿ�������Щ������ʽ֮һ��
 * - MR_DIALOG_OK
 * - MR_DIALOG_OK_CANCEL
 * - MR_DIALOG_CANCEL
 *
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT PLT_Text_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of plttext */
	
#endif
