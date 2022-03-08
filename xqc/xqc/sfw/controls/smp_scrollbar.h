#ifndef _SMP_SCROLLBAR_H
#define _SMP_SCROLLBAR_H

#include "window.h"


/*������

��Ҫ�ü򵥵Ĺ�������
   - ������������
   - ���ù������ڵ���Ϣ
   - ��ӵ�������
   - ��Ӧ��Ϣ��֪ͨ

  \code
  	//����������
  	hScrbar = SGL_CreateWindow(SMP_Scrollbar_WndProc, ...);
  	SMP_Scrollbar_SetInfo(hScrbar);
  	SGL_AddChildWindow(hWnd, hScrbar);

  	//��Ӧ��Ϣ��֪ͨ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);
		if(id == "the scrollbar id" && code == SMP_SCRBARN_VALUECHANGED)
		{
			int index = (int)lParam; //��ǰ������ֵ
		}	
  \endcode

  @ingroup controls
  @{
*/

/*
������Ϣ

* \��������ֵ����֪ͨ��Ϣ
*
* \code
*	case WM_COMMAND:
*		WID id = LOWORD(wParam);
*		WORD code = HIWORD(wParam);
*
if(id == "the scrollbar id" && code == SMP_SCRBARN_VALUECHANGED)
{
	int index = (int)lParam; //��ǰ������ֵ
}	
* \endcode
*
* \����index   �������ĵ�ǰֵ
*/
#define SMP_SCRBARN_VALUECHANGED		0x0001

/** 
���ڵĳ�Ա����

 * \���ù���������Ϣ
 *
 * \����hWnd        ���ھ��
 * \����min         ���ֵ
 * \����max         ���ֵ
 * \����pagesize    ��Ӧ����¼�ʱÿҳ��С
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_Scrollbar_SetInfo(HWND hWnd, int min, int max, int pagesize);

/**
 * \��ù�������Ϣ
 *
 * \����hWnd       ���ھ��
 * \����min        ���ֵ
 * \����max        ���ֵ
 * \����pagesize   ÿҳ��С
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_Scrollbar_GetInfo(HWND hWnd, int* min, int* max, int* pagesize);
/**
 * \���ù������Ĳ���
 *
 * \����hWnd    ���ھ��
 * \����step    ����ֵ
 */
VOID SMP_Scrollbar_SetStep(HWND hWnd, int step);
/**
 * \���ù���������λ��
 *
 * \����hWnd        ���ھ��
 * \����value       ��ֵ
 * \����redraw      �Ƿ��ػ洰��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_Scrollbar_SetValue(HWND hWnd, int value, BOOL redraw);
/**
 * \�õ��������ĵ�ǰλ��
 *
 * \����hWnd        ���ھ��
 * \���ص�ǰλ��ֵ
 */
int SMP_Scrollbar_GetValue(HWND hWnd);
/**
 * \���Ϲ���
 *
 * \����hWnd        ���ھ��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_Scrollbar_PageUp(HWND hWnd);

/**
 * \���¹���
 *
 * \����hWnd        ���ھ��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_Scrollbar_PageDown(HWND hWnd);
/**
 * \���������Ϲ���
 *
 * \����hWnd     ���ھ��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_Scrollbar_LineUp(HWND hWnd);

/**
 * \���������¹���
 *
 * \����hWnd     ���ھ��
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_Scrollbar_LineDown(HWND hWnd);


/** 
���ڳ���

 * \�������Ĵ��ڹ���
 *
 * \����hWnd        ���ھ��
 * \����Msg         ������Ϣ
 * \����wParam      ��һ������
 * \����lParam      �ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_Scrollbar_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

		/** @} */

/** @} end of smp_scrollbar */

#endif /* _SMP_SCROLLBAR_H */

