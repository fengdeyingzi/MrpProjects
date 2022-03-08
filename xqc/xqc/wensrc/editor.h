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
	int Modify;//����ļ��Ƿ��޸�
	int mode;//�򿪷�ʽ
	int Hightype, //�߹��Ƿ���ʾ
		EditShowStatus,//��ʾ״̬��
		EditShowFocus,//��ʾ������
		EditShowCursor,//��ʾ���
		EditShowX,//��ʾxƫ��
		EditShowSpace,//�о�
		EditShowPos,//����ƫ��
		EditFocusNodePos,//���ƫ��
		EditLineAll,//������
		EditLineFocus,//������
		EditShowNum;//��ʾ����

	EDITRECT EditShow;//��ʾ����
	EditListLink_p EditListHead,//�ܱ༭����
		EditFristNode,//��ʾͷ���
		EditFocusNode;//������
}EDITTEXT,*EDITTEXT_P;


typedef struct EDITWOEDNODE {
	struct EDITWOEDNODE *pre;
	struct EDITWOEDNODE *next;
	char mainfile[128];//����ʱ���ļ�
	EDITRECT EditShow;//��ʾ����
	int loadtype;//�򿪷�ʽ
	EDITTEXT_P docListHead;//�ܽ��
	EDITTEXT_P docFocusNode;//��ǰ�ڵ�

}EDITWOED,*EDITWOED_P;


extern EDITWOED_P working,workingheat;


#define  WORDING  (working)
#define  WORDOBG  (WORDING->docFocusNode)
#define  WORDSHOUPOS  (WORDOBG->EditShowPos)




 void output(char *text);
 //ɾ��ȫ����
 void DelAllEditListNode(EditListLink_p head);

//ͨ��һ������ȡβ���
 EditListLink_p EditGetEndNode(EditListLink_p head);
 //ͨ��һ���ı�����һ������
 EditListLink_p MakeEditList(char *scr);
 int SaveListToFile(EditListLink_p head,char *filename);
 EditListLink_p EditMakeNode(char *text);//������
 EditListLink_p InsertEditListNode(EditListLink_p pre,EditListLink_p node);//������
 EditListLink_p DelAEditListNode(EditListLink_p head,EditListLink_p node);//ɾ��һ��
 EditListLink_p EditGetPreNNode(EditListLink_p node,int n);
 EditListLink_p EditGetNextNNode(EditListLink_p node,int n);





//��ȡһ�еĳ���
int GetLineLen(char *scr);
//��ȡ������ָ��
void GetObgText(char ***dt);
//�����ļ�,���Զ��ͷ�֮ǰ��head
int EditLoadMccFile(char *filename);
//����һ���ַ�Ҫw����Ҫ�೤
int TextWN(char *text,int w);
//����һ���ַ�ǰlen����ʾ���
int TextNW(char *text,int len);
//��ȡ��ǰ���� �ܽ��
int GetEditGetTwoNodeLen(void);
//��ת��
int GotoLine(int line);
//��ȡ��ǰ����
int GetFocusLine(void);

//ѡ�� �������
int32 EditSetCopyStart(void);
//ѡ�� �����յ�
int32 EditSetCopyEnd(void);
//ȡ��ѡ��
int32 CleanSelect(void);
//����ѡ������
int32 CopyStrText(void);
//ճ��ѡ������
int32 PasteStrText(void);
void UDyidongyueshu(void);
void LRyidongyueshu(void);//������Լ��
void CleanLib(void);
int32 ColInit(void);
int32 EditInsertText(char *text);
int NumFind(char *text);
int32 EditSetCopyStart(void);//�������
int32 CleanSelect(void);
int32 EditSetCopyEnd(void);//�����յ�
int32 CleanSelect(void);
int32 CopyStrText(void);
int32 PasteStrText(void);

//���༭��
void DrawEditor(int data);
//�༭�����ڴ�С
void SetEditorListShow(EDITTEXT_P s,int x,int y,int w,int h);

//չ������
void UnfoldSign(void);

//�½�һ��������
EDITWOED_P NowWork (void);
//�ڹ���������һ���ĵ�
//һ���������ɰ�������ĵ�
EDITTEXT_P NowWord(void);
//ɾ��һ���������е��ĵ�
EDITTEXT_P DelAEdit(EDITTEXT_P head,EDITTEXT_P node);
EDITTEXT_P DelWode(EDITTEXT_P node);//ɾ��һ���������ĵ�
//ɾ��������ȫ���ĵ�
void DelAllEdit(EDITTEXT_P head);
//ɾ��һ��������
EDITWOED_P DelAWode(EDITWOED_P head,EDITWOED_P node);
//ɾ��ȫ��������
void DelAllWode(EDITWOED_P head);



//��ʼ������
int32 EditorInit(void);
//�ͷ�����
int32 EditFreeAll(void);
//�༭����ͣ����ͣ���ж�ʱ��
int32  EditPause(void);
//�༭���ָ����ָ����ж�ʱ��
int32 EditResume(void);//
//���ļ������Զ��ͷ�֮ǰ��head
int32 EditLoadFile(char *filename,int mode);
//���棬�������ɹ�����fg==1 ��ǰ�ļ������ı�
int32 EditSaveToFile(char *name,int fg);//

//�������
void EditUp(void);
//�������
void EditDown(void);
//�������
void EditLeft(void);
//�������
void EditRight(void);

int32 EditEvent(int32 code, int32 p1, int32 p2);
void EditKeyLongPress(int32 data);
void KeyLongPress(int p1,int p2);
void KeyFree();

//����һ���ı�
int32 EditInsertText(char *text);



#endif