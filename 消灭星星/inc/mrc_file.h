#ifndef MRC_FILE_H
#define MRC_FILE_H

#include "mrc_base.h"

//�����ַ�
extern char *strrchr(char *s, int c);

//��ȡ�ļ����ݵ�ָ��
//�Զ������ڴ棬������һ�ֽ�
extern int mrc_readFileMalloc(int32 file, int32 pos, int32 len, char **data);

//��ȡ�ļ����ݵ�ָ��
//���������ڴ�
extern int mrc_readFileNot(int32 file, int32 pos, int32 len, void *data);

//д��int8���ļ�
extern int mrc_writeDataFile(int32 file, int32 pos, char *data, int32 len);

//��ȡ�ļ�int8������
//���������ڴ�
extern int8 mrc_readFileInt8(int32 file, int32 pos);

//д��int8���ļ�
extern int mrc_writeInt8File(int32 file, int32 pos, int8 data);

//��ȡ�ļ�int16������
//���������ڴ�
extern int16 mrc_readFileInt16(int32 file, int32 pos);

//д��int16���ļ�
extern int mrc_writeInt16File(int32 file, int32 pos, int16 data);

//��ȡ�ļ�int32������
//���������ڴ�
extern int32 mrc_readFileInt32(int32 file, int32 pos);

//д��int32���ļ�
extern int mrc_writeInt32File(int32 file, int32 pos, int32 data);

#endif

