#ifndef _EDITOR_H
#define _EDITOR_H

enum{
	ED_FILE_RDONLY=1,
	ED_FILE_RDWR=2
};



typedef struct {
	int16            x;
	int16            y;
	uint16            w;
	uint16            h;
}EDITRECT;
#define  RectValue(rect,xx,yy,ww,hh) do{rect.x=xx;rect.y=yy;rect.w=ww;rect.h=hh;}while(0)
typedef struct editlistnode {
	struct editlistnode *pre;
	struct editlistnode *next;
	char *text;
}EditListLink,*EditListLink_p;

typedef struct EDITTEXTNODE {

	struct EDITTEXTNODE *pre;
	struct EDITTEXTNODE *next;
	char WorkFile[128];
	int Modify;//检测文件是否被修改
	int mode;//打开方式
	int Hightype, //高光是否显示
		EditShowStatus,//显示状态栏
		EditShowFocus,//显示焦点行
		EditShowCursor,//显示光标
		EditShowX,//显示x偏移
		EditShowSpace,//行距
		EditShowPos,//横向偏移
		EditFocusNodePos,//光标偏移
		EditLineAll,//总行数
		EditLineFocus,//焦点行
		EditShowNum;//显示行数

	EDITRECT EditShow;//显示区域
	EditListLink_p EditListHead,//总编辑链表
		EditFristNode,//显示头结点
		EditFocusNode;//焦点结点
}EDITTEXT,*EDITTEXT_P;


typedef struct EDITWOEDNODE {
	struct EDITWOEDNODE *pre;
	struct EDITWOEDNODE *next;
	char mainfile[128];//启动时主文件
	EDITRECT EditShow;//显示区域
	int loadtype;//打开方式
	EDITTEXT_P docListHead;//总结点
	EDITTEXT_P docFocusNode;//当前节点

}EDITWOED,*EDITWOED_P;


extern EDITWOED_P working,workingheat;


#define  WORDING  (working)
#define  WORDOBG  (WORDING->docFocusNode)
#define  WORDSHOUPOS  (WORDOBG->EditShowPos)




 void output(char *text);
 //删除全部行
 void DelAllEditListNode(EditListLink_p head);

//通过一个结点获取尾结点
 EditListLink_p EditGetEndNode(EditListLink_p head);
 //通过一段文本制造一条链表
 EditListLink_p MakeEditList(char *scr);
 int SaveListToFile(EditListLink_p head,char *filename);
 EditListLink_p EditMakeNode(char *text);//制造结点
 EditListLink_p InsertEditListNode(EditListLink_p pre,EditListLink_p node);//插入结点
 EditListLink_p DelAEditListNode(EditListLink_p head,EditListLink_p node);//删除一个
 EditListLink_p EditGetPreNNode(EditListLink_p node,int n);
 EditListLink_p EditGetNextNNode(EditListLink_p node,int n);





//获取一行的长度
int GetLineLen(char *scr);
//获取焦点行指针
void GetObgText(char ***dt);
//加载文件,会自动释放之前的head
int EditLoadMccFile(char *filename);
//测试一段字符要w宽需要多长
int TextWN(char *text,int w);
//测试一段字符前len的显示宽度
int TextNW(char *text,int len);
//获取当前行数 总结点
int GetEditGetTwoNodeLen(void);
//跳转行
int GotoLine(int line);
//获取当前行数
int GetFocusLine(void);

//选择 设置起点
int32 EditSetCopyStart(void);
//选择 设置终点
int32 EditSetCopyEnd(void);
//取消选择
int32 CleanSelect(void);
//复制选择内容
int32 CopyStrText(void);
//粘贴选择内容
int32 PasteStrText(void);
void UDyidongyueshu(void);
void LRyidongyueshu(void);//左右移约束
void CleanLib(void);
int32 ColInit(void);
int32 EditInsertText(char *text);
int NumFind(char *text);
int32 EditSetCopyStart(void);//设置起点
int32 CleanSelect(void);
int32 EditSetCopyEnd(void);//设置终点
int32 CleanSelect(void);
int32 CopyStrText(void);
int32 PasteStrText(void);

//画编辑器
void DrawEditor(int data);
//编辑器窗口大小
void SetEditorListShow(EDITTEXT_P s,int x,int y,int w,int h);

//展开符号
void UnfoldSign(void);

//新建一个工作区
EDITWOED_P NowWork (void);
//在工作区建立一个文档
//一个工作区可包含多个文档
EDITTEXT_P NowWord(void);
//删除一个工作组中的文档
EDITTEXT_P DelAEdit(EDITTEXT_P head,EDITTEXT_P node);
EDITTEXT_P DelWode(EDITTEXT_P node);//删除一个工作组文档
//删除工作组全部文档
void DelAllEdit(EDITTEXT_P head);
//删除一个工作组
EDITWOED_P DelAWode(EDITWOED_P head,EDITWOED_P node);
//删除全部工作组
void DelAllWode(EDITWOED_P head);



//初始化所有
int32 EditorInit(void);
//释放所有
int32 EditFreeAll(void);
//编辑器暂停，暂停所有定时器
int32  EditPause(void);
//编辑器恢复，恢复所有定时器
int32 EditResume(void);//
//打开文件，会自动释放之前的head
int32 EditLoadFile(char *filename,int mode);
//保存，如果保存成功并且fg==1 当前文件名被改变
int32 EditSaveToFile(char *name,int fg);//

//光标上移
void EditUp(void);
//光标下移
void EditDown(void);
//光标左移
void EditLeft(void);
//光标右移
void EditRight(void);

int32 EditEvent(int32 code, int32 p1, int32 p2);
void EditKeyLongPress(int32 data);
void KeyLongPress(int p1,int p2);
void KeyFree();

//插入一段文本
int32 EditInsertText(char *text);



#endif