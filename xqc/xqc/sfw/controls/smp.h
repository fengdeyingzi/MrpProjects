#ifndef _SMP_H
#define _SMP_H

#include "types.h"

//�û��������ɫ
#define COLOR_focus				0x00fe9900
#define COLOR_border			0x000a3a73
#define COLOR_controlbg			0x00e7f1f3
#define COLOR_controlhili			0x0075a4c7
#define COLOR_controlborder		0x00cfe3fe
#define COLOR_wndheader		0x00074c87


#ifdef SCREEN_SIZE_176220

#define SCREEN_WIDTH	176
#define SCREEN_HEIGHT	220

#define SMP_RADIOBOX_SIZE		16
#define SMP_HEADER_HEIGHT		25
#define SMP_TOOLBAR_HEIGHT	22 
#define SMP_ITEM_HEIGHT		20
#define SMP_ITEM_SPACE			2
#define SMP_ITEM_MARGIN		5
#define SMP_LIST_TOP_MARGIN	4
#define SMP_MSGBOX_MARGIN		4
#define SMP_MSGBOX_TBHEIHT	22
#endif


#ifdef SCREEN_SIZE_240320

extern int32 SCREEN_WIDTH,SCREEN_HEIGHT;

#define SMP_RADIOBOX_SIZE		20
#define SMP_HEADER_HEIGHT		24
#define SMP_TOOLBAR_HEIGHT	23 
#define SMP_ITEM_HEIGHT		26 
#define SMP_ITEM_SPACE			3
#define SMP_ITEM_MARGIN		9
#define SMP_LIST_TOP_MARGIN	3
#define SMP_MSGBOX_MARGIN		8
#define SMP_MSGBOX_TBHEIHT	26
#endif


#ifdef SCREEN_SIZE_240400

#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT	400

#define SMP_RADIOBOX_SIZE		20
#define SMP_HEADER_HEIGHT		29
#define SMP_TOOLBAR_HEIGHT	26 
#define SMP_ITEM_HEIGHT		26 
#define SMP_ITEM_SPACE			3
#define SMP_ITEM_MARGIN		9
#define SMP_LIST_TOP_MARGIN	3
#define SMP_MSGBOX_MARGIN		8
#define SMP_MSGBOX_TBHEIHT	26

#endif

#define SMP_ITEM_LENGTH			(SCREEN_WIDTH - 2*SMP_ITEM_MARGIN)
#define SMP_ITEM_SPACE_HEIGHT		(SMP_ITEM_HEIGHT + SMP_ITEM_SPACE)
#define SMP_CONTENT_VIEW_WIDTH	SCREEN_WIDTH
#define SMP_CONTENT_VIEW_HEIGHT	(SCREEN_HEIGHT - SMP_HEADER_HEIGHT - SMP_TOOLBAR_HEIGHT)
#define SMP_CHECKBOX_SIZE			SMP_RADIOBOX_SIZE
#define SMP_SCRBAR_WIDTH			14
#define SMP_MENU_BOTTOM_HEIGHT	SMP_TOOLBAR_HEIGHT
#define SMP_LIST_LEFT_MARGIN		3	
#define SMP_LIST_ITEM_HEIGHT		SMP_ITEM_HEIGHT

/**��/�ҿռ�
 */
#define SMP_ITEM_CONTENT_MARGIN		4 //pixels



/*******88*/
VOID SMP_DrawRadioBox(int x, int y, BOOL checked);

/**
 * \���Ƹ�ѡ���ܡ�
 * \����x		���λ��
 * \����Y		����λ��
 * \����check	�Ƿ�ѡ�и�ѡ��
 */

VOID SMP_DrawCheckBox(int x, int y, BOOL checked);

/**
 * \brief Draw a message box frame.\����Ϣ����
 *
 * \param x the left position of the msgbox
 * \param y the top position of the msgbox
 * \param w the width of the msgbox
 * \param h the height of the msgbox
 */
VOID SMP_DrawMsgBoxFrame(int x, int y, int w, int h);


/**
 * \����һ�����㴰�ڵı��⡣
 *
 * \����x			Ϊ��MsgBox���λ��
 * \����y			����λ�õ���MsgBox
 * \����w			�����MsgBox
 * \����h			�߶���MsgBox
 * \����bgcolor		������ɫ
 * \����fgcolor		ǰ��ɫ
 * \����bmpID		λͼ��ID
 * \����str			����ͷ
 */
VOID SMP_DrawWndHeader(int x, int y, int w, int h, Uint32 bgcolor, Uint32 fgcolor, DWORD bmpID, PCWSTR str);


#endif


