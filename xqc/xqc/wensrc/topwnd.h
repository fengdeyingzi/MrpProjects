#ifndef _TOPWND_H
#define _TOPWND_H

#include "window.h"

/**
* \简短的顶层窗口ID开始
 */
#define TOPWND_ID_START		300


/**
* \简短的顶级窗口ID

*

*顶层窗口ID应该有一个条目

* inittopwindow实现
 */
enum {
	TOPWND_MAINWND = TOPWND_ID_START,
	//在这里添加你的顶层窗口ID
	
	TOPWND_GUANYU, //继续
	TOPWND_BANGZHU,
	TOPWND_CHARU,
	TOPWND_OPENDLG,
	TOPWND_SET,
	TOPWND_PALETTE,
	TOPWND_KEYPAD,

	MAX_TOPWND_COUNT
};

/**
* \\短暂的初始化的顶窗

*

*这个函数应该被称为initapplication。
 */
VOID InitTopWindow(VOID);

/**
* \\简单得顶窗

*

* \\参数WID顶部窗口ID

* \\返回
 */
HWND GetTopWindow(WID wid);

/**
* \\短暂显示前窗

* \\参数WID顶部窗口ID

* \\参数顶部窗口监听器监听器

* \\参数用户数据用户特定的数据

* \\返回顶部的窗口句柄，空时失败
 */
HWND ShowTopWindow(WID wid, HWND listener, DWORD userdata);

/**
* \\简短的隐藏的顶窗

*

* \\参数WID顶部窗口ID

* \\参数破坏如果毁灭窗口

* \\参数重画如果刷新顶窗
 */
VOID HideTopWindow(WID wid, BOOL destroy, BOOL redraw);


#endif
