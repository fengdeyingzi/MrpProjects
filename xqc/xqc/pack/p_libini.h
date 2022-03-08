/********************************************************
 * Libini	: C���������ļ�������
*/
#ifndef ALAN_LIBINI_H
#define ALAN_LIBINI_H

#include <mrc_base.h>

#define LINE_LEN 128	// INI�ļ��г���
#define SECT_LEN 64	// �ڵ����Ƴ���
#define VINT_LEN 64	// ������ֵ����
#define VCHR_LEN 64	// �ַ���ֵ����

#define DEF_SECT_ROOT "information"	// Ĭ�ϸ��ڵ�����
#define SECT_DOT '@' // �����ڽڵ��¶�λ�����ı�ʶ��

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
 * ini_open	: ����INI�ļ���������
 * 		  ����ֵΪINI�ļ�����ָ��
 * @fname	: Ҫ������INI�ļ���
*/
INI *ini_open(const char *fname);

/********************************************************
 * ini_create	: ����һ���µ�INI�ļ�
 * 		  ����ֵΪINI�ļ�����ָ��
 * @fname	: Ҫ������INI�ļ���
*/
INI *ini_create(const char *fname);

/********************************************************
 * ini_save	: ���������е�INI�ļ����ݱ��浽�ļ�
 * 		  ����ֵΪ0����ɹ�������Ϊ����ʧ��
 * @pini	: Ҫ�����INI�ļ�ָ��
*/
int ini_save(INI *pini);

/********************************************************
 * ini_close	: �ͷ�INI�ļ�ռ�õ��ڴ���Դ
 * 		  �޷���ֵ
 * @pini	: Ҫ�ͷŵ�INI�ļ�ָ��
*/
void ini_close(INI *pini);

/********************************************************
 * ini_get	: ȡ��ָ�����ļ�ֵ
 * 		  ����ֵΪ�����ֵ���ַ���ָ��
 * @pini	: INI�ļ�ָ��
 * @key		: �����ַ���ָ��
 * @value	: ���ڱ��淵��ֵ���ַ���ָ��
*/
char *ini_get(INI *pini,const char *key,char *value);

/********************************************************
 * ini_get_int	: ȡ��ָ���������ͼ�ֵ
 * 		  ����ֵ���ͼ�ֵ
 * @pini	: INI�ļ�ָ��
 * @key		: �����ַ���ָ��
*/
int ini_get_int(INI *pini,const char *key);

/********************************************************
 * ini_set	: ����ָ�����ļ�ֵ
 * 		  ����ֵΪ1���óɹ���0����ʧ��
 * @pini	: INI�ļ�ָ��
 * @key		: �����ַ���ָ��
 * @value	: ���ڱ���Ҫ���õ�ֵ���ַ���ָ��
*/
int ini_set(INI *pini,const char *key,const char *value);

/********************************************************
 * ini_set_int	: ������ֵ����ָ�����ļ�ֵ
 * 		  ����ֵΪ1���óɹ���0����ʧ��
 * @pini	: INI�ļ�ָ��
 * @key		: �����ַ���ָ��
 * @value	: ���ڱ���Ҫ���õ�ֵ���ַ���ָ��
*/
int ini_set_int(INI *pini,const char *key,int value);

/********************************************************
 * ini_append	: ����¼�
 * 		  ����ֵΪ1���óɹ���0����ʧ��
 * @pini	: INI�ļ�ָ��
 * @key		: �����ַ���ָ��
 * @value	: ��ֵ�ַ���ָ��
*/
int ini_append(INI *pini,const char *key,const char *value);

/********************************************************
 * ini_append_int:��������ֵ������¼�
 * 		  ����ֵΪ1���óɹ���0����ʧ��
 * @pini	: INI�ļ�ָ��
 * @key		: �����ַ���ָ��
 * @value	: ���ͼ�ֵ
*/
int ini_append_int(INI *pini,const char *key,int value);

/********************************************************
 * ini_remove	: �Ƴ�ָ����
 * 		  ����ֵΪ1���óɹ���0����ʧ��
 * @pini	: INI�ļ�ָ��
 * @key		: �����ַ���ָ��
*/
int ini_remove(INI *pini,const char *key);

#endif//ALAN_LIBINI_H
