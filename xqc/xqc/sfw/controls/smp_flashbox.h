#ifndef _SMP_FLASHBOX_H
#define _SMP_FLASHBOX_H

#include "window.h"

/**

 Flash��

  ��Ҫʹ��Flash�У�
   - �������򴰿�
   - ��������ƿ��е���Ϣ
   - ��ӵ�������


  \code
  //֡��Ϣ
  static const DWORD fbFrames[] = {
  	BMP_FRAME1,
  	BMP_FRAME2,
  	BMP_FRAME3
  };

  //��Ĵ���
  hControl = SGL_CreateWindow(SMP_FlashBox_WndProc, ...);
  SMP_FlashBox_SetInfo(hControl, fbFrames, sizeof(fbFrames)/sizeof(fbFrames[0]), 1000, TRUE);
  SGL_AddChildWindow(hParent, hControl);
  
  \endcode


���ڳ�Ա����

 * \�������ÿ��е���Ϣ��
 *
 * \������hWnd���ھ���������
 * \����������Դ����������֡����
 * \��������Ĵ�С�ߴ��֡
 * \���������֮֡��ļ��ֵ��MS
 * \����ѭ����ѭ������������ƣ���ͣ�����һ֡palying
 * /
*/
VOID SMP_FlashBox_SetInfo(HWND hWnd, const DWORD* frms, Uint32 size, Uint32 interval, BOOL loop);

/**
 * \brief Play the flash from the index frame.���Ŵ�������������
 *
 * \param hWnd the flash box window handle������hWnd���ھ���������
 * \param index the index of the frame to play����ָ���֡��������
 * \sa SMP_FlashBox_StopɽSMP_FlashBox_Stop
 */
VOID SMP_FlashBox_Start(HWND hWnd, Uint32 index);

/**
 * \brief Continue playing.��������
 *
 * \param hWnd the flash box window handle������hWnd���ھ���������
 * \sa SMP_FlashBox_Stop
 */
VOID SMP_FlashBox_Play(HWND hWnd);

/**
 * \brief Stop playing.ֹͣ����
 *
 * The stop flash box will auto playing when shown again.��������ֹͣ����ʱ���Զ��ٴ���ʾ
 *
 * \param hWnd the flash box window handle \������hWnd���ھ���������
 * \sa SMP_FlashBox_Start, SMP_FlashBox_Play\ɽSMP_FlashBox_Start��SMP_FlashBox_Play
 */
VOID SMP_FlashBox_Stop(HWND hWnd);

/**
���ڳ���
 * \brief The flash box window procedure.\�����Ĵ��ڹ���
 *
 * \param hWnd the window handle\������hWnd���ھ��
 * \param Msg the window message\������Ϣ�Ĵ�����Ϣ
 * \param wParam the first parameter\����wParam�ĵ�һ������
 * \param lParam the second parameter \����lParam�ĵڶ�������
 * \return the result of message process \������Ϣ�Ĵ�����
 */
LRESULT SMP_FlashBox_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	/** @} */
	
/** @} end of smp_flashbox*/

#endif
