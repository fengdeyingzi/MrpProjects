#include "mrc_base.h"
#include "mrc_exb.h"
#include "libini.h"

#define FILESTARTPOS 240 //mrp包文件，文件信息流开始位置
#define FILENAMELEN 128 //最大文件名长度
int32 FileLen;//文件总长度
int32 HeadLen;//文件头长度

void (*print_func)(void *opaque, const char *msg);
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
	char projectname[128];
	char filename[128];
	char appname[128];
	int appid;
	int version;
	int visible;
	int cpu;
	char vendor[128];
	char output[128];
	char description[128];
	char include[5][128];
	char config[5][128];

}AppTou,*AppTou_p;
AppTou apptou;
// 设置信息输出函数
void pack_set_print_func(void (*print_funcs)(void *opaque, const char *msg))
{
	print_func=print_funcs;
}
int tsy=0;
void tishi(char *text,int l)
{
	if(print_func)
		print_func(NULL,text);
}
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
	mrc_write(hand,"MRPG",4);
	mrc_write(hand,&HeadLen,4);
	mrc_write(hand,&FileLen,4);
	mrc_write(hand,&listpos,4);
	mrc_seek(hand,16,MR_SEEK_SET);
	mrc_write(hand,apptou.filename,11);
	//mrc_seek(hand,28,MR_SEEK_SET);
	//mrc_write(hand,apptou.visible,24);
	mrc_seek(hand,28,MR_SEEK_SET);
	if(apptou.appname[0]==0)
		strcpy(apptou.appname,"我的程序");
	mrc_write(hand,apptou.appname,16);
	mrc_seek(hand,68,MR_SEEK_SET);
	mrc_write(hand,&apptou.appid,4);
	mrc_write(hand,&apptou.version,4);
	mrc_seek(hand,88,MR_SEEK_SET);
	mrc_write(hand,apptou.vendor,38);
	mrc_seek(hand,128,MR_SEEK_SET);
	mrc_write(hand,apptou.description,60);

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
		tishi(temp->filename,0);
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
AppListLink_p  FindDataFile(char *path)
{
	AppListLink_p head;
	char temppath[FILENAMELEN*2];
	char data[FILENAMELEN+1];
	int32 findhand;
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
AppListLink_p  FindCharFile(AppListLink_p  head,char **path)
{
	int i=0;
	if(path==NULL)
		return head;
	while(path[i])
		InsertAppListText(head,path[i++]);

	return head;
}


void mpr_tou(INI *pini)
{
	ini_get(pini,"information@projectname",apptou.projectname);
	ini_get(pini,"information@filename",apptou.filename);
	ini_get(pini,"information@appname",apptou.appname);
	apptou.appid = ini_get_int(pini,"information@appid");
	apptou.version = ini_get_int(pini,"information@version");
	apptou.visible = ini_get_int(pini,"information@visible");
	apptou.cpu = ini_get_int(pini,"information@cpu");
	ini_get(pini,"information@vendor",apptou.vendor);
	ini_get(pini,"information@output",apptou.output);
	ini_get(pini,"information@description",apptou.description);
}


int PackgedMrp(char *cfile,char **fj,char *mrpname)/////////////打包运行时被调用
{
	AppListLink_p head;
	head=MakeAppListNode("head");

	tishi("收集主程序文件",1);
	if(cfile)
		head=FindDataFile(cfile);
	tishi("收集资源文件",1);
	head=FindCharFile(head,fj);
	tishi("开始打包",1);
	tishi("正在写入数据",1);
	CreateMrpFile(mrpname,head);
	tishi("写入完成",1);
	DelAllAppListLink(head);
	tishi("内存释放",1);
	head=NULL;
	tishi("打包完成",1);
	tishi("保存在：",1);
	tishi(mrpname,1);
	return 0;
}
