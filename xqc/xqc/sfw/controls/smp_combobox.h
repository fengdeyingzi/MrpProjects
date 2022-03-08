#ifndef _SMP_COMBOBOX_H
#define _SMP_COMBOBOX_H

#include "window.h"

/**
  ����Ͽ�

  ��Ҫʹ����Ͽ�
   - ������Ͽ򴰿�
   - ������Ͽ���Ŀ��ѡ����Ŀ
   - ��ӵ�������
   - ��Ӧ��Ϣ��֪ͨ

  \code
  	//������Ͽ�
  	hCombo = SGL_CreateWindow(SMP_ComboBox_WndProc, ...);
  	SMP_ComboBox_SetItems(hCombo, ...);
  	SMP_ComboBox_SetSelectedItem(hCombo, ...);
  	SGL_AddChildWindow(hWnd, hCombo);

  	//��Ӧ��Ϣ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the combo id" && code == SMP_COMBON_VALUECHANGED)
		{
			int index = (int)lParam; //��ǰѡ��������
			...
		}	
  \endcode


������Ϣ
 * ����Ͽ��ֵ����֪ͨ��Ϣ��
 *
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the combo id" && code == SMP_COMBON_VALUECHANGED)
 *		{
 *			int index = (int)lParam; //��ǰѡ��������
 *			...
 *		}
 * \endcode
 *
 * ����index	��ǰѡ��������
 */

#define SMP_COMBON_VALUECHANGED		0x0001


/** 
 * \������Ͽ��б�Ĵ�С
 *
 * \����hWnd	��Ͽ�Ĵ��ھ��
 * \����size	�б�Ĵ�С
 * \�ɹ�����TRUE�����򷵻�FALSE
 */
BOOL SMP_ComboBox_SetSize(HWND hWnd, Sint32 size);

/**
 * \��ȡ��Ͽ��б�Ĵ�С.
 * \����hWnd	��Ͽ�Ĵ��ھ��
 * \������Ͽ��б�Ĵ�С
 */
Sint32 SMP_ComboBox_GetSize(HWND hWnd);

/**
 * \������Ͽ��б������Ϣ
 * \����hWnd		��Ͽ�Ĵ��ھ��
 * \����index		�б�����
 * \����str			��ʾ���ַ���
 * \����userdata	�û��ض�������
 */
VOID SMP_ComboBox_SetItem(HWND hWnd, Sint32 index, PCWSTR str, DWORD userdata);

/**
 * \��ȡ��Ͽ��б������Ϣ
 * \����hWnd			��Ͽ�Ĵ��ھ��
 * \����index			������
 * \����[��]str			��ʾ���ַ���
 * \����[��]userdata	�û��ض�������
 */
VOID SMP_ComboBox_GetItem(HWND hWnd, Sint32 index, PCWSTR * str, DWORD* userdata);

/**
 ��������Ͽ��б��
 ����һ��SMP_ComboBox_SetSize / SMP_ComboBox_SetItem wraper����
 ���ԣ����SMP_ComboBox_SetItemsʹ���벻Ҫ��SMP_ComboBox_SetSize / SMP_ComboBox_SetItem��
 ������hWnd���ھ������Ͽ�
 ���������Ͽ��б��������
 ��������Ĵ�С�ߴ����Ʒ
 �ɹ�����true�����򷵻�FALSE
 * \��Ӧ SMP_ComboBox_SetSize, SMP_ComboBox_SetItem
 */
BOOL SMP_ComboBox_SetItems(HWND hWnd, const DWORD* items, Sint32 size);

/**
 * \ѡ�����������
 * \����hWnd		��Ͽ�Ĵ��ھ��
 * \����index		�б��������
 * \����redraw		�Ƿ��ػ���Ͽ�
 * \����notify		�Ƿ���֪ͨ��Ϣ
 */
VOID SMP_ComboBox_SetSelectedItem(HWND hWnd, Sint32 index, BOOL redraw, BOOL notify);

/**
 * \��ȡѡ���������
 * \����hWnd		��Ͽ�Ĵ��ھ��
 * \������ѡ��Ŀ������
 */
Sint32 SMP_ComboBox_GetSelectedItem(HWND hWnd);

/** 
���ڳ���
 * \��Ͽ����
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_ComboBox_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */
	
/** @} end of smp_progbar */

#endif
