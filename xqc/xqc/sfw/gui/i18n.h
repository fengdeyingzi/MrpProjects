/*
** i18n.h: the i18n header file.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#ifndef _SGL_I18N_H
#define _SGL_I18N_H

#include "types.h"


#ifdef __cplusplus
extern "C"{
#endif  /* __cplusplus */


#define SGL_MAX_LNG_COUNT		50
#define MR_PLAT_VALUE_BASE	1000

//include the string ids
#include "res_str.h"


/**
 * \��Դ����

  \���ʻ����ַ�������

  ������ϵͳ��������componets��
  - Excel��managemented�ַ�����Դ����ͼƬ����
	��д���е�Ӧ�ó�����Excel�ַ�����Դ����������ÿ����һ�����
	�������п��Ը�����Ҫɾ������ӡ��༭Excel��
	��һ�°�ť����CodeGen�������ɴ���
  - ��Ӧ�ó���ʹ��ĳЩAPI��
	  \code
		#include "i18n.h"
		...
		//���ַ���ID��STR_OK��

		PWCSTR str = SGL_LoadString(STR_OK);
	  \endcode

  <br><br>
  \image html strings.gif "Excel Manangement String Resource"

  @ingroup resource
  @{

	 

 * ��ʼ�����ʻ���ϵͳ
 *
 * �˹���֮ǰ��������κ��������ʻ����ܡ��ÿ�ܽ�����ʱ��ϵͳΪ������ˡ�
 * ���Բ�Ҫ�ں�����
 */
VOID GUIAPI SGL_I18nInitialize(VOID);

/**
 * \���õ�ǰ�����ԣ��Ⲣ����Ӱ���ַ�������SGL_LoadString����
 * \�����µ�����ʶ����
 * \return 
 * 	- �ɹ�����TRUE
 * 	- ���򷵻�FALSE
 */
BOOL GUIAPI SGL_SetCurrentLanguage(DWORD lang);

/**
 * \��ȡ��ǰʹ�õ�����
 *
 * \���ص�ǰ����
 */
DWORD GUIAPI SGL_GetCurrentLanguage(VOID);

/**
 * \��ȡ��ǰ�����������ַ���ID������ַ��� 
 * \����str		�ַ���ID
 * \return 
 * 	- Unicode�ַ�����ָ��
 * 	- NULLʱ��Ч
 */
PCWSTR GUIAPI SGL_LoadString(DWORD str);

/**
 * \�ͷ������ڴ���Դ
 * \����lang	����
 * \�ɹ�����true�����򷵻�FALSE
 */
BOOL GUIAPI SGL_ReleaseResource(DWORD lang);

/** @} end of i18n  */

/** @} end of resource */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SGL_I18N_H */
