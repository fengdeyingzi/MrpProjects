#include "mrc_base.h"

#include "xqc.h"
#include "picoc.h"
#include "interpreter.h"
#include "platform.h"
#include "mrc_exb.h "
#include "momo.h "
#include "ext.h "
#include "libini.h"

#define PICINITSUC 0x01   //������
#define PLATRUMMODE 0x02  //�༭��
#define EXITRUMMODE 0x04  //������˳�
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
int HEAP_SIZE;         /* �ѺͶ�ջ�ռ� */
int Debugts;
#ifdef includefile
int PACKGEDMRP;
extern char *includechar[20];
void mpr_tou(INI *pini);
#endif
/*�������*/
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
			TextScreen("��������",0);
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
		ProgramFail(NULL, "%s()δ����",FuncName);

	VariableGet(NULL, TableStrRegister(FuncName), &FuncValue);
	if (FuncValue->Typ->Base != TypeFunction)
		ProgramFail(NULL, "�޷�ʹ��%s",FuncName);

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
		_SET_BIT(suc,PICINITSUC);//��������
		HEAP_SIZE=128*1024;
		if(xqc_setinis()==-1)
		{
			TextScreen("����editor.mrp���У�����",0);
			_SET_BIT(suc,EXITRUMMODE);
			return;
		}
#ifdef includefile
		if(!PACKGEDMRP)
			TextScreen("��������",0);
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

				TextScreen("�����������",0);

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
Ӧ�ó�ʼ������
�ú�����Ӧ�ó�ʼ���ڼ䱻mythroadƽ̨���ã����������
�����н���ȫ�ֱ����ĳ�ʼ���ȹ�����
����ֵ:
MR_SUCCESS  Ӧ�ó�ʼ���ɹ�
MR_FAILED      Ӧ�ó�ʼ��ʧ��
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
Ӧ���¼�����
�ú�����Ӧ�������ڼ䣬ÿ��mythroadƽ̨�յ��¼�ʱ
������
����:
code:
      code�Ŀ���ȡֵ����:
enum {
   MR_KEY_PRESS,           //���������¼�
   MR_KEY_RELEASE,        //�����ͷ��¼�
   MR_MOUSE_DOWN,       //����������꣩�����¼�
   MR_MOUSE_UP,            //����������̧꣩��/�ͷ��¼�
   MR_MENU_SELECT,       //�˵�ѡ���¼�
   MR_MENU_RETURN,       //�˵������¼�
   MR_DIALOG_EVENT,      // �Ի���/�༭��/�ı����¼�
   MR_EVENT02,             //VM�������벻Ҫʹ��
   MR_EXIT_EVENT,           //Ӧ���˳��¼�
   MR_EVENT03,                 //VM�������벻Ҫʹ��
   MR_LOCALUI_EVENT         //���ػ��ӿ��¼�
};

param0:
   ��codeΪMR_KEY_PRESS��MR_KEY_RELEASEʱ��param0�Ŀ���ȡֵ����:
enum {
   MR_KEY_0,               //���� 0
   MR_KEY_1,               //���� 1
   MR_KEY_2,               //���� 2
   MR_KEY_3,               //���� 3
   MR_KEY_4,               //���� 4
   MR_KEY_5,               //���� 5
   MR_KEY_6,               //���� 6
   MR_KEY_7,               //���� 7
   MR_KEY_8,               //���� 8
   MR_KEY_9,               //���� 9
   MR_KEY_STAR,            //���� *
   MR_KEY_POUND,           //���� #
   MR_KEY_UP,              //���� ��
   MR_KEY_DOWN,            //���� ��
   MR_KEY_LEFT,            //���� ��
   MR_KEY_RIGHT,           //���� ��
   MR_KEY_POWER,           //���� �һ���
   MR_KEY_SOFTLEFT,        //���� �����
   MR_KEY_SOFTRIGHT,       //���� �����
   MR_KEY_SEND,            //���� ������
   MR_KEY_SELECT,          //���� ȷ��/ѡ����������м���ȷ�ϼ���������Ϊ�ü���
   MR_KEY_VOLUME_UP,          //���� �����
   MR_KEY_VOLUME_DOWN,          //���� �����
   MR_KEY_NONE             //���� ����
};
��codeΪMR_MOUSE_DOWN��MR_MOUSE_UPʱ��param0Ϊ��Ļ��x���ꣻ
��codeΪMR_MENU_SELECTʱ��param0Ϊ�˵�index��
��codeΪMR_DIALOG_EVENTʱ��param0�Ŀ���ȡֵ����:
enum {
   MR_DIALOG_KEY_OK,         //�Ի���/�ı���ȵ�"ȷ��"�������(ѡ��)
   MR_DIALOG_KEY_CANCEL  //�Ի���/�ı���ȵ�"ȡ��"("����")�������(ѡ��)
};
��codeΪMR_LOCALUI_EVENTʱ��param0�Ŀ���ȡֵ����:
enum {
   MR_LOCALUI_KEY_OK,       //���ؿؼ���"ȷ��"�������(ѡ��)
   MR_LOCALUI_KEY_CANCEL//���ؿؼ���"ȡ��"("����")�������(ѡ��)
};

param1:
��codeΪMR_MOUSE_DOWN��MR_MOUSE_UPʱ��param1Ϊ��Ļ��y���ꣻ

����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
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
Ӧ����ͣ����
�ú�����Ӧ�ñ���ͣʱ���е绰���¼���ͣӦ�ã�
��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
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
�ú�����Ӧ�ûָ�����ʱ��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
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
�ú�����Ӧ���˳�ʱ��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
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
