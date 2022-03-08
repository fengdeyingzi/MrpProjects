#ifndef _SMP_LABEL_H
#define _SMP_LABEL_H

#include "window.h"


/**
  \��ǩ

  ��Ҫʹ�ñ�ǩ��
   - ������ǩ����
   - ���ñ�ǩ������
   - ��ӵ�������

  \code
  hControl = SGL_CreateWindow(SMP_Label_WndProc, ...);
  SMP_Label_SetContent(hControl, RESID_INVALID, SGL_LoadString(STR_HELLO), 0);
  SGL_AddChildWindow(hParent, hControl);
  \endcode


������ʽ

 ˵��������һ�����ӡ�
*/
#define SMP_LABELS_LINK		0x0001L

/**
 * ˵����ǩ��һ����̬�ı�ǩ��������Ӧ�¼�
 */
#define SMP_LABELS_STATIC		0x0002L

/**
˵����ǩ�����ݽ��Զ�����
 */
#define SMP_LABELS_AUTOSCROLL		0x0004L

/**
˵����ǩ�����������Ķ���
 */
#define SMP_LABELS_HCENTER		0x0008L


/*
������Ϣ
 * \�����ǩʱ����
 *
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the label id" && code == SMP_LABELN_CLICKED)
 *		{
 *			HWND hLabel = (HWND)lParam;
 * 			//handle the button click notify message
 *		}
 * \endcode
 *
 * \����hLabel	���ʹ���Ϣ�ı�ǩ���
 */
#define SMP_LABELN_CLICKED		0x0001

/*
  ���ڳ�Ա����
 * \���ñ�ǩ����
 *
 * \����hWnd		��ǩ�Ĵ��ھ��
 * \����bmpID		λͼ��ID(��ʾ�����)
 * \����content		���ַ�������
 * \����interval	�Զ������ٶ�
 *	- >0ʱ����ǩ����SMP_LABELS_AUTOSCROLL���
 *	- 0ʱ����ǩ������SMP_LABELS_AUTOSCROLL���
 */
VOID SMP_Label_SetContent(HWND hWnd, DWORD bmpID, PCWSTR content, Uint32 interval);

/**
* \������ֹͣ������ǩ
*
* \ע��:��ǩ������SMP_LABELS_AUTOSCROLL���
*
* \����hWnd		��ǩ�Ĵ��ھ��
* \��Ӧ SMP_Label_StopAutoScroll
*/
VOID SMP_Label_StartAutoScroll(HWND hWnd, BOOL reset);

/**
 * \ֹͣ����
 *
 * \ע���ǩ������SMP_LABELS_AUTOSCROLL���
 * ��ǩ����palyingֹͣ���Զ�palyingʱ�ٴ���ʾ��
 * ���ԣ�����ֹͣһ����ǩ��SMP_Label_StartAutoScroll����Ҫ�ٵ���ǩ��ʾ��
 * \����hWnd	��ǩ�Ĵ��ھ��
 * \��Ӧ SMP_Label_StartAutoScroll
 */
VOID SMP_Label_StopAutoScroll(HWND hWnd);

/** 
���ڳ���
 * \��ǩ�Ĵ��ڹ���	
 *
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_Label_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	/** @} */

/** @} end of smp_label */

#endif
