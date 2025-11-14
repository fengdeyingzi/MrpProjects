#include "mrc_base.h"

/**
 * 测试调用其他文件函数
 */
void cls1(void) {
	mrc_clearScreen(0, 0, 0);
	mrc_drawText("哈喽沃德", 0, 0, 255, 255, 255, 0, 1);
	mrc_refreshScreen(0, 0, 480, 640);
}