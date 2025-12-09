#ifndef _WALLPAPER_H_
#define _WALLPAPER_H_

#include "mpc.h"
#include "mrc_graphics.h"
#include "fopen.h"

/* DFA文件头信息 (16字节) */
typedef struct {
    char magic[4];        /* "DFA\0" */
    uint16 version;       /* 版本 (1) */
    uint16 width;         /* 图像宽度 */
    uint16 height;        /* 图像高度 */
    uint16 totalFrames;   /* 总帧数 */
    uint16 keyframeInterval; /* 关键帧间隔 */
    uint8 compressionMode; /* 压缩模式 */
    uint8 reserved;       /* 保留 */
} DFA_HEADER;

/* DFA帧头信息 (6字节) */
typedef struct {
    uint8 frameType;      /* 帧类型: 0=关键帧, 1=差分帧, 2=跳过帧, 16=矩形区域差分 */
    uint8 reserved;       /* 保留 */
    uint32 dataSize;      /* 帧数据大小 */
} DFA_FRAME_HEADER;

/* 直接差分块头 (6字节) */
typedef struct {
    uint32 startIdx;      /* 起始像素索引 */
    uint16 length;        /* 连续像素数量 */
} DFA_DIFF_BLOCK_HEADER;

/* RLE块 (6字节) */
typedef struct {
    uint16 pixelIdx;      /* 起始像素索引 */
    uint16 repeatCount;   /* 重复次数 */
    uint16 color;         /* RGB565颜色值 */
} DFA_RLE_BLOCK;

/* 矩形区域头 (8字节) */
typedef struct {
    uint16 x;            /* 矩形左上角X坐标 */
    uint16 y;            /* 矩形左上角Y坐标 */
    uint16 width;        /* 矩形宽度 */
    uint16 height;       /* 矩形高度 */
} DFA_RECT_HEADER;

/* 壁纸信息 */
typedef struct {
    char name[32];            /* 壁纸名称 */
    char dfaName[32];         /* dfa文件名（不含扩展名） */
    int isExtracted;          /* 是否已解压 */
} WALLPAPER_INFO;

/* 壁纸列表 */
typedef struct {
    WALLPAPER_INFO *wallpapers; /* 壁纸数组 */
    int count;                  /* 壁纸数量 */
    int currentIndex;           /* 当前壁纸索引 */
} WALLPAPER_LIST;

/* DFA帧信息 */
typedef struct {
    uint32 offset;             /* 帧数据在文件中的偏移 */
    uint32 dataSize;           /* 帧数据大小 */
    uint8 frameType;           /* 帧类型 */
} DFA_FRAME_INFO;

/* 动态壁纸播放器 */
typedef struct {
    FILE *file;                /* dfa文件指针 */
    DFA_HEADER header;         /* DFA文件头 */
    DFA_FRAME_INFO *frameInfos; /* 帧信息表 */
    uint16 *frameBuffer;       /* 当前帧缓冲区 (RGB565格式)，也用作前一帧数据 */
    BITMAP_565 *currentBitmap; /* 当前帧bitmap */
    uint32 currentFrame;       /* 当前帧索引 */
    int isPaused;              /* 是否暂停 */
    WALLPAPER_LIST list;       /* 壁纸列表 */
} WALLPAPER_PLAYER;

void drawCenterUI(char *text);

/* 初始化壁纸播放器 */
int wallpaper_init(WALLPAPER_PLAYER *player);

/* 加载dfa文件 */
int wallpaper_loadDfa(WALLPAPER_PLAYER *player, const char *filename);

/* 绘制当前帧 */
void wallpaper_drawFrame(WALLPAPER_PLAYER *player);

/* 下一帧 */
int wallpaper_nextFrame(WALLPAPER_PLAYER *player);

/* 加载第一帧（第0帧） */
int wallpaper_loadFirstFrame(WALLPAPER_PLAYER *player);

/* 暂停/继续 */
void wallpaper_togglePause(WALLPAPER_PLAYER *player);

/* 释放资源 */
void wallpaper_free(WALLPAPER_PLAYER *player);

/* 资源解压：合并dfa分片文件 */
int wallpaper_extractResources(void);

/* 读取plays.txt文件 */
int wallpaper_readPlaylist(WALLPAPER_LIST *list);

/* 释放壁纸列表 */
void wallpaper_freeList(WALLPAPER_LIST *list);

/* 解压指定壁纸 */
int wallpaper_extractWallpaper(const char *dfaName);

/* 切换壁纸 */
int wallpaper_switchWallpaper(WALLPAPER_PLAYER *player, int direction);

/* 获取当前壁纸名称 */
const char* wallpaper_getCurrentName(WALLPAPER_PLAYER *player);

/* DFA解码函数 */
int dfa_decodeKeyFrame(WALLPAPER_PLAYER *player, uint8 *data, uint32 dataSize);
int dfa_decodeDiffFrame(WALLPAPER_PLAYER *player, uint8 *data, uint32 dataSize);
int dfa_decodeSkipFrame(WALLPAPER_PLAYER *player);
int dfa_decodeRectFrame(WALLPAPER_PLAYER *player, uint8 *data, uint32 dataSize);

#endif
