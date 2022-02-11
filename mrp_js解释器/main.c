#include "mrc_base.h"

int SCRW;
int SCRH;
int32 mrc_init()
{
	mr_screeninfo scrinfo;
	mrc_getScreenInfo(&scrinfo);
	SCRW = scrinfo.width;
	SCRH = scrinfo.height;
	mrc_clearScreen(255,255,255);
	mrc_drawText("helloworld",0,0, 20,20,20, 0, 1);
	mrc_refreshScreen(0,0,SCRW,SCRH);
	return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
	return 0;
}
int32 mrc_pause()
{
	return 0;
}
int32 mrc_resume()
{
	return 0;
}
int32 mrc_exitApp()
{
	return 0;
}

 int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1){
	return 0;
 }
 int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5){
	return 0;
}