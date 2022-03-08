#ifndef _SMP_CALENDAR_H
#define _SMP_CALENDAR_H

#include "window.h"

/**
  defgroup smp_calendar�򵥵�����

  ��Ҫʹ�ü򵥵�������
 - ������������
 - ���õ�ǰ����
 - ��ӵ�������
 - ��Ӧ��Ϣ������֪ͨ

  \code
	//����һ����������
	hControl = SGL_CreateWindow(SMP_Calendar_WndProc, ...);
	SMP_Calendar_SetDate(hControl,  ...);
	SGL_AddChildWindow(hWnd, hControl);

	//��Ӧ��Ϣ֪ͨ������
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the calendar id")
		{
			hControl = (HWND)lParam;
			switch(code)
			{
			case SMP_CALENDARN_SELECTED:
				//����֪ͨ�¼���
				break;
			}
		}	
  \endcode
*/ 

/**
 *����NOTIFY��Ϣ
 *��̵ķ��ͣ����û�ѡ��һ������
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the calendar id" && code == SMP_CALENDARN_SELECTED)
 *		{
 *			HWND hControl = (HWND)lParam; //��������
 * 			//����֪ͨ��Ϣ������
 *		}
 * \endcode




 *����hControl�������ھ��
 */
#define SMP_CALENDARN_SELECTED	0x0001

/**
���ڳ�Ա����
 * \brief Set the calendar date���������������
 * \param hWnd the calendar window handle������hWnd���ھ��������
 * \param year the new value of year������һ�����µļ�ֵ
 * \param month the new value of month�����·�һ���µ���ֵ
 * \param day the new value of day����һ��һ�����ֵ
 */
VOID SMP_Calendar_SetDate(HWND hWnd, int year, int month, int day);

/**
 * \brief Get the current date��̻�ȡ��ǰ����
 * \param hWnd the calendar window handle������hWnd���ھ��������
 * \param[out] year return the value of the year����[���]�귵�����ֵ
 * \param[out] month return the value of the month����[���]�·����·�ֵ
 * \param[out] day return the value of the day����[���]�շ���ֵ��һ��
 */
VOID SMP_Calendar_GetDate(HWND hWnd, int* year, int* month, int* day);

/*
���ڳ���
 * \brief The simple calendar window procedure. \�򱨼򵥵��������ڹ���
 *
 * \param hWnd the window handle\������hWnd���ھ��
 * \param Msg the window message\������Ϣ�Ĵ�����Ϣ
 * \param wParam the first parameter\����wParam�ĵ�һ������
 * \param lParam the second parameter\����lParam�ĵڶ�������
 * \return the result of message process ������Ϣ�Ĵ�����
 */
LRESULT SMP_Calendar_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of smp_calendar */

#endif
