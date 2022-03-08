#ifndef _BASE_H
#define _BASE_H

/*
���ļ�ֻ�൱�ڰ����ļ���û���κ�ʵ�����壬Ҳ����˵���������ӻ��޸Ķ��岻�����κ�Ч��
�������Ҫ���Ӷ��壬�붨���������ļ���,Ҫʹ���������нӿ�������Ĵ��������ӡ�#include "base.h"��

���ļ��д󲿷ֶ�����ݱ�׼MRPAPI�������������mrc_drawTextEx����Ϊdtextex��
�������һ��Ϊȥ��"mrc_"���ѳ��õ��ʽ�����д���Ѵ�д��ΪСд
����API��Ϊ��������ԭ��������޸ģ�����mrc_drawTextExԭ�����Խṹ��ʽ���ݲ����ģ�����dtextex��ȴ�޸�Ϊ��ָ�뷽ʽ���ݲ���

�벻Ҫ�����ļ�����Ϊ�����ļ������������Ĵ����У�������ܻᵼ��ϵͳ�쳣������
*/



//ϵͳ�汾����������ĸ��£����ܻ������Щ�������ꡢ���͵Ȳ�һ�¶��������г���
//������Դ�����ж�ϵͳ�汾�Ƿ���ͬ
#define _VERSION 1001

/*
���ļ�Ϊϵͳ�ڲ��򵥷�װ��MRP����ʵ����MRPAPI��<mrc_base.h>�Ĳ��ֹ���
*/
typedef  unsigned short     uint16;      //�з���16bit����
typedef  unsigned long int  uint32;      //�޷���32bit����
typedef  long int           int32;       //�з���32bit����
typedef  unsigned char      uint8;       //�޷���8bit����
typedef  signed char        int8;        //�з���8bit����
typedef  signed short       int16;       //�з���16bit����

//��������Ϊ�󲿷�ϵͳ�����ķ���ֵ��û����ϵͳ�ڶ��壬�����Ҫ��Щ�궨����������ͷ�ļ��ж���
#define MR_SUCCESS  0    //�ɹ�
#define MR_FAILED   -1    //ʧ��
#define MR_IGNORE   1     //������
#define MR_WAITING  2     //�첽(������)ģʽ

#define NULL (void*)0
#define TRUE 1
#define FALSE 0
#define SCRW  //ϵͳ�ڲ��ֵ꣬Ϊ��Ļ��
#define SCRH  //ϵͳ�ڲ��ֵ꣬Ϊ��Ļ��


//��������ֵ��δ�������������Ҳ����ʹ�ã�����֪�����ֵ��
enum {  
   _0,           //���� 0
   _1,           //���� 1
   _2,           //���� 2
   _3,           //���� 3
   _4,           //���� 4
   _5,           //���� 5
   _6,           //���� 6
   _7,           //���� 7
   _8,           //���� 8
   _9,           //���� 9
   _STAR,        //���� *
   _POUND,       //���� #
   _UP,          //���� ��
   _DOWN,        //���� ��
   _LEFT,        //���� ��
   _RIGHT,       //���� ��
   _SLEFT=17,    //���� �����
   _SRIGHT,      //���� �����
   _SEND,        //���� ������
   _SELECT       //���� ȷ��/ѡ����������м���ȷ�ϼ���������Ϊ�ü���
};

//�����¼��������¼����Լ����壩
enum {
    KY_DOWN, 	 //��������
    KY_UP,       //�����ͷ�
    MS_DOWN, 	 //��갴��
    MS_UP, 	     //����ͷ�
    MN_SLT, //�˵�ѡ��
    MN_RET, //�˵�����
    MR_DIALOG, //�Ի���
    MS_MOVE=12   //����ƶ�
};

enum {
    DLG_OK,         //�Ի���/�ı���ȵ�"ȷ��"�������(ѡ��)
    DLG_CANCEL  //�Ի���/�ı���ȵ�"ȡ��"("����")�������(ѡ��)
};

enum
{
    SEEK_SET,             //���ļ���ʼ��ʼ
    SEEK_CUR,             //�ӵ�ǰλ�ÿ�ʼ
    SEEK_END             //���ļ�ĩβ��ʼ
};
enum
{
    IS_FILE=1,      //�ļ�
    IS_DIR=2,      //Ŀ¼
    IS_INVALID=8,  //��Ч(���ļ�����Ŀ¼)
};

typedef struct {
    uint16            x;
    uint16            y;
    uint16            w;
    uint16            h;
}rectst;

typedef struct {
    uint8            r;
    uint8            g;
    uint8            b;
}colorst;


/********************************C�⺯��********************************/

/*�ú���������printf�������ƣ������Ǳ����������
��Ϣ����ӡ��print.txt�ļ���(��mrpapi�е�����)*/
void printf(const char *format,...);

//�����ڴ�ӿ�,ע��!�����ϸ����������ڴ�,�Է��ڴ�й©!ϵͳ����Ϊ����������ڴ��ʹ�����
void* malloc(int size);

//�ͷ���malloc������ڴ�
void free(void *address);

//��src��ָ�ڴ�������count���ֽڵ�dest��ָ�ڴ�����src��dest��ָ�ڴ��������ص�����������ָ��dest��ָ�롣
void *memcpy(void *dest, void *src, unsigned int count)

//��src��ָ�ڴ�������count���ֽڵ�dest��ָ�ڴ�����src��dest��ָ�ڴ���������ص��������ƺ�dest���ݻᱻ���ġ�
void *memmove(void *dest, const void *src, unsigned int count);

//��src��ָ��NULL�������ַ������Ƶ�dest��ָ�������С�src��dest��ָ�ڴ����򲻿����ص�����dest�������㹻�Ŀռ�������src���ַ�����
char *strcpy(char *dest,char *src);

//���ַ���src�����n���ַ����Ƶ��ַ�����dest�У���������strcpyһ������NULL�Ϳ�ʼ���ƣ����ǵȴչ�n���ַ��ſ�ʼ���ơ�
char *strncpy(char *dest, char *src, int32 n);

//��src��ָ�ַ�����ӵ�dest��β��(����dest��β����'\0')�����'\0'��src��dest��ָ�ڴ����򲻿����ص���dest�������㹻�Ŀռ�������src���ַ�����
char *strcat(char *dest,char *src);

//��src��ָ�ַ�����ǰn���ַ���ӵ�dest��β��(����dest��β����'\0')�����'\0'��src��dest��ָ�ڴ����򲻿����ص���dest�������㹻�Ŀռ�������src���ַ�����
char *strncat(char *dest,char *src,int n);

//�Ƚ��ڴ�����buf1��buf2��ǰcount���ֽڡ�
int memcmp(void *buf1, void *buf2, unsigned int count);

// �Ƚ��ַ���s1��s2��
// ����ֵ��
// ��s1<s2ʱ������ֵ<0
// ��s1=s2ʱ������ֵ=0
// ��s1>s2ʱ������ֵ>0
int strcmp(const char *s1,const char * s2);

//���Ƚϣ��Ƚ��ַ���str1��str2��ǰmaxlen���ַ���
int strncmp(char *str1, char *str2, int maxlen);

//��buf��ָ�ڴ������ǰcount���ֽڲ����ַ�ch������һ�������ַ�chʱֹͣ���ҡ�
//����ɹ�������ָ���ַ�ch��ָ�룻���򷵻�NULL��
void *memchr(void *buf, char ch, unsigned count);

// ��s��ָ���ĳһ���ڴ��е�ÿ���ֽڵ�����ȫ������Ϊchָ����ASCIIֵ�� ��Ĵ�С�ɵ���������ָ����
// �������ͨ��Ϊ��������ڴ�����ʼ������, �䷵��ֵΪָ��s��ָ�롣
void *memset(void *s, int ch, unsigned n);

// �����ַ���s�ĳ��ȡ�
uint32 strlen(char *s);

/*
��ĸ��str1�в���ָ���ַ���str2�ĵ�һ�γ���λ�ã����ȽϽ�����NULL��

����˵����
str1      [IN]          �����Ҵ����ڵ�ĸ��
str2      [IN]          �����Ҵ�
����ֵ��
NULL         ��ĸ��str1��δ���ҵ��Ӵ�str2��
��NULL       ĸ��str1�е�һ�γ����Ӵ�str2��λ��ָ��
*/
char *strstr(char *str1, char *str2);

// ���ִ���ʽ����(��mrpapi�е�����)
char *sprintf(char *str, char *format, ...);

//�������ַ���ת��Ϊ�������硰1234��ת��Ϊ1234��
int atoi(const char *nptr);

// ���ַ���ת�����޷��ų���������һ��ʼ��ɨ�����nptr�ַ���������ǰ��Ŀո��ַ�����ֱ���������ֻ��������Ųſ�ʼ��ת���������������ֻ��ַ�������ʱ('')����ת��������������ء�������endptr��ΪNULL����Ὣ����������������ֹ��nptr�е��ַ�ָ����endptr���ء�
uint32 strtoul(const char *nptr,char **endptr,int base);


/*
����һ���ַ�������һ���ַ��������һ�γ��ֵ�λ�ã������ش��ַ����е����λ���� һֱ���ַ��������������ַ���
����˵����
str     [IN]          ����ѯ�ַ���ĸ��
c       [IN]          ��ѯ�ַ�
����ֵ��
��NULL��   ����ĸ��src���״γ����ַ�c���Ӵ�ָ�룻
NULL��     ĸ��src�в������ַ�c��
*/
char *strrchr(char *str, char c);

// ����unicode�ַ���s�ĳ��ȣ�������Ϊ����\0\0����
int32 wstrlen(char *s);
/********************************C�⺯������****************/


/*******************************��ں���*********************/
/*
Ӧ�ó�ʼ������
�ú�����Ӧ�ó�ʼ���ڼ䱻mythroadƽ̨���ã����������
�����н���ȫ�ֱ����ĳ�ʼ���ȹ�����
����ֵ:
0  Ӧ�ó�ʼ���ɹ�
-1      Ӧ�ó�ʼ��ʧ��
*/
int init(void);

/*
Ӧ���¼�����
�ú�����Ӧ�������ڼ䣬ÿ��mythroadƽ̨�յ��¼�ʱ������
����:
code:
      code�Ŀ���ȡֵ����:
enum {
      KY_DOWN, 	 //��������
      KY_UP,       //�����ͷ�
      MS_DOWN, 	 //��갴��
      MS_UP, 	     //����ͷ�
      MS_MOVE=12   //����ƶ�
};

p0:
   ��codeΪKY_DOWN��KY_UPʱ��p0�Ŀ���ȡֵ����:
//��������ֵ��δ�������������Ҳ����ʹ�ã�����֪�����ֵ��
enum {  
   _0,           //���� 0
   _1,           //���� 1
   _2,           //���� 2
   _3,           //���� 3
   _4,           //���� 4
   _5,           //���� 5
   _6,           //���� 6
   _7,           //���� 7
   _8,           //���� 8
   _9,           //���� 9
   _STAR,        //���� *
   _POUND,       //���� #
   _UP,          //���� ��
   _DOWN,        //���� ��
   _LEFT,        //���� ��
   _RIGHT,       //���� ��
   _SLEFT=17,    //���� �����
   _SRIGHT,      //���� �����
   _SEND,        //���� ������
   _SELECT       //���� ȷ��/ѡ����������м���ȷ�ϼ���������Ϊ�ü���
   };

��codeΪ����¼�ʱ��p0Ϊ��Ļ��x���ꣻp1Ϊ��Ļ��y���ꣻ
�����¼�������ֵ��ο�MRPAPI
����ֵ:
0  �����ɹ�
-1 ����ʧ��
*/
int event(int code, int p0, int p1);

/*
Ӧ����ͣ����
�ú�����Ӧ�ñ���ͣʱ���е绰���¼���ͣӦ�ã�
��mythroadƽ̨���á�

����ֵ:
0  �����ɹ�
-1 ����ʧ��
*/
int pause(void);

/*
�ú�����Ӧ�ûָ�����ʱ��mythroadƽ̨���á�

����ֵ:
0  �����ɹ�
-1 ����ʧ��
*/
int resume(void);

////////////////////////////����Ϊƽ̨����ʵ�ֵ���ں���//////////////////////////////
/*
��mode��ʽ���ļ�������ļ������ڣ�����modeֵ
�ж��Ƿ񴴽�֮��

����:
filename	�ļ���
mode	�ļ��򿪷�ʽ
     modeȡֵ
1 //��ֻ���ķ�ʽ���ļ���
2  //��ֻд�ķ�ʽ���ļ����������ʽ���ܱ�ʵ��Ϊ��4��ͬ�Ĳ�����
4  //�Զ�д�ķ�ʽ���ļ���
8  //����ļ������ڣ�����
���ļ����ò������ᵥ������
��ֻ��������ֵһͬ���֣�ʹ
��"��"���㣩
mode���ܵ�ȡֵ��
a��	ǰ��������������һ��
b��	ǰ��������������һ����8��"��"ֵ
����:
�� 0	       �ļ����
0	           ʧ��
*/
int32 open(const char* filename,  uint32 mode);

/*
�ر��ļ���
����:
f	�ļ����
����:
0	�ɹ�
-1	ʧ��
*/
int32 close(int32 f);

/*
ȡ���ļ�������Ϣ��
����:
filename	�ļ���
����:
      IS_FILE     1//���ļ�
      IS_DIR      2//��Ŀ¼
      IS_INVALID  8//�ļ������ڣ���Ȳ����ļ�Ҳ����Ŀ¼
*/
int32 filestate(const char* filename);

/*
д�ļ�
����:
f	�ļ����
p	��д�����ݴ�ŵ�ַ
l	��д�����ݳ���
����:
      >0                   ȷ��д����ֽ���
      -1      ʧ��
*/
int32 write(int32 f,void *p,uint32 l);

/*
��ȡ�ļ������ݵ�ָ���Ļ��塣
����:
f	�ļ����
p	�ļ������ַ
l	���峤��
����:
      >=0                ȷ�ж�ȡ���ֽ���
      -1      ʧ��
*/
int32 read(int32 f,void *p,uint32 l);



/*
�����ļ�ָ�롣
����:
f	�ļ����
pos	�ļ�ָ��λ��
method	���ܵ�ȡֵΪ��
   SEEK_SET, 
   SEEK_CUR, 
   SEEK_END
����:
      0  �ɹ�
      -1   ʧ��
*/
int32 seek(int32 f, int32 pos, int method);

/*
ȡ���ļ����ȡ�
����:
filename	�ļ��� 
����:
      >=0   �ļ�����
      -1   ʧ��/�ļ�������
*/
int32 getlen(const char* filename);

/*
ɾ���ļ���
����:
filename	�ļ���
����:
      0  �ɹ�
      -1   ʧ��
*/
int32 remove(const char* filename);

/*
�ļ���������
����:
oldname	���ļ���
newname	���ļ���
����:
      0  �ɹ�
      -1   ʧ��
*/
int32 rename(const char* oldname, const char* newname);

/*
����Ŀ¼��
����:
name	Ŀ¼��
����:
      0  �ɹ�
      -1   ʧ��
*/
int32 mkdir(const char* name);

/*
ɾ��Ŀ¼(����Ϊ��Ŀ¼)
����:
name	Ŀ¼��
����:
      0  �ɹ�
      -1   ʧ��
*/
int32 rmdir(const char* name);

/*
׼������nameָ����Ŀ¼����dsm���øú�����ϵͳ
��ʼ��һ��Ŀ¼�����������ص�һ��Ŀ¼�����Ľ��
��ÿ��dsm����һ��findnext������ϵͳ������һ��
��Ŀ¼�µ��ļ���һ����Ŀ¼�����ú���ֻ���ز���
�������nameΪ���ַ���""ʱ��ע��nameָ��մ�����
name����NULL��������Mythroadƽ̨������ļ���ǰĿ¼��
�磺���ֻ���"/dsmdata/"��ΪMythroadƽ̨������ļ���ǰ
Ŀ¼����nameΪ���ַ���ʱ������Ŀ¼"/dsmdata"��
����һ��Ŀ¼�����ļ���"a.bmp"��"b.mrp"��Ŀ¼"data"����
mr_c_findStart���ز��Ҿ����"a.bmp"/handle�������ŵ�findnext
����"b.mrp"/0��"data"/0��XX(��Ч)/-1��
����:
name	Ŀ¼��
len	��������С
���:
buffer  �����������ڴ�Ų��ҳɹ�ʱ��һ���ļ�����һ��
            ��Ŀ¼��
����:
      >0                  ���Ҿ������findnext��findstop����ʹ��
      -1      ʧ��
*/
int32 findstart(const char* name, char* buffer, uint32 len);

/*
ȡ��һ��Ŀ¼�����Ľ���������������buffer�С���
Ŀ¼�еĽ�����������󣬷���-1��
������Ҫע����ǣ�ʹ��findnext��õ���Ŀ¼��
����"."��".."��
����:
   search_handle	����findstartʱ���صĲ��Ҿ��
   len	��������С
���:
   buffer  �����������ڴ�Ų��ҳɹ�ʱ�ļ�����һ����Ŀ¼��
����:
      0  �����ɹ�
      -1   ��������������ʧ��
*/
int32 findnext(int32 search_handle, char* buffer, uint32 len);

/*
Ŀ¼������������ֹһ��findstart������Ŀ¼������
����:
search_handle        ����findstartʱ���صĲ��Ҿ��
����:
      0  �ɹ�
      -1   ʧ��
*/
int32 findstop(int32 search_handle);

/********************************��ͼ�ӿ�********************************/

/*
��ָ���ı���������Ļָ��λ�á�
����:
pcText:         ����ʾ���ַ���
x, y:             ����ʾ���ı����Ͻ�x, y
r, g, b:          ������ɫr,g,b��r,g,b��ȡֵ��Χ��0~255����ͬ��
is_unicode:    �Ƿ���unicode��TRUE(1)��ʾʹ��unicode���룬FALSE(0)��
                     ʾʹ��GB2312���롣
font:              �����С�����ܵ�ֵ��   
                        0 С����
                        1 ��������
                        2 ������
����ֵ:
   0  �����ɹ�
   -1      ����ʧ��
*/
int32 dtext(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);

/*
�뺯��dtext�������ƣ���ͬ������dtext��
��ʾ���ַ����������Ͻ�Ϊ��x, y�������Ϊ��w, h��
�ľ��η�Χ֮�ڡ�
1936����֮��汾��:���Զ�����ʱ������ʾ�����
����
1935�汾�޷���ʾ����ַ���
����:
pcText:         ����ʾ���ַ���
x, y:             ����ʾ���ı����Ͻ�x, y
rect:            ��ʾ���ַ����Ʒ�Χ(MRPAPI��������ָ�봫�ݣ�
color:          ������ɫ(MRPAPI��������ָ�봫�ݣ�
font:              �����С�����ܵ�ֵ��   
                        0 С����
                        1 ��������
                        2 ������
flag������ȡ���µ�ֵ��Ҳ��������Щֵ����ӣ�
         1 //�Ƿ�ʹ��UNICODE��, �����ֽ���
         2 //�Ƿ��Զ�����
����ֵ:
    ָʾ������Ļ�ϵ�һ��δ������ʾ�ַ���������
��������Text��unicode����ֵ����ʹ����ʹ��gb������
�������������ص�����ֵҲ�Ǹ��ݸ��ַ���ת��Ϊ
unicode���������ֵ���������ַ�����������ʾ����
���ظ��ִ�ת��Ϊunicode����ĳ��ȡ�
*/
int32 dtextex(char* pcText, int16 x, int16 y, rectst *rect, colorst *color, int flag, uint16 font);

/*
��������������ʾ�ı�����Ӧ�����ұ���ʾ���ı���
��һ���ַ������ң��ú�����֧���Զ����С�
������֧��GB
����:
pcText:         ����ʾ���ַ���
x, y:             ����ʾ���ı����Ͻ�x, y
rect:            ��ʾ���ַ����Ʒ�Χ(MRPAPI��������ָ�봫�ݣ�
color:          ������ɫ(MRPAPI��������ָ�봫�ݣ�
font:              �����С�����ܵ�ֵ��   
                        0 С����
                        1 ��������
                        2 ������
flag��          ��Ч�������ò�����Ϊ��ͳһ
����ֵ:
    ָʾ������Ļ�ϵ�һ��δ������ʾ�ַ�������(�ֽ�)��
��������Text��unicode����ֵ���������ַ���������
��ʾ���򷵻ظ��ִ�ת��Ϊ0��
*/
int32 dtextright(char* pcText, int16 x, int16 y, rectst *rect, colorst *color, int flag, uint16 font);

/*
��gb�ַ���ת��ΪUnicode�ַ�����������һƬ�ڴ汣
��Unicode�ַ�������Unicode�ַ�����ָ�뷵�ء�
����:
cp: �����gb�ַ���
err����NULL����errΪ��NULL������ת������ʱerr���س�
          ���ַ�������
���:
err����errΪ��NULL������ת������ʱerr���س����ַ�
             ������
size�������Unicode�ַ�������
����:
    NULL        ת������
    ����    Unicode�ַ���ָ��
*/
uint16* c2u(char *cp, int32 *err, int32 *size);


/*
��ȡ�ַ�������ʾ��Ⱥ͸߶ȡ�(MRPAPIԭ��Ϊmrc_textWidthHeight)
����:
pcText:         ����ʾ���ַ���
is_unicode:    �Ƿ���unicode��TRUE(1)��ʾʹ��unicode���룬FALSE(0)��
                     ʾʹ��GB2312���롣
font:              �����С�����ܵ�ֵ��   
                        0 С����
                        1 ��������
                        2 ������
���:
w���ַ�������ʾ���
h���ַ�������ʾ�߶�
����:
    0  �����ɹ�
    -1      ����ʧ��
*/
int32 textwh(char* pcText, int is_unicode, uint16 font, int32* w, int32* h);


/*
������Unicode�ַ�������
���ش���ʾ�ַ�������ʾ�ڿ�Ϊw�������
��Ҫ��������
pcText:         ����ʾ���ַ���
font:              �����С�����ܵ�ֵ��   
                        0 С����
                        1 ��������
                        2 ������
w                  ����ʾ���
 ����:
    -1      ����ʧ��
    ����           �ַ�������
*/
int32 unitextrow(uint16* pcText, uint16 font, int32 w);



/*
���ƾ�����ָ������
����:
x,y,w,h:	λ��
r,g,b	     ������ɫ
*/
void drect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);

/*
�����߶Ρ�
����:
x1,y1,x2,y2:	��ĩ��λ��
r,g,b	          ������ɫ
*/
void dline(int16 x1, int16 y1, int16 x2, int16 y2, uint8 r, uint8 g, uint8 b);

/*
����Ļ��ָ��λ�û��Ƶ㡣
����:
x, y	           ���Ƶ��λ��
nativecolor	�����ɫ����R:G:B = 5:6:5��
*/
void dpoint(int16 x, int16 y, uint16 nativecolor);

/*
����Ļ��ָ��λ�û��Ƶ㡣
����:
x, y	           ���Ƶ��λ��
r,g,b	          ������ɫ
*/
void dpointex(int16 x, int16 y, int32 r, int32 g, int32 b);

/*
ʹ��ָ������ɫ�����Ļ��
����:
r,g,b	          ������ɫ
*/
void cls(int32 r, int32 g, int32 b);

/*
ˢ����Ļָ�������򡣸ú����Ĺ����ǽ�mythroad��Ļ
����ָ��������ˢ�µ���Ļ�ϡ�
����:
x, y, w, h	       ��Ļָ�����������Ͻ�Ϊ��x,y�������
                        Ϊ��w,h����
*/
void ref(int16 x, int16 y, uint16 w, uint16 h);

/*
����Ļ�����Ͻ�x,y�����Ϊw,h�ľ��������ڵ�r,g,b�ֱ�
��ǿperr/256, perg/256, perb/256����
��perr=256, perg=0, perb=0����ֻ�������������ڵĺ�ɫ����
perr=perg= perb=128����ʹ���������ڲ���һ�ְ�͸����Ч
����
����:
x,y,w,h	��Ļλ�á�
perr, perg, perb	r,g,b�仯����ֵ��
*/
void effsetcon(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb);




/********************************���ػ�UI�ӿ�********************************/

/*
����һ���˵��������ز˵������title�ǲ˵��ı��⡣

����:
title	�˵��ı��⣬unicode���룬�����ֽ���
num	�˵������Ŀ

����:
      ������   �˵����
      -1   ʧ��
*/
int32 menucreate(const char* title, int16 num);

/*
    �ڲ˵�������һ���˵������index�Ǹò˵����
    ��ţ����û�ѡ��ĳ���˵���ʱ����index��ͨ��
    event�������͵�Ӧ���С�

����:
menu  �˵��ľ��
text �˵�������֣�ʹ��unicode���룬�����ֽ���
index    �˵����index�ţ���0��ʼ��

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 menuset(int32 menu, const char *text, int32 index);

/*
��ʾ�˵������˵���ʾʱ������û�ѡ���˲˵���
��ĳһ�ϵͳ������MythroadӦ����Ϣ��ͨ��
event�������͸�MythroadӦ�ã���Ϣ����ΪMN_SLT��
����1Ϊ�ò˵����index������û�ѡ��
���˳��ò˵���ϵͳ������MythroadӦ����Ϣ��ͨ��
event�������͸�MythroadӦ�ã���Ϣ����ΪMN_RET��

����:
menu �˵��ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 menushow(int32 menu);

/*
�ͷŲ˵���

����:
menu �˵��ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 menudel(int32 menu);

/*
ˢ�²˵���ʾ��

����:
menu �˵��ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 menuref(int32 menu);

/*
����һ���Ի��򣬲����ضԻ����������Ի�����
ʾʱ������û����˶Ի����ϵ�ĳ������ϵͳ����
��MythroadӦ����Ϣ��ͨ��event�������͸�MythroadӦ
�ã���Ϣ����ΪMR_DIALOG������Ϊ�ð�����ID��
"ȷ��"��IDΪ��0��"ȡ��"��IDΪ��1��

����:
title	�Ի���ı��⣬unicode���룬�����ֽ���
text	�Ի������ݣ�unicode���룬�����ֽ���
type	�Ի������ͣ�
      0���Ի�����"ȷ��"����
      1���Ի�����"ȷ��"��"ȡ��"����
      2���Ի����� "����"����

����:
      ������   �Ի�����
      -1   ʧ��
*/
int32 dlgcreate(const char * title, const char * text, int32 type);

/*
�ͷŶԻ���

����:
dialog  �Ի���ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 dlgdel(int32 dialog);

/*
ˢ�¶Ի������ʾ��

����:
dialog	�Ի���ľ��
title	�Ի���ı��⣬unicode���룬�����ֽ���
text	�Ի������ݣ�unicode���룬�����ֽ���
type	��typeΪ-1����ʾtype���䡣
�Ի������ͣ�
      0���Ի�����"ȷ��"����
      1���Ի�����"ȷ��"��"ȡ��"����
      2���Ի����� "����"����

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 dlgref(int32 dialog, const char * title, const char * text, int32 type);

/*
����һ���ı��򣬲������ı��������ı�������
��ʾֻ����������Ϣ���ı���ͶԻ���û�б���
�����𣬽�������ʾ��ʽ�ϵĲ�ͬ����ʹ��������
����Ҫ�����ǣ��Ի��������һ��϶̣��ı����
����һ��ϳ����Ի���һ��ʵ��Ϊ����ʽ�Ĵ��ڣ�
�ı���һ��ʵ��Ϊȫ��ʽ�Ĵ��ڡ�Ҳ�������ֻ���
�Ի�����ı���ʹ������ͬ�ķ�ʽʵ�֡�
�ı���ͶԻ������Ϣ������һ���ġ����ı�����
ʾʱ������û�ѡ�����ı����ϵ�ĳ������ϵͳ��
����MythroadӦ����Ϣ��ͨ��event�������͸�Mythroad
ƽ̨����Ϣ����ΪMR_DIALOG������Ϊ�ð�����ID��
"ȷ��"��IDΪ��0��"ȡ��"��IDΪ��1��

����:
title	�ı���ı��⣬unicode���룬�����ֽ���
text	�ı������ݣ�unicode���룬�����ֽ���
type	�ı������ͣ�
      0���ı�����"ȷ��"����
      1���ı�����"ȷ��"��"ȡ��"����
      2���ı����� "ȡ��/����"����

����:
      ������   �ı�����
      -1   ʧ��
*/
int32 textcreate(const char * title, const char * text, int32 type);

/*
�ͷ��ı���

����:
text �ı���ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 textdel(int32 text);

/*
ˢ���ı�����ʾ��

����:
handle	�ı���ľ��
title	�ı���ı��⣬unicode���룬�����ֽ���
text	�ı������ݣ�unicode���룬�����ֽ���

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 textref(int32 handle, const char * title, const char * text);

/*
����һ���༭�򣬲����ر༭�������༭������
��ʾ���ṩ�û��༭������Ϣ��text�Ǳ༭����ʾ��
��ʼ���ݡ�
���༭����ʾʱ������û�ѡ���˱༭���ϵ�ĳ��
����ϵͳ������MythroadӦ����Ϣ��ͨ��event����
���͸�MythroadӦ�ã���Ϣ����ΪMR_DIALOG������
Ϊ�ð�����ID��"ȷ��"��IDΪ��0��"ȡ��
"��IDΪ��1��

����:
title	�༭��ı��⣬unicode���룬�����ֽ���
text	�༭��ĳ�ʼ���ݣ�unicode���룬�����ֽ���
type	���뷨���ͣ�
      0�� �κ��ַ�
      1�� ����
      2�� ���룬��"*"��ʾ
max_size	������������ַ���unicode������������ÿһ
                �����ġ���ĸ�����֡����Ŷ���һ���ַ���
����:
      ������   �༭����
      -1   ʧ��
*/
int32 editcreate(const char * title, const char * text, int32 type, int32 max_size);

/*
�ͷű༭��

����:
edit �༭��ľ��

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 editdel(int32 edit);

/*
��ȡ�༭�����ݣ�unicode���롣�����������ڱ༭��
�ͷź���Ȼʹ�ñ༭������ݣ���Ҫ���б������
�ݡ��ú�����Ҫ�ڱ༭���ͷ�֮ǰ���á�

����:
edit �༭��ľ��

����:
      ��NULL       �༭�������ָ�룬unicode���롣
      NULL            ʧ��
*/
char* editget(int32 edit);

/********************************�����ӿ�********************************/
/*
�˳���ʽ����mode������0ʱ��Ӧ�õ���exit����֪ͨmythroad��Ӧ�ý��Զ����ú���exitapp()��
*/
void set_exit(int mode);
/*
�˳�Ӧ�ã�Ӧ�õ��øú���֪ͨmythroad��Ӧ�ý�Ҫ�˳���
*/
void exit(void);

/*
��ȡϵͳʱ�䣬��λ���롣ϵͳʱ�������Mythroadƽ
̨��������֮ǰ������ʱ��Ϊ��ʼʱ�䣬���ش���
ʼʱ�䵽Ŀǰ�����ĺ���������������������ܷ�
�ص���ϵͳ�����󾭹��ĺ�������
����:
     ��λ�����ϵͳʱ��
*/
int32 getuptime(void);


/*
��ȡϵͳ����ʱ�䡣
mr_datetime��ʽ����(���ڲ����ã�������Ҫ�Լ���������ṹ������
typedef struct
{
   uint16 year;                 //��
   uint8  month;                //��
   uint8  day;                  //��
   uint8  hour;                 //ʱ��24Сʱ��
   uint8  minute;               //��
   uint8  second;               //��
}mr_datetime;

���:
datetime	����ʱ��

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 getdatetime(mr_datetime* datetime);

/*
��ȡϵͳ��ʣ����ڴ���

����:
      ϵͳ��ʣ����ڴ�������λ�ֽ�
*/
uint32 getmemremain(void);

/*
�����ֻ��𶯡�

����:
ms             �𶯳�����ʱ�䣬��λ����

����:
      0     �ɹ�
      -1         ʧ��
*/
int32 shake(int32 ms);

/*
ȡ��ϵͳ���ܹ��ڴ��С
*/
int32 getsysmem(void);



/*
��ָ����mrp�ļ��ж�ȡָ�����ļ���ʵ��ʹ��mrc_readFileFromMrpEx
����:
packname     mrp�ļ���
filename        ����ȡ�ļ����ļ���
*filebuf          ��ȡ�ļ����������
                    ���ļ���ѹ����ʽ���ʱ����:
                    *filebuf==NULL����������һƬ�ռ��ŷ��ص��ļ�
                    *filebuf!=NULL����ʱ����ʹ��*filebufָ��Ŀռ���
                           ���ص��ļ�����
*filelen          ��*filebuf!=NULL���ļ���ѹ����ʽ���ʱ��ָ��
                    *filebufָ��Ŀռ��С�����ÿռ��С������
                    ��Ž�ѹ�Ժ���ļ�����������ʧ��
lookfor          ָ�������Ĳ�����ʽ:
                    0:    ��ȡmrp���ļ������ݣ���ͨ��*filebuf��*filelen
                               ���ظ����ݣ���*filebuf==NULLʱ�����ص��ڴ�
                               ��Ҫ�������ͷţ�
                    1:    ��������mrp���Ƿ���ڸ��ļ���������ȡ
                               �ļ�����
                    2:    ��mrp�ļ�λ��ROM��RAM��ʱ����ȡ����ȡ��
                                ����ԭʼ���ݣ���ʹ�ļ�������ѹ��Ҳ
                                �����н�ѹ�����ص��ڴ治��Ҫ�ͷţ�
                                ��mrp�ļ�λ���ļ�ϵͳ��ʱ����ͬ��
                                lookfor==0
                    3:    ��ǿ���ķ�ʽ��ȡmrp���ļ������ݣ�����
                               lookfor==0�ķ�ʽ��ȡ�ļ�ʱ���ļ��Ƿ�ѹ��
                               ���ж�������Ӧ�ģ����Բ�����ȫ��֤�ļ�
                               ����ȷ�ԣ�lookfor==3ʱ��ǿ��Լ���ļ�������
                               ѹ����������ǿ��顣��*filebuf==NULLʱ������
                               ���ڴ���Ҫ�������ͷţ�
                     4:��3��ͬ����ͬ����ʹ��malloc8�����ڴ棻
                     5:��ͨ��filelen�����ļ����ȣ�

���:
*filebuf         ��lookfor==0��2��3ʱ�����ļ�����ָ��
*filelen         ��lookfor==0��2��3ʱ�����ļ����ݳ���

����:
      0   �ɹ�����lookfor==1ʱ��ʾ�ļ�����
      -1       ʧ��

(���շ����в��ִ�����ʹ��readFileFromMrpEx����ʱ��
�Խ�����жϽ�ʹ����filebuf���ж��Ƿ�ΪNULL����������
�˺����ķ���ֵ��
      ���������ѣ�readFileFromMrpEx�����ڲ���ʧ��ʱ������
      ֵ��ʧ�ܣ���filebuf��ֵ��δ���壨Ҳ����˵��������
      NULL��Ҳ���ܲ���NULL�����������ʹ�ú�������ֵ��
      �н���жϣ�ʹ��filebuf�����ж��ǲ����յġ�)free�ͷ�
*/
int32 readfilefrommrp(char* packname, const char* filename, 
               uint8 ** filebuf, int32 *filelen, int32 lookfor);


/*
����:
data     �ļ�����ָ��
filelen          �ļ����ݳ���
*/
void freefiledata(void* data, int32 filelen);

/*
�ͷ���ԭʼ�ڴ����뷽ʽ����Ŀռ䡣
����:
add     ָ��
size   ����
*/
void freeorigin(void* add, int32 size);


/////////////////////���½ӿڲ����Ƕ�����mrc_base.h�ļ��У���Ϊ��ʹ�÷����ͳһ�����ڱ�ϵͳ��//////////////////////////
// ���º���������mrc_exb.h�ļ���
/*
ԭ������:mrc_unicodeToGb2312 
���룺
��unicode������ַ���ת��gb2312����
input����Ҫת����unicode�ַ���
input_len:��Ҫת����unicode�ַ������ȣ���λ�ֽ���
output��ת���ɹ��Ժ��gb2312�����ַ�����Ż�������
���������ڴ���Ӧ�õ������ṩ�������ͷš�
ע�⣺���������������ʹ��malloc���䴦��Ŀռ䣬�����ֻ����ܻ����������
output_len��output�������ĳ��ȣ���λ�ֽ���

���أ�
    MR_SUCCESS	ת���ɹ�
    MR_FAILED   ת��ʧ��

*/
int32 u2c(uint8* input, int32 input_len, uint8** output, int32* output_len);


int32  runmrp(char * mrp_name,char* filename);

/*
ԭ��������:mrc_GetParentPath
����:��ȡ��ǰ����MRP���ڸ�Ŀ¼�����ص�BUFFָ�����ʱ�����Ա�ֶ��ͷ�
����:void
����ֵ:char*  ��Ŀ¼·��ָ��
		    NULL  ��ȡʧ��
*/
char* getparentpath(void);


// ��������ӣ�����getuptime�õ���ϵͳ����ʱ����Ϊ���ӣ����Ա�֤rand���ظ���
void sand(uint32 seed);

// �������������
uint32 rand(void);

/*
���Ͷ���
pNumber �绰���룬ASCII�� 
pContent ��Ϣ���ݣ�ASCII���Unicode�룬��flags�������� 
flags              0��2bit����������
                   0: ASCII��
                   1: Unicode��, �����ֽ���
                   3bit���Ƿ�����״̬����
                   1������״̬����
                   0��������״̬����
                   4bit���Ƿ����ط��ͽ�����ɹ�/ʧ�ܣ���ʾ
                   1������
                   0��������
                   5��31 bit������ 
�ɹ���������MR_SUCCESS,ʧ�ܷ���MR_FAILED, δ׼���÷���MR_IGNORE
*/
int32 sms(char* pNumber, char*pContent, int32 flags);

/*
����:mrc_LCDCanSleep
	�򿪡��ر�LCD��ʱ���߹��ܡ�
	
����:
	char CanSleep=1 ʱ������LCD��ʱ���ߣ�
	char CanSleep=0ʱ��������LCD��ʱ���ߡ�

�����
MR_SUCCESS ���ɹ�
MR_FAILED  ��ʧ��
MR_IGNORE  : ��֧�ָù���
*/
int32 lcd(char CanSleep);



// ���º���������mrc_bmp.h�ļ���//////////////////////////////////////


//ȡ��Ļ���������ڴ��ַ
uint16 *getscrbuf(void);

//������Ļ���������ڴ��ַ
void setscrbuf(uint16 *ptr);

//��ȡ��Ļ���������
void getscrsize(int32 *w,int32 *h);

//������Ļ���������
void setscrsize(int32 w, int32 h);


enum {
    BM_OR,          //SRC .OR. DST*   ��͸��Ч��
    BM_XOR,         //SRC .XOR. DST*
    BM_COPY,        //DST = SRC*      ����
    BM_NOT,         //DST = (!SRC)*
    BM_MERGENOT,    //DST .OR. (!SRC)
    BM_ANDNOT,      //DST .AND. (!SRC)
    BM_TRANS, //͸��ɫ����ʾ��ͼƬ�ĵ�һ�����أ����Ͻǵ����أ���͸��ɫ
    BM_AND,         //DST AND SRC
    BM_GRAY,        //�ҶȻ�ͼ�� �൱��BM_TRANSPARENT���ҶȻ�ͼ��DST�Ҷ� �� 30%R + 59%G + 11%BͼƬ���
    BM_REVERSE      //�����ͼ���൱��BM_TRANSPARENT�������ͼ��V1939��
};
/*
ropȡֵ(ϵͳû��Ԥ�ȶ�����Щ��):
#define TRANS_NONE				0x0000   û�н���ת���ͷ�ת�� 
#define TRANS_ROT90				0x0100   90��ת���� 
#define TRANS_ROT180			0x0200   180��ת���� 
#define TRANS_ROT270			0x0300   270��ת���� 
#define TRANS_MIRROR			0x0400   ���ҷ�ת�� 
#define TRANS_MIRROR_ROT90		0x0500   ���ҷ�ת��90��ת���� 
#define TRANS_MIRROR_ROT180		0x0600   ���ҷ�ת��180��ת���� 
#define TRANS_MIRROR_ROT270		0x0700   ���ҷ�ת��270��ת���� 
����:
p        ԴͼƬָ��
x,y     ��Ļ	��ʾλ��
mw		ͼƬ��ԭʼ��ȣ���������ȡ�
rop : (BM_TRANSPARENT ��BM_COPY�������������ģʽĬ��BM_COPY����) | (TRANS_NONE��TRANS_MIRROR_ROT270,����
		������mrc_bitmapshowEx��ͬ)
sx,sy           ԴͼƬ����ʼλ��
w,h             ������ͼƬ�Ŀ��
transcolor : ͸��ɫ (16bit),������MAKERGB�꽫(r,g,b)ת��Ϊ16bit�ĸ�ʽ��

����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ��
*/
int32 bmpshowflip(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
		int16 sx, int16 sy, uint16 transcolor);


/*
mrc_aux.h�ļ���
��ȡmrp��һ���ļ���ָ���������ݵ�ָ���ռ䡣����
��ȡ�ļ�filename��len���ȵ����ݵ�bufָ��ĵ�ַ�ϣ���
�ļ�����С��len������ȡ�ļ����ȵ����ݣ�len����ֵ
Ϊʵ�ʶ�ȡ�ĳ��ȡ�

����:
filename          �ļ���
buf                  ��ȡ���ļ��ı���ռ�
len                  ������ȡ����

���:
len                  ʵ�ʶ�ȡ����

����:
      0  �ɹ�
      -1   ʧ��
*/
int32 readall(char* filename, char* buf, int32 *len);

void wap(char* wap);


/********************************��ʱ���ӿ�********************************/

/*
������ʱ��

����:
      ��0     ��ʱ�����
      0          ʧ��
*/
int32 timercreate(void);

/*
ɾ����ʱ��

����:
t           ��ʱ�����
*/
void timerdel(int32 t);

/*
ֹͣ��ʱ��

����:
t           ��ʱ�����
*/
void timerstop(int32 t);

/*
��ʱ���ص�����(ֻ��һ����ʽ���ڱ�ϵͳ��Ч)
����:
data:
   ������ʱ��ʱ�����data������
*/
typedef void (*timerCB)(int32 data);

/*
������ʱ��

����:
t           ��ʱ�����
time      ��ʱ��ʱ������λ����
data      ��ʱ������
f           ��ʱ���ص�����������Ϊ��֧�ֺ���ָ�����ǽ������У���������Ӧ������ص������ĺ������ַ�����
loop      �Ƿ�ѭ����0:��ѭ����1:ѭ��

����:
      0     �ɹ�
      -1         ʧ��
*/
int32 timerstart (int32 t, int32 time, int32 data, char* f, int32 loop);


/*
���Ķ�ʱ��ʱ����

����:
t           ��ʱ�����
time      ��ʱ��ʱ������λ����

����:
      0     �ɹ�
      -1         ʧ��
*/
int32 timersettime(int32 t, int32 time);

///////////////////���º���Ϊϵͳ��չ����////////////////////

//��ʾһ��ͼƬ
int img(char* filename, int x,int y);

/*
���������
����mode����ѡ����ֵ
enum E_SHAD_MODE ����Ҫ�Լ�����,��ֱ������ֵ��
{
    SHADE_UPDOWN,		//���ϵ���
    SHADE_LEFTRIGHT,	//������
    SHADE_DOWNUP,		//���µ���
    SHADE_RIGHTLEFT		//���ҵ���
};
���续һ����ɫ����ɫ����ľ��Σ�
shaderect(0,0,176,220,0xff0000,0xffff00,SHADE_UPDOWN);
*/
void shaderect(int x, int y, int w, int h, int32 pixelA, int32 pixelB, int mode);
/*
��ȡ��ǰmrp�е��ļ����ڴ��У��û���Ҫ�ֶ���free�����ͷ��ڴ�
name   mrp�����ļ�����
len    ��ȡ���ֽ���
*/
void* readmrpfile(char* name,uint32* len);
/*
��ʾmrp���ڵ�ͼƬ
name mrp�����ļ�����
ropȡֵ:
1��TRANS_NONE�� û�н���ת���ͷ�ת�� 
2��TRANS_ROT90�� 90��ת���� 
3��TRANS_ROT180�� 180��ת���� 
4��TRANS_ROT270�� 270��ת���� 
5��TRANS_MIRROR�� ���ҷ�ת�� 
6��TRANS_MIRROR_ROT90�� ���ҷ�ת��90��ת���� 
7��TRANS_MIRROR_ROT180�� ���ҷ�ת��180��ת���� 
8��TRANS_MIRROR_ROT270�� ���ҷ�ת��270��ת���� 
����:
p        ԴͼƬָ��
x,y     ��Ļ	��ʾλ��
mw		ͼƬ��ԭʼ��ȣ���������ȡ�
rop : (BM_TRANSPARENT ��BM_COPY�������������ģʽĬ��BM_COPY����) | (TRANS_NONE��TRANS_MIRROR_ROT270,����
		������mrc_bitmapshowEx��ͬ)
sx,sy           ԴͼƬ����ʼλ��
w,h             ������ͼƬ�Ŀ��
transcolor : ͸��ɫ (16bit),������MAKERGB�꽫(r,g,b)ת��Ϊ16bit�ĸ�ʽ��

����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ��
*/
int32 bmpshowflipmrp(char* name,int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop,int16 sx, int16 sy, uint16 transcolor);
int32 bmpshowflipmrpex(char* name,int16 x,int16 y,int16 mw,int16 w,int16 h,uint16 rop,int16 sx,int16 sy);
/*
��ȡ��ǰ��Ļ
bmp24λͼƬ
name ·��
*/
int32 printscreen(char* name);

/*
ɾ�������ļ��а����ļ����ڵ��ļ�
dir �ļ�������
*/
int32 removedir(char* dir);
/*
 �л�MRP����Ŀ¼
 strDiskΪĿ¼�ַ���,��"A:/","B:/"
*/
int32 switchdisk(char* strDisk);

#endif
 