#ifndef _BASE_H
#define _BASE_H
#include "mrc_base.h"
#include "mrc_exb.h"
#include "mrc_bmp.h"


#define _VERSION 1001

int32 SCRWH(int type);

#define SCRW SCRWH(0) //系统内部宏，值为屏幕宽
#define SCRH SCRWH(1) //系统内部宏，值为屏幕高


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
/********************************C库函数结束****************/

/*******************************入口函数*********************/
#define init mrc_init
#define event  mrc_event
#define pause  mrc_pause
#define resume  mrc_resume
#define exitapp mrc_exitApp
#define exitApp mrc_exitApp

////////////////////////////以上为平台必需实现的入口函数//////////////////////////////
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


/********************************绘图接口********************************/

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

/********************************本地化UI接口********************************/
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

/********************************其他接口********************************/

#define exit mrc_exit
#define getuptime mrc_getUptime
#define getdatetime mrc_getDatetime
#define getmemremain mrc_getMemoryRemain
#define shake mrc_startShake
#define getsysmem mrc_getSysMem
#define readfilefrommrp mrc_readFileFromMrpEx
#define freefiledata mrc_freeFileData
#define freeorigin mrc_freeOrigin

/////////////////////以下接口并不是定义在mrc_base.h文件中，但为了使用方便而统一定义在本系统中//////////////////////////
// 以下函数定义在mrc_exb.h文件中
#define u2c mrc_unicodeToGb2312
#define runmrp(mrp_name,filename) mrc_runMrp(mrp_name,filename,NULL)


/*
原函数名称:mrc_GetParentPath
功能:获取当前运行MRP所在父目录，返回的BUFF指针空闲时需程序员手动释放
参数:void
返回值:char*  父目录路径指针
		    NULL  获取失败
*/
char* getparentpath(void);//////////////////////////////////////////////////////////


#define sand   mrc_sand
#define rand   mrc_rand
#define sms   mrc_sendSms
#define lcd   mrc_LCDCanSleep




// 以下函数定义在mrc_bmp.h文件中//////////////////////////////////////

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

/********************************定时器接口********************************/


#define timercreate   mrc_timerCreate
#define timerdel   mrc_timerDelete

#define timerstop   mrc_timerStop
typedef void (*timerCB)(int32 data);
int32 timerstart (int32 t, int32 time, int32 data, char *f, int32 loop);
#define timersettime   mrc_timerSetTimeEx

///////////////////以下函数为系统扩展函数////////////////////
#include "base.c"

#define img  DrawIMG
#define shaderect DrawShadeRect

#endif
 