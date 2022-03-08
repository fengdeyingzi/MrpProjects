#ifndef _BASE_H_I
#define _BASE_H_I
typedef struct
{
	uint8       IMEI[16];	      //IMEI ����Ϊ15�ֽ�
	uint8       IMSI[16];	      //IMSI ����Ϊ15�ֽ�
	char        manufactory[8];  //�����������7���ַ������ֽ���\0
	char        type[8];         //mobile type�����7���ַ������ֽ���\0
	uint32      ver;            //��ֲ����汾
	uint8       spare[12];       //����
}userinfo;

extern int32 sleep(uint32 ms);
extern int32 getSysInfo(mr_userinfo* info);
extern int32 sendSms(char* pNumber, char*pContent, int32 flags);
extern void call(char *number);
extern void connectWAP(char* wap);


extern int32 plat(int32 code, int32 param);
extern uint8* getFixMrp(int32 index);
extern uint8* getFixMrpByName(char* name);
extern int32 encode(uint8  *in, uint32 len,uint8 *out);
extern int32 setReloadFile(char* mrpname, char* filename);
extern int32 getReloadFile(char** mrpname, char** filename);


int32 loadSmsCfg(void);
int32 readSmsCfg(int32 pos, char* p, int32 len);
int32 setSmsCfg(int32 pos, char* p, int32 len);
int32 saveSmsCfg(void);

char* getMrParam(void);//1946
char* getMrEntry(void);//1952, V2000-V2002��֧�֣�


extern void* malloc8(int size);
extern void free8(void *address);


/*
�˳�Ӧ�ã�Ӧ�õ��øú���֪ͨmythroad��Ӧ�ý�Ҫ��
����
��������˳�ǰ�����ûص�����cb��
*/
extern void exitImmediately( timerCB cb, int32 data);




int32 isZip(uint8* inputbuf, int32 inputlen);

/*
��ѹ����

���:
outputbuf    ��ѹ������ݣ���ҪӦ���Լ��ͷ�( free)��
outputlen    ��ѹ������ݳ���
      
����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ�ܡ�
      MR_IGNORE        ���ݲ��ǿ�ʶ���ѹ����ʽ��
*/
int32 unzip(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen);
int32 unzipVM(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen);

/*��ȡ��extģ��ȫ�ֱ���ʹ�õ��ڴ�ռ��С(�ֽ�)*/
int32 getExtGlobalMem(void);


int32 checkMrp(char* mrp_name);

int32 timerInit(void);

/*******************************C�������**********************************/

/*
�ñ��������˸�ģ���C�����������
*/
extern int32 extHandle;

/*
�ú�����Ҫ����extģ����ʵ�֣�������extģ���е�
�� extSendAppEvent����ʱ����extģ���еĸú�������
�á�
����:
app,code,param0,param1    ��Ϊģ��䴫�ݵĲ���;
����ֵ:
����ֵ������Ϊ��ģ�� extSendAppEvent�����ķ���ֵ
*/
extern int32 extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1);
extern int32 extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4,
								   int32 p5);


/*
���� extLoad����һ��ext�ļ���������C�������
����:
buf     ָ��ext�ļ����ݵ�ָ��
len     ext�ļ�����
����ֵ:
MR_FAILED      ����ʧ��
����           C���������
*/
int32 extLoad(uint8* buf, int32 len);

/*
�����ͷ�һ���Ѿ����ص�C�������

ע��:
     �����Ҫ����ģ�鷢����ģ����¼��������е��ø�
  �����ͷ���ģ�飬��Ҫע����ǣ��������� extFree����
  ֮��ʵ������ģ�������еĴ���ռ����߼����Ѿ���
  �ͷţ�ע�ⲻҪ�� extFree����֮���ٽ������ڴ�������
  �صĲ�����

����:
ext     C���������
����ֵ:
MR_FAILED      ����ʧ��
MR_SUCCESS  �����ɹ�
*/
int32 extFree(int32 ext);


/*
�����ͷ�һ���Ѿ����ص�C�����������Ӧ�Ķ�ʱ����
���˶�ʱ��֮�⣬�������ͷ�������Դ��


����:
ext     C���������
����ֵ:
MR_FAILED      ����ʧ��
MR_SUCCESS  �����ɹ�
*/
int32 extFreeTimer(int32 ext);

/*
��������һ���Ѿ����ص�C��������ģʽ��
0:��ģ��
1:��ͨ���
2:Ӧ�ò��
*/
void extSetMode(int32 ext, int32 mode);


/*
������һ���Ѿ����ص�C���������г�ʼ����
����:
ext     C���������
����ֵ:
MR_FAILED      ����ʧ��
MR_SUCCESS  �����ɹ�
*/
int32 extInit(int32 ext);

/*
�������¼������Ѿ���ʼ����C��������С�
����:
ext     C���������
���������� event��ͬ
����ֵ:
C������� event����ֵ
*/
int32 extEvent(int32 ext, int32 code, int32 param0, int32 param1);

/*
��������ͣ���������Ѿ���ʼ����C��������С�
����:
ext     C���������
����ֵ:
C������� pause����ֵ
*/
int32 extPause(int32 ext);

/*
�������ָ����������Ѿ���ʼ����C��������С�
����:
ext     C���������
����ֵ:
C������� resume����ֵ
*/
int32 extResume(int32 ext);
/*
����extģ���е��øú�������ext(����)ģ���е�
 extRecvAppEvent�����������á�
����:
app,code,param0,param1    ��Ϊģ��䴫�ݵĲ���;
����ֵ:
����ֵΪ��extģ�� extRecvAppEvent�����ķ���ֵ
*/
int32 extSendAppEvent(int32 app, int32 code, int32 param0, int32 param1);
int32 extSendAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3,int32 p4, int32 p5);


int32 extMpsFpCall(int32 ext, mpsFpFuncType func, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

int32 extInitMainModule(void);

#endif