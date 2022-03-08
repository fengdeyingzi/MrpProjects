/*
** types.h: the types header file, contains all the base types for simple gui library.
**
** Copyright (C) 2007-2008 SKY-MOBI AS.  All rights reserved.
**
** This file is part of the simple gui library.
** It may not be redistributed under any circumstances.
*/
#ifndef _SGL_TYPES_H
#define _SGL_TYPES_H

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \8λ�޷����ַ����͵Ķ���
 */
typedef unsigned char   Uint8;
/**
 * \8λ�з����ַ����͵Ķ���
 */
typedef signed char     Sint8;
/**
 * \16λ�޷����������Ͷ���
 */
typedef unsigned short  Uint16;
/**
 * \16λ�з����������Ͷ���
 */
typedef signed short    Sint16;
/**
 * \32λ�޷����������Ͷ���
 */
typedef unsigned int    Uint32;
/**
 * \32λ�з����������Ͷ���
 */
typedef signed int      Sint32;

/**
 * \����ֵ���͵Ķ���
 */
typedef int BOOL;

/**
 * \FALSEֵ������Ϊ0
 */
#ifndef FALSE
#define FALSE       0
#endif

/**
 * \TRUEֵ������Ϊ1
 */
#ifndef TRUE
#define TRUE        1
#endif

/**
 * \��ָ��
 */
#ifndef NULL
#define NULL            ((void *)0)
#endif

/**
 * \void���͵Ķ���
 */
#define VOID            void

/**
 * \GUIAPI���Ͷ���
 */
#define GUIAPI

/**
 * \���ڻ�ؼ��ľ�����Ͷ���
 */
typedef struct SGL_Window* HWND;

/**
 * \�˵��ľ�����Ͷ���
 */
typedef HWND HMENU;

/**
 * \����ľ�����Ͷ��塣
 */
typedef Uint32 HFONT;

/**
 * \λͼ�ľ�����Ͷ���
 */
typedef VOID* HBITMAP;

/**
 * \�������͵Ķ���
 */
typedef Uint16 WID;

/**
 * \8λ�з����ַ����͵Ķ���
 */
//typedef char char;

/**
 * \8λ�޷����ַ����͵Ķ���
 */
typedef Uint8 UCHAR;

/**
 * \wstringָ������Ͷ���
 */
typedef UCHAR* PWSTR;

/**
 * \const wstringָ������Ͷ���
 */
typedef const UCHAR* PCWSTR;

/**
 * \�ַ���ָ������Ͷ���
 */
typedef char* PSTR;

typedef char CHAR;

/**
 * \const�ַ���ָ������Ͷ���
 */
typedef const char* PCSTR;

/**
 * \�޷����ֽڵ����Ͷ���
 */
typedef unsigned char   BYTE;

/**
 * \�ֽ�ָ������Ͷ���
 */
typedef BYTE*		PBYTE;

/**
 * \�з����ַ������Ͷ���
 */
typedef signed char     SBYTE;

/**
 * \�޷��Ŷ��������֣������Ͷ���
 */
typedef unsigned short  WORD;

/**
 * \�з��Ŷ��������͵Ķ��塣
 */
typedef signed short    SWORD;

/**
 * \�޷��ų����ͣ�˫�֣����Ͷ��塣
 */
typedef unsigned long   DWORD;

/**
 * \һ���з��ų��������͵Ķ��塣
 */
typedef signed long     SDWORD;

/**
 * \�޷����������Ͷ���
 */
typedef unsigned int    UINT;

/**
 * \���������Ͷ���
 */
typedef long            LONG;

/**
 * \��һ����Ϣ�������Ͷ���
 */
typedef UINT            WPARAM;

/**
 * \�ڶ�����Ϣ�������Ͷ���
 */
typedef DWORD           LPARAM;

/**
 *\������Ͷ���
 */
typedef DWORD		LRESULT;

/**
 * \�ֵĵͰ�λ�ֽ�
 */
#define LOBYTE(w)           ((BYTE)(w))

/**
 * \�ֵĸ߰�λ�ֽ�
 */
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

/**
 * \�ߵ��ֽںϲ�Ϊ��
 */
#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)))

/**
 * \����˫�ֵĵ���
 */
#define LOWORD(l)           ((WORD)(DWORD)(l))

/**
 * \����˫�ֵĸ���
 */
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))

/**
 * \����˫�ֵĵ���
 */
#define LOSWORD(l)          ((SWORD)(DWORD)(l))

/**
 * \����˫�ֵĸ���
 */
#define HISWORD(l)          ((SWORD)((((DWORD)(l)) >> 16) & 0xFFFF))

/**
 * \�ߵ��ֺϲ�Ϊ˫��
 */
#define MAKELONG(low, high) ((DWORD)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))

/**
 * \����������
 */
typedef struct _RECT
{
    int left;		/*���ε����Ͻ�X����*/
    int top;		/*���ε����Ͻ�Y����*/
    int width;		/*���εĿ��*/
    int height;		/*���εĸ߶�*/

} RECT, *PRECT;

/**
 * \�귵�ص�RECT��ȷλ��
 */
#define RECT_RIGHT(r) (r->left + r->width - 1)

/**
 * \�귵�صľ��εײ�λ��
 */
#define RECT_BOTTOM(r) (r->top + r->height - 1)

/**
 * \��ṹ���Ͷ���
 */
typedef struct _POINT
{
    int x;		/*X����ĵ�*/
    int y;		/*Y����ĵ�*/

}POINT, *PPOINT;

/**
 * \2ά����ṹ
 */
typedef struct _SIZE
{
    int cx;
    int cy;
}SIZE, *PSIZE;

/* ����24λRGB888��ʽRGB���أ�0x00RRGGBB��*/
#define RGB2PIXEL888(r,g,b)   (((r) << 16) | ((g) << 8) | (b))

/* ����16λRGB565��ʽ����*/
#define RGB2PIXEL565(r,g,b)   ((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))

/* ����24λ��RGB ��0x00RRGGBB��* RGB888��ʽ����*/
#define COLOR2PIXEL888(c)    \
    ((((c) & 0xff) << 16) | ((c) & 0xff00) | (((c) & 0xff0000) >> 16))

/* ����16λ��RGB ��0x00RRGGBB��* RGB565��ʽ����*/
#define COLOR2PIXEL565(c)    \
    ((((c) & 0xf8) << 8) | (((c) & 0xfc00) >> 5) | (((c) & 0xf80000) >> 19))

/* ����24λ����RGB888λR��G��B���*/
#define PIXEL888RED(pixel)        (Uint8)(((pixel) >> 16) & 0xff)
#define PIXEL888GREEN(pixel)      (Uint8)(((pixel) >> 8) & 0xff)
#define PIXEL888BLUE(pixel)       (Uint8)((pixel) & 0xff)

/* ����16λ����RGB565λR��G��B���*/
#define PIXEL565RED(pixel)        (Uint8)(((pixel) >> 11) & 0x1f)
#define PIXEL565GREEN(pixel)      (Uint8)(((pixel) >> 5) & 0x3f)
#define PIXEL565BLUE(pixel)       (Uint8)((pixel) & 0x1f)

/**
 * \�귵�������Ԫ�ظ���
 */
#define TABLESIZE(table)    (sizeof(table)/sizeof(table[0]))


#ifndef MAX
#define MAX(x, y)           (((x) > (y))?(x):(y))
#endif


#ifndef MIN
#define MIN(x, y)           (((x) < (y))?(x):(y))
#endif

/**
 * \�귵�ؾ���ֵ
 */
#ifndef ABS
#define ABS(x)              (((x)<0) ? -(x) : (x))
#endif


#ifndef DIV
#define DIV(x, y)		((x) / (y))
#endif


#ifndef MOD
#define MOD(x, y)		((x) % (y))
#endif

/**
 * \��Ч��Դ
 */
#ifndef RESID_INVALID
#define RESID_INVALID	0xFFFFFFFFL
#endif

/** @} end of sgltypes */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _SGL_TYPES_H */

