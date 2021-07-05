/*
** Copyright zengming00
**
** Create date: 2012-03-08 by zengming00
**
** Version: 1.0
**
** 注：“冒泡开发实验系统”以下简称mpc
** 为方便mpc中创建的源程序在计算机中调试或生成而创建
** 此文件只适用于核心版本号为1001的“冒泡开发实验系统”所生成的源代码
**
** 移植步骤：
**     1.整理所有源程序中的include和文件路径， 同时将"base.h"、"sound.h"、"ctype.h"删除
**     2.添加 【#include "mpc.h"】，添加mpc.c文件
**     3.将timerstart的第四个参数去除双引号，如:timerstart(timer,100,p1,"updown",1);
**         改为timerstart(timer,100,p1,updown,1);
**     4.在源程序init入口处添加 【mpc_init();】注意，必须最先在init函数中调用
**
** 常见问题：
**     1.mpc中所有宏和函数等都是全局的，因此当手机开发者的源码不遵守游戏规则时编译将会出现大量错误
**        这时就需要移植者手工整理这些宏、函数。。。的声明（--!）
**     2.手机开发者在源码中使用了只有mpc才支持的脚本模式（类似bat脚本的功能），使得编译出现语法错误
**        这些脚本的功能可以看作是初始化功能，因为在mpc中首先被执行的并不是init函数而是这些脚本，所
**        以，只要把这些脚本放到init函数中即可
**     3.局部变量或全局变量未初始化，mpc中所有变量都会被初始化为0，当手机开发者没有注意到这一点时，会给
**        移植带来很大的麻烦
**     4.类型不一致，导致mrpbuilder失败，“#define open mrc_open ”可以看出，mpc与API之间几乎是无差别的
**          当出现大量类型不一致时，可以修改宏定义使其进行强制类型转换(参考u2c)
**     5.局部变量定义的问题，mpc支持不在语句块开头处定义变量的功能（C99)，这一功能使得程序变得易读、方便，但是
**         这会导致一些编译器无法编译，此时需要将这些变量提升到语句块的头部
**
** 其他建议：
**     手机开发者应尽量遵守C语言（C89）规范，过度使用mpc特有功能导致的后果是得不偿失的
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



//#define _VERSION 1001  “冒泡开发实验系统”内部核心版本号

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



/********************************C库函数********************************/

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

/********************************绘图接口********************************/

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



/********************************本地化UI接口********************************/

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
/********************************其他接口********************************/
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
// 以下函数定义在mrc_bmp.h文件中//////////////////////////////////////


#define getscrbuf w_getScreenBuffer
#define setscrbuf w_setScreenBuffer
#define getscrsize mrc_getScreenSize
#define setscrsize mrc_setScreenSize

#define    BM_TRANS BM_TRANSPARENT

#define bmpshowflip mrc_bitmapShowFlip


#define readall my_readAllEx
//extern void mrc_connectWAP(char* wap);
#define wap mrc_connectWAP

/********************************定时器接口********************************/

#define timercreate mrc_timerCreate

#define timerdel mrc_timerDelete

#define timerstop mrc_timerStop

#define timerstart mrc_timerStart

#define timersettime mrc_timerSetTimeEx
///////////////////以下函数为系统扩展函数////////////////////

#define img DrawIMG

#define shaderect DrawShadeRect

/*

void mpc_init(void);

int32 my_readAllEx(char* filename, char* buf, int32 *len);

int DrawIMG(char* filename, int x,int y);

void DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode);

*/

/********************************自定义********************************/

void mpc_init(void);


//void * mrc_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor);









//////////////////////////////////
//#include <xl.h>






#ifdef __cplusplus
}
#endif


#endif

