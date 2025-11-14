#ifndef __UNICODE_H__
#define __UNICODE_H__
#include "mrc_base.h"

/*
 * unicode strcat
 * 参数:
 * dest:			目的字符串
 * src:				源字符串
 * 返回值:			无
 */
char *uniStrcat(char *dest, char *src);

/*
 * unocode strcpy
 */
void uniStrcpy(char * dest, char * src);

/*
 * unicode ascii to ascii
 * 参数:
 * input:			unocode字符串(只支持ascii形式的unicode, 不支持中文)
 * output:			输出ascii码字符串buf
 * outLen:			输出的buf长度
 * 返回值:			无
 */
void utoc(char * input, char * outPut, int32 outLen);

/* 
 * ascii to unicode
 * 参数:
 * dest:			输出对应ascii的unocode形式
 * src:				输入ascii码字符串
 * srcLen:			输入ascii码字符串的长度
 * 返回值:			无
 */
void ctou(char *dest, char *src, int32 srcLen);

/*
 * unicode strcmp, unicode字符串比较
 * 参数:
 * s1:				unicode字符串1
 * s2:				unicode字符串2
 * 返回值:			0:	相同,  非0  不相同
 */
int32 ucsCmp(char *s1,char *s2);

/* unicode strcmp, unicode字符串比较
 * 参数:
 * s1:				unicode字符串1
 * s2:				unicode字符串2
 * n:				比较的unocode字符个数
 * 返回值:			0:	相同,  非0  不相同
 */
int32 ucsNCmp(char * s1, char * s2, int32 n);

/* 过滤unocode字符串的前后空格 */
char* uniStrtrim(char *str);
/* 过滤unocode字符串的所有空格和换行符 */
char* uniStrtrimLF(char *str);


#endif /* __UNICODE_H__ */
