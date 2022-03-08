#ifndef __GZIPFILE_H__
#define __GZIPFILE_H__

// ������ö��
enum GZ_RESULT_ENUM
{
    GZ_RESULT_PROCESSING = 0, // ��������У��ص����ص�ǰ�����С
    GZ_RESULT_END_OK,         // ��������
    GZ_RESULT_END_ERROR,      // �쳣����

    GZ_RESULT_MAX
};

// ѹ��/��ѹ���Ȼص�
// �������Ѵ���ԭ�ļ���С�������С��������
typedef void (*GzCb)(int32 TotalIn, int32 TotalOut, int32 Result);

// ѹ���ļ�
// ����������ļ��������ļ���ѹ�����𣬽��Ȼص�
// ���أ�GZ_RESULT_PROCESSING - �����У��Ժ�ͨ���ص�֪ͨ���
//       GZ_RESULT_END_ERROR - ʧ�ܣ������ٻص�
int32 GzipFile(const char *destfile, const char *srcfile, int32 level, GzCb pscb);

// ֹͣѹ���ļ�
// һ�㲻���ֶ����ã�����ʱ��������ѹ��
int32 GzipFileStop(void);

#endif // __GZIPFILE_H__
