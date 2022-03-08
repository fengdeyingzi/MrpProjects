/*
** funs.h: the functions header file.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/

#ifndef _SGL_RECT_H
#define _SGL_RECT_H

#include "gal.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \���2�����Ƿ��ཻ
 *
 * \����psrc1	��һ������
 * \����psrc2	�ڶ�������
 */
BOOL GUIAPI DoesIntersect(PRECT psrc1, PRECT psrc2);

/**
 * \brief union psrc to pdst
 *
 * \����pdst	dest����
 * \����psrc	src����
 */
VOID GUIAPI UnionRect(PRECT pdst, PRECT psrc);

/**
 * \�ཻ����
 *
 * \����pdst	�������
 * \����psrc1	��һ����
 * \����psrc2	�ڶ�������
 */
VOID GUIAPI IntersectRect(PRECT pdst, PRECT psrc1, PRECT psrc2);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SGL_RECT_H */

