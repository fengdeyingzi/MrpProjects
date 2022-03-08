#ifndef _SMP_H
#define _SMP_H

#include "types.h"

//用户定义的颜色
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

/**左/右空间
 */
#define SMP_ITEM_CONTENT_MARGIN		4 //pixels



/*******88*/
VOID SMP_DrawRadioBox(int x, int y, BOOL checked);

/**
 * \绘制复选框框架。
 * \参数x		左侧位置
 * \参数Y		顶部位置
 * \参数check	是否选中复选框
 */

VOID SMP_DrawCheckBox(int x, int y, BOOL checked);

/**
 * \brief Draw a message box frame.\画信息框框架
 *
 * \param x the left position of the msgbox
 * \param y the top position of the msgbox
 * \param w the width of the msgbox
 * \param h the height of the msgbox
 */
VOID SMP_DrawMsgBoxFrame(int x, int y, int w, int h);


/**
 * \绘制一个顶层窗口的标题。
 *
 * \参数x			为在MsgBox左侧位置
 * \参数y			顶部位置的在MsgBox
 * \参数w			宽度在MsgBox
 * \参数h			高度在MsgBox
 * \参数bgcolor		背景颜色
 * \参数fgcolor		前景色
 * \参数bmpID		位图的ID
 * \参数str			标题头
 */
VOID SMP_DrawWndHeader(int x, int y, int w, int h, Uint32 bgcolor, Uint32 fgcolor, DWORD bmpID, PCWSTR str);


#endif


