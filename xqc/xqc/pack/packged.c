#include "mrc_base.h"
#include "mrc_exb.h"
#include "mrc_bmp.h"
#include "libini.h"

#define FILESTARTPOS 240 //mrp包文件，文件信息流开始位置
#define FILENAMELEN 128 //最大文件名长度
int32 FileLen;//文件总长度
int32 HeadLen;//文件头长度
/////////////////////////待打包文件链表//////////////////////////////////
typedef struct applistnode {
	struct applistnode *pre;
	struct applistnode *next;
	int32 namepos;    //文件名写入指针
	int32 namelen;    //文件名长度
	char *filename;   //文件名
	int32 datapos;    //文件写入指针
	int32 datalen;    //文件长度
}AppListLink,*AppListLink_p;

typedef struct  appgongtou{
	char mrpg[4];//4
	int HeadLen;//8
	int FileLen;//12
	int listpos;//16
	char filename[12];//28
	char appname[24];//52
	char chuan[16];//68
	int appid;//72
	int version;//76
	int visible;//88
	int cpu;
	//int cp;
	char vendor[40];//128
	char description[64];//192
	int version16;
	int appid16;
	char config[40];

}AppTou,*AppTou_p;
AppTou apptou;

int tsy=0;

#define mallocchar(text) memcpy(malloc(strlen(text)+1),text,strlen(text)+1)
char *includechar[50];
int includechar_i;
//添加一个附件
void includecharnode(char *file)
{
	includechar[includechar_i++]=mallocchar(file);
}
//清空附件
void includecharfree()
{
	int i;
	for(i=0;i<includechar_i;includechar[i++]=NULL)
		free(includechar[i]);
	includechar_i=0;
}
// 提取两个指针间的字符串
char *ttexttwo(char *start,char *end)
{
	char *text;
	int l=(int)(++end)-(int)start;
	text=malloc(l+1);
	text[l]=0;
	memcpy(text,start,l);
	return text;
}
//提取换行符前的内容
char *ttextend(char *str)
{
	char *temp;
	temp=strstr(str,"\n");
	if(temp)
		return ttexttwo(str,temp);
	else
		return mallocchar(str);
}
void tishi(char *text)
{
	if(!text)
		return;
	mrc_drawRect(0,tsy*20,240,20,0,0,0);
	mrc_drawText(text,0,tsy*20,240,240,240,0,0);
	mrc_refreshScreen(0,0,240,320);
	tsy++;
}
//获取目录内的文件名
char *chrfile(char *text)
{
	char *temp;
	temp=mrc_strrchr(text,'/');
	if(temp)
		temp++;
	else
		temp=text;
	return temp;
}

int32 SetAppListInfo(AppListLink_p head)
{
	AppListLink_p temp;
	int32 tempnamepos;
	int32 tempdatapos;
	int32 filelistlen;
	tempnamepos=FILESTARTPOS;
	tempdatapos=0;
	filelistlen=0;
	temp=head;
	if(temp!=NULL)
		temp=temp->next;
	else 
		return -1;
	while(temp!=NULL)
	{
		temp->datalen=mrc_getLen(temp->filename);
		temp->namelen=mrc_strlen(chrfile(temp->filename))+1+16;
		temp->namepos=tempnamepos;
		tempnamepos+=temp->namelen;
		temp=temp->next;
	}
	temp=head;
	tempdatapos=tempnamepos;
	while(temp!=NULL)
	{
		tempdatapos+=mrc_strlen(chrfile(temp->filename))+1+8;
		temp->datapos=tempdatapos;
		tempdatapos+=temp->datalen;
		temp=temp->next;
	}	
	HeadLen=tempnamepos-4;
	FileLen=tempdatapos;
	return tempnamepos-4;
}
//向mrp包写入mrp信息
int32 WriteMrpInfo(char *filename)
{
	int32 hand;
	int32 listpos=FILESTARTPOS;
	hand=mrc_open(filename,2);
	if(hand==0)
		return -1;
	memcpy(apptou.mrpg,"MRPG",4);
	apptou.HeadLen=HeadLen;
	apptou.FileLen=FileLen;
	apptou.listpos=listpos;
	//mrc_write(hand,"MRPG",4);
	//mrc_write(hand,&HeadLen,4);
	//mrc_write(hand,&FileLen,4);
	//mrc_write(hand,&listpos,4);
	//mrc_seek(hand,28,MR_SEEK_SET);
	//mrc_write(hand,"PACK",4);
	mrc_write(hand,&apptou,FILESTARTPOS);
	mrc_close(hand);
	return 0;
}
//向mrp包写入文件信息
int32 WriteFileInfo(AppListLink_p node,char *filename)
{
	int32 templen;
	int32 writehand;
	char *temp;
	temp=chrfile(node->filename);
	writehand=mrc_open(filename,2);
	mrc_seek(writehand,node->namepos,MR_SEEK_SET);
	templen=mrc_strlen(temp)+1;
	mrc_write(writehand,&templen,4);
	mrc_write(writehand,temp,templen);
	mrc_write(writehand,&node->datapos,4);
	mrc_write(writehand,&node->datalen,4);
	mrc_close(writehand);
	return 0;
}
//向mrp包写入文件数据
int32 WriteFileData(AppListLink_p node,char *filename)
{
	int32 templen;
	int32 datahand,writehand;
	char *data;
	char *temp;
	temp=chrfile(node->filename);
	data=mrc_malloc(node->datalen);
	datahand=mrc_open(node->filename,1);
	if(datahand==0)
	{
		mrc_free(data);
		return -1;
	}
	mrc_read(datahand,data,node->datalen);
	mrc_close(datahand);
	writehand=mrc_open(filename,2);
	mrc_seek(writehand,node->datapos-mrc_strlen(temp)-1-8,MR_SEEK_SET);
	templen=mrc_strlen(temp)+1;
	mrc_write(writehand,&templen,4);
	mrc_write(writehand,temp,mrc_strlen(temp)+1);
	mrc_write(writehand,&node->datalen,4);
	mrc_seek(writehand,node->datapos,MR_SEEK_SET);
	mrc_write(writehand,data,node->datalen);
	mrc_close(writehand);
	mrc_free(data);
	return 0;

}
//创建一个空文件
int32 CreateNullFile(char *filename,int32 len)
{
	char data[1024];
	int32 hand,i,n;
	mrc_memset(data,0,1024);
	i=0;
	n=len/1024;
	hand=mrc_open(filename,2+8);
	for(i=0;i<n;i++)
	{
		mrc_write(hand,data,1024);
	}
	mrc_write(hand,data,len%1024);
	mrc_close(hand);
	return 0;

}
/////////////////////////////////////////////////////////////
//制造一个节点
AppListLink_p MakeAppListNode(char *filename)
{
	AppListLink_p temp;
	temp=mrc_malloc(sizeof(AppListLink));
	if(temp==NULL)
		return NULL;
	mrc_memset(temp,0,sizeof(AppListLink));
	temp->filename=mrc_malloc(mrc_strlen(filename)+1);
	mrc_memcpy(temp->filename,filename,mrc_strlen(filename)+1);
	return temp;
}
//插入
AppListLink_p InsertAppListNode(AppListLink_p pre,AppListLink_p node)
{
	AppListLink_p next;
	if(pre==NULL || node==NULL)
		return NULL;
	next=pre->next;
	pre->next=node;
	node->pre=pre;
	node->next=next;
	if(next!=NULL)
		next->pre=node;
	return node;
}
AppListLink_p InsertAppListText(AppListLink_p pre,char *node)
{
	return InsertAppListNode(pre,MakeAppListNode(node));
}
//删除一个
void DelAAppListNode(AppListLink_p node)
{
	AppListLink_p pre,next;
	if(node==NULL)
		return ;
	pre=node->pre;
	next=node->next;
	if(pre!=NULL)
		pre->next=next;
	if(next!=NULL)
		next->pre=pre;
	mrc_free(node->filename);
	mrc_free(node);
}
//删除全部
void DelAllAppListLink(AppListLink_p node)
{
	AppListLink_p  pre,next;
	if(node==NULL)
		return ;
	pre=node->pre;
	next=node->next;
	while(pre!=NULL)
	{
		DelAAppListNode(pre);
		pre=pre->pre;
	}
	while(next!=NULL)
	{
		DelAAppListNode(next);
		next=next->next;
	}
}
//完整的打包
int32 CreateMrpFile(char *filename,AppListLink_p head)
{
	AppListLink_p temp;
	if(head==NULL)
	{
		return -1;
	}
	mrc_remove(filename);
	SetAppListInfo(head);
	CreateNullFile(filename,FileLen);
	temp=head->next;
	WriteMrpInfo(filename);
	while(temp!=NULL)
	{
		WriteFileInfo(temp,filename);
		WriteFileData(temp,filename);
		temp=temp->next;
	}
	return 0;
}
//int memtouchar(char *out,char *text,char *yuan)
//{
//	if(out=strstr(out,text))
//		memcpy(yuan,out+strlen(text),strlen(out)-strlen(text));
//	else
//		return -1;
//	return 0;
//}
//void memtouint(char *out,char *text,int *yuan)
//{
//	if(out=strstr(out,text))
//		*yuan=atoi(out+strlen(text));
//}
//插入一个文件夹
AppListLink_p  FindDataFile(char *path)
{
	AppListLink_p head;
	char temppath[FILENAMELEN*2];
	char data[FILENAMELEN+1];
	int32 findhand;
	head=MakeAppListNode("head");
	findhand=mrc_findStart(path,data,FILENAMELEN);
	if(findhand==0)
		return head;
	temppath[0]=0;
	mrc_sprintf(temppath,"%s/%s",path,data);
	if(mrc_fileState(temppath)==MR_IS_FILE)
	{
		InsertAppListText(head,temppath);
	}
	while(mrc_findGetNext(findhand,data,FILENAMELEN)==0)
	{
		temppath[0]=0;
		mrc_sprintf(temppath,"%s/%s",path,data);
		if(mrc_fileState(temppath)==MR_IS_FILE)
		{
			InsertAppListText(head,temppath);
		}
	}
	mrc_findStop(findhand);
	return head;
}
//插入附件
AppListLink_p  FindCharFile(AppListLink_p  head,char **path)
{
	int i=0;
	if(path==NULL)
		return head;
	while(path[i])
		InsertAppListText(head,path[i++]);

	return head;
}

char *c_file, **c_fj, *c_mrpname;
int HEAP_SIZE,SCREEN_WIDTH,SCREEN_HEIGHT,Debugts;
int setuptext(char *text)
{
	char temp[128];
	mr_screeninfo screen;
	text[0]=0;

	sprintf(temp,"HEAP_SIZE=%d\n",HEAP_SIZE/1024);
	strcat(text,temp);

	mrc_getScreenInfo(&screen);
	if(SCREEN_WIDTH == (uint16)(screen.width)&&SCREEN_HEIGHT == (uint16)(screen.height))
		sprintf(temp,"SCRWH=%d,%d\n",0,0);
	else
		sprintf(temp,"SCRWH=%d,%d\n",SCREEN_WIDTH,SCREEN_HEIGHT);
	strcat(text,temp);

	if(Debugts)
	{
		sprintf(temp,"DEBUG=%d\n",Debugts);
		strcat(text,temp);
	}



	return 0;
}
int setupfile(char *file)
{
	int32 f;
	char text[512];
	mrc_remove(file);
	f=mrc_open(file,12);
	if(!f)
		return -1;
	setuptext(text);
	mrc_write(f,text,strlen(text));
	return mrc_close(f);
}
//插入关键文件
void gjfile()
{
	includecharnode("xqc/runner/start.mr");
	includecharnode("xqc/runner/cfunction.ext");
	setupfile("xqc/temp/setup.ini");
	includecharnode("xqc/temp/setup.ini");
}
int ttwjjc(char *name)
{
	char temp[128],*temp_p;
	char *nei[]={"base.h","ctype.h","sound.h","net.h"};
	int i,l=sizeof(nei)/4;
	for(i=0;i<l;i++)
		if(!strcmp(name,nei[i]))
			return 2;
	temp_p=mrc_strrchr(name,'.');
	if(temp_p)
	{
		sprintf(temp,"xqc/include/%s.ext",chrfile(ttexttwo(name,temp_p-1)));
		if(mrc_fileState(temp)==1)
		{
			includecharnode(temp);
			return 1;
		}
	}
	if(mrc_fileState(name)==1)
	{
		includecharnode(name);
		return 0;
	}
	return -1;
}
//读取源码中的头文件
int touwenjian(char *name)
{
	uint32 len;
	char *text,*text2,*temp,*temp2;
	text=text2=mrc_readAll(name,&len);
	if(!text)
		return -1;
	do
	{
		temp=strstr(text2,"#include");
		if(!temp)
			goto tiao;
		temp+=8;text2=temp;
		temp=ttextend(temp);
		temp2=strstr(temp,"\"");
		if(temp2)
		{
			temp2=ttexttwo(temp2+1,strstr(temp2+1,"\"")-1);
			if(!ttwjjc(temp2))
				touwenjian(temp2);
			touwenjian(temp2);
			free(temp2);
		}
		else
			if(temp2=strstr(temp,"<"))
			{
				temp2=ttexttwo(temp2+1,strstr(temp2+1,">")-1);
				if(!ttwjjc(temp2))
					touwenjian(temp2);
				free(temp2);
			}
	}while(1);
	free(text);
	return 0;
tiao:
	free(text);
	return -1;
}
//读取一个文件、每64字节是一个节点
char **fujian(char *name)
{
	int32 len,f;
	char **temp;
	len=mrc_getLen(name);
	if(len>0)
		if(f=mrc_open(name,1))
		{
			int i;
			temp=malloc(len/64*4);
			temp[0]=malloc(len);
			mrc_read(f,temp[0],len);
			for(i=1;i<len/64;i++)
				temp[i]=temp[0]+i*64;
			return temp;
		}
		return NULL;
}
//读取头配置信息
void mpr_tou(INI *inif)
{
	char temp[128];
	memset(temp,0,128);
	ini_get(inif,"information@filename",temp);
	memcpy(apptou.filename,temp,12);

	memset(temp,0,128);
	ini_get(inif,"information@appname",temp);
	memcpy(apptou.appname,temp,24);

	apptou.appid=ini_get_int(inif,"information@appid");
	apptou.version=ini_get_int(inif,"information@version");
	apptou.visible=ini_get_int(inif,"information@visible");

	memset(temp,0,128);
	ini_get(inif,"information@vendor",temp);
	memcpy(apptou.vendor,temp,40);

	memset(temp,0,128);
	ini_get(inif,"information@description",temp);
	memcpy(apptou.description,temp,64);
}
void mpr_ini(INI *inif)
{
	int tempint;
	
	tempint=ini_get_int(inif,"information@SCRW");
	if(tempint>0)
		SCREEN_WIDTH=tempint;
	
	tempint=ini_get_int(inif,"information@SCRH");
	if(tempint>0)
		SCREEN_HEIGHT=tempint;
	
	tempint=ini_get_int(inif,"information@HEAP_SIZE");
	if(tempint>0)
	HEAP_SIZE=tempint;
	tempint=ini_get_int(inif,"information@DEBUG");
	if(tempint>0)
	Debugts=tempint;


}
char config_z[10][64];
//读取输出信息
void mpr_config(INI *inif)
{
	char temp[128],temp2[128],*temp_p,*temp_p1;
	int i=0,ii=0,t_i=0,len=0,dou=',';

	memset(&config_z[0][0],0,640);

	memset(temp,0,128);
	ini_get(inif,"information@config",temp);
	//memcpy(apptou.description,temp,64);
	temp_p=mrc_strchr(temp,dou);
	temp_p1=temp;
	if(temp_p==NULL)
		strcpy(config_z[i++],temp);
	else
	{
		do 
		{

			memcpy(config_z[i],temp_p1,strlen(temp_p1)-strlen(temp_p));
			i++;
			if(temp_p[0]==',')
				temp_p++;
			temp_p1=temp_p;

		} while (temp_p=mrc_strchr(temp_p1,dou));
		temp_p=mrc_strrchr(temp,',')+1;
		memcpy(config_z[i++],temp_p,strlen(temp_p));
	}
	if(config_z[0][0])
	{
		for(ii=0;ii<i;ii++)
		{
			gjfile();
			sprintf(temp2,"information@output_%s",config_z[ii]);
			ini_get(inif,temp2,temp);
			c_mrpname=mallocchar(temp);

			sprintf(temp2,"information@file_%s",config_z[ii]);
			ini_get(inif,temp2,temp);
			c_file=mallocchar(temp);

			sprintf(temp2,"information@c_%s",config_z[ii]);
			ini_get(inif,temp2,temp);
			//c_mrpname=mallocchar(temp);
			includecharnode(temp);
			touwenjian(temp);
			PackgedMrp(c_file,c_mrpname);
		}
	}

}

int mpr(char *name)
{
	INI *inif;
	inif=ini_open(name,"information");
	if(inif==NULL)
		return -1;

	//读取头信息
	mpr_tou(inif);
	//读取配置信息
	mpr_ini(inif);
	//读取输出信息
	mpr_config(inif);

	ini_close(inif);
	return 0;
}

//获取外部mrp传来的信息
int setini()
{
	uint32 len,len2,ll=0,i=0;
	char *text=NULL,*a,*b,dou=',',end='\n',temp[64];
	int16 *yan;
#ifdef SIM_MOD
	int32 f;
	f=mrc_open("xqc/scr.tem",1);
	mrc_read(f,w_getScreenBuffer(),240*320*2);
	mrc_close(f);
#endif
	yan=(int16 *)w_getScreenBuffer();
	if(yan[0]*yan[1]+'p'==yan[2])
		text=(char *)(yan+3);
	if(text==NULL)
		return -1;

	if(text==NULL)
		return 0;
	len=strlen(text);


	i=ll=0;
	if((a=strstr(text,b="CFILE="))!=NULL)
	{
		int ii;
		a+=strlen(b);
		len2=len-(int)(a-text);
		do 
		{
			if(a[i++] == end)
				break;
			if(i==1||a[i]==dou)
			{
				ll++;
				switch(ll)
				{
				case 1:
					memset(temp,0,64);
					ii=(int)((char *)memchr(a,end,len2)-a);
					memcpy(temp,a,ii-1);
					c_file=mallocchar(temp);
					break;
				}
			}

		} while (i<len2);
	}

	i=ll=0;
	if((a=strstr(text,b="CFUJIA="))!=NULL)
	{
		int ii;
		a+=strlen(b);
		len2=len-(int)(a-text);
		do 
		{
			if(a[i++] == end)
				break;
			if(i==1||a[i]==dou)
			{
				ll++;
				switch(ll)
				{
				case 1:
					memset(temp,0,64);
					ii=(int)((char *)memchr(a,end,len2)-a);
					memcpy(temp,a,ii-1);
					c_fj=fujian(temp);
					break;
				}
			}

		} while (i<len2);
	}

	i=ll=0;
	if((a=strstr(text,b="CMRPNAME="))!=NULL)
	{
		int ii;
		a+=strlen(b);
		len2=len-(int)(a-text);
		do 
		{
			if(a[i++] == end)
				break;
			if(i==1||a[i]==dou)
			{
				ll++;
				switch(ll)
				{
				case 1:
					memset(temp,0,64);
					ii=(int)((char *)memchr(a,end,len2)-a);
					memcpy(temp,a,ii-1);
					c_mrpname=mallocchar(temp);
					break;
				}
			}

		} while (i<len2);
	}


	return 0;
}

int PackgedMrp(char *cfile,char *mrpname)/////////////打包运行时被调用
{
	AppListLink_p head;

	tishi("收集主程序文件");
	head=FindDataFile(cfile);
	tishi("收集资源文件");
	head=FindCharFile(head,&includechar[0]);
	tishi("开始打包");
	tishi("正在写入数据");
	CreateMrpFile(mrpname,head);
	tishi("写入完成");
	DelAllAppListLink(head);
	includecharfree();
	tishi("内存释放");
	head=NULL;
	tishi("打包完成");
	tishi("保存在：");
	tishi(mrpname);
	return 0;
}

int32 mrc_init(void)
{
	char aaa[123456];
	int32 i,RE=0;//setini();
	for(i=includechar_i=0;i<50;includechar[i++]=NULL);
	HEAP_SIZE=128*1024;
	SCREEN_WIDTH=SCREEN_HEIGHT=Debugts=0;
	memset(aaa,0,123456);
	c_file="c/temp";c_fj=NULL;c_mrpname="pack.mrp";
	includechar_i=0;
	memset(&apptou,0,sizeof(AppTou));
	mpr("测试.mpr");

	//PackgedMrp(c_file,c_fj,c_mrpname);
	return RE;

}

int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	return 0;
}

int32 mrc_pause()

{

	return MR_IGNORE;

}



int32 mrc_resume()

{

	return MR_IGNORE;

}



int32 mrc_exitApp()

{

	return MR_SUCCESS;

}