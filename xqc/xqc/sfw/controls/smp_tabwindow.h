#ifndef _SMP_TABWINDOW_H
#define _SMP_TABWINDOW_H

#include "window.h"
#include "smp.h"


/**
  \defgroup smp_tabwindow Simple tab window

  ��Ҫʹ�ü򵥵�tab���ڣ�
 - ����ѡ�����
 - ������ǩ����ӵ�ѡ�����
 - ��ӱ�ǩ���ڵĸ�����
 - ��Ӧ��Ϣ��֪ͨ
  

  \code
	//create a button����һ����ť
	HWND hTabWnd = SGL_CreateWindow(SMP_TabWindow_WndProc, ...);
	SGL_AddChildWindow(hWnd, hTabWnd);

	hControl = SGL_CreateWindow(TAB1_WndProc, ...);
	SMP_TabWindow_AddTab(hTabWnd, BMP_BUSY, SGL_LoadString(STR_OK), hControl);		

	//response to tab window notify messages��Ӧ��Ϣѡ�����֪ͨ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the tab window id")
		{
			Sint32 index = (Sint32)lParam; // the tab index/��ǩָ��

			switch(code)
			{
			case SMP_TABWNDN_TABSWITCHED:
				//handle the notify event./����֪ͨ�¼�
				break;
			}
		}	
  \endcode

  @ingroup controls
  @{
*/
	/**
	* \name Window Styles
	* @{
	*/

/**
 * \brief Indicates that right arrow pressed��ʾ�����Ҽ�ͷ
 */
#define SMP_TABWNDS_RIGHTARROW			0x0001L

/**
 * \brief Indicates that left arrow pressed��ʾ�������
 */
#define SMP_TABWNDS_LEFTARROW			0x0002L

/** 

����NOTIFY��Ϣ

 * \brief Sent when tab window switched \��̵ķ���ѡ������л�ʱ
 *
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 * 
 *		if(id == "the tab window id")
 *		{
 *			Sint32 index = (Sint32)lParam; // the tab index/��ǩָ��
 *			switch(code)
 *			{
 *			case SMP_TABWNDN_TABSWITCHED:
 *				//handle the notify event./����֪ͨ�¼�
 *				break;
 *			}
 *		}	
 * \endcode
 *
 * \param index the high light tab index ����ָ��ĸ߹�ѡ�����
 */
#define SMP_TABWNDN_TABSWITCHED		0x0001

	/** @} */

/**
 * \brief The tab window bar height��̵ı�ǩ�������ĸ߶�
 */
#define SMP_TABWND_TABBAR_HEIGHT		SMP_ITEM_HEIGHT

/**
 * \brief The max tab the tab window support��̵�����ǩ�ϵı�ǩ����֧��
 */
#define SMP_TABWND_TABCOUNT			12

/**
���ڳ�Ա����

 * \brief Add a tab window������ѡ�����
 *
 * \param hWnd the tab window handle������hWnd���ھ���ı�ǩ
 * \param bmp the icon idͼ���ID
 * \param title the child tab window title\���������Ӵ��ڵı���
 * \param hTab the  child window handle\����hTab�Ӵ��ھ��
 */
VOID SMP_TabWindow_AddTab(HWND hWnd, DWORD bmp, PCWSTR title, HWND hTab);

/**
 * \brief Remove a tab\���ɾ����ǩ
 *
 * \param hWnd the tab window handle\������hWnd���ھ���ı�ǩ
 * \param id the child tab window id\������ʶ��ǩ���Ӵ���ID
 */
VOID SMP_TabWindow_RemoveTab(HWND hWnd, WID id);

/**
 * \brief Remove all the tabsɾ�����еı�ǩ
 *
 * \param hWnd the tab window handle������hWnd���ھ���ı�ǩ
 */
VOID SMP_TabWindow_RemoveAllTabs(HWND hWnd);

/**
 * \brief Get the tab child window by index\������Ӵ��ڵ�ѡ�����
 *
 * \param hWnd the tab window handle \������hWnd���ھ���ı�ǩ
 * \param index the child index\����ָ��ָ���ĺ���
 * \return the child tab window handle\�����ӱ�ǩ���ھ��
 */
HWND SMP_TabWindow_GetTabByIndex(HWND hWnd, Sint32 index);

/**
 * \brief Get the tab child window by id\������Ӵ���ID�ı�ǩ
 *
 * \param hWnd the tab window handle\������hWnd���ھ���ı�ǩ
 * \param id the child tab window id\������ʶ��ǩ���Ӵ���ID
 * \return the child tab window handle \�����ӱ�ǩ���ھ��
 */
HWND SMP_TabWindow_GetTabByID(HWND hWnd, WID id);

/**
 * \brief Get the active tab index\��̻�ȡ��ǰ��ǩָ��
 *
 * \param hWnd the tab window handle\������hWnd���ھ���ı�ǩ
 * \return the active tab index \���ص�ǰ��ǩָ��
 */
Sint32 SMP_TabWindow_GetActiveTabIndex(HWND hWnd);

/**
 * \brief Set the tab with id as the active tab.\��̵�������Ϊ���ǩ��ID��ǩ
 *
 * \param hWnd the tab window handle\������hWnd���ھ���ı�ǩ
 * \param id the child window handle\������ʶ�Ӵ��ھ��
 */
VOID SMP_TabWindow_SetActiveTabByID(HWND hWnd, WID id);

/**
 * \brief Set the tab of index as the active tab\�������ָ����Ϊ��ǰ��ǩ��ǩ
 *
 * \param hWnd the tab window handle\������hWnd���ھ���ı�ǩ
 * \param index the child window index\����ָ��ָ�����Ӵ���
 */
VOID SMP_TabWindow_SetActiveTabByIndex(HWND hWnd, Sint32 index);

/**

���ڳ���

 * \brief The simple tab window procedure.	\��̵ļ򵥱�ǩ�Ĵ��ڹ���
 *
 * \param hWnd the tab window handle\������hWnd���ھ���ı�
 * \param Msg the window message\������Ϣ�Ĵ�����Ϣ
 * \param wParam the first parameter\����wParam�ĵ�һ������
 * \param lParam the second parameter * \����lParam�ĵڶ�������
 * \return the result of message process  * \������Ϣ�Ĵ�����
 */
LRESULT SMP_TabWindow_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	/** @} */

/** @} end of smp_tabwindow */


#endif
