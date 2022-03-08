#ifndef _SMP_DATEPICKER_H
#define _SMP_DATEPICKER_H

#include "window.h"

/**
defgroup smp_datepickerDatePicker
   ��Ҫʹ������ѡ������
 - ��������ѡ��������
 - ���õ�ǰ����
 - ��ӵ�������
 - ��Ӧ��Ϣ��֪ͨ
 
  \code
	//����һ������ѡ����
	hControl = SGL_CreateWindow(SMP_DatePicker_WndProc, ...);
	SMP_DatePicker_Set2CurrentDate(hControl, FALSE, FALSE);
	SGL_AddChildWindow(hWnd, hControl);

	//��Ӧ��Ϣ��֪ͨ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the datepicker id")
		{
			switch(code)
			{
			case SMP_DATEPICKERN_VALUECHANGED:
				HWND hControl = (HWND)lParam; // DatePicker�Ĵ��ھ��
				//����֪ͨ�¼���
				break;
			}
		}	
  \endcode
*/

/*
����NOTIFY��Ϣ 
 *\DatePicker�Ĵ���ʱ���͵�ֵ�ı䡣
 *
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the calendar id" && code == SMP_DATEPICKERN_VALUECHANGED)
 *		{
			HWND hControl = (HWND)lParam; //DatePicker�Ĵ��ھ��
 * 			//�����֪ͨ��Ϣ
 *		}
 * \endcode
 *
 * ����hControl�������ʹ���Ϣ����
*/
#define SMP_DATEPICKERN_VALUECHANGED		0x0001

/** 
���ڳ�Ա����
 * \brief Set the new date �趨�µ�����
 *
 * \param hWnd the date picker window handle����ѡ����������hWnd���ھ��
 * \param year the year of the date����һ��һ�������
 * \param month the month of the date�����·ݵ����ڵ��·�
 * \param day the day of the date����һ��һ�������
 * \param notify if send notify message�����������֪ͨ��Ϣ֪ͨ
 * \param update if redraw the window�������£�����ػ洰��
 */
VOID SMP_DatePicker_SetDate(HWND hWnd, int year, int month, int day, BOOL notify, BOOL update);

/**
 * \brief Get the date from the control��ȡ���ڴӿ���
 *
 * \param hWnd the Date picker window handle������hWnd���ھ��������ѡ����
 * \param[out] year the year of the date����[���]��ȫ�������
 * \param[out] month the month of the date����[��]�·ݵ����ڵ��·�
 * \param[out] day the day of the date����[��]һ��һ�������
 */
VOID SMP_DatePicker_GetDate(HWND hWnd, int* year, int* month, int* day);

/**
 * \brief Set the Date picker to current date������ѡ��������Ϊ��ǰ����
 *
 * \param hWnd the date picker window handle������hWnd���ھ��������ѡ����
 * \param notify if send notify message�����������֪ͨ��Ϣ֪ͨ
 * \param update if redraw the window�������£�����ػ洰��
 */
VOID SMP_DatePicker_Set2CurrentDate(HWND hWnd, BOOL notify, BOOL update);

/** 
���ڳ���
 * \brief The simple Date picker window procedure.����ѡ�������ڹ���
 *
 * \param hWnd the window handle������hWnd���ھ��
 * \param Msg the window message\������Ϣ�Ĵ�����Ϣ
 * \param wParam the first parameter\����wParam�ĵ�һ������
 * \param lParam the second parameter\����lParam�ĵڶ�������
 * \return the result of message process \������Ϣ�Ĵ�����
 */
LRESULT SMP_DatePicker_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of smp_datepicker */


#endif
