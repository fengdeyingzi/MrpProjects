
#ifndef __BASE_H__
#define __BASE_H__
#include "mr_helper.h"



//系统版本，由于软件的更新，可能会造成有些函数、宏、类型等不一致而导致运行出错
//建议在源码中判断系统版本是否相同
#define _VERSION 3100
typedef int32 size_t;
/*
typedef unsigned short uint16;    //有符号16bit整型
typedef unsigned long int uint32; //无符号32bit整型
typedef unsigned long long uint64; //新加类型
typedef long int int32;           //有符号32bit整型
typedef unsigned char uint8;      //无符号8bit整型
typedef signed char int8;         //有符号8bit整型
typedef signed short int16;       //有符号16bit整型
*/

//下面四项为大部分系统函数的返回值，没有在系统内定义，如果需要这些宏定义请在其他头文件中定义
#define MR_SUCCESS 0 //成功
#define MR_FAILED -1 //失败
#define MR_IGNORE 1  //不关心
#define MR_WAITING 2 //异步(非阻塞)模式

// #define NULL (void *)0
#define TRUE 1
#define FALSE 0
extern int32 SCRW ;//系统内部宏，值为屏幕宽
extern int32 SCRH ;//系统内部宏，值为屏幕高
#define TCC 1

//基本按键值（未定义的其他按键也可以使用，但需知道其键值）
enum
{
    _0,          //按键 0
    _1,          //按键 1
    _2,          //按键 2
    _3,          //按键 3
    _4,          //按键 4
    _5,          //按键 5
    _6,          //按键 6
    _7,          //按键 7
    _8,          //按键 8
    _9,          //按键 9
    _STAR,       //按键 *
    _POUND,      //按键 #
    _UP,         //按键 上
    _DOWN,       //按键 下
    _LEFT,       //按键 左
    _RIGHT,      //按键 右
    _SLEFT = 17, //按键 左软键
    _SRIGHT,     //按键 右软键
    _MENU = 17,  //菜单键
    _BACK,       //返回键
    _SEND,       //按键 接听键
    _SELECT      //按键 确认/选择（若方向键中间有确认键，建议设为该键）
};

//基本事件（其他事件需自己定义）
enum
{
    KY_DOWN,     //按键按下
    KY_UP,       //按键释放
    MS_DOWN,     //鼠标按下
    MS_UP,       //鼠标释放
    MN_SLT,      //菜单选择
    MN_RET,      //菜单返回
    MR_DIALOG,   //对话框
    MS_MOVE = 12 //鼠标移动
};

enum
{
    DLG_OK,    //对话框/文本框等的"确定"键被点击(选择)
    DLG_CANCEL //对话框/文本框等的"取消"("返回")键被点击(选择)
};

enum
{
    SEEK_SET, //从文件起始开始
    SEEK_CUR, //从当前位置开始
    SEEK_END  //从文件末尾开始
};
enum
{
    IS_FILE = 1,    //文件
    IS_DIR = 2,     //目录
    IS_INVALID = 8, //无效(非文件、非目录)
};
#ifndef MRC_BASE_H
#define MR_FILE_RDONLY     1//以只读的方式打开文件。
#define MR_FILE_WRONLY     2//以只写的方式打开文件。
#define MR_FILE_RDWR       4//以读写的方式打开文件。
#define MR_FILE_CREATE      8//如果文件不存在，创建该文件。
#endif

typedef struct
{
    uint16 x;
    uint16 y;
    uint16 w;
    uint16 h;
} rectst;

typedef struct
{
    uint8 r;
    uint8 g;
    uint8 b;
} colorst;



enum
{
    SHADE_UPDOWN,       //从上到下
    SHADE_LEFTRIGHT,    //从左到右
    SHADE_DOWNUP,       //从下到上
    SHADE_RIGHTLEFT     //从右到左
};

enum{
      UI_TOAST,
      UI_EDITDIALOG,
      UI_TEXTDIALOG,
      UI_DIALOG,
      UI_FILELIST,
      UI_MENU
};



typedef void (*mrc_timerCB)(int32 data);
typedef struct MRC_TIMER {
    int32 time;    //定时器时长
    int32 uptime;  //剩余时间(无效)
    uint32 starttime; //定时器创建时间(无效)
    int32 data;  //data数据
    mrc_timerCB timerCB;  //回调函数
    int32 loop;  //是否循环
    int32 isrun;  //是否在运行(无效，被runnable取代)
    struct MRC_TIMER *next;  //指向下一个定时器
    int32 isStop;
} _TIMER;
#define SDL_TIMER 0x8001

typedef struct CAPP_EDITDIALOG{
      char *title;
      char *title_plat;
      char *text;
      char *text_plat;
      int32 type;
      int32 max_size;
} CAPP_EDITDIALOG;

typedef struct CAPP_DIALOG{
      char *name; //对话框类型 edit编辑框 dialog对话框 text文本框
      char *title;
      char *title_plat;
      char *text;
      char *text_plat;
      int32 type;
      int32 max_size;
} CAPP_DIALOG;

typedef struct _iobuf
{
    // char*    _ptr; //文件名？？
    // int  _cnt;
    // char*    _base;
    // int  _flag;
    int32 _file; //进程级文件表 记录文件打开数
    // int  _charbuf;
    // int  _bufsiz;
    char *_tmpfname;
} FILE;

int32 mrc_init(void);
int32 mrc_pause(void);
int32 mrc_resume(void);
int32 mrc_exitApp(void);
int32 mrc_event(int32 code, int32 p0, int32 p1);

int init(void);
int event(int type,int p1,int p2);
int pause(void);
int resume(void);
int exitApp(void);


void base_init(); //base函数库初始化

extern void* mr_malloc(int32 len);

//请屏
void mrc_clearScreen(int r, int g, int b);
//刷新屏幕
void mrc_refreshScreen(int16 x, int16 y, uint16 w, uint16 h);







typedef void (*MRC_PRINTF)(const char* format, ...);


// typedef const char* (*T_mr_getCharBitmap)(uint16 ch, uint16 fontSize, int* width, int* height);

// typedef int32 (*T_mr_timerStart)(uint16 t);
// typedef int32 (*T_mr_timerStop)(void);
// typedef uint32 (*T_mr_getTime)(void);
typedef int32 (*getdatetime)(mr_datetime* datetime);
typedef int32 (*mr_getUserInfo)(mr_userinfo* info);
// typedef int32 (*sleep)(uint32 ms);
typedef int32 (*mr_plat)(int32 code, int32 param);

// typedef void (*MR_PLAT_EX_CB)(uint8* output, int32 output_len);
typedef int32 (*mr_platEx)(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB* cb);

typedef int32 (*mr_ferrno)(void);
// typedef int32 (*mrc_open)(const char* filename, uint32 mode);
// typedef int32 (*mrc_close)(int32 f);
typedef int32 (*T_mr_info)(const char* filename);
// typedef int32 (*mrc_write)(int32 f,const void* p, uint32 l);
// typedef int32 (*mrc_read)(int32 f, void* p, uint32 l);

// typedef int32 (*mrc_seek)(int32 f, int32 pos, int method);
// typedef int32 (*mrc_getLen)(const char* filename);
// typedef int32 (*mrc_remove)(const char* filename);
// typedef int32 (*mrc_rename)(const char* oldname, const char* newname);
// typedef int32 (*mrc_mkDir)(const char* name);
// typedef int32 (*mrc_rmDir)(const char* name);

// typedef int32 (*mrc_findStart)(const char* name, char* buffer, uint32 len);
// typedef int32 (*mrc_findGetNext)(int32 search_handle, char* buffer, uint32 len);
// typedef int32 (*mrc_findStop)(int32 search_handle);
// typedef int32 (*mrc_exit)(void);
/*
typedef int32 (*T_mr_startShake)(int32 ms);
typedef int32 (*T_mr_stopShake)(void);

typedef int32 (*T_mr_playSound)(int type, const void* data, uint32 dataLen, int32 loop);
typedef int32 (*T_mr_stopSound)(int type);

typedef int32 (*T_mr_sendSms)(char* pNumber, char* pContent, int32 flags);
typedef void (*T_mr_call)(char* number);
typedef int32 (*T_mr_getNetworkID)(void);
*/
// typedef void (*mrc_wap)(char* wap);

typedef void (*mr_platDrawChar)(uint16 ch, int32 x, int32 y, int32 color);

// typedef int32 (*mrc_menuCreate)(const char* title, int16 num);
// typedef int32 (*mrc_menuSetItem)(int32 menu, const char* text, int32 index);
// typedef int32 (*mrc_menuShow)(int32 menu);
// typedef int32 (*mrc_menuSetFocus)(int32 menu, int32 index);
// typedef int32 (*mrc_menuRelease)(int32 menu);
// typedef int32 (*mrc_menuRefresh)(int32 menu);

// typedef int32 (*mrc_dialogCreate)(const char* title, const char* text, int32 type);
// typedef int32 (*mrc_dialogRelease)(int32 dialog);
// typedef int32 (*mrc_dialogRefresh)(int32 dialog, const char* title, const char* text, int32 type);

// typedef int32 (*mrc_textCreate)(const char* title, const char* text, int32 type);
// typedef int32 (*mrc_textRelease)(int32 text);
// typedef int32 (*mrc_textRefresh)(int32 handle, const char* title, const char* text);

// typedef int32 (*mrc_editCreate)(const char* title, const char* text, int32 type, int32 max_size);
// typedef int32 (*mrc_editRelease)(int32 edit);
// typedef const char* (*mrc_editGetText)(int32 edit);



// typedef int32 (*mrc_getScreenInfo)(mr_screeninfo* screeninfo);

typedef int32 (*MR_INIT_NETWORK_CB)(int32 result);
typedef int32 (*MR_GET_HOST_CB)(int32 ip);

// typedef int32 (*mrc_initNetwork)(MR_INIT_NETWORK_CB cb, const char* mode);
// typedef int32 (*mrc_closeNetwork)(void);
// typedef int32 (*mrc_getHostByName)(const char* name, MR_GET_HOST_CB cb);
// typedef int32 (*mrc_socket)(int32 type, int32 protocol);
// typedef int32 (*mrc_connect)(int32 s, int32 ip, uint16 port, int32 type);
// typedef int32 (*mrc_closeSocket)(int32 s);
// typedef int32 (*mrc_recv)(int32 s, char* buf, int len);
// typedef int32 (*mrc_recvfrom)(int32 s, char* buf, int len, int32* ip, uint16* port);
// typedef int32 (*mrc_send)(int32 s, const char* buf, int len);
// typedef int32 (*mrc_sendto)(int32 s, const char* buf, int len, int32 ip, uint16 port);



typedef int (*MRC_SPRINTF)(char* s, const char* format, ...);







void capp_exit();
// int mrc_getlen(const char* filename);
// int mrc_wstrlen(const char * txt);
typedef int32 (*MRC_TIMERCREATE)();
typedef int32 (*MRC_TIMERSTART)(int t, int time, int data, mrc_timerCB timerCB, int loop);
typedef int32 (*MRC_TIMERSETTIME)(int32 t,int32 time);
typedef int32 (*MRC_TIMERSTOP)(int t);
typedef int32 (*MRC_TIMERDEL)(int t);
typedef uint32 (*MRC_GETUPTIME)();
typedef int (*mrc_dtext)(const char *pcText, int x,int y, int r,int g, int b, int is_unicode, int font);
typedef int (*mrc_dtextline)(const char *pcText, int x,int y, int r,int g, int b, int is_unicode, int font);
int mrc_dtextex(char *pcText, int x,int y, rectst *rect, colorst *color, int is_unicode, int font);
int mrc_textwh(char *pcText, int is_unicode, int font, int32* w, int32* h);
int mrc_unitextrow(uint16* pcText, uint16 font,int32 w);
void mrc_drect(int x,int y,int w,int h,int r,int g,int b);
void mrc_dline(int x1,int x2,int y1,int y2,int r, int g,int b);
void mrc_dpoint(int x,int y,uint16 nativecolor);
void mrc_dpointex(int x,int y,int r,int g,int b);

int32 mrc_effsetcon(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb);
void mrc_shaderect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode);
int32 mrc_img(char *filename, int x,int y);
void mrc_getscrsize(int32 *w,int32 *h);
void mrc_setscrsize(int32 w,int32 h);
void LOG_I(char *text);




FILE *capp_fopen(const char *path, const char *mode);
unsigned int capp_fread(void *buf, size_t size, size_t count, FILE *f);
int capp_fseek(FILE *f, long offset, int mode);
int capp_fwrite(const void *buf, size_t size, size_t count, FILE *f);
int capp_fflush(FILE *f);
int capp_fclose(FILE *f);

uint16 *w_getScreenBuffer();
void mrc_drawPoint(int16 x, int16 y, uint16 nativecolor);
void mrc_drawPointEx(int16 x, int16 y, int32 r, int32 g, int32 b);
#define mrc_drawPoint dpoint
#define mrc_drawPointEx dpointex
void mrc_drawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
void mrc_drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 r, uint8 g, uint8 b);
int32 mrc_bitmapShowFlip(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
		int16 sx, int16 sy, uint16 transcolor);

int mrc_bitmapShowEx(uint16* p,
   int16 x,
   int16 y,
   int16 mw,
   int16 w,
   int16 h,
   uint16 rop,
   int16 sx,
   int16 sy
);
void *mrc_malloc(int len);
void mrc_free(void *data);


// #define cls mrc_clearScreen
// #define ref _ref
// #define malloc mrc_malloc
// #define free mrc_free
// #define refall() ref(0,0,SCRW,SCRH)


// extern T_mr_malloc mallocdata;
// extern T_mr_free freedata;
// extern T_mr_realloc realloc;
// extern T_memcpy memcpy;
// extern T_memmove memmove;
// extern T_strcpy strcpy;
// extern T_strncpy strncpy;
// extern T_strcat strcat;
// extern T_strncat strncat;
// extern T_memcmp memcmp;
// extern T_strcmp strcmp;
// extern T_strncmp strncmp;
// extern T_strcoll strcoll;
// extern T_memchr memchr;
// extern T_memset memset;
// extern T_strlen strlen;
// extern T_strstr strstr;
// extern T_strtoul strtoul;
// extern T_rand rand;
// extern T_mr_connectWAP wap;
// extern T_mr_open mrc_open;
// extern T_mr_close mrc_close;
// extern T_mr_write mrc_write;
// extern T_mr_read mrc_read;
// extern mrc_dtextline dtextline;
// extern mrc_dtext dtext;
// extern T_atoi atoi;
// extern T_mr_printf printf;
// extern T_sprintf sprintf;

// extern T_mr_getTime getuptime;
// extern T_mr_remove remove;
// extern T_mr_getLen getlen;
// extern T_mr_rename rename;
// extern T_mr_mkDir mkDir;
// extern T_mr_rmDir rmDir;
// extern MRC_TIMERCREATE timercreate;
// extern MRC_TIMERSTART timerstart;
// extern MRC_TIMERSETTIME timersettime;
// extern MRC_TIMERSTOP timerstop;
// extern MRC_TIMERDEL timerdel; 
// extern T__mr_readFile mrc_readFileFromMrp;


#endif
