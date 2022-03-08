#ifndef _KEYBOARD_H
#define _KEYBOARD_H
extern int 	EditKeyboardH,EditKeyboardType;


void CleanLib(void);
int StateText(char *word,char *text);
int GetNum(char w);
EditListLink_p EditFindFormLib(char *text);
EditListLink_p FindNodeByText(EditListLink_p head,char *text);
void AddLib(char *text);
int WordFindFromLib(void);
int EditLoadWordLib(char *filename);
void DrawFunction(void);//��������
void DrawCiTiao(void);//������
void DrawSign(void);//������
void DrawDaoHang(void);//�������
void DrawKeyboardKey9(void);//���ż��������
void DrawKeyboard(void);
void UnfoldKeyboard(void);//չ��/�ر� �������
void UnfoldSign(void);//չ������
void CleanLib(void);
int NumFind(char *text);//�������ַ���
void WordSelect(EditListLink_p node);
void KeyInput(int32 data);
void InputStart(void);
int KeyboardEvent(int32 code,int32 p1,int32 p2);//��������¼�
int MouseEvent(int32 code,int32 p1,int32 p2);
int SignEvent(int32 code,int32 p1,int32 p2);
int DaoHangEvent(int32 code,int32 p1,int32 p2);
int32 EditEvent(int32 code, int32 p1, int32 p2);

int32 EditReLoad(void);
void key_close(void);
int32  key_EditPause(void);//�༭����ͣ����ͣ���ж�ʱ��




#endif
