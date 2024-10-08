
#include <mrc_base.h>
#include "fopen.h"
#include "mpc.h"

#define O_RDONLY MR_FILE_RDONLY
#define O_WRONLY MR_FILE_WRONLY
#define O_CREAT MR_FILE_CREATE

FILE *fopen(const char *path, const char *mode)
{
    int file_mode = 0;
    int seek_mode = MR_SEEK_SET;
    int32 f = 0;
    FILE *file = NULL;
    if (strcmp(mode, "r") == 0 || strcmp(mode, "rb") == 0)
    { //��ֻ����ʽ���ļ������ļ�������ڡ�
        file_mode = O_RDONLY;
    }
    else if (strcmp(mode, "r+") == 0)
    { //�Զ�/д��ʽ���ļ������ļ�������ڡ�
        file_mode = O_RDONLY | O_WRONLY;
    }
    else if (strcmp(mode, "rb+") == 0)
    { //�Զ�/д��ʽ��һ���������ļ���ֻ�����/д���ݡ�
        file_mode = O_RDONLY | O_WRONLY;
    }
    else if (strcmp(mode, "rt+") == 0)
    { //�Զ�/д��ʽ��һ���ı��ļ����������д��
        file_mode = O_RDONLY | O_WRONLY;
    }
    else if (strcmp(mode, "w") == 0)
    { //��ֻд�ļ������ļ������򳤶���Ϊ0�������ļ�������ʧ�����������򴴽����ļ���
        file_mode = O_WRONLY | O_CREAT;
        remove(path);
    }
    else if (strcmp(mode, "w+") == 0)
    { //�򿪿ɶ�/д�ļ������ļ��������ļ�������Ϊ�㣬�����ļ����ݻ���ʧ�����ļ��������������ļ���
        file_mode = O_RDONLY | O_WRONLY | O_CREAT;
        remove(path);
    }
    else if (strcmp(mode, "a") == 0)
    { //�Ը��ӵķ�ʽ��ֻд�ļ������ļ������ڣ���Ὠ�����ļ�������ļ����ڣ�д������ݻᱻ�ӵ��ļ�β�����ļ�ԭ�ȵ����ݻᱻ������EOF������)��
        file_mode = O_WRONLY | O_CREAT;
        seek_mode = MR_SEEK_END;
    }
    else if (strcmp(mode, "a+") == 0)
    { //�Ը��ӷ�ʽ�򿪿ɶ�/д���ļ������ļ������ڣ���Ὠ�����ļ�������ļ����ڣ���д������ݻᱻ�ӵ��ļ�β�󣬼��ļ�ԭ�ȵ����ݻᱻ������ԭ����EOF�� ������)��
        file_mode = O_RDONLY | O_WRONLY;
        seek_mode = MR_SEEK_END;
    }
    else if (strcmp(mode, "wb") == 0)
    { //��ֻд��ʽ�򿪻��½�һ���������ļ���ֻ����д���ݡ�
        file_mode = O_WRONLY;
    }
    else if (strcmp(mode, "wb+") == 0)
    { //�Զ�/д��ʽ�򿪻���һ���������ļ����������д��
        file_mode = O_RDONLY | O_WRONLY;
    }
    else if (strcmp(mode, "wt+") == 0)
    { //�Զ�/д��ʽ�򿪻���һ���ı��ļ��������д��
        file_mode = O_RDONLY | O_WRONLY;
    }
    else if (strcmp(mode, "at+") == 0)
    { //�Զ�/д��ʽ��һ���ı��ļ�������������ı�ĩ׷�����ݡ�
        file_mode = O_RDONLY | O_WRONLY;
        seek_mode = MR_SEEK_END;
    }
    else if (strcmp(mode, "ab+") == 0)
    { //�Զ�/д��ʽ��һ���������ļ�������������ļ�ĩ׷�����ݡ�
        file_mode = O_RDONLY | O_WRONLY;
        seek_mode = MR_SEEK_END;
    }
    f = open(path, file_mode);
    if (f != 0)
    {
        seek(f, 0, seek_mode);
        file = malloc(sizeof(FILE));
        file->_tmpfname = (char*)path;
        file->_file = f;
        return file;
    }

    return NULL;
}
unsigned int fread(void *buf, size_t size, size_t count, FILE *f)
{
    int re = read(f->_file, buf, size * count);
    if (re == -1)
        return -1;
    else
    {
        int count = re % size;
        seek(f->_file, -count, SEEK_CUR);
        return re / size;
    }
}

int fseek(FILE *f, long offset, int mode)
{
    return seek(f->_file, offset, mode);
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