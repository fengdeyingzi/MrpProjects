#ifndef _SMP_INPUTBOX_H
#define _SMP_INPUTBOX_H

#include "window.h"

/*
  �����
  ��Ҫʹ�������
  - ��ʾInputBox
  - ��Ӧ��Ϣ

  \code
  	//ֻ��Ҫ���������������ʾInputBox
  	SMP_InputBox(...);

  	//��Ӧ��Ϣ��֪ͨ
	case WM_COMMAND:
		WID id = LOWORD(wParam);
		WORD code = HIWORD(wParam);

		if(id == "the InputBox id")
		{
			PWSTR str = (PWSTR)lParam;

			if(code == INPUT_OK)
			{
				//�������strΪ�õ����ַ���(unicode����)
                
			}else{
				//����ȡ��
			}
		}	
  \endcode
*/

/**
 * \INPUT�������
 */
#define INPUT_OK		0x0001L

/**
 * \INPUT����ȡ��
 */
#define INPUT_CANCEL	0x0002L

typedef struct SMP_InputBoxData
{
	PCWSTR title;	    /*��ʾ�����뷨���ڵı���*/	
	UCHAR buffer[512];	/*�༭������������*/	
	Uint32 size;	    /*��������С*/
    HWND hEdit;

}SMP_InputBoxData, *SMP_PInputBoxData;
#define SMP_GetBuffer SMP_InputBoxData.buffer
 /*
 * \��ʾһ�������
 * \����id					����ID
 * \����hParent				�����ھ��
 * \����title				�����ַ���
 * \����content				�����ַ���
 * \����maxsize             ��󳤶�
 * \����style
 * - ES_NUM��               ֧�ֺ�������
 * - ES_PWD��               ֧����������
 * - ES_ALPHA��             ֧����������,alpha�����֣��ַ���
 * \����listener			����֪ͨ��Ϣ������Ϊ��ʱ�����ڽ��õ���Ϣ
 * \����InputBox�ľ��
 **/
HWND SMP_InputBox(WID id, HWND hParent, PCWSTR title, PCWSTR content, Uint32 maxsize, DWORD style, HWND listener);

/*
���ڳ���

 * \����hDlg		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_InputBox_WndProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);








#endif

