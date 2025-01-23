#include "mrc_file.h"
#include <mrc_base.h>

char *strrchr(char *s, int c)
{
  char *found = NULL, *p;
  c = (unsigned char)c;
  if (c == '\0')
    return (char*)mrc_strchr((const char*)s, '\0');
  while ((p = (char*)mrc_strchr((const char*)s, c)) != NULL)
  {
    found = p;
    s = p + 1;
  }
  return (char *)found;
}

//��ȡ�ļ����ݵ�ָ��
//�Զ������ڴ棬������һ�ֽ�
int mrc_readFileMalloc(int32 file, int32 pos, int32 len, char **data)
{
  *data = mrc_malloc(sizeof(char) * (len + 1));
  mrc_memset(*data, '\0', sizeof(char) * (len + 1));
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_read(file, *data, len);
  return 0;
}

//��ȡ�ļ����ݵ�ָ��
//���������ڴ�
int mrc_readFileNot(int32 file, int32 pos, int32 len, void *data)
{
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_read(file, data, len);
  return 0;
}

//д��int8���ļ�
int mrc_writeDataFile(int32 file, int32 pos, char *data, int32 len)
{
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_write(file, data, len);
  return 0;
}

//��ȡ�ļ�int8������
//���������ڴ�
int8 mrc_readFileInt8(int32 file, int32 pos)
{
  int8 data;
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_read(file, &data, 1);
  return data;
}

//д��int8���ļ�
int mrc_writeInt8File(int32 file, int32 pos, int8 data)
{
  int8 wdata = data;
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_write(file, &wdata, 1);
  return 0;
}

//��ȡ�ļ�int16������
//���������ڴ�
int16 mrc_readFileInt16(int32 file, int32 pos)
{
  int16 data;
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_read(file, &data, 2);
  return data << 8 | data >> 8;
}

//д��int16���ļ�
int mrc_writeInt16File(int32 file, int32 pos, int16 data)
{
  int8 wdata = data >> 8 & 0xff;
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_write(file, &wdata, 1);

  wdata = data & 0xff;
  mrc_seek(file, pos + 1, MR_SEEK_SET);
  mrc_write(file, &wdata, 1);
  return 0;
}

//��ȡ�ļ�int32������
//���������ڴ�
int32 mrc_readFileInt32(int32 file, int32 pos)
{
  int32 data;
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_read(file, &data, 4);
  return (data << 24 & 0xff000000) | (data << 8 & 0xff0000) | (data >> 8 & 0xff00) | (data >> 24 & 0xff);
}

//д��int32���ļ�
int mrc_writeInt32File(int32 file, int32 pos, int32 data)
{
  int8 wdata = data >> 24 & 0xff;
  mrc_seek(file, pos, MR_SEEK_SET);
  mrc_write(file, &wdata, 1);

  wdata = data >> 16 & 0xff;
  mrc_seek(file, pos + 1, MR_SEEK_SET);
  mrc_write(file, &wdata, 1);

  wdata = data >> 8 & 0xff;
  mrc_seek(file, pos + 2, MR_SEEK_SET);
  mrc_write(file, &wdata, 1);

  wdata = data & 0xff;
  mrc_seek(file, pos + 3, MR_SEEK_SET);
  mrc_write(file, &wdata, 1);
  return 0;
}

