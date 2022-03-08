#ifndef __GZIPFILE_H__
#define __GZIPFILE_H__

// 处理结果枚举
enum GZ_RESULT_ENUM
{
    GZ_RESULT_PROCESSING = 0, // 处理过程中，回调返回当前处理大小
    GZ_RESULT_END_OK,         // 正常结束
    GZ_RESULT_END_ERROR,      // 异常结束

    GZ_RESULT_MAX
};

// 压缩/解压进度回调
// 参数：已处理原文件大小，输出大小，处理结果
typedef void (*GzCb)(int32 TotalIn, int32 TotalOut, int32 Result);

// 压缩文件
// 参数：输出文件，输入文件，压缩级别，进度回调
// 返回：GZ_RESULT_PROCESSING - 处理中，以后通过回调通知结果
//       GZ_RESULT_END_ERROR - 失败，不会再回调
int32 GzipFile(const char *destfile, const char *srcfile, int32 level, GzCb pscb);

// 停止压缩文件
// 一般不需手动调用，调用时立即结束压缩
int32 GzipFileStop(void);

#endif // __GZIPFILE_H__
