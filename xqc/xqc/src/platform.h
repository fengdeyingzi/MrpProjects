/* �����ض�ƽ̨�İ����Ͷ��嶼������ļ��� */
#ifndef PLATFORM_H
#define PLATFORM_H


#define ALIGN_TYPE void*                    /* Ĭ�ϵ��������ͣ�����ʹ�� */
#define GLOBAL_TABLE_SIZE 97                /* ȫ�ֱ����� */
#define STRING_TABLE_SIZE 97                /* �����ַ�����Ĵ�С */
#define STRING_LITERAL_TABLE_SIZE 97        /* �ַ������ֱ�Ĵ�С */
#define PARAMETER_MAX 16                    /* ����������������� */
#define LINEBUFFER_MAX 256                  /* �е�����ַ��� */
#define LOCAL_TABLE_SIZE 11                 /* �ֲ�������Ĵ�С������չ�� */
#define STRUCT_TABLE_SIZE 11                /* �ṹ/���ϳ�Ա��Ĵ�С������չ�� */

#define INTERACTIVE_PROMPT_START "starting picoc " _VERSION "\n"
#define INTERACTIVE_PROMPT_STATEMENT "picoc> "
#define INTERACTIVE_PROMPT_LINE "     > "

#define XQC_PRINT_FILE "c/print.txt"


// #define USE_MALLOC_STACK
// #define USE_MALLOC_HEAP

#include "mrc_base.h"
#include "mrc_base_i.h"

typedef char * va_list;
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#define va_start(ap,v)  ( ap = (va_list)(&(v)) + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )
#define assert(x)       //MRC_ASSERT(x)


#endif /* PLATFORM_H */
