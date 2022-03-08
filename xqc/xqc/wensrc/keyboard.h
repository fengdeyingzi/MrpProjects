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
void DrawFunction(void);//画工具箱
void DrawCiTiao(void);//画词条
void DrawSign(void);//标点符号
void DrawDaoHang(void);//画方向键
void DrawKeyboardKey9(void);//画九键虚拟键盘
void DrawKeyboard(void);
void UnfoldKeyboard(void);//展开/关闭 虚拟键盘
void UnfoldSign(void);//展开符号
void CleanLib(void);
int NumFind(char *text);//查找数字符号
void WordSelect(EditListLink_p node);
void KeyInput(int32 data);
void InputStart(void);
int KeyboardEvent(int32 code,int32 p1,int32 p2);//物理键盘事件
int MouseEvent(int32 code,int32 p1,int32 p2);
int SignEvent(int32 code,int32 p1,int32 p2);
int DaoHangEvent(int32 code,int32 p1,int32 p2);
int32 EditEvent(int32 code, int32 p1, int32 p2);

int32 EditReLoad(void);
void key_close(void);
int32  key_EditPause(void);//编辑器暂停，暂停所有定时器




#endif
