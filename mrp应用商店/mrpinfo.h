#ifndef __MRPINFO_H__
#define __MRPINFO_H__
#include <mrc_base.h>

#define MAX_PATH_LENGTH 16
#define MAX_MAGIC_LENGTH 4
#define MAX_FILE_NAME_LENGTH 12
#define MAX_DISPLAY_NAME_LENGTH 24
#define MAX_AUTH_STR_LENGTH 16
#define MAX_VENDOR_LENGTH 40
#define MAX_DESC_LENGTH 64
#define MAX_RESERVE3_LENGTH 31 // 240 - 209

typedef struct {
    char magic[MAX_MAGIC_LENGTH]; // 固定标识'MRPG'
    int32 fileStart; // 文件头的长度+文件列表的长度-8
    int32 mrpTotalLen; // mrp文件的总长度
    int32 mrpHeaderSize; // 文件头的长度
    char fileName[MAX_FILE_NAME_LENGTH]; // GB2312编码带'\0'
    char displayName[MAX_DISPLAY_NAME_LENGTH]; // GB2312编码带'\0'
    char authStr[MAX_AUTH_STR_LENGTH]; // 授权字符串
    int32 appid; // app ID
    int32 version; // 版本
    int32 flag; // 显示标志及其他
    int32 builderVersion; // 编译器版本
    int32 crc32; // CRC值
    char vendor[MAX_VENDOR_LENGTH]; // 供应商
    char desc[MAX_DESC_LENGTH]; // 描述
    int32 appidBE; // 大端appid
    int32 versionBE; // 大端version
    int32 reserve2; // 保留字段
    int16 screenWidth; // 屏幕宽度
    int16 screenHeight; // 屏幕高度
    char plat; // 平台
    char reserve3[MAX_RESERVE3_LENGTH]; // 预留字段
} Config;

int32 readMrpInfo(char* path, Config* config);

#endif
