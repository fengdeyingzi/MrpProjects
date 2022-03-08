#ifndef MRC_BASE_H_I
#define MRC_BASE_H_I

typedef struct
{
	uint8       IMEI[16];	      //IMEI ����Ϊ15�ֽ�
	uint8       IMSI[16];	      //IMSI ����Ϊ15�ֽ�
	char        manufactory[8];  //�����������7���ַ������ֽ���\0
	char        type[8];         //mobile type�����7���ַ������ֽ���\0
	uint32      ver;            //��ֲ����汾
	uint8       spare[12];       //����
}mr_userinfo;

extern int32 mrc_sleep(uint32 ms);
extern int32 mrc_getSysInfo(mr_userinfo* info);
extern int32 mrc_sendSms(char* pNumber, char*pContent, int32 flags);
extern void mrc_call(char *number);
extern void mrc_connectWAP(char* wap);

extern int32 mrc_plat(int32 code, int32 param);
typedef void (*MR_PLAT_EX_CB)(uint8* output, int32 output_len);
extern int32 mrc_platEx(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
extern uint8* mrc_getFixMrp(int32 index);
extern uint8* mrc_getFixMrpByName(char* name);
extern int32 mrc_encode(uint8  *in, uint32 len,uint8 *out);
extern int32 mrc_setReloadFile(char* mrpname, char* filename);
extern int32 mrc_getReloadFile(char** mrpname, char** filename);


int32 mrc_loadSmsCfg(void);
int32 mrc_readSmsCfg(int32 pos, char* p, int32 len);
int32 mrc_setSmsCfg(int32 pos, char* p, int32 len);
int32 mrc_saveSmsCfg(void);

char* mrc_getMrParam(void);//1946
char* mrc_getMrEntry(void);//1952, V2000-V2002��֧�֣�


extern void* mrc_malloc8(int size);
extern void mrc_free8(void *address);


/*
�˳�Ӧ�ã�Ӧ�õ��øú���֪ͨmythroad��Ӧ�ý�Ҫ��
����
��������˳�ǰ�����ûص�����cb��
*/
extern void mrc_exitImmediately(mrc_timerCB cb, int32 data);


/*
ȡ���û���ID����ÿ���ֻ��û���mythroad��������ֻ�
��������һ��ID�ţ�������ͬ�û�ID����ͬ�ĸ��ʽ�С��

���:
id      һ��ָ�룬ָ��һƬ16�ֽڳ����ڴ�ռ䣬��
         �ռ䱣�����û�ID��
      
����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ�ܣ�Ϊ��ֹ��ͬһ�ֻ������ɵ�ID��ͬ��
                               mythroad������ֻ���������һ���ļ�⣬��
                               �������ֻ��������Ƿǳ�ȷ��ʱ��������
                               ʧ�ܡ�
*/
int32 mrc_getUserID(uint8** id);

/*
��1945��ǰ��parameter������Ч��
Ŀǰ��SDK�汾parameter����Ч�ģ���ʹ֮��Ч����V1945��ǰ
�ǻ�������ġ�
*/
extern int32 mrc_runMrp(char* mrp_name, char* file_name, char* parameter);

/*
���Ͷ�����Ϣ

����:
      MR_SUCCESS   ���ͳɹ�
      MR_FAILED   ����ʧ��
      MR_IGNORE   δ׼����
*/
//int32 mrc_payment(void);



typedef struct
{
   int32   id;
   int32   ver;
   char*  sidName;
}mrc_appInfoSt;


/*
�ú����������벻Ҫ��ʹ�ã�
*/
mrc_appInfoSt * mrc_getAppInfo(void);

/*
  �õ�Ӧ����Ϣ
  ����: 
      packFileName  Ӧ�õİ�����
  ���:
      MR_FAILED  �ɹ�
      MR_SUCCESS ʧ��
  
  �������: 
	  appid     Ӧ��ID         
	  appver    Ӧ�ð汾��
	  appname   Ӧ������
*/
//int32 mrc_getAppInfoEx(const char *packFileName,int32 *appid,int32 *appver,char* appname);




int32 mrc_isZip(uint8* inputbuf, int32 inputlen);

/*
��ѹ����

���:
outputbuf    ��ѹ������ݣ���ҪӦ���Լ��ͷ�(mrc_free)��
outputlen    ��ѹ������ݳ���
      
����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ�ܡ�
      MR_IGNORE        ���ݲ��ǿ�ʶ���ѹ����ʽ��
*/
int32 mrc_unzip(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen);
int32 mrc_unzipVM(uint8* inputbuf, int32 inputlen, uint8** outputbuf, int32* outputlen);


/*��ͬ�� mrc_unzip �Ĺ���,���Ǿ��и�mrc_unzipVMһ�����ٶ�,�����mrc_unzipVM����ĵط��ǿ��Լ��ٷ�ֵ�ڴ�,
inputbuf��outputbuf���������ֽڶ���*/
//int32 mrc_unzipVMEx(uint8 *inputbuf,int32 inputlen,uint8** outputbuf,int32 *outputlen);

/*��ȡ��extģ��ȫ�ֱ���ʹ�õ��ڴ�ռ��С(�ֽ�)*/
int32 mrc_getExtGlobalMem(void);


int32 mrc_checkMrp(char* mrp_name);

/*
��mrc_checkMrp�������ƣ���ͬ����:
mrc_checkMrpB����mrpͷ�е�ƽ̨�ֶν��м�顣
*/
//int32 mrc_checkMrpB(char* mrp_name);

int32 mrc_timerInit(void);


/*******************************C�������**********************************/

/*
�ñ��������˸�ģ���C�����������
*/
extern int32 mrc_extHandle;


/*
�ú�����Ҫ����extģ����ʵ�֣�������extģ���е�
��mrc_extSendAppEvent����ʱ����extģ���еĸú�������
�á�
����:
app,code,param0,param1    ��Ϊģ��䴫�ݵĲ���;
����ֵ:
����ֵ������Ϊ��ģ��mrc_extSendAppEvent�����ķ���ֵ
*/
extern int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1);
extern int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4,
                                int32 p5);



#ifdef SDK_MOD
typedef int32 (*MR_LOAD_C_FUNCTION)(int32 code);
typedef int32 (*mrc_init_t)(void);
typedef int32 (*mrc_event_t)(int32 code, int32 param0, int32 param1);
typedef int32 (*mrc_pause_t)(void);
typedef int32 (*mrc_resume_t)(void);
typedef int32 (*mrc_exitApp_t)(void);

/*
����mrc_extLoad����һ��ext�ļ���������C�������
��SDK�Ļ����£���Ҫ����C����������ں�����
����:
buf     ָ��ext�ļ����ݵ�ָ��
len     ext�ļ�����
����ֵ:
MR_FAILED      ����ʧ��
����           C���������
*/
int32 mrc_extLoad(uint8* buf, int32 len, 
      //MR_LOAD_C_FUNCTION load_f,
      mrc_init_t init_f,
      mrc_event_t event_f,
      mrc_pause_t pause_f,
      mrc_resume_t resume_f,
      mrc_exitApp_t exitApp_f);
#else
/*
����mrc_extLoad����һ��ext�ļ���������C�������
����:
buf     ָ��ext�ļ����ݵ�ָ��
len     ext�ļ�����
����ֵ:
MR_FAILED      ����ʧ��
����           C���������
*/
int32 mrc_extLoad(uint8* buf, int32 len);
#endif

/*
�����ͷ�һ���Ѿ����ص�C�������

ע��:
     �����Ҫ����ģ�鷢����ģ����¼��������е��ø�
  �����ͷ���ģ�飬��Ҫע����ǣ���������mrc_extFree����
  ֮��ʵ������ģ�������еĴ���ռ����߼����Ѿ���
  �ͷţ�ע�ⲻҪ��mrc_extFree����֮���ٽ������ڴ�������
  �صĲ�����

����:
ext     C���������
����ֵ:
MR_FAILED      ����ʧ��
MR_SUCCESS  �����ɹ�
*/
int32 mrc_extFree(int32 ext);



/*
 * extLoadA() �� extLoad() ������������
 *  ��ģ���ڼ�����ģ���д����ģ���mr_table�����������ڴ�����ĺ���ָ�롣
 *  
 *  Ŀ������ģ����Խӹ���ģ�����е��ڴ������
 *  mainextMrMAlloc/mainextMrFree/mainextMrReadFile����ģ��ʵ�֡�
 *  ע:mrc_bitmapLoad�Ⱥ�����������SDK����ʵ�ֵģ��������е��ڴ�
 *  ����Ҳ�Ǳ���ģ��ӹܵġ�
 */
#ifdef SDK_MOD
//int32 mrc_extLoadA(uint8* buf, int32 len, 
                   int32 mainextMrMalloc, int32 mainextMrFree, int32 mainextMrReadFile,
                   mrc_init_t init_f,
                   mrc_event_t event_f,
                   mrc_pause_t pause_f,
                   mrc_resume_t resume_f,
                   mrc_exitApp_t exitApp_f);
#else
//int32 mrc_extLoadA(uint8* buf, int32 len,
//                   int32 mainextMrMalloc, int32 mainextMrFree, int32 mainextMrReadFile);
#endif

//int32 mrc_extFreeA(int32 ext);


/*
�����ͷ�һ���Ѿ����ص�C�����������Ӧ�Ķ�ʱ����
���˶�ʱ��֮�⣬�������ͷ�������Դ��


����:
ext     C���������
����ֵ:
MR_FAILED      ����ʧ��
MR_SUCCESS  �����ɹ�
*/
int32 mrc_extFreeTimer(int32 ext);

/*
��������һ���Ѿ����ص�C��������ģʽ��
0:��ģ��
1:��ͨ���
2:Ӧ�ò��
*/
void mrc_extSetMode(int32 ext, int32 mode);

/*
�����ӳ��ͷ�һ���Ѿ����ص�C�������

ע��:
   �ú����Ĺ�����mrc_extFree��ͬ����C��������ȷ���ͷ�ʱ��
�����ڸú������õĹ����У����Ǹô�Ӧ�õ��÷��ص�VMʱ
�������ú��������ӳ��ͷ�һ��C������򣬵���һ��Ӧ��
�����ж�ε��øú����������һ�ε�����Ч��

����:
ext     C���������
����ֵ:
MR_SUCCESS
*/
//int32 mrc_extFreeLater(int32 ext);

/*
������һ���Ѿ����ص�C���������г�ʼ����
����:
ext     C���������
����ֵ:
MR_FAILED      ����ʧ��
MR_SUCCESS  �����ɹ�
*/
int32 mrc_extInit(int32 ext);

/*
�������¼������Ѿ���ʼ����C��������С�
����:
ext     C���������
����������mrc_event��ͬ
����ֵ:
C�������mrc_event����ֵ
*/
int32 mrc_extEvent(int32 ext, int32 code, int32 param0, int32 param1);

/*
��������ͣ���������Ѿ���ʼ����C��������С�
����:
ext     C���������
����ֵ:
C�������mrc_pause����ֵ
*/
int32 mrc_extPause(int32 ext);

/*
�������ָ����������Ѿ���ʼ����C��������С�
����:
ext     C���������
����ֵ:
C�������mrc_resume����ֵ
*/
int32 mrc_extResume(int32 ext);

/*
 * block/unblock/isBlocked�������Ʋ�����¼�״̬��
 *
 */
//extern int32 mrc_extBlock(int32 ext);
//extern int32 mrc_extUnblock(int32 ext);
//extern int32 mrc_extIsBlocked(int32 ext);


/*
����extģ���е��øú�������ext(����)ģ���е�
mrc_extRecvAppEvent�����������á�
����:
app,code,param0,param1    ��Ϊģ��䴫�ݵĲ���;
����ֵ:
����ֵΪ��extģ��mrc_extRecvAppEvent�����ķ���ֵ
*/
int32 mrc_extSendAppEvent(int32 app, int32 code, int32 param0, int32 param1);
int32 mrc_extSendAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, 
                                                           int32 p4, int32 p5);

typedef int32 (*mpsFpFuncType)(int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);


int32 mrc_extMpsFpCall(int32 ext, mpsFpFuncType func, int32 p0, int32 p1, int32 p2, int32 p3,
                                   int32 p4, int32 p5);

int32 mrc_extInitMainModule(void);

//
//typedef void (*mrc_refreshScreenHook_t)(void);
//
//extern void mrc_regRefreshScreenHook(mrc_refreshScreenHook_t hook);
//


/*
 *  ���������Ƚϵײ�Ľӿ������л�RW�������ġ�
 **/
//extern int32 mrc_extRWCntxSwitch(void);
//extern void mrc_extRWCntxRestore(int32 cntx);


/***************************************************************
 *
 *  
 *    if (�°���ģ��)  {
 *        v1 = ����App���е�plugins.lst��bltin_appid����Ӧ�� appVer;
 *        v2 = "dstFolder\\mrpName"��appVer;
 *        if (v1 > v2)  { ��ѹ��App���е����õ�mrpName �� dstFolder\\mrpName }
 *        return MAX(v1,v2);
 *    }
 *    else {
 *         return CHECK_EXT_VERSION( bltin_appid - 480000,   mrpName, dstFolder);
 *    }
 *
 *   NOTE:
 *        dstFolder �벻Ҫ��NULL�� ֱ��д���� "plugins"
 *
 *   For example:
 *             mrc_pluginapp_checkExtVersion(480010, "plugins", "netpay.mrp");
 *  
 ****************************************************************/

//extern int32 mrc_pluginapp_checkExtVersion(int32 bltin_appid,
//									const char* dstFolder,
//									const char* mrpName);
//
//
//
//extern void mrc_setScrBuf(uint16 * buf, int32 width, int32 height);


/*
��������������������VM״̬�ı���ͻָ���������������
Ӧ�û�����
*/
/*
���������������Ӧ���ֳ���������ļ�������ͨ��mrc_setSmsCfg
���浽ȫ���������ĵ�12��120�ֽ�(ƫ��MR_SECTION_LEN*11��ʼ)����
�ļ����ᱻmrc_loadVM��������VM״̬��
ע��:����ʹ����screen buf��Ϊ��ת�ռ䣬��screen buf������ֲ���
��ģ���������ʧ�ܣ�
����:
filename   ��������Ӧ���ֳ����ļ�����
����ֵ:
MR_FAILED      ����ʧ��
MR_SUCCESS  �����ɹ�
*/
//int32 mrc_saveVM(char* filename);
/*
�����ָ�Ӧ���ֳ���
ע��: ��������Ὣ��ǰģ����ص�screen buf�У������������
֮�󣬲��ܶ�screen buf����д������Ŀǰ�Լ��ص�screen buf������
Ӳ���룬��ǰģ�鲻Ҫ����20K��
����ֵ:
MR_FAILED      ����ʧ��
MR_SUCCESS  �����ɹ�
*/
//int32 mrc_loadVM(void);

/*************************will be delete************************************/


/*
��buf��len�е�windows bmp 256ɫ��ʽ��ͼƬ����(sx, sy)��ʼ��
���Ϊw, h�����򣬻��Ƶ�ͼƬ����di��(x, y)��ʼ�ĵط���

����:
di               Ŀ��ͼƬ�������
buf,len        ָ�򱣴�windows bmp 256ɫ��ʽ��ͼƬ���ڴ�
w,h             ����������Ŀ��
x,y             ��������������Ļ�ϵ���ʼλ��
sx,sy          ������������ԴͼƬ����ʼλ��

����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ��
*/
//int32 mrc_bmpBufferDrawold(uint16 di, uint8* buf,  int32 len, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);

/*
��mrp����Ϊfilename��windows bmp 256ɫ��ʽ��ͼƬ����(sx, sy)��
ʼ�Ŀ��Ϊw, h�����򣬻��Ƶ���Ļ��(x, y)��ʼ�ĵط���

��mythroad�����У���Ϊ"*.bmp"���ļ�������Ϊ16Bit��24Bitɫ��
windows bmp��ʽ�����Ըýӿ�ʹ�õ�ͼƬ�ļ�������Ҫ��
��׺�޸�Ϊ��"*.bmp"����������޸�Ϊ"*.bm"��


����:
filename     �ļ���
w,h             ����������Ŀ��
x,y             ��������������Ļ�ϵ���ʼλ��
sx,sy          ������������ԴͼƬ����ʼλ��

����:
      MR_SUCCESS     �ɹ�
      MR_FAILED         ʧ��
*/
//int32 mrc_bmpShowold(char* filename, int32 x, int32 y, int32 w, int32 h, int32 sx, int32 sy);

/* ���Ժ���,���cond����,���ӡ�����ļ������к� */
void mrc_assert(int32 cond,const char* fileName,int32 nLine);

/* ���� */
#define MRC_ASSERT(cond)  mrc_assert((cond),__FILE__,__LINE__);


/*�õ���ջ��Ϣ,��ջ��ַ�ǴӸߵ���
    ����:
	   stackBegin:  ջ����ַ
	   stackLen:    ��ջ����
	   stackCurPos: ��ǰ��ջ��ַ
    ����:
	   MR_IGNORE: �޷�ȡ�ö�ջ��Ϣ 
*/
//int32 mrc_getStackInfo(int32 *stackBegin,int32 *stackLen,int32 *stackCurPos);
/*************************will be delete************************************/


#endif
