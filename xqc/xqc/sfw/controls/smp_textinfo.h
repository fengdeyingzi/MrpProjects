#ifndef _SMP_TEXTINFO_H
#define _SMP_TEXTINFO_H

#include "window.h"

/**
  \�ı���


  ��Ҫʹ�õ�TextInfo���ڣ�
   - ����TextInfo����
   - ���õ�TextInfo����

  \code
  	hTextInfo = SGL_CreateWindow(SMP_TextInfo_WndProc, ...);
  	SMP_TextInfo_SetContent(hTextInfo, ...);
  	SGL_AddChildWindow(hWnd, hTextInfo);
  \endcode
 
*/

/**
 * \���֧����
 */
#define SMP_TEXTINFO_LINECOUNT	50

	/**
	* \name Window Styles
	* @{
	*/

/**
 * \˵������û�й�����
 */
#define SMP_TEXTINFOS_NOSCRBAR		0x0001L

//���Ʊ߿�
#define SMP_TEXTINFOS_HASBOARD		0x0002L


	/** @} */

	/**
	 * \name Window Member Functions
	 * @{
	 */

/** 
 * \�����ı���Ϣ����
 *
 * \����hWnd		���ھ��
 * \����pContent	����
 */
VOID SMP_TextInfo_SetContent(HWND hWnd, PCWSTR pContent);


/**
 * \���²�������
 *
 * \������hWnd	TextInfo���ھ��
 */
VOID SMP_TextInfo_Refresh(HWND hWnd);

	/** @} */

	/**
	 * \name Window Procedure
	 * @{
	 */

/** 
 * \TextInfo���ڹ���
 *
 * \����hWnd		���ھ��
 * \����Msg			������Ϣ
 * \����wParam		��һ������
 * \����lParam		�ڶ�������
 * \������Ϣ�Ĵ�����
 */
LRESULT SMP_TextInfo_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	/** @} */

/** @} end of smp_textinfo */

#endif /* _SMP_TEXTINFO_H */
