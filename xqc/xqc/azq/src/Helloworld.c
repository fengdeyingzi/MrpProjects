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
	AZ_SET=300,//设置界面
	AZ_MTK,//MTK认证
	AZ_ANZHUANG,//正在安装
	AZ_YICUNZAI,//检测到已经安装
	AZ_ANORUN,//安装或卸载
	AZ_UNINSTALL,//正在卸载
	AZ_OK,//完成

	AZ_END//结束
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
/*调试输出*/
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
//工作目录+文件名
char *getworking(char *name)
{
	sprintf(&workingdt[0],"%s%s",workingdir,name);
	return &workingdt[0];
}
//获取当前mrp操作
char *getsetmrp()
{
	if(config[config_i].file[0])
		return getworking(workingname);
	else
		return NULL;
}
//安装程序初始化
void anzhuanginit()
{
	memset(&andir[0],0,sizeof(ANDIR)*200);
	jyi=jyz=0;
	mrpyu[0]=0;
	if(getsetmrp())
		strcpy(mrpyu,getsetmrp());
}
//添加卸载项目文件
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
//进度条
void jindu(int xx,int yy,int ww,int hh,int dd,int zz)
{
	mrc_drawRect(xx,yy,ww,hh,255,255,255);
	mrc_drawRect(xx+1,yy+1,(ww-2)*dd/zz,hh-2,50,200,50);
	mrc_refreshScreen(xx,yy,ww,hh);
}
//屏幕单行显示
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
		tishi("--成功",y,2,0x00ff00);
	else
		tishi("--失败",y,2,0xff0000);
}

//
//创建一条路径,成功返回0失败返回-1
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
//读取一行
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
//读取内部文件
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
//创建并写入文件
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
//内部文件直接写入到文件
void neiwrite(char *name,char *name2)
{
	void *p;int32 len;
	p=filenei(getsetmrp(),name,&len);
	writeflie(name2,p,len);
	mrc_free(p);
}

//检测是否有解压到相对目录的
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
//组合文本分解
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

//启动mrp
int32 runmrps(char *name)
{
	char *qd;
	if(mrc_fileState("xqc/mtk.txt")==MR_IS_FILE)
		qd="start.mr";
	else
		qd="cfunction.ext";
	return mrc_runMrp(name,qd,NULL);
}
//逐个安装类型
void jieya(int data)
{
	char temp[128];
	temp[0]=0;
	switch(andir[jyi].lx)
	{
	case 1:
		sprintf(temp,"抽取:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		jyp=filenei(mrpyu,andir[jyi].text,&jylen);
		tsgb(20,jyp?1:0);
		if(jyp==NULL)
			return;
		break;
	case 2:
		sprintf(temp,"解压:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		tsgb(20,(andir[jyi].text&&writeflie(andir[jyi].text,jyp,jylen))?0:1);
		if(jyp)
			mrc_free(jyp);
		uninstallfile(andir[jyi].text);
		break;
	case 3:
		sprintf(temp,"设定mrp:%s",andir[jyi].text);
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
		sprintf(temp,"创建:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		tsgb(20,(MakePath(andir[jyi].text))?0:1);
		break;
	case 5:
		sprintf(temp,"删除:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		tsgb(20,(mrc_remove(andir[jyi].text))?0:1);
		break;
	case 6:
		sprintf(temp,"执行:%s",andir[jyi].text);
		tishi(temp,20,0,0Xffffff);
		tsgb(20,(runmrps(andir[jyi].text))?0:1);
		break;
	}
}


int jiec;
//全屏提示框
void scrtishi(char *bt,char *text,char *a1,char *a2,char *a3)
{
	int32 ww,hh;
	fontwh("钱",0,FONT,&ww,&hh);
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

//安装程序初始化
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



//开始安装
void anzhuang(char *setuptext)
{
	char text[128];

	if(setuptext)
		sprintf(text,"加载配置项%s",setuptext);
	else
		sprintf(text,"加载配置项%s@setup.ini",config[config_i].file);
	tishi(text,150,1,0xffffff);
	jyz=jmain(setuptext);
	if(jyz<=0)
	{
		tishi("失败",150,1,0xffffff);
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

//启动函数
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
		tishi("正在启动",150,1,0xffffff);
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
//查找一串，分隔符分串成功返回0

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
//安装工程
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
//提示框事件
void xianjie(int hend,int code)
{
	if(code==AZ_SET)
		jiec=hend;
	switch(hend)
	{
	case AZ_YICUNZAI:
		{
			scrtishi("已安装","检测到您的程序已经安装，请选择：\n1.下一步\n2.直接启动程序\n3.退出安装程序","下一步","启动","退出");
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
			scrtishi("已安装","请选择：\n1.覆盖安装\n2.卸载程序\n3.退出安装程序","覆盖","卸载","退出");
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
			scrtishi("设置机型","请选择您的机型：\n1.我是新型机\n2.退出安装程序\n3.我是低端机","新型机","退出","低端机");
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
					text="这个文件是用来识别低端机的，请勿删除！";
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
应用初始化函数
该函数在应用初始化期间被mythroad平台调用，可以在这个
函数中进行全局变量的初始化等工作。
返回值:
MR_SUCCESS  应用初始化成功
MR_FAILED      应用初始化失败
*/
int32 mrc_init(void)
{
	mrc_winInit();
	
	mrc_winNew(0, DemoWinWinEvent, DemoKeyWinEvent);

	return MR_SUCCESS;
}



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
int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	mrc_winEvent(code, param0, param1);
	return MR_SUCCESS;
}

/*
应用暂停函数
该函数在应用被暂停时（有电话等事件暂停应用）
被mythroad平台调用。
返回值:
MR_SUCCESS  操作成功
MR_FAILED      操作失败
*/
int32 mrc_pause(void)
{
	return 0;
}

/*
该函数在应用恢复运行时被mythroad平台调用。
返回值:
MR_SUCCESS  操作成功
MR_FAILED      操作失败
*/
int32 mrc_resume(void)
{
	mrc_winResume();
	return 0;
}

/*
该函数在应用退出时被mythroad平台调用。
返回值:
MR_SUCCESS  操作成功
MR_FAILED      操作失败
*/
int32 mrc_exitApp(void)
{
	tishi("正在退出。。。",0,0,0xff0000);
	closefont();
	return 0;
}