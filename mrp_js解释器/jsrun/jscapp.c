
#include <stdarg.h>
#include "jscapp.h"
#include <mrc_base.h>

// static char printBuf[PRINTF_BUF_LEN] = {0};
char printf_file[20]; /* 运行时初始化 */

void jscapp_init(void) {
	strcpy(printf_file, "print.txt");
}

void file_printf(const char *format, ...)
{
    int fd;
    static char printfBuf[PRINTF_BUF_LEN] = {0};
    va_list args;
    int n;
    va_start(args, format);
    n = vsnprintf(printfBuf,PRINTF_BUF_LEN, format, args);
    mrc_printf("printf:%s", printfBuf);
    fd = mrc_open(printf_file, 12);
    if(fd>0){ //打开成功
        mrc_seek(fd, 0, MR_SEEK_END);
        mrc_write(fd,&printfBuf,strlen(printfBuf));
        mrc_close(fd);
    }
    va_end(args);
}
/*
void file_printf(const char *format, ...)
{
    int fd;
    static char printfBuf[PRINTF_BUF_LEN] = {0};
    va_list args;
    int n;
    va_start(args, format);
    n = vsnprintf(printfBuf,PRINTF_BUF_LEN, format, args);
    printf("printf:%s", printfBuf);
    #ifdef _LINUX
    fd = open(printf_file,O_CREAT|O_WRONLY, S_IRWXU|S_IRUSR|S_IXUSR|S_IROTH|S_IXOTH);
    #else
    fd = open(printf_file,O_CREAT|O_WRONLY);
    #endif
    if(fd>=0){ //打开成功
        lseek(fd, 0, SEEK_END);
        write(fd,&printfBuf,strlen(printfBuf));
        close(fd);
    }
    va_end(args);
}
*/
