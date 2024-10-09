#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__


#include <mrc_base.h>

typedef struct {
    char *buffer;     // ���ڴ洢�ַ����Ķ�̬����
    int32 size;      // ��ǰ�ַ�������
    int32 capacity;  // ������������
} STRING_BUFFER;

// ��ʼ��STRING_BUFFER
STRING_BUFFER* createStringBuffer();

// �ͷ�STRING_BUFFER
void freeStringBuffer(STRING_BUFFER *sb);
// ����ַ���
void append(STRING_BUFFER *sb, const char *str);

// ɾ���ַ���ĩβ��n���ַ�
void deleteLast(STRING_BUFFER *sb, int n);

// ��ȡ��ǰ�ַ���
const char* getString(STRING_BUFFER *sb);


#endif
