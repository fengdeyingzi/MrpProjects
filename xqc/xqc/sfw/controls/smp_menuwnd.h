#ifndef _SMP_MENUWND_H
#define _SMP_MENUWND_H

#include "window.h"

/**
  �˵�Ϊ�������Ӵ�

  �ⲻ��һ�����ڻ�ؼ��ڸ������С�
  �������һ�����ڹ�������ӹ��ܣ�����������£������ڣ�
  ��һ�����㴰�ڲ˵��ϵ���ʾ����������Ӧ�ı�
  ������/�������ȷ��/ȡ������ֻ�����Ĭ�ϵĴ��ڹ�����Ϊ�������
  ���Ķ��㴰�ڣ��е����˵���
  
  \code
	LRESULT  YOUR_WINDOW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch(Msg)
		{
			//���������Ϣ
		}
		//�������������ΪĬ��
		return SMP_MenuWnd_WndProc(hWnd, Msg, wParam, lParam);
	}  
  \endcode

���ڳ���

 * \�˵��Ĵ��ڹ��̡�
 *
 * \����hWnd	���ھ��
 * \����Msg		������Ϣ
 * \����wParam	��һ������
 * \����lParam	�ڶ�������
 * \���ظý��̵Ľ��
 */
LRESULT SMP_MenuWnd_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); 

	/** @} */

/** @} end of smp_menuwnd */


#endif

