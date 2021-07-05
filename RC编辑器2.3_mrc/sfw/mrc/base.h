//此文件用于做到与mpc通用，没有实际意义

#ifndef _BASE_H_
#define _BASE_H_


#include <mpc.h>
#define LOGFILE "c/print.txt"

void printfln(char *text);



long mrc_init()
{
mpc_init();

remove(LOGFILE);
printfln("启动");
init();
return 0;
}


long mrc_event(int32 type,int32 p1,int32 p2)
{
event(type,p1,p2);

return 0;
}


long mrc_pause()
{

return pause();
}

long mrc_resume()
{
return resume();
}

long mrc_exitApp()
{
return 0;
}


//新版sdk不添加安全模块是的需要
/*
int32 mrc_exrtRecvAppEventEx()
{
	return 0;
}

int32 mrc_extRecvAppEvent()
{
    return 0;
}
*/


void printfs(char *text)
{
long f=open(LOGFILE,12);
seek(f,0,SEEK_END);
write(f,text,strlen(text));
close(f);

}

void printfln(char *text)
{

long f=open(LOGFILE,12);
seek(f,0,SEEK_END);
write(f,text,strlen(text));
write(f,"\x0d\x0a",2);
close(f);

}

void printfd(long size)
{
char text[32];
long f=open(LOGFILE,12);
seek(f,0,SEEK_END);
sprintf(text," %d ",size);
write(f,text,strlen(text));
close(f);
}

#endif






