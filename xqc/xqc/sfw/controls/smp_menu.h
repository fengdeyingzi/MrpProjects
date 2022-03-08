#ifndef _SMP_MENU_H
#define _SMP_MENU_H

#include "window.h"

/**
 * ������˵��ؼ�

  ��Ҫʹ�ò˵���
  - ���ò˵���
  - �˵�����API����ʾ�˵�
  - ��Ӧ֪ͨ��Ϣ�Ĳ˵�

���������ò˵���Ĳ�ͬ��ʽ������ʾһ�������˵���

  
 ʾ������ 1:
  \code
  	SMP_Menu_ClearMenuItems();
  	SMP_Menu_SetMenuItem(0, ..., 1);
  	SMP_Menu_SetMenuItem(1, ..., 2);
  	SMP_Menu_SetMenuItem(2, ..., 3);
  	...
  	SMP_Menu_Popup(...);
  \endcode

 ʾ������ 2:
  \code
  	//ȫ�ֲ˵�����Ϣ
	static const DWORD miOptions [] =
	{
		STR_CONNECT,
		STR_EXIT,
		STR_REGISTER,
		...
	};
	//��Ĵ���

  	SMP_Menu_ClearMenuItems();
	SMP_Menu_SetMenuItem2(0, miOptions, sizeof(miOptions)/sizeof(miOptions[0]));
	...
  	SMP_Menu_Popup(...);
  \endcode

 ʾ������3:
  \code
 /����һ�����Ӳ˵��Ĳ˵���:

  	//ȫ�ֲ˵�����Ϣ
	static const DWORD miOptions [] =
	{
		STR_CONNECT,
		STR_EXIT,
		STR_REGISTER,
		...
	};

	SMP_Menu_Popup2(..., miOptions, sizeof(miOptions)/sizeof(miOptions[0]));
  \endcode

�˵���Ӧ֪ͨ�¼�
  \code
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam); // �˵����ID

		if(id == "the menu id")
		{
			DWORD userdata = (DWORD)lParam; //if setted with the menu item/���setted�Ĳ˵���

			switch(code)
			{
			case STR_CONNECT:
				//handle the notify event.
				break;
			case STR_EXIT:
				break;
			case STR_REGISTER:
				break;
			...
			}
		}	
  \endcode




 * \�˵��е����˵���������������Ҫ������������
 */
#define SMP_MENU_MAX_MENUITEM	40
/*
������ʽ

��ʾ�ڵ����ĺ�����x��y����λ���ǲ˵�����ť��
*/
#define SMP_MENUS_BOTTOMLEFT			0x0001L	

#if 0 //not implemented yet
/**
 \��ʾ�ڵ����ĺ�����x��y����λ���ǲ˵����Ұ�ť��
 */
#define SMP_MENUS_BOTTOMRIGHT		0x0002L
#endif

/**
 * \brief Indicates the (x, y) position in popup function is the menu (left, top)
		 \��ʾ�ڵ����ĺ�����x��y����λ���ǲ˵������ϣ�
 */
#define SMP_MENUS_TOPLEFT				0x0004L

#if 0 //not implemented yet
/**
 * \brief Indicates the (x, y) position in popup function is the menu (right, top)
			 \��ʾ�ڵ����ĺ�����x��y����λ���ǲ˵����ң��ϣ�
 */
#define SMP_MENUS_TOPRIGHT			0x0008L
#endif

/**
 * \��ʾ��һ���̶��Ŀ�ȵ�λ�˵�
 *
 *����SMP_Menu_PopupFlat������ʾƽ����Ĳ˵����ַ��
 *�˵�ֻ��һ��ƽ̹��ˮƽ�����Բ�Ҫʹ�ô�Ϊ�����˵���ʽ��
 *����ʹ������Ͽ��һЩ�����ؼ���
 */
#define SMP_MENUS_FLAT				0x0010L

	/** @} */

/**
 * \����˵���ID
 */
#define SMP_MENU_SEPERATOR		0xffff

	/**
	 * \name Window Member Functions
	 * @{
	 */

/**
 * \����˵���
 */
VOID SMP_Menu_ClearMenuItems(VOID);

/**
 * \���ò˵���
 * 
 * \����index	�˵��������
 * \����id		�˵����ID
 * \����str		�˵�����ʾ���ַ���
 * \����data	�˵�������
 * \����next	��һ���˵��-1��ʾû����һ���˵���
 */
VOID SMP_Menu_SetMenuItem(int index, WID id, PCWSTR str, DWORD data, int next);

/**
 * \���ò˵����ַ�����Դ
 *
 * \����start	��ʼ��
 * \����items	�ַ�����Դ����
 * \����size	�ַ�����Դ����Ĵ�С
 */
VOID SMP_Menu_SetMenuItem2(int start, const DWORD* items, int size);

/**
 * \����һ���Ӳ˵�
 *
 * \����index	�Ӳ˵�λ��
 * \����sub		���Ӳ˵��Ĳ˵�����
 */
VOID SMP_Menu_SetSubMenu(int index, int sub);

/**
 * \ѡ�в˵���
 * 
 * \����index	�˵��������
 * \����check	TRUEѡ�У�FALSE��ѡ��
 */
VOID SMP_Menu_CheckMenuItem(int index, BOOL check);
/**
 * \���ò˵���
 *
 * \����index	�˵��������
 * \����disable	TRUE���ã�FALSEʹ��
 */
VOID SMP_Menu_DisableMenuItem(int index, BOOL disable); 

/**
 * \����ȫ�ֲ˵�
 *
 * \����id			�˵��Ĵ���ID
 * \����style		�˵��ķ��
 * \����hParent		���ھ��
 * \����x			��ߵ�λ��
 * \����Y			���ߵ�λ��
 * \����listener	����WM_COMMAND��Ϣ�ļ�������
 * \����ȫ�ֲ˵����
 */
HMENU SMP_Menu_Popup(WID id, DWORD style, HWND hParent, int x, int y, HWND listener);

/**
 * \�����˵���ȫ�ֲ˵���
 *
 * \����id			�˵����ڵ�ID
 * \����style		�˵��ķ��
 * \����hParent		���ھ��
 * \����x			���λ��
 * \����Y			����λ��
 * \����items		�˵����ַ�������
 * \����size		�˵����ַ�������Ĵ�С
 * \����listener	����WM_COMMAND��Ϣ�ļ�������
 * \����ȫ�ֲ˵����
 */
HMENU SMP_Menu_Popup2(WID id, DWORD style, HWND hParent, int x, int y, const DWORD* items, int size, HWND listener);

/**
 * \����ȫ�̶ֹ���ȷ��Ĳ˵�
 *
 * \����id			�˵����ڵ�ID
 * \����style		�˵��ķ��
 * \����hParent		���ھ��
 * \����x			���λ��
 * \����Y			����λ��
 * \����w			�˵��Ŀ��
 * \����listener	����WM_COMMAND��Ϣ�ļ�������
 * \����ȫ�ֲ˵����
 */
HMENU SMP_Menu_PopupFlat(WID id, DWORD style, HWND hParent, int x, int y, int w, HWND listener);


/*
���ڳ���
 * \�˵�����
 *
 * \����hMenu	���ھ��
 * \����Msg		������Ϣ
 * \����wParam	��һ������
 * \����lParam	�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_Menu_WndProc(HMENU hMenu, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of smp_menu */

/** @} end of controls */

#endif /* _SMP_MENU_H */

