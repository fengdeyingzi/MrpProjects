#ifndef _PLT_MENU_H
#define _PLT_MENU_H

#include "window.h"

/**
   \ƽ̨�˵�����

  ��Ҫʹ�ø�ƽ̨�˵���
   - ����ƽ̨�˵�����
   - ���ò˵�����Ϣ
   - ���ò˵�����Ϣ
   - ��ӵ�����
   - ��ӦMR_MENU_SELECT

  \����

  //����һ��ƽ̨�˵�����ʾ��
  hMenu = SGL_CreateWindow��PLT_Menu_WndProc��
  0��0��SCREEN_WIDTH��SCREEN_HEIGHT��
  ���֤��0��0��;
  PLT_Menu_SetInfo��hMenu�����⣬��Ŀ��;
  PLT_Menu_SetMenuItem��hMenu��0��...);
  PLT_Menu_SetMenuItem��hMenu��1��...);
  PLT_Menu_SetMenuItem��hMenu��2��...);
  ...
  _LISTENER��hMenu��=hWnd��//��Ҫ�������ü�����
  SGL_AddChildWindow��HWND_DESKTOP��hMenu��;

  //��Ӧ��WM_COMMAND
  ������WM_COMMAND��
  ��Ů���뷢չ��ID = LOWORD��Ϊ��wParam������;
  Word����= HIWORD��wParam������;
  ��������==��ƽ̨�˵�ID����
  {
  ���������== MR_MENU_SELECT��
  {
  ڹ��ָ��=��int����lParam��; / /ѡ��˵��������
  / /��Ĵ���
  ......
  }�������������== MR_MENU_RETURN��{
/ /��Ĵ���
......
  }
  }
  
  \ endcode

  */


/*
 * \���ò˵�����Ϣ��
 *
 * �˺���Ӧ�ò˵����ں���ô���
 * ���ò˵�����Ϣ��
 *
 * \����hMenu	ƽ̨�˵�����
 * \����title	�����ַ���
 * \����title	�˵����ַ���
 */


VOID PLT_Menu_SetInfo(HWND hMenu, PCWSTR title, Sint32 items);

/**
 * \����һ���˵���
 *
 * \����hMenu	ƽ̨�˵����
 * \����index	�˵��������
 * \����title	�˵����ַ���
 */
VOID PLT_Menu_SetMenuItem(HWND hMenu, int index, PCWSTR title);

/**
 * \ƽ̨�˵��Ĵ��ڹ��̡�
 *
 * \����hMenu		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT PLT_Menu_WndProc(HWND hMenu, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	/** @} */

/** @} end of pltmenu */

#endif
