/*
** Copyright zengming00
**
** Create date: 2012-03-08 by zengming00
**
** Version: 1.1  增加SetRootPath接口  2022/2/13
** Version: 1.0
**
** 注：“冒泡开发实验系统”以下简称mpc，函数名简单
** 为方便mpc中创建的源程序在计算机中调试或生成而创建
**
** mpc库使用步骤：
**     1.添加 【#include "mpc.h"】，编译时用mpc_mtk.lib，含t的是thumb模式的，（vs2005调试时项目属性链接器用mpc_WIN32.lib）
**     2.在源程序init入口处添加 【mpc_init();】注意，必须最先在init函数中调用
**     3.在源程序exitapp处添加 【mpc_exit();】注意，必须最后在exitapp函数中调用
**     int32 exitapp()
       {
	   //好处是应用执行exit退出后 或被动强制退出时, 手机系统会自动调用本函数.所以建议：应用全局性资源的释放在该函数内进行；
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



#define _VERSION 1001  //“冒泡开发实验系统”内部核心版本号
#define MAKECOLOR565(color) ((uint16)((color>>8)&0xf800) |  (uint16)((color>>5)&0x07e0) | (uint16)((color>>3)&0x1f))


//ctype
#define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isdigit(c) ((c) >= '0' && (c) <= '9')
#define isalnum(c) (isalpha(c) || isdigit(c))
#define isprint(ch) ((ch) >= 0x20 && (ch) <= 0x7e)
#define tolower(c) ((c) |= 0x20)
#define toupper(c) ((c) &= 0xdf)


//base

extern int16  SCRW;//屏幕宽
extern int16  SCRH;//屏幕高
extern int16  FONTW;//手机中等字库字体的宽
extern int16  FONTH;//手机中等字库字体的高


//基本按键值（未定义的其他按键也可以使用，但需知道其键值）
enum
{
  _0,           //按键 0
  _1,           //按键 1
  _2,           //按键 2
  _3,           //按键 3
  _4,           //按键 4
  _5,           //按键 5
  _6,           //按键 6
  _7,           //按键 7
  _8,           //按键 8
  _9,           //按键 9
  _STAR,        //按键 *
  _POUND,       //按键 #
  _UP,          //按键 上
  _DOWN,        //按键 下
  _LEFT,        //按键 左
  _RIGHT,       //按键 右
  _SLEFT=17,    //按键 左软键
  _SRIGHT,      //按键 右软键
  _SEND,        //按键 接听键
  _SELECT       //按键 确认/选择（若方向键中间有确认键，建议设为该键）
};

//基本事件（其他事件需自己定义）
enum
{
  KY_DOWN, 	 //按键按下
  KY_UP,       //按键释放
  MS_DOWN, 	 //鼠标按下
  MS_UP, 	     //鼠标释放
  MN_SLT, //菜单选择
  MN_RET, //菜单返回
  MR_DIALOG, //对话框
  MS_MOVE=12   //鼠标移动
};

enum
{
  DLG_OK,         //对话框/文本框等的"确定"键被点击(选择)
  DLG_CANCEL  //对话框/文本框等的"取消"("返回")键被点击(选择)
};

enum
{
  SEEK_SET,             //从文件起始开始
  SEEK_CUR,             //从当前位置开始
  SEEK_END             //从文件末尾开始
};
enum
{
  IS_FILE=1,      //文件
  IS_DIR=2,      //目录
  IS_INVALID=8  //无效(非文件、非目录)
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


/********************************C库函数********************************/

#define  printf(...)

#define strrchr mrc_strrchr

#define wstrlen mrc_wstrlen


/*******************************框架函数*********************************/

#define init            MRC_EXT_INIT
#define exitapp         MRC_EXT_EXIT
#define event           mrc_event
#define pause           mrc_pause
#define resume          mrc_resume

//////////////////////以上为平台必需实现的函数////////////////////////

/********************************文件接口********************************/

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

#define dpoint mrc_drawPoint

#define dpointex mrc_drawPointEx

#define cls mrc_clearScreen

#define ref mrc_refreshScreen

#define effsetcon mrc_EffSetCon

/********************************声音接口********************************/

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


/********************************网络接口********************************/

#define initnetwork mrc_initNetwork
#define gethostbyname mrc_getHostByName
// #define socket mrc_socket
// #define connect mrc_connect
// #define getsocketstate mrc_getSocketState
// #define send mrc_send
// #define recv mrc_recv
#define closesocket mrc_closeSocket
#define closenetwork mrc_closeNetwork

/********************************其他接口********************************/
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

// 以下函数定义在mrc_bmp.h文件中//////////////////////////////////////


#define getscrbuf w_getScreenBuffer
#define setscrbuf w_setScreenBuffer
#define getscrsize mrc_getScreenSize
#define setscrsize mrc_setScreenSize

#define BM_TRANS BM_TRANSPARENT

#define bmpshowflip mrc_bitmapShowFlip

extern void mrc_connectWAP(char* wap);
#define wap mrc_connectWAP

/********************************定时器接口********************************/

#define timercreate mrc_timerCreate

#define timerdel mrc_timerDelete

#define timerstop mrc_timerStop

#define timerstart mrc_timerStart

#define timersettime mrc_timerSetTimeEx

/*****************************以下为mpc扩展接口****************************/

void mpc_init(void);//加载本库

void mpc_exit(void);//退出卸载

#define img DrawIMG

#define shaderect DrawShadeRect

#define isPointCollRect isPointInRect

#define readall my_readAllEx

int32 my_readAllEx(char* filename, char* buf, int32 *len);//读取文件len长度到buf，

int DrawIMG(char* filename, int x,int y);//调试无效果，真机会调用系统接口绘制图片

void DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode);//产生渐变色

int32 getColor(int32 color1, uint32 color2);//混合两个颜色

void drawPoint(int x,int y,uint32 color);//画点

int getLineSize(int x,int y, int x2,int y2);//获取两点之间的长度 的平方

void drawRect(int32 x,int32 y,int32 w,int32 h,uint32 color);//画矩形

void drawCir(int32 x,int32 y,int32 r,uint32 color);//画圆

void DrawEffRect(int16 x, int16 y, int16 w, int16 h, char r, char g, char b);//画透明圆角矩形

void toast(char *Msg, int sp);//toast消息，sp为毫秒，会调用用sky12字库，请参阅《斯凯Mythroad 单机MRP开发SDK发布说明_AF01.32X2》字库插件章节

void *readFileFromAssets(char *filename, int32 *len);//读取MRP包内的资源，返回内容地址

int isPointInRect(int p, int q, int x, int y, int w, int h);//判断点是否在矩形区域内

void toSpin(int px,int py,int rx,int ry,int r,int* x,int* y);//用来获得点（rx，ry）绕点（px，py）旋转r度后的坐标（x，y）

void SetRootPath(void);//设置平台根目录为T卡，（平台默认根目录为T卡mythroad，如要访问T卡根目录，例如歌曲，则需使用本接口）



#ifdef __cplusplus
}
#endif


#endif