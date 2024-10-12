#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <mrc_base.h>

#define MAX_BUFFER_LEN 1024*20 //��󱣴�20kb ���˾Ͷ���
typedef struct STRING_BUFFER
{
    char *data;
    char *header; //ͷ��Ϣ
    char *body; //����
    int32 body_len; //body����
    int32 len; //�ڴ泤��
    int32 max; //�ڴ��С
} STRING_BUFFER;

STRING_BUFFER* buffer_create(void);
int32 buffer_append(STRING_BUFFER* buffer, char c);
int32 buffer_appendx(STRING_BUFFER* buffer, char *buf, int32 len);
int32 buffer_clear(STRING_BUFFER* buffer);
int32 buffer_free(STRING_BUFFER* buffer);
char *strstr_x(char* text, int32 len, char *seText);
char *getline_value(char *text);
int32 isContentSuccess(char *text,int32 content_len);
#endif
