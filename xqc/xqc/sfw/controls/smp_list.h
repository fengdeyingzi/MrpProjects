#ifndef _SMP_LIST_H
#define _SMP_LIST_H

#include "window.h"

/**
  \列表控件

  要使用列表控件:
  - 创建你的数据回调
  - 创建列表控件
  - 设置列表数据回调
  - 增加到父窗口
  - 响应消息

  \code
  int YourListGetTotal(VOID)//列表总数回调
  {
  	...
  }

  VOID YourListGetRowData(int index, PSMPROWDATA pRowData)//获取列表行数据
  {
  	...
  }

  //创建列表控件
  hList = SGL_CreateWindow(SMP_List_WndProc, ...);
  SMP_List_SetDataProvider(hList, YourListGetTotal, YourListGetRowData);
  SGL_AddChildWindow(hWnd, hList);

  //响应通知消息
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the list id")
		{
			int index = (int)lParam; //选择行的位置
			switch(code)
			{
			case SMP_LISTN_HILICHANGED:
				break;
			case SMP_LISTN_SELECTED:
				break;
			case SMP_LISTN_CLICKED:
				break;
			}
		}	  
  \endcode
*/

/**
 * 列表没有滚动条
 */
#define SMP_LISTS_NOSCRBAR			0x0001L
/**
 * \高亮行改变时发送
 *
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the list id" && code == SMP_LISTN_HILICHANGED)
 *		{
 *			int index = (int)lParam;
 * 			//通知消息句柄
 *		}
 * \endcode
 *
 * \参数index  当前高亮位置
 */
#define SMP_LISTN_HILICHANGED			0x0001

/**
 * \按键弹起时发送
 *
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the list id" && code == SMP_LISTN_SELECTED)
 *		{
 *			int index = (int)lParam;
 * 			//通知消息句柄
 *		}
 * \endcode
 *
 * \参数index  当前高亮位置
 */
#define SMP_LISTN_SELECTED			0x0002

/**
 * \触屏弹起时发送
 *
 * \code
 *	case WM_COMMAND:
 *		WID id = LOWORD(wParam);
 *		WORD code = HIWORD(wParam);
 *
 *		if(id == "the list id" && code == SMP_LISTN_CLICKED)
 *		{
 *			int index = (int)lParam;
 * 			//通知消息句柄
 *		}
 * \endcode
 *
 * \参数index  当前高亮位置
 */
#define SMP_LISTN_CLICKED				0x0003

	/** @} */

/**
 * \brief Max supported collumns.  
 */
#define SMP_LIST_MAX_STR_COL		2

/**
 * \brief Row information  行信息
 */
typedef struct SMP_RowData
{
	/** left margin     左间距*/
	Uint16 margin;
	/** icon col width  行上图标宽度*/
	Uint16 colWidth0;
	/**                 行上图标宽 */
	uint16 colHeight0;
	/** icon handle     图标句柄*/
	HBITMAP hBmp;
	/** col information 行信息*/
	struct{
		/** col string  行字符*/
		PCWSTR str;
		/** col width   行字符显示宽度*/
		Uint16 width;
	}cols[SMP_LIST_MAX_STR_COL];	
}SMPROWDATA, *PSMPROWDATA;

/**
 * \brief The function provide to list to get total row count.
 *
 * \return the total rows of the list 返回列表总行数
 */
typedef int (*SMP_List_GetTotal)(VOID);
/**
 * \回调函数,获取行数据
 *
 * \参数 index            行位置
 * \参数[out] pRowData    行数据
 */
typedef VOID (*SMP_List_GetRowData)(int index, PSMPROWDATA pRowData);

/**
 * \画行程序
 *
 * \参数 index		行位置
 * \参数 r			矩形区域
 * \参数 pRowData	行数据
 * \参数 hilight	高亮行位置
 * \参数 flush		是否刷新
 */
typedef VOID (*SMP_List_DrawRow)(int index, PRECT r, PSMPROWDATA pRowData, int hilight, BOOL flush);

	/**
	 * \name Window Member Functions
	 * @{
	 */

/**
 * \brief Set user data provide callbacks.
 *
 * \param hList the list window handle
 * \param fnGetTotal the function to get total row count
 * \param fnGetRowData the function to get row data by index that from 0
 * 设置list控件的获取内容和获取行数回调函数
 * 这个list控件和一般的GUI实现的list控件有所不同
 * 它本身并不存储list的内容, 而是通过两个回调函数来获取总共多少行和每行的详细信息
 * 所以使用者需要告诉list控件两个回调函数
 * 参数:
 * hWnd:			list控件的句柄
 * fnGetTotal:		获取列表总共行数的回调函数
 * fnGetRowData:	获取列表每行的内容的回调函数
 * 返回值: 无
 */
VOID SMP_List_SetDataProvider(HWND hList, SMP_List_GetTotal fnGetTotal, SMP_List_GetRowData fnGetRowData);


/**
 * \设置一个特殊画行程序
 *
 * \参数 hList			窗口句柄
 * \参数 fnDrawRow		新画行程序
 */
VOID SMP_List_SetDrawRowCallback(HWND hList, SMP_List_DrawRow fnDrawRow);

/**
 * \brief The default draw row function.默认画行函数
 *
 * \param index the index of the list item 列表索引
 * \param r the list item draw rect 列表内行绘制矩形
 * \param pRowData the row data 行数据
 * \param hilight current hilight list item index 当前高亮行索引
 * \param flush if flush to screen 是否刷新
 */
VOID SMP_List_DefaultDrawRowCallback(int index, PRECT r, PSMPROWDATA pRowData, int hilight, BOOL flush);

/**
 * \通过行序改变高亮行.
 *
 * \参数hList     窗口句柄
 * \参数index     行位置
 */
 
VOID SMP_List_HilightByIndex(HWND hList, int index);

/**
 * \brief Get current high light row index.
 *
 * \param hList the List window handle
 * \return the row index
 * \获取高亮行位置
 *
 * \参数hList	列表窗口句柄
 * \返回行位置
 */
int SMP_List_GetHilightIndex(HWND hList);

/**
* \获取当前页起始项位置和每页的大小
*
* \参数hList				列表窗口句柄
* \参数[out] pagestart		当前页第一行索引
* \参数[out] pagesize		每页显示的行数
 */
VOID SMP_List_GetPageInfo(HWND hList, int* pagestart, int* pagesize);

/**
* \上翻页
*
* \参数 hList     窗口句柄
 */
VOID SMP_List_PageUp(HWND hList);

/**
* \下翻页
*
* \参数 hList     窗口句柄
 */
VOID SMP_List_PageDown(HWND hList);

/**
* \上翻行
*
* \参数 hList     窗口句柄
 */
VOID SMP_List_LineUp(HWND hList);

/**
* \下翻行
*
* \参数 hList     窗口句柄
*/
VOID SMP_List_LineDown(HWND hList);

/**
 * \通知列表,总行数改变
 *
 * \参数hList		窗口句柄
 * 刷新list控件, 当list的行数发生变化以后调用
 */
VOID SMP_List_Refresh(HWND hList);


/**
 * \重设列表
 * 
 * \参数 hList     窗口句柄
 * 焦点回到第一行
 */
VOID SMP_List_Reset(HWND hList);

	/** @} */

	/**
	 * \name Window Procedure
	 * @{
	 */

/**
 * \brief The simple list window procedure
			.窗口过程
 *
 * \param hList the window handle
 * \param Msg the window message
 * \param wParam the first parameter
 * \param lParam the second parameter
 * \return the result of message process 
 */
LRESULT SMP_List_WndProc(HWND hList, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */

/** @} end of smp_list */

#endif /* _SMP_LIST_H */

