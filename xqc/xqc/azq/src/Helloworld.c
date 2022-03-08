#include <mrc_base.h>
#include "mrc_win.h"

#include "Helloworld.h"
#include <mrc_base_i.h>
#include <mrc_exb.h>
#include <mrc_bmp.h>
#include <libini.h>
#include <font.h>
int32 SCRW, SCRH;
char workingdir[64],workingdt[128];

#define  workingname "temp.mrp"
#define  uninstallname "uninstall.ini"

void xianjie(int c,int code);

typedef struct
{
	char config[128];
	char projectname[128];
	char file[64];
}CONFIG;
CONFIG config[10];

enum{
	AZ_SET=300,//���ý���
	AZ_MTK,//MTK��֤
	AZ_ANZHUANG,//���ڰ�װ
	AZ_YICUNZAI,//��⵽�Ѿ���װ
	AZ_ANORUN,//��װ��ж��
	AZ_UNINSTALL,//����ж��
	AZ_OK,//���

	AZ_END//����
};

typedef struct
{
	char text[256];
	char textex[256];
	int lx;
}ANDIR;
ANDIR andir[200];

typedef struct TEXT_BOXEX
{
	char text[200][256];
	mr_screenRectSt TextRect[200];
	int obgi;
	int all;
	mr_screenRectSt Rect;
	mr_colourSt TextCol;
	mr_colourSt BackCol;
}TEXT_BOX;
TEXT_BOX TextBox;



char mrpyu[128];
int jyi,jyz;
int32 jylen;
void *jyp;
int config_i,config_ge;

void drawtextbox()
{
	int i=0;
	mrc_drawRect(TextBox.Rect.x,TextBox.Rect.y,TextBox.Rect.w,TextBox.Rect.h,TextBox.BackCol.r,TextBox.BackCol.g,TextBox.BackCol.b);
	while(i<TextBox.all)
	{
	}
	mrc_refreshScreen(TextBox.Rect.x,TextBox.Rect.y,TextBox.Rect.w,TextBox.Rect.h);
}
/*�������*/
void prints(char *text,int n)
{
	char Number[25];int32 f;
	if(!text)
		text="null";
	sprintf(&Number[0],"%d\n\n",n);
	f=mrc_open("print.txt",12);
	if(f>0){
		mrc_seek(f,0,2);
		mrc_write(f,text,strlen(text));
		mrc_write(f,Number,strlen(Number));
		mrc_close(f);
	}
}
//typedef struct
//{
//	int f;char *dt;
//	int16 x;int16 y;int16 w;int16 h;
//	uint16 color;uint8 font;
//	uint8 *hc;uint8 hclen;
//	int b;uint16 a;
//	int16 i;uint16 g;
//	uint16 ii;int32 l;
//	uint8 z;uint8 fontt;
//}fontst;
//fontst ft;
//����Ŀ¼+�ļ���
char *getworking(char *name)
{
	sprintf(&workingdt[0],"%s%s",workingdir,name);
	return &workingdt[0];
}
//��ȡ��ǰmrp����
char *getsetmrp()
{
	if(config[config_i].file[0])
		return getworking(workingname);
	else
		return NULL;
}
//��װ�����ʼ��
void anzhuanginit()
{
	memset(&andir[0],0,sizeof(ANDIR)*200);
	jyi=jyz=0;
	mrpyu[0]=0;
	if(getsetmrp())
		strcpy(mrpyu,getsetmrp());
}
//���ж����Ŀ�ļ�
void uninstallfile(char *name)
{
	int f;
	f=mrc_open(getworking(uninstallname),12);
	mrc_seek(f,0,2);
	mrc_write(f,(void *)"remove=",strlen("remove="));
	mrc_write(f,name,strlen(name));
	mrc_write(f,(void *)"\n",strlen("\n"));
	mrc_close(f);
}
//������
void jindu(int xx,int yy,int ww,int hh,int dd,int zz)
{
	mrc_drawRect(xx,yy,ww,hh,255,255,255);
	mrc_drawRect(xx+1,yy+1,(ww-2)*dd/zz,hh-2,50,200,50);
	mrc_refreshScreen(xx,yy,ww,hh);
}
//��Ļ������ʾ
void tishi(char *text,int y,int mode,long color)
{
	int16 x;
	int32 w,h;
	mr_colourSt *C=(void *)&color;
	fontwh(text,0,FONT,&w,&h);
	x=(int16)(mode==1?(SCRW-w)/2:(mode==2?SCRW-w:0));
	if(mode!=2)mrc_drawRect(0,(int16)y,(int16)SCRW,(int16)h+1,0,0,0);
	else mrc_drawRect((int16)x,(int16)y,(int16)w,(int16)h+1,0,0,0);
	gbsez(text,(int16)x,(int16)y,C->b,C->g,C->r,0,FONT);
	mrc_drawLine((int16)x,(int16)(y+h),x+(int16)w,(int16)(y+h),C->b,C->g,C->r);
	mrc_refreshScreen(0,(int16)y,(int16)SCRW,(int16)h+1);
}

void tsgb(int y,int data)
{
	if(data)
		tishi("--�ɹ�",y,2,0x00ff00);
	else
		tishi("--ʧ��",y,2,0xff0000);
}

//
//����һ��·��,�ɹ�����0ʧ�ܷ���-1
int32 MakePath(char *path)
{
	int32 i,t;
	char *dir=NULL;
	char *dir2=NULL;

	if(!path) return -1;
	i = strlen(path);
	if(i>127) return -1;
	dir=(char*)mrc_malloc(256);
	dir2=&dir[128];
	memset(dir,0,256);
	do
	{
		for(t=i=0; *path; i++,path++)
		{
			if(*path == '/')
			{t=1;path++;break;}
			dir[i]=*path;
		}
		dir[i]=0;
		if(t && *dir)
		{
			mrc_strcat(dir2,dir);
			mrc_mkDir(dir2);
			mrc_strcat(dir2,"/");
		}
	}while(*path);
	free(dir);
	return 0;
}
//��ȡһ��
int readline(char **p, char *out)
{
	int i=0,fi=0,tmp=0;
	if(*p==NULL)
		return -1;
	do{
		if(!(tmp=(*p)[fi]))
			goto AA;
		fi++;
	}while(tmp=='\n' || tmp=='\r');
	fi--;
	do{
		out[i]=tmp;
		i++;fi++;
		if(!(tmp=(*p)[fi]))
		{
			out[i]=0;
			(*p)+=fi;
			return 0;
		}
		if(tmp=='\n' || tmp=='\r')
			break;
	}while(i<LINE_LEN && tmp);
	(*p)+=fi;
	return out[i]=0;
AA:
	out[i]=0;
	return -1;

}
//��ȡ�ڲ��ļ�
void *filenei(char *file,const char *name,int32 *Len)
{
	int32 len=0;
	void *p=NULL,*P=NULL;
	Len[0]=0;
	if(file==NULL)
		goto tiao;
	if(!file[0])
		goto tiao;
	mrc_readFileFromMrpEx(file,name,(uint8 **)&p,(int32 *)&len,0);
	*Len=len;
	return p;
tiao:
	p=mrc_readFileFromMrp(name, &len, 0);
	if(p==NULL)
		return NULL;
	P=mrc_malloc(len+1);
	if(P==NULL)
	{
		mrc_freeFileData(p,len);
		return NULL;
	}
	memset(P,0,len+1);
	memcpy(P,p,len);
	mrc_freeFileData(p,len);
	*Len=len;
	return P;
}
//������д���ļ�
int writeflie(char *name,void *p,uint32 Len)
{
	int32 f;
	mrc_remove(name);
	f=mrc_open(name,12);
	if(!f)
		return -1;
	mrc_write(f,p,Len);
	return mrc_close(f);
}
//�ڲ��ļ�ֱ��д�뵽�ļ�
void neiwrite(char *name,char *name2)
{
	void *p;int32 len;
	p=filenei(getsetmrp(),name,&len);
	writeflie(name2,p,len);
	mrc_free(p);
}

//����Ƿ��н�ѹ�����Ŀ¼��
void setdir()
{
	char *zdir,text[128];int i;
	zdir=mrc_malloc(128);
	if(mrc_GetCurrentDirectory(128,zdir)<=0);
	for (i=0;i<jyz;i++)
		if(andir[i].text[0]=='@')
		{
			strcpy(text,andir[i].text+1);
			sprintf(andir[i].text,"%s%s",zdir[0]==0?"/":zdir,text);
		}
		free(zdir);
}
//����ı��ֽ�
int shutest(int32 dqg,char *text,char *biao,int i)
{
	if(text==mrc_strstr(text,biao))
	{
		char *fenhou;
		andir[dqg].lx=i;
		fenhou=mrc_strrchr(text,',');
		if(fenhou)
		{

			memcpy(memset(andir[dqg].text,0,256),text+strlen(biao),strlen(text)-strlen(biao)-strlen(fenhou));
			fenhou++;
			andir[++dqg].lx=++i;
			memcpy(memset(andir[dqg].text,0,256),fenhou,strlen(fenhou));
			return 2;
		}
		else
			memcpy(memset(andir[dqg].text,0,256),text+strlen(biao),strlen(text)-strlen(biao));
		return 1;
	}
	return 0;
}

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
//�����װ����
void jieya(int data)
{
	char temp[128];
	temp[0]=0;
	switch(andir[jyi].lx)
	{
	case 1:
		sprintf(temp,"��ȡ:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		jyp=filenei(mrpyu,andir[jyi].text,&jylen);
		tsgb(20,jyp?1:0);
		if(jyp==NULL)
			return;
		break;
	case 2:
		sprintf(temp,"��ѹ:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		tsgb(20,(andir[jyi].text&&writeflie(andir[jyi].text,jyp,jylen))?0:1);
		if(jyp)
			mrc_free(jyp);
		uninstallfile(andir[jyi].text);
		break;
	case 3:
		sprintf(temp,"�趨mrp:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		if(mrc_strstr(andir[jyi].text,"null")==andir[jyi].text)
		{
			mrpyu[0]=0;
			if(getsetmrp())
				strcpy(mrpyu,getsetmrp());
		}
		else
		{
			mrpyu[0]=0;
			strcpy(mrpyu,andir[jyi].text);
		}
		tsgb(20,1);
		break;
	case 4:
		sprintf(temp,"����:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		tsgb(20,(MakePath(andir[jyi].text))?0:1);
		break;
	case 5:
		sprintf(temp,"ɾ��:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		tsgb(20,(mrc_remove(andir[jyi].text))?0:1);
		break;
	case 6:
		sprintf(temp,"ִ��:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		tsgb(20,(runmrps(andir[jyi].text))?0:1);
		break;
	}
}


int jiec;
//ȫ����ʾ��
void scrtishi(char *bt,char *text,char *a1,char *a2,char *a3)
{
	int32 ww,hh;
	fontwh("Ǯ",0,FONT,&ww,&hh);
	mrc_clearScreen(0,0,0);
	if(bt!=NULL)
	{
		mrc_drawRect(0,0,(int16)SCRW,(int16)hh,180,180,180);
		gbsez(bt,0,0,0,0,0,0,FONT);
	}
	if(text!=NULL)
	{
		mr_screenRectSt rect; mr_colourSt colorst;
		rect.x=5;rect.y=(int16)hh+5;rect.w=(int16)SCRW-10;rect.h=(int16)(SCRH-hh*2-10);
		colorst.r=colorst.g=colorst.b=200;
		gbsez2(text,rect.x,rect.y,rect,colorst,2,FONT);
	}
	mrc_drawRect(0,(int16)(SCRH-hh),(int16)SCRW,(int16)hh,180,180,180);
	if(a1!=NULL)
		gbsez(a1,0,(int16)(SCRH-hh),0,0,0,0,FONT);
	if(a2!=NULL)
	{
		fontwh(a2,0,FONT,&ww,&hh);
		gbsez(a2,(int16)(SCRW-ww)/2,(int16)(SCRH-hh),0,0,0,0,FONT);
	}
	if(a3!=NULL)
	{
		fontwh(a3,0,FONT,&ww,&hh);
		gbsez(a3,(int16)(SCRW-ww),(int16)(SCRH-hh),0,0,0,0,FONT);
	}
	mrc_refreshScreen(0,0,(int16)SCRW,(int16)SCRH);
}


void Safeapp(int32 data);

//��װ�����ʼ��
int jmain(char *setuptext)
{ 
	int32 len;
	char temp[128],*biao;
	int32 i=0,j;

	anzhuanginit();
	mrpyu[0]=0;
	if(!setuptext&&getsetmrp())
		strcpy(mrpyu,getsetmrp());
	if(setuptext)
	{
		int32 f;
		len=mrc_getLen(setuptext);
		if(len<0)
			return -1;
		f=mrc_open(setuptext,1);
		setuptext=mrc_malloc(len+1);
		mrc_read(f,setuptext,len);
		mrc_close(f);
	}else
		setuptext=filenei(mrpyu,"setup.ini",&len);
	biao=setuptext;
	setuptext[len]=0;
	if(setuptext==NULL)
		return -1;
	while(!readline(&biao,temp))
	{
		if(j=shutest(i,temp,"file=",1))i+=j;
		if(j=shutest(i,temp,"mrp=",3))i+=j;
		if(j=shutest(i,temp,"dir=",4))i+=j;
		if(j=shutest(i,temp,"remove=",5))i+=j;
		if(j=shutest(i,temp,"runmrp=",6))i+=j;
	}
	mrc_free(setuptext);
	setdir();
	return i;
}



//��ʼ��װ
void anzhuang(char *setuptext)
{
	char text[128];

	if(setuptext)
		sprintf(text,"����������%s",setuptext);
	else
		sprintf(text,"����������%s@setup.ini",config[config_i].file);
	tishi(text,150,1,0xffffff);
	jyz=jmain(setuptext);
	if(jyz<=0)
	{
		tishi("ʧ��",150,1,0xffffff);
		return;
	}

	for(jyi=0;jyi <= jyz;jyi++)
	{
		if(config_ge)
		{
			tishi(mrpyu,60,1,0xffffff);
			tishi(config[config_i].projectname,80,1,0xffffff);
			jindu(5,100,SCRW-10,7,config_i,config_ge);
		}

		sprintf(&text[0],"%d%%",jyi*100/jyz);
		tishi(&text[0],150,1,0xffffff);
		jindu(5,130,SCRW-10,7,jyi,jyz);
		jieya(0);
	}
}

//��������
void Safeapp(int32 data)
{
	if(!data)
	{

		if(jiec==AZ_MTK)
			jiec=AZ_ANZHUANG;
		else
		{
			xianjie(AZ_MTK,AZ_SET);
			return;
		}
		config_i=0;

		mrc_clearScreen(0,0,0);
		mrc_refreshScreen(0,0,(int16)SCRW,(int16)SCRH);
		tishi("��������",150,1,0xffffff);
		data=mrc_timerCreate();
		mrc_remove(getworking(workingname));
		mrc_remove(getworking(uninstallname));
		mrc_timerStart(data,1,data,Safeapp,0);
		return;
	}
	else
	{
		void *p;int32 len;
		if(config[config_i].config[0])
		{
			if(config[config_i].file[0])
			{
				p=filenei(NULL,config[config_i].file,&len);
				writeflie(getworking(workingname),p,len);
				mrc_free(p);
			}
			anzhuang(NULL);
			mrc_remove(getworking(workingname));
			config_i++;
			if(data>0)
				mrc_timerStart(data,1,data,Safeapp,0);
			return;
		}

		if(data>0)
			mrc_timerDelete(data);
	}
}
//����һ�����ָ����ִ��ɹ�����0

int getdou(char **dt,char *temp)
{
	char *p;
	if(*dt==NULL||(*dt)[0]==0)
		return -1;
	p=mrc_memchr(*dt,',',strlen(*dt));
	if(p==NULL)
	{
		strcpy(temp,*dt);
		*dt+=strlen(*dt);
		for(p=temp; 0!=*p&&'\r'!=*p&&'\n'!=*p; p++);
		*p=0;
		return 0;
	}
	memcpy(temp,*dt,strlen(*dt)-strlen(p));
	temp[strlen(*dt)-strlen(p)]=0;
	*dt=p+1;
	return 0;
}
//��װ����
void getini()
{
	INI *p;
	int32 len,ii;
	char v1[128],*name;
	name=filenei(NULL,"xqcset.ini",&len);
	p=ini_open(name);
	mrc_free(name);
	ini_get(p,"setup@working",v1);
	MakePath(v1);
	strcpy(workingdir,v1);
	ini_get(p,"setup@config",v1);
	name=v1;
	for(len=0;getdou(&name,config[len].config)==0;len++);
	config[len].config[0]=0;
	config_ge=len-1;
	for(ii=0;ii<len;ii++)
	{
		sprintf(v1,"%s@projectname",config[ii].config);
		ini_get(p,v1,config[ii].projectname);
		sprintf(v1,"%s@file",config[ii].config);
		ini_get(p,v1,config[ii].file);
	}
	ini_close(p);

}

//
//��ʾ���¼�
void xianjie(int hend,int code)
{
	if(code==AZ_SET)
		jiec=hend;
	switch(hend)
	{
	case AZ_YICUNZAI:
		{
			scrtishi("�Ѱ�װ","��⵽���ĳ����Ѿ���װ����ѡ��\n1.��һ��\n2.ֱ����������\n3.�˳���װ����","��һ��","����","�˳�");
			switch(code)
			{
			case MR_KEY_SOFTLEFT:case MR_KEY_1:
				jiec=AZ_ANZHUANG;
				xianjie(AZ_ANORUN,AZ_SET);
				return;
			case MR_KEY_SELECT:case MR_KEY_2:
				runmrps("xqc/editor.mrp");
				return;
			case MR_KEY_SOFTRIGHT:case MR_KEY_3:
				mrc_exit();
				return;
			}
			return;
		}

	case AZ_ANORUN:
		{
			scrtishi("�Ѱ�װ","��ѡ��\n1.���ǰ�װ\n2.ж�س���\n3.�˳���װ����","����","ж��","�˳�");
			switch(code)
			{
			case MR_KEY_SOFTLEFT:case MR_KEY_1:
				jiec=AZ_ANZHUANG;
				Safeapp(0);
				return;
			case MR_KEY_SELECT:case MR_KEY_2:
				config_ge=0;
				mrc_clearScreen(0,0,0);
				mrc_refreshScreen(0,0,(int16)SCRW,(int16)SCRH);
				anzhuang(getworking(uninstallname));
				return;
			case MR_KEY_SOFTRIGHT:case MR_KEY_3:
				mrc_exit();
				return;
			}
			return;
		}
	case AZ_MTK:
		{
			scrtishi("���û���","��ѡ�����Ļ��ͣ�\n1.�������ͻ�\n2.�˳���װ����\n3.���ǵͶ˻�","���ͻ�","�˳�","�Ͷ˻�");
			switch(code)
			{
			case MR_KEY_SOFTLEFT:case MR_KEY_1:
				mrc_remove("xqc/mtk.txt");
				Safeapp(0);
				return;
			case MR_KEY_SELECT:case MR_KEY_2:
				mrc_exit();
				return;
			case MR_KEY_SOFTRIGHT:case MR_KEY_3:
				{
					int32 f;char *text="xqc/mtk.txt";
					mrc_remove(text);
					f=mrc_open(text,12);
					text="����ļ�������ʶ��Ͷ˻��ģ�����ɾ����";
					mrc_write(f,text,strlen(text));
					mrc_close(f);
					Safeapp(0);
				}
				return;
			}
			return;
		}
	}
}


void DemoWinWinEvent(int32 data, int32 eventId)
{
	switch (eventId)
	{
	case WIN_EVENT_SHOW:
	case WIN_EVENT_REFRESH:	
		{
			mr_screeninfo screen;
			mrc_getScreenInfo(&screen);
			SCRW = (uint16)(screen.width);
			SCRH = (uint16)(screen.height);
			if(!SCRW||!SCRH)
			{
				SCRW=240;
				SCRH=320;
			}

			//	memset(&ft,0,sizeof(ft));
			jiec=0;
			FONT=1;
			openfont(FONT);
			getini();


			if(mrc_getLen(getworking(uninstallname))>0)
				xianjie(AZ_YICUNZAI,AZ_SET);
			if(!jiec)
			{
				jiec=AZ_ANZHUANG;
				Safeapp(0);
			}
		}
		break;
	case WIN_EVENT_PAUSE:
		break;
	case WIN_EVENT_EXIT:
		break;
	case WIN_EVENT_UPDATE:
		break;
	}
}

void DemoKeyWinEvent(int32 data, int32 type, int32 p1, int32 p2)
{
	if(MR_KEY_PRESS == type)
	{
		xianjie(jiec,p1);
	}
	else if(MR_MOUSE_UP == type)
	{
	}
	else
		if(type==MR_MOUSE_UP)
		if(p2>SCRH-30)
		{
			if(p1<SCRW/3)
				xianjie(jiec,MR_KEY_SOFTLEFT);
			else
				if(p1-SCRW/3*2>0)
					xianjie(jiec,MR_KEY_SOFTRIGHT);
				else
					xianjie(jiec,MR_KEY_SELECT);
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
int32 mrc_init(void)
{
	mrc_winInit();
	
	mrc_winNew(0, DemoWinWinEvent, DemoKeyWinEvent);

	return MR_SUCCESS;
}



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
int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	mrc_winEvent(code, param0, param1);
	return MR_SUCCESS;
}

/*
Ӧ����ͣ����
�ú�����Ӧ�ñ���ͣʱ���е绰���¼���ͣӦ�ã�
��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
*/
int32 mrc_pause(void)
{
	return 0;
}

/*
�ú�����Ӧ�ûָ�����ʱ��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
*/
int32 mrc_resume(void)
{
	mrc_winResume();
	return 0;
}

/*
�ú�����Ӧ���˳�ʱ��mythroadƽ̨���á�
����ֵ:
MR_SUCCESS  �����ɹ�
MR_FAILED      ����ʧ��
*/
int32 mrc_exitApp(void)
{
	tishi("�����˳�������",0,0,0xff0000);
	closefont();
	return 0;
}