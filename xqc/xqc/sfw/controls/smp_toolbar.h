#ifndef _SMP_TOOLBAR_H
#define _SMP_TOOLBAR_H

#include "window.h"

/**
  \������

  ��������֪ͨ��Ϣ��ת�Ƶ���Ҫ�¼���
   - �� - �����ע��
   - �� - �����(ѡ��)
   - �� - �����ע��

  ��Ҫʹ�ù�������
   - ����������
   - ���ù��������ַ���
   - ���������/ѡ��/��

  \code
  	//����������

  	hToolbar = SGL_CreateWindow(SMP_Toolbar_WndProc, ...);
  	SMP_Toolbar_SetStrings(hToolbar, ...);
  	SGL_AddChildWindow(hWnd, hToolbar);

  	//��Ӧ
  	case WM_KEYUP:
  		if(MR_KEY_SOFTLEFT == wParam)
  		{
  			//��
  		}else if(MR_KEY_SELECT == wParam){
  			//��
  		}else if(MR_KEY_SOFTRIGHT == wParam){
  			//��
  		}
  \endcode

*/

/**
 * \\���֧�ֹ�������
 */
#define SMP_TOOLBAR_ITEMCOUNT		3

/**
 * \��������ߵ�λ��
 */
#define SMP_TOOLBARLOC_LEFT		0

/**
 * \�������м��λ��
 */
#define SMP_TOOLBARLOC_MID		1

/**
 * \�������ұߵ�λ��
 */
#define SMP_TOOLBARLOC_RIGHT		2

	/**
		���ڳ�Ա����
	 * @{
	 */

/**

 * \���ù��������ַ���
 *
 * \����hWnd		���������
 * \����left		��ߵ��ַ���ID
 * \����mid			�м���ַ���ID
 * \����right		�ұߵ��ַ���ID
 * \����redraw		�ػ湤����
 */
VOID SMP_Toolbar_SetStrings(HWND hWnd, DWORD left, DWORD mid, DWORD right, BOOL redraw);

/**
 * \��ȡ��������Ŀ���ַ���ID
 * 
 * \����hWnd			�������Ĵ��ھ��
 * \����location		��Ŀλ��
 * \������ĿID
 */
DWORD SMP_Toolbar_GetString(HWND hWnd, int location);

/** 
���ڳ���
 * \�������Ĵ��ڹ���
 *
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_Toolbar_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of smp_toolbar */

#endif /* _SMP_TOOLBAR_H */

