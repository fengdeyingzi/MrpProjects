
// #include <stdlib.h>
#include <mrc_base.h>
#include <mrc_base_i.h>
#include "lua.h"
#include "base.h"

extern int32 lua_app_main (int argc, char **argv);
extern int32 lua_app_event(int32 type, int32 p1,int32 p2);
extern int32 lua_app_pause();
extern int32 lua_app_resume();
extern int32 lua_app_exit();
int SCRW;
int SCRH;
CAPP_FILE *base_stdin;
CAPP_FILE *base_stdout;
CAPP_FILE *base_stderr;

int event(int type,int p1,int p2){
    
    return 0;
}

int pause(){
    return 0;
}

int resume(){
    return 0;
}

int exitApp(){
    return 0;
}
int init()
{
	
    return 0;
    // system("pause");
}

void drawPrint(char *text){
    mrc_clearScreen(255,255,255);
	mrc_drawText(text,0,0, 20,20,20, 0, 1);
	mrc_refreshScreen(0,0,SCRW,SCRH);
    mrc_sleep(300);
    mrc_printf("drawPrint %s",text);
}


int32 mrc_init()
{
    mr_screeninfo scrinfo;
    char *args[3] = {"hello.lua","hello.lua",NULL};
    
	
	mrc_getScreenInfo(&scrinfo);
	SCRW = scrinfo.width;
	SCRH = scrinfo.height;
    base_stdin = capp_fopen("in.lua","r");
    base_stdout = capp_fopen("print.txt","wb+");
    base_stderr = base_stdout;
	mrc_clearScreen(255,255,255);
	mrc_drawText("test lua",0,0, 20,20,20, 0, 1);
	mrc_refreshScreen(0,0,SCRW,SCRH);
    mrc_sleep(100);
    
    lua_app_main(2,args);
    
	return 0;
}

int32 mrc_event(int32 type, int32 p0, int32 p1)
{
    if(type == MR_KEY_UP){
        mrc_exit();
    }
	return lua_app_event(type,p0,p1);
}
int32 mrc_pause()
{
	return lua_app_pause();
}
int32 mrc_resume()
{
	return lua_app_resume();
}
int32 mrc_exitApp()
{
	return lua_app_exit();
}

 int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1){
	return 0;
 }
 int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5){
	return 0;
}

int main(){
    return 0;
}
