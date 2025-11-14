#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__


#include <mrc_base.h>

typedef struct {
    char *buffer;     // 用于存储字符串的动态数组
    int32 size;      // 当前字符串长度
    int32 capacity;  // 缓冲区总容量
} STRING_BUFFER;

// 初始化STRING_BUFFER
STRING_BUFFER* createStringBuffer();

// 释放STRING_BUFFER
void freeStringBuffer(STRING_BUFFER *sb);
// 添加字符串
void append(STRING_BUFFER *sb, const char *str);

// 删除字符串末尾的n个字符
void deleteLast(STRING_BUFFER *sb, int n);

// 获取当前字符串
const char* getString(STRING_BUFFER *sb);


#endif
