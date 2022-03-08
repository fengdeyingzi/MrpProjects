/*
** dialog.c: 简单通用对话框源码（例程）
** 
** Copyright 默水瓶
**
** Create date: 2011-01-31 by 默水瓶
**
** 可以任意使用，但要求注明原作者信息。如果你将此源码加以完善了，还请开源共享此资源！
*/

/*
void DialogEvent(char *DialogFilename)	//通用对话框销毁后会调用此函数，可以在这里处理接收到文件（路径）后的事件

void Dialog_Init(char *initDir, int8 fileOrDir)		//通用对话框初始化

*/  

#include "mrc_base.h"
#include "mrc_win.h"
#include "dialog.h"

struct{
	int8 w;
	int8 h;
}DialogFont;//字体宽高
mr_screeninfo DialogScreenInfo;//屏幕信息



mr_screenRectSt DialogLeft, DialogRight, DialogMiddle;//触屏区域

int8 DialogListLen;
int8 DialogListSum;
int8 DialogState;
char *DialogFilename[10];
char *DialogDir;
char *DialogBuffer;
char *DialogTitle;
int32 DialogSearch_handle;
int32 DialogCode;
Ptr_DialogEvent Dialog_Event;

void DialogFindStart(int8 isfirst);

char* Dialog_GetFileName()
{
	char *tempname;	
	tempname = (char*)malloc(50);
	mrc_memset(tempname,0,50);
/*
	if(DialogCode == MR_IS_DIR)
	{
		if(mrc_strcmp(".",DialogBuffer)==0 || mrc_strcmp("..",DialogBuffer)==0)
	}
*/
	if(mrc_strcmp("",DialogDir)!=0)
	{
		mrc_strcpy(tempname,DialogDir);
		mrc_strcat(tempname,"/");
	}
	mrc_strcat(tempname,DialogFilename[DialogState]);
	return tempname;
}

void Dialog_Delete()
{
	int8 i;

	for(i=0;i<DialogListLen;i++)
	{
		mrc_free((void*)DialogFilename[i]);
	}
	mrc_free((void*)DialogBuffer);
	mrc_findStop(DialogSearch_handle);
	mrc_winClose();
}

void Dialog_Init(char *initDir, int8 fileOrDir, char *dialogTitle, Ptr_DialogEvent DialogEvent)
{
	int8 i;

	DialogDir = initDir;
	DialogCode = fileOrDir;
	DialogTitle = dialogTitle;
	Dialog_Event = DialogEvent;

	//获取屏幕和字体的基础信息
	mrc_getScreenInfo(&DialogScreenInfo);
	mrc_textWidthHeight("高", 0, 1, (int32*)&DialogFont.w, (int32*)&DialogFont.h);
	DialogFont.w += 2;
	DialogFont.h += 2;

	DialogListLen = (DialogScreenInfo.height - DialogFont.h * 2)/DialogFont.h;
	DialogState = 0;
	if(DialogListLen > 10)
	{
		DialogListLen = 10;
	}
	for(i=0;i<DialogListLen;i++)
	{
		DialogFilename[i] = (char*)malloc(50);
	}
	DialogBuffer = (char*)malloc(50);
}
int8 DialogPointInObject(int32 px, int32 py, mr_screenRectSt *Rect)
{ 
    if (px < (Rect->x-1)) return FALSE; 
    if (px > Rect->x + Rect->w) return FALSE; 
    if (py < (Rect->y-1)) return FALSE; 
    if (py > Rect->y + Rect->h) return FALSE; 
    return TRUE; 
}
void DialogDrawButton(char* pcText, int8 position, mr_screenRectSt *Rect)
{ 
    int16 x=0, y=0; 
    int32 width, height; 

	mrc_textWidthHeight(pcText, FALSE, 1, &width, &height); 
	switch(position)//当position为1时文字在最左边, 为2时在中间, 为3时文字在最右边
	{
	case 3:
		x = (int16)(DialogScreenInfo.width - width - 1); 
		break;
	case 2:
		x = (int16)((DialogScreenInfo.width - width)/2);
		break;
	case 1:
		x = 2;
	}

	y = (int16)(DialogScreenInfo.height - height - 1); 
    mrc_drawText(pcText, x+1, y, 0, 0, 0, FALSE, 1); 
    mrc_drawText(pcText, x, y, 255, 255, 255, FALSE, 1); 

    Rect->x = x; 
    Rect->y = y; 
    Rect->w = width; 
    Rect->h = height; 
} 
void DialogFindStart(int8 isfirst)
{
	char *tempname;

	tempname = (char*)malloc(50);

	for(DialogListSum=0;DialogListSum<DialogListLen;DialogListSum++)
	{
		mrc_memset(DialogFilename[DialogListSum],0,50);
	}

	DialogListSum = 0;
	if(isfirst)
	{
		mrc_memset(tempname,0,50);
		mrc_strcpy(tempname,DialogDir);
		mrc_strcat(tempname,"/");
		mrc_strcat(tempname,DialogBuffer);
		if(DialogCode == MR_IS_DIR)
		{
			if(mrc_strcmp(".",DialogBuffer)==0 || mrc_strcmp("..",DialogBuffer)==0)
			goto a;
		}
		if(mrc_fileState(tempname) == DialogCode)
		{	
			mrc_strcpy(DialogFilename[DialogListSum],DialogBuffer);
			DialogListSum++;
		}
	}

a:	while(!mrc_findGetNext(DialogSearch_handle,DialogBuffer,50))
	{ 
		mrc_memset(tempname,0,50);
		mrc_strcpy(tempname,DialogDir);
		mrc_strcat(tempname,"/");
		mrc_strcat(tempname,DialogBuffer);
		if(DialogCode == MR_IS_DIR)
		{
			if(mrc_strcmp(".",DialogBuffer)==0 || mrc_strcmp("..",DialogBuffer)==0)
			continue;
		}
		if(mrc_fileState(tempname) == DialogCode)
		{	
			mrc_strcpy(DialogFilename[DialogListSum],DialogBuffer);
			DialogListSum++;
		}
		if(DialogListSum >= DialogListLen)
		{
			break;
		}
	}
	if(DialogListSum == 0)
	{ 
		DialogListSum = 1;
	}
	mrc_free((void*)tempname);

}
void DrawDialog()
{
	int8 i;

	mrc_drawRect(0,0,DialogScreenInfo.width,DialogFont.h,112,164,192);//标题
	mrc_drawRect(0,DialogFont.h,DialogScreenInfo.width,DialogScreenInfo.height-DialogFont.h*2,240,248,255);//列表背景
	mrc_drawRect(0,DialogScreenInfo.height-DialogFont.h,DialogScreenInfo.width,DialogFont.h,112,164,192);//工具条
	if(DialogState>=DialogListSum) DialogState = DialogListSum-1;
	mrc_drawRect(0,DialogFont.h * (DialogState+1),DialogScreenInfo.width,DialogFont.h,0x99,0xcc,0xff);//选择条
	mrc_drawText(DialogTitle,1,0,0,0,0,0,1);
	mrc_drawText(DialogTitle,0,0,255,255,255,0,1);
	DialogDrawButton("选定",1,&DialogLeft);
	DialogDrawButton("首页下页",2,&DialogMiddle);
	DialogDrawButton("取消",3,&DialogRight);
	for(i=0;i<DialogListLen;i++)
	{
		mrc_drawText(DialogFilename[i],10,DialogFont.h * (i+1),205,92,92,0,1);//文字
	}
	mrc_refreshScreen(0,0,DialogScreenInfo.width ,DialogScreenInfo.height );
}
void DialogWinEvent(int32 data, int32 eventId)
{
	switch (eventId)
	{
	case WIN_EVENT_SHOW:
		DialogSearch_handle = mrc_findStart(DialogDir,DialogBuffer,50);
		DialogFindStart(TRUE);

	case WIN_EVENT_REFRESH:
		DrawDialog();
		break;
	case WIN_EVENT_PAUSE:
		break;
	case WIN_EVENT_EXIT:
		break;
	case WIN_EVENT_UPDATE:
		break;
	}
}
void DialogKeyEvent(int32 data, int32 type, int32 p1, int32 p2)
{	
	int8 aaa;
	mr_screenRectSt rect;

	if(MR_KEY_PRESS == type)
	{
		switch(p1)
		{
		case MR_KEY_SELECT:	
		case MR_KEY_SOFTLEFT:
			Dialog_Event(data, Dialog_GetFileName());
		case MR_KEY_SOFTRIGHT:
			Dialog_Delete();
			return;
		case MR_KEY_UP:
			if(DialogState>0) DialogState -= 1;
			else DialogState = DialogListSum-1;
			DrawDialog();
			return;
		case MR_KEY_DOWN:
			if(DialogState<DialogListSum-1) DialogState += 1;
			else DialogState = 0;
			DrawDialog();
			return;
		case MR_KEY_LEFT:
			mrc_findStop(DialogSearch_handle);
			DialogSearch_handle = mrc_findStart(DialogDir,DialogBuffer,50);
			DialogFindStart(TRUE);
			DrawDialog();
			return;
		case MR_KEY_RIGHT:
			DialogFindStart(FALSE);
			DrawDialog();
			return;
		}
	}
	else if(MR_MOUSE_DOWN == type)
	{
		aaa = p2/DialogFont.h - 1;
		if(aaa <= DialogListSum-1 && aaa!=-1)
		{
			DialogState = aaa;
			DrawDialog();
			Dialog_Event(data, Dialog_GetFileName());
			Dialog_Delete();

		}
		else if(DialogPointInObject(p1,p2,&DialogRight))
		{	
			Dialog_Delete();
			return;
		}
		else if(DialogPointInObject(p1,p2,&DialogLeft))
		{
			Dialog_Event(data, Dialog_GetFileName());
			Dialog_Delete();
			return;
		}
		rect = DialogMiddle;
		rect.w = rect.w / 2;
		if(DialogPointInObject(p1,p2,&rect))//首页
		{
			mrc_findStop(DialogSearch_handle);
			DialogSearch_handle = mrc_findStart(DialogDir,DialogBuffer,50);
			DialogFindStart(TRUE);
			DrawDialog();
			return;
		}
		rect.x += rect.w;
		if(DialogPointInObject(p1,p2,&rect))//下页
		{
			DialogFindStart(FALSE);
			DrawDialog();
		}
	}
}



