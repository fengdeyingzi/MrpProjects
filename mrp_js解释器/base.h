
#ifndef __BASE_H__
#define __BASE_H__
#include <mrc_base.h>
typedef unsigned int size_t;

typedef struct _iobuf_capp
{
    // char*    _ptr; //文件名？？
    // int  _cnt;
    // char*    _base;
    // int  _flag;
    int32 _file; //进程级文件表 记录文件打开数
    // int  _charbuf;
    // int  _bufsiz;
    char *_tmpfname;
} CAPP_FILE;

CAPP_FILE *capp_fopen(const char *path, const char *mode);
unsigned int capp_fread(void *buf, size_t size, size_t count, CAPP_FILE *f);
int capp_fseek(CAPP_FILE *f, long offset, int mode);
int capp_fwrite( void *buf, size_t size, size_t count, CAPP_FILE *f);
int capp_fflush(CAPP_FILE *f);
int capp_fclose(CAPP_FILE *f);
int capp_ftell(CAPP_FILE *f);
long mrc_strtol(const char *text,char **endptr,int base);
int capp_putchar(char ch);
#endif
