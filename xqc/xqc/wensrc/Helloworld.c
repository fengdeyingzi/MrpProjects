#include "mrc_base.h"
#include "wenexp.h"
///////////////////////////////////////
enum Windows{
	EDITOR,//编辑器
	MAINMENU,//主菜单
	FILEVIEW,//文件浏览
	SETING//设置
};
enum Windows MainWindow=EDITOR;
////////////////////全局//////////////
int32 EditEditHand=0,//编辑框句柄
     EditFunction=0;
enum {
	INSERT,
	GOTOLINE,
	RENAME
};

#define mallocchar(text) memcpy(malloc(strlen(text)+1),text,strlen(text)+1)
#define Malloc(len) mrc_memset(mrc_malloc(len),0,len)

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
int CreateEdit(int data,char *text)
{
	switch(data)
	{
	case INSERT:
		EditEditHand=MyCreate("请输入要插入的内容","",MR_EDIT_ANY,255);//编辑框句柄
		EditFunction=INSERT;
		break;
	case GOTOLINE:
		EditEditHand=MyCreate("请输入行数","",MR_EDIT_NUMERIC,8);//编辑框句柄
		EditFunction=GOTOLINE;
		break;
	case RENAME:
		EditEditHand=MyCreate("请输入文件名",text,MR_EDIT_NUMERIC,8);//编辑框句柄
		EditFunction=RENAME;
		break;
	}
	return 0;
}
int32 mrc_unicodeToGb2312(uint8* input, int32 input_len, uint8** output, int32* output_len);
char *GetEditText()
{
	int32 inputlen=0,outputlen=0;
	char *input=NULL,*output=NULL;
	input=(char*)mrc_editGetText(EditEditHand);
	inputlen=mrc_wstrlen(input);
	output=mrc_malloc(inputlen+1);
	output[0]=0;
	mrc_unicodeToGb2312((uint8*)input,inputlen,(uint8**)&output,&outputlen);
	return output;
}
int GotoLine(int line);
int32 EditInsertText(char *text);//在光标处插入一段文字y
int32 DoEdit()
{
	int32 num=0;
	char *text=GetEditText();
			mrc_editRelease(EditEditHand);
			switch(EditFunction)
			{
				case INSERT:
					EditInsertText(text);
					break;
				case GOTOLINE:
					num=mrc_atoi(text);
					GotoLine(num);
					break;
			}
			mrc_free(text);
			EditEditHand=0;
			MainWindow=EDITOR;
	return 0;
}

//////////////////////////////////////

mr_colourSt BackCol,FontCol,FocusCol,FocusFontCol,OtherCol;//各种颜色
int SCRW=240,SCRH=320;
char *WorkFile=NULL;
int32 FONT;

///////////////////公用接口/////////////////
int32  EditPause(void);
int32 EditResume(void);
int32 EditorInit(void);//初始化
int32 EditFreeAll(void);//释放所有
int32 EditEvent(int32 code, int32 p1, int32 p2);
int32 EditSaveToFile(char *name);
int32 EditLoadFile(char *filename);//会自动释放之前的head
void SetEditorListShow(int x,int y,int h);


int32 MenuEvent(int32,int32,int32);

int32 FileViewEvent(int32,int32,int32);
void DrawMainMenu(void);
int FileViewInit(void);
int MenuInit(void);
int32 FileResume(void);
void LoadMenu(int data);
int32 FileViewEvent(int32 code,int32 p1,int32 p2);
////////////////////////////////////////////
int32 ColInit()//初始化颜色
{
	BackCol.r=255;BackCol.g=255;BackCol.b=255;//背景
	FontCol.r=5;FontCol.g=5;FontCol.b=5;//文字
	FocusCol.r=120;FocusCol.g=255;FocusCol.b=255;//焦点背景
	FocusFontCol.r=255;FocusFontCol.g=0;FocusFontCol.b=0;//焦点文字
	OtherCol.r=0;OtherCol.g=172;OtherCol.b=210;//其他
	return 0;
}
///////////////////基本/////////////////////
char *Str_Clean=NULL,*Str_Menu=NULL;

void EditFile(char *path,char *name)
{
	char *temp;
	temp=(char*)mrc_malloc(mrc_strlen(path)+mrc_strlen(name)+3);
	temp[0]=0;
	if(temp==NULL)
		return ;
	mrc_strcpy(temp,path);
	mrc_strcat(temp,"/");
	mrc_strcat(temp,name);
	if(WorkFile)
		mrc_free(WorkFile);
	WorkFile=temp;
}
int SaveFile()
{
	if(WorkFile)
	return  EditSaveToFile(WorkFile);
	return -1;
}
int fileaz(char *mrpfile,char *file)
{
	int32 hand=0,len=0;
	void *data=NULL;
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

	return 0;
}



int setini()
{
	uint32 len,len2,ll=0,i=0;
	char *text=NULL,*a,*b,dou=',',end='\n';
	int16 *yan;
#ifdef SIM_MOD
	int32 f;
	f=mrc_open("xqc/scr2.tem",1);
	if(!f)
		return 0;
	mrc_read(f,w_getScreenBuffer(),240*320*2);
	mrc_close(f);
#endif
	yan=(int16 *)w_getScreenBuffer();
	if(yan[0]*yan[1]+'w'==yan[2])
		text=(char *)(yan+3);
	if(text==NULL)
		return -1;

	if(text==NULL)
		return 0;
	len=strlen(text);


	i=ll=0;
	if((a=strstr(text,b="MAINNAME="))!=NULL)
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
					ii=(int)((char *)memchr(a,end,len2)-a);
					WorkFile=memcpy(Malloc(ii),a,ii-1);
					break;
				}
			}

		} while (i<len2);

	}


#ifndef includefile
	free(text);
#endif
	return 0;
}

#include "libini.h"

uint8 s16210(char a)
{
	if(a>='a'&&a<='f')
		return a-'a'+10;

	if(a>='A'&&a<='F')
		return a-'A'+10;

	if(a>='0'&&a<='9')
		return a-'0';
	return 0;
}
uint8 s162210(char *a)
{
	uint8 r;
	r=s16210(a[0]);
	r<<=4;
	r+=s16210(a[1]);
	return r;
}
void s162color(char *dt,mr_colourSt *color)
{
	color->r=s162210(dt);
	color->g=s162210(dt+2);
	color->b=s162210(dt+4);
}
void s162rgb(char *dt,mr_colourSt *color)
{
	if(dt!=NULL)
		if(dt[0]=='0'&&(dt[1]=='x'||dt[1]=='X'))
			s162color(dt+2,color);
}

char *srgb216(char *dt,mr_colourSt color)
{
	sprintf(dt,"0x%x%x%x",color.r,color.g,color.b);
	return dt;
}
int skinmain()
{
	INI *pini;
	char v1[32];
	char v2[32];
	char v3[32];
	int i1, i2, i3;

	//return 7;
	pini = ini_open("xqc/skin.ini");
	if (!pini) {
		return -1;
	}
	//SET
	FONT = ini_get_int(pini,"SET@FONT");


	//COLOR
	ini_get(pini,"COLOR@BACKCOL",v1);
	s162rgb(v1,&BackCol);

	ini_get(pini,"COLOR@FONTCOL",v1);
	s162rgb(v1,&FontCol);

	ini_get(pini,"COLOR@FOCUSCOL",v1);
	s162rgb(v1,&FocusCol);

	ini_get(pini,"COLOR@FOCUSFONTCOL",v1);
	s162rgb(v1,&FocusFontCol);

	ini_get(pini,"COLOR@OTHERCOL",v1);
	s162rgb(v1,&OtherCol);

	ini_close(pini);

	return 0;
}

void DrawDownTitle()
{
	int textw,texth;
	fontwh(Str_Menu,0,FONT,(int32*)&textw,(int32*)&texth);
	mrc_drawRect(0,SCRH-texth+1,SCRW,texth,OtherCol.r,OtherCol.g,OtherCol.b);
	gbsez(Str_Menu,5,SCRH-texth+1,FontCol.r,FontCol.r,FontCol.b,0,FONT);
	fontwh(Str_Clean,0,FONT,(int32*)&textw,(int32*)&texth);
	gbsez(Str_Clean,SCRH-texth-5,SCRH-texth,FontCol.r,FontCol.r,FontCol.b,0,FONT);
}/*
int32 mrc_init(void)
{
	int textw,texth;
	mr_screeninfo scrinfo;
	Str_Clean="删除";
	Str_Menu="菜单";
	WorkFile="test.mpr";
	az();
	setini();
	mrc_getScreenInfo(&scrinfo);
	SCRW=scrinfo.width;
	SCRH=scrinfo.height;
	if(SCRW==0||SCRH==0)
	{
	SCRW=240;
	SCRH=320;
	}
	fontwh(Str_Menu,0,FONT,(int32*)&textw,(int32*)&texth);
	ColInit();
	MenuInit();
	EditorInit();
	SetEditorListShow(5,0,SCRH-texth);
	EditLoadFile(WorkFile);
	mrc_pause();
	mrc_resume();
	return 0;
}*/
int32 mrc_appEvent(int32 code,int32 p1,int32 p2)
{
	static int32 tcode,tp1,tp2;
	char *text=NULL;
	//eventan(&code,&p1,&p2);
	switch(MainWindow)
	{
	case EDITOR:
		if(code==MR_DIALOG_EVENT)
		{ 
			DoEdit();
			mrc_resume();
			return 0;
		}
		if(code==MR_KEY_RELEASE)
		{
			if(p1==MR_KEY_SOFTLEFT)
			{
				mrc_pause();
				MainWindow=MAINMENU;
				LoadMenu(0);
				mrc_resume();
				return 0;
			}
		}else
		if(code==MR_MOUSE_DOWN)
		{
			tcode=code;
			tp1=p1;
			tp2=p2;
		}else
		if(code==MR_MOUSE_UP)
		{
			if(tp1>0&&tp1<50&&tp2>SCRH-20&&tp2<SCRH)
			if(p1>0&&p1<50&&p2>SCRH-20&&p2<SCRH)
			{
				mrc_pause();
				MainWindow=MAINMENU;
				LoadMenu(0);
				mrc_resume();
				return 0;
			}
		}
		return EditEvent(code,p1,p2);
		break;
	case MAINMENU:
		return MenuEvent(code,p1,p2);
		break;
	case FILEVIEW:
		return FileViewEvent(code,p1,p2);
		break;
	}
	
	return 0;
}



int32 mrc_pause(void)
{
	switch(MainWindow)
	{
	case EDITOR:
		EditPause();
		return 0;
		break;
	case MAINMENU:
		//MenuResume();
		return 0;
		break;
	}
	return 0;
}

int32 mrc_resume(void)
{
	switch(MainWindow)
	{
	case EDITOR:
		EditResume();
		return 0;
		break;
	case MAINMENU:
		DrawMainMenu();
		return 0;
		break;
	case FILEVIEW:
		FileResume();
		return 0;
		break;
	}
	return 0;
}

int32 mrc_exitApp(void)
{
	EditFreeAll();
	return 0;
}
