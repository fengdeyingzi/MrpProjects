#include "mrc_base.h" 
#include "mrc_win.h"
#include "mrc_exb.h"

#include "dialog.h"
#include "mrp.h"

#define MainText "\x68\x39\x76\xee\x5f\x55\x0\x3a\x0\x6d\x0\x79\x0\x74\x0\x68\x0\x72\x0\x6f\x0\x61\x0\x64\x0\xa\x62\x53\x53\x5\x76\xee\x5f\x55\x0\x3a\x0\x74\x0\x65\x0\x6d\x0\x70\x0\xa\x62\x53\x53\x5\x65\x87\x4e\xf6\x0\x3a\x0\x4d\x0\x79\x0\x4d\x0\x72\x0\x70\x0\x2e\x0\x6d\x0\x72\x0\x70\x0\xa\x0\x28\x66\x82\x4e\xd\x65\x2f\x63\x1\x65\x87\x4e\xf6\x53\x8b\x7f\x29\x0\x21\x0\x29"

mr_screenRectSt rect;
mr_colourSt color;
mr_screenRectSt gbuttonjy, gbuttonjb, gbuttondb, gSoftRight, gSoftLeft;
mr_screeninfo ScreenInfo;
char *mrpfile;
int8 state;

struct{
	int32 w;
	int32 h;
}font;

void showmain(void);



void DialogEvent(int32 data, char *filename)
{

	//在这里处理对话框销毁后接收文件名的事件
	mrc_memset(mrpfile,0,50);
	mrc_strcpy(mrpfile,filename);
	//使用完毕要释放资源
	mrc_free(filename);
}
void ShowLineText(int8 line, char *text, uint8 r, uint8 g, uint8 b)
{
	static int8 sum;
	if(!line)
	{
		mrc_clearScreen(0,0,0);
		sum=0;
	}
	if(sum >= (ScreenInfo.height / font.h - 1))
	{
		mrc_clearScreen(0,0,0);
		sum=0;
	}
	else
	{
		sum++;
	}
	mrc_drawText(text, 0, font.h * (sum-1), r, g, b, 0,1);
	mrc_refreshScreen(0, 0, ScreenInfo.width, ScreenInfo.height);
}
void textback(void)//画文字背景
{
	int32 y,h;
	y=ScreenInfo.height-font.h-2;
	h=font.h/2;
	mrc_drawRect(0,y,ScreenInfo.width,h,100,150,200);
	y+=h;
	h=ScreenInfo.height-y;
	mrc_drawRect(0,y,ScreenInfo.width,h,50,100,200);
}

int8 IsPointInObject(int32 px, int32 py, mr_screenRectSt *Rect) //判断是否为指定区域
{ 
    if (px < (Rect->x-1)) return FALSE; 
    if (px > Rect->x + Rect->w) return FALSE; 
    if (py < (Rect->y-1)) return FALSE; 
    if (py > Rect->y + Rect->h) return FALSE; 
    return TRUE; 
} 

void tooldrawButton(char* pcText, int8 position, mr_screenRectSt *Rect)
{ 
    int16 x, y; 
    int32 width, height; 

	mrc_textWidthHeight(pcText, FALSE, 1, &width, &height); 
   if(position == 3)
   {
	   x = (int16)(ScreenInfo.width - width - 1); 
   }
   else
   {
	   x = 2;
   }
    y = (int16)(ScreenInfo.height - height-2); 
    mrc_drawText(pcText, x, y, 255, 255, 255, FALSE, 1); 
    mrc_drawText(pcText, x-1, y, 250, 250, 250, FALSE, 1); 

    Rect->x = x; 
    Rect->y = y; 
    Rect->w = width; 
    Rect->h = height; 
} 
void drawButton(char* pcText, int32 line, mr_screenRectSt *Rect) //解压
{ 
    int16 x, y; 
    int32 width, height; 
    mrc_textWidthHeight(pcText, FALSE, 1, &width, &height); 
    x = 0; 
    y = font.h * line; 
    mrc_drawText(pcText, x, y, 255, 255, 255, FALSE, 1); 

    Rect->x = x; 
    Rect->y = y; 
    Rect->w = width; 
    Rect->h = height; 
} 

void showmain(void)
{
	mrc_clearScreen(0, 0, 0);
	mrc_drawTextEx(MainText,
	rect.x,rect.y,rect,color,
	DRAW_TEXT_EX_IS_UNICODE | DRAW_TEXT_EX_IS_AUTO_NEWLINE,MR_FONT_MEDIUM);
	textback();
	drawButton("1键:解压文件到目录",5,&gbuttonjy);
	drawButton("2键:解包压缩文件",6,&gbuttonjb);
	drawButton("3键:打包所有文件",7,&gbuttondb);
	
	mrc_drawText("解压文件:",0,font.h * 8,255,255,255,0,1);
	mrc_drawText(mrpfile,0,font.h * 9,255,255,255,0,1);

	tooldrawButton("选择文件",1,&gSoftLeft);
	tooldrawButton("退出",3,&gSoftRight);
	mrc_refreshScreen(0,0,ScreenInfo.width ,ScreenInfo.height );
}
void Demo1WinWinEvent(int32 data, int32 eventId)
{
	switch (eventId)
	{
	case WIN_EVENT_SHOW:
	case WIN_EVENT_REFRESH:	
		showmain();
		break;
	case WIN_EVENT_PAUSE:
		break;
	case WIN_EVENT_EXIT:
		break;
	case WIN_EVENT_UPDATE:
		break;
	}
}
void NewText(char *text)
{
	mrc_clearScreen(0,0,0);
	mrc_drawTextEx(text,
	rect.x,rect.y,rect,color,
	DRAW_TEXT_EX_IS_UNICODE | DRAW_TEXT_EX_IS_AUTO_NEWLINE,MR_FONT_MEDIUM);
	mrc_refreshScreen(0, 0, ScreenInfo.width , ScreenInfo.height );
}
void progbar(uint8 value, uint8 error)
{
	char *temp;

	switch(value)
	{
	case 1:
		ShowLineText(0,"启动打包模块...",248,0,0);
		break;
	case 2:
		ShowLineText(1,"开始搜索文件...",248,0,0);
		break;
	case 3:
		ShowLineText(1,"开始写文件头...",248,0,0);
		break;
	case 4:
		ShowLineText(1,"创建文件列表...",248,0,0);
		break;
	case 5:
		ShowLineText(1,"读入文件数据...",248,0,0);
		break;
	case 6:
		ShowLineText(1,"更新文件信息...",248,0,0);
		break;
	case 7:
		if(error)
		{
			temp = (char*)malloc(50);
			mrc_sprintf(temp,"出现了%d个错误",error);
			ShowLineText(1,temp,248,0,0);
			mrc_free((void*)temp);
		}
		break;
	}
}
void dabao(void)
{
	int32 ret=0;
	char *aa="我的MRP程序";
	TOMRPINFO MRPinfo;

	MRPinfo.appid = 1000;//APPID
	MRPinfo.version = 1001;//版本ID
	MRPinfo.appname = aa;
	MRPinfo.description = aa;
	MRPinfo.filename = "MyMrp.mrp";
	MRPinfo.vendor = "默水瓶";

	ret = ToMrp("./temp","./MyMrp.mrp",204800, &MRPinfo, progbar);
	switch(ret)
	{
	case MRP_SUCCESS:
		ShowLineText(1,"打包操作成功！",255,255,255);
		return;
	case MRP_SEARCHFAILED:
		ShowLineText(1,"无法搜索文件！",255,255,255);
		break;
	case MRP_OPENFAILED:
		ShowLineText(1,"无法打开文件！",255,255,255);
		break;
	case MRP_LISTLENFAILED:
		ShowLineText(1,"没有找到文件！",255,255,255);
		break;
	default:
		break;
	}
	ShowLineText(1,"打包操作失败！",255,255,255);
}

void jb(void)
{		
	if(state == 10) return;

	state = 10;
	//正在解包。。。可能需要较长时间！请等待。。。。
	NewText("\x6b\x63\x57\x28\x89\xe3\x53\x5\x30\x2\x30\x2\x30\x2\x53\xef\x80\xfd\x97\x0\x89\x81\x8f\x83\x95\x7f\x65\xf6\x95\xf4\xff\x1\x0\xa\x8b\xf7\x7b\x49\x5f\x85\x30\x2\x30\x2\x30\x2\x30\x2");
	if(MR_FAILED != UnMrp(mrpfile,"temp",204800, FALSE))
	{
		NewText("\x89\xe3\x53\x5\x5b\x8c\x62\x10\xff\x1");
	}
	else
	{
		NewText("\x89\xe3\x53\x5\x59\x31\x8d\x25\xff\x1");
	}	
	textback();
	tooldrawButton("返回",1,&gSoftLeft);
	mrc_refreshScreen(0,0,ScreenInfo.width ,ScreenInfo.height );

}
void jy(void)
{			
	if(state == 10) return;

	state = 10;
	//正在解压。。。可能需要较长时间！请等待。。。。
	NewText("\x6b\x63\x57\x28\x89\xe3\x53\x8b\x30\x2\x30\x2\x30\x2\x53\xef\x80\xfd\x97\x0\x89\x81\x8f\x83\x95\x7f\x65\xf6\x95\xf4\xff\x1\x0\xa\x8b\xf7\x7b\x49\x5f\x85\x30\x2\x30\x2\x30\x2\x30\x2");
	if(MR_FAILED != UnMrp(mrpfile,"temp",204800, TRUE))
	{
		//解压完成！
		NewText("\x89\xe3\x53\x8b\x5b\x8c\x62\x10\xff\x1");
	}
	else
	{
		//解压失败！
		NewText("\x89\xe3\x53\x8b\x59\x31\x8d\x25\xff\x1");
	}
	textback();
	tooldrawButton("返回",1,&gSoftLeft);
	mrc_refreshScreen(0,0,ScreenInfo.width ,ScreenInfo.height );

}
void db(void)
{
	if(state == 10) return;

	state = 10;

	dabao();
	textback();
	tooldrawButton("返回",1,&gSoftLeft);
	mrc_refreshScreen(0,0,ScreenInfo.width ,ScreenInfo.height );

}

void keyLeft(void)
{
	if(state == 10) 
	{
		showmain();
		state = 0;
	}
	else
	{
		Dialog_Init("",MR_IS_FILE,"默水瓶QQ:243786753",DialogEvent);
		mrc_winNew(0, DialogWinEvent, DialogKeyEvent);
	}
}

void keyRight(void)
{
	if(state != 10)
	{
		mrc_winClose();
		mrc_exit();
	}
}

void Demo1KeyWinEvent(int32 data, int32 type, int32 p1, int32 p2)
{
	if(MR_KEY_PRESS == type)
	{
		switch(p1)
		{
		case MR_KEY_SOFTRIGHT:
			keyRight();
			return;
		case MR_KEY_SOFTLEFT:
			keyLeft();
			return;
		case MR_KEY_1://解压
			jy();
			return;
		case MR_KEY_2://提取
			jb();
			return;
		case MR_KEY_3:
			db();
			return;
		default:
			return;
		}
	}
	else if(MR_MOUSE_DOWN == type)
	{
		if(IsPointInObject(p1,p2,&gSoftRight))
		{
			keyRight();
		}
		else if(IsPointInObject(p1,p2,&gSoftLeft))
		{
			keyLeft();
		}
		else if(IsPointInObject(p1,p2,&gbuttonjy))
		{
			jy();
		}
		else if(IsPointInObject(p1,p2,&gbuttonjb))
		{
			jb();
		}
		else if(IsPointInObject(p1,p2,&gbuttondb))
		{
			db();
		}
	}
}

int32 mrc_init(void)
{	
	mrpfile = (char*)malloc(50);
	mrc_memset(mrpfile,0,50);

	mrc_getScreenInfo(&ScreenInfo); 
	mrc_textWidthHeight("高", 0, 1, &font.w, &font.h);
	font.h += 2;
	rect.x = 0, rect.y =0, rect.w = ScreenInfo.width, rect.h = ScreenInfo.height; 
	color.r = 255, color.g = 255, color.b = 0; 

	mrc_winInit();
	mrc_winNew(0, Demo1WinWinEvent, Demo1KeyWinEvent);
		
	return MR_SUCCESS;
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
	mrc_winEvent(code, param0, param1);
	return MR_SUCCESS;
}


int32 mrc_pause(void)
{
	return 0;
}


int32 mrc_resume(void)
{
	mrc_winResume();
	return 0;
}


int32 mrc_exitApp(void)
{
	mrc_free(mrpfile);
	mrc_clearScreen(0,0,0);
	mrc_drawText("正在退出。。。",0,0,255,255,255,0,MR_FONT_MEDIUM);
	mrc_refreshScreen(0,0,240,320);
	return 0;
}