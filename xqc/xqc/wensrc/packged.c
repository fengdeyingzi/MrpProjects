#include "mrc_base.h"
////////////////////////////望尘11制作//////////////////////////////
///////////////////////////q:1012371864/////////////////////////////

#define FILESTARTPOS 240 //mrp包文件，文件信息流开始位置
#define FILENAMELEN 255 //最大文件名长度
int32 FileLen;//文件总长度
int32 HeadLen;//文件头长度

/////////////////////////待打包文件链表//////////////////////////////////
typedef struct applistnode {
	struct applistnode *pre;
	struct applistnode *next;
	int32 namepos;
	int32 namelen;
	char *filename;
	int32 datapos;
	int32 datalen;
}AppListLink,*AppListLink_p;

#ifndef SIM_MOD

////设置文件列表所有信息、、文件偏移，大小，文件名长度，等
int32 SetAppListInfo(AppListLink_p head)
{
	char path[1024];
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
		mrc_memset(path,0,1024);
		mrc_sprintf(path,"%s/%s","",temp->filename);
		temp->datalen=mrc_getLen(path);
		temp->namelen=mrc_strlen(temp->filename)+1+16;
		temp->namepos=tempnamepos;
		tempnamepos+=temp->namelen;
		temp=temp->next;
	}
	temp=head;
	tempdatapos=tempnamepos;
	while(temp!=NULL)
	{
		tempdatapos+=mrc_strlen(temp->filename)+1+8;
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
	mrc_seek(hand,28,MR_SEEK_SET);
	mrc_write(hand,"PACK",4);
	return 0;
}
//向mrp包写入文件信息
int32 WriteFileInfo(AppListLink_p node,char *filename)
{
	int32 templen;
	int32 writehand;
	writehand=mrc_open(filename,2);
	mrc_seek(writehand,node->namepos,MR_SEEK_SET);
	templen=mrc_strlen(node->filename)+1;
	mrc_write(writehand,&templen,4);
	mrc_write(writehand,node->filename,templen);
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
	char path[1024];
	mrc_sprintf(path,"%s/%s","",node->filename);
	data=mrc_malloc(node->datalen);
	datahand=mrc_open(path,1);
	if(datahand==0)
	{
		mrc_free(data);
		return -1;
	}
	mrc_read(datahand,data,node->datalen);
	mrc_close(datahand);
	writehand=mrc_open(filename,2);
	mrc_seek(writehand,node->datapos-mrc_strlen(node->filename)-1-8,MR_SEEK_SET);
	templen=mrc_strlen(node->filename)+1;
	mrc_write(writehand,&templen,4);
	mrc_write(writehand,node->filename,mrc_strlen(node->filename)+1);
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
	mrc_memset(temp->filename,0,mrc_strlen(filename)+1);
	mrc_strcpy(temp->filename,filename);
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
//搜索一个目录，制造一条待打包文件的链表
AppListLink_p  FindDataFile(char *path)
{
	AppListLink_p head;
	char temppath[256+FILENAMELEN*2];
	char savepath[256+FILENAMELEN*2];
	char data[FILENAMELEN+1];
	int32 findhand;
	head=MakeAppListNode("head");
	findhand=mrc_findStart(path,data,FILENAMELEN);
	if(findhand==0)
		return NULL;
		temppath[0]=0;
		savepath[0]=0;
		mrc_sprintf(temppath,"%s/%s",path,data);
		if(mrc_fileState(temppath)==MR_IS_FILE)
		{
			InsertAppListNode(head,MakeAppListNode(data));
		}
	while(mrc_findGetNext(findhand,data,FILENAMELEN)==0)
	{
		temppath[0]=0;
		savepath[0]=0;
		mrc_sprintf(temppath,"%s/%s",path,data);
		if(mrc_fileState(temppath)==MR_IS_FILE)
		{
			InsertAppListNode(head,MakeAppListNode(data));
		}
	}
	mrc_findStop(findhand);
	return head;
}
#endif

int PackgedDirMrp(char *cfile,char *mrpname)/////////////打包运行时被调用
{
	AppListLink_p head;
	head=FindDataFile(cfile);
	CreateMrpFile(mrpname,head);
	DelAllAppListLink(head);
	head=NULL;
	return 0;
}
/*
/////////////////////////////////////////////////////////////
int32 mrc_init(void)
{
	AppListLink_p head;
	head=FindDataFile(PACKPATH);
	CreateMrpFile("MyPack.mrp",head);
	DelAllAppListLink(head);
	head=NULL;
	mrc_clearScreen(255,255,255);
	gbsez("打包完成",0,0,0,0,0,0,0);
	mrc_refreshScreen(0,0,240,320);
	return MR_SUCCESS;
}

int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
	mrc_exit();
	return MR_SUCCESS;
}

int32 mrc_pause(void)
{
	return 0;
}

int32 mrc_resume(void)
{
	return 0;
}

int32 mrc_exitApp(void)
{
	return 0;
}*/