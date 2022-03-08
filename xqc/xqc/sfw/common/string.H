#ifndef _STRING_H
#define _STRING_H

#include "types.h"

/* 转换unicode的结构
   使用后用 mrc_freeOrigin 释放内存
 */
typedef struct {
	uint16* pText;  //unicode字符串指针
	int32 len;      //字符串缓冲长度
}UnicodeSt;


//unicode转GB编码函数
uint8*  UnicodeToChar(PWSTR unicode); //有分配内存

//GB转Unicode编码函数
uint16* CharToUnicode(PSTR gb);     //没有分配内存

VOID CharToUnicode2(char* in, uint16** out, int32* outlen);


//屏幕中央显示文字函数
void DrawTextMidpoint(char* text,uint8 R,uint8 G,uint8 B,int8 is_unicode);

/**
 * \转换为小写的字符串。
 *
 * \参数str    转换的字符串
 * \返回小写字符串
 */
 
PSTR tolower(PSTR str);

/**
 * \转换为大写的字符串。
 *
 * \参数str    转换的字符串
 * \返回大写字符串
 */
 
PSTR toupper(PSTR str);

/**
 * \brief Compare two string with case ignore.
 *
 * \param dst the first string
 * \param src the second string
 * \return
 *	<0 when dst < src
 *	=0 when dst = src
 *	>0 when dst > src
 *
 * \param dst 第一个字符串参数
 * \param src 第二个字符串参数
 * \返回
 *	<0 当 dst < src
 *	=0 当 dst = src
 *	>0 当 dst > src
 *
 * \比较两个case忽略字符串。
 */
 
Sint32 stricmp(PCSTR dst, PCSTR src);

/**
 * \brief Trim left space chars here just ' '.
 *
 * \param str the string to be trimed
 * \return the trimed string
 * \修整字符只是在这里留下空间'。
 *
 * \参数str的字符串被trimed
 * \返回trimed字符串
 */
PSTR trimleft(PSTR str);

/**
 * \brief Trim right space chars here just ' '.
 *
 * \param str the string to be trimed
 * \return the trimed string
 * \修整正确的空间字符这里只是' '。
 *
 * \参数str的字符串被trimed
 * \返回trimed字符串
 */
PSTR trimright(PSTR str);

/**
 * \brief Trim(整理) left&right space chars.
 *
 * \param str the string to be trimed
 * \return the trimed string
 * \修整左，右空间字符。
 *
 * \参数str的字符串被trimed
 * \返回trimed字符串
 */
PSTR trim(PSTR str);

/**
 * \brief Split(分离) the string with specific(特殊) char.
 * 
 * \param str the string to be splited
 * \param sep the seperator char
 * \param[out] values the splited string array
 * \param size the max size of values
 * \return real size of values array returned
 * \的拆分与特定字符的字符串。
 *
 * \参数str的字符串被splited
 * \参数的分隔符字符九月
 * \参数[出]字符串数组值的splited
 * \参数值的大小，最大尺寸
 * \返回返回值数组的实际大小
 */
Sint32 split(PSTR str, CHAR sep, PSTR values[], Sint32 size);

/**
* \得到wstring的长度。
*
* \返回字节的wstring的长度(字节)
 * \brief Get length of the wstring in bytes.
 *
 * \param str the wstring
 * \return the length of the wstring in bytes
 */
Sint32 wstrlen(PCWSTR str);

/**
 * \brief Copy characters of one wstring to another.
 *
 * \param dst Destination string
 * \param src Source string
 * \return the destination string
 * \一wstring的字符复制到另一个。
 *
 * \参数dst的目标字符串
 * \ src输入字符串参数
 * \返回目标字符串
 */
PWSTR wstrcpy(PWSTR dst, PCWSTR src);

/**
 * \brief Copy characters of one wstring to another.
 *
 * \param dst Destination string
 * \param src Source string
 * \param size of bytes to be copied
 * \return the destination string

 * \一wstring的字符复制到另一个。
 *
 * \参数dst     目标字符串
 * \参数src     输入字符串
 * \参数        要复制的字节大小
 * \返回目标字符串
 */
PWSTR wstrncpy(PWSTR dst, PCWSTR src, Sint32 size);

/**
 * \brief Compare strings.
 *
 * \param str1 the first string
 * \param str2 the second string
 * \return
 *	0 when str1 == str2
 *	<0 when str1 < str2
 *	>0 when str1 > str2

 * \比较字符串。
 *
 * \参数 str1 the 第一个字符串
 * \参数 str2 the 第二个字符串
 * \返回
 *	 0 当 str1 == str2
 *	<0 当 str1 < str2
 *	>0 当 str1 > str2
 */
Sint32 wstrcmp(PCWSTR str1, PCWSTR str2);

/**
 * \brief Find a wchar in wstring.
 * 
 * \param str the wstring
 * \param chr1 the first byte of the wchar
 * \param chr2 the second byte of the wchar
 * \return the first occurrence of c in string, NULL when not found

 * \查找wstring中的一个wchar。
 *
 * \参数str     wstring
 * \参数chr1    wchar的第一字节
 * \参数chr2    wchar的第二字节
 * \返回c第一次出现在字符串，NULL时未找到
 */
PCWSTR wstrchr(PCWSTR str, UCHAR chr1, UCHAR chr2);

/**
 * \brief Trim left a wstring.
 *
 * \param str the wstring to be trimed
 * \return the trimed string

 * \修整留下了wstring的。
 *
 * \参数str中的wstring的是trimed
 * \返回trimed字符串
 */
PWSTR wtrimleft(PWSTR str);

/**
 * \brief Trim right a wstring.
 * 
 * \param str the wstring to be trimed
 * \return the trimed string
 *
 * \修整一个wstring的权利。
 *
 * \参数str中的wstring的是trimed
 * \返回trimed字符串
 */
PWSTR wtrimright(PWSTR str);

/**
 * \brief Trim a wstring.
 * 
 * \param str the wstring to be trimed
 * \return the trimed string

 * \修剪wstring的。
 *
 * \参数str中的wstring的是trimed
 * \返回trimed字符串
 */
PWSTR wtrim(PWSTR str);

/**
 * \brief Convert utf8 string to unicode string.
 *
 * \param utf8 the utf8 string to be transformed
 * \param unicode the unicode buffer for transformed
 * \param size the max size of unicode buffer
 * \return the transformed size in bytes

 * \utf8字符串转换为Unicode字符串。
 *
 * \参数UTF8               要转换的UTF8字符串
 * \参数[入,出]Unicode     转换Unicode的缓冲区
 * \参数[入,出]size        Unicode缓冲区的大小
 * \返回转换后的长度(字节)
 */
Sint32 str2wstr(PCSTR utf8, PWSTR unicode, Sint32 size);

/**
 * \brief Convert unicode string to utf8 string.
 *
 * \param unicode the unicode string to be transformed
 * \param utf8 the utf8 buffer for transformed
 * \param size the max size of utf8 buffer
 * \return the transformed size in bytes
 * \Unicode字符串转换成utf8的字符串。
 *
 * \参数Unicode 要转换的Unicode字符串
 * \参数UTF8    转换utf8的缓冲区
 * \参数size    utf8缓冲区的最大大小
 * \返回转换后的长度(字节)
 */
Sint32 wstr2str(PCWSTR unicode, PSTR utf8, Sint32 size);

char *strDup(char *pcStr);
PWSTR wstrcat(PWSTR dst, PCWSTR src);
char *strChrEx(const char *src,int c);
PWSTR trimCRLF(PWSTR str);
char *itoa(int num,char *str,int radix);
PWSTR wstrdup(PCWSTR str);

#endif
