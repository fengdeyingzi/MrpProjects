#include "mrc_base.h"

#include "xqc.h"
#include "picoc.h"
#include "interpreter.h"
#include "platform.h"
#include "mrc_exb.h "
#include "momo.h "
#include "ext.h "
#include "libini.h"

#define PICINITSUC 0x01   //运行器
#define PLATRUMMODE 0x02  //编辑器
#define EXITRUMMODE 0x04  //任意键退出
#define DIRLEN  18

//#define SCRCD
#ifdef SIM_MOD
#define  LIANHE
#endif



int32 _code;
int32 _p0;
int32 _p1;
char suc;
char tempfile_path[64];


uint16 SCREEN_WIDTH;
uint16 SCREEN_HEIGHT;
int HEAP_SIZE;         /* 堆和堆栈空间 */
int Debugts;
#ifdef includefile
int PACKGEDMRP;
extern char *includechar[20];
void mpr_tou(INI *pini);
#endif
/*调试输出*/
void prints(char *text,int n)
{
	char a[25];int32 f;
	sprintf(a,"\r%d\n\n",n);
	f=mrc_open("print.txt",12);
	if(f>0)
	{
		mrc_seek(f,0,2);
		mrc_write(f,text,strlen(text));
		mrc_write(f,a,strlen(a));
		mrc_close(f);
	}
}
char *getfile(char *file)
{
	char *temp;
	if(temp=mrc_strrchr(file,'/'))
		return temp+1;
	if(temp=mrc_strrchr(file,'\\'))
		return temp+1;
	else
		return file;
}
void *filenei(const char *name,uint32 *Len)
{
	int32 len=0;
	char *p=NULL,*P=NULL;
	Len[0]=0;
	p=mrc_readFileFromMrp(name, &len, 3);
	if(p==NULL)return NULL;
	P=mrc_malloc(len+1);
	if(P==NULL)
	{
		mrc_freeFileData(p,len);
		return NULL;
	}
	P[len]=0;
	memcpy(P,p,len);
	mrc_freeFileData(p,len);
	*Len=len;
	return (void*)P;
}
int xqc_setinis()
{
	INI *p;
	int tmpi;
#ifdef includefile
	p=ini_open("xqc/temp/exchange.ini");
#else
	char *text;uint32 len;
#ifdef SIM_MOD
	text=mrc_readAll("xqc/temp/setup.ini",&len);
	text[len]=0;
#else
	text=filenei("setup.ini",&len);
#endif
	p=ini_open((char *)text);
#endif
	if(!p)
		return -1;
	ini_get(p,"exchange@MAINNAME",&tempfile_path[0]);
	tmpi=ini_get_int(p,"exchange@SCRW");
	if(tmpi)
		SCREEN_WIDTH=tmpi;
	tmpi=ini_get_int(p,"exchange@SCRH");
	if(tmpi)
		SCREEN_HEIGHT=tmpi;
	HEAP_SIZE=ini_get_int(p,"exchange@HEAP_SIZE")*1024;
	Debugts=ini_get_int(p,"exchange@DEBUG");
#ifdef includefile
	PACKGEDMRP=ini_get_int(p,"exchange@PACKGEDMRP");
#endif
	ini_close(p);
	return 0;
}


int setupfile(char *file)
{
	mr_screeninfo screen;
	INI *p;
	mrc_remove(file);
	p=ini_create(file);
	if(!p)
		return -1;
	ini_append(p,"exchange@MAINNAME",getfile(tempfile_path));
	ini_append_int(p,"exchange@HEAP_SIZE",HEAP_SIZE/1024);
	mrc_getScreenInfo(&screen);
	if(SCREEN_WIDTH != (uint16)(screen.width)||SCREEN_HEIGHT != (uint16)(screen.height))
	{
		ini_append_int(p,"exchange@SCRW",SCREEN_WIDTH);
		ini_append_int(p,"exchange@SCRH",SCREEN_HEIGHT);
	}
	ini_append_int(p,"exchange@DEBUG",0);
	ini_save(p);
	ini_close(p);
	return 0;
}

#ifndef SIM_MOD
int32 runmrps(char *name)
{
	char *qd;
	if(mrc_fileState("xqc/mtk.txt")==MR_IS_FILE)
		qd="start.mr";
	else
		qd="cfunction.ext";
	return mrc_runMrp(name,qd,NULL);
}

#endif
void SafeExit(int32 data)
{
	if(!data)
	{
#ifdef includefile
		if(!PACKGEDMRP)
			TextScreen("结束运行",0);
#endif
		data=mrc_timerCreate();
		mrc_timerStart(data,1,data,SafeExit,0);
		return;
	}
	else
	{
		if(suc!=EXITRUMMODE)
		{
			mrc_timerDelete(data);
			PicocCleanup();
			if(Debugts)
			{
				int r;
				r=mrc_getLen(XQC_PRINT_FILE);
				if(r>0)
				{
					int32 f;
					char *dt=mrc_malloc(r+1);
					f=mrc_open(XQC_PRINT_FILE,MR_FILE_RDONLY);
					mrc_read(f,dt,r);
					dt[r]=0;
					mrc_close(f);
					TextScreen(dt,0);
					mrc_free(dt);
					_SET_BIT(suc,EXITRUMMODE);
					Debugts=0;
					return;
				}
			}
		}
#ifdef includefile
#ifdef SIM_MOD
		_SET_BIT(suc,PLATRUMMODE);
		mrc_resume();
#else
				runmrps("xqc/editor.mrp");
#endif
#else
		mrc_exit();
#endif
	}
}
int CheckBaseCall(char *FuncName,struct ValueType *ReturnType,int NumParams)
{
	struct Value *FuncValue = NULL;

	if(FuncName==NULL) return -1;
	if (!VariableDefined(TableStrRegister(FuncName)))
		ProgramFail(NULL, "%s()未定义",FuncName);

	VariableGet(NULL, TableStrRegister(FuncName), &FuncValue);
	if (FuncValue->Typ->Base != TypeFunction)
		ProgramFail(NULL, "无法使用%s",FuncName);

	if (FuncValue->Val->FuncDef.ReturnType == ReturnType)
	{
		if (FuncValue->Val->FuncDef.NumParams != NumParams) return -1;
	}else return -1;

	return 0;
}
char *mrp_cfile,*mrp_mrpname;
char **mrpfj;
int PicocCallInit(void)
{
	if(CheckBaseCall("event",&IntType,3)) return -1;
	if(CheckBaseCall("init",&IntType,0)) return -1;
	if(CheckBaseCall("pause",&IntType,0)) return -1;
	if(CheckBaseCall("resume",&IntType,0)) return -1;

	VariableDefinePlatformVar(NULL, "_code", &IntType, (union AnyValue *)&_code, FALSE);
	VariableDefinePlatformVar(NULL, "_p0", &IntType, (union AnyValue *)&_p0, FALSE);
	VariableDefinePlatformVar(NULL, "_p1", &IntType, (union AnyValue *)&_p1, FALSE);
	VariableDefinePlatformVar(NULL, "_ret", &IntType, (union AnyValue *)&FuncRet, TRUE);


	return 0;
}

char outputchar[64],outdirchar[64];
void Merger(char *a1,char *a2,char*a3)
{
	if(a1==NULL)
		if(a2==NULL)
			return;
	else
		strcpy(a3,a2);
	else
	{
		strcpy(a3,a1);
		strcat(a3,"/");
		strcat(a3,a2);
	}
}
void PicocRun(int32 data)
{

	if(!data)
	{
		_SET_BIT(suc,PICINITSUC);//启动运行
		HEAP_SIZE=128*1024;
		if(xqc_setinis()==-1)
		{
			TextScreen("请用editor.mrp运行！！！",0);
			_SET_BIT(suc,EXITRUMMODE);
			return;
		}
#ifdef includefile
		if(!PACKGEDMRP)
			TextScreen("启动运行",0);
#endif
		data=mrc_timerCreate();
		mrc_timerStart(data,1,data,PicocRun,0);
	}
	else
	{
		INI *pini;
		char *name;
		mrc_timerDelete(data);

		_SET_BIT(suc,PICINITSUC);
#ifdef includefile
		name="xqc/temp/temp.txt";
		_code=(int32)"c/temp/";
		_p0=(int32)includechar;
		_p1=(int32)"pack.mrp";
		if(strlen((char*)tempfile_path)>4&&(!memcmp(((char*)tempfile_path)+strlen((char*)tempfile_path)-4,".mpr",4)))
		{
			char *a,temp[128],dir[64];

			pini = ini_open(&tempfile_path[0]);
			if (!pini) {
				return;
			}
			a=mrc_strrchr(&tempfile_path[0],'/');
			if(a)
			{
				a[0]=0;
				strcpy(&dir[0],&tempfile_path[0]);
			}
			else
				dir[0]=0;	
			mpr_tou(pini);
			ini_get(pini,"information@outmain",(char*)temp);
			Merger(&dir[0],&temp[0],&tempfile_path[0]);
			ini_get(pini,"information@outdir",temp);
			Merger(&dir[0],&temp[0],&outdirchar[0]);
			_code=(int32)outdirchar;
			ini_get(pini,"information@output",temp);
			Merger(&dir[0],&temp[0],&outputchar[0]);
			_p1=(int32)outputchar;
			ini_close(pini);
			PACKGEDMRP=1;
			name=(char*)tempfile_path;
		}
#else
		name=(char*)tempfile_path;
#endif

		PicocInitialise(HEAP_SIZE);

		if (PicocPlatformSetExitPoint())
		{
			SafeExit(0);
			return;
		}
		PicocPlatformScanFile((const char*)name);

		if(PicocCallInit())
			SafeExit(0);
		else
		{
			char *zhi="_ret=init();";
#ifdef includefile
			if(PACKGEDMRP)
			{
				freeFunc();

				TextScreen("启动打包程序",0);

				includecharnode((char*)tempfile_path);
				includecharnode("xqc/runner/start.mr"); 
				includecharnode("xqc/runner/cfunction.ext");
				setupfile("xqc/temp/setup.ini");
				includecharnode("xqc/temp/setup.ini");
	
				_p0=(int32)includechar;
				IncludeFile("packged.h");

				
				zhi="_ret=PackgedMrp((char*)_code,(char**)_p0,(char*)_p1);printf(\"\r\");exit();";
				//_SET_BIT(suc,PLATRUMMODE);

			}
#endif
			PicocParse(" ", zhi, strlen(zhi), TRUE, TRUE, FALSE);
		}
	}
}




/*
应用初始化函数
该函数在应用初始化期间被mythroad平台调用，可以在这个
函数中进行全局变量的初始化等工作。
返回值:
MR_SUCCESS  应用初始化成功
MR_FAILED      应用初始化失败
*/
int32 xqc_init_main(void)
{
	mr_screeninfo screen;
	mrc_getScreenInfo(&screen);
	SCREEN_WIDTH = (uint16)(screen.width);
	SCREEN_HEIGHT = (uint16)(screen.height);
	if(!SCREEN_WIDTH||!SCREEN_HEIGHT)
	{
		SCREEN_WIDTH=240;
		SCREEN_HEIGHT=320;
	}
	mrc_mkDir("xqc/temp/");
	StackInit();
	mrc_remove("print.txt");
	Debugts=1;
	PicocRun(0);
	return MR_SUCCESS;
}
#ifdef LIANHE
int32 xqc_init(void)
{
	return xqc_init_main();
}
#else
int32 mrc_init(void)
{
	return xqc_init_main();
}
#endif



/*
应用事件函数
该函数在应用运行期间，每当mythroad平台收到事件时
被调用
输入:
code:
      code的可能取值如下:
enum {
   MR_KEY_PRESS,           //按键按下事件
   MR_KEY_RELEASE,        //按键释放事件
   MR_MOUSE_DOWN,       //触摸屏（鼠标）按下事件
   MR_MOUSE_UP,            //触摸屏（鼠标）抬起/释放事件
   MR_MENU_SELECT,       //菜单选中事件
   MR_MENU_RETURN,       //菜单返回事件
   MR_DIALOG_EVENT,      // 对话框/编辑框/文本框事件
   MR_EVENT02,             //VM保留，请不要使用
   MR_EXIT_EVENT,           //应用退出事件
   MR_EVENT03,                 //VM保留，请不要使用
   MR_LOCALUI_EVENT         //本地化接口事件
};

param0:
   当code为MR_KEY_PRESS或MR_KEY_RELEASE时，param0的可能取值如下:
enum {
   MR_KEY_0,               //按键 0
   MR_KEY_1,               //按键 1
   MR_KEY_2,               //按键 2
   MR_KEY_3,               //按键 3
   MR_KEY_4,               //按键 4
   MR_KEY_5,               //按键 5
   MR_KEY_6,               //按键 6
   MR_KEY_7,               //按键 7
   MR_KEY_8,               //按键 8
   MR_KEY_9,               //按键 9
   MR_KEY_STAR,            //按键 *
   MR_KEY_POUND,           //按键 #
   MR_KEY_UP,              //按键 上
   MR_KEY_DOWN,            //按键 下
   MR_KEY_LEFT,            //按键 左
   MR_KEY_RIGHT,           //按键 右
   MR_KEY_POWER,           //按键 挂机键
   MR_KEY_SOFTLEFT,        //按键 左软键
   MR_KEY_SOFTRIGHT,       //按键 右软键
   MR_KEY_SEND,            //按键 接听键
   MR_KEY_SELECT,          //按键 确认/选择（若方向键中间有确认键，建议设为该键）
   MR_KEY_VOLUME_UP,          //按键 侧键上
   MR_KEY_VOLUME_DOWN,          //按键 侧键下
   MR_KEY_NONE             //按键 保留
};
当code为MR_MOUSE_DOWN或MR_MOUSE_UP时，param0为屏幕的x坐标；
当code为MR_MENU_SELECT时，param0为菜单index；
当code为MR_DIALOG_EVENT时，param0的可能取值如下:
enum {
   MR_DIALOG_KEY_OK,         //对话框/文本框等的"确定"键被点击(选择)
   MR_DIALOG_KEY_CANCEL  //对话框/文本框等的"取消"("返回")键被点击(选择)
};
当code为MR_LOCALUI_EVENT时，param0的可能取值如下:
enum {
   MR_LOCALUI_KEY_OK,       //本地控件的"确定"键被点击(选择)
   MR_LOCALUI_KEY_CANCEL//本地控件的"取消"("返回")键被点击(选择)
};

param1:
当code为MR_MOUSE_DOWN或MR_MOUSE_UP时，param1为屏幕的y坐标；

返回值:
MR_SUCCESS  操作成功
MR_FAILED      操作失败
*/
#ifdef LIANHE
int32 xqc_appEvent(int32 code, int32 param0, int32 param1)
{
	if(_IS_SET_BIT(suc,PICINITSUC))
	{
		_code=code;
		_p0=param0;
		_p1=param1;


		if (PicocPlatformSetExitPoint())
		{
			SafeExit(0);
			return FuncRet;
		}
		PicocParse(" ", "_ret=event(_code,_p0,_p1);", strlen("_ret=event(_code,_p0,_p1);"), TRUE, TRUE, FALSE);
	}
	else
		SafeExit(0);
	return FuncRet;
	return MR_SUCCESS;
}
#else
int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	if(_IS_SET_BIT(suc,PICINITSUC))
	{
		_code=code;
		_p0=param0;
		_p1=param1;


		if (PicocPlatformSetExitPoint())
		{
			SafeExit(0);
			return FuncRet;
		}
		PicocParse(" ", "_ret=event(_code,_p0,_p1);", strlen("_ret=event(_code,_p0,_p1);"), TRUE, TRUE, FALSE);
	}
	else
		SafeExit(0);
	return FuncRet;
	return MR_SUCCESS;
}
#endif
/*
应用暂停函数
该函数在应用被暂停时（有电话等事件暂停应用）
被mythroad平台调用。
返回值:
MR_SUCCESS  操作成功
MR_FAILED      操作失败
*/
#ifdef LIANHE

int32 xqc_pause(void)
{
	if(_IS_SET_BIT(suc,PICINITSUC))
	{
		if (PicocPlatformSetExitPoint())
		{
			SafeExit(0);
			return FuncRet;
		}
		PicocParse(" ", "_ret=pause();", strlen("_ret=pause();"), TRUE, TRUE, FALSE);
	}
	return FuncRet;
	return 0;
}
#else

int32 mrc_pause(void)
{
	if(_IS_SET_BIT(suc,PICINITSUC))
	{
		if (PicocPlatformSetExitPoint())
		{
			SafeExit(0);
			return FuncRet;
		}
		PicocParse(" ", "_ret=pause();", strlen("_ret=pause();"), TRUE, TRUE, FALSE);
	}
	return FuncRet;
	return 0;
}
#endif
/*
该函数在应用恢复运行时被mythroad平台调用。
返回值:
MR_SUCCESS  操作成功
MR_FAILED      操作失败
*/
#ifdef LIANHE
int32 xqc_resume(void)
{
	if(_IS_SET_BIT(suc,PICINITSUC))
	{
		if (PicocPlatformSetExitPoint())
		{
			SafeExit(0);
			return FuncRet;
		}
		PicocParse(" ", "_ret=resume();", strlen("_ret=resume();"), TRUE, TRUE, FALSE);
	}
	return FuncRet;
	return 0;
}
#else
int32 mrc_resume(void)
{
	if(_IS_SET_BIT(suc,PICINITSUC))
	{
		if (PicocPlatformSetExitPoint())
		{
			SafeExit(0);
			return FuncRet;
		}
		PicocParse(" ", "_ret=resume();", strlen("_ret=resume();"), TRUE, TRUE, FALSE);
	}
	return FuncRet;
	return 0;
}
#endif


/*
该函数在应用退出时被mythroad平台调用。
返回值:
MR_SUCCESS  操作成功
MR_FAILED      操作失败
*/
#ifdef LIANHE
int32 xqc_exitApp(void)
{
	return 0;
}
#else
int32 mrc_exitApp(void)
{
	return 0;
}
#endif


int32 ext_RecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5)
{
	switch(code)
	{
	case EXT_FUNCRET:
		return (int32)&FuncRet;
		break;
	case EXT_INTTYPE:
		return (int32)&IntType;
		break;
#ifndef NO_FP
	case EXT_FPTYPE:
		return (int32)&FPType;
		break;
#endif
	case EXT_CHECKBASECALL:
		return CheckBaseCall((char *)p0,(struct ValueType *)p1,(int) p2);
		break;
	case EXT_PROGRMFAIL:
		ProgramFail((struct ParseState *)p0, (const char *)p1, (const char *)p2);
			break;
	case EXT_PICOCPARSE:
		PicocParse((const char *)p0, (const char *)p1, (int) p2, (int) p3, (int) p4, FALSE);
		break;
	case EXT_VARIABLE:
		VariableDefinePlatformVar((struct ParseState *)p0, (char *)p1, (struct ValueType *) p2, (union AnyValue *)p3, (int)p4);
		break;
	}
	return 0;
}
