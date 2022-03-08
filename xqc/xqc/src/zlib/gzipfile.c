/*
 * 压缩为gz文件
 * 版权所有 (C) 2011-2012 eleqian
 * [5/2/2012]
 */

#include <mrc_base.h>

#include "zlib.h"
#include "gzipfile.h"

//释放动态申请内存
#ifndef TRYFREE
# define TRYFREE(p) \
    if (NULL != p) \
    { \
        free(p); \
        p = NULL; \
    }
#endif

//关闭文件
#ifndef TRYCLOSE
# define TRYCLOSE(fd) \
    if (0 != fd) \
    { \
        mrc_close(fd); \
        fd = 0; \
    }
#endif

typedef struct
{
    int32 ifd;  // 输入文件句柄
    int32 ofd;  // 输出文件句柄

    uint32 bufSize;  // 数据缓冲区大小
    uint8 *inBuf;    // 输入缓冲区
    uint8 *outBuf;   // 输出缓冲区

    z_stream strm;   // zlib结构体

    int32 timer;     // 运行定时器
    GzCb cb;         // 进度回调
}GZ_PROCESS_ST;

static GZ_PROCESS_ST GzProcess; // 压缩解压相关信息
static int32 InitState = 0;     // 初始化状态

/************************************************************************/
/* 初始化                                                               */
/************************************************************************/

//初始化压缩函数的一些数据
int32 Gzip_Init(void)
{
    extern void mrcInit_Deflate(void); 
    extern void mrcInit_Tree(void);

    mrcInit_Deflate();
    mrcInit_Tree();
    InitState = 1;

    return 0;
}

/************************************************************************/
/* 压缩为gzip文件                                                       */
/************************************************************************/

//进行实际压缩
static int32 GzipDo(void)
{
    int32 ret = -1;
    int32 flush;
    int32 size_r, size_w;

    size_r = mrc_read(GzProcess.ifd, GzProcess.inBuf, GzProcess.bufSize);
    if (size_r > 0) //有更多数据
    {
        GzProcess.strm.avail_in = (uint32)size_r;
        GzProcess.strm.next_in = GzProcess.inBuf;
        flush = (GzProcess.strm.avail_in < GzProcess.bufSize) ? Z_FINISH : Z_NO_FLUSH;

        do { //循环直到压缩完一个输入缓冲区
            GzProcess.strm.avail_out = GzProcess.bufSize;
            GzProcess.strm.next_out = GzProcess.outBuf;
            ret = deflate(&GzProcess.strm, flush);
            size_w = mrc_write(GzProcess.ofd, GzProcess.outBuf, GzProcess.bufSize - GzProcess.strm.avail_out);
            if (size_w != GzProcess.bufSize - GzProcess.strm.avail_out)
                return -1;
        } while (0 == GzProcess.strm.avail_out);
    }
    else if (size_r == 0)
    {
        ret = Z_STREAM_END;
    }

    return ret;
}

//进行间断式压缩
static void GzipTimerCb(int32 data)
{
    int32 ret, usedtime;

    usedtime = mrc_getUptime();

    do {
        ret = GzipDo();

        if (ret == Z_OK)
        {
            (*GzProcess.cb)(GzProcess.strm.total_in, GzProcess.strm.total_out, GZ_RESULT_PROCESSING);
        }
        else if (ret == Z_STREAM_END)
        {
            (*GzProcess.cb)(GzProcess.strm.total_in, GzProcess.strm.total_out, GZ_RESULT_END_OK);
            GzipFileStop();
            return;
        }
        else
        {
            (*GzProcess.cb)(GzProcess.strm.total_in, GzProcess.strm.total_out, GZ_RESULT_END_ERROR);
            GzipFileStop();
            return;
        }
    } while (mrc_getUptime() - usedtime < 1000); //每压缩1s暂停50ms

    mrc_timerStart(GzProcess.timer, 50, 0, GzipTimerCb, 0);
}

//参数：目标文件，原始文件，压缩率，进度回调
//返回：压缩后大小，-1为失败
int32 GzipFile(const char *destfile, const char *srcfile, int32 level, GzCb pscb)
{
    int32 ret;

    if (!InitState)
        Gzip_Init();

    //初始化变量
    memset(&GzProcess, 0, sizeof(GzProcess));

    GzProcess.strm.zalloc = (alloc_func)0;
    GzProcess.strm.zfree = (free_func)0;
    GzProcess.strm.opaque = (voidpf)0;

    GzProcess.cb = pscb;
    GzProcess.bufSize = 32 * 1024;

    //打开文件
    GzProcess.ifd = mrc_open(srcfile, MR_FILE_RDONLY);
    GzProcess.ofd = mrc_open(destfile, MR_FILE_RDWR | MR_FILE_CREATE);
    if (0 == GzProcess.ifd || 0 == GzProcess.ofd)
    {
        TRYCLOSE(GzProcess.ifd);
        TRYCLOSE(GzProcess.ofd);

        return GZ_RESULT_END_ERROR;
    }

    //初始化压缩例程
    ret = deflateInit2(&GzProcess.strm, level, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY);
    if (Z_OK == ret)
    {
        GzProcess.inBuf = (uint8 *)malloc(GzProcess.bufSize);
        GzProcess.outBuf = (uint8 *)malloc(GzProcess.bufSize);

        GzProcess.timer = mrc_timerCreate();
        mrc_timerStart(GzProcess.timer, 10, 0, GzipTimerCb, 1);

        return GZ_RESULT_PROCESSING;
    }
    else
    {
        TRYCLOSE(GzProcess.ifd);
        TRYCLOSE(GzProcess.ofd);

        return GZ_RESULT_END_ERROR;
    }
}

//停止Gzip压缩
int32 GzipFileStop(void)
{
    int32 ret;

    if (!InitState)
        Gzip_Init();

    TRYFREE(GzProcess.inBuf);
    TRYFREE(GzProcess.outBuf);

    TRYCLOSE(GzProcess.ifd);
    TRYCLOSE(GzProcess.ofd);

    if (0 != GzProcess.timer)
    {
        mrc_timerStop(GzProcess.timer);
        mrc_timerDelete(GzProcess.timer);
        GzProcess.timer = 0;
    }

    ret = deflateEnd(&GzProcess.strm);

    return ret;
}
