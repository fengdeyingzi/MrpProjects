#include "mrc_base.h"

extern int32 mrc_platExCP(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
extern int32 mrc_platCP(int32 code, int32 param);

/*
功能:
	打开、关闭LCD定时休眠功能。
	
输入:
	char CanSleep=1 时，允许LCD定时休眠；
	char CanSleep=0时，不允许LCD定时休眠。

输出：
MR_SUCCESS ：成功
MR_FAILED  ：失败
MR_IGNORE  : 不支持该功能
*/
int32 mrc_LCDCanSleep(char CanSleep);