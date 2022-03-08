#include "mrc_base.h"
#include "wenexp.h"
#include "skin.h"
#include "editormain.h "
#include "HighLight.h"
#include "editor.h"
#include "Keyboard.h "

#define WORDLIBFILE "xqc/wordlib.txt"
enum {
	KEY9,//字母9键小写。
	KEY91,//字母九键大写
	KEYBOARD,//字母全键盘小写
	KEYBOARD1,//字母全键盘大写
	KEYNUM,//数字
	KEYUDLR//上下左右
};

int 	KeyShowX=5;//X


//////////////////全局量//////////////////////
extern mr_colourSt BackCol,FontCol,FocusCol,FocusFontCol,OtherCol;//外联,各种颜色
extern int SCRW,SCRH;
extern int32 FONT,SET_HEAP_SIZE;

static int eventcount;

static int32 InputTimer=0;//按键循环标志//按键延时输入timer

static int WordType=0;//0小写,1大写,2数字
static int PressX=-1,PressY=-1;//按下的键位

int EditKeyboardH=240*3/2,//虚拟键盘高度
EditKeyboardType,//虚拟键盘类型
EditFindLib=0,//是否从词库中搜索

FunctionType=0,  //工具箱类型 0键盘收拢，1键盘展开
SignType=0,//标点符号状态优先于function，0关闭，1打开
SignPreFunction=0,
DaoHang=0;//方向键是否展开

static EditListLink_p 		   EditWordLib=NULL;//词库链表

static char FunctionText[100];//最大100;

static EditListLink_p EditListTemp,//临时链表
EditCiTiao=NULL,//词条链表
EditCiTiaoFocus=NULL;//词条焦点


char keyedt[2][8];















int StateText(char *word,char *text)
{
	int wordlen=0,textlen=0;
	char t=0,c=0;
	wordlen=mrc_strlen(word);
	textlen=mrc_strlen(text);
	if(textlen>wordlen)
		return 0;
	while(word[0]!=0&&text[0]!=0)
	{
		if(word[0]>='A'&&word[0]<='Z')
			c=word[0]-('A'-'a');
		else
			c=word[0];

		if(c>='a'&&c<='c')
			t='2';
		else
			if(c>='d'&&c<='f')
				t='3';
			else
				if(c>='g'&&c<='i')
					t='4';
				else
					if(c>='j'&&c<='l')
						t='5';
					else
						if(c>='m'&&c<='o')
							t='6';
						else
							if(c>='p'&&c<='s')
								t='7';
							else
								if(c>='t'&&c<='v')
									t='8';
								else
									if(c>='w'&&c<='z')
										t='9';
									else
									{
										if(c==' ')
											t='0';
										else
											if((c<'0'||c>'9')&&c<=0x7f)
												t='1';
									}
									if(word[0]!=text[0]&&t!=text[0])
										break;
									word++;
									text++;
	}
	if(text[0]==0)
		return 1;
	else
		return 0;
}
int GetNum(char w)
{
	char t=0,c=0;
	if(w>='A'&&w<='Z')
		c=w-('A'-'a');
	else
		c=w;

	if(c>='a'&&c<='c')
		t='2';
	else
		if(c>='d'&&c<='f')
			t='3';
		else
			if(c>='g'&&c<='i')
				t='4';
			else
				if(c>='j'&&c<='l')
					t='5';
				else
					if(c>='m'&&c<='o')
						t='6';
					else
						if(c>='p'&&c<='s')
							t='7';
						else
							if(c>='t'&&c<='v')
								t='8';
							else
								if(c>='w'&&c<='z')
									t='9';
								else
								{
									if(c==' ')
										t='0';
									else
										if((c<'0'||c>'9')&&c<=0x7f)
											t='1';
								}
								return t;
}
EditListLink_p EditFindFormLib(char *text)
{
	EditListLink_p temp=NULL,head=NULL,end=NULL;
	if(text[0]==0)
		return NULL;
	temp=EditWordLib;
	while(temp)
	{
		if(StateText(temp->text,text)==1)
		{
			if(head==NULL)
			{
				head=EditMakeNode(temp->text);
				end=head;
			}
			else
			{
				InsertEditListNode(end,EditMakeNode(temp->text));
				if(end->next!=NULL)
					end=end->next;
			}
		}
		temp=temp->next;
	}
	return head;
}
EditListLink_p FindNodeByText(EditListLink_p head,char *text)
{
	if(head==NULL||text==NULL)
		return NULL;
	while(head)
	{
		if(mrc_strcmp(text,head->text)==0)
			return head;
		head=head->next;
	}
	return NULL;
}
void AddLib(char *text)
{
	EditListLink_p temp;
	temp=FindNodeByText(EditWordLib,text);
	if(temp==NULL)
	{
		temp=EditMakeNode(text);
		temp->next=EditWordLib;
		if(EditWordLib!=NULL)
			EditWordLib->pre=temp;
		EditWordLib=temp;
	}
	else
	{
		EditWordLib=DelAEditListNode(EditWordLib,temp);
		temp=EditMakeNode(text);
		temp->next=EditWordLib;
		if(EditWordLib!=NULL)
			EditWordLib->pre=temp;
		EditWordLib=temp;
	}
}
int WordFindFromLib()
{
	int pos=0,len=0;
	EditListLink_p end,tlink=NULL;
	if(EditCiTiao!=NULL)
		DelAllEditListNode(EditCiTiao);
	EditCiTiao=EditFindFormLib(FunctionText);
	EditCiTiaoFocus=EditCiTiao;
	end=EditGetEndNode(EditCiTiao);
	len=mrc_strlen(FunctionText);
	pos=NumFind(FunctionText);
	if(pos>=len)
		return -1;
	switch(FunctionText[pos])
	{
	case '0':
		break;
	case '1':
		tlink=MakeEditList("!\n\"\n#\n$\n%\n&\n'\n*\n+\n,\n-\n.\n/\n:\n;\n<\n=\n>\n?\n@\n[\n\\\n]\n^\n_\n{\n|\n}");
		break;
	case '2':
		tlink=MakeEditList("a\nb\nc\nA\nB\nC");
		break;
	case '3':
		tlink=MakeEditList("d\ne\nf\nD\nE\nF");
		break;
	case '4':
		tlink=MakeEditList("g\nh\ni\nG\nH\nI");
		break;
	case '5':
		tlink=MakeEditList("j\nk\nl\nJ\nK\nL");
		break;
	case '6':
		tlink=MakeEditList("m\nn\no\nM\nN\nO");
		break;
	case '7':
		tlink=MakeEditList("p\nq\nr\ns\nP\nQ\nR\nS");
		break;
	case '8':
		tlink=MakeEditList("t\nu\nv\nT\nU\nV");
		break;
	case '9':
		tlink=MakeEditList("w\nx\ny\nz\nW\nX\nY\nZ");
		break;
	}
	if(EditCiTiao==NULL)
	{
		EditCiTiao=tlink;
		EditCiTiaoFocus=EditCiTiao;
		return 0;
	}
	else
	{
		end->next=tlink;
		if(tlink!=NULL)
			tlink->pre=end;
	}
	EditCiTiaoFocus=EditCiTiao;
	return 0;
}
int EditLoadWordLib(char *filename)
{
	int len=0,filehand=0;
	char *temp=NULL;
	if(mrc_fileState(filename)!=MR_IS_FILE)
		fileaz(filename,filename);
	if(mrc_fileState(filename)!=MR_IS_FILE)
	{
		output("文件");
		output(filename);
		output("不存在\n");
		return -1;
	}
	len=mrc_getLen(filename);
	temp=mrc_malloc(len+1);
	temp[len]=0;
	if(temp==NULL)
	{
		output("内存不足\n");
		return -1;
	}
	filehand=mrc_open(filename,MR_FILE_RDONLY);
	if(filehand<=0)
	{
		mrc_free(temp);
		output("打开文件");
		output(filename);
		output("失败\n");
		return -1;
	}
	mrc_read(filehand,temp,len);
	DelAllEditListNode(EditWordLib);
	EditWordLib=NULL;
	EditWordLib=MakeEditList(temp);
	//EditCiTiaoFocus=EditCiTiao;
	mrc_free(temp);
	mrc_close(filehand);
	return 0;
}






void DrawFunction()//画工具箱
{
	int w,h;
	char *str[3];
	EDITRECT *showrect=&WORDOBG->EditShow;

	str[0]="abc";
	str[1]="ABC";
	str[2]="123";
	showrect->y+=WORDOBG->EditShowSpace;
	mrc_drawRect(0,showrect->y+WORDOBG->EditShowNum*WORDOBG->EditShowSpace,SCRW,WORDOBG->EditShowSpace,OtherCol.r,OtherCol.g,OtherCol.b);
	gbsez(FunctionText,KeyShowX,showrect->y+WORDOBG->EditShowNum*WORDOBG->EditShowSpace+1,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
	mrc_drawRect(SCRW-WORDOBG->EditShowSpace*2,showrect->y+WORDOBG->EditShowNum*WORDOBG->EditShowSpace+1,WORDOBG->EditShowSpace*2-1,WORDOBG->EditShowSpace-2,OtherCol.r,OtherCol.g,OtherCol.b);
	if(FunctionType==0)
		gbsez("+",SCRW-WORDOBG->EditShowSpace/2-WORDOBG->EditShowSpace,showrect->y+WORDOBG->EditShowNum*WORDOBG->EditShowSpace+1,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
	else
		gbsez("-",SCRW-WORDOBG->EditShowSpace/2-WORDOBG->EditShowSpace,showrect->y+WORDOBG->EditShowNum*WORDOBG->EditShowSpace+1,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
	switch(WordType)
	{
	case 0://小写
		fontwh(str[0],0,(uint16)FONT,(int32*)&w,(int32*)&h);
		gbsez(str[0],SCRW-WORDOBG->EditShowSpace*2-w,showrect->y+WORDOBG->EditShowNum*WORDOBG->EditShowSpace+1,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
		break;
	case 1://大写
		fontwh(str[1],0,(uint16)FONT,(int32*)&w,(int32*)&h);
		gbsez(str[1],SCRW-WORDOBG->EditShowSpace*2-w,showrect->y+WORDOBG->EditShowNum*WORDOBG->EditShowSpace+1,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
		break;
	case 2://字符
		fontwh(str[2],0,(uint16)FONT,(int32*)&w,(int32*)&h);
		gbsez(str[2],SCRW-WORDOBG->EditShowSpace*2-w,showrect->y+WORDOBG->EditShowNum*WORDOBG->EditShowSpace+1,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
		break;
	}
	showrect->y-=WORDOBG->EditShowSpace;
}
void DrawCiTiao()//画词条
{
	int keyh,keyw,w,h;
	EDITRECT *showrect=&WORDOBG->EditShow;

	keyh=EditKeyboardH/5;
	keyw=SCRW/5;
	if(FunctionType!=0)
	{
		mrc_drawRect(0,showrect->y+showrect->h-EditKeyboardH+1,SCRW,keyh-2,OtherCol.r,OtherCol.g,OtherCol.b);//候选条
		if(EditCiTiao!=NULL&&EditCiTiaoFocus!=NULL)
		{
			gbsez(EditCiTiaoFocus->text,0,showrect->y+showrect->h-EditKeyboardH+1,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
			if(EditCiTiaoFocus->next!=NULL)
			{
				mrc_drawRect(SCRW*2/5,showrect->y+showrect->h-EditKeyboardH+1,SCRW*2/5,keyh-2,OtherCol.r,OtherCol.g,OtherCol.b);
				gbsez(EditCiTiaoFocus->next->text,SCRW*2/5,showrect->y+showrect->h-EditKeyboardH+1,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
			}
		}
		if(DaoHang==0)
			mrc_drawRect(SCRW*4/5,showrect->y+showrect->h-EditKeyboardH+1,SCRW/5,keyh-2,OtherCol.r,OtherCol.g,OtherCol.b);
		else
			mrc_drawRect(SCRW*4/5,showrect->y+showrect->h-EditKeyboardH+1,SCRW/5,keyh-2,FocusCol.r,FocusCol.g,FocusCol.b);
		fontwh("导航",0,(uint16)FONT,(int32*)&w,(int32*)&h);
		gbsez("导航",SCRW*4/5+(SCRW/5-w)/2,showrect->y+showrect->h-EditKeyboardH+(keyh-h)/2,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
		return ;
	}
}
void DrawSign()//标点符号
{
	char *str[5][5];
	int keyh=10,keyw=10,i=0,j=0,w=0,h=0;
	EDITRECT *showrect=&WORDOBG->EditShow;

	//+-*/
	//&|!^
	//;,
	//%#\?
	//{}[]
	//_:"'
	str[0][0]="+";str[0][1]="-";str[0][2]="*";str[0][3]="/";str[0][4]="<-";
	str[1][0]="&";str[1][1]="|";str[1][2]="!";str[1][3]="^";str[1][4]="返回";
	str[2][0]="()";str[2][1]="{}";str[2][2]="<>";str[2][3]="[]";str[2][4]=",";
	str[3][0]="%";str[3][1]="#";str[3][2]="\\";str[3][3]="?";str[3][4]=".";
	str[4][0]="_";str[4][1]=":";str[4][2]="\"\"";str[4][3]="''";str[4][4]="\\n";
	if(FunctionType==0)//虚拟键盘没有展开
		return ;
	keyh=EditKeyboardH/5;
	keyw=SCRW/5;
	if(EditKeyboardType==KEY9)
	{
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
			{
				if(PressX==j&&PressY==i)
					mrc_drawRect(1+j*keyw,showrect->y+showrect->h-EditKeyboardH+keyh*i+1,keyw-2,keyh-2,FocusCol.r,FocusCol.g,FocusCol.b);
				else
					mrc_drawRect(1+j*keyw,showrect->y+showrect->h-EditKeyboardH+keyh*i+1,keyw-2,keyh-2,OtherCol.r,OtherCol.g,OtherCol.b);
				fontwh((str[i][j]),0,(uint16)FONT,(int32*)&w,(int32*)&h);
				gbsez((str[i][j]),1+j*keyw+(keyw-w)/2,showrect->y+showrect->h-EditKeyboardH+keyh*i+1+(keyh-h)/2,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
			}
	}
}
void DrawDaoHang()//画方向键
{
	char *str[4][5];
	int keyh=10,keyw=10,i=0,j=0,w=0,h=0;
	EDITRECT *showrect=&WORDOBG->EditShow;

	if(FunctionType==0||DaoHang==0||SignType==1)//虚拟键盘没有展开
		return ;
	str[0][0]="返回";str[0][1]="";str[0][2]="上";str[0][3]="";str[0][4]="<-";
	str[1][0]="返回";str[1][1]="左";str[1][2]="确定";str[1][3]="右";str[1][4]="复制";
	str[2][0]="返回";str[2][1]="";str[2][2]="下";str[2][3]="";str[2][4]="剪切";
	str[3][0]="返回";str[3][1]="起点";str[3][2]="终点";str[3][3]="取消";str[3][4]="粘贴";

	keyh=EditKeyboardH/5;
	keyw=SCRW/5;
	showrect->y+=keyh;
	for(i=0;i<4;i++)
		for(j=0;j<5;j++)
		{
			if(PressX==j&&PressY==i)
				mrc_drawRect(1+j*keyw,showrect->y+showrect->h-EditKeyboardH+keyh*i+1,keyw-2,keyh-2,FocusCol.r,FocusCol.g,FocusCol.b);
			else
				mrc_drawRect(1+j*keyw,showrect->y+showrect->h-EditKeyboardH+keyh*i+1,keyw-2,keyh-2,OtherCol.r,OtherCol.g,OtherCol.b);
			fontwh((str[i][j]),0,(uint16)FONT,(int32*)&w,(int32*)&h);
			gbsez((str[i][j]),1+j*keyw+(keyw-w)/2,showrect->y+showrect->h-EditKeyboardH+keyh*i+1+(keyh-h)/2,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
		}
		showrect->y-=keyh;
}
void DrawKeyboardKey9()//画九键虚拟键盘
{
	char *str[4][5],*STR[4][5],*num[4][5];
	int keyh=10,keyw=10,i=0,j=0,w=0,h=0;
	EDITRECT *showrect=&WORDOBG->EditShow;

	if(FunctionType==0)//虚拟键盘没有展开
		return ;
	str[0][0]=",";str[0][1]="=";str[0][2]="abc";str[0][3]="def";str[0][4]="<-";
	str[1][0]=";";str[1][1]="ghi";str[1][2]="jkl";str[1][3]="mno";str[1][4]="词";
	str[2][0]="()";str[2][1]="pqrs";str[2][2]="tuv";str[2][3]="wxyz";str[2][4]="ABC";
	str[3][0]="{}";str[3][1]="符";str[3][2]="空";str[3][3]="全";str[3][4]="\\n";

	STR[0][0]=",";STR[0][1]="=";STR[0][2]="ABC";STR[0][3]="DEF";STR[0][4]="<-";
	STR[1][0]=";";STR[1][1]="GHI";STR[1][2]="JKL";STR[1][3]="MNO";STR[1][4]="词";
	STR[2][0]="()";STR[2][1]="PQRS";STR[2][2]="TUV";STR[2][3]="WXYZ";STR[2][4]="123";
	STR[3][0]="{}";STR[3][1]="符";STR[3][2]="空";STR[3][3]="全";STR[3][4]="\\n";

	num[0][0]="+";num[0][1]="1";num[0][2]="2";num[0][3]="3";num[0][4]="<-";
	num[1][0]="-";num[1][1]="4";num[1][2]="5";num[1][3]="6";num[1][4]="词";
	num[2][0]="*";num[2][1]="7";num[2][2]="8";num[2][3]="9";num[2][4]="abc";
	num[3][0]="/";num[3][1]="符";num[3][2]="0";num[3][3]="=";num[3][4]="\\n";
	if(EditFindLib==1)
	{
		str[0][1]="1";
		STR[0][1]="1";
		STR[3][2]="0";
		str[3][2]="0";
	}



	keyh=EditKeyboardH/5;
	keyw=SCRW/5;
	showrect->y+=keyh;
	if(EditKeyboardType==KEY9)
	{
		for(i=0;i<4;i++)
			for(j=0;j<5;j++)
			{
				if(PressX==j&&PressY==i)
					mrc_drawRect(1+j*keyw,showrect->y+showrect->h-EditKeyboardH+keyh*i+1,keyw-2,keyh-2,FocusCol.r,FocusCol.g,FocusCol.b);
				else
					mrc_drawRect(1+j*keyw,showrect->y+showrect->h-EditKeyboardH+keyh*i+1,keyw-2,keyh-2,OtherCol.r,OtherCol.g,OtherCol.b);
				if(WordType==0)//小写键盘
				{
					fontwh((str[i][j]),0,(uint16)FONT,(int32*)&w,(int32*)&h);
					gbsez((str[i][j]),1+j*keyw+(keyw-w)/2,showrect->y+showrect->h-EditKeyboardH+keyh*i+1+(keyh-h)/2,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
				}
				else if(WordType==1)//大写键盘
				{
					fontwh((STR[i][j]),0,(uint16)FONT,(int32*)&w,(int32*)&h);
					gbsez((STR[i][j]),1+j*keyw+(keyw-w)/2,showrect->y+showrect->h-EditKeyboardH+keyh*i+1+(keyh-h)/2,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
				}
				else if(WordType==2)//数字键盘
				{
					fontwh((num[i][j]),0,(uint16)FONT,(int32*)&w,(int32*)&h);
					gbsez((num[i][j]),1+j*keyw+(keyw-w)/2,showrect->y+showrect->h-EditKeyboardH+keyh*i+1+(keyh-h)/2,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
				}
			}
	}
	if(EditFindLib!=0)//表示从词库查找功能打开
	{
		i=1;
		j=4;
		fontwh(str[i][j],0,(uint16)FONT,(int32*)&w,(int32*)&h);
		gbsez((str[i][j]),1+j*keyw+(keyw-w)/2,showrect->y+showrect->h-EditKeyboardH+keyh*i+1+(keyh-h)/2,255,255,255,0,(uint16)FONT);
	}
	showrect->y-=keyh;
}

void DrawKeyboard()
{
	if(FunctionType==0)//虚拟键盘没有展开
		return ;
	if(FunctionType==1&&SignType==0)
		switch(EditKeyboardType)
	{
		case KEY9:
			DrawCiTiao();
			DrawKeyboardKey9();
			return;
			break;
	}
	if(SignType==1)
	{
		DrawSign();
	}
}

void UnfoldKeyboard()//展开/关闭 虚拟键盘
{
	EDITRECT *showrect=&WORDOBG->EditShow;

	EditKeyboardH=SCRW*2/3;
	if(EditKeyboardH>showrect->h)
		return;
	if(FunctionType==0)
	{
		FunctionType=1;
		SetEditorListShow(WORDOBG,showrect->x,showrect->y,showrect->w,showrect->h);
		UDyidongyueshu();
		return ;
	}
	EditFindLib=0;
	SignType=0;
	CleanLib();
	FunctionType=0;
	SetEditorListShow(WORDOBG,showrect->x,showrect->y,showrect->w,showrect->h);
	UDyidongyueshu();
}
void UnfoldSign()//展开符号
{
	SignPreFunction=FunctionType;
	if(FunctionType==0)
	{
		UnfoldKeyboard();
	}
	SignType=1;
	PressX=0;PressY=0;
}


/**/
void CleanLib()
{
	FunctionText[0]=0;
	FunctionText[1]=0;
	DelAllEditListNode(EditCiTiao);
	EditCiTiao=NULL;
	EditCiTiaoFocus=NULL;
}
int NumFind(char *text)//查找数字符号
{
	int i=0;
	for(;text[i]!=0;i++)
	{
		if(text[i]>='0'&&text[i]<='9')
			break;
	}
	return i;
}
void WordSelect(EditListLink_p node)
{
	int p=0,len=0,wordlen=0;
	p=NumFind(FunctionText);
	len=mrc_strlen(FunctionText);
	if(p>=len)
	{
		EditInsertText(FunctionText);
		EditInsertText(" ");
		AddLib(FunctionText);
		CleanLib();
		return ;
	}
	if(node!=NULL)
	{
		wordlen=mrc_strlen(node->text);
		if(wordlen>=len)
		{
			EditInsertText(node->text);
			EditInsertText(" ");
			AddLib(node->text);
			CleanLib();
		}
		else if(wordlen==1)
		{
			FunctionText[p]=node->text[0];
			WordFindFromLib();
		}
		else
		{
			EditInsertText(FunctionText);
			EditInsertText(" ");
			AddLib(FunctionText);
			CleanLib();
		}
	}
	else
	{
		EditInsertText(FunctionText);
		EditInsertText(" ");
		AddLib(FunctionText);
		CleanLib();
	}
}
//写入延迟插入字符
void KeyInput(int32 data)
{
	EditInsertText((char *)&FunctionText[0]);
	FunctionText[0]=0;
	eventcount=0;
	DrawEditor(0);
}
//延迟插入一个字符
void InputStart()
{
	FunctionText[1]=0;
	eventcount++;
	mrc_timerStart(InputTimer,500,0,(mrc_timerCB)KeyInput,0);
}



int KeyboardEvent(int32 code,int32 p1,int32 p2)//物理键盘事件
{
	int tlen=0,tpos=0;
	static int32 px=0,py=0;
	px=p1;py=p2;
	if(FunctionType==0)
	{
		if(SignType==1||EditFindLib==0)
		{
		}
		else
		{
			if(code==MR_KEY_RELEASE)
			{

			}
			return 0;
		}
	}
	if(code==MR_KEY_PRESS)
	{
		if(WordType==0&&EditFindLib==0)//小写键盘
		{
			switch(p1)
		{
			case  MR_KEY_SELECT:
				EditInsertText("\n");
				break;
			case  MR_KEY_SOFTRIGHT:
				DelText();
				PressX=4;PressY=0;
				break;
			case MR_KEY_SEND:
				if(EditFindLib!=0)
					EditFindLib=0;
				else
				{
					EditFindLib=1; 
					UnfoldKeyboard();
				}
				CleanLib();
				return 0;
				break;
			case  MR_KEY_STAR:
				PressX=4;PressY=3;
				//UnfoldSign();
				break;
			case MR_KEY_POUND:
				WordType=1;
				FunctionText[0]=0;FunctionText[1]=0;
				DrawEditor(0);
				return 0;
				break;
			case  MR_KEY_0:
				PressX=2;PressY=3;
				if(FunctionText[0]==0)
					EditInsertText(" ");
				else
					KeyInput(0);
				break;
			case  MR_KEY_1:
				PressX=1;PressY=0;
				EditInsertText("=");
				break;
			}
			if(p1>1&&p1<10)
			{
				PressX=((p1-1)%3)+1;
				PressY=(p1-1)/3;
				if(FunctionText[0]&&(p1!=px))
					KeyInput(0);
				FunctionText[0]=keyedt[0][p1-2]+eventcount%keyedt[1][p1-2];
				InputStart();
			}
		}
		if(WordType==1&&EditFindLib==0)//大写键盘
		{
			switch(p1)
		{
			case  MR_KEY_SELECT:
				EditInsertText("\n");
				break;
			case  MR_KEY_SOFTRIGHT:
				PressX=4;
				PressY=0;
				DelText();
				break;
			case MR_KEY_SEND:
				if(EditFindLib!=0)
					EditFindLib=0;
				else
				{
					EditFindLib=1;
					UnfoldKeyboard();
				}
				CleanLib();
				return 0;
				break;
			case  MR_KEY_STAR:////////////////////////////////////////////////////////////////////
				PressX=1;
				PressY=3;
				break;
			case MR_KEY_POUND:
				WordType=2;
				FunctionText[0]=0;
				FunctionText[1]=0;
				DrawEditor(0);
				return 0;
				break;
			case  MR_KEY_0:
				PressX=2;
				PressY=3;
				if(FunctionText[0]==0)
					EditInsertText(" ");
				else
					KeyInput(0);
				break;
			case  MR_KEY_1:
				PressX=1;
				PressY=0;
				EditInsertText("=");
				break;
		}
		if(p1>1&&p1<10)
		{
			PressX=((p1-1)%3)+1;
			PressY=(p1-1)/3;
			if(FunctionText[0] && p1!=px)
				KeyInput(0);
			FunctionText[0]=keyedt[0][p1-2]+eventcount%keyedt[1][p1-2]+('A'-'a');
			InputStart();
		}
		}
		if(WordType==2)//数字键盘
		{
			switch(p1)
		{
			case  MR_KEY_SELECT:
				EditInsertText("\n");
				break;
			case  MR_KEY_SOFTRIGHT:
				PressX=4;
				PressY=0;
				DelText();
				break;
			case MR_KEY_SEND:
				if(EditFindLib!=0)
					EditFindLib=0;
				else
				{
					EditFindLib=1;
					UnfoldKeyboard();
				}
				CleanLib();
				break;
			case  MR_KEY_STAR:
				PressX=1;
				PressY=3;
				break;
			case MR_KEY_POUND:
				WordType=0;
				FunctionText[0]=0;
				FunctionText[1]=0;
				DrawEditor(0);
				return 0;
				break;
		}
		if(p1>=0&&p1<10)
		{
			
			FunctionText[0]=(char)p1+'0';
			FunctionText[1]=0;
			EditInsertText(&FunctionText[0]);
			if(!p1)
			{
				PressX=2;
				PressY=3;
			}
			else
			{
				PressX=(p1-1)%3+1;
				PressY=(p1-1)/3;
			}
		}
		}
		KeyLongPress(p1,p2);
		DrawEditor(0);
		if(EditFindLib!=1)
			return 0;
	}
	if(EditFindLib==1&&code==MR_KEY_PRESS&&WordType!=2)
	{
		switch(p1)
		{
		case MR_KEY_SELECT:
			WordSelect(EditCiTiaoFocus);
			return 0;
			break;
		case  MR_KEY_SOFTRIGHT:
			if(FunctionText[0]==0)
				DelText();
			else
			{
				tlen=mrc_strlen(FunctionText);
				tpos=NumFind(FunctionText);
				if(tpos==0)
				{
					FunctionText[tlen-1]=0;
					WordFindFromLib();
				}
				else
				{
					if(tpos>=1)
					{
						FunctionText[tpos-1]=GetNum(FunctionText[tpos-1]);
						WordFindFromLib();
					}
				}
			}
			PressX=4;
			PressY=0;
			break;
		case  MR_KEY_UP:
			if(FunctionText[0]==0)
				return -1;
			else
			{
				if(EditCiTiaoFocus!=NULL)
				{
					if(EditCiTiaoFocus->pre!=NULL)
					{
						EditCiTiaoFocus=EditCiTiaoFocus->pre;
						if(EditCiTiaoFocus->pre!=NULL)
							EditCiTiaoFocus=EditCiTiaoFocus->pre;
					}
					else
					{
						EditInsertText(FunctionText);
						CleanLib();
					}
				}
				else
				{
					EditInsertText(FunctionText);
					CleanLib();
				}
			}
			break;
		case  MR_KEY_DOWN:
			if(FunctionText[0]==0)
				return -1;
			else
			{
				if(EditCiTiaoFocus!=NULL)
				{
					if(EditCiTiaoFocus->next!=NULL)
						EditCiTiaoFocus=EditCiTiaoFocus->next;
					if(EditCiTiaoFocus->next!=NULL)
						EditCiTiaoFocus=EditCiTiaoFocus->next;
				}
			}
			break;
		case  MR_KEY_LEFT:
			if(FunctionText[0]==0)
				return -1;
			else
			{
				if(EditCiTiaoFocus!=NULL)
				{
					if(EditCiTiaoFocus->pre!=NULL)
						EditCiTiaoFocus=EditCiTiaoFocus->pre;
				}
			}
			break;
		case  MR_KEY_RIGHT:
			if(FunctionText[0]==0)
				return -1;
			else
			{
				if(EditCiTiaoFocus!=NULL)
				{
					if(EditCiTiaoFocus->next!=NULL)
						EditCiTiaoFocus=EditCiTiaoFocus->next;
				}
			}
			break;
		case MR_KEY_SEND:
			if(EditFindLib!=0)
				EditFindLib=0;
			else
			{
				EditFindLib=1;
				UnfoldKeyboard();
			}
			CleanLib();
			return 0;
			break;
		case  MR_KEY_STAR:
			PressX=1;
			PressY=3;
			break;
		case MR_KEY_POUND:
			WordType++;
			if(WordType>=3)
				WordType=-1;
			FunctionText[0]=0;
			FunctionText[1]=0;
			DrawEditor(0);
			return 0;
			break;
			}
			if(p1>=0&&p1<10)
			{
				if(mrc_strlen(FunctionText)<99)
				{
					char *aa=malloc(2);
					aa[1]=0;
					aa[0]='0'+p1;
					mrc_strcat(FunctionText,aa);
					free(aa);
					WordFindFromLib();
					if(!p1)
					{
						PressX=2;
						PressY=3;
					}
					else
					{
						PressX=(p1-1)%3+1;
						PressY=(p1-1)/3;
					}
				}
			}
		KeyLongPress(p1,p2);
		DrawEditor(0);
		return 0;
	}
	if(code==MR_KEY_RELEASE)
	{
		KeyFree();
		PressX=-1;
		PressY=-1;
		if(p1==MR_KEY_STAR)
		{
			UnfoldSign();
		}
		if(EditFindLib==1)
		{
		}
		DrawEditor(0);
	}

	return 0;
}
//触屏事件
int MouseEvent(int32 code,int32 p1,int32 p2)
{
	static int tcode=0,tp1=0,tp2=0,tpos=0,move=0;
	static EditListLink_p frist=NULL,focus=NULL;
	int n=0;
	EDITRECT *showrect=&WORDOBG->EditShow;

	if(WORDOBG==NULL)
		return -1;
	if(WORDOBG->EditFocusNode==NULL)
		return -1;
	if(code==MR_MOUSE_DOWN)
	{
		move=0;//没有大范围移动
		tcode=code;
		tp1=p1;
		tp2=p2;
		
		if(FunctionType==1)
		{
			if(tp2>KeyShowX+(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace)//表示按下时在虚拟键盘区
			{
				PressX=(tp1-0)/(SCRW/5);
				PressY=(tp2-(showrect->y+showrect->h-EditKeyboardH))/(EditKeyboardH/5);
				PressY--;
				switch(PressX)
				{
				case 0:
					if(WordType==0||WordType==1)//字母
						switch(PressY)
					{
						case 0:
							EditInsertText(",");
							break;
						case 1:
							EditInsertText(";");
							break;
						case 2:
							EditInsertText("()");
							EditLeft();
							break;
						case 3:
							EditInsertText("{}");
							EditLeft();
							break;
					}
					if(WordType==2)//数字
						switch(PressY)
					{
						case 0:
							EditInsertText("+");
							break;
						case 1:
							EditInsertText("-");
							break;
						case 2:
							EditInsertText("*");
							break;
						case 3:
							EditInsertText("/");
							break;
					}
					break;
				case 1:
					switch(PressY)
					{
					case 0:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_1,0);
						break;
					case 1:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_4,0);
						break;
					case 2:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_7,0);
						break;
					case 3:
						break;
					}
					break;
				case 2:
					switch(PressY)
					{
					case 0:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_2,0);
						break;
					case 1:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_5,0);
						break;
					case 2:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_8,0);
						break;
					case 3:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_0,0);
						break;
					}
					break;
				case 3:
					switch(PressY)
					{
					case 0:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_3,0);
						break;
					case 1:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_6,0);
						break;
					case 2:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_9,0);
						break;
					case 3:
						if(WordType==0||WordType==1)//字母
						{//切换至全键盘
						}
						else
						{

							EditInsertText("=");
						}
						break;
					}
					break;
				case 4:
					switch(PressY)
					{
					case 0:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_SOFTRIGHT,0);
						break;
					case 1:
						if(EditFindLib==0)
							EditFindLib=1;
						else
							EditFindLib=0;
						CleanLib();
						break;
					case 2:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_POUND,0);
						break;
					case 3:
						//KeyboardEvent(MR_KEY_PRESS,MR_KEY_STAR,0);
						EditInsertText("\n");
						break;
					}
					break;
				}
			}
		}
	}
	if(code==MR_MOUSE_UP)
	{
		PressX=-1;PressY=-1;
		UDyidongyueshu();
		KeyFree();
	
		if(FunctionType==1&&tp2>showrect->y+showrect->h-(FunctionType==0 ? 0:EditKeyboardH))
		{
			PressX=(tp1-0)/(SCRW/5);
			PressY=(tp2-(showrect->y+showrect->h-EditKeyboardH))/(EditKeyboardH/5);
			PressY--;
			if(PressX==1&&PressY==3)
			{
				UnfoldSign();
				DrawEditor(0);
				return 0;
			}
		}
		if(((PressX==0||PressX==1)&PressY)==-1)
		{
			WordSelect(EditCiTiaoFocus);
		}
		if(((PressX==2||PressX==3)&PressY)==-1)
		{
			if(EditCiTiaoFocus!=NULL)
				WordSelect(EditCiTiaoFocus->next);
			else
				WordSelect(NULL);
		}
		if(PressX==4&&PressY==-1)
		{
			if(DaoHang==0)
				DaoHang=1;
			else
				DaoHang=0;
		}
		PressX=-1;PressY=-1;
	}
	DrawEditor(0);
	return 0;
}

//符号事件
int SignEvent(int32 code,int32 p1,int32 p2)
{
	char *str[5][5];
	static int tcode=0,tp1=0,tp2=0,tpos=0,move=0;
	static EditListLink_p frist=NULL,focus=NULL;
	int n=0;
	EDITRECT *showrect=&WORDOBG->EditShow;

	str[0][0]="+";str[0][1]="-";str[0][2]="*";str[0][3]="/";str[0][4]="<-";
	str[1][0]="&";str[1][1]="|";str[1][2]="!";str[1][3]="^";str[1][4]="返回";
	str[2][0]="()";str[2][1]="{}";str[2][2]="<>";str[2][3]="[]";str[2][4]=",";
	str[3][0]="%";str[3][1]="#";str[3][2]="\\";str[3][3]="?";str[3][4]=".";
	str[4][0]="_";str[4][1]=":";str[4][2]="\"\"";str[4][3]="''";str[4][4]="\n";
	if(WORDOBG->EditFocusNode==NULL)
		return 0;
	if(code==MR_MOUSE_DOWN)
	{
		move=0;//没有大范围移动
		tcode=code;
		tp1=p1;
		tp2=p2;
		tpos=WORDSHOUPOS;
		frist=WORDOBG->EditFristNode;
		focus=WORDOBG->EditFocusNode;
		n=(p2-showrect->y)/WORDOBG->EditShowSpace-1;
		if(tp2>KeyShowX+WORDOBG->EditShowSpace&&tp2<KeyShowX+(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace)//表示按下时在编辑区
		{
			WORDOBG->EditFocusNode=EditGetNextNNode(WORDOBG->EditFristNode,n>=WORDOBG->EditShowNum? WORDOBG->EditShowNum-1:n);
			WORDOBG->EditFocusNodePos=TextWN(WORDOBG->EditFocusNode->text,p1-KeyShowX-WORDSHOUPOS);
		}
		if(FunctionType==1)
		{
			if(tp2>KeyShowX+(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace)//表示按下时在虚拟键盘区
			{
				PressX=(tp1-0)/(SCRW/5);
				PressY=(tp2-(showrect->y+showrect->h-EditKeyboardH))/(EditKeyboardH/5);
				if((PressX>=0&&PressX<5)&&PressY>=0&&PressY<5)
				{
					SignEvent(MR_KEY_PRESS,MR_KEY_SELECT,0);
				}
			}
		}
	}
	if(code==MR_MOUSE_MOVE)
	{
		if(p1-tp1>=10||p2-tp2>=10||p1-tp1<=-10||p2-tp2<=-10)
			move=1;//大范围移动了
		if(move==1)
		{
			if(tp2>KeyShowX+WORDOBG->EditShowSpace&&tp2<KeyShowX+(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace)//表示按下时在编辑区
			{
				if(p2>tp2)//上移
				{
					WORDOBG->EditFristNode=EditGetPreNNode(frist,(p2-tp2)/WORDOBG->EditShowSpace);
					n=(p2-showrect->y)/WORDOBG->EditShowSpace-1;
					//EditFocusNode=EditGetNextNNode(WORDOBG->EditFristNode,n>=WORDOBG->EditShowNum? WORDOBG->EditShowNum-1:n);
				}
				if(p2<tp2)//下移
				{
					WORDOBG->EditFristNode=EditGetNextNNode(frist,(tp2-p2)/WORDOBG->EditShowSpace);
					n=(p2-showrect->y)/WORDOBG->EditShowSpace-1;
					//WORDOBG->EditFocusNode=EditGetNextNNode(WORDOBG->EditFristNode,n>=WORDOBG->EditShowNum? WORDOBG->EditShowNum-1:n);
				}
				WORDSHOUPOS=tpos+p1-tp1;
				WORDOBG->EditFocusNodePos=TextWN(WORDOBG->EditFocusNode->text,p1-KeyShowX-WORDSHOUPOS);
			}
		}
	}
	if(code==MR_MOUSE_UP)
	{
		PressX=-1;PressY=-1;
		UDyidongyueshu();
		KeyFree();
		//if()
		if(tp2>KeyShowX+(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace)//表示按下时在虚拟键盘区
		{
			PressX=(tp1-0)/(SCRW/5);
			PressY=(tp2-(showrect->y+showrect->h-EditKeyboardH))/(EditKeyboardH/5);
			if((PressX>=0&&PressX<5)&&PressY>=0&&PressY<5)
			{
				if(PressX==4&&PressY==0)
					DelText();
				if(PressX==4&&PressY==1)
				{
					SignType=0;
					if(SignPreFunction==0&&FunctionType!=0)//之前没有展开，现在关闭它
					{
						UnfoldKeyboard();
					}
					PressX=-1;
					PressY=-1;
					DrawEditor(0);
					return 0;
				}
			}
		}
		if(move==0)
		{
			if(tp1>SCRW-WORDOBG->EditShowSpace*2 && tp2 > showrect->y+(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace&& tp1<SCRW-WORDOBG->EditShowSpace*2+WORDOBG->EditShowSpace*2&&tp2<showrect->y+(WORDOBG->EditShowNum+2)*WORDOBG->EditShowSpace)
			{
				UnfoldKeyboard();
				DrawEditor(0);
				return 0;
			}
		}
	}
	if(code==MR_KEY_PRESS)
	{
		switch(p1)
		{
		case MR_KEY_SOFTRIGHT:
			break;
		case MR_KEY_SELECT:
		case MR_KEY_SOFTLEFT:
			if(PressX>=0&&PressX<5&&PressY>=0&&PressY<5&&!(PressX==4&&PressY==0)&&!(PressX==4&&PressY==1))
				EditInsertText(str[PressY][PressX]);
			break;
		case  MR_KEY_UP:
			PressY--;
			if(PressY<0)
				PressY=4;
			break;
		case  MR_KEY_DOWN:
			PressY++;
			if(PressY>4)
				PressY=0;
			break;
		case  MR_KEY_LEFT:
			PressX--;
			if(PressX<0)
				PressX=4;
			break;
		case  MR_KEY_RIGHT:
			PressX++;
			if(PressX>4)
				PressX=0;
			break;
		}
	}
	if(code==MR_KEY_RELEASE)
	{
		switch(p1)
		{
		case MR_KEY_POUND:
		case MR_KEY_SOFTRIGHT:
			SignType=0;
			if(SignPreFunction==0&&FunctionType!=0)//之前没有展开，现在关闭它
			{
				UnfoldKeyboard();
			}
			PressX=-1;PressY=-1;
			break;
		}
	}
	DrawEditor(0);
	return 0;
}

//导航事件
int DaoHangEvent(int32 code,int32 p1,int32 p2)
{
	static int tcode=0,tp1=0,tp2=0,tpos=0,move=0;
	static EditListLink_p frist=NULL,focus=NULL;
	int n=0,m=0;
	EDITRECT *showrect=&WORDOBG->EditShow;

	if(WORDOBG->EditFocusNode==NULL)
		return 0;
	if(code==MR_MOUSE_DOWN)
	{
		move=0;//没有大范围移动
		tcode=code;
		tp1=p1;
		tp2=p2;
		if(FunctionType==1)
		{
			if(tp2>KeyShowX+(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace)//表示按下时在虚拟键盘区
			{
				PressX=(tp1-0)/(SCRW/5);
				PressY=(tp2-(showrect->y+showrect->h-EditKeyboardH))/(EditKeyboardH/5);
				PressY--;
				switch(PressX)
				{
				case 1:
					switch(PressY)
					{
					case 1:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_LEFT,0);
						break;
					}
					break;
				case 2:
					switch(PressY)
					{
					case 0:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_UP,0);
						break;
					case 1:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_SELECT,0);
						break;
					case 2:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_DOWN,0);
						break;
					}
					break;
				case 3:
					switch(PressY)
					{
					case 1:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_RIGHT,0);
						break;
					}
					break;
				case 4:
					switch(PressY)
					{
					case 0:
						KeyboardEvent(MR_KEY_PRESS,MR_KEY_SOFTRIGHT,0);
						break;
					case 3:
						break;
					}
					break;
				}
			}
		}
	}
	
	if(code==MR_MOUSE_UP)
	{
		PressX=-1;PressY=-1;
		UDyidongyueshu();
		KeyFree();

		PressX=(tp1-0)/(SCRW/5);
		PressY=(tp2-(showrect->y+showrect->h-EditKeyboardH))/(EditKeyboardH/5);
		m=PressX+PressY*5;
		if(m==0||m==1)
		{
			WordSelect(EditCiTiaoFocus);
		}
		if(m==2||m==3)
		{
			if(EditCiTiaoFocus!=NULL)
				WordSelect(EditCiTiaoFocus->next);
			else
				WordSelect(NULL);
		}
		if(m==4)
		{
			if(DaoHang==1)
				DaoHang=0;
			else
				DaoHang=1;
		}
		switch(m)
		{
		case 14://复制
			CopyStrText();
			break;
		case 19:
			//剪切
			break;
		case 21://起点
			EditSetCopyStart();
			break;
		case 22://终点
			EditSetCopyEnd();
			break;
		case 23://取消
			CleanSelect();
			break;
		case 24://粘贴
			PasteStrText();
			break;
		}
		if(m==5||m==10||m==15||m==20)
		{
			DaoHang=0;
		}
		PressX=-1;
		PressY=-1;
	}
	DrawEditor(0);
	return 0;
}
int32 EditReLoad()
{
	EditKeyboardH=SCRH/2;
	EditFindLib=0;
	eventcount=0;
	InputTimer=0;//按键循环标志//按键延时输入timer

	WordType=0;//0小写,1大写,2数字
	PressX=PressY=-1;//按下的键位

	EditKeyboardType=0;//虚拟键盘类型
	EditFindLib=0;//是否从词库中搜索
	FunctionType=0;  //工具箱类型 0键盘收拢，1键盘展开
	SignType=0;//标点符号状态优先于function，0关闭，1打开
	SignPreFunction=0;
	DaoHang=0;//方向键是否展开
	EditWordLib=NULL;//词库链表
	EditListTemp;//临时链表
	EditCiTiao=NULL;//词条链表
	EditCiTiaoFocus=NULL;//词条焦点
	memset(&FunctionText[0],0,100);//最大100;

	EditLoadWordLib(WORDLIBFILE);//词库
	InputTimer=mrc_timerCreate();
	keyedt[0][0]='a';keyedt[1][0]=3;
	keyedt[0][1]='d';keyedt[1][1]=3;
	keyedt[0][2]='g';keyedt[1][2]=3;
	keyedt[0][3]='j';keyedt[1][3]=3;
	keyedt[0][4]='m';keyedt[1][4]=3;
	keyedt[0][5]='p';keyedt[1][5]=4;
	keyedt[0][6]='t';keyedt[1][6]=3;
	keyedt[0][7]='w';keyedt[1][7]=4;



	return 0;
}



void key_close()
{

	if(EditWordLib!=NULL)
		SaveListToFile(EditWordLib,WORDLIBFILE);
	DelAllEditListNode(EditListTemp);
	DelAllEditListNode(EditWordLib);
	DelAllEditListNode(EditCiTiao);

	EditListTemp=NULL;//临时链表
	EditWordLib=NULL;//词库链表
	EditCiTiao=NULL;//词条链表
	EditCiTiaoFocus=NULL;//词条焦点
	if(InputTimer)
		mrc_timerDelete(InputTimer);//按键延时输入


	InputTimer=0;

}


int32  key_EditPause()//编辑器暂停，暂停所有定时器
{
	mrc_timerStop(InputTimer);
	return 0;
}


int32 VirtualKeyEvent(int32 code, int32 p1, int32 p2)
{
	if(SignType==0)//非符号状态
	{
		if(DaoHang==0)
		{
			if(code==MR_KEY_PRESS||code==MR_KEY_RELEASE)
			{
				return KeyboardEvent(code,p1,p2);
			}
			if(code==MR_MOUSE_DOWN||code==MR_MOUSE_MOVE||code==MR_MOUSE_UP)
			{
				return MouseEvent(code,p1,p2);
			}
		}
		else
		{
			if(code==MR_KEY_PRESS||code==MR_KEY_RELEASE)
			{
				return KeyboardEvent(code,p1,p2);
			}
			if(code==MR_MOUSE_DOWN||code==MR_MOUSE_MOVE||code==MR_MOUSE_UP)
			{
				return DaoHangEvent(code,p1,p2);
			}
		}
	}
	if(SignType==1)//符号状态
	{
		return SignEvent(code,p1,p2);
	}
	return -1;

}