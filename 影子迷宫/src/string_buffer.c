#include <mrc_base.h>
#include "string_buffer.h"

// ��ʼ��STRING_BUFFER
STRING_BUFFER* createStringBuffer() {
    STRING_BUFFER *sb = (STRING_BUFFER *)mrc_malloc(sizeof(STRING_BUFFER));
    sb->size = 0;
    sb->capacity = 16; // ��ʼ����
    sb->buffer = (char *)mrc_malloc(sb->capacity * sizeof(char));
    sb->buffer[0] = '\0'; // ȷ����ʼ�ַ����ǿյ�
    return sb;
}

// �ͷ�STRING_BUFFER
void freeStringBuffer(STRING_BUFFER *sb) {
    mrc_free(sb->buffer);
    mrc_free(sb);
}

// ����ַ���
void append(STRING_BUFFER *sb, const char *str) {
    char *tempbuf;
    int32 len = mrc_strlen(str);
    // ȷ�����㹻�Ŀռ�
    while (sb->size + len >= sb->capacity) {
        sb->capacity *= 2; // ��������
        tempbuf = sb->buffer;
        
        sb->buffer = (char *)mrc_malloc(sb->capacity * sizeof(char));
        mrc_memset(sb->buffer, 0, sb->capacity * sizeof(char));
        mrc_memcpy(sb->buffer, tempbuf, sb->size);
        mrc_free(tempbuf);
    }
    mrc_strcpy(sb->buffer+sb->size, str);
    sb->size += len;
}

// ɾ���ַ���ĩβ��n���ַ�
void deleteLast(STRING_BUFFER *sb, int n) {
    if (n > sb->size) {
        n = sb->size; // ��ֹɾ��������ǰ��С
    }
    sb->size -= n;
    sb->buffer[sb->size] = '\0'; // ȷ���ַ�����NULL��β
}

// ��ȡ��ǰ�ַ���
const char* getString(STRING_BUFFER *sb) {
    return sb->buffer;
}

