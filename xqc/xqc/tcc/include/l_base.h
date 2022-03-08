#ifndef _BASE_H
#define _BASE_H
#include "mrc_base.h"
#include "mrc_exb.h"
#include "mrc_bmp.h"


#define _VERSION 1001

int32 SCRWH(int type);

#define SCRW SCRWH(0) //ϵͳ�ڲ��ֵ꣬Ϊ��Ļ��
#define SCRH SCRWH(1) //ϵͳ�ڲ��ֵ꣬Ϊ��Ļ��


enum {  
   _0,_1,_2,_3,_4,_5,_6,_7,_8,_9,
   _STAR,_POUND,_UP,_DOWN,_LEFT,_RIGHT,_SLEFT=17,
   _SRIGHT,_SEND,_SELECT
};

enum {
    KY_DOWN,KY_UP,
	MS_DOWN,MS_UP,
    MN_SLT,MN_RET,
    MR_DIALOG,MS_MOVE=12
};

enum {
    DLG_OK,
    DLG_CANCEL
};

enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};
enum
{
    IS_FILE=1,
    IS_DIR=2,
    IS_INVALID=8
};

typedef struct {
    uint16 x;
    uint16 y;
    uint16 w;
    uint16 h;
}rectst;

typedef struct {
    uint8 r;
    uint8 g;
    uint8 b;
}colorst;


#define strrchr  mrc_strrchr 
#define wstrlen  mrc_wstrlen
/********************************C�⺯������****************/

/*******************************��ں���*********************/
#define init mrc_init
#define event  mrc_event
#define pause  mrc_pause
#define resume  mrc_resume
#define exitapp mrc_exitApp
#define exitApp mrc_exitApp

////////////////////////////����Ϊƽ̨����ʵ�ֵ���ں���//////////////////////////////
#define printf mrc_printf
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
int32 dtextex(char* pcText, int16 x, int16 y, rectst *rect, colorst *color, int flag, uint16 font);
int32 dtextright(char* pcText, int16 x, int16 y, rectst *rect, colorst *color, int flag, uint16 font);
#define c2u mrc_c2u
#define textwh mrc_textWidthHeight
#define unitextrow mrc_unicodeTextRow
#define drect mrc_drawRect
#define dline mrc_drawLine
#define dpoint mrc_drawPoint
#define dpointex mrc_drawPointEx
#define cls mrc_clearScreen
#define ref mrc_refreshScreen
#define effsetcon mrc_EffSetCon

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

/********************************�����ӿ�********************************/

#define exit mrc_exit
#define getuptime mrc_getUptime
#define getdatetime mrc_getDatetime
#define getmemremain mrc_getMemoryRemain
#define shake mrc_startShake
#define getsysmem mrc_getSysMem
#define readfilefrommrp mrc_readFileFromMrpEx
#define freefiledata mrc_freeFileData
#define freeorigin mrc_freeOrigin

/////////////////////���½ӿڲ����Ƕ�����mrc_base.h�ļ��У���Ϊ��ʹ�÷����ͳһ�����ڱ�ϵͳ��//////////////////////////
// ���º���������mrc_exb.h�ļ���
#define u2c mrc_unicodeToGb2312
#define runmrp(mrp_name,filename) mrc_runMrp(mrp_name,filename,NULL)


/*
ԭ��������:mrc_GetParentPath
����:��ȡ��ǰ����MRP���ڸ�Ŀ¼�����ص�BUFFָ�����ʱ�����Ա�ֶ��ͷ�
����:void
����ֵ:char*  ��Ŀ¼·��ָ��
		    NULL  ��ȡʧ��
*/
char* getparentpath(void);//////////////////////////////////////////////////////////


#define sand   mrc_sand
#define rand   mrc_rand
#define sms   mrc_sendSms
#define lcd   mrc_LCDCanSleep




// ���º���������mrc_bmp.h�ļ���//////////////////////////////////////

#define getscrbuf   w_getScreenBuffer
#define setscrbuf   w_setScreenBuffer
#define getscrsize   mrc_getScreenSize
#define setscrsize   mrc_setScreenSize

enum {
    BM_OR,
	BM_XOR,
	BM_COPY,
	BM_NOT,
	BM_MERGENOT,
    BM_ANDNOT,
    BM_TRANS,
    BM_AND,
    BM_GRAY,
    BM_REVERSE
};

#define bmpshowflip   mrc_bitmapShowFlip
int32 readall(char* filename, char* buf, int32 *len);
#define wap   mrc_connectWAP

/********************************��ʱ���ӿ�********************************/


#define timercreate   mrc_timerCreate
#define timerdel   mrc_timerDelete

#define timerstop   mrc_timerStop
typedef void (*timerCB)(int32 data);
int32 timerstart (int32 t, int32 time, int32 data, char *f, int32 loop);
#define timersettime   mrc_timerSetTimeEx

///////////////////���º���Ϊϵͳ��չ����////////////////////
#include "base.c"

#define img  DrawIMG
#define shaderect DrawShadeRect

#endif
 