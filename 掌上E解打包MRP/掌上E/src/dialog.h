#ifndef _DIALOG_H_
#define _DIALOG_H_



/*
回调
*/
typedef void (*Ptr_DialogEvent)(int32 data, char *filename);




void DialogWinEvent(int32 data, int32 eventId);
void DialogKeyEvent(int32 data, int32 type, int32 p1, int32 p2);
/*
初始化通用对话框
*/
void Dialog_Init(char *initDir, int8 fileOrDir, char *dialogTitle, Ptr_DialogEvent DialogEvent);










#endif