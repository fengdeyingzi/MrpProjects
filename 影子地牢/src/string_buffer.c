#include <mrc_base.h>
#include "string_buffer.h"

// 初始化STRING_BUFFER
STRING_BUFFER* createStringBuffer() {
    STRING_BUFFER *sb = (STRING_BUFFER *)mrc_malloc(sizeof(STRING_BUFFER));
    sb->size = 0;
    sb->capacity = 16; // 初始容量
    sb->buffer = (char *)mrc_malloc(sb->capacity * sizeof(char));
    sb->buffer[0] = '\0'; // 确保初始字符串是空的
    return sb;
}

// 释放STRING_BUFFER
void freeStringBuffer(STRING_BUFFER *sb) {
    mrc_free(sb->buffer);
    mrc_free(sb);
}

// 添加字符串
void append(STRING_BUFFER *sb, const char *str) {
    char *tempbuf;
    int32 len = mrc_strlen(str);
    // 确保有足够的空间
    while (sb->size + len >= sb->capacity) {
        sb->capacity *= 2; // 扩大容量
        tempbuf = sb->buffer;
        
        sb->buffer = (char *)mrc_malloc(sb->capacity * sizeof(char));
        mrc_memset(sb->buffer, 0, sb->capacity * sizeof(char));
        mrc_memcpy(sb->buffer, tempbuf, sb->size);
        mrc_free(tempbuf);
    }
    mrc_strcpy(sb->buffer+sb->size, str);
    sb->size += len;
}

// 删除字符串末尾的n个字符
void deleteLast(STRING_BUFFER *sb, int n) {
    if (n > sb->size) {
        n = sb->size; // 防止删除超过当前大小
    }
    sb->size -= n;
    sb->buffer[sb->size] = '\0'; // 确保字符串以NULL结尾
}

// 获取当前字符串
const char* getString(STRING_BUFFER *sb) {
    return sb->buffer;
}

