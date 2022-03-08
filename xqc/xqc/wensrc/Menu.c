#include "mrc_base.h"
#include "mrc_bmp.h"
#include "wenexp.h"
extern mr_colourSt BackCol,FontCol,FocusCol,FocusFontCol,OtherCol;//外联,各种颜色
extern int SCRW,SCRH,PACKGEDMRP;
#define MUNUTEXTLEN 20
typedef struct MenuListSt{//菜单结构
	int menuid;
	char text[MUNUTEXTLEN+2];
	int x;
	int y;
	int w;
	int h;
	//struct MenuListSt *father;//父菜单，已废弃
	struct MenuListSt *child;//字菜单
	struct MenuListSt *pre;
	struct MenuListSt *next;
} MenuList,*MenuList_p;
typedef void (*MenuCallBack)(int32 data);
#define MENUMAX 5 //菜单最高级数


enum Windows{
	EDITOR,//编辑器
	MAINMENU,//主菜单
	FILEVIEW,//文件浏览
	SETING//设置
};

extern enum Windows MainWindow;

typedef struct MenuHandSt{
	MenuList_p head;//头
	MenuList_p focus[MENUMAX];//焦点菜单
	MenuCallBack callback;//会叫
	MenuCallBack background;//背景
}MenuHand,*MenuHand_p;

enum {
	INSERT,
	GOTOLINE,
	RENAME
};

///////////////////公用接口/////////////////
int32  EditPause(void);
int32 EditResume(void);
int32 EditorInit(void);//初始化
int32 EditFreeAll(void);//释放所有
int32 EditEvent(int32 code, int32 p1, int32 p2);
int32 EditSaveToFile(char *name);
int32 EditLoadFile(char *filename);//会自动释放之前的head
int32 EditInsertText(char *text);//在光标处插入一段文字y
void SetEditorListShow(int x,int y,int h);

int32 EditSetCopyStart(void);//设置起点
int32 CleanSelect(void);
int32 EditSetCopyEnd(void);//设置终点
int32 CleanSelect(void);
int32 CopyStrText(void);
int32 PasteStrText(void);
int32 GotoLine(int line);
void DrawFileView(int data);

int32 FileViewInit(void);
int32 MenuResume(void);
int SaveFile(void);
void EditFile(char *dir,char *name);
int CreateEdit(int data,char *text);
////////////////////////////////////////////
void CallBack(int id);
void ReSelectMenu(MenuHand *menuhand);
void SelectMenu(MenuHand *menuhand);
extern void DrawEditor(int data);
void DrawFileView(int data);

int PackgedMrp(char *cfile,char *mrpname);
void FileMenuCallBack(int id);

static int32 MenuKeyTimer=0,KeyTimerLoop=0;
MenuHand *MainMenu,EditMenu,FileMenu;

extern int FileMenuShow;
extern char *WorkFile;

MenuList_p MakeMenuNode(char *text,int id)//创建一个结点
{
	MenuList_p temp=mrc_malloc(sizeof(MenuList));
	mrc_memset(temp,0,sizeof(MenuList));
	if(text==NULL)
		return NULL;
	mrc_strncpy(temp->text,text,MUNUTEXTLEN);
	temp->menuid=id;
	return temp;
}
MenuList_p GetMenuEndNode(MenuList_p head)
{
	if(head==NULL)
		return NULL;
	while(head->next!=NULL)
	{
		head=head->next;
	}
	return head;
}
MenuList_p AddMenu(MenuList_p head,char *text,int id)//添加一个菜单项
{
	MenuList_p temp=NULL,end=NULL;
	temp=MakeMenuNode(text,id);
	if(head==NULL)
		return temp;
	else
	{
		end=GetMenuEndNode(head);
		end->next=temp;
		if(temp)
			temp->pre=end;
	}
	return head;
}

MenuList_p FindMenuById(MenuList_p head,int id)//递归查找id
{
	MenuList_p temp=NULL;
	while(head)
	{
		if(head->menuid==id)
			return head;
		if(head->child!=NULL)
		{
			temp=FindMenuById(head->child,id);
			if(temp!=NULL)
				return temp;
		}
		head=head->next;
	}
	return NULL;
}

MenuList_p GetMenuFristNode(MenuList_p node)
{
	while(node)
	{
		if(node->pre==NULL)
			break;
		node=node->pre;
	}
	return node;
}
void DelMenuAll(MenuList_p head)//递归删除整个菜单表
{
	MenuList_p temp=NULL;
	if(head==NULL)
		return ;
	while(head)
	{
		if(head->child!=NULL)
			 DelMenuAll(head->child);
		temp=head;
		head=head->next;
		mrc_free(temp);
	}
}

void MenuInit()
{
	MenuList_p temp=NULL;
	temp=AddMenu(temp,"插入",1);
	temp=AddMenu(temp,"跳转",6);
	temp=AddMenu(temp,"导航",2);
	temp=AddMenu(temp,"文件",3);
	temp=AddMenu(temp,"运行",4);
	temp=AddMenu(temp,"退出",5);
	EditMenu.head=temp;
	temp=NULL;
	temp=AddMenu(temp,"打开",31);
	temp=AddMenu(temp,"保存",32);
	temp=AddMenu(temp,"另存为",33);
	temp=AddMenu(temp,"打包",34);
	FindMenuById(EditMenu.head,3)->child=temp;
	temp=NULL;
	temp=AddMenu(temp,"设置起点",21);
	temp=AddMenu(temp,"设置终点",22);
	temp=AddMenu(temp,"复制",23);
	temp=AddMenu(temp,"粘贴",24);
	temp=AddMenu(temp,"取消选择",25);
	FindMenuById(EditMenu.head,2)->child=temp;
	ReSelectMenu(&EditMenu);
	EditMenu.callback=(MenuCallBack)CallBack;
	EditMenu.background=(MenuCallBack)DrawEditor;

	temp=NULL;
	temp=AddMenu(temp,"打开",1);
	temp=AddMenu(temp,"文件",2);
	temp=AddMenu(temp,"打包",3);
	temp=AddMenu(temp,"返回",4);
	FileMenu.head =temp;
	temp=NULL;
	temp=AddMenu(temp,"新建",21);
	temp=AddMenu(temp,"重命名",22);
	temp=AddMenu(temp,"删除",23);
	temp=AddMenu(temp,"运行",24);
	FindMenuById(FileMenu.head,2)->child=temp;

	ReSelectMenu(&FileMenu);
	FileMenu.callback=(MenuCallBack)FileMenuCallBack;
	FileMenu.background=(MenuCallBack)DrawFileView;

	MainMenu=&EditMenu;
	MenuKeyTimer=mrc_timerCreate();
}
void MenuFreeAll()
{
	if(MenuKeyTimer)
		mrc_timerDelete(MenuKeyTimer);
	MenuKeyTimer=0;
	DelMenuAll(EditMenu.head);
	mrc_memset(&EditMenu,0,sizeof(EditMenu));
	DelMenuAll(FileMenu.head);
	mrc_memset(&FileMenu,0,sizeof(FileMenu));
}

int MenuListWH(MenuList_p head,MenuList_p focus,int32 *showw,int32 *showh,int32 *focush)//判断一个菜单显示所占得宽度，以及是否可以显示
{
	int32 w=0,h=0,maxw=0,maxh=0,i=0,sign=-1;
	MenuList_p temp=NULL;
	*showw=0;
	*showh=0;
	*focush=0;
	if(focus==NULL||head==NULL)
		return -1;
	temp=head;
	for(i=0;temp!=NULL;i++,temp=temp->next)
	{
		if(temp==focus)
			sign=i;//焦点存在可以画菜单
		fontwh(temp->text,0,FONT,&w,&h);
		if(w>maxw)
			maxw=w;
		if(h>maxh)
			maxh=h;
	}
	*showw=maxw;
	*showh=maxh*i;
	*focush=sign*maxh;
	if(sign==-1)
		return -1;
	return 0;
}
void DrawMenuList(MenuList_p head,MenuList_p focus,int x,int y)
{
	int w=0,h=0,i=0,maxw=0,maxh=0,sign=0,textw,texth;
	MenuList_p temp;
	temp=head;
	if(focus==NULL||head==NULL)
		return ;
	for(i=0;temp!=NULL;i++,temp=temp->next)
	{
		if(temp==focus)
			sign=1;//焦点存在可以画菜单
		fontwh(temp->text,0,FONT,(int32*)&w,(int32*)&h);
		if(w>maxw)
			maxw=w;
		if(h>maxh)
			maxh=h;
	}
	temp=head;
	fontwh(">",0,FONT,(int32*)&textw,(int32*)&texth);
	for(i=0;temp!=NULL;i++,temp=temp->next)
	{
		if(temp==focus)
			mrc_drawRect(x,y+i*maxh,maxw,maxh,FocusCol.r,FocusCol.g,FocusCol.b);
		else
			mrc_drawRect(x,y+i*maxh,maxw,maxh,BackCol.r,BackCol.g,BackCol.b);
			mrc_drawRect(x+maxw,y+i*maxh,textw,maxh,FocusCol.r,FocusCol.g,FocusCol.b);
			gbsez(temp->text,x,y+i*maxh,FontCol.r,FontCol.g,FontCol.b,0,FONT);
		if(temp->child!=NULL)
			gbsez(">",x+maxw,y+i*maxh,FontCol.r,FontCol.g,FontCol.b,0,FONT);
	}
	mrc_drawLine(x+1,y-1,x+maxw+textw-1,y-1,OtherCol.r,OtherCol.g,OtherCol.b);
	mrc_drawLine(x,y,x,y+maxh*i-1,OtherCol.r,OtherCol.g,OtherCol.b);
	mrc_drawLine(x+maxw+textw,y,x+maxw+textw,y+maxh*i-1,OtherCol.r,OtherCol.g,OtherCol.b);
	mrc_drawLine(x+1,y+maxh*i,x+maxw+textw-1,y+maxh*i,OtherCol.r,OtherCol.g,OtherCol.b);
}
void DrawMenu(MenuHand menuhand)
{
	int32 showw,showh,i,x,y,w,h,focush=0;
	MenuList_p temp;
	temp = menuhand.head;
	fontwh("高",0,FONT,&w,&h);
	x=2;
	y=SCRH-h;
	for (i=0;i<MENUMAX;i++)
	{
		if(menuhand.focus[i]==NULL)
			break;
		else
		{
			if(MenuListWH(temp,menuhand.focus[i],&showw,&showh,&focush)==0)
			{
				if(x+showw>SCRW-10)
					x=SCRW-showw-10;
				if(y+showh>SCRH-h)
					y=SCRH-showh-h;
				DrawMenuList(temp,menuhand.focus[i],x,y);
				temp->x=x;
				temp->y=y;
				temp->w=showw;
				temp->h=showh;
				temp=menuhand.focus[i]->child;
				x+=showw;
				y=y+focush;
				continue ;
			}
			else
				break;
		}
	}
}
void DrawMainMenu()
{
	EditMenu.background(1);
	DrawMenu(EditMenu);
	mrc_refreshScreen(0,0,SCRW,SCRH);
}
void DrawFileMenu()
{
	FileMenu.background(1);
	DrawMenu(FileMenu);
	mrc_refreshScreen(0,0,SCRW,SCRH);
}
void Pack()
{
	//char mrp[1024];
	//mrp[0]=0;
	//mrc_sprintf(mrp,"%s.mrp",WorkFile);
	if(WorkFile!=NULL)
	{
		if(mrc_strlen(WorkFile)>0)
		{
			PackgedMrp(WorkFile,"pack.mrp");
		}
	}
}
int setuptext(char *text)
{
	char temp[128];
	sprintf(temp,"MAINNAME=%s\r\n",WorkFile);
	strcat(text,temp);

	sprintf(temp,"HEAP_SIZE=%d\n",256);
	strcat(text,temp);

	sprintf(temp,"SCRWH=%d,%d\n",0,0);
	strcat(text,temp);

	sprintf(temp,"DEBUG=%d\n",1);
	strcat(text,temp);

	if(PACKGEDMRP)
	{
		sprintf(temp,"PACKGEDMRP=%d\n",PACKGEDMRP);
		strcat(text,temp);
	}

	return 0;
}
extern uint32   mrc_rand(void);

int32 runmrps(char *name)
{
	char *qd;
	if(mrc_fileState("xqc/mtk.txt")==MR_IS_FILE)
		qd="start.mr";
	else
		qd="cfunction.ext";
	return mrc_runMrp(name,qd,NULL);
}
void setupini(int32 data)
{
	if(!data)
	{
		data=mrc_timerCreate();
		mrc_timerStart(data,1,data,setupini,0);
	}
	else
	{

#ifdef SIM_MOD
	int32 f;
#endif
	uint16 *yan;
	mrc_timerDelete(data);

	yan=(uint16 *)w_getScreenBuffer();
	yan[0]=(uint16)mrc_rand()%100;
	yan[1]=(uint16)mrc_rand()%100;
	yan[2]=yan[0]*yan[1]+'c';
	yan[3]=0;
	setuptext((char *)(yan+3));
#ifdef SIM_MOD
	f=mrc_open("xqc/scr.tem",12);
	mrc_write(f,yan,240*320*2);
	mrc_close(f);
	xqc_init();
#else
	runmrps("xqc/debug.mrp");
#endif
	}
}
void Debugging()
{
	//char mrp[1024];
	//mrp[0]=0;
	//mrc_sprintf(mrp,"%s.mrp",WorkFile);
	if(WorkFile!=NULL)
	{
		if(mrc_strlen(WorkFile)>0)
		{
			SaveFile();
			setupini(0);
		}
	}
}

//主菜单事件
void CallBack(int id)
{
	switch(id)
	{
	case 1:
		mrc_pause();
		CreateEdit(INSERT,"");
		ReSelectMenu(MainMenu);
		MainWindow=EDITOR;
		break;
	case 4://运行
		PACKGEDMRP=0;
		Debugging();
		break;
	case 6:
		mrc_pause();
		CreateEdit(GOTOLINE,"");
		ReSelectMenu(MainMenu);
		MainWindow=EDITOR;
		break;
	case 5:
		EditFreeAll();
		MenuFreeAll();
		mrc_exit();
		break;
	case 21:
		EditSetCopyStart();
		break;
	case 22:
		EditSetCopyEnd();
		break;
	case 23:
		CopyStrText();
		break;
	case 24:
		PasteStrText();
		break;
	case 25:
		CleanSelect();
		break;
	case 31://打开
		mrc_pause();
		MainWindow=FILEVIEW;
		FileViewInit();
		ReSelectMenu(MainMenu);
		return ;
		break;
	case 32://保存
		SaveFile();
		break;
	case 33://另存
		SaveFile();
		break;
	case 34://打包
		PACKGEDMRP=1;
		Debugging();
		break;
	default:
		break;
	}
	ReSelectMenu(MainMenu);
	mrc_pause();
	MainWindow=EDITOR;
	mrc_resume();
}

//文件事件
void FileMenuCallBack(int id)
{
	switch(id)
	{
	case 1://打开
		FileSelect();
		break;
	case 3://打包
		break;
	case 4://返回
		FileFreeAll();
		MainWindow=EDITOR;
		mrc_resume();
		return;
	case 21://新建
		break;
	case 22://重命名
		break;
	case 23://删除
		break;
	case 24://运行
		FileRun();
		break;
	}
	ReSelectMenu(MainMenu);
	mrc_pause();
	FileMenuShow=0;
	mrc_resume();
}
void BackMenu(MenuHand *menuhand)
{
	int i=0;
	if(menuhand==NULL)
	{
		return;
	}
	for(i=MENUMAX-1;i>=0;i--)
	{
		if(i==0)
		{
			menuhand->callback(-1);
			return ;
		}
		if(menuhand->focus[i]!=NULL)
		{
			menuhand->focus[i]=NULL;
			return ;
		}
	}
	return ;
}
void ReSelectMenu(MenuHand *menuhand)
{
	int i=0;
	if(menuhand==NULL)
		return;
	for(i=0;i<MENUMAX;i++)
	{
		menuhand->focus[i]=NULL;
	}
	menuhand->focus[0]=menuhand->head;
}
void SelectMenu(MenuHand *menuhand)
{
	int i=0;
	if(menuhand==NULL)
		return;
	if(menuhand->focus[0]==NULL)
	{
		menuhand->focus[0]=menuhand->head;
		return ;
	}
	for(i=0;i<MENUMAX;i++)
	{
		if(i==MENUMAX-1)
		{
			if(menuhand->focus[i]!=NULL)
				menuhand->callback(menuhand->focus[i]->menuid);
			return ;
		}
		if(menuhand->focus[i+1]==NULL)
		{
			if(menuhand->focus[i]!=NULL)
			{
				menuhand->focus[i+1]=menuhand->focus[i]->child;
				if(menuhand->focus[i+1]==NULL)
					menuhand->callback(menuhand->focus[i]->menuid);
			}
			return ;
		}
	}
}
void MenuUp(MenuHand *menuhand)
{
	int i=0;
	if(menuhand==NULL)
		return;
	if(menuhand->focus[0]==NULL)
		return ;
	for(i=0;i<MENUMAX;i++)
	{
		if(menuhand->focus[i]==NULL)
			break;
	}
	i--;
	if(menuhand->focus[i]->pre!=NULL)
		menuhand->focus[i]=menuhand->focus[i]->pre;
	else
	{
		menuhand->focus[i]=GetMenuEndNode(menuhand->focus[i]);
	}
}
void MenuDown(MenuHand *menuhand)
{
	int i=0;
	if(menuhand==NULL)
		return;
	if(menuhand->focus[0]==NULL)
		return ;
	for(i=0;i<MENUMAX;i++)
	{
		if(menuhand->focus[i]==NULL)
			break;
	}
	i--;
	if(menuhand->focus[i]->next!=NULL)
		menuhand->focus[i]=menuhand->focus[i]->next;
	else
	{
		menuhand->focus[i]=GetMenuFristNode(menuhand->focus[i]);
	}
}
int32 MenuEvent(int32 code,int32 p1,int32 p2);
void MenuKeyLoop(int32 data)
{
	KeyTimerLoop++;
	if(KeyTimerLoop==0||KeyTimerLoop>5)
		MenuEvent(MR_KEY_PRESS,data,0x7fffffff);
}
static void KeyLongPress(int32 p1,int32 p2)
{
	if(p2!=0x7fffffff)
		KeyTimerLoop=0;
	mrc_timerStart(MenuKeyTimer,50,p1,(mrc_timerCB)MenuKeyLoop,1);
}
static void KeyFree()
{
	mrc_timerStop(MenuKeyTimer);
}
int32 MenuEvent(int32 code,int32 p1,int32 p2)
{
	if(MR_KEY_PRESS==code)
	{
		switch(p1)
		{
		case MR_KEY_DOWN:
			MenuDown(MainMenu);
			MenuResume();
			break;
		case MR_KEY_UP:
			MenuUp(MainMenu);
			MenuResume();
			break;
		}
		KeyLongPress(p1,p2);
	}
	if(MR_KEY_RELEASE==code)
	{
		KeyFree();
		switch(p1)
		{
		case MR_KEY_LEFT:
		case MR_KEY_SOFTRIGHT:
			BackMenu(MainMenu);
			mrc_resume();
			break;
		case MR_KEY_SELECT:
		case MR_KEY_SOFTLEFT:
		case MR_KEY_RIGHT:
			SelectMenu(MainMenu);
			mrc_resume();
			break;
		}
	}
	if(MR_MOUSE_DOWN==code)
	{

	}
	return 0;
}
int32 MenuResume()
{
	if(MainMenu)
	{
		MainMenu->background(1);
		DrawMenu(*MainMenu);
	}
	mrc_refreshScreen(0,0,SCRW,SCRH);
	return 0;
}
void LoadMenu(int data)
{
	switch(data)
	{
	case 0:
		MainMenu=&EditMenu;
		break;
	case 1:
		MainMenu=&FileMenu;
		break;
	}
}
