
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
    int32 len;
    // int  _charbuf;
    // int  _bufsiz;
    char *_tmpfname;
} CAPP_FILE;

CAPP_FILE *capp_fopen(const char *path, const char *mode);
unsigned int capp_fread(void *buf, size_t size, size_t count, CAPP_FILE *f);
int capp_fseek(CAPP_FILE *f, long offset, int mode);
int capp_fwrite(const void *buf, size_t size, size_t count, CAPP_FILE *f);
int capp_fprintf(CAPP_FILE *f, char *format, ...);
int capp_fflush(CAPP_FILE *f);
int capp_fclose(CAPP_FILE *f);
int capp_feof(CAPP_FILE *f);
int capp_getc(CAPP_FILE *f);
CAPP_FILE *capp_tmpfile();
CAPP_FILE *capp_freopen(const char *path,const char *mode, CAPP_FILE *file);
int capp_ferror(CAPP_FILE *file);
int capp_ftell(CAPP_FILE *f);
long mrc_strtol(const char *text,char **endptr,int base);
void *realloc(void *ptr, size_t newsize);
extern CAPP_FILE *base_stdin;
extern CAPP_FILE *base_stdout;
extern CAPP_FILE *base_stderr;
#define EOF      (-1)
#ifndef _IOFBF
#define _IOFBF           0x100 /* fully buffered IO */
#define _IOLBF           0x200 /* line buffered IO */
#define _IONBF           0x400 /* unbuffered IO */
#endif
char *getenv(const char *encar);

#define strerror(err) "no error"
char *capp_strpbrk(const char *str1, const char *str2);
size_t capp_strspn (const char *s,const char *accept);
double capp_strtod(const char* s, char** endptr);
#endif
