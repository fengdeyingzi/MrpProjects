// #include "mpc.h"
#include "mrc_base.h"
#include "xl_coding.h"
// #include "encode.h"

// ex_coding����ʵ�� ���ܱ���ת������
char *ex_coding(char *text, int len, char *coding, char *tocoding)
{
  char *temp_un = NULL;
  int32 len_un = 0;
  char *retext = NULL;
  int32 relen = len;
  // �ж��ı��Ƿ�Ϊun���� ������� ��ת����un���루��ˣ�
  if (strcmp(coding, "UTF16-BE") == 0 || strcmp(coding, "utf16-be") == 0 || strcmp(coding, "UNICODE") == 0 || strcmp(coding, "unicode") == 0)
  {
    temp_un = text;
    len_un = len;
    relen = len * 2;
    mrc_printf("��ת������ unicode");
  }
  else
  {
    if (strcmp(coding, "GBK") == 0 || strcmp(coding, "gbk") == 0 || strcmp(coding, "GB2312") == 0 || strcmp(coding, "gb2312") == 0)
    {
      mrc_printf("��ת������ gbk");
      temp_un = gbToUn(text);
      len_un = mrc_wstrlen(temp_un);
      relen = len * 2;
    }
    else if (strcmp(coding, "UTF-8") == 0 || strcmp(coding, "utf-8") == 0)
    {
      mrc_printf("��ת������ utf");
      len_un = len * 2 + 2;
      temp_un = malloc(len_un);
      memset(temp_un, 0, len_un);
      relen = len * 2 + 2;
      UTF8ToUni(text, temp_un, len_un);
    }
    else if (strcmp(coding, "ANSI") == 0 || strcmp(coding, "ansi") == 0)
    {
      mrc_printf("��ת������ ansi");
      relen = len * 2 + 2;
      temp_un = gbToUn(text);
      len_un = mrc_wstrlen(temp_un);
    }
    else
    {
      mrc_printf("��ת������ ���ʧ��");
      return NULL;
    }
  }
  if (strcmp(tocoding, "unicode") == 0 || strcmp(tocoding, "UNICODE") == 0 || strcmp(tocoding, "UTF16-BE") == 0 || strcmp(tocoding, "utf16-be") == 0)
  {
    mrc_printf("ת��Ϊunicode");
    return temp_un;
  }
  else if (strcmp(tocoding, "UTF-8") == 0 || strcmp(tocoding, "utf-8") == 0)
  {
    mrc_printf("ת������ gbk");
    retext = malloc(relen);
    memset(retext, 0, relen);
    UniToUTF8(temp_un, retext, relen);
    return retext;
  }
  else if (strcmp(tocoding, "gbk") == 0 || strcmp(tocoding, "GBK") == 0 || strcmp(tocoding, "gb2312") == 0 || strcmp(tocoding, "GB2312") == 0)
  {
    mrc_printf("ת������ gbk");
    return unToGb(temp_un);
  }
  mrc_printf("��֧�ֵı����ʽ");
  return NULL;
}

// �ж�utf���룬0Ϊ�ɹ���-1ʧ��
int IsUTF8(void *pBuffer, long size)
{
  int IsUTF8 = 0;
  unsigned char *start = (unsigned char *)pBuffer;
  unsigned char *end = (unsigned char *)pBuffer + size;
  while (start < end)
  {
    if (*start < 0x80) // (10000000): ֵС��0x80��ΪASCII�ַ�
    {
      start++;
    }
    else if (*start <
             (0xC0)) // (11000000): ֵ����0x80��0xC0֮���Ϊ��ЧUTF-8�ַ�
    {
      IsUTF8 = -1;
      break;
    }
    else if (*start < (0xE0)) // (11100000): �˷�Χ��Ϊ2�ֽ�UTF-8�ַ�
    {
      if (start >= end - 1)
        break;
      if ((start[1] & (0xC0)) != 0x80)
      {
        IsUTF8 = -1;
        break;
      }
      start += 2;
    }
    else if (*start < (0xF0)) // (11110000): �˷�Χ��Ϊ3�ֽ�UTF-8�ַ�
    {
      if (start >= end - 2)
        break;
      if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
      {
        IsUTF8 = -1;
        break;
      }
      start += 3;
    }
    else
    {
      IsUTF8 = -1;
      break;
    }
  }
  return IsUTF8;
}

// UTF8ת��ΪUnicode
// ������UTF8�ַ�����Unicode����������������С
// ���أ�������ʹ�ô�С
int UTF8ToUni(char *utf8str, char *unistr, int size)
{
  int32 i = 0, u = 0;
  uint8 *utf8, *unicode;

  utf8 = (unsigned char *)utf8str;
  unicode = (unsigned char *)unistr;

  while ((utf8[i]) && u < size - 2)
  {
    /*
    //|utf8[i+1]
    if(!utf8[i])
    {
      i++;
      if (!utf8[i])
      {
        break;
      }
    }
    */
    if ((utf8[i] & 0x80) == 0)
    {
      // one byte 0...
      unicode[u] = 0;
      unicode[u + 1] = utf8[i++];
    }
    else if ((utf8[i] & 0x20) == 0)
    {
      // two bytes 110... 10...
      unicode[u] = (unsigned char)((utf8[i] & 0x1f) >> 2);
      unicode[u + 1] = (unsigned char)((utf8[i] << 6) & 0xff) | (utf8[i + 1] & 0x3f);
      i += 2;
    }
    else
    {
      // three bytes 1110... 10... 10...
      unicode[u] = (unsigned char)((utf8[i] << 4) & 0xff) | ((utf8[i + 1] & 0x3f) >> 2);
      unicode[u + 1] = (unsigned char)((utf8[i + 1] << 6) & 0xff) | (utf8[i + 2] & 0x3f);
      i += 3;
    }

    u += 2;
  }

  unicode[u] = '\0';
  unicode[u + 1] = '\0';

  return u;
}

// gb����תunicode ���ص��ַ�����Ҫ�ͷ��ڴ�
char *gbToUn(char *text)
{
  int32 len, err;
  return (char *)mrc_c2u(text, &err, &len);
}

// unicode����תgb ���ص��ַ�����Ҫ�ͷ��ڴ�
char *unToGb(char *text)
{
  char *input = text;
  int output_len;
  int input_len = mrc_wstrlen(input) + 2;
  char *output = mrc_malloc(input_len + 2);
  mrc_memset(output, 0, input_len + 2);
  mrc_printf("input_len %d\n", input_len);

  mrc_unicodeToGb2312((uint8 *)input, (int32)input_len, (uint8 **)&output, (int32 *)&output_len);
  //  output = UCS2BEStrToGBStr(input,&output_len);
  return output;
}

// Unicodeת��ΪUTF8
// ������Unicode�ַ�����UTF8����������������С
// ���أ�������ʹ�ô�С
int UniToUTF8(char *unistr, char *utf8str, int size)
{
  int u = 0, i = 0;
  unsigned char *unicode, *utf8;

  unicode = (unsigned char *)unistr;
  utf8 = (unsigned char *)utf8str;

  while ((*(unicode + u) || *(unicode + u + 1)) && i < size - 1)
  {
    if (*(unicode + u) == 0 && *(unicode + u + 1) < 128)
    {
      // 0 - 7 bits
      utf8[i++] = *(unicode + u + 1);
    }
    else if ((*(unicode + u) & 0xf8) == 0)
    {
      // 8 - 11 bits
      utf8[i++] = 0xc0 | ((unicode[u] << 2) & 0xff) | (unicode[u + 1] >> 6);
      utf8[i++] = 0x80 | (unicode[u + 1] & 0x3f);
    }
    else
    {
      // 12 - 16 bits
      utf8[i++] = 0xe0 | (unicode[u] >> 4);
      utf8[i++] = 0x80 | ((unicode[u] << 2) & 0x3f) | (unicode[u + 1] >> 6);
      utf8[i++] = 0x80 | (unicode[u + 1] & 0x3f);
    }

    u += 2;
  }

  utf8[i] = '\0';

  return i;
}

// ��gbתutf����Ҫfree
char *GbToUtf(char *text)
{
  int32 len, size;
  char *un_text;
  char *utf_text;
  len = mrc_strlen(text) * 2 + 2;

  // �����ڴ�
  utf_text = mrc_malloc(len);

  // תun��תutf8
  un_text = (char *)mrc_c2u(text, NULL, &size);
  UniToUTF8(un_text, utf_text, len);

  // �ͷŲ�����
  mrc_free(un_text);
  return utf_text;
}
