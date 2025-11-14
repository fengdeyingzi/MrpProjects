#include "mrc_base.h"
#include "filetrace.h"


extern long FixedByFrac(long a, long b);


int32 mrc_init(void)
{
	long a=65539,b=65539,c=0;

	filetraceInit("testUseAsm.txt");

	c = FixedByFrac(a, b);
	
	FTRACE_I("a=%d, b=%d", a, b);
	FTRACE_I("汇编运算结果：%d", c);

	return MR_SUCCESS;
}

int32 mrc_exitApp(void)
{	
	filetraceClose();
	return MR_SUCCESS;
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{//计费状态下，自动返回
	return MR_SUCCESS;
}

int32 mrc_pause()
{//计费状态下，自动返回
	return MR_SUCCESS;	
}

int32 mrc_resume()
{
	return MR_SUCCESS;
}

#ifndef SDK_MOD
int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
	return MR_SUCCESS;
}
int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4,int32 p5)
{
	return MR_SUCCESS;
}
#endif