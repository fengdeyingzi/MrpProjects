#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <mrc_base.h>

#define MAX_BUFFER_LEN 1024*20 //最大保存20kb 多了就丢弃
typedef struct STRING_BUFFER
{
    char *data;
    char *header; //头信息
    char *body; //身体
    int32 len; //内存长度
    int32 max; //内存大小
} STRING_BUFFER;

STRING_BUFFER* buffer_create();
int32 buffer_append(STRING_BUFFER* buffer, char c);
int32 buffer_appendx(STRING_BUFFER* buffer, char *buf, int32 len);
int32 buffer_free(STRING_BUFFER* buffer);
char *strstr_x(char* text, int32 len, char *seText);
char *getline_value(char *text);
int32 isContentSuccess(char *text,int32 content_len);
#endif
