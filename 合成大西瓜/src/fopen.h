#ifndef _FOPEN_H_
#define _FOPEN_H_

#include "mpc.h"
typedef struct _iobuf
{
    int _file; //进程级文件表 记录文件打开数
    char *_tmpfname;
} FILE;

FILE *fopen(const char *path, const char *mode);
int fseek(FILE *f, long offset, int mode);
unsigned int fread(void *buf, size_t size, size_t count, FILE *f);
unsigned int fwrite(const void *buf, size_t size, size_t count, FILE *f);
int fflush(FILE *f);
int fclose(FILE *f);

#endif
