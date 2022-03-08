/********************************************************
 * Libini	: C语言配置文件解析器
*/
#ifndef ALAN_LIBINI_H
#define ALAN_LIBINI_H

#include <mrc_base.h>

#define LINE_LEN 128	// INI文件行长度
#define SECT_LEN 64	// 节点名称长度
#define VINT_LEN 64	// 整型数值长度
#define VCHR_LEN 64	// 字符型值长度

#define DEF_SECT_ROOT "information"	// 默认根节点名称
#define SECT_DOT '@' // 用于在节点下定位键名的标识符

typedef struct ini_item_ {
	char			*key;
	char			*value;
	char			*section;
	struct ini_item_	*next;

} ini_item;

typedef struct {
	char			*fname;
	struct ini_item_	*header;
	struct ini_item_	*tailer;
	int			 length;
} INI;

/********************************************************
 * ini_open	: 解析INI文件到缓冲区
 * 		  返回值为INI文件操作指针
 * @fname	: 要操作的INI文件名
*/
INI *ini_open(const char *fname);

/********************************************************
 * ini_create	: 创建一个新的INI文件
 * 		  返回值为INI文件操作指针
 * @fname	: 要创建的INI文件名
*/
INI *ini_create(const char *fname);

/********************************************************
 * ini_save	: 将缓冲区中的INI文件内容保存到文件
 * 		  返回值为0保存成功，否则为保存失败
 * @pini	: 要保存的INI文件指针
*/
int ini_save(INI *pini);

/********************************************************
 * ini_close	: 释放INI文件占用的内存资源
 * 		  无返回值
 * @pini	: 要释放的INI文件指针
*/
void ini_close(INI *pini);

/********************************************************
 * ini_get	: 取得指定键的键值
 * 		  返回值为保存键值的字符串指针
 * @pini	: INI文件指针
 * @key		: 键名字符串指针
 * @value	: 用于保存返回值的字符串指针
*/
char *ini_get(INI *pini,const char *key,char *value);

/********************************************************
 * ini_get_int	: 取得指定键的整型键值
 * 		  返回值整型键值
 * @pini	: INI文件指针
 * @key		: 键名字符串指针
*/
int ini_get_int(INI *pini,const char *key);

/********************************************************
 * ini_set	: 设置指定键的键值
 * 		  返回值为1设置成功，0设置失败
 * @pini	: INI文件指针
 * @key		: 键名字符串指针
 * @value	: 用于保存要设置的值的字符串指针
*/
int ini_set(INI *pini,const char *key,const char *value);

/********************************************************
 * ini_set_int	: 用整型值设置指定键的键值
 * 		  返回值为1设置成功，0设置失败
 * @pini	: INI文件指针
 * @key		: 键名字符串指针
 * @value	: 用于保存要设置的值的字符串指针
*/
int ini_set_int(INI *pini,const char *key,int value);

/********************************************************
 * ini_append	: 添加新键
 * 		  返回值为1设置成功，0设置失败
 * @pini	: INI文件指针
 * @key		: 键名字符串指针
 * @value	: 键值字符串指针
*/
int ini_append(INI *pini,const char *key,const char *value);

/********************************************************
 * ini_append_int:用整型数值来添加新键
 * 		  返回值为1设置成功，0设置失败
 * @pini	: INI文件指针
 * @key		: 键名字符串指针
 * @value	: 整型键值
*/
int ini_append_int(INI *pini,const char *key,int value);

/********************************************************
 * ini_remove	: 移除指定键
 * 		  返回值为1设置成功，0设置失败
 * @pini	: INI文件指针
 * @key		: 键名字符串指针
*/
int ini_remove(INI *pini,const char *key);

#endif//ALAN_LIBINI_H
