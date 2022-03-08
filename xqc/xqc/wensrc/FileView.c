#include "mrc_base.h"
#include "mrc_exb.h"
#include "wenexp.h"

typedef struct Filelistnode {
	struct Filelistnode *pre;
	struct Filelistnode *next;
	char *text;
	int filestate;
}FileListLink,*FileListLink_p;


enum Windows{
	EDITOR,//编辑器
	MAINMENU,//主菜单
	FILEVIEW,//文件浏览
	SETING//设置
};

extern enum Windows MainWindow;
extern char *WorkFile;

static void output(char *text)
{char * mrc_strrchr (
					 const char * string,
					 int32 ch
					 );
	int32 hand=0;
	hand=mrc_open("print.txt",8|2);
	mrc_seek(hand,0,MR_SEEK_END);
	mrc_write(hand,text,mrc_strlen(text));
	mrc_close(hand);
}
int32 MenuEvent(int32 code,int32 p1,int32 p2);
void DrawMainMenu(void);
void DrawFileMenu(void);
void LoadMenu(int data);
int32 FileFreeAll(void);
int SaveFile();
void EditFile(char *dir,char *name);
#define FILENAMELEN 256
//////////////////全局量//////////////////////
extern mr_colourSt BackCol,FontCol,FocusCol,FocusFontCol,OtherCol;//外联,各种颜色
extern int SCRW,SCRH;

static int32 FileKeyTimer=0,KeyTimerLoop=0;
char *ScanfPath=NULL;
///////////////////功能/////////////////////
FileListLink_p FileListHead=NULL,
          FileFristNode=NULL,
		  FileFocusNode=NULL
		  ;
int FileShowNum=10,FileShowSpace=16,FileMenuShow=0;
FileListLink_p FileMakeNode(char *text,int filestate)//制造结点
{
	FileListLink_p temp;
	temp=(FileListLink_p )mrc_malloc(sizeof(FileListLink));
	if(temp==NULL)
	{
		output("制造节点时内存不足\n");
		return NULL;
	}
	temp->next=NULL;
	temp->pre=NULL;
	temp->text=(char *)mrc_malloc(mrc_strlen(text)+1);
	temp->text[mrc_strlen(text)]=0;
	if(temp->text==NULL)
	{
		output("制造节点复制字符串时内存不足\n");
		mrc_free(temp);
		return NULL;
	}
	mrc_strcpy(temp->text,text);
	temp->filestate=filestate;
	return temp;
}
FileListLink_p InsertFileListNode(FileListLink_p pre,FileListLink_p node)//插入结点
{
	FileListLink_p temp=NULL,next=NULL;
	if(pre==NULL)
		return node;
	next=pre->next;
	if(node==NULL)
		return pre;
	pre->next=node;
	node->pre=pre;
	node->next=next;
	if(next!=NULL)
		next->pre=node;
	return NULL;
}
FileListLink_p DelAFileListNode(FileListLink_p head,FileListLink_p node)//删除一个
{
	FileListLink_p pre=NULL,next=NULL;
	if(head==NULL||node==NULL)
		return head;
	if(node==head)
	{
		next=node->next;
		if(node->text!=NULL)
			mrc_free(node->text);
		mrc_free(node);
		if(next!=NULL)
			next->pre=NULL;
		return next;
	}
	pre=node->pre;
	next=node->next;
	if(pre!=NULL)
		pre->next=next;
	if(next!=NULL)
		next->pre=pre;
	if(node->text!=NULL)
		mrc_free(node->text);
	mrc_free(node);
	return head;
}
void DelAllFileListNode(FileListLink_p head)//删除全部
{
	if(head==NULL)
		return ;
	while(head->next!=NULL)
		head=DelAFileListNode(head,head->next);
	head=DelAFileListNode(head,head);
}
FileListLink_p FileGetEndNode(FileListLink_p head)//通过一个结点获取尾结点
{
	if(head==NULL)
		return NULL;
	while(head->next!=NULL)
		head=head->next;
	return head;
}
FileListLink_p FileGetFristNode(FileListLink_p node)//通过一个结点获取head结点
{
	if(node==NULL)
		return NULL;
	while(node->pre!=NULL)
		node=node->pre;
	return node;
}
FileListLink_p FileGetNextNNode(FileListLink_p node,int n)
{
	FileListLink_p temp=NULL;
	int i=0;
	if(node==NULL)
		return NULL;
	temp=node;
	for(;i<n;i++)
	{
		if(temp->next==NULL)
			break;
		temp=temp->next;
	}
	/**
	if(i<n)
		return node;
		*/
	return temp;
}
FileListLink_p FileGetPreNNode(FileListLink_p node,int n)
{
	FileListLink_p temp=NULL;
	int i=0;
	if(node==NULL)
		return NULL;
	temp=node;
	for(;i<n;i++)
	{
		if(temp->pre==NULL)
			break;
		temp=temp->pre;
	}/*
	if(i<n)
		return node;*/
	return temp;
}

FileListLink_p FileScanfPath(char *path)
{
	FileListLink_p dir=NULL,dirend=NULL,file=NULL,fileend=NULL,temp=NULL;
	int32 filefindhand=0,pathlen;
	char *text,name[FILENAMELEN+2];
	name[0]=0;
	pathlen=mrc_strlen(path);
	text=(char *)mrc_malloc(pathlen+FILENAMELEN+2);
	if(text==NULL)
	{
		return NULL;
	}
	text[pathlen+FILENAMELEN]=0;
	text[pathlen+FILENAMELEN+1]=0;
	filefindhand=mrc_findStart(path,name,FILENAMELEN);
	mrc_findGetNext(filefindhand,name,FILENAMELEN);
	while(mrc_findGetNext(filefindhand,name,FILENAMELEN)==MR_SUCCESS)
	{
		mrc_sprintf(text,"%s/%s",path,name);
		switch(mrc_fileState(text))
		{
		case MR_IS_FILE:
			if(file==NULL||fileend==NULL)
			{
				file=FileMakeNode(name, MR_IS_FILE);
				fileend=file;
			}
			else
			{
				InsertFileListNode(fileend,FileMakeNode(name,MR_IS_FILE));
				if(fileend->next!=NULL)
					fileend=fileend->next;
			}
			break;
		case MR_IS_DIR:
			if(dir==NULL||dirend==NULL)
			{
				dir=FileMakeNode(name, MR_IS_DIR);
				dirend=dir;
			}
			else
			{
				InsertFileListNode(dirend,FileMakeNode(name,MR_IS_DIR));
				if(dirend->next!=NULL)
					dirend=dirend->next;
			}
			break;
			break;
		case MR_IS_INVALID:
			break;
		}
	}
	mrc_free(text);
	if(dirend!=NULL)
	{
		dirend->next=file;
		if(file!=NULL)
			file->pre=dirend;
		return dir;
	}
	else
	{
		return file;
	}
}


int FileGetTwoNodeLen(FileListLink_p pre,FileListLink_p next)//获取两个结点间的距离
{
	int i=0;
	if(pre==NULL||next==NULL)
		return -1;
	for(i=0;pre!=NULL;i++)
	{
		if(pre==next)
			break;
		pre=pre->next;
	}
	if(pre==NULL)
		return -1;
	return i;
}

static void UDyidongyueshu()//上下移移约束
{
	int twonodelen=-1,i=0;
	if(FileFristNode==NULL)
		return ;
	if(FileFristNode->pre==FileFocusNode)//加快速度
		twonodelen=-1;
	else
		twonodelen=FileGetTwoNodeLen(FileFristNode,FileFocusNode);
	if(twonodelen>=FileShowNum)//该下移了
		if(FileFristNode->next!=NULL)
		{
			FileFristNode=FileGetPreNNode(FileFocusNode,FileShowNum-1);
		}
	if(twonodelen<0)//该上移了
		if(FileFristNode->next!=NULL)
		{
			FileFristNode=FileFocusNode;
		}
}
void FileUp()//光标上移
{
	if(FileFocusNode!=NULL)
	if(FileFocusNode->pre!=NULL)
	{
		FileFocusNode=FileFocusNode->pre;
		UDyidongyueshu();
	}
	else
	{
		FileFocusNode=FileGetEndNode(FileFocusNode);
		UDyidongyueshu();
	}
}
void FileLeft()
{
	if(FileFocusNode!=NULL)
	{
		FileFocusNode=FileGetPreNNode(FileFocusNode,FileShowNum-1);
		UDyidongyueshu();
	}
}
void FileDown()//光标下移
{
	if(FileFocusNode!=NULL)
	if(FileFocusNode->next!=NULL)
	{
		FileFocusNode=FileFocusNode->next;
		UDyidongyueshu();
	}
	else
	{
		FileFocusNode=FileGetFristNode(FileFocusNode);
		UDyidongyueshu();
	}
}
void FileRight()
{
	if(FileFocusNode!=NULL)
	{
		FileFocusNode=FileGetNextNNode(FileFocusNode,FileShowNum-1);
		UDyidongyueshu();
	}
}

int OpenDir(char *path,char *name)
{
	int tlen=0;
	char *temp=NULL;
	temp=(char*)mrc_malloc(mrc_strlen(path)+mrc_strlen(name)+3);
	temp[0]=0;
	if(temp==NULL)
		return -1;
	mrc_strcpy(temp,path);
	mrc_strcat(temp,"/");
	mrc_strcat(temp,name);
	DelAllFileListNode(FileListHead);
	FileListHead=NULL;
	FileFristNode=NULL;
	FileFocusNode=NULL;
	FileListHead=FileScanfPath(temp);
	FileFristNode=FileListHead;
	FileFocusNode=FileListHead;
	if(ScanfPath!=NULL)
		mrc_free(ScanfPath);
	ScanfPath=temp;
	tlen=mrc_strlen(ScanfPath);
	if(tlen>0&&ScanfPath[tlen-1]=='/')
		ScanfPath[tlen-1]=0;
	return 0;
}

//画文件列表
void DrawFileView(int data)
{
	int i=0,w,h;
	FileListLink_p temp;
	temp=FileFristNode;
	mrc_drawRect(0,0,SCRW,SCRH,BackCol.r,BackCol.g,BackCol.b );
	mrc_drawRect(0,0,SCRW,FileShowSpace,OtherCol.r,OtherCol.g,OtherCol.b );
	if(ScanfPath)
		gbsez(ScanfPath,5,0,FontCol.r,FontCol.g,FontCol.b,0,FONT);
	for(i=1;i<=FileShowNum;i++)
	{
		if(temp==NULL)
			break;
		if(temp==FileFocusNode)
			mrc_drawRect(0,i*FileShowSpace,SCRW,FileShowSpace,FocusCol.r,FocusCol.g,FocusCol.b );
		if(temp->filestate==MR_IS_DIR)
			gbsez(temp->text,5,i*FileShowSpace,FontCol.r,FontCol.g,FontCol.b,0,FONT);
		if(temp->filestate==MR_IS_FILE)
			gbsez(temp->text,5,i*FileShowSpace,FocusFontCol.r,FocusFontCol.g,FocusFontCol.b,0,FONT);
		temp=temp->next;
	}
	mrc_drawRect(0,SCRH-FileShowSpace,SCRW,FileShowSpace,OtherCol.r,OtherCol.g,OtherCol.b );
	gbsez("菜单",5,SCRH-FileShowSpace,FontCol.r,FontCol.g,FontCol.b,0,FONT);
	fontwh("返回",0,FONT,(int32*)&w,(int32*)&h);
	gbsez("返回",SCRW-w-5,SCRH-FileShowSpace,FontCol.r,FontCol.g,FontCol.b,0,FONT);
	if(data==0)
		mrc_refreshScreen(0,0,SCRW,SCRH);
}
int FileSelect()
{
	if(FileFocusNode==NULL)
	{
		return -1;
		DrawFileView(0);
		return 0;
	}
	if(FileFocusNode->filestate==MR_IS_DIR)
	{
		OpenDir(ScanfPath,FileFocusNode->text);
		DrawFileView(0);
		return 0;
	}
	if(FileFocusNode->filestate==MR_IS_FILE)
	{
		EditFile(ScanfPath,FileFocusNode->text);
		EditLoadFile(WorkFile);

		FileFreeAll();
		MainWindow=EDITOR;
		mrc_resume();
		return 0;
	}
	return 0;
}
enum{
	g_c,
	g_txt,
	g_ini,
	g_end
};
char *reg_ge(int ge)
{
	switch(ge)
	{
	case g_c:return "c";
	case g_txt:return "txt";
	case g_ini:return "ini";
	}
	return NULL;
}
int bihou(char *name)
{
	int i;
	name=mrc_strrchr(name,'.');
	if(name==NULL)
		return -1;
	name++;
	for(i=0;i<g_end;i++)
		if(!strcmp(name,reg_ge(i)))
			return i;
	return -1;
}
extern void setupini(int32 data);

int FileRun()
{
	if(FileFocusNode==NULL)
	{
		return -1;
		DrawFileView(0);
		return 0;
	}
	if(FileFocusNode->filestate==MR_IS_DIR)
	{
		return -1;
	}
	if(FileFocusNode->filestate==MR_IS_FILE)
	{
		switch(bihou(FileFocusNode->text))
		{
		case g_c:
			EditFile(ScanfPath,FileFocusNode->text);
			setupini(0);
			return 0;
		case g_txt:
		case g_ini:
	EditFile(ScanfPath,FileFocusNode->text);
		EditLoadFile(WorkFile);

		FileFreeAll();
		MainWindow=EDITOR;
		mrc_resume();
		return 0;
		}
	}
	return -1;
}

int FileBack()
{
	int textlen=0,pos=0;
	char *en=NULL;
	if(ScanfPath==NULL)
	{
		FileFreeAll();
		MainWindow=EDITOR;
		mrc_resume();
		return 0;
	}
	textlen=mrc_strlen(ScanfPath);
	if(textlen==0)
	{
		FileFreeAll();
		MainWindow=EDITOR;
		mrc_resume();
		return 0;
	}
	en=mrc_strrchr(ScanfPath,'/');
	if(en==NULL)
	{
		OpenDir("","");
		DrawFileView(0);
		return 0;
	}
	else
	{
		en[0]=0;
		OpenDir(ScanfPath,"");
		DrawFileView(0);
	}
	return 0;
}
int32 FileViewInit()
{
	int32 w;
	FileKeyTimer=mrc_timerCreate();
	fontwh("鼎qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM",0,FONT,(int32*)&w,(int32*)&FileShowSpace);
	FileShowNum=SCRH/FileShowSpace-2;
	FileMenuShow=0;
	OpenDir("c","");
	DrawFileView(0);
	return 0;
}
int32 FileViewEvent(int32 code,int32 p1,int32 p2);
void FileKeyLoop(int32 data)
{
	KeyTimerLoop++;
	if(KeyTimerLoop==0||KeyTimerLoop>5)
		FileViewEvent(MR_KEY_PRESS,data,0x7fffffff);
}
static void KeyLongPress(int32 p1,int32 p2)
{
	if(p2!=0x7fffffff)
		KeyTimerLoop=0;
	mrc_timerStart(FileKeyTimer,50,p1,(mrc_timerCB)FileKeyLoop,1);
}
static void KeyFree()
{
	mrc_timerStop(FileKeyTimer);
}
int32 FileViewEvent(int32 code,int32 p1,int32 p2)
{
	if(FileMenuShow==0)
	{
	if(code==MR_KEY_PRESS)
	{
		switch(p1)
		{
		case MR_KEY_DOWN:
			FileDown();
			break;
		case MR_KEY_LEFT:
			FileLeft();
			break;
		case MR_KEY_UP:
			FileUp();
			break;
		case MR_KEY_RIGHT:
			FileRight();
			break;
		}
		DrawFileView(0);
		KeyLongPress(p1,p2);
	}
		if(code==MR_KEY_RELEASE)
		{
			KeyFree();
			if(p1==MR_KEY_SOFTLEFT)
			{
				mrc_pause();
				FileMenuShow=1;//菜单打开
				LoadMenu(1);
				DrawFileMenu();
				//mrc_resume();
				return 0;
			}
			if(MR_KEY_SELECT==p1)
			{
				FileSelect();
			}
			if(MR_KEY_SOFTRIGHT==p1)
			{
				FileBack();
			}
		}
	}
	else
		return MenuEvent(code,p1,p2);
	return 0;
}
int32 FileResume()
{
	if(FileMenuShow==0)
		DrawFileView(0);
	else
		DrawFileMenu();
	return 0;
}
int32 FileFreeAll()
{
	if(FileKeyTimer)
		mrc_timerDelete(FileKeyTimer);
	FileKeyTimer=0;
	DelAllFileListNode(FileListHead);
	FileListHead=NULL;
	FileFristNode=NULL;
	FileFocusNode=NULL;
	if(ScanfPath!=NULL)
		mrc_free(ScanfPath);
	ScanfPath=NULL;
	return 0;
}