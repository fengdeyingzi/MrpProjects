/*
** bmp.h: the bitmap header file.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#ifndef _SGL_BMP_H
#define _SGL_BMP_H

#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif  /* __cplusplus */

/*
	λͼ����

		λͼ�����ǹ���Ķ����������ַ���������ͬ����
		λͼ������ģ��ֻ�ǲ�ͣ��λͼ�ļ�����Ϣ�����������ݡ�

		�����Ĳ�����λͼ��ģ�顣
		- λͼ�ļ�����Ϣ��д��Excel����ͼƬ����
		��ͬ����Ļ�ߴ�����в�ͬ��λͼ�ļ�����Ϣ��
		- ����һ����Ļ��С��ֻҪ������кꡣ
		��SCREEN_SIZE_176220���壬��SCREEN_SIZE_176220����Ϣ����ʹ�á�
		- ������ȷ��λͼ�ļ���MRP���ļ���
		- ��������λͼ��ID

		\code
#include "bmp.h"
		...
		//λͼ��ID��BMP_BUSY��

		int w, h; //��Ⱥ͸߶�

	HBITMAP bmp = SGL_LoadBitmap(BMP_BUSY, &w, &h);
	\endcode

		<br><br>
		*/
	 
#include "res_bmp.h"

/**
 * \��λͼID����λͼ���û�Ҳ���Ե���SGL_ReleaseBitmap���ͷ�λͼ
 *
 * \����bmp			λͼ��ID
 * \����[��]		λͼ�Ŀ�ȣ�����Ҫʱ����ΪNULL
 * \����[��]height	λͼ�ĸ߶ȣ�����Ҫʱ����ΪNULL
 * \����
 *	- �ɹ������ص�λͼ���
 *	- ���򣬿�
 * \��Ӧ SGL_ReleaseBitmap.
 */
HBITMAP GUIAPI SGL_LoadBitmap(DWORD bmp, int* width, int* height);

/**
 * \����λͼ���ͷ��ڴ棬���ܶ�û�м��ص�λͼʹ�á�
 * \����bmp		λͼ��ID
 * \��Ӧ SGL_LoadBitmap
 */
VOID GUIAPI SGL_ReleaseBitmap(DWORD bmp);

/** @} end of bitmap */
	
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SGL_BMP_H */

