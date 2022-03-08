#include "mrc_base.h"
#include "mrc_base_i.h"
#include "mrc_exb.h"
#include "Editor.h"
#include "editormain.h"
#include "mrc_bmp.h"
#include "libini.h"
#include "logo.h"
#include "font.h"


enum Windows MainWindow=EDITOR;

////////////////////ȫ��//////////////
int32 EditEditHand=0,//�༭����
EditFunction=0;
int PACKGEDMRP=0;
int SET_HEAP_SIZE,SET_DEBUG;




//////////////////////////////////////


int SCRW=240,SCRH=320;
//�����ļ�
int memfile(const char* dst,const char* src)
{
	int32 dstf,srcf,len,t;
	uint8 *buf;

	if(!mrc_strcmp(dst,src)) goto err;
	t=mrc_getMemoryRemain()/2;
	buf = mrc_malloc(t);
	if(!buf) goto err;
	mrc_remove(dst);
	dstf = mrc_open(dst,MR_FILE_CREATE|MR_FILE_RDWR);
	if(!dstf) 
	{
		free(buf);
		goto err;
	}
	len = mrc_getLen(src);
	srcf = mrc_open(src,MR_FILE_RDONLY);
	if(!srcf || !len)
	{
		free(buf);
		mrc_close(dstf);
		goto err;
	}
	while(len)
	{
		if(len > t)
		{
			len -= t;
		}
		else
		{
			t = len;
			len ^= len;
		}
		mrc_read(srcf,buf,t);
		mrc_write(dstf,buf,t);
	}
	free(buf);
	mrc_close(dstf);
	mrc_close(srcf);

	return 0;
err:
	return -1;
}

//�����ļ���
int32 memdir(const char* dst,const char* dirName)
{
	char fullName[128];
	char fileName[64];
	char fuledst[128];
	int32 ret = -1;
	int32 dirNameLen;
	int32 dirdstLen;

	if(!dirName)
		return MR_FAILED;    
	memset(fullName, 0, sizeof(fullName));
	memset(fileName, 0, sizeof(fileName));
	memset(fuledst, 0, sizeof(fuledst));

	dirNameLen = strlen(dirName);
	dirdstLen = strlen(dst);
	memcpy(fullName, dirName, dirNameLen);
	memcpy(fuledst, dst, dirdstLen);
	if(fullName[dirNameLen-1] != '/')//׼��ƴ��Ŀ¼�µ����ļ�����Ŀ¼
	{
		fullName[dirNameLen] = '/';
		dirNameLen++;
	}
	if(fuledst[dirdstLen-1] != '/')//׼��ƴ��Ŀ¼�µ����ļ�����Ŀ¼
	{
		fuledst[dirdstLen] = '/';
		dirdstLen++;
	}
	ret = mrc_mkDir(dst);
	if(mrc_fileState(dirName) == MR_IS_DIR)
	{
		int32 fd;
		int32 result = MR_FAILED;

		fd = mrc_findStart(dirName, fileName, sizeof(fileName));
		if(fd > 0)//���������Ч
		{
			result = MR_SUCCESS;
			while(result == MR_SUCCESS)
			{
				//���fileNameȡֵΪ��.����ʶ������ֵ��Ч����������δ���
				if(fileName[0] && fileName[0] != '.')
				{
					int32 length = strlen(fileName);
					memcpy(&fullName[dirNameLen], fileName, length);
					fullName[dirNameLen+length] = 0;
					memcpy(&fuledst[dirdstLen], fileName, length);
					fuledst[dirdstLen+length] = 0;

					length = mrc_fileState(fullName);
					if(length == MR_IS_FILE)//�ļ����ͣ�ɾ���ļ�
						memfile(fuledst,fullName);
					else if(length == MR_IS_DIR)//�ļ������ͣ��ݹ�ɾ�����ļ���
						memdir(fuledst,fullName);
				}
				memset(fileName, 0, sizeof(fileName));
				result = mrc_findGetNext(fd, fileName, sizeof(fileName));
			}
			mrc_findStop(fd);
		}
		//����һ�μ�����,�������ɾ���ļ��е�����
		fd = mrc_findStart("",fileName,sizeof(fileName));
		mrc_findStop(fd);
	}
	return ret;
}






///////////////////����/////////////////////
char *Str_Clean=NULL,*Str_Menu=NULL;

void EditFile(char *path,char *name)
{
	mrc_strcpy(WORDOBG->WorkFile,path);
	mrc_strcat(WORDOBG->WorkFile,"/");
	mrc_strcat(WORDOBG->WorkFile,name);
}
int SaveFile()
{
	if(WORDOBG->WorkFile[0])
		return  EditSaveToFile(WORDOBG->WorkFile,1);
	return -1;
}
int fileaz(char *mrpfile,char *file)
{
	int32 hand=0,len=0;
	void *data=NULL;
	char *name=NULL;
	if((name=mrc_strrchr(mrpfile,'/'))!=NULL)
		mrpfile=name+1;
	data=mrc_readFileFromMrp(mrpfile,&len,0);
	if(data!=NULL)
	{
		hand=mrc_open(file,2|8);
		if(hand>0)
		{
			mrc_write(hand,data,len);
			mrc_close(hand);
		}
		mrc_freeFileData(data,len);
		return 0;
	}
	else
		return -1;
	return 0;
}

int az()
{
	if(mrc_fileState("c")!=MR_IS_DIR)
	{
		mrc_mkDir("c");
	}
	if(mrc_fileState("xqc/temp")!=MR_IS_DIR)
	{
		mrc_mkDir("xqc/temp");
	}
	return 0;
}
void SetInit()
{
	SET_HEAP_SIZE=128;
	SET_DEBUG=1;
}

int IniSetSave()
{
	INI *p;
	mrc_remove("xqc/setting.ini");
	p=ini_create("xqc/setting.ini");
	if(!p)
		return -1;
	ini_append_int(p,"SET@FONT",FONT);
	SET_HEAP_SIZE=SET_HEAP_SIZE<64?64:SET_HEAP_SIZE;
	ini_append_int(p,"SET@HEAP_SIZE",SET_HEAP_SIZE);
	ini_append_int(p,"SET@SCRW",0);
	ini_append_int(p,"SET@SCRH",0);
	ini_append_int(p,"SET@DEBUG",SET_DEBUG);
	ini_save(p);
	ini_close(p);
	return 0;
}

int IniSetInit()
{
	INI *p;
	p=ini_open("xqc/setting.ini");
	if(!p)
		return -1;
	SET_HEAP_SIZE=ini_get_int(p,"SET@HEAP_SIZE");
	if(SET_HEAP_SIZE<64)
		SET_HEAP_SIZE=64;
	FONT=ini_get_int(p,"SET@FONT");
	ini_close(p);
	return 0;
}
//��¼�༭����Ϣ���������������Ϣ
int IniTempSave()
{
	INI *p;
	mrc_remove("xqc/temp/exchange.ini");
	p=ini_create("xqc/temp/exchange.ini");
	if(!p)
		return -1;
	ini_append(p,"exchange@MAINNAME",WORDOBG->WorkFile);
	ini_append_int(p,"exchange@HEAP_SIZE",SET_HEAP_SIZE);
	ini_append_int(p,"exchange@SCRW",0);
	ini_append_int(p,"exchange@SCRH",0);
	ini_append_int(p,"exchange@DEBUG",1);
	ini_append_int(p,"exchange@PACKGEDMRP",PACKGEDMRP);
	ini_append_int(p,"exchange@LING",GetEditGetTwoNodeLen());
	ini_save(p);
	ini_close(p);
	return 0;
}

int IniTempInit()
{
	INI *p;
	p=ini_open("xqc/temp/exchange.ini");
	if(!p)
		return -1;
	if(WORDOBG)
	{
		ini_get(p,"exchange@MAINNAME",WORDOBG->WorkFile);
		GotoLine(ini_get_int(p,"exchange@LING"));
	}
	ini_close(p);
	return 0;
}
int32 MyCreate(char *title,char *text,int type,int len)
{
	int32 hand=0,size=0;
	char *utitle=NULL,*utext=NULL;
	utitle=(char*)mrc_c2u(title,NULL,&size);
	utext=(char*)mrc_c2u(text,NULL,&size);
	hand=EditEditHand=mrc_editCreate(utitle,utext,type,len);
	mrc_free(utitle);
	mrc_free(utext);
	return hand;
}

void editorinit()
{
	SET_HEAP_SIZE=200;
	EditorInit();
	SetInit();
	if(IniSetInit())
		IniSetSave();
	openfont(FONT);//�ֿ�
	skin_rw_color(0);//Ƥ��

	az();//�ļ���
	logo_init();

	if(!EditLoadFile("xqc/temp/temp.txt",ED_FILE_RDWR))
		WORDOBG->WorkFile[0]=0;
	IniTempInit();
}

int Packdir(char *dir)
{
	char mrp[256];
	if(dir==NULL)
		return -1;
	mrp[0]=0;
	strcmp(mrp,dir);
	strcat(mrp,".mrp");
	return PackgedDirMrp(dir,mrp);

}


extern uint32   mrc_rand(void);
//����mrp
int32 runmrps(char *name)
{
	char *qd;
	if(mrc_fileState("xqc/mtk.txt")==MR_IS_FILE)
		qd="start.mr";
	else
		qd="cfunction.ext";
	return mrc_runMrp(name,qd,NULL);
}
//����������
void startdebug(int32 data)
{
	if(!data)
	{
		data=mrc_timerCreate();
		mrc_timerStart(data,1,data,startdebug,0);
	}
	else
	{

		mrc_timerDelete(data);
		IniTempSave();

#ifdef SIM_MOD

		//xqc_init();
		tcc_init();
#else
		
		runmrps("xqc/runner/ruuner.mrp");
		//runmrps("xqc/debug.mrp");
#endif
	}
}
//����ǰ��ʼ������������������
void Debugging()
{
	EditSaveToFile("xqc/temp/temp.txt",0);
	startdebug(0);
}

//��ʽ����
enum{
	g_c,
	g_txt,
	g_ini,
	g_vcprog,
	g_end
};
//�������ͷ��غ��
char *reg_ge(int ge)
{
	switch(ge)
	{
	case g_c:return "c";
	case g_txt:return "txt";
	case g_ini:return "ini";
	case g_vcprog:return "vcprog";
	}
	return NULL;
}
//���ظ�ʽ����
int bihou(char *name)
{
	int i;
	name=mrc_strrchr((const char *)name,(int32)'.');
	if(name==NULL)
		return -1;
	name++;
	for(i=0;i<g_end;i++)
		if(!strcmp(name,reg_ge(i)))
			return i;
	return -1;
}
