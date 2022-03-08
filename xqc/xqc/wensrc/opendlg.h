#ifndef _OPENDLG_H
#define _OPENDLG_H

#include "window.h"

typedef enum {
	DIALOG_SELECTFILE, //ѡ���ļ�
	DIALOG_SELECTDIR   //ѡ���ļ���
}SMP_DIALOG_TYPE;

//���¹��ⲿʹ��,�����öԻ�������ݻᱻ�ı�
extern char SMP_DIALOG_CUR_PATH[128];  //��ǰ����·��(���Ҫ���ó�ʼĿ¼,��Ŀ¼��Ӧ����"/"����"moc/"
extern char SMP_DIALOG_PATH[256];      //�Ի������ٺ󷵻�·��(�ļ����ļ���)
extern char SMP_DIALOG_TEMP_PATH[256]; //��ʱʹ��
extern char SMP_DIALOG_TEMP[128];      //·������ʱʹ��
extern uint8 SMP_DIALOG_ID;

#define DIALOG_GET   WM_USER+1


VOID ShowOpenDlg(uint8 Id,WID wndid, SMP_DIALOG_TYPE type);

//ע�ᴰ�ں���
LRESULT OpenDlg_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#endif
