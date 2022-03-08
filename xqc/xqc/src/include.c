#include "picoc.h"
#include "interpreter.h"
#include "mrc_base_i.h "
#include "mrc_bmp.h "
#include "..\syslib\haning.h"

extern int32 Base_SetupFunc(void);
extern int32 mr_Base_SetupFunc(void);
extern int32 ctype_init(void);
extern int32 sound_init(void);
extern int32 net_init(void);
#ifndef NO_FP
extern int32 math_init(void);
#ifndef NO_3D
extern int32 sd_init(void);
#endif
#endif
extern int32 base_i_init(void);
extern int32 graphics_init(void);
#ifdef includefile
extern int32 Packged_init(void);
#endif

#define mallocchar(text) memcpy(malloc(strlen(text)+1),text,strlen(text)+1)

/* ���ǿ��԰���һ�����б� */
struct IncludeLibrary
{
    char *IncludeName;
    int32 (*SetupFunction)(void);
    struct LibraryFunction *FuncList;
    const char *SetupCSource;
    struct IncludeLibrary *NextLib;
};
#ifdef includefile
extern int PACKGEDMRP;
#endif
struct IncludeLibrary *IncludeLibList = NULL;

#define  m_lge 10
int m_i,includechar_i;
struct ExtState extst[m_lge];
char *includechar[50];

int32 initext(struct ExtState *a,char *name)
{
	int32 *scr;
#ifdef includefile
	char temp[128];
#endif

#ifdef SIM_MOD
	scr=(int32*)w_getScreenBuffer();
	scr[0]=(int32)ext_RecvAppEventEx;
	if(!strcmp(name,"math.ext"))
		return math_init();

	if(!strcmp(name,"base_i.ext"))
		return base_i_init();

	if(!strcmp(name,"acmera.ext"))
		return acmera_init();

	if(!strcmp(name,"sound.ext"))
		return sound_init();
	if(!strcmp(name,"graphics.ext"))
		return graphics_init();
	if(!strcmp(name,"packged.ext"))
		return Packged_init();

#else
	a->name=name;
#ifdef includefile
	sprintf(temp,"xqc/include/%s",name);
	a->p=mrc_readAll(temp,&a->len);

#else
	a->p=mrc_readFileFromMrp(name,(int32 *)&a->len,0);
#endif
	a->ID=mrc_extLoad(a->p,a->len);
	if(a->ID)
	{
		int re;
		scr=(int32*)w_getScreenBuffer();
		scr[0]=(int32)ext_RecvAppEventEx;
		re=mrc_extInit(a->ID);
		mrc_extEvent(a->ID,0,(int32)ext_RecvAppEventEx,0);
		//*scr=(int32)(int32*)ext_RecvAppEventEx;
		return re;
	}
	else
		return (int32)NULL;
#endif
return 0;
}
void freeFunc()
{
	int i=0;
	while(i<m_lge)
	{
		if(extst[i].ID)
		{
			mrc_extFree(extst[i].ID);
#ifdef includefile
			free(extst[i].p);
#else
			mrc_freeFileData(extst[i].p,extst[i].len);
#endif
			extst[i].ID=0;
		}
		i++;
	}
}
void includecharnode(char *file)
{
#ifdef includefile
	if(PACKGEDMRP)
		includechar[includechar_i++]=mallocchar(file);
#endif
}
int32 SetupFunc(char *file)
{
#ifdef includefile
	char temp[128];int32 re;
	sprintf(temp,"xqc/include/%s",file);
	re=mrc_fileState(temp);
	if(PACKGEDMRP&&re==1&&file!=mrc_strstr(file,"packged.ext"))
		includecharnode(temp);
	return re==1?0:-1;
#else
	uint8 *a=NULL;
	return mrc_readFileFromMrpEx((char *)mrc_getPackName(),(const char*)file,(uint8 **)&a,(int32 *)NULL,1);
#endif
}


/* ��ʼ�����õİ����� */
void IncludeInit(void)
{
	int i=0;
	for(i=includechar_i=0;i<50;includechar[i++]=NULL);
	for(i=0;i<m_lge;extst[i++].ID=0);
	m_i=0;
	IncludeRegister("base.h", &Base_SetupFunc);
	IncludeRegister("mrc_base.h", &mr_Base_SetupFunc);
	IncludeRegister("ctype.h", &ctype_init);
	IncludeRegister("sound.h", &sound_init);
	IncludeRegister("net.h", &net_init);
#ifndef NO_FP
	IncludeRegister("math.h", &math_init);
#ifndef NO_3D
	IncludeRegister("3d.h", &sd_init);
#endif
#endif
	IncludeRegister("base_i.h", &base_i_init);
	IncludeRegister("graphics.h", &graphics_init);
#ifdef includefile
	IncludeRegister("packged.h", &Packged_init);
#endif
}

/* �������ϵͳͷ�ļ���ʹ�õĿռ� */
void IncludeCleanup(void)
{
    struct IncludeLibrary *ThisInclude = IncludeLibList;
    struct IncludeLibrary *NextInclude;
    
    while (ThisInclude != NULL)
    {
        NextInclude = ThisInclude->NextLib;
        HeapFreeMem(ThisInclude);
        ThisInclude = NextInclude;
    }

    IncludeLibList = NULL;
}

/* ע��һ���µ����ð����ļ� */
void IncludeRegister(const char *IncludeName, int32 (*SetupFunction)(void))
{
    struct IncludeLibrary *NewLib = HeapAllocMem(sizeof(struct IncludeLibrary));
    NewLib->IncludeName = TableStrRegister(IncludeName);
    NewLib->SetupFunction = SetupFunction;
    NewLib->NextLib = IncludeLibList;
    IncludeLibList = NewLib;
}

/* �������е�ϵͳͷ�ļ� */
void PicocIncludeAllSystemHeaders(void)
{
    struct IncludeLibrary *ThisInclude = IncludeLibList;
    
    for (; ThisInclude != NULL; ThisInclude = ThisInclude->NextLib)
        IncludeFile(ThisInclude->IncludeName);
}

/* ����һЩԤ����Ŀ⣬����ʵ�ʵ��ļ� */
void IncludeFile(char *FileName)
{
	struct IncludeLibrary *LInclude;
	int32 len=0;char temp[128],*name=FileName;
	struct HJG *p;
	memset(temp,0,128);
	/* ɨ��������ļ����������Ƿ������ǵ�Ԥ����İ����б��� */
	for (LInclude = IncludeLibList; LInclude != NULL; LInclude = LInclude->NextLib)
	{
		if (strcmp(LInclude->IncludeName, FileName) == 0)
		{
			/* ���� - ��ֹ�ظ����� */
			if (!VariableDefined(FileName))
			{

				VariableDefine(NULL, FileName, NULL, &VoidType, FALSE);

				/* ���ж�������������������һ�� */
				if (LInclude->SetupFunction != NULL)
					p=(void *)(*LInclude->SetupFunction)();
				LInclude->FuncList=p->Functions;
				LInclude->SetupCSource=p->Defs;
				free(p);      

				/* �������õ�CԴ���� - ���Զ������͵�. */
				if (LInclude->SetupCSource != NULL)
				{
					//LInclude->SetupCSource=filenei(FileName,&len);

					PicocParse(FileName, LInclude->SetupCSource, strlen(LInclude->SetupCSource), TRUE, TRUE, FALSE);
				}
				/* ���ú����� */
				if (LInclude->FuncList != NULL)
					LibraryAdd(&GlobalTable, FileName, LInclude->FuncList);
			}

			return;
		}
	}

	name=strstr(FileName,".");
	if(name!=NULL)
	{
		MR_HAN *han;
		name=memcpy(temp,FileName,strlen(FileName)-strlen(name));
		strcat(temp,".ext");
		
		/* ���� - ��ֹ�ظ����� */
		if (!VariableDefined(FileName))
		{
			if(SetupFunc(temp)==MR_SUCCESS)
			{
				VariableDefine(NULL, FileName, NULL, &VoidType, FALSE);

				p=(void *)initext(&extst[m_i++],temp);
				if(p==NULL)
					return;

				/* �������õ�CԴ���� - ���Զ������͵�. */
				if (p->Defs != NULL)
					PicocParse(FileName, p->Defs, strlen(p->Defs), TRUE, TRUE, FALSE);

				/* ���ú����� */
				if (p->Functions != NULL)
					LibraryAdd(&GlobalTable, FileName, p->Functions);
				//if(p->zhuan==NULL)
				han=*p->zhuan;
					*han=(MR_HAN)(ext_RecvAppEventEx);
				free(p);      
				return;
			}

		}
	}
	/* ����Ԥ������ļ�����һ���������ļ� */
	if(PicocPlatformScanFile(FileName))
		includecharnode(FileName);
}

