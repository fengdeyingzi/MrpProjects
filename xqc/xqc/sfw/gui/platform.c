/*
** platform.c: implements of platform.h
** 
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** Create date: 2007-12-27 by wandonglin
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#include "window.h"
#include "i18n.h"
#include "mrc_network.h"
#include "platform.h"
#include "bmp.h"
#include "mrc_bmp.h"
#ifdef SIM_MOD
#include <momo.h>
extern char suc;
#define PLATRUMMODE 0x02  //±à¼­Æ÷
int32 xqc_appEvent(int32 code, int32 param0, int32 param1);
#endif
#ifdef ENABLE_TRACE_TO_FILE

char g_TraceBuffer[FILE_TRACE_BUFFER_SIZE];
int32 g_TraceFD;

#endif
int32 SCREEN_WIDTH,SCREEN_HEIGHT;


int32 mrc_init(void)
{
	mr_screeninfo scrinfo;

	mrc_getScreenInfo(&scrinfo);
	SCREEN_WIDTH=scrinfo.width;
	SCREEN_HEIGHT=scrinfo.height;
#ifdef SIM_MOD
	suc=PLATRUMMODE;
#endif
#ifdef ENABLE_TRACE_TO_FILE
	SGL_FILE_TRACE_BEGIN("trace.log");
#endif

	SGL_TRACE("%s, %d: INIT APPLICATION\n", __FILE__, __LINE__);

	//initialize i18n
	SGL_I18nInitialize();

	//initialize THE SKY GUI LIBRARY
	SGL_Initialize();

#if !OMIT_NETWORK_MODULE
	//initialize the network module
	mrc_Socket_Initialize();
	mrc_Http_Initialize();
#endif

	//call user application entrance
	return InitApplication();
}

int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	//translate the message from mythroad to simple gui library	

#ifdef SIM_MOD
	if(!_IS_SET_BIT(suc,PLATRUMMODE))
	{
		xqc_appEvent(code, param0, param1);
		return 0;
	}
#endif
	switch(code)
	{

	case MR_MOUSE_DOWN:
		SGL_DispatchEvents(WM_MOUSEDOWN, param0, param1);
		break;

	case MR_MOUSE_MOVE:
		SGL_DispatchEvents(WM_MOUSEMOVE, param0, param1);
		break;

	case MR_MOUSE_UP:
		SGL_DispatchEvents(WM_MOUSEUP, param0, param1);
		break;

	case MR_KEY_PRESS:
		SGL_DispatchEvents(WM_KEYDOWN, param0, param1);
		break;

	case MR_KEY_RELEASE:
		SGL_DispatchEvents(WM_KEYUP, param0, param1);
		break;
	
	default:
		SGL_DispatchEvents(code, param0, param1);
		break;
	}
	return 0;
}

int32 mrc_pause(void)
{
	SGL_TRACE("%s, %d: PAUSE APPLICATION\n", __FILE__, __LINE__);
	
	SGL_SuspendDrawing();

#ifdef ENABLE_SGL_KEYREPEAT		
	SGL_StopKeyRepeat();
#endif

	//clall the user application specified function
	return PauseApplication();
}

int32 mrc_resume(void)
{
	HWND hTopWnd = SGL_GetNextChild(HWND_DESKTOP, NULL);

	SGL_TRACE("%s, %d: RESUME APPLICATION\n", __FILE__, __LINE__);

	SGL_UnsuspendDrawing();

	if(hTopWnd)
		SGL_UpdateWindow(hTopWnd);

	//class the user application specified function
	return ResumeApplication();
}

int32 mrc_exitApp(void)
{
	//TODO:
	//call user application function
	LRESULT r;


	SGL_SuspendDrawing();
	r = ExitApplication();

	SGL_TRACE("%s,%d: EXIT APPLICATION\n", __FILE__, __LINE__);
	
	//terminate the SKY GUI LIBRARY
	SGL_Terminate();

#if !OMIT_NETWORK_MODULE
	//terminate the network module
	mrc_Socket_Terminate();
	mrc_Http_Terminate();
#endif

#ifdef ENABLE_TRACE_TO_FILE
	SGL_FILE_TRACE_END();
#endif

	return r;
}


#ifndef _EXT_RELEASE

void sgl_assert(const char* file, int line, int exp)
{
	if(!exp)
	{
		SGL_TRACE("assert failed %s:%d\n", file, line);
		mrc_exit();
	}
}


#define MAX_LOGFILE_SIZE	1024*10
void sgl_log(const char* file, void* buffer, int size)
{
	char tmp[32];
	int32 stream, sendbytes;
	char * data = (char*)buffer;
	if(size <= 0) return;

	if(mrc_getLen(file) > MAX_LOGFILE_SIZE)
	{
		SGL_STRCPY(tmp, file);
		SGL_STRCAT(tmp, ".bak");
		mrc_remove(tmp);
		mrc_rename(file, tmp);
	}
	
	stream = mrc_open(file, MR_FILE_WRONLY | MR_FILE_CREATE);
	if(stream)
	{
		mrc_seek(stream, 0, MR_SEEK_END);
		while(size > 0 && (sendbytes = mrc_write(stream, data, size)) >= 0) 
		{
			SGL_TRACE("sgl_log: %d, %d\n", size, sendbytes);
			size -= sendbytes;
			data += sendbytes;
		}
		mrc_close(stream);
	}
}

#endif

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5)
{
	// TODO:

	return 0;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
	// TODO:
	
	return 0;
}


#ifdef WIN32
#include "mrc_bmp.h"
int32 mrc_Img2bmp(uint8 *pImg, int32 ImgSize, MRC_IMG_TYPE ImgType, uint16 **bmp, int32 *w, int32 *h)
{
	return MR_FAILED;
}
#endif


#ifdef MR_SPREADTRUM_MOD
int main(int argc, char* argv[])
{
	return 0;
}
#endif


