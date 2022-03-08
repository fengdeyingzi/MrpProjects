/*
** sound.h: the sound header file.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#ifndef _SGL_SOUND_H
#define _SGL_SOUD_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


/**
  \��������

    \code
		#include "sound.h"
		...
		//һ��������IDΪ��SOUND_ONLINE��

		SGL_PlaySound(SOUND_ONLINE, FALSE);
	\endcode
  *//**
  \��������

    \code
		#include "sound.h"
		...
		//һ��������IDΪ��SOUND_ONLINE��

		SGL_PlaySound(SOUND_ONLINE, FALSE);
	\endcode
  */

#include "res_sound.h"

/**
 * \��ID����������
 * \����sound	����ʶ����
 * \����loop	����ģʽ
 *	-TRUE		ѭ������
 *	-FALSE		����һ��
 * \�ɹ�����true�����򷵻�FALSE
 */
BOOL GUIAPI SGL_PlaySound(DWORD sound, BOOL loop);

/**
 * \ֹͣ����
 *
 * \����sound	����ʶ����
 * \�ɹ�����true�����򷵻�FALSE
 */
BOOL GUIAPI SGL_StopSound(DWORD sound);

/**
 * \�ͷ�������Դ
 *
 * \����sound	����ʶ����
 * \û�м���ʱ��Ҫʹ��
 */
VOID GUIAPI SGL_ReleaseSound(DWORD sound);

/** @} end of sound */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
