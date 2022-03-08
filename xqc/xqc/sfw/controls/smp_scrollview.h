#ifndef _SMP_SCROLLVIEW_H
#define _SMP_SCROLLVIEW_H

#include "window.h"

/**
  \����

  ��Ҫ�ü򵥵Ĺ�����
 - ��������
 - ��ȡ�Ĺ���������ͼ
 - ������Ĵ��ڵ����ݲ鿴
 - �������Ĵ��ڣ����Ķ���C���߾�����������
  

  \code
	//����һ������

	hScrollView = SGL_CreateWindow(SMP_ScrollView_WndProc, ...);
	hContent = SMP_ScrollView_GetContentView(hScrollView);

	hControl = SGL_CreateWindow("your window", ...);
	_LISTENER(hControl) = hWnd; //Ӧ��������ļ�������֪ͨ��Ϣ�����͵���hWnd

	SGL_AddChildWindow(hContent, hControl);

	......
	SGL_AddChildWindow(hWnd, hScrollView);
  \endcode
   @ingroup controls
  @{
*/

	/**
	 * \name Window Member Functions
	 * @{
	 */
/*
���ڳ�Ա����

* \brief Get the scrollview content view��ȡ�������ݵĿ���
*
* For we must add your window to a scrollview's content view not the scrolview itself.
��Ϊ���Ǳ������һ���������ڵ����ݵĿ�������scrolview����
*
* \param hWnd the scrollview window handle \����hWnd   ���ھ��
* \return the content view handle\���ص�������ͼ����
*/
HWND SMP_ScrollView_GetContentView(HWND hWnd);


/**
���ڳ���

 * \�������ڹ���
 *
 * \����hWnd		���ھ��
 * \����			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */

LRESULT SMP_ScrollView_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of smp_scrollview */

#endif
