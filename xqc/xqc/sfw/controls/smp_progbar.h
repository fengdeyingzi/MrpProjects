#ifndef _SMP_PROGBAR_H
#define _SMP_PROGBAR_H

#include "window.h"


/**
  \������

  ��Ҫʹ�ý�������

  - ����������
  - ���ý�������Χ
  - ��ӵ�������
  - ��Ӧ��Ϣ��֪ͨ

  \code
  	//����������
  	hPorgBar = SGL_CreateWindow(SMP_ProgBar_WndProc, ...);
  	SMP_ProgBar_SetRange(hPorgBar, 1, 30);  //Ĭ�Ϸ�Χ��[0 - 99]
  	SGL_AddChildWindow(hWnd, hPorgBar);

  	//��Ӧ��Ϣ��֪ͨ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the progbar id" && code == SMP_PROGBARN_VALUECHANGED)
		{
			int value = (int)lParam; //��ǰ��������ֵ
		}	
  \endcode
  
 
  @ingroup controls
  @{



  ����֪ͨ��Ϣ

  * \�������Ĵ���ֵ������Ϣ
  *
  * \code
  *	case WM_COMMAND:
  *		WID id = LOWORD(wParam);
  *		WORD code = HIWORD(wParam);
  *
  if(id == "the progbar id" && code == SMP_PROGBARN_VALUECHANGED)
  {
  int value = (int)lParam; //��ǰprogbar��ֵ
  }	
  * \endcode

  * \ͨ��ֵ
  */
#define SMP_PROGBARN_VALUECHANGED		0x0001

	/** @} */

/** 
���ڵĳ�Ա����

 * \���ý�������Χ���ô��ڵ�Ĭ�Ϸ�Χ��[0-99]
 *
 * \������hWnd		�������Ĵ��ھ��
 * \����min			����������Сֵ
 * \����max			�����������ֵ
 */
VOID SMP_ProgBar_SetRange(HWND hWnd, Sint32 min, Sint32 max);

/**
 * \���ý�������ֵ
 *
 * \����hWnd	����Ľ�����
 * \����value	��ֵ
 * \����redraw	�Ƿ��ػ����
 * \����notify	���ֵ�仯����֪ͨ��Ϣ
 */
VOID SMP_ProgBar_SetValue(HWND hWnd, Sint32 value, BOOL redraw, BOOL notify);

/** 
���ڳ���

 * \����������
 *
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_ProgBar_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	/** @} */
	
/** @} end of smp_progbar */

#endif
