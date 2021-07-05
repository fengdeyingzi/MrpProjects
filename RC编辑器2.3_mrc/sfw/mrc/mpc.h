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

int SCRW ;
int SCRH ;
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



//sound
enum{_MIDI=1,_WAVE,_MP3,_AMR,_PCM,_M4A,_AMR_WB};


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
    KY_DOWN,     //��������
    KY_UP,       //�����ͷ�
    MS_DOWN,     //��갴��
    MS_UP,       //����ͷ�
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
    IS_INVALID=8  //��Ч(���ļ�����Ŀ¼)
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



enum
{
    SHADE_UPDOWN,       //���ϵ���
    SHADE_LEFTRIGHT,    //������
    SHADE_DOWNUP,       //���µ���
    SHADE_RIGHTLEFT     //���ҵ���
};


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

void mpc_init()
{
    mr_screeninfo screen;

    mrc_getScreenInfo(&screen);
    SCRW = (uint16)screen.width;
    SCRH = (uint16)screen.height;
}


//void * mrc_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor);

void *readmrpfile(char *filename,int32 *len)
{
return mrc_readFileFromMrp (filename,len,0);


}



void freefiledata(void *data,int32 filelen)
{
 mrc_freeFileData(data,(int32)filelen);
}



void ShadeRect(int x, int y, int w, int h, int AR, int AG, int AB, int BR, int BG, int BB, int mode)
{
    int16 i,j,t;

    BR-=AR; BG-=AG; BB-=AB;
    switch(mode)
    {
    case SHADE_UPDOWN:
        t=x+w-1;
        for(i=0;     i<h;     i++)
            mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*i/h),(uint8)(AG+BG*i/h),(uint8)(AB+BB*i/h));
        return;
    case SHADE_DOWNUP:
        t=x+w-1;
        for(i=h-1,j=0;    i>=0;    i--,j++)
            mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*j/h),(uint8)(AG+BG*j/h),(uint8)(AB+BB*j/h));
        return;
    case SHADE_LEFTRIGHT:
        t=y+h-1;
        for(i=0;     i<w;     i++)
            mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*i/w),(uint8)(AG+BG*i/w),(uint8)(AB+BB*i/w));
        return;
    case SHADE_RIGHTLEFT:
        t=y+h-1;
        for(i=w-1,j=0;    i>=0;    i--,j++)
            mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*j/w),(uint8)(AG+BG*j/w),(uint8)(AB+BB*j/w));
        return;
    }
}

void DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode)
{
    //RGBA+(RGBB-RGBA)*N/Step
    int32 AR,AG,AB;
    int32 BR,BG,BB;

    AR = (uint8)(pixelA >> 16) & 0xff;
    AG = (uint8)(pixelA >> 8) & 0xff;
    AB = (uint8)(pixelA) & 0xff;

    BR = (uint8)((pixelB >> 16) & 0xff);
    BG = (uint8)((pixelB >> 8) & 0xff);
    BB = (uint8)((pixelB) & 0xff);
    ShadeRect(x,y,w,h,AR,AG,AB,BR,BG,BB,mode);

}

//////////////////////////////////
//#include <xl.h>






#ifdef __cplusplus
}
#endif


#endif

