#include <mrc_base.h>

#include "tcc.h"
#include "l_lib.h"
#include "packged.h"
#include "libini.h"
//#pragma comment(lib, "libtcc.lib")

/*---------------------------------------------------------------------------*/
#define  XQC_CODE_O "xqc/temp/code.o"
#define  XQC_ERR_FILE "tccout.txt"

TCCState *mainsf;//�������������ӿ�ָ��
void *outmemsf;//�����������ָ��
char *filename[100];//�ռ���Դ�ļ�
char *filenamec[100];//�ռ�C��ʽ�ļ�
char *config[10];//�����
int filename_i,filenamec_i;//��ǰ�ļ�ָ��λ��
char tempfilf_mpr[128];//������Ŀ����
int debug_mpr,//��������
	PACKGEDMRP_mpr;//�����������
uint16 SCREEN_WIDTH;
uint16 SCREEN_HEIGHT;
void *tcc_filenei(const char *name,uint32 *Len)
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

#define  mallocchar(str)  strcpy(malloc(strlen(str)+1),str)
void charszfz(char **p,char *str)
{
	int i=0;
	if(p==NULL)
		return;
	while(p[i++]);
	p[i-1]=mallocchar(str);
	p[i]=NULL;
}
//�ļ����ļ��ϲ�
void Mergerdf(char *a1,char *a2,char*a3)
{
	if(a1==NULL)
		if(a2==NULL)
			return;
		else
			strcpy(a3,a2);
	else
	{
		if(a1[0])
		{
			strcpy(a3,a1);
			strcat(a3,"/");
		}
		strcat(a3,a2);
	}
}
char *stamp(char *name)
{
	char *a;
	if(name==NULL)
		return NULL;
	a=mrc_strrchr(name,'.');
	if(a==NULL)
		return NULL;
	if(a+1==NULL)
		return NULL;
	return a+1;
}
//�жϺ��
int StringAmp(char *name,char *hc)
{
	char *temp;
	temp=stamp(name);
	if(temp==NULL)
		return -1;
	else
		return strcmp(temp,hc);
}
int32 (*eventsf)(int32 code, int32 param0, int32 param1);
//����Ԥ�����
void loadlib(TCCState *s)
{
	l_base_init(s);
	l_sound_init(s);
	l_bmp_init(s);
	l_exb_init(s);
	l_network_init(s);
	//l_jgraphics_init(s);
	l_image_init(s);
	l_camera_init(s);
	l_base_i_init(s);
	l_math_init(s);
	l_tcc_init(s);
}
//��ȡ����ı��ļ���һ���ڴ�����
char *randfiles(char **name)
{
	char *re,*s;
	int32 len=0,lens=0,f,i=0;
	while(name[i])
	{
		len=mrc_getLen(name[i++]);
		lens+=len+2;
	}
	s=re=mrc_malloc(lens);
	i=0;
	while(name[i])
	{
		len=mrc_getLen(name[i]);
		f=mrc_open(name[i],MR_FILE_RDONLY);
		mrc_read(f,s,len);
		mrc_close(f);
		s+=len;
		strcpy(s,"\n");
		s+=strlen("\n");
		i++;
	}
	s[0]=0;
	return re;
}
int filemrp;
typedef struct {
	void *f;
	uint32 seek;
	uint32 len;
	uint32 mode;
}FILEF;

//��
int32 xq_open(const char *name,uint32 mode)
{
	if(filemrp)
	{
		FILEF *f=malloc(sizeof(FILEF));
		f->mode=mode;
		f->f=tcc_filenei(name,&f->len);
		if(f->f)
			return (int32)f;
		else
		{
			free(f);
			return 0;
		}
	}
	else
		return mrc_open(name,mode);
}
//�ر�
int32 xq_close(int32 f)
{
	if(!f)
		return -1;
	if(filemrp)
	{
		FILEF *p=(FILEF*)f;
		free(p->f);
		free(p);
		return 0;
	}
	else
		return mrc_close(f);
}
//д��
int32 xq_write(int32 f,void *p,uint32 l)
{
	if(!f||l<=0||p==NULL)
		return 0;
	if(filemrp)
	{
		FILEF *p=(FILEF*)f;
		if(p->mode&MR_FILE_WRONLY||p->mode&MR_FILE_RDWR)
		{
			return 0;
		}
		else
			return -1;
	}
	else
		return mrc_write(f,p,l);
}
//��ȡ
int32 xq_read(int32 f,void *pp,uint32 l)
{
	if(!f||l<=0||pp==NULL)
		return 0;
	if(filemrp)
	{
		FILEF *p=(FILEF*)f;
		if(p->mode&MR_FILE_RDONLY||p->mode&MR_FILE_RDWR)
		{
			if(p->seek+l>p->len)
				l=p->len-p->seek;
			memcpy(pp,(char*)p->f+p->seek,l);
			p->seek+=l;
			return l;
		}
		else
			return -1;
	}
	else
		return mrc_read(f,pp,l);
}
//����ָ��λ��
int32 xq_seek(int32 f,int32 l,int mode)
{
	if(!f)
		return 0;
	if(filemrp)
	{
		FILEF *p=(FILEF*)f;
		switch(mode)
		{
		case MR_SEEK_SET:
			p->seek=l;
			break;
		case MR_SEEK_CUR:
			p->seek+=l;
			if(p->seek>p->len)
				p->seek=p->len;
			break;
		case MR_SEEK_END:
			p->seek=p->len;
			break;
		}
		return 0;
	}
	else
		return mrc_seek(f,l,mode);
}
//����ָ��λ��
int32 xq_tell(int32 f)
{
	if(!f)
		return 0;
	if(filemrp)
	{
		FILEF *p=(FILEF*)f;
		return p->seek;
	}
	else
		return mrc_tell(f);
}


//��ȡ�ļ����ڴ�
void *mallocfile(char *text)
{
	int32 f;uint32 len;
	len=mrc_getLen(text);
	if(len<=0)
		return NULL;

	f=mrc_open(text,12);
	if(f>0)
	{
		text=malloc(len+1);
		text[len+1]=0;
		mrc_seek(f,0,2);
		mrc_read(f,text,len);
		mrc_close(f);
	}
	else
		return NULL;
	return text;
}
void writefile(char *name,void *p,uint32 len)
{
	int32 f;
	mrc_remove(name);
	f=mrc_open(name,12);
	if(!f)
		return;
	mrc_write(f,p,len);
	mrc_close(f);
}


// ��¼��־
static void DLog(const char* str, int32 num)
{
    int32 fd;
    char numStr[16];

    fd = mrc_open("tcclog.txt", MR_FILE_RDWR | MR_FILE_CREATE);
    mrc_seek(fd, 0, MR_SEEK_END);
    mrc_write(fd, (void*)str, strlen(str));
    sprintf(numStr, "%d\r\n", num);
    mrc_write(fd, numStr, strlen(numStr));
    mrc_close(fd);
}
int prsi=0;
// ��ʾ����Ϣ
static void PrintScn(const char *str)
{
	prsi=prsi>15?0:prsi+1;
    mrc_drawRect(1, prsi*20,240,20,0, 0, 0);
    mrc_drawText((char*)str, 0, prsi*20, 255, 0, 0, 0, 1);
    mrc_refreshScreen(0,  prsi*20, 240, 20);
}

static int32 g_StartTime;

static void SetStartTime(void)
{
    g_StartTime = mrc_getUptime();
}

// �������ʱ��
static void LogUsedTime(void)
{
    int32 time1;

    time1 = mrc_getUptime();
    DLog("use time: ", time1 - g_StartTime);
    g_StartTime = mrc_getUptime();
}

// ����ڴ�ʣ��
void PrintSysMem(void)
{
    uint32 remain;

    remain = mrc_getMemoryRemain();
    DLog("mem left: ", remain);
}

// ���tcc������Ϣ
 void errfunc(void *opaque, const char *msg)
{
    int32 fd;
	PrintScn(msg);
    fd = mrc_open(XQC_ERR_FILE, MR_FILE_RDWR | MR_FILE_CREATE);
    if (0 != fd)
    {
        mrc_seek(fd, 0, MR_SEEK_END);
        mrc_write(fd, (void*)msg, strlen(msg));
        mrc_write(fd, "\r\n", 2);
        mrc_close(fd);
    }
}
 //ִ��init
typedef void (*GzCbs)(int32 TotalIn, int32 TotalOut, int32 Result);

char InfoBuf[256];
// ������ö��
enum GZ_RESULT_ENUM
{
	GZ_RESULT_PROCESSING = 0, // ��������У��ص����ص�ǰ�����С
	GZ_RESULT_END_OK,         // ��������
	GZ_RESULT_END_ERROR,      // �쳣����

	GZ_RESULT_MAX
};
static int32 FileSize;

static void gz_cb(int32 TotalIn, int32 TotalOut, int32 Result)
{
	switch (Result)
	{
	case GZ_RESULT_PROCESSING:
		{
			sprintf(InfoBuf, "����ѹ��...\nԭʼ��С: %d Byte\n�����С: %d Byte\n�����С: %d Byte", FileSize, TotalIn, TotalOut);
			errfunc(NULL,"gzѹ��");
			errfunc(NULL, InfoBuf);
		}
		break;
	case GZ_RESULT_END_OK:
		sprintf(InfoBuf, "ѹ����ɡ�\nԭʼ��С: %d Byte\n�����С: %d Byte\nѹ������: %d%%", TotalIn, TotalOut, TotalOut * 100 / TotalIn);
		errfunc(NULL,"gzѹ��");
		errfunc(NULL, InfoBuf);
		break;
	case GZ_RESULT_END_ERROR:
		errfunc(NULL,"gzѹ��,ѹ��ʧ�ܣ�\n�ļ���д����");
		break;
	}
}
int32 gzipfile(char *dir,char **file)
{
	TCCState *s;
    int ret;
	char *aaa;
	int i=0;
	int outsize;
	void *jiqima;
	int32 (*GzipFiles)(const char*, const char *, int32 , GzCbs );
	aaa=malloc(128);

//	GzipFiles Gzip;
	filemrp=0;
    // �½�һ��TCCʵ��
    mainsf=s = tcc_new();

    // ��ʹ�ñ�׼��
    s->nostdinc = 1;
    s->nostdlib = 1;

    // ������ڴ�
    s->output_type = TCC_OUTPUT_MEMORY;

    // ������Ϣ���
    tcc_set_error_func(s, NULL, errfunc);
	l_base_init(s);
	ret = tcc_add_file(s, "xqc/include/gzip.o");
	if(ret)
		return -1;

	// ���������������Ҫ���ڴ��С
	ret = tcc_relocate_ex(s, NULL);
	if (-1 == ret)
		return -2;

	// �����ڴ�
	outsize = ret;
	jiqima = malloc(outsize);
	if (NULL == jiqima)
		return -3;
	memset(jiqima, 0, outsize);

	// �ض�λ�����еķ���,��������ڴ���
	ret = tcc_relocate_ex(s, jiqima);
	if (-1 == ret)
	{
		free(jiqima);
		return -4;
	}
	GzipFiles=(int32 (*)(const char *, const char *, int32 , GzCbs ))tcc_get_symbol(s, "GzipFile");
	if(GzipFiles==NULL)
	{
		errfunc(NULL,"ִ��ѹ��ʱ����:δ�ҵ�����GzipFile");
		return -1;
	}
	while(file[i])
	{
		FileSize=mrc_getLen(file[i]);
		strcpy(aaa,dir);
		strcat(aaa,"/");
		strcat(aaa,strrchr(file[i],'/')==NULL?file[i]:strrchr(file[i],'/'));
		strcat(aaa,".gz");
		errfunc(NULL,aaa);
#ifdef SIM_MOD
		GzipFile(aaa,file[i],6,gz_cb);
#else
		GzipFiles(aaa,file[i],6,gz_cb);
#endif
		i++;
	}
	free(aaa);
	free(jiqima);
}
void maaini(int32 data)
{
	if(data)
	{
		int32 (*func)(void);
		mrc_timerDelete(data);
		func = (int32(*)(void))tcc_get_symbol(mainsf, "mrc_init");
		if (!func) 
			return;
#ifdef SIM_MOD
		errfunc(NULL,"^-^ִ�гɹ�^-^");
#else
		 func();
#endif
	}
	else
	{
		data=mrc_timerCreate();
		mrc_timerStart(data,1,data,maaini,0);
	}
}
// �������o�ļ�
int compile_o(char *work,char **dir,char **file,char *create,int library)
{
    int ret = 0;
    TCCState *s;
	char *temp;
	int i;
	if(dir==NULL)
		return -1;
	if(file==NULL)
		return -1;
	temp=malloc(128);

    // �½�һ��TCCʵ��
    s = tcc_new();

    // ��ʹ�ñ�׼��
    s->nostdinc = 1;
    s->nostdlib = 1;


    // ������Ϣ���
    tcc_set_error_func(s, NULL, errfunc);
	pack_set_print_func(errfunc);
    // ���*.o�ļ�
    s->output_type = TCC_OUTPUT_OBJ;
    s->output_format = TCC_OUTPUT_FORMAT_ELF;  // TCC_OUTPUT_FORMAT_BINARY;

	i=0;
	tcc_add_include_path(s,"/");
	while(dir[i])
	{
		if(dir[i][0]=='@')
			tcc_add_include_path(s,&dir[i][1]);
		else
		{
			Mergerdf(work,dir[i],temp);
			tcc_add_include_path(s,temp);
		}
		i++;
	}
	if(!(library&1))
		tcc_add_file(s,"xqc/include/stdlib.o");
	i=0;
	while(file[i])
	{
		sprintf(temp,"����c�ļ�:%s...",file[i]);
		errfunc(NULL,temp);
		if(tcc_add_file(s, file[i++])<0)
			goto tiao;
	}


	if(!(library&2))
	{
		// ��ȡtcc�ű��е�һ����������
		if (!tcc_get_symbol(s, "mrc_init")) 
		{
			errfunc(NULL,"mrc_initδ����");
			goto tiao;
		}
		if (!tcc_get_symbol(s, "mrc_event")) 
		{
			errfunc(NULL,"mrc_eventδ����");
			goto tiao;
		}
		if (!tcc_get_symbol(s, "mrc_resume")) 
		{
			errfunc(NULL,"mrc_resumeδ����");
			goto tiao;
		}
		if (!tcc_get_symbol(s, "mrc_resume")) 
		{
			errfunc(NULL,"mrc_resumeδ����");
			goto tiao;
		}
	}
	strcpy(temp,create);
	strcat(temp,".tem");
	mrc_remove(temp);

    ret = tcc_output_file(s, temp);
    if (ret != 0)
		goto tiao;
	mrc_remove(create);
	mrc_rename(temp,create);

	free(temp);
	tcc_delete(s);
    return 0;
tiao:
	free(temp);
    tcc_delete(s);
    return -1;
}

int32 project(char *name)
{
	INI *pini;
	char *a,*temp,*temp1,*temp2,*dir,*includedir[10],*output,*outprint;
	int config_ge,config_i,i,lib,res;
	if(PACKGEDMRP_mpr)
	{
	uint32 len;
	a=tcc_filenei("runner.ext",&len);
	if(a)
	{
		writefile("cfunction.ext",a,len);
		free(a);
	}

	a=tcc_filenei("start.mr",&len);
	if(a)
	{
		writefile("start.mr",a,len);
		free(a);
	}
	}
	temp=malloc(128);
	temp1=malloc(128);
	temp2=malloc(128);
	dir=malloc(128);
	output=malloc(128);
	outprint=malloc(128);
	strcpy(dir,name);
	a=mrc_strrchr(&dir[0],'/');
	if(a)
		a[0]=0;
	else
		dir[0]=0;

	pini = ini_open(name);
	if (!pini)
		return -1;
	ini_get(pini,"information@config",(char*)temp);
	config_ge=ini_strtok(config,temp);

	for(config_i=0;config_i<config_ge;config_i++)
	{
		filename_i=filenamec_i=0;
		sprintf(outprint,"����<%s>����...",config[config_i]);
		errfunc(NULL,outprint);
		ini_get(pini,"information@include",(char*)temp);
		ini_strtok(includedir,temp);
		for (i=0;i<1000;i++)
		{
			sprintf(temp2,"files_%s@file%02d",config[config_i],i);
			ini_get(pini,temp2,temp1);
			if(temp1[0])
			{
				if(stamp(temp1)&&(StringAmp(temp1,"c")==0||StringAmp(temp1,"o")==0))
				{
					filenamec[filenamec_i]=malloc(strlen(temp1)+strlen(dir)+2);
					Mergerdf(dir,temp1,filenamec[filenamec_i]);
				}
				else
				{
					filenamec[filenamec_i]=malloc(strlen(temp1)+strlen(dir)+2);
					Mergerdf(dir,temp1,filenamec[filenamec_i]);
				}
				filenamec_i++;
			}
			sprintf(temp2,"files@file%02d",i);
			ini_get(pini,temp2,temp1);
			if(temp1[0])
			{
				if(stamp(temp1)&&(StringAmp(temp1,"c")==0||StringAmp(temp1,"o")==0))
				{
					filenamec[filenamec_i]=malloc(strlen(temp1)+strlen(dir)+2);
					Mergerdf(dir,temp1,filenamec[filenamec_i]);
				}
				else
				{
					filenamec[filenamec_i]=malloc(strlen(temp1)+strlen(dir)+2);
					Mergerdf(dir,temp1,filenamec[filenamec_i]);
				}
				filenamec_i++;
			}
		}

		filenamec[filenamec_i]=NULL;
		sprintf(temp2,"config_%s@%s",config[config_i],"nostdlib");//�Ƿ����ñ�׼��
		lib=ini_get_int(pini,temp2);
		sprintf(temp2,"config_%s@%s",config[config_i],"library");//�Ƿ�����ں���
		lib+=ini_get_int(pini,temp2)<<1;
		res=compile_o(dir,includedir,filenamec,XQC_CODE_O,lib);
		if(res)
			break;
		if(!res&&PACKGEDMRP_mpr)
		{
			mpr_tou(pini);
			sprintf(temp2,"config_%s@%s",config[config_i],"output");
			ini_get(pini,temp2,temp1);
			if(temp1[0]=='@')
				strcpy(output,temp1+1);
			else
				Mergerdf(dir,temp1,output);
			sprintf(temp2,"config_%s@%s",config[config_i],"pack");
			if(ini_get_int(pini,temp2))
			{
				mrc_remove(output);
				mrc_rename(XQC_CODE_O,output);
			}
			else
			{
				charszfz(filename,XQC_CODE_O);
				charszfz(filename,"cfunction.ext");
				charszfz(filename,"start.mr");
				errfunc(NULL,"������Դ�ļ�...");
				sprintf(temp2,"config_%s@%s",config[config_i],"gzip");
				lib=ini_get_int(pini,temp2);
				if(!lib&1)
					gzipfile("xqc/temp",filename);
				PackgedMrp(NULL,filename,output);
				i=0;
				while(filename[i])
					errfunc(NULL,filename[i++]);
			}
		}
		ini_strtokfree(filenamec);
		ini_strtokfree(filename);
		ini_strtokfree(includedir);
		errfunc(NULL,"");
		errfunc(NULL,output);
	}

	free(temp);
	free(temp1);
	free(temp2);
	free(dir);
	free(output);
	free(outprint);
	if(PACKGEDMRP_mpr)
	{
		mrc_remove("cfunction.ext");
		mrc_remove("start.mr");
	}
	return res;
}



// ��������o�ļ�
int run_o(char *name,int mode)
{
    int ret;
    int outsize;
    TCCState *s;
	filemrp=mode;
    // �½�һ��TCCʵ��
    mainsf=s = tcc_new();

    // ��ʹ�ñ�׼��
    s->nostdinc = 1;
    s->nostdlib = 1;

    // ������ڴ�
    s->output_type = TCC_OUTPUT_MEMORY;

    // ������Ϣ���
    tcc_set_error_func(s, NULL, errfunc);
	loadlib(s);

    // ����o�ļ�

    ret = tcc_add_file(s, name);
    if (ret != 0)
        return -1;

    // ���������������Ҫ���ڴ��С
    ret = tcc_relocate_ex(s, NULL);
    if (-1 == ret)
        return -2;

    // �����ڴ�
    outsize = ret;
    outmemsf = malloc(outsize);
    if (NULL == outmemsf)
        return -3;
    memset(outmemsf, 0, outsize);

    // �ض�λ�����еķ���,��������ڴ���
    ret = tcc_relocate_ex(s, outmemsf);
    if (-1 == ret)
    {
        free(outmemsf);
        return -4;
    }


	// ��ȡtcc�ű��е�һ����������
	eventsf = (int32(*)(int32,int32,int32))tcc_get_symbol(s, "mrc_event");
	if (!eventsf) 
		return -1;

	maaini(0);

    return 0;
}

int tcc_setinis()
{
	INI *p;
	int tmpi;
	p=ini_open("xqc/temp/exchange.ini");

	if(!p)
		return -1;
	ini_get(p,"exchange@MAINNAME",&tempfilf_mpr[0]);
	tmpi=ini_get_int(p,"exchange@SCRW");
	if(tmpi)
		SCREEN_WIDTH=tmpi;
	tmpi=ini_get_int(p,"exchange@SCRH");
	if(tmpi)
		SCREEN_HEIGHT=tmpi;
	debug_mpr=ini_get_int(p,"exchange@DEBUG");
	PACKGEDMRP_mpr=ini_get_int(p,"exchange@PACKGEDMRP");
	ini_close(p);
	return 0;
}

/*---------------------------------------------------------------------------*/
//��������ʼ��
void runmain()
{
	run_o("code.o",1);
}
//��Ŀ��ʼ�����жϱ�����Ե�
void projectmain()
{
	int res=-1;
	tcc_setinis();
	errfunc(NULL,"���ڱ��롭��");
	if(tempfilf_mpr[0])
		if(!StringAmp((char*)tempfilf_mpr,"mpr"))
		{
			errfunc(NULL,tempfilf_mpr);
			res=project(tempfilf_mpr);
		}
	else
		errfunc(NULL,"����mpr��Ŀ�ļ�");
	else
		errfunc(NULL,"���ļ�ʱʧ��,NULL");
	if(res)
		errfunc(NULL,"---����ʧ��---");
	else
		errfunc(NULL,"---�������---");
	if(!res&&debug_mpr&& !PACKGEDMRP_mpr)
		run_o("xqc/temp/code.o",0);
	else
	errfunc(NULL,"***�����������***");

}
void mainio();
//����mrp
int32 tcc_runmrps(char *name)
{
	char *qd;
	if(mrc_fileState("xqc/mtk.txt")==MR_IS_FILE)
		qd="start.mr";
	else
		qd="cfunction.ext";
	return mrc_runMrp(name,qd,NULL);
}
int32 tcc_exitApp(void)
{	
	int32 (*func)(void);
	if(mainsf)
	{
		func=(int32 (*)(void))tcc_get_symbol(mainsf, "mrc_exitApp");
		if(func)
			func();
		// �ͷ�TCC���ĵ���Դ�����ܷ��ں�������֮ǰ
		tcc_delete(mainsf);
	}

	// �ͷ�������������ĵ��ڴ�.
	if(outmemsf)
		free(outmemsf);

	if(debug_mpr||PACKGEDMRP_mpr)
		tcc_runmrps("xqc/editor.mrp");
	else
		mrc_exit();
	return MR_SUCCESS;
}

#ifdef SIM_MOD

int32 tcc_init(void)
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
	mainsf=outmemsf=NULL;
	filemrp=0;
	eventsf=NULL;
	mrc_remove(XQC_ERR_FILE);

	mainio();
	return MR_SUCCESS;
}



#else  /* !SDK_MOD */

int32 mrc_init(void)
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
	mainsf=outmemsf=NULL;
	filemrp=0;
	eventsf=NULL;

	mainio();
	return MR_SUCCESS;
}

int32 mrc_exitApp(void)
{	
	return MR_SUCCESS;
}




int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	if(eventsf)
		eventsf(code,param0,param1);
	else
		tcc_exitApp();
	return MR_SUCCESS;
}
//int32 mrc_event(int32 code, int32 param0, int32 param1)
//{
//	return MR_SUCCESS;
//}

int32 mrc_pause()
{
	int32 (*func)(void);
	func=(int32 (*)(void))tcc_get_symbol(mainsf, "mrc_pause");
	if(func)
		func();
	return MR_SUCCESS;	
}

int32 mrc_resume()
{
	int32 (*func)(void);
	func=(int32 (*)(void))tcc_get_symbol(mainsf, "mrc_resume");
	if(func)
		func();
	return MR_SUCCESS;
}

int32 mrc_extRecvAppEvent(int32 app,int32 code,int32 param0,int32 param1)
{
    return MR_SUCCESS;
}
int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4,int32 p5)
{
    return MR_SUCCESS;
}

#endif  /* SDK_MOD */
