/*
** Copyright zengming00
**
** Create date: 2012-03-08 by zengming00
**
** Version: 1.1  ����SetRootPath�ӿ�  2022/2/13
** Version: 1.0
**
** ע����ð�ݿ���ʵ��ϵͳ�����¼��mpc����������
** Ϊ����mpc�д�����Դ�����ڼ�����е��Ի����ɶ�����
**
** mpc��ʹ�ò��裺
**     1.��� ��#include "mpc.h"��������ʱ��mpc_mtk.lib����t����thumbģʽ�ģ���vs2005����ʱ��Ŀ������������mpc_WIN32.lib��
**     2.��Դ����init��ڴ���� ��mpc_init();��ע�⣬����������init�����е���
**     3.��Դ����exitapp����� ��mpc_exit();��ע�⣬���������exitapp�����е���
**     int32 exitapp()
       {
	   //�ô���Ӧ��ִ��exit�˳��� �򱻶�ǿ���˳�ʱ, �ֻ�ϵͳ���Զ����ñ�����.���Խ��飺Ӧ��ȫ������Դ���ͷ��ڸú����ڽ��У�
         mpc_exit();
         return 0;
	   }
**
** 
*/

#ifndef _MPC_H__
#define _MPC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mrc_base.h"
#include "mrc_exb.h"
#include "mrc_sound.h"
#include "mrc_bmp.h"



#define _VERSION 1001  //��ð�ݿ���ʵ��ϵͳ���ڲ����İ汾��
#define MAKECOLOR565(color) ((uint16)((color>>8)&0xf800) |  (uint16)((color>>5)&0x07e0) | (uint16)((color>>3)&0x1f))


//ctype
#define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isdigit(c) ((c) >= '0' && (c) <= '9')
#define isalnum(c) (isalpha(c) || isdigit(c))
#define isprint(ch) ((ch) >= 0x20 && (ch) <= 0x7e)
#define tolower(c) ((c) |= 0x20)
#define toupper(c) ((c) &= 0xdf)


//base

extern int16  SCRW;//��Ļ��
extern int16  SCRH;//��Ļ��
extern int16  FONTW;//�ֻ��е��ֿ�����Ŀ�
extern int16  FONTH;//�ֻ��е��ֿ�����ĸ�


//��������ֵ��δ�������������Ҳ����ʹ�ã�����֪�����ֵ��
enum
{
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
enum
{
  KY_DOWN, 	 //��������
  KY_UP,       //�����ͷ�
  MS_DOWN, 	 //��갴��
  MS_UP, 	     //����ͷ�
  MN_SLT, //�˵�ѡ��
  MN_RET, //�˵�����
  MR_DIALOG, //�Ի���
  MS_MOVE=12   //����ƶ�
};

enum
{
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
  IS_INVALID=8  //��Ч(���ļ�����Ŀ¼)
};

//sound
enum {_MIDI=1,_WAVE,_MP3,_AMR,_PCM,_M4A,_AMR_WB};

typedef struct
{
  uint16            x;
  uint16            y;
  uint16            w;
  uint16            h;
} rectst;

typedef struct
{
  uint8            r;
  uint8            g;
  uint8            b;
} colorst;


/********************************C�⺯��********************************/

#define  printf(...)

#define strrchr mrc_strrchr

#define wstrlen mrc_wstrlen


/*******************************��ܺ���*********************************/

#define init            MRC_EXT_INIT
#define exitapp         MRC_EXT_EXIT
#define event           mrc_event
#define pause           mrc_pause
#define resume          mrc_resume

//////////////////////����Ϊƽ̨����ʵ�ֵĺ���////////////////////////

/********************************�ļ��ӿ�********************************/

#define open mrc_open

#define close mrc_close

#define filestate mrc_fileState

#define write mrc_write

#define read mrc_read

#define seek mrc_seek

#define getlen mrc_getLen

#define remove mrc_remove

#define rename mrc_rename

#define mkdir mrc_mkDir

#define rmdir mrc_rmDir

#define findstart mrc_findStart

#define findnext mrc_findGetNext

#define findstop mrc_findStop

/********************************��ͼ�ӿ�********************************/

#define dtext mrc_drawText
#define dtextex( pcText,  x,  y, rect, color,  flag,  font)\
    mrc_drawTextEx(pcText,x, y, *(mr_screenRectSt*)rect,*(mr_colourSt*)color, flag, font)

#define dtextright( pcText,  x,  y, rect, color,  flag,  font)\
    mrc_drawTextRight(pcText,x, y, *(mr_screenRectSt*)rect,*(mr_colourSt*)color, flag, font)

#define c2u(cp, err, size) mrc_c2u((char *)cp, (int32 *)err, (int32 *)size)

#define textwh(pcText, is_unicode, font,  w,  h)\
    mrc_textWidthHeight((char*) pcText, (int) is_unicode, (uint16) font, (int32*) w, (int32*) h)

#define unitextrow  mrc_unicodeTextRow

#define drect mrc_drawRect

#define dline mrc_drawLine

#define dpoint mrc_drawPoint

#define dpointex mrc_drawPointEx

#define cls mrc_clearScreen

#define ref mrc_refreshScreen

#define effsetcon mrc_EffSetCon

/********************************�����ӿ�********************************/

#define  soundinit       mrc_playSoundExInit
#define soundloadfile       mrc_playSoundExLoadFile
#define soundplay( type,  block,  loop) mrc_playSoundEx(type,block,loop,NULL)
#define soundpause       mrc_pauseSoundEx
#define soundresume        mrc_resumeSoundEx
#define soundstop         mrc_stopSoundEx
#define soundclose         mrc_closeSoundEx
#define setvolume        mrc_setVolume
#define getsoundtotaltime        mrc_getSoundTotalTime
#define getsoundcurtime          mrc_getSoundCurTime
#define getsoundcurtimems        mrc_getSoundCurTimeMs
#define setplaypos( type, pos)         mrc_setPlayPos(type,*(T_DSM_AUDIO_POS*)pos)
#define setplaytime( type, pos)         mrc_setPlayTime(type,*(T_DSM_AUDIO_POS*)pos)
#define getdevicestate        mrc_getDeviceState

/********************************���ػ�UI�ӿ�********************************/

#define menucreate mrc_menuCreate

#define menuset mrc_menuSetItem

#define menushow mrc_menuShow

#define menudel mrc_menuRelease

#define menuref mrc_menuRefresh

#define dlgcreate mrc_dialogCreate

#define dlgdel mrc_dialogRelease
#define dlgref mrc_dialogRefresh
#define textcreate mrc_textCreate
#define textdel mrc_textRelease
#define textref mrc_textRefresh
#define editcreate mrc_editCreate
#define editdel mrc_editRelease

#define editget mrc_editGetText


/********************************����ӿ�********************************/

#define initnetwork mrc_initNetwork
#define gethostbyname mrc_getHostByName
// #define socket mrc_socket
// #define connect mrc_connect
// #define getsocketstate mrc_getSocketState
// #define send mrc_send
// #define recv mrc_recv
#define closesocket mrc_closeSocket
#define closenetwork mrc_closeNetwork

/********************************�����ӿ�********************************/
#define exit  mrc_exit

#define getuptime mrc_getUptime

#define getdatetime mrc_getDatetime
#define getmemremain mrc_getMemoryRemain
#define shake mrc_startShake
#define getsysmem mrc_getSysMem

#define readfilefrommrp mrc_readFileFromMrpEx

#define freefiledata mrc_freeFileData
#define freeorigin mrc_freeOrigin

#define u2c( input,  input_len, output,  output_len)\
    mrc_unicodeToGb2312((uint8*) input, (int32) input_len, (uint8**) output, (int32*) output_len)

int32 mrc_runMrp(char* mrp_name,char* file_name,char* parameter);
#define  runmrp( mrp_name, filename) mrc_runMrp( mrp_name, filename,NULL)

#define getparentpath mrc_GetParentPath

#define sand mrc_sand
#define rand mrc_rand
extern int32 mrc_sendSms(char* pNumber, char*pContent, int32 flags);
#define sms mrc_sendSms
#define lcd mrc_LCDCanSleep
extern int32 mrc_sleep(uint32 ms);
#define sleep mrc_sleep

// ���º���������mrc_bmp.h�ļ���//////////////////////////////////////


#define getscrbuf w_getScreenBuffer
#define setscrbuf w_setScreenBuffer
#define getscrsize mrc_getScreenSize
#define setscrsize mrc_setScreenSize

#define BM_TRANS BM_TRANSPARENT

#define bmpshowflip mrc_bitmapShowFlip

extern void mrc_connectWAP(char* wap);
#define wap mrc_connectWAP

/********************************��ʱ���ӿ�********************************/

#define timercreate mrc_timerCreate

#define timerdel mrc_timerDelete

#define timerstop mrc_timerStop

#define timerstart mrc_timerStart

#define timersettime mrc_timerSetTimeEx

/*****************************����Ϊmpc��չ�ӿ�****************************/

void mpc_init(void);//���ر���

void mpc_exit(void);//�˳�ж��

#define img DrawIMG

#define shaderect DrawShadeRect

#define isPointCollRect isPointInRect

#define readall my_readAllEx

int32 my_readAllEx(char* filename, char* buf, int32 *len);//��ȡ�ļ�len���ȵ�buf��

int DrawIMG(char* filename, int x,int y);//������Ч������������ϵͳ�ӿڻ���ͼƬ

void DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode);//��������ɫ

int32 getColor(int32 color1, uint32 color2);//���������ɫ

void drawPoint(int x,int y,uint32 color);//����

int getLineSize(int x,int y, int x2,int y2);//��ȡ����֮��ĳ��� ��ƽ��

void drawRect(int32 x,int32 y,int32 w,int32 h,uint32 color);//������

void drawCir(int32 x,int32 y,int32 r,uint32 color);//��Բ

void DrawEffRect(int16 x, int16 y, int16 w, int16 h, char r, char g, char b);//��͸��Բ�Ǿ���

void toast(char *Msg, int sp);//toast��Ϣ��spΪ���룬�������sky12�ֿ⣬����ġ�˹��Mythroad ����MRP����SDK����˵��_AF01.32X2���ֿ����½�

void *readFileFromAssets(char *filename, int32 *len);//��ȡMRP���ڵ���Դ���������ݵ�ַ

int isPointInRect(int p, int q, int x, int y, int w, int h);//�жϵ��Ƿ��ھ���������

void toSpin(int px,int py,int rx,int ry,int r,int* x,int* y);//������õ㣨rx��ry���Ƶ㣨px��py����תr�Ⱥ�����꣨x��y��

void SetRootPath(void);//����ƽ̨��Ŀ¼ΪT������ƽ̨Ĭ�ϸ�Ŀ¼ΪT��mythroad����Ҫ����T����Ŀ¼���������������ʹ�ñ��ӿڣ�



#ifdef __cplusplus
}
#endif


#endif