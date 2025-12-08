
#include <mrc_base.h>
#include "fopen.h"
#include "mpc.h"

#define O_RDONLY MR_FILE_RDONLY
#define O_WRONLY MR_FILE_WRONLY
#define O_CREAT MR_FILE_CREATE
#define O_RDWR MR_FILE_RDWR

FILE *fopen(const char *path, const char *mode)
{
    int file_mode = 0;
    int seek_mode = MR_SEEK_SET;
    int32 f = 0;
    FILE *file = NULL;
    int filelen = 0;
    if (strcmp(mode, "r") == 0 || strcmp(mode, "rb") == 0)
    { //以只读方式打开文件，该文件必须存在。
        file_mode = O_RDONLY;
    }
    else if (strcmp(mode, "r+") == 0)
    { //以读/写方式打开文件，该文件必须存在。
        file_mode = MR_FILE_RDWR;
    }
    else if (strcmp(mode, "rb+") == 0)
    { //以读/写方式打开一个二进制文件，只允许读/写数据。
        file_mode = MR_FILE_RDWR;
    }
    else if (strcmp(mode, "rt+") == 0)
    { //以读/写方式打开一个文本文件，允许读和写。
        file_mode = MR_FILE_RDWR;
    }
    else if (strcmp(mode, "w") == 0)
    { //打开只写文件，若文件存在则长度清为0，即该文件内容消失，若不存在则创建该文件。
        file_mode = O_WRONLY | O_CREAT;
        remove(path);
    }
    else if (strcmp(mode, "w+") == 0)
    { //打开可读/写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
        file_mode = MR_FILE_RDWR | O_CREAT;
        remove(path);
    }
    else if (strcmp(mode, "a") == 0)
    { //以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留（EOF符保留)。
        file_mode = O_WRONLY | O_CREAT;
        seek_mode = MR_SEEK_END;
    }
    else if (strcmp(mode, "a+") == 0)
    { //以附加方式打开可读/写的文件。若文件不存在，则会建立该文件，如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（原来的EOF符 不保留)。
        file_mode = MR_FILE_RDWR;
        seek_mode = MR_SEEK_END;
    }
    else if (strcmp(mode, "wb") == 0)
    { //以只写方式打开或新建一个二进制文件，只允许写数据。
        file_mode = O_WRONLY;
    }
    else if (strcmp(mode, "wb+") == 0)
    { //以读/写方式打开或建立一个二进制文件，允许读和写。
        file_mode = MR_FILE_RDWR;
    }
    else if (strcmp(mode, "wt+") == 0)
    { //以读/写方式打开或建立一个文本文件，允许读写。
        file_mode = MR_FILE_RDWR;
    }
    else if (strcmp(mode, "at+") == 0)
    { //以读/写方式打开一个文本文件，允许读或在文本末追加数据。
        file_mode = MR_FILE_RDWR;
        seek_mode = MR_SEEK_END;
    }
    else if (strcmp(mode, "ab+") == 0)
    { //以读/写方式打开一个二进制文件，允许读或在文件末追加数据。
        file_mode = MR_FILE_RDWR;
        seek_mode = MR_SEEK_END;
    }
    filelen = mrc_getLen(path);
    f = open(path, file_mode);
    if (f != 0)
    {
        seek(f, 0, seek_mode);
        file = malloc(sizeof(FILE));
        file->_tmpfname = (char*)path;
        file->_file = f;
        file->len = filelen;
        return file;
    }

    return NULL;
}
unsigned int fread(void *buf, size_t size, size_t count, FILE *f)
{
    size_t total_read = 0;
    size_t n;
    char *ptr = (char *)buf;
    while (total_read < count) {
         n = read(f->_file, ptr + total_read, size * count - total_read);
        
        if (n == MR_FAILED) {
            return total_read / size;
        }
        if(n == 0){
            break;
        }
        total_read += n;
        // mrc_printf("read size = %d", total_read);
    }
    // int re = read(f->_file, buf, size * count);
    
    

    return total_read / size;
}

int fseek(FILE *f, long offset, int mode)
{
    return seek(f->_file, offset, mode);
}

long ftell(FILE *f)
{
    return f->len;
}

unsigned int fwrite(const void *buf, size_t size, size_t count, FILE *f)
{
    int32 re = write(f->_file, (void*)buf, size * count);
    if (re == -1)
        return -1;
    else
        return re / size;
}

int fflush(FILE *f)
{
    return MR_SUCCESS;
}
int fclose(FILE *f)
{
    int rec = close(f->_file);
    if(rec == MR_SUCCESS){
        free(f);
    }
    return rec;
}