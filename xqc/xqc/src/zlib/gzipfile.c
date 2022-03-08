/*
 * ѹ��Ϊgz�ļ�
 * ��Ȩ���� (C) 2011-2012 eleqian
 * [5/2/2012]
 */

#include <mrc_base.h>

#include "zlib.h"
#include "gzipfile.h"

//�ͷŶ�̬�����ڴ�
#ifndef TRYFREE
# define TRYFREE(p) \
    if (NULL != p) \
    { \
        free(p); \
        p = NULL; \
    }
#endif

//�ر��ļ�
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
    int32 ifd;  // �����ļ����
    int32 ofd;  // ����ļ����

    uint32 bufSize;  // ���ݻ�������С
    uint8 *inBuf;    // ���뻺����
    uint8 *outBuf;   // ���������

    z_stream strm;   // zlib�ṹ��

    int32 timer;     // ���ж�ʱ��
    GzCb cb;         // ���Ȼص�
}GZ_PROCESS_ST;

static GZ_PROCESS_ST GzProcess; // ѹ����ѹ�����Ϣ
static int32 InitState = 0;     // ��ʼ��״̬

/************************************************************************/
/* ��ʼ��                                                               */
/************************************************************************/

//��ʼ��ѹ��������һЩ����
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
/* ѹ��Ϊgzip�ļ�                                                       */
/************************************************************************/

//����ʵ��ѹ��
static int32 GzipDo(void)
{
    int32 ret = -1;
    int32 flush;
    int32 size_r, size_w;

    size_r = mrc_read(GzProcess.ifd, GzProcess.inBuf, GzProcess.bufSize);
    if (size_r > 0) //�и�������
    {
        GzProcess.strm.avail_in = (uint32)size_r;
        GzProcess.strm.next_in = GzProcess.inBuf;
        flush = (GzProcess.strm.avail_in < GzProcess.bufSize) ? Z_FINISH : Z_NO_FLUSH;

        do { //ѭ��ֱ��ѹ����һ�����뻺����
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

//���м��ʽѹ��
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
    } while (mrc_getUptime() - usedtime < 1000); //ÿѹ��1s��ͣ50ms

    mrc_timerStart(GzProcess.timer, 50, 0, GzipTimerCb, 0);
}

//������Ŀ���ļ���ԭʼ�ļ���ѹ���ʣ����Ȼص�
//���أ�ѹ�����С��-1Ϊʧ��
int32 GzipFile(const char *destfile, const char *srcfile, int32 level, GzCb pscb)
{
    int32 ret;

    if (!InitState)
        Gzip_Init();

    //��ʼ������
    memset(&GzProcess, 0, sizeof(GzProcess));

    GzProcess.strm.zalloc = (alloc_func)0;
    GzProcess.strm.zfree = (free_func)0;
    GzProcess.strm.opaque = (voidpf)0;

    GzProcess.cb = pscb;
    GzProcess.bufSize = 32 * 1024;

    //���ļ�
    GzProcess.ifd = mrc_open(srcfile, MR_FILE_RDONLY);
    GzProcess.ofd = mrc_open(destfile, MR_FILE_RDWR | MR_FILE_CREATE);
    if (0 == GzProcess.ifd || 0 == GzProcess.ofd)
    {
        TRYCLOSE(GzProcess.ifd);
        TRYCLOSE(GzProcess.ofd);

        return GZ_RESULT_END_ERROR;
    }

    //��ʼ��ѹ������
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

//ֹͣGzipѹ��
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
