#ifndef MRC_FILE_H
#define MRC_FILE_H

#include "mrc_base.h"

//倒找字符
extern char *strrchr(char *s, int c);

//读取文件数据到指针
//自动申请内存，多申请一字节
extern int mrc_readFileMalloc(int32 file, int32 pos, int32 len, char **data);

//读取文件数据到指针
//不用申请内存
extern int mrc_readFileNot(int32 file, int32 pos, int32 len, void *data);

//写入int8到文件
extern int mrc_writeDataFile(int32 file, int32 pos, char *data, int32 len);

//读取文件int8并返回
//不用申请内存
extern int8 mrc_readFileInt8(int32 file, int32 pos);

//写入int8到文件
extern int mrc_writeInt8File(int32 file, int32 pos, int8 data);

//读取文件int16并返回
//不用申请内存
extern int16 mrc_readFileInt16(int32 file, int32 pos);

//写入int16到文件
extern int mrc_writeInt16File(int32 file, int32 pos, int16 data);

//读取文件int32并返回
//不用申请内存
extern int32 mrc_readFileInt32(int32 file, int32 pos);

//写入int32到文件
extern int mrc_writeInt32File(int32 file, int32 pos, int32 data);

#endif

