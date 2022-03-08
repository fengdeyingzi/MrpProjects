#ifndef SKY_SDK_EXB_H
#define SKY_SDK_EXB_H

#include "mrc_base.h"


#define MRC_PLAT_VALUE_BASE 1000
void   mrc_sand(uint32   seed);
uint32   mrc_rand(void);


  #define abs(VAL) (((VAL)>0)?(VAL):(-(VAL)))

/*
����:
	�򿪡��ر�LCD��ʱ���߹��ܡ�
	
����:
	char CanSleep=1 ʱ������LCD��ʱ���ߣ�
	char CanSleep=0ʱ��������LCD��ʱ���ߡ�

�����
MR_SUCCESS ���ɹ�
MR_FAILED  ��ʧ��
MR_IGNORE  : ��֧�ָù���
*/
int32 mrc_LCDCanSleep(char CanSleep);

/*
������:
					mrc_GetDiskFreeSpaceEx:
���ܣ�
					���ָ���̷��Ŀռ���Ϣ��

���룺
					lpDirectoryName��Ŀ������ϵ�һ��Ŀ¼����ΪNULL��ʾ��ǰĿ¼��
					Ŀǰ���Ʊ���Ϊ"C"�����ֻ��ϵ�T����
�����
					TotalKB	�ô����ܵ���������λΪKB��
					FreeKB	�ô��̿�����������λΪKB��
����ֵ:	
					���̷������ֽ�������ʧ�ܻ�����ֽ���Ϊ0ʱ������0��
*/
uint32 mrc_GetDiskFreeSpace32(
  uint8 *lpDirectoryName,                 // pointer to the directory name
  uint32 *TotalKB, // receives the number of bytes on disk
  uint32 *FreeKB // receives the free bytes on disk
);



/*
��������:		mrc_strrchr

��������:		��һ���ַ���������һ���ַ������ֵ�λ�á�

�������:		string����NULL��β���ַ�����
								c����Ҫ���ҵ��ַ���

����ֵ:			���������ַ��ڴ��������ֵ�λ��ָ�룬��û���ҵ���
								����NULL��
*/
char * mrc_strrchr (
        const char * string,
        int32 ch
        );




typedef enum {
   MRP_ORICODING,//ֱ�Ӷ�ȡ���롣ֻ��˹������Ȩ�û�����ִ�и����Ͳ�����
   MRP_FILENAME,//MRP�ļ����ƣ����11����Ч�ַ���������'\0'
   MRP_APPNAME,//MRPӦ�����ƣ��û��ɼ���Ӧ���������23����Ч�ַ���������'\0'������"�׵�1944"��
   MRP_APPID,//ÿ��Ӧ�ö�Ӧһ��Ψһ��ID��uint32���͡�
   MRP_APPVER,//uint32����   
   MRP_VENDER,//��������Ϣ�����39����Ч�ַ���������'\0'��
   MRP_DESCRIPTION,   //Ӧ��������Ϣ �����63����Ч�ַ�
   MRP_VERSION,//�Ʒ�ͨ���汾���,uint16���ͣ������ֽڡ�
   MRP_CHANNEL,//�Ʒ�ͨ�����,�����ֽڣ��͵�ַ��RecBuf[0]���ƶ����ߵ�ַ��RecBuf[1]����ͨ��
   MRP_CTRCHN,
   MRP_NONE
}E_MRP_INFOID;

/*
��������:		mrc_GetMrpInfo

��������:		�ӵ�ǰMRP�ļ��ж�ȡͷ��Ϣ��

�������:		CMD�����������롣
								RecBuf�����շ������ݵĻ�����ָ�룻
								BufLen�����շ������ݵĻ������ֽ�����

����ֵ:			��������֧�֡�����ʧ�ܵ����ʱ����������ֵС�ڵ���0��
								�������ɹ�����������ֵΪRecBuf��ʵ����д���ֽ�����
*/
int32 mrc_GetMrpInfo(char *MrpName,E_MRP_INFOID CMD,uint8 *RecBuf,int32 BufLen);

/*************************************************************************
  Function:       mrc_tell
  Description:    �õ���ǰ�ļ���дָ���λ�á��ο�MSDN��_tellʵ�֡�
  Input:          aFileHandle:�ļ����
  Output:         
  Return:         �����������ǰ�ļ���дָ�롣����ֵС��0,��ʾ��������
  Others:         None
��ע:
  1���˺���Ŀǰֻ����ģ������ʹ�ã����ֻ���ʹ�ý�����MR_FAILED(-1)��
  2��Ŀǰ���ṩ32bit�汾������ȡ�ļ����ȳ���2GB�����������
  
**************************************************************************/
int32 mrc_tell(int32 aFileHandle);

//��һ���޷���32bit�����������ֽ���(little-endian)ת��Ϊ�����ֽ���(big-endian)��
uint32 mrc_htonl(uint32 hostlong);
//��һ���޷���32bit�����������ֽ���(big-endian)ת��Ϊ�����ֽ���(little-endian)��
uint32 mrc_ntohl(uint32 netlong);

/*************************************************************************
  Function:       mrc_BCDEncode
  Description:    �������ASCII���ִ�����BCDѹ�����롣
  Input:          char *in	:ASCII���ִ�;
  						char *out :����ַ��������������������߸��𴴽��û�����������
  						���벻����mrc_strlen(in)/2+mrc_strlen(in)%2�����⣬������޴�������־��
  Output:        char *out	:BCD�����ַ�����
  Return:         ���������*in��*out������NULL���򷵻�out��ָ�룬���򷵻�NULL��
  Others:         None
**************************************************************************/
char* mrc_BCDEncode(char *in,char *out);

/*
��������:mrc_BCDDecode:
	���ܼ��:��ѹ��BCD��������ִ����������
����:
	char *bcd:	ѹ��BCD��������ִ���
	int32 len:	ѹ��BCD��������ִ����ȣ�

���:	
	char *output:	����ַ������ÿռ��ɵ����߸���������ͷţ������������������ͷ��ڴ档	
����ֵ:
	�ɹ�:����ָ��output��
	ʧ��:����NULL
*/
char *mrc_BCDDecode(char *bcd,int32 len,char *output);

//����ͬmrc_getPackName(void);
char* mrc_getPackNameMr(void);

//����ͬ int32 mrc_setPackName(char * name);
int32 mrc_setPackNameMr(char * name);

/*
��������:GetCurLanguageID:
���ܼ��:��ȡ�ֻ��ĵ�ǰ��������
����:
		�ޣ�
		
���:

���أ�
MR_PLAT_VALUE_BASEֵ��1000
MR_PLAT_VALUE_BASE + 0 �����ļ�1000
MR_PLAT_VALUE_BASE + 1 ��Ӣ�ļ�1001
MR_FAILED              ����ȡʧ��
MR_IGNORE              ����֧�ָù���

*/
int32 GetCurLanguageID(void);


/*
��������:md5MakeFromMem
���ܼ��:���ڴ��е����ݼ�����Կ�����MD5��
����:
char *key:	��Կ������������Կ����ΪNULL���ɡ���������Կ��
					����Կ������һ�����MD5
uint8* in:		������ָ�롣
int32 offset:	��in+offset�ĵ�ַƫ�ƿ�ʼ��len���ֽڵ����ݼ���MD5��
					���offset<0������ʧ�ܡ�
int32 len:		��Ҫ��������������ֽ���������ֵС��0������ʧ�ܡ�
					���len=0����key!=NULL���򽫷���key��MD5��
					����len==0 && key==NULL�����ص�MD5ֵ������MD5�����ľ���ʵ�֡�

���:
uint8* out:	����������MR_SUCCESS����ͨ��out����16���ֽڵ�MD5У��ֵ��
					��16���ֽڵ��ڴ��ɵ��������롢�ͷš�

���أ�
MR_SUCCESS:MD5��ȡ�ɹ���
MR_FAILED    :��ȡʧ�ܣ���ʱout�е�ֵ�ǲ�ȷ���ģ�������ΪMD5ֵʹ�á�
*/
int32 md5MakeFromMem(char *key,uint8* in,int32 offset , int32 len, uint8* out);


/*
��������:md5MakeFromFileHandle
���ܼ��:���ļ��е����ݼ�����Կ����MD5��
����:
char *key:	��Կ������������Կ����ΪNULL���ɡ���������Կ��
					����Կ������һ�����MD5��
int32 fhandle:	��Ҫ����MD5���ļ������
int32 offset:	���ļ�ͷ��offset�ֽڵĵ�ַƫ�ƿ�ʼ��len���ֽڵ����ݼ���MD5��
					���offset<0������ʧ�ܡ�
int32 len:		��Ҫ��������������ֽ�����
				(1)������ֵС��0������ʧ�ܡ�
				(2)�����len=0����key!=NULL���򽫷���key��MD5������
							��len==0 && key==NULL�����ص�MD5ֵ������MD5�����ľ���ʵ�֡�
				(3)��������ʱ����ͨ���ļ������ȡ�ļ����ȣ����Խ���
							����9�ζ�ȡʧ�ܻ�������9�ζ���0���ֽ�ʱ����ʧ�ܣ�
							��(len+offset)>filelenʱͨ��������ʧ�ܡ�
���:
uint8* out:	����������MR_SUCCESS����ͨ��out����16���ֽڵ�MD5У��ֵ��
					��16���ֽڵ��ڴ��ɵ��������롢�ͷš�

���أ�
MR_SUCCESS:MD5��ȡ�ɹ���
MR_FAILED    :��ȡʧ�ܣ���ʱout�е�ֵ�ǲ�ȷ���ģ�������ΪMD5ֵʹ�á�
*/
int32 md5MakeFromFileHandle(char *key,int32 fhandle,int32 offset , int32 len, uint8* out);
/*
��������:md5MakeFromFile
���ܼ��:���ļ��е����ݼ�����Կ����MD5��
����:
char *key:	��Կ������������Կ����ΪNULL���ɡ���������Կ��
					����Կ������һ�����MD5
char *filename:	��Ҫ����MD5���ļ�����
int32 offset:	���ļ�ͷ��offset�ֽڵĵ�ַƫ�ƿ�ʼ��len���ֽڵ����ݼ���MD5��
					���offset<0������ʧ�ܡ�
int32 len:		��Ҫ��������������ֽ�����
				(1)������ֵС��0������ʧ�ܡ�
				(2)�����len=0����key!=NULL���򽫷���key��MD5������
							��len==0 && key==NULL�����ص�MD5ֵ������MD5�����ľ���ʵ�֡�
				(3)�����(offset+len)>�ļ����ȣ���ȡ���ļ�ĩβΪֹ������������
					
���:
uint8* out:	����������MR_SUCCESS����ͨ��out����16���ֽڵ�MD5У��ֵ��
					��16���ֽڵ��ڴ��ɵ��������롢�ͷš�

���أ�
MR_SUCCESS:MD5��ȡ�ɹ���
MR_FAILED    :��ȡʧ�ܣ���ʱout�е�ֵ�ǲ�ȷ���ģ�������ΪMD5ֵʹ�á�
*/
int32 md5MakeFromFile(char *key,char *filename,int32 offset , int32 len, uint8* out);


/*
��������:md5ToString
���ܼ��:��16�ֽڵ�MD5У��ֵת��Ϊ32�ֽڵ�ASCII�ɼ��ַ�����
����:
uint8 *in:	MD5У��ֵ��
���:
uint8* out:	����������MR_SUCCESS����ͨ��out����32���ֽڵ�MD5У��ֵ��
					��32���ֽڵ��ڴ��ɵ��������롢�ͷš�

���أ�
MR_SUCCESS:MD5��ȡ�ɹ���
MR_FAILED    :��ȡʧ��
*/
int32 md5ToString(uint8* in, uint8* out,  int32 inBufLen, int32 outBufLen);



/*
���ٴ�����ʹ��˵��
ʹ�ò��衣
1��	���ü��ٴ������������������ MR_IGNORE ��ô�Ͳ�֧�֡�
2��	���óɹ��Ժ��������ٴ������������ɹ��Ժ󣬾Ϳ�����mr_event()
�н��������ˡ�
3��	mr_event()��
param1  MR_MOTION_TITLE�¼��Ժ��ʾ�н��յ����ٴ��������¼��ˡ�
Param2  ��ʾx��y�����ƫ�ƣ�xռ�õ�λ16bit��yռ�ø�λ��16bit��
Param3  ��ʾz����ƫ��
Ӧ�ó���ֻ��Ҫ����Щ���ݽ��д���Ϳ��ԡ�
4��	ֹͣ���ٴ�������Ӧ��ʹ�ý����Ժ�ֹͣ��������

�ر�ע����
��������Ҫ����direct�����򣩡�shark���𶯣���ת���Ƕȷ������󣬿���ͨ�����Ӷ�x
��y��z��Щԭʼ���ݴ����Ժ�õ���
*/



typedef enum
{
    MR_MOTION_SENSITIVE_LOW,
    MR_MOTION_SENSITIVE_NORMAL,
    MR_MOTION_SENSITIVE_HIGH,

    NUM_OF_MDI_MOTION_SENSITIVE
} MR_MOTION_SENSITIVE_ENUM;//�����������жȡ�ö�����͡�

typedef struct 
{
	uint8 isStart;
	int32 key;//0:IDLE,2:up,4:left;6:right;8:down
	int16 x;
	int16 y;
	int32 maxRange;
}CTRL_MOTION2KEY_T;
int32 mrc_motion2KeyStart(int32 sensitive);
int32 mrc_motion2Key(CTRL_MOTION2KEY_T *ctrlMotion2key,int32 *code,int32 *param0,int32 *param1);
int32 mrc_motion2KeyStop(void);

typedef enum
{
    MOTION_SENSITIVE_LOW,
    MOTION_SENSITIVE_NORMAL,
    MOTION_SENSITIVE_HIGH
}MOTION_SENSITIVE_T;

typedef enum
{
	MOTION_SHAKE_NO_DIRECT,
	MOTION_SHAKE_LEFT,
	MOTION_SHAKE_RIGHT,
	MOTION_SHAKE_FORWARD,
	MOTION_SHAKE_BACKWARD
}MOTION_SHAKE_T;

typedef enum
{
	MOTION_EVENT_SHAKE,
	MOTION_EVENT_TILT
}MOTION_EVENT_T;

typedef struct
{
	int32 x;
	int32 y;
	int32 z;
}T_MOTION_TILT;

#define MR_MOTION_STOP_LISTEN               4001
#define MR_MOTION_SENOR_PWRON 		4002
#define MR_MOTION_SENOR_PWROFF 		4003
#define MR_MOTION_LISTEN_SHAKE             4004
#define MR_MOTION_LISTEN_TILT                4005
#define MR_MOTION_SENSOR_MAXRANGE	4006

//�򿪴�����
int32 mrc_motionSensorPowerOn(void);

//�رմ�����
int32 mrc_motionSensorPowerOff(void);

//��ͣ���մ������¼�
int32 mrc_motionSensorStopListen(void);

//�����ζ��¼�
int32 mrc_motionSensorStartShakeListen(int32 sensitive);

//��������¼�
int32 mrc_motionSensorStartTiltListen(int32 sensitive);

/*��ȡ���ز�����ȡֵ��Χ�����ֵMAXRANGE��
�ɹ�:
	����MR_PLAT_VALUE_BASE+MAXRANGE�������Ҫ�����ս����ȥ1000(MR_PLAT_VALUE_BASE��ֵ)
ʧ��:
	����MR_FAILED;
��֧��:
	����MR_INGORE;
*/
int32 mrc_motionSensorGetRange(void);


/*
������:mrc_unicodeToGb2312
���룺
��unicode������ַ���ת��gb2312����
input����Ҫת����unicode�ַ���
input_len:��Ҫת����unicode�ַ������ȣ���λ�ֽ���
output��ת���ɹ��Ժ��bg2312�����ַ�����Ż�������
���������ڴ���Ӧ�õ������ṩ�������ͷš�
ע�⣺���������������ʹ��malloc���䴦��Ŀռ䣬�����ֻ����ܻ����������
output_len��output�������ĳ��ȣ���λ�ֽ���

���أ�
    MR_SUCCESS	ת���ɹ�
    MR_FAILED   ת��ʧ��

*/
int32 mrc_unicodeToGb2312(uint8* input, int32 input_len, uint8** output, int32* output_len);


/*
������:mrc_getRegAppAddr
	��ȡ�̻�Ӧ�á��ڴ澵��Ӧ�õ�����ʱ���ص�ַ��
	Ӧ�����ƿ���ͨ��mrc_getPackName()��ȡ��
���룺
char *MrpName:	Ӧ�����ƣ��̻�Ӧ�ö���"*"��ͷ	�ģ�RAMӦ�ö���"$"��ͷ�ģ�

���أ�
NULL:��ȡʧ�ܣ�
����ֵΪӦ���ڴ��ַ��
*/
int32 mrc_getRegAppAddr(char *MrpName);



//-----------------------¼������-------------------------------------
enum
{
	DSM_ERROR_NO_ERROR =0,
	DSM_ERROR_UNKNOW,
	DSM_ERROR_NO_SPACE,
	DSM_ERROR_TERMINATED
};

typedef enum
{
   SRC_NAME = 0,    //������ֲ�ӿ����ļ���
   SRC_STREAM   //������ֲ�ӿڵ���ͼƬ������
}VOICE_REC_TYPE;

typedef struct{
    char *src;       		//¼���ļ�������ļ�����������streamģʽ�¿�ΪNULL
    int32 src_len;		//������streamģʽʱ���Ըò�����
    int32 src_type;	//�μ�VOICE_REC_TYPE����Ϊ SRC_STREAM ��ʾʵʱ��ȡ������ģʽ��
    int32 format;		//MR_SOUND_WAV , MR_SOUND_PCM , MR_SOUND_AMR , MR_SOUND_AMR_WB 
}VOICE_REC_REQ_T;

typedef void (*VOICE_REC_CB)(uint8* output, int32 output_len);

/*
������:mrc_voiceRecStart



���룺
VOICE_REC_REQ_T *voiceCtl

���أ�
MR_SUCCESS	: 	����ɹ�
MR_FAILED  		:	����ʧ��
MR_IGNORE  	:	��֧�ָù���

ע��
¼�����쳣���ͨ��mr_event֪ͨ��mr_event�Ĳ������£�
��һ����MR_ERROR_EVENT
�ڶ�����DSM_ERROR_NO_SPACE	���ռ���
        				DSM_ERROR_TERMINATED�����ж�
        				DSM_ERROR_UNKNOW		��δ֪����
��������0
*/
int32 mrc_voiceRecStart(VOICE_REC_REQ_T *voiceCtl);

/*
������:mrc_voiceRecStreamProc
	stream¼��ģʽ�»�ȡ��ǰ��¼���ݣ�����ʹ���߿��Կ�������ʮ����Ķ�ʱ����ʱ��ѯ����ȡ���������ݡ�
���룺
	�ޣ� 
���:
	*output  ����ǰ¼�����ݶ�ȡ��ַ��
    *output_len  ��¼�����ݳ��ȣ�
	MR_PLAT_EX_CB *cb �� ��output_len>0 ���� Ӧ�ô�����������ַ�е�����ʱ��������ô˻ص����ƶ��豸����Ķ�ָ�롣

���أ�
MR_SUCCESS	: 	����ɹ�
MR_FAILED  		:	����ʧ��
MR_IGNORE  	:	��֧�ָù���

ע��:
	��ʹ������ʽ¼����ʹ��WAV��ʽ����Ҫ��¼����������Ӧ�����wavͷ��Ϣ�������޷�������
*/
int32 mrc_voiceRecStreamProc(uint8 **output,int32 *output_len,VOICE_REC_CB *cb);

/*
������:mrc_voiceRecPause
	��ͣ¼��
���룺
	�ޡ� 
���:
	�ޡ�
���أ�
MR_SUCCESS	: 	����ɹ�
MR_FAILED  		:	����ʧ��
MR_IGNORE  	:	��֧�ָù���
*/
int32 mrc_voiceRecPause(void);

/*
������:mrc_voiceRecResume
	�ָ�¼��
���룺
	�ޡ� 
���:
	�ޡ�
���أ�
MR_SUCCESS	: 	����ɹ�
MR_FAILED  		:	����ʧ��
MR_IGNORE  	:	��֧�ָù���
*/
int32 mrc_voiceRecResume(void);

/*
������:mrc_voiceRecStop
	ֹͣ¼��
���룺
	�ޡ� 
���:
	�ޡ�
���أ�
MR_SUCCESS	: 	����ɹ�
MR_FAILED  		:	����ʧ��
MR_IGNORE  	:	��֧�ָù���
*/
int32 mrc_voiceRecStop(void);



/*
������:mrc_isTouchSupport
	���ܣ���ȡ�ֻ��Ƿ��Ǵ�������Ϣ��
	���룺
		  ��
	����ֵ��
	1000 ���Ǵ������ֻ�������˵������Ϸ�в��ó��ִ�������ص���ʾ��
	1001 ���д������ֻ���һ��Ҳ�����ּ�������ͬʱ���ִ������Ͱ�����ʾ��
	1002 �����������ֻ�������˵������Ϸ�в��ó��ְ�����������ʾ��
	MR_FAILED	����ȡʧ�ܡ�
	MR_IGNORE	����֧�ֻ�ȡ��������Ϣ��

ע�⣬����ֱ��ʹ��MR_SUCCESS�ж��Ƿ�֧�ִ�������
*/
int32 mrc_isTouchSupport(void);



int32 mrc_writeBigEndianU32(uint32 data,uint8 *p);

int32 mrc_writeBigEndianU16(uint16 data,uint8 *p);

uint32 readBigEndianUint32(uint8 *p);

uint16 readBigEndianUint16(uint8 *p);

uint32 readLittleEndianUint32(uint8 *p);

uint16 readLittleEndianUint16(uint8 *p);

typedef struct
{
	uint8       IMEI[16];	     //IMEI ����Ϊ15�ֽ�
	uint8       IMSI[16];	     //IMSI ����Ϊ15�ֽ�
	char        manufactory[8];  //�ֻ���ƹ�˾�������7���ַ������ֽ���\0
	char        type[8];         //�ֻ��з���Ŀ����mobile type�����7���ַ������ֽ���\0
	uint32      ver;             //��ֲ����汾
	uint8       spare[12];       //����
}MRC_PHONEINFO_T;
/*
������:mrc_getphoneInfo
	���ܣ���ȡ�ֻ��ĳ��̻�����Ϣ��
	���룺
		  ��
	���:
			MRC_PHONEINFO_T* phoneInfo;���е�IMSI������α�룬ֻ����ʵ��IMSI��ӳ�䡣
	���أ�
	MR_SUCCESS :	��ȡ�˹�����
	MR_FAILED	����ȡʧ�ܡ�
	MR_IGNORE	����֧�ֻ�ȡ��������Ϣ��

*/

int32 mrc_getphoneInfo(MRC_PHONEINFO_T* phoneInfo);



/*
������:mrc_getSdkVer
	���ܣ���ȡSDK�Ʒ�ģ��汾�š�
	���룺
		  ��
	���أ�
		SDK�İ汾�š��汾��ΪAABBCCCDDD��ʽ��int32���������У�
		AAΪ���λ�汾�ţ�
		BBΪ�θ�λ�汾�ţ�
		CCC���ӹ��ܰ汾�ţ�ÿ���д�Ĺ��ܱ仯�����޸��ӹ��ܰ汾�š�
		DDD��Build�汾�ţ�ÿ�θĶ�����֮�󣬶����޸�Build�汾�š�

*/

int32 mrc_getSdkVer(void);

typedef struct
{
	char       IMEI[16];	     //IMEI ����Ϊ15�ֽڣ����һ���ֽ���'\0'���
	char       IMSI[16];	     //IMSI ����Ϊ15�ֽڣ����һ���ֽ���'\0'���
	char       manufactory[8];  //�ֻ���ƹ�˾�������7���ַ������ֽ���\0
	char       type[8];         //�ֻ��з���Ŀ����mobile type�����7���ַ������ֽ���\0
	unsigned int     ver;             //��ֲ����汾
	unsigned char    spare[12];       //����
}MR_PHONEINFO_T;

typedef void (*MR_PLAT_EX_CB)(uint8* output, int32 output_len);
extern int32 mrc_platExCP(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
extern int32 mrc_platCP(int32 code, int32 param);

/*
��������:	mrc_UserInfoMD5
����˵��:	���û���Ϣ��MD5ֵ���ظ�Ӧ�á�

����:
Option:	0���û��ֻ�IMEI��MD5ֵ��
				1���û�SIM����IMSI�����MD5ֵ��
				3���û�SIM����IMEI��MD5ֵ��

char *out:	�û�����MD5����Ļ�������33���ֽڣ�
						���������û�����������ͷ��ڴ�ռ䣬��NULL��β��
���:
	����õ���MD5����32��ASCII�ַ�����
MR_SUCCESS:����MD5�ɹ���
MR_FAILED:����MD5ʧ�ܡ�
*/
int32 mrc_UserInfoMD5(uint32 Option,char *out);


/**************************************************************
��������:���ñ��ص绰�����ָ������Ķ��ŷ���

��������:int32 mrc_SendSms(T_SENDSMS_REQ *sendSms);

����˵��:  result  0:���ͳɹ�   1:��ȡ�绰��ʧ��    2:���Ͷ���ʧ��   3:Ŀ�����ΪNULL     4:�û�δѡ����5:�绰������ʧ��  

**************************************************************/

typedef struct
{
	char *pNum;/*�ò�����������*/
	char *pContent;/*unicode*/
}T_PLAT_SENDSMS_REQ;

typedef void (*SmsSendResultCB)(int32 result);
typedef enum
{
	SMSSEND_SUCCESS = 0,
	SMSEND_ERROR_CREADPBOBJ = 1,
	SMSEND_ERROR_SENDSMS = 2,
	SMSEND_ERROR_PHONENUM = 3,
	SMSEND_ERROR_UNSELNUM = 4,
	SMSEND_ERROR_READRECORD = 5,
	SMSEND_ERROR_DESTORYPBOBJ = 6
}E_SMSEND_CODE;

typedef struct
{
	char *pContent;/*unicode*/
	SmsSendResultCB    sendResultCB; //���ŷ��ͻص�	
}T_SENDSMS_REQ;

int32 mrc_SendSms(T_SENDSMS_REQ *sendSms);

/*
��������  mrc_getTCardState()
����:	  ��ѯT��״̬
����:	  ��
����ֵ:	  ��MSDC_STATEö�ٶ���
			typedef enum
			{
			   MR_MSDC_NOT_EXIST,  		//No MSDC
			   MR_MSDC_OK,			 	//MSDC Exist
			   MR_MSDC_NOT_USEFULL 		//MSDC BUSY
			}MSDC_STATE;
*/
int32 mrc_getTCardState(void);

/*
��������:mrc_lauchMrp
����:����˹��DSM����ָ��Ŀ¼�µ�MRP
����:mrp_name   ָ������MRP������·���������Mythroad��Ŀ¼
����ֵ:MR_FAILED 		Ӧ�ö����ƴ����DSM���þܾ�
		    MR_SUCCESS           Ŀ��MRP������������ǰ���е�MRP���յ�mrc_exitApp��
*/
int32  mrc_lauchMrp(char * mrp_name);
/*
��������:mrc_GetParentPath
����:��ȡ��ǰ����MRP���ڸ�Ŀ¼�����ص�BUFFָ�����ʱ�����Ա�ֶ��ͷ�
����:void
����ֵ:char*  ��Ŀ¼·��ָ��
		    NULL  ��ȡʧ��
*/
char* mrc_GetParentPath(void);

/*
��������:
��������:��ȡMRP�浵�ļ��Ĵ���Ŀ¼
����:void
����ֵ :char* MRP�浵�ļ��Ĵ���Ŀ¼�����ص�BUFFָ�����ʱ�����Ա�ֶ��ͷ�
		    NULL  ��ȡʧ��
*/
char * mrc_getMrpResPath(void);

/*
��������:char* mrc_getUAInfo(void);
����:��ȡ��ǰ�ֻ��ն˵�UA��Ϣ����:���̡����͡���Ļ��ƽ̨��
����:void
����ֵ:char* �ύ˹����֤��UA����
*/
int32 mrc_getUAInfo( char ** outUaData, int32 *outUaLen);

/*
��������:mrc_Cellinfo_Init
����:��վС����Ϣ��ʼ������mrc_Cellinfo_Release��mrc_Cellinfo_Get���ʹ��
����:��
����ֵ:MR_SUCCESS�ɹ�  MR_FAILED  ʧ��  MR_IGNORE���Բ�֧�ָù��ܡ�
*/
int32 mrc_Cellinfo_Init(void);
/*
��������:mrc_Cellinfo_Release
����:��վС����Ϣ��Դ�ͷţ���mrc_Cellinfo_Release��mrc_Cellinfo_Get���ʹ��
����:��
����ֵ:MR_SUCCESS�ɹ�  MR_FAILED  ʧ��  MR_IGNORE���Բ�֧�ָù��ܡ�
*/
int32 mrc_Cellinfo_Release(void);
/*
��������:mrc_Cellinfo_Get
����:��վС����Ϣ��Դ��ȡ����mrc_Cellinfo_Release��mrc_Cellinfo_Get���ʹ��
����:PT_DSM_CELL_INFO  ���ṹ����,����ʱ�������ռ䣬�ռ�Ҳ�����ͷš�
����ֵ:MR_SUCCESS�ɹ�  MR_FAILED  ʧ��  MR_IGNORE���Բ�֧�ָù��ܡ�
*/
typedef struct t_cellInfo
{
	uint16 lac;        
	uint16 cell_id;
	uint8 mnc[2];
	uint8 mcc[3];
	uint8 mnc3[4];/*��ֲ�汾x15x�����ӵ��ֶΣ�mnc3[0]�б�ʾ��mnc�ĵ���λ*/
}T_DSM_CELL_INFO, *PT_DSM_CELL_INFO;
int32 mrc_Cellinfo_Get(PT_DSM_CELL_INFO *cellInfo);

/*****
Func:
		get the "IMSI" string of sim card
Param:
		void
Return:
		NULL as failed; 
           	not NULL as Success,please remember to free the point,for string memeory is malloc in the function.
Example:
		char * imsi = mrc_GetPhoneInfo_IMSI();
		if(NULL == imsi)
		{
			mrc_printf("IMSI = NULL");
		}
		else
		{
			mrc_printf("IMSI = %s", imsi);
			mrc_free(imsi);
		}
*****/
char* mrc_GetPhoneInfo_IMSI(void);


//base64 �������
enum {
   CRYPT_OK=0,             /* Result OK */
   CRYPT_ERROR,            /* Generic Error */
   CRYPT_NOP,              /* Not a failure but no operation was performed */

   CRYPT_INVALID_KEYSIZE,  /* Invalid key size given */
   CRYPT_INVALID_ROUNDS,   /* Invalid number of rounds */
   CRYPT_FAIL_TESTVECTOR,  /* Algorithm failed test vectors */

   CRYPT_BUFFER_OVERFLOW,  /* Not enough space for output */
   CRYPT_INVALID_PACKET,   /* Invalid input packet given */

   CRYPT_INVALID_PRNGSIZE, /* Invalid number of bits for a PRNG */
   CRYPT_ERROR_READPRNG,   /* Could not read enough from PRNG */

   CRYPT_INVALID_CIPHER,   /* Invalid cipher specified */
   CRYPT_INVALID_HASH,     /* Invalid hash specified */
   CRYPT_INVALID_PRNG,     /* Invalid PRNG specified */

   CRYPT_MEM,              /* Out of memory */

   CRYPT_PK_TYPE_MISMATCH, /* Not equivalent types of PK keys */
   CRYPT_PK_NOT_PRIVATE,   /* Requires a private PK key */

   CRYPT_INVALID_ARG,      /* Generic invalid argument */
   CRYPT_FILE_NOTFOUND,    /* File Not Found */

   CRYPT_PK_INVALID_TYPE,  /* Invalid type of PK key */
   CRYPT_PK_INVALID_SYSTEM,/* Invalid PK system specified */
   CRYPT_PK_DUP,           /* Duplicate key already in key ring */
   CRYPT_PK_NOT_FOUND,     /* Key not found in keyring */
   CRYPT_PK_INVALID_SIZE,  /* Invalid size input for PK parameters */

   CRYPT_INVALID_PRIME_SIZE,/* Invalid size of prime requested */
   CRYPT_PK_INVALID_PADDING /* Invalid padding on input */
};

/*
MD5������
*/
//  ��MD5�㷨ȡɢ��ֵ
//  ����:	szSour Դ�ַ���
//			iLen   Դ�ַ�������
//			szDest Ŀ�Ĵ�(16�ֽ�)
#ifndef BYTE
typedef unsigned char   BYTE;
#endif

void mrc_md5(BYTE *szSour, int iLen, BYTE *szDest);//16���ֽڵ�������ֵ
void mrc_md5ToString(BYTE *md5Value, char*strMd5, int32 strMd5Len);//32���ֽڵ��ַ���ֵ

int32 mrc_base64_encode(uint8 *in,  int32 inlen, uint8*out, int32 *outlen);

typedef enum{

	MR_FMGR_TYPE_ALL = 1,
	MR_FMGR_TYPE_FOLDER,
	MR_FMGR_TYPE_FOLDER_DOT,
	MR_FMGR_TYPE_UNKNOW,
	// image 
	MR_FMGR_TYPE_BMP,
	MR_FMGR_TYPE_JPG,
	MR_FMGR_TYPE_JPEG,
	MR_FMGR_TYPE_GIF,
	MR_FMGR_TYPE_PNG,
	MR_FMGR_TYPE_EMS,
	MR_FMGR_TYPE_ANM,
	MR_FMGR_TYPE_WBMP,
	MR_FMGR_TYPE_WBM,
	// audio 
	MR_FMGR_TYPE_IMY,
	MR_FMGR_TYPE_MID,
	MR_FMGR_TYPE_MIDI,
	MR_FMGR_TYPE_WAV,
	MR_FMGR_TYPE_AMR,
	MR_FMGR_TYPE_AAC,
	MR_FMGR_TYPE_DAF,
	MR_FMGR_TYPE_VM,
	MR_FMGR_TYPE_AWB,
	MR_FMGR_TYPE_AIF,
	MR_FMGR_TYPE_AIFF,
	MR_FMGR_TYPE_AIFC,
	MR_FMGR_TYPE_AU,
	MR_FMGR_TYPE_SND,
	MR_FMGR_TYPE_M4A,
	MR_FMGR_TYPE_MMF,
	MR_FMGR_TYPE_WMA,
	//video
	MR_FMGR_TYPE_3GP,
	MR_FMGR_TYPE_MP4,
	MR_FMGR_TYPE_AVI,
	//others 
	MR_FMGR_TYPE_JAD,
	MR_FMGR_TYPE_JAR,
	MR_FMGR_TYPE_VCF,
	MR_FMGR_TYPE_VCS,
	MR_FMGR_TYPE_THEME,
	MR_FMGR_TYPE_MRP,
	MR_FMGR_TYPE_NES,
	MR_FMGR_TYPE_ZIP

}FMGR_FILE_TYPE;
typedef struct 
{
	int32  dft_list;
	char   list_path[260];  //gb����·��
}MR_T_FMGR_REQ;

/*
��������:�����ļ���������
��������:
����:	FMGR_FILE_TYPE    filterType
		BOOL 			  isShow;		TRUE ��ʶ������ʾfilterType�����ļ�    FALSE  ���˲���ʾfileType�����ļ�
����	MR_SUCCESS ���ɹ�
		MR_FAILED  ��ʧ��
		MR_IGNORE  : ��֧�ָù���

*/
int32 mrc_setFileBrowserFilter(FMGR_FILE_TYPE fileType, int32 isShow);

/*
��������:int32 mrc_CreateFileBrowserObj(MR_T_FMGR_REQ req_type)
��������:�����ļ�������񣬿�������Ĭ�����Ŀ¼
����:	req_type ��MR_T_FMGR_REQ�ṹ�嶨�壬����dft_list�ṹ����ΪĿ¼���ͣ�ȡֵ��FMGR_FOLD_TYPEö�ٶ���
		���ҽ���dft_listȡֵΪMR_DSPL_NONEʱ���ṹ����list_pathָ���Ŀ¼·����Ч������ö�����ֻ����a��Ŀ¼·��	
����	MR_SUCCESS ���ɹ�
		MR_FAILED  ��ʧ��
		MR_IGNORE  : ��֧�ָù���
*/
int32 mrc_CreateFileBrowserObj(MR_T_FMGR_REQ req_type);

/*
��������:int32 mrc_DestoryFileBrowserObj()
��������:�������������
����:��
����	MR_SUCCESS ���ɹ�
		MR_FAILED  ��ʧ��
		MR_IGNORE  : ��֧�ָù���
*/
int32 mrc_DestoryFileBrowserObj(void);

/*
��������:int32 mrc_GetBrowserFileName(char ** fileName, int32 *nameLen)
��������:��ȡ�ļ������ѡ���ļ���·����Ϊ����·����mrc_appEvent�д���
			 MR_LOCALUI_OK:��ʾ�û�ѡ�����ļ���
			 MR_LOCALUI_CANCEL:��ʾ�û�ȡ���ļ�ѡ��
����:	fileName    �ļ������棬ͬ�ײ����洢�ռ䣬ֱ�Ӵ���ֵΪNULL��ָ�뼴�ɣ�ע��ȡ����ָ��ĵ�ַ
		nameLen		�ļ����ĳ���
����	MR_SUCCESS ���ɹ�
		MR_FAILED  ��ʧ��
		MR_IGNORE  : ��֧�ָù���
*/
int32 mrc_GetBrowserFileName(char ** fileName, int32 *nameLen);

#endif
