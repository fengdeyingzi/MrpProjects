#include "mrc_base.h"
#include "HighLight.h"
#include "editor.h"
#include "font.h"



 void output(char *text)
{
	int32 hand=0;
	hand=mrc_open("print.txt",8|2);
	mrc_seek(hand,0,MR_SEEK_END);
	mrc_write(hand,text,mrc_strlen(text));
	mrc_close(hand);
}
//#define   isprint(ch)  !(ch >= 0x20 && ch <= 0x7e)//ASCII可打印字符
////////////////////////////////////////



EDITWOED_P working,workingheat;

//////////////////全局量//////////////////////
mr_colourSt BackCol,FontCol,FocusCol,FocusFontCol,OtherCol;//各种颜色
static char *CopyStr=NULL;

static int32 EditFocusTimerHand=0,KeyTimerLoop=0,EditKeyTimerHand=0;
static EditListLink_p 		   CopyStrStartNode=NULL,//复制起点
		   CopyStrEndNode=NULL//复制终点
		   ;
 int 	EditLineAll=0,//总行数
	EditLineFocus=0,//焦点行

		CopyStrStartNodePos=0,//复制起点
		CopyStrEndNodePos=0//复制终点
	;



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
///////////////////功能/////////////////////

EditListLink_p EditMakeNode(char *text)//制造结点
{
	EditListLink_p temp;
	temp=(EditListLink_p )mrc_malloc(sizeof(EditListLink));
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
	return temp;
}
EditListLink_p InsertEditListNode(EditListLink_p pre,EditListLink_p node)//插入结点
{
	EditListLink_p next=NULL;
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
EditListLink_p DelAEditListNode(EditListLink_p head,EditListLink_p node)//删除一个
{
	EditListLink_p pre=NULL,next=NULL;
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
void DelAllEditListNode(EditListLink_p head)//删除全部行
{
	if(head==NULL)
		return ;
	while(head->next!=NULL)
		head=DelAEditListNode(head,head->next);
	head=DelAEditListNode(head,head);
}
EditListLink_p EditGetEndNode(EditListLink_p head)//通过一个结点获取尾结点
{
	if(head==NULL)
		return NULL;
	while(head->next!=NULL)
		head=head->next;
	return head;
}
EditListLink_p EditGetNextNNode(EditListLink_p node,int n)
{
	EditListLink_p temp=NULL;
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
EditListLink_p EditGetPreNNode(EditListLink_p node,int n)
{
	EditListLink_p temp=NULL;
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
int GotoLine(int line)
{
	EditListLink_p temp=NULL;
	temp=EditGetNextNNode(WORDOBG->EditListHead,line);
	if(temp==NULL)
		return -1;
	WORDOBG->EditFocusNode=temp;
	WORDOBG->EditFocusNodePos=0;
	UDyidongyueshu();
	return 0;
}

int GetLineLen(char *scr)//获取一行的长度
{
	int i=0;
	for(i=0;scr[i]!=0;i++)
		if(scr[i]=='\n')
			return i+1;
	return i;
}
void GetObgText(char ***dt)
{
	*dt=&(WORDOBG->EditFocusNode->text);
}
EditListLink_p EditReadLine(char *scr,int *len,EditListLink_p ListHead)//读取一行并解析，中间函数请慎用
{
	char *text=NULL;
	if(scr==NULL)
		return NULL;
	*len=GetLineLen(scr);
	text=mrc_malloc(*len+1);
	if(text==NULL)
	{
		output("读取行时遇到内存不足\n");
		return NULL;
	}
	text[*len]=0;
	mrc_strncpy(text,scr,*len);
	if(*len>0&&text[*len-1]=='\n')
	{
		text[*len-1]=0;
		if(*len>1&&text[*len-2]==0x0d)
			text[*len-2]=0;
	}
	if(ListHead==NULL)
	{
		ListHead=EditMakeNode(text);
		mrc_free(text);
		return ListHead;
	}
	else
	{
		InsertEditListNode(EditGetEndNode(ListHead),EditMakeNode(text));
		mrc_free(text);
	}
	return NULL;
}
EditListLink_p MakeEditList(char *scr)//通过一段文本制造一条链表
{
	EditListLink_p temp=NULL,t=NULL;
	int pos=0,len=0,dlen=0;
	if(scr==NULL)
		return NULL;
	len=mrc_strlen(scr);
	while(1)
	{
		t=EditReadLine(scr+pos,&dlen,temp);
		if(t!=NULL)
			temp=t;
		pos+=dlen;
		if(pos>=len)
			break;
	}
	if(len>0&&scr[len-1]=='\n')
	{
		InsertEditListNode(EditGetEndNode(temp),EditMakeNode(""));
	}
	return temp;
}
int EditLoadMccFile(char *filename)//加载文件,会自动释放之前的head
{
	int len=0,filehand=0;
	char *temp=NULL;


	if(mrc_fileState(filename)!=MR_IS_FILE)
	{
		output("文件'");
		output(filename);
		output("'不存在\n");
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

	WORDOBG=NowWord();
	WORDOBG->EditListHead=MakeEditList(temp);
	mrc_free(temp);
	mrc_close(filehand);
	return 0;
}
int EditGetTwoNodeLen(EditListLink_p pre,EditListLink_p next)//获取两个结点间的距离
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
int EditGetLinkLen(EditListLink_p head)
{
	int i=0;
	for(i=0;head!=NULL;i++)
	{
		head=head->next;
	}
	return i;
}
void yueshu()//约束函数，为InsertText函数的中间函数，请慎用
{
	UDyidongyueshu();
	/*
	int twonodelen=0,i=0;
	twonodelen=EditGetTwoNodeLen(WORDOBG->EditFristNode,WORDOBG->EditFocusNode);
	if(twonodelen>=WORDOBG->EditShowNum)
	{
		WORDOBG->EditFristNode=WORDOBG->EditFocusNode;
		for(i=1;i<WORDOBG->EditShowNum;i++)
			WORDOBG->EditFristNode=WORDOBG->EditFristNode->pre;
	}
	*/
}
int str_nnum(char *text)
{
	int n=0,i=0;
	for(;text[i]!=0;i++)
	{
		if(text[i]=='\n')
			n++;
	}
	return n;
}
int strendfind_n(char *text)//获取最后一个换行到末尾的距离
{
	int flg=-1,i=0,len=mrc_strlen(text);
	for(;text[i]!=0;i++)
	{
		if(text[i]=='\n')
			flg=i+1;
	}
	return len-flg;
}
int getstrn(char *text,int pos)//获取离pos最近的有效子的pos
{
	int i=0,len=0;
	len=mrc_strlen(text);
	for(;text[i]!=0;)
	{
		if(i>=pos)
			break;
		if((uint8)text[i]<=0x7f)
			i++;
		else
		{
			if(i<=len-2)//汉字
				i+=2;
			else
				i++;
		}
	}
	return i;
}
void InsertText(EditListLink_p node,int pos,char *text)//插入一段文本,前两个参数无效
{
	char *temp=NULL;
	int endlen=0,nodetextlen=0,templen=0,textlen=0;
	EditListLink_p tlink=NULL,tp=NULL,tpn=NULL;
	node=WORDOBG->EditFocusNode;
	pos=WORDOBG->EditFocusNodePos;
	if(node==NULL||pos<0||pos>mrc_strlen(node->text))
		return ;
	if(WORDOBG->EditListHead==NULL||node==NULL)
		return ;
	nodetextlen=mrc_strlen(node->text);
	textlen=mrc_strlen(text);
	endlen=nodetextlen-pos;
	temp=mrc_malloc(nodetextlen+textlen+2);
	if(temp==NULL)
	{
		output("插入文字时遇到内存不足\n");
		return ;
	}
	mrc_memset(temp,0,nodetextlen+textlen+2);
	mrc_strncpy(temp,node->text,pos);
	mrc_strcat(temp,text);
	templen=mrc_strlen(temp);
	mrc_strcat(temp,node->text+pos);
	if(mrc_strchr(text,'\n')==NULL)//没有换行
	{
		mrc_free(node->text);
		node->text=temp;
		WORDOBG->EditFocusNodePos+=textlen;
		LRyidongyueshu();
		return ;
	}
	WORDOBG->EditLineAll+=str_nnum(text);//总行数加换行数
	tlink=MakeEditList(temp);
	if(tlink==NULL)
	{
		mrc_free(temp);
		return ;
	}
	if(node==WORDOBG->EditListHead)
	{
		WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->next;
		tpn=node;
		WORDOBG->EditListHead=WORDOBG->EditListHead->next;
		tp=EditGetEndNode(tlink);
		tp->next=WORDOBG->EditListHead;
		if(WORDOBG->EditListHead!=NULL)
		WORDOBG->EditListHead->pre=tp;
		WORDOBG->EditListHead=tlink;
		WORDOBG->EditFristNode=tlink;
		WORDOBG->EditFocusNode=tp;
		WORDOBG->EditFocusNodePos=mrc_strlen(WORDOBG->EditFocusNode->text)-endlen;
		UDyidongyueshu();
		mrc_free(tpn->text);
		mrc_free(tpn);
		tpn=NULL;
		mrc_free(temp);
		return ;
	}
	if(node==WORDOBG->EditFristNode)
	{
		WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->next;
		tpn=node;
		tp=EditGetEndNode(tlink);
		node->pre->next=tlink;
		tlink->pre=node->pre;
		node->next->pre=tp;
		tp->next=node->next;
		WORDOBG->EditFristNode=tlink;
		WORDOBG->EditFocusNode=tp;
		WORDOBG->EditFocusNodePos=mrc_strlen(WORDOBG->EditFocusNode->text)-endlen;
		UDyidongyueshu();
		mrc_free(tpn->text);
		mrc_free(tpn);
		tpn=NULL;
		mrc_free(temp);
		return ;
	}
	
	if(node->next==NULL)
	{
		WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->next;
		tpn=node;
		tp=EditGetEndNode(tlink);
		node->pre->next=tlink;
		tlink->pre=node->pre;
		if(node->next!=NULL)
			node->next->pre=tp;
		tp->next=node->next;
		WORDOBG->EditFocusNode=tp;
		WORDOBG->EditFocusNodePos=mrc_strlen(WORDOBG->EditFocusNode->text)-endlen;
		UDyidongyueshu();
		mrc_free(tpn->text);
		mrc_free(tpn);
		tpn=NULL;
		mrc_free(temp);
		return ;
	}
		WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->next;
		tpn=node;
		tp=EditGetEndNode(tlink);
		node->pre->next=tlink;
		tlink->pre=node->pre;
		node->next->pre=tp;
		tp->next=node->next;
		WORDOBG->EditFocusNode=tp;
		WORDOBG->EditFocusNodePos=mrc_strlen(WORDOBG->EditFocusNode->text)-endlen;
		UDyidongyueshu();
		mrc_free(tpn->text);
		mrc_free(tpn);
		tpn=NULL;
		mrc_free(temp);
		return ;
	mrc_free(temp);
}

int TextWN(char *text,int w)//测试一段字符要w宽需要多长
{
	int tw=0,h=0,len=0,i=0,allw=0;
	char temp[4];
	len=mrc_strlen(text);
	if(w<=0)
		return 0;
	for(i=0;i<len;)
	{
		if(text[i]==0)
			break;
		((int *)temp)[0]=0;
		if((uint8)text[i]<=0x7f)//英文
		{
			temp[0]=text[i];
			i++;
		}
		else
		{
			if((uint8)text[i]!=0)//中文
			{
				temp[0]=text[i];
				i++;
				temp[1]=text[i];
				i++;
			}
			else//未知
			{
				temp[0]=text[i];
				i+=1;
			}
		}
		fontwh(temp,0,(uint16)FONT,(int32*)&tw,(int32*)&h);
		allw+=tw;
		if(allw>=w-tw/2)
			break;
	}
	return i;
}
int TextNW(char *text,int len)//测试一段字符前len的显示宽度
{
	int w=0,h=0;
	char temp[2];//=(char*)mrc_malloc(len+2);//+2不解释
	if(text==NULL)
	{
		return 0;
	}
	temp[0]=text[len];
	temp[1]=text[len+1];
	text[len]=0;
	text[len+1]=0;
	fontwh(text,0,(uint16)FONT,(int32*)&w,(int32*)&h);
	text[len]=temp[0];
	text[len+1]=temp[1];
	return w;
}
void LRyidongyueshu()//左右移约束
{
	int len=0,textfocusx=0;
	EDITRECT *showrect=&WORDOBG->EditShow;

	len=mrc_strlen(WORDOBG->EditFocusNode->text);
	if(WORDSHOUPOS >0)
		WORDSHOUPOS=0;
	if(len<=0)
		WORDOBG->EditFocusNodePos=0;
	if(len<=WORDOBG->EditFocusNodePos)
		WORDOBG->EditFocusNodePos=WORDOBG->EditFocusNodePos;
	textfocusx=TextNW(WORDOBG->EditFocusNode->text,WORDOBG->EditFocusNodePos);
			if(showrect->x+WORDSHOUPOS+textfocusx<showrect->x+10)
			{
				if(textfocusx>50)
					WORDSHOUPOS=0-showrect->x-textfocusx+50;//WORDSHOUPOS+=50;
				else
					WORDSHOUPOS=0;
			}
			else
			{
				if(showrect->x+WORDSHOUPOS+textfocusx>=showrect->w+showrect->x-10)
					WORDSHOUPOS=showrect->w-textfocusx-50;
			}
}
void UDyidongyueshu()//上下移移约束
{
	int twonodelen=-1,i=0;
	if(WORDOBG->EditFristNode==NULL)
		return ;
	if(WORDOBG->EditFristNode->pre==WORDOBG->EditFocusNode)//加快速度
		twonodelen=-1;
	else
		twonodelen=EditGetTwoNodeLen(WORDOBG->EditFristNode,WORDOBG->EditFocusNode);
	if(twonodelen>=WORDOBG->EditShowNum)//该下移了
		if(WORDOBG->EditFristNode->next!=NULL)
		{
	if(twonodelen>=WORDOBG->EditShowNum)
	{
		WORDOBG->EditFristNode=WORDOBG->EditFocusNode;
		for(i=1;i<WORDOBG->EditShowNum;i++)
			WORDOBG->EditFristNode=WORDOBG->EditFristNode->pre;
	}
		}
	if(twonodelen<0)//该上移了
		if(WORDOBG->EditFristNode->next!=NULL)
		{
			WORDOBG->EditFristNode=WORDOBG->EditFocusNode;
		}
		LRyidongyueshu();
}
void DelText()//删除文本
{
	//int flg=0;
	//EditListLink_p temp=NULL;
	if(WORDOBG->EditFristNode==NULL||WORDOBG->EditFocusNode==NULL||WORDOBG->EditListHead==NULL)
		return ;
	if(WORDOBG->EditFocusNodePos==0)
	{
		if(WORDOBG->EditFocusNode==WORDOBG->EditListHead)
		{
			//LRyidongyueshu();
			return;
		}
		WORDOBG->EditLineAll--;//总行数--
		if(WORDOBG->EditFocusNode==WORDOBG->EditFristNode||WORDOBG->EditFocusNode==WORDOBG->EditFristNode->next)
		{
			WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->pre;
			WORDOBG->EditFocusNodePos=mrc_strlen(WORDOBG->EditFocusNode->text);
			EditInsertText(WORDOBG->EditFocusNode->next->text);
			WORDOBG->EditFocusNodePos-=mrc_strlen(WORDOBG->EditFocusNode->next->text);
			WORDOBG->EditListHead=DelAEditListNode(WORDOBG->EditListHead,WORDOBG->EditFocusNode->next);
			WORDOBG->EditFristNode=WORDOBG->EditFocusNode;
			UDyidongyueshu();
			return ;
		}
		else
		{
			WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->pre;
			WORDOBG->EditFocusNodePos=mrc_strlen(WORDOBG->EditFocusNode->text);
			EditInsertText(WORDOBG->EditFocusNode->next->text);
			WORDOBG->EditFocusNodePos-=mrc_strlen(WORDOBG->EditFocusNode->next->text);
			WORDOBG->EditListHead=DelAEditListNode(WORDOBG->EditListHead,WORDOBG->EditFocusNode->next);
			UDyidongyueshu();
			return;
		}
	}
	else if(WORDOBG->EditFocusNodePos==1)
	{
		mrc_strcpy(WORDOBG->EditFocusNode->text,WORDOBG->EditFocusNode->text+1);
		WORDOBG->EditFocusNodePos=0;
		LRyidongyueshu();
		return ;
	}
	else
	{
		if((uint8)WORDOBG->EditFocusNode->text[WORDOBG->EditFocusNodePos-1]>0x7f)
		{
			if(WORDOBG->EditFocusNode->text[WORDOBG->EditFocusNodePos-2]!=0)//是汉字
			{
				mrc_strcpy(WORDOBG->EditFocusNode->text+WORDOBG->EditFocusNodePos-2,WORDOBG->EditFocusNode->text+WORDOBG->EditFocusNodePos);
				WORDOBG->EditFocusNodePos-=2;
				LRyidongyueshu();
				return ;
			}
		}
		mrc_strcpy(WORDOBG->EditFocusNode->text+WORDOBG->EditFocusNodePos-1,WORDOBG->EditFocusNode->text+WORDOBG->EditFocusNodePos);
		WORDOBG->EditFocusNodePos-=1;
			LRyidongyueshu();
		return ;
	}
}
void EditUp()//光标上移
{
	int len;
	if(WORDOBG->EditFocusNode!=NULL)
	if(WORDOBG->EditFocusNode->pre!=NULL)
	{
		WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->pre;
		len=strlen(WORDOBG->EditFocusNode->text);
		if(len<WORDOBG->EditFocusNodePos)
			WORDOBG->EditFocusNodePos=len;
		else
		{
			WORDOBG->EditFocusNodePos=getstrn(WORDOBG->EditFocusNode->text,WORDOBG->EditFocusNodePos);
		}
		UDyidongyueshu();
	}
}
void EditDown()//光标下移
{
	int len;
	if(WORDOBG->EditFocusNode!=NULL)
	if(WORDOBG->EditFocusNode->next!=NULL)
	{
		WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->next;
		len=strlen(WORDOBG->EditFocusNode->text);
		if(len<WORDOBG->EditFocusNodePos)
			WORDOBG->EditFocusNodePos=len;
		else
		{
			WORDOBG->EditFocusNodePos=getstrn(WORDOBG->EditFocusNode->text,WORDOBG->EditFocusNodePos);
		}
		UDyidongyueshu();
	}
}


void EditLeft()//光标左移
{
	//int twonodelen=0,textfocusx=0;
	if(WORDOBG->EditFristNode==NULL||WORDOBG->EditFocusNode==NULL||WORDOBG->EditListHead==NULL)
		return ;
	if(WORDOBG->EditFocusNodePos==1&&(WORDOBG->EditFocusNodePos=0)==0)goto End1;

	if(WORDOBG->EditFocusNodePos>0)
	{
		if((uint8)WORDOBG->EditFocusNode->text[WORDOBG->EditFocusNodePos-1]>0x7f)
		{
			if(WORDOBG->EditFocusNodePos>=2 && WORDOBG->EditFocusNode->text[WORDOBG->EditFocusNodePos-2]!=0)//是汉子占两字节
			{
				WORDOBG->EditFocusNodePos-=2;
				goto End2;
			}
			else//未知的字符占一字节
			{
				WORDOBG->EditFocusNodePos--;
				goto End2 ;
			}
		}
		else//是英文字占1字节
		{
			WORDOBG->EditFocusNodePos--;
			goto End2;
		}
	}
	else//该换行了
	{
		if(WORDOBG->EditFocusNode->pre!=NULL)
		{
			WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->pre;
			WORDOBG->EditFocusNodePos=strlen(WORDOBG->EditFocusNode->text);
			WORDSHOUPOS=TextNW(WORDOBG->EditFocusNode->text,WORDOBG->EditFocusNodePos);
			if(WORDSHOUPOS>WORDOBG->EditShow.x+WORDOBG->EditShow.w-10)WORDSHOUPOS=-(WORDSHOUPOS-WORDOBG->EditShow.x+WORDOBG->EditShow.w+50);
			else WORDSHOUPOS=0;
			goto End1;
		}
		else
		goto End1 ;
	}
	return ;
End1:
	/*
	twonodelen=EditGetTwoNodeLen(WORDOBG->EditFocusNode,WORDOBG->EditFristNode);
	if(twonodelen>0)
		if(WORDOBG->EditFristNode->pre!=NULL)
		{
			WORDOBG->EditFristNode=WORDOBG->EditFristNode->pre;
		} */
		UDyidongyueshu();
	return ;
End2:
		LRyidongyueshu();
		/*
	textfocusx=TextNW(WORDOBG->EditFocusNode->text,WORDOBG->EditFocusNodePos);
	if(showrect->x+WORDSHOUPOS+textfocusx<50)
	{
		WORDSHOUPOS+=50;
		WORDSHOUPOS=WORDSHOUPOS>0?0:WORDSHOUPOS;
	}*/
	return ;

}

void EditRight()//光标右移
{
	//int twonodelen=0,textfocusx=0;
	if(WORDOBG->EditFristNode==NULL||WORDOBG->EditFocusNode==NULL||WORDOBG->EditListHead==NULL)
		return ;
	if(WORDOBG->EditFocusNode->text[WORDOBG->EditFocusNodePos]!=0)
	{
		if((uint8)WORDOBG->EditFocusNode->text[WORDOBG->EditFocusNodePos]>0x7f)
		{
			if(WORDOBG->EditFocusNode->text[WORDOBG->EditFocusNodePos+1]!=0)//是汉子占两字节
			{
				WORDOBG->EditFocusNodePos+=2;
				goto End1;
			}
			else//未知的字符占一字节
			{
				WORDOBG->EditFocusNodePos+=1;
				goto End1 ;
			}
		}
		else//是英文字占1字节
		{
				WORDOBG->EditFocusNodePos+=1;
				goto End1 ;
		}
	}
	else//该换行了
	{
		if(WORDOBG->EditFocusNode->next!=NULL)
		{
			WORDOBG->EditFocusNode=WORDOBG->EditFocusNode->next;
			WORDOBG->EditFocusNodePos=0;
			WORDSHOUPOS=0;
		}
		goto End2 ;
	}
	return ;
End1:
	LRyidongyueshu();
	/*
	textfocusx=TextNW(WORDOBG->EditFocusNode->text,WORDOBG->EditFocusNodePos);
	if(showrect->x+WORDSHOUPOS+textfocusx>=SCRW-10)
		WORDSHOUPOS-=50;
		*/
	return ;
End2:
	UDyidongyueshu();
	/*
	twonodelen=EditGetTwoNodeLen(WORDOBG->EditFristNode,WORDOBG->EditFocusNode);
	if(twonodelen>=EditShowNum)
		if(WORDOBG->EditFristNode->next!=NULL)
			WORDOBG->EditFristNode=WORDOBG->EditFristNode->next;
			*/
	return ;
}
int SaveListToFile(EditListLink_p head,char *filename)
{
	int hand=0;
	if(mrc_fileState(filename)==MR_IS_FILE)
	{
		mrc_remove(filename);
	}
	hand=mrc_open(filename,8|2);
	if(hand<=0)
	{
		output("打开文件失败\n");
		return -1;
	}
	mrc_seek(hand,0,MR_SEEK_END);
	while(head)
	{
		mrc_write(hand,head->text,mrc_strlen(head->text));
		if(head->next!=NULL)
			mrc_write(hand,"\n",1);
		else
			break;
		head=head->next;
	}
	mrc_close(hand);
	return 0;
}
///////////////////界面/////////////////////
int GetEditGetTwoNodeLen()
{
	return EditGetTwoNodeLen(WORDOBG->EditListHead,WORDOBG->EditFocusNode)+1;
}
void DrawMsg()
{
	char text[70];
	EDITRECT *showrect=&WORDOBG->EditShow;
	text[69]=0;
	WORDOBG->EditLineFocus=GetEditGetTwoNodeLen();
	if(WORDOBG->EditFocusNode!=NULL)
	{
		mrc_sprintf(text,"line:%d/%d  pos:%d/%d",WORDOBG->EditLineFocus,WORDOBG->EditLineAll,WORDOBG->EditFocusNodePos,mrc_strlen(WORDOBG->EditFocusNode->text));
	}
	else
	{
		mrc_strcpy(text,"没有打开文件");
	}
	mrc_drawRect(showrect->x,showrect->y,showrect->w,WORDOBG->EditShowSpace,OtherCol.r,OtherCol.g,OtherCol.b);
	gbsez(text,showrect->x,showrect->y,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
}
int DrawFocusNodeN=0;//为光标的显示标志
int GetFocusLine()
{
	return EditGetTwoNodeLen(WORDOBG->EditFristNode,WORDOBG->EditFocusNode);
}
void DrawFocusNode(int32 data)//画焦点行，及判断光标是否显示
{
	int twonodelen=0,textfocusx=0;
	EDITRECT *showrect;
	if(WORDOBG==NULL&& !WORDOBG->EditShowFocus)
		return;
	showrect=&WORDOBG->EditShow;
	if(WORDOBG->EditFristNode==NULL||WORDOBG->EditFocusNode==NULL||WORDOBG->EditListHead==NULL)
		return ;
	twonodelen=GetFocusLine();
	DrawMsg();
	if(twonodelen==-1||twonodelen>=WORDOBG->EditShowNum)
		return ;
	if(WORDOBG->EditShowStatus)
		showrect->y+=WORDOBG->EditShowSpace;
	mrc_drawRect(WORDOBG->EditShowX+showrect->x,showrect->y+twonodelen*WORDOBG->EditShowSpace,showrect->w-WORDOBG->EditShowX*2,WORDOBG->EditShowSpace,FocusCol.r,FocusCol.g,FocusCol.b);
	if(WORDOBG->EditFocusNode->text!=NULL)
	{
		gbsez(WORDOBG->EditFocusNode->text,showrect->x+WORDOBG->EditShowX+WORDSHOUPOS,showrect->y+twonodelen*WORDOBG->EditShowSpace,FocusFontCol.r,FocusFontCol.g,FocusFontCol.b,0,(uint16)FONT);
		if(WORDOBG->Hightype)
			DrawTextKeyWord(WORDOBG->EditFocusNode->text,showrect->x+WORDOBG->EditShowX+WORDSHOUPOS,showrect->y+twonodelen*WORDOBG->EditShowSpace);
	}
	textfocusx=TextNW(WORDOBG->EditFocusNode->text,WORDOBG->EditFocusNodePos);
	if(WORDOBG->EditShowCursor)
	if(DrawFocusNodeN==0)
		mrc_drawRect(showrect->x+WORDOBG->EditShowX+WORDSHOUPOS+textfocusx,showrect->y+twonodelen*WORDOBG->EditShowSpace,2,WORDOBG->EditShowSpace,0,0,0);
	else 
		if(DrawFocusNodeN==1)
		mrc_drawRect(showrect->x+WORDOBG->EditShowX+WORDSHOUPOS+textfocusx,showrect->y+twonodelen*WORDOBG->EditShowSpace,2,WORDOBG->EditShowSpace,255,255,255);
		if(CopyStrStartNode!=NULL)
		{
			if(CopyStrStartNode==WORDOBG->EditFocusNode&&CopyStrStartNodePos>=0)
			{
				textfocusx=TextNW(WORDOBG->EditFocusNode->text,CopyStrStartNodePos);
				mrc_drawRect(showrect->x+WORDOBG->EditShowX+WORDSHOUPOS+textfocusx,showrect->y+twonodelen*WORDOBG->EditShowSpace,2,4,255,0,0);
			}
		}
		if(CopyStrEndNode!=NULL)
		{
			if(CopyStrEndNode==WORDOBG->EditFocusNode&&CopyStrEndNodePos>=0)
			{
				textfocusx=TextNW(WORDOBG->EditFocusNode->text,CopyStrEndNodePos);
				mrc_drawRect(showrect->x+WORDOBG->EditShowX+WORDSHOUPOS+textfocusx,showrect->y+(twonodelen+1)*WORDOBG->EditShowSpace-4,2,4,255,0,0);
			}
		}
	if(data)
		mrc_refreshScreen(showrect->x,showrect->y+twonodelen*WORDOBG->EditShowSpace,showrect->w,WORDOBG->EditShowSpace);
	DrawFocusNodeN++;
	if(DrawFocusNodeN>=3)
		DrawFocusNodeN=0;
	if(WORDOBG->EditShowStatus)
		showrect->y-=WORDOBG->EditShowSpace;
}
void ReDrawFocusNode()//重画焦点行，及判断光标是否显示
{
	DrawFocusNodeN=0;
	DrawFocusNode(0);
}
//画滚动条
void Scrollbar(int moed,int big,int x,int y,int w,int h)
{
	int shor=20;
	if(!big||big<=moed)
		return;
	mrc_EffSetCon(x,y,w,h,200,200,200);
	h-=shor;
	mrc_EffSetCon(x,y+h*moed/big,w,shor,100,100,100);
}
void DrawEditor(int data)//画编辑器
{
	int i=0,textfocusx=0,hh=0;
	EditListLink_p temp=NULL;
	EDITRECT *showrect;
	if(!WORDOBG)
		return;
	showrect=&WORDOBG->EditShow;
	if(WORDOBG->EditListHead==NULL||WORDOBG->EditFristNode==NULL)
		return ;
	temp=WORDOBG->EditFristNode;
	if(WORDOBG->EditShowStatus)
		showrect->y+=WORDOBG->EditShowSpace;
	mrc_drawRect(showrect->x,showrect->y,showrect->w,showrect->h,BackCol.r,BackCol.g,BackCol.b );
	for(;i<WORDOBG->EditShowNum;i++)
	{
		if(temp==NULL)
			break;
		if(temp->text!=NULL)
		{
			gbsez(temp->text,showrect->x+WORDOBG->EditShowX+WORDSHOUPOS,showrect->y+i*WORDOBG->EditShowSpace,FontCol.r,FontCol.g,FontCol.b,0,(uint16)FONT);
			if(WORDOBG->Hightype)
				DrawTextKeyWord(temp->text,showrect->x+WORDOBG->EditShowX+WORDSHOUPOS,showrect->y+i*WORDOBG->EditShowSpace);
		}
		if(CopyStrStartNode!=NULL)
		{
			if(CopyStrStartNode==temp&&CopyStrStartNodePos>=0)
			{
				CopyStrStartNodePos=getstrn(temp->text,CopyStrStartNodePos);
				textfocusx=TextNW(temp->text,CopyStrStartNodePos);
				mrc_drawRect(showrect->x+WORDOBG->EditShowX+WORDSHOUPOS+textfocusx,showrect->y+i*WORDOBG->EditShowSpace,3,4,255,0,0);
			}
		}
		if(CopyStrEndNode!=NULL)
		{
			if(CopyStrEndNode==temp&&CopyStrEndNodePos>=0)
			{
				CopyStrEndNodePos=getstrn(temp->text,CopyStrEndNodePos);
				textfocusx=TextNW(temp->text,CopyStrEndNodePos);
				mrc_drawRect(showrect->x+WORDOBG->EditShowX+WORDSHOUPOS+textfocusx,showrect->y+(i+1)*WORDOBG->EditShowSpace-4,3,4,255,0,0);
			}
		}
		temp=temp->next;
	}
	if(WORDOBG->EditShowStatus)
		showrect->y-=WORDOBG->EditShowSpace;
	Scrollbar(WORDOBG->EditLineFocus-1,WORDOBG->EditLineAll-1,showrect->x+showrect->w-5,showrect->y+(WORDOBG->EditShowStatus?WORDOBG->EditShowSpace:0),4,WORDOBG->EditShowSpace*WORDOBG->EditShowNum);
	if(WORDOBG->mode&ED_FILE_RDWR&&WORDOBG->EditShowFocus)
	{
		ReDrawFocusNode();
	}
	if(data==0)
		mrc_refreshScreen(showrect->x,showrect->y,showrect->w,showrect->h);
}
void SetEditorListShow(EDITTEXT_P s,int x,int y,int w,int h)
{
	RectValue(s->EditShow,x,y,w,h);
	fontwh("鼎qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM",0,(uint16)FONT,(int32*)&w,(int32*)&WORDOBG->EditShowSpace);
	if(WORDOBG->EditShowSpace<=0)
		WORDOBG->EditShowSpace=16;
	s->EditShowNum=(s->EditShow.h)/WORDOBG->EditShowSpace;
	s->EditShowX=5;

	if(s->EditShowStatus)
		WORDOBG->EditShowNum-=1;//为工具箱预留一行的空间
	if(WORDOBG->EditShowNum<=0)
		WORDOBG->EditShowNum=1;


}
void SetEditorListShowEx(EDITTEXT_P s)
{
	SetEditorListShow(s,WORDING->EditShow.x,WORDING->EditShow.y,WORDING->EditShow.w,WORDING->EditShow.h);
}

EDITTEXT_P NowEditor(EDITTEXT_P pre,EDITTEXT_P node)//插入结点 文档
{
	EDITTEXT_P next=NULL;
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
EDITTEXT_P DelAEdit(EDITTEXT_P head,EDITTEXT_P node)//删除一个工作组中的文档
{
	EDITTEXT_P pre=NULL,next=NULL;
	if(head==NULL||node==NULL)
		return head;
	if(node==head)
	{
		next=node->next;
		if(node->EditListHead!=NULL)
			DelAllEditListNode(node->EditListHead);
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
	if(node->EditListHead!=NULL)
		DelAllEditListNode(node->EditListHead);
	mrc_free(node);
	return pre;
}

EDITTEXT_P DelWode(EDITTEXT_P node)//删除一个文档
{
	EDITTEXT_P temp;
	if(node==NULL)
		return NULL;
	//if(node->next!=NULL)
	//	return DelAEdit(node->next,node);
	//temp=node->pre;
	temp=DelAEdit(WORDING->docListHead,node);
	if(temp==NULL)
	{
		WORDING->docListHead=NULL;
		WORDING->docFocusNode=NULL;
		return NULL;
	}
	return temp;
}
void DelAllEdit(EDITTEXT_P head)//删除工作组全部文档
{
	if(head==NULL)
		return ;
	while(head!=NULL)
		head=DelWode(head);
}
EDITWOED_P DelAWode(EDITWOED_P head,EDITWOED_P node)//删除一个工作组
{
	EDITWOED_P pre=NULL,next=NULL;
	if(head==NULL||node==NULL)
		return head;
	if(node==head)
	{
		next=node->next;
		if(node->docFocusNode!=NULL)
			DelAllEdit(node->docFocusNode);
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
	if(node->docFocusNode!=NULL)
		DelAllEdit(node->docFocusNode);
	mrc_free(node);
	return head;
}

void DelAllWode(EDITWOED_P head)//删除全部工作组
{
	if(head==NULL)
		return ;
	while(head->next!=NULL)
		head=DelAWode(head,head->next);
	head=DelAWode(head,head);
}
EDITTEXT_P EditGetEnd(EDITTEXT_P head)//通过一个结点获取尾结点
{
	if(head==NULL)
		return NULL;
	while(head->next!=NULL)
		head=head->next;
	return head;
}



EDITTEXT_P NowWord()
{
	EDITTEXT_P p;
	p=malloc(sizeof(EDITTEXT));
	if(p==NULL)
		return NULL;
	p->EditShow.x=WORDING->EditShow.x;
	p->EditShow.y=WORDING->EditShow.y;
	p->EditShow.w=WORDING->EditShow.w;
	p->EditShow.h=WORDING->EditShow.h;
	if(WORDOBG)
		NowEditor(WORDING->docListHead,p);
	else
	{
		WORDOBG=p;
		WORDING->docListHead=WORDOBG;
	}
	return p;
}




EDITWOED_P NowWork ()
{
	EDITWOED_P p;
	p=malloc(sizeof(EDITWOED));
	return p;
}



int32 EditorInit()
{
	EDITWOED_P s;
	mr_screeninfo scrinfo;
	mrc_getScreenInfo(&scrinfo);

	WORDING=s=NowWork();
	working=workingheat=s;
	s->EditShow.x=s->EditShow.x=5;
	s->EditShow.w=scrinfo.width-10;
	s->EditShow.h=scrinfo.height-10;
	FONT=1;
	ColInit();//系统UI
	fontinit();//字库
	HighLightInit();//高光
	//SetEditorListShow(WORDOBG,5,0,SCRW,SCRH);//界面大小

	EditFocusTimerHand=mrc_timerCreate();
	EditKeyTimerHand=mrc_timerCreate();
	//return MR_SUCCESS;


	return MR_SUCCESS;
}



//编辑器暂停,暂停所有定时器
int32  EditPause()
{
	mrc_timerStop(EditFocusTimerHand);
	mrc_timerStop(EditKeyTimerHand);
	return 0;
}
int32 EditResume()
{
	if(WORDOBG)
	{
		if(WORDOBG->EditShowFocus)
			mrc_timerStart(EditFocusTimerHand,500,1,DrawFocusNode,1);
		DrawEditor(1);
	}
	return 0;
}
int32 EditFreeAll()
{
	if(WORDING!=NULL)
		DelAllWode(WORDING);
	WORDING=NULL;
	if(EditKeyTimerHand)
		mrc_timerDelete(EditKeyTimerHand);
	EditKeyTimerHand=0;

	if(EditFocusTimerHand)
		mrc_timerDelete(EditFocusTimerHand);
	EditFocusTimerHand=0;
	if(CopyStr!=NULL)
		mrc_free(CopyStr);
	CopyStr=NULL;
	return 0;
}
int32 EditSaveToFile(char *name,int fg)
{
	int32 re;
	re=SaveListToFile(WORDOBG->EditListHead,name);
	if(!re&&fg)
		strcpy(WORDOBG->WorkFile,name);
	return re;
}
int32 EditNullNum(EditListLink_p node,int pos)//分析得出下一行的空格数
{
	int m1=0,//()数(与)相互抵消
		m2=0,//{}数
		//s=0,//是否有分号
		pos1=0,//字符出现之前的空格数
		pos2=0,//pos之后字符出现之前的空格数
		i=0,
		len=0
		;
	if(node==NULL)
		return 0;
	len=mrc_strlen(node->text);
	if(pos>len)
		pos=len;
	if(pos<0)
		pos=0;
	for(i=0;i<pos;i++)
	{
		if(node->text[i]==' ')
			;
		else
			break;
	}
	pos1=i;
	/*
	for(i=pos1;i<pos;i++)
	{
		switch(node->text[i])
		{
		case '(':
			m1++;
			break;
		case ')':
			m1--;
			break;
		case '{':
			m2++;
			break;
		case '}':
			m2--;
			break;
		case ';':
			s=1;
			break;
		}
	}*/
	for(i=pos;i<len;i++)
	{
		if(node->text[i]==' ')
			;
		else
			break;
	}
	if(node->text[pos-1]=='{')
		m2++;
	if(node->text[pos-1]=='}')
		m2--;
	if(node->text[pos]=='}')
		m2--;
	pos2=i-pos;
	return pos1-pos2+m1+m2;
}

int32 EditLoadFile(char *filename,int mode)
{
	if(filename==NULL)
	WORDOBG=NowWord();
	else
	if(EditLoadMccFile(filename)==-1)
	{
		return -1;
	}
	WORDOBG->mode=mode;
	WORDOBG->EditShowFocus=1;
	switch(mode)
	{
	case ED_FILE_RDONLY:
		WORDOBG->EditShowFocus=1;
		WORDOBG->Hightype=1;
		WORDOBG->EditShowStatus=1;
		break;
	case ED_FILE_RDWR:
		WORDOBG->EditShowFocus=1;
		WORDOBG->Hightype=1;
		WORDOBG->EditShowStatus=1;
		WORDOBG->EditShowCursor=1;
		break;
	}
	if(WORDOBG->EditListHead==NULL)
	{
		WORDOBG->EditListHead=EditMakeNode("");
		WORDOBG->EditFristNode=WORDOBG->EditListHead;
		WORDOBG->EditFocusNode=WORDOBG->EditListHead;
		WORDOBG->EditFocusNodePos=0;
	}
	SetEditorListShowEx(WORDOBG);
	if(filename!=NULL)
		strcpy(WORDOBG->WorkFile,filename);
	WORDOBG->EditFocusNodePos=0;
	WORDOBG->EditFristNode=WORDOBG->EditListHead;
	WORDOBG->EditFocusNode=WORDOBG->EditListHead;
	WORDOBG->EditLineAll=EditGetLinkLen(WORDOBG->EditListHead);
	return 0;
}
int32 EditSetCopyStart()//设置起点
{
	CopyStrStartNode=WORDOBG->EditFocusNode;
	CopyStrStartNodePos=WORDOBG->EditFocusNodePos;
	return 0;
}
int32 EditSetCopyEnd()//设置终点
{
	CopyStrEndNode=WORDOBG->EditFocusNode;
	CopyStrEndNodePos=WORDOBG->EditFocusNodePos;
	return 0;
}
int32 CleanSelect()
{
	CopyStrStartNode=NULL;
	CopyStrStartNodePos=0;
	CopyStrEndNode=NULL;
	CopyStrEndNodePos=0;
	return 0;
}
int32 CopyStrText()
{
	int s=0,e=0,len=0;
	EditListLink_p temp=NULL;
	if(CopyStr!=NULL)
		mrc_free(CopyStr);
	CopyStr=NULL;
	if(CopyStrStartNode==NULL||CopyStrEndNode==NULL)
		return -1;
	temp=WORDOBG->EditListHead;
	while(temp)//检测起始结点是否有效
	{
		if(temp==CopyStrStartNode)
			s=1;
		if(temp==CopyStrEndNode&&s==1)//保证end在start之前
			e=1;
		if(s==1&&e==1)
			break;
		temp=temp->next;
	}
	if(s==1&&e==1)//结点有效
	{
		if(CopyStrStartNode==CopyStrEndNode)
		{
			CopyStrStartNodePos=getstrn(CopyStrStartNode->text,CopyStrStartNodePos);
			CopyStrEndNodePos=getstrn(CopyStrEndNode->text,CopyStrEndNodePos);
			if(CopyStrStartNodePos>=0&&CopyStrStartNodePos<CopyStrEndNodePos)//有效
			{
				len=CopyStrEndNodePos-CopyStrStartNodePos;
				CopyStr=mrc_malloc(len+2);
				CopyStr[len]=0;
				CopyStr[len+1]=0;
				mrc_strncpy(CopyStr,CopyStrStartNode->text+CopyStrStartNodePos,len);
				CleanSelect();
				return 0;
			}
			else//无效
			{
				return -1;
			}
		}
		else
		{
			CopyStrStartNodePos=getstrn(CopyStrStartNode->text,CopyStrStartNodePos);
			CopyStrEndNodePos=getstrn(CopyStrEndNode->text,CopyStrEndNodePos);
			len=mrc_strlen(CopyStrStartNode->text)-CopyStrStartNodePos;
			temp=CopyStrStartNode->next;
			len+=1;//换行
			while(temp!=CopyStrEndNode)
			{
				len+=mrc_strlen(temp->text);
				len+=1;
				temp=temp->next;
			}
			len+=CopyStrEndNodePos;
			CopyStr=mrc_malloc(len+2);
			CopyStr[len]=0;
			CopyStr[len+1]=0;
			mrc_strcpy(CopyStr,CopyStrStartNode->text+CopyStrStartNodePos);
			temp=CopyStrStartNode->next;
			mrc_strcat(CopyStr,"\n");
			while(temp!=CopyStrEndNode)
			{
				mrc_strcat(CopyStr,temp->text);
				mrc_strcat(CopyStr,"\n");
				temp=temp->next;
			}
			mrc_strncat(CopyStr,CopyStrEndNode->text,CopyStrEndNodePos);
			CleanSelect();
			return 0;
		}
	}
	else//无效
	{
		return 1;
	}
	CleanSelect();
}
int32 PasteStrText()
{
	if(CopyStr!=NULL)
		EditInsertText(CopyStr);
	return 0;
}

int EditMouseEvent(int32 code,int32 p1,int32 p2)
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
		tpos=WORDSHOUPOS;
		frist=WORDOBG->EditFristNode;
		focus=WORDOBG->EditFocusNode;
		n=(p2-showrect->y)/WORDOBG->EditShowSpace-1;
		if(tp2>WORDOBG->EditShowSpace&&tp2<(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace)//表示按下时在编辑区
		{
			WORDOBG->EditFocusNode=EditGetNextNNode(WORDOBG->EditFristNode,n>=WORDOBG->EditShowNum? WORDOBG->EditShowNum-1:n);
			WORDOBG->EditFocusNodePos=TextWN(WORDOBG->EditFocusNode->text,p1-WORDSHOUPOS);
		}
	}

	if(code==MR_MOUSE_MOVE)
	{
		if(p1-tp1>=10||p2-tp2>=10||p1-tp1<=-10||p2-tp2<=-10)
			move=1;//大范围移动了
		if(move==1)
		{
			if(tp2>WORDOBG->EditShowSpace+WORDOBG->EditShow.y&&tp2<(WORDOBG->EditShowNum+1)*WORDOBG->EditShowSpace+WORDOBG->EditShow.y)//表示按下时在编辑区
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
				WORDOBG->EditFocusNodePos=TextWN(WORDOBG->EditFocusNode->text,p1-WORDSHOUPOS);
			}
		}
	}
	if(code==MR_MOUSE_UP)
	{
		
	}
	DrawEditor(0);
	return 0;
}



int32 EditEvent(int32 code, int32 p1, int32 p2)
{
	if(!WORDING)
		return -1;
	if(!WORDOBG)
		return -1;
	//VirtualKeyEvent(code,p1,p2);
	if(code==MR_KEY_PRESS)
	{
			switch(p1)
			{
			case  MR_KEY_SOFTRIGHT:
				DelText();
				break;
			case  MR_KEY_UP:
				EditUp();
				break;
			case  MR_KEY_DOWN:
				EditDown();
				break;
			case  MR_KEY_LEFT:
				EditLeft();
				break;
			case  MR_KEY_RIGHT:
				EditRight();
				break;
			case  MR_KEY_1:
				GotoLine(GetFocusLine()-WORDOBG->EditShowNum);
				break;
			case  MR_KEY_4:
				GotoLine(GetFocusLine()+WORDOBG->EditShowNum);
				break;
			case  MR_KEY_5:
				EditInsertText("\n");
				break;
			}
			KeyLongPress(p1,p2);
			DrawEditor(0);
	}
	else
		if(code==MR_KEY_RELEASE)
			KeyFree();
		else
			EditMouseEvent(code,p1,p2);



	return MR_SUCCESS;
}



void EditKeyLongPress(int32 data)
{
	if(KeyTimerLoop>5)
		EditEvent(MR_KEY_PRESS,data,0x7fffffff);
	KeyTimerLoop++;
	if(KeyTimerLoop>100)
		KeyTimerLoop=60;
}
void KeyLongPress(int p1,int p2)
{
	if(p2!=0x7fffffff)
		KeyTimerLoop=0;
	mrc_timerStart(EditKeyTimerHand,50,p1,(mrc_timerCB)EditKeyLongPress,1);
}
void KeyFree()
{
	mrc_timerStop(EditKeyTimerHand);
}


int32 EditInsertText(char *text)
{
	int n=0,i=0;
	if(text==NULL)
		return -1;
	if(text[0]==0)
		return 0;
	else
	{
		if(text[1]!=0)
			InsertText(WORDOBG->EditFocusNode,WORDOBG->EditFocusNodePos,text);
		else
		{
			if(text[0]=='\n')
			{
				n=EditNullNum(WORDOBG->EditFocusNode,WORDOBG->EditFocusNodePos);
				InsertText(WORDOBG->EditFocusNode,WORDOBG->EditFocusNodePos,"\n");
				for(i=0;i<n;i++)
				{
					InsertText(WORDOBG->EditFocusNode,WORDOBG->EditFocusNodePos," ");
				}
			}
			else
			{
				InsertText(WORDOBG->EditFocusNode,WORDOBG->EditFocusNodePos,text);
			}
		}
	}
	return 0;
}
