/*
** Copyright zengming00
**
** Create date: 2012-03-08 by zengming00
**
** Version: 1.0
**
** ע����ð�ݿ���ʵ��ϵͳ�����¼��mpc
** Ϊ����mpc�д�����Դ�����ڼ�����е��Ի����ɶ�����
** ���ļ�ֻ�����ں��İ汾��Ϊ1001�ġ�ð�ݿ���ʵ��ϵͳ�������ɵ�Դ����
**
** ��ֲ���裺
**     1.��������Դ�����е�include���ļ�·���� ͬʱ��"base.h"��"sound.h"��"ctype.h"ɾ��
**     2.��� ��#include "mpc.h"�������mpc.c�ļ�
**     3.��timerstart�ĵ��ĸ�����ȥ��˫���ţ���:timerstart(timer,100,p1,"updown",1);
**         ��Ϊtimerstart(timer,100,p1,updown,1);
**     4.��Դ����init��ڴ���� ��mpc_init();��ע�⣬����������init�����е���
**
** �������⣺
**     1.mpc�����к�ͺ����ȶ���ȫ�ֵģ���˵��ֻ������ߵ�Դ�벻������Ϸ����ʱ���뽫����ִ�������
**        ��ʱ����Ҫ��ֲ���ֹ�������Щ�ꡢ������������������--!��
**     2.�ֻ���������Դ����ʹ����ֻ��mpc��֧�ֵĽű�ģʽ������bat�ű��Ĺ��ܣ���ʹ�ñ�������﷨����
**        ��Щ�ű��Ĺ��ܿ��Կ����ǳ�ʼ�����ܣ���Ϊ��mpc�����ȱ�ִ�еĲ�����init����������Щ�ű�����
**        �ԣ�ֻҪ����Щ�ű��ŵ�init�����м���
**     3.�ֲ�������ȫ�ֱ���δ��ʼ����mpc�����б������ᱻ��ʼ��Ϊ0�����ֻ�������û��ע�⵽��һ��ʱ�����
**        ��ֲ�����ܴ���鷳
**     4.���Ͳ�һ�£�����mrpbuilderʧ�ܣ���#define open mrc_open �����Կ�����mpc��API֮�伸�����޲���
**          �����ִ������Ͳ�һ��ʱ�������޸ĺ궨��ʹ�����ǿ������ת��(�ο�u2c)
**     5.�ֲ�������������⣬mpc֧�ֲ������鿪ͷ����������Ĺ��ܣ�C99)����һ����ʹ�ó������׶������㣬����
**         ��ᵼ��һЩ�������޷����룬��ʱ��Ҫ����Щ���������������ͷ��
**
** �������飺
**     �ֻ�������Ӧ��������C���ԣ�C89���淶������ʹ��mpc���й��ܵ��µĺ���ǵò���ʧ��
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



//#define _VERSION 1001  ��ð�ݿ���ʵ��ϵͳ���ڲ����İ汾��

extern int32 SCRW ;
extern int32 SCRH ;
#define true 1
#define false 0
/*
#define mrc_pause mrc_appPause 
#define mrc_resume mrc_appResume
#define mrc_appPause mrc_pause
#define mrc_appResume mrc_resume
*/
//ctype
#define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isdigit(c) ((c) >= '0' && (c) <= '9')
#define isalnum(c) (isalpha(c) || isdigit(c))
#define isprint(ch) ((ch) >= 0x20 && (ch) <= 0x7e)
#define tolower(c) ((c) |= 0x20)
#define toupper(c) ((c) &= 0xdf)






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



//base


//extern int16 SCRW;  
//extern int16  SCRH ;



/********************************C�⺯��********************************/

#define  printf(...)

#define strrchr mrc_strrchr

#define wstrlen mrc_wstrlen



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

#define dpoint mrc_drawPointEx

#define dpointex mrc_drawPointEx
#define cls mrc_clearScreen
#define ref mrc_refreshScreen

#define effsetcon mrc_EffSetCon



/********************************���ػ�UI�ӿ�********************************/

/*
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
*/
#define editcreate mrc_editCreate
#define editdel mrc_editRelease

#define editget mrc_editGetText
/********************************�����ӿ�********************************/
#define exit mrc_exit

#define getuptime mrc_getUptime

#define getdatetime mrc_getDatetime
#define getmemremain mrc_getMemoryRemain
#define shake mrc_startShake
#define getsysmem mrc_getSysMem

#define readfilefrommrp mrc_readFileFromMrpEx


//#define freefiledata mrc_freeFileData
#define freeorigin mrc_freeOrigin

#define u2c( input,  input_len, output,  output_len) mrc_unicodeToGb2312((uint8*) input, (int32) input_len, (uint8**) output, (int32*) output_len)

int32 mrc_runMrp(char* mrp_name,char* file_name,char* parameter);
#define  runmrp( mrp_name, filename) mrc_runMrp( mrp_name, filename,NULL)

#define getparentpath mrc_GetParentPath

#define sand mrc_sand
#define rand mrc_rand
//extern int32 mrc_sendSms(char* pNumber, char*pContent, int32 flags);
#define sms mrc_sendSms
#define lcd mrc_LCDCanSleep
extern int32 mrc_sleep(uint32 ms);
#define sleep mrc_sleep
// ���º���������mrc_bmp.h�ļ���//////////////////////////////////////


#define getscrbuf w_getScreenBuffer
#define setscrbuf w_setScreenBuffer
#define getscrsize mrc_getScreenSize
#define setscrsize mrc_setScreenSize

#define    BM_TRANS BM_TRANSPARENT

#define bmpshowflip mrc_bitmapShowFlip


#define readall my_readAllEx
//extern void mrc_connectWAP(char* wap);
#define wap mrc_connectWAP

/********************************��ʱ���ӿ�********************************/

#define timercreate mrc_timerCreate

#define timerdel mrc_timerDelete

#define timerstop mrc_timerStop

#define timerstart mrc_timerStart

#define timersettime mrc_timerSetTimeEx
///////////////////���º���Ϊϵͳ��չ����////////////////////

#define img DrawIMG

#define shaderect DrawShadeRect

/*

void mpc_init(void);

int32 my_readAllEx(char* filename, char* buf, int32 *len);

int DrawIMG(char* filename, int x,int y);

void DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode);

*/

/********************************�Զ���********************************/

void mpc_init(void);


//void * mrc_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor);









//////////////////////////////////
//#include <xl.h>






#ifdef __cplusplus
}
#endif


#endif

