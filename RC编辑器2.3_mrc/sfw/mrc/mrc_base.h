#ifndef MRC_BASE_H
#define MRC_BASE_H



typedef  unsigned long long  uint64;
typedef  long long          int64;

typedef  unsigned short     uint16;
typedef  unsigned long int  uint32;
typedef  long int           int32;
typedef  unsigned char      uint8;
typedef  signed char          int8;
typedef  signed short         int16;

#define MR_SUCCESS  0 
#define MR_FAILED   -1
#define MR_IGNORE   1
#define MR_WAITING   2

#ifndef NULL
#define NULL (void*)0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifdef MR_SPREADTRUM_MOD
#pragma no_check_stack
#endif

enum {
   MR_KEY_0,MR_KEY_1,MR_KEY_2,MR_KEY_3,MR_KEY_4,
   MR_KEY_5,MR_KEY_6,MR_KEY_7,MR_KEY_8,MR_KEY_9,
   MR_KEY_STAR,MR_KEY_POUND,
   MR_KEY_UP,MR_KEY_DOWN,MR_KEY_LEFT,MR_KEY_RIGHT,
   MR_KEY_POWER,MR_KEY_SOFTLEFT,MR_KEY_SOFTRIGHT,MR_KEY_SEND,
   MR_KEY_SELECT,MR_KEY_VOLUME_UP,MR_KEY_VOLUME_DOWN,MR_KEY_NONE
};

enum {
   MR_NET_ID_MOBILE,
   MR_NET_ID_CN,
   MR_NET_ID_CDMA,
   MR_NET_ID_NONE
};

enum {
   MR_EDIT_ANY,
   MR_EDIT_NUMERIC,
   MR_EDIT_PASSWORD
};

enum {
   MR_SIM_NEW,
   MR_SIM_CHANGE,
   MR_SIM_SAME
};

enum {
   MR_DIALOG_OK,
   MR_DIALOG_OK_CANCEL,
   MR_DIALOG_CANCEL
};

enum {
   MR_DIALOG_KEY_OK,
   MR_DIALOG_KEY_CANCEL
};

enum {
   MR_LOCALUI_KEY_OK,
   MR_LOCALUI_KEY_CANCEL
};


enum {
   MR_KEY_PRESS,
   MR_KEY_RELEASE,
   MR_MOUSE_DOWN,
   MR_MOUSE_UP,
   MR_MENU_SELECT,
   MR_MENU_RETURN,
   MR_DIALOG_EVENT,
   MR_EVENT01,
   MR_EXIT_EVENT,
   MR_EVENT02,
   MR_LOCALUI_EVENT,
   MR_OSD_EVENT,
   MR_MOUSE_MOVE,
   MR_ERROR_EVENT, 
   MR_TIMER_EVENT=1001,
   MR_NET_EVENT

};

typedef enum 
{
   MR_SOUND_MIDI,
   MR_SOUND_WAV,
   MR_SOUND_MP3,
   MR_SOUND_PCM,
   MR_SOUND_M4A,
   MR_SOUND_AMR,
   MR_SOUND_AMR_WB
}MR_SOUND_TYPE;

typedef enum 
{
   MR_FONT_SMALL,
   MR_FONT_MEDIUM,
   MR_FONT_BIG
}MR_FONT_TYPE;

enum
{
   MR_SEEK_SET,
   MR_SEEK_CUR,
   MR_SEEK_END
};

enum
{
   MR_SOCK_STREAM,
   MR_SOCK_DGRAM
};

enum
{
   MR_IPPROTO_TCP,
   MR_IPPROTO_UDP
};

enum
{
   MR_ENCODE_ASCII,
   MR_ENCODE_UNICODE
};

enum {
   MR_SOCKET_BLOCK,
   MR_SOCKET_NONBLOCK
};

enum {
   BM_OR,
   BM_XOR,
   BM_COPY,
   BM_NOT,
   BM_MERGENOT,
   BM_ANDNOT,
   BM_TRANSPARENT,
   BM_AND,
   BM_GRAY,
   BM_REVERSE
};

#define MR_IS_FILE     1
#define MR_IS_DIR      2
#define MR_IS_INVALID  8

#define MR_FILE_RDONLY        1
#define MR_FILE_WRONLY        2
#define MR_FILE_RDWR          4
#define MR_FILE_CREATE        8
#define MR_FILE_RECREATE      16

#define DRAW_TEXT_EX_IS_UNICODE         1 
#define DRAW_TEXT_EX_IS_AUTO_NEWLINE    2

#define MR_PLAT_VALUE_BASE 1000 


typedef struct
{
   int32 socket;
   int32 port;
}mr_bind_st;

typedef struct
{
   uint16 year;
   uint8  month;
   uint8  day;
   uint8  hour;
   uint8  minute;
   uint8  second;
}mr_datetime;

typedef struct
{
   uint32 width;
   uint32 height;
   uint32 bit;
}mr_screeninfo;


typedef struct {
   uint16            w;
   uint16            h;
   uint32            buflen;
   uint32            type;
   uint16*            p;
}mr_bitmapSt;

typedef struct {
   uint16*           p;
   uint16            w;
   uint16            h;
   uint16            x;
   uint16            y;
}mr_bitmapDrawSt;

typedef struct {
   int16            x;
   int16            y;
   uint16            w;
   uint16            h;
}mr_screenRectSt;

typedef struct {
   uint8            r;
   uint8            g;
   uint8            b;
}mr_colourSt;

typedef struct {
   uint16            h;
}mr_spriteSt;

typedef struct  {
   int16            x;
   int16            y;
   uint16            w;
   uint16            h;
   int16            x1;
   int16            y1;
   int16            x2;
   int16            y2;
   uint16            tilew;
   uint16            tileh;
}mr_tileSt;


extern void mrc_printf(const char *format,...);

#if defined SDK_MOD &&  defined MEM_LEAK_DEBUG
void *mrc_mallocEx(int size, char* file, int line);
#define malloc(size) mrc_mallocEx(size, __FILE__, __LINE__)
#define free(p) {\
	mrc_free(p);\
	mrc_printf("free,%lu,FILE:%s,LINE:%d", p, __FILE__, __LINE__);\
}
#else	

#define malloc(len)   memset(mrc_malloc(len),0,len)   
#define free     mrc_free     
#endif

#define strchr   mrc_strchr   
#define memcpy   mrc_memcpy   
#define memmove  mrc_memmove  
#define strcpy   mrc_strcpy   
#define strncpy  mrc_strncpy  
#define strcat   mrc_strcat   
#define strncat  mrc_strncat  
#define memcmp   mrc_memcmp   
#define strcmp   mrc_strcmp   
#define strncmp  mrc_strncmp  
#define strcoll  mrc_strcoll  
#define memchr   mrc_memchr   
#define memset   mrc_memset   
#define strlen   mrc_strlen   
#define strstr   mrc_strstr   
#define sprintf  mrc_sprintf             
#define atoi     mrc_atoi     
#define strtoul  mrc_strtoul  

extern void *MR_MEMCPY(void * s1, const void * s2, int n);
extern void *MR_MEMMOVE(void * s1, const void * s2, int n);
extern char *MR_STRCPY(char * s1, const char * s2);
extern char *MR_STRNCPY(char * s1, const char * s2, int n);
extern char *MR_STRCAT(char * s1, const char * s2);
extern char *MR_STRNCAT(char * s1, const char * s2, int n);
extern int MR_MEMCMP(const void * s1, const void * s2, int n);
extern int MR_STRCMP(const char * s1, const char * s2);
extern int MR_STRNCMP(const char * s1, const char * s2, int n);
extern int MR_STRCOLL(const char * s1, const char * s2);
extern void *MR_MEMCHR(const void * s, int c, int n);
extern void *MR_MEMSET(void * s, int c, int n);
extern int MR_STRLEN(const char * s);
extern char *MR_STRSTR(const char * s1, const char * s2);
extern int MR_SPRINTF(char * s, const char * format, ...);
extern int MR_ATOI(const char * nptr);
extern unsigned long int MR_STRTOUL(const char * nptr, char ** endptr, int base);

extern void* mrc_malloc(int size);
extern void mrc_free(void *address);
extern void mrc_freeEx(void *address);
extern char *mrc_strchr(const char *src,int c);
extern int32 mrc_wstrlen(char * txt);

#define mrc_memcpy(s1, s2, n)       MR_MEMCPY(s1, s2, n)
#define mrc_memmove(s1, s2, n)   MR_MEMMOVE(s1, s2, n)
#define mrc_strcpy(s1, s2)              MR_STRCPY(s1, s2)
#define mrc_strncpy(s1, s2, n)        MR_STRNCPY(s1, s2, n)
#define mrc_strcat(s1, s2)               MR_STRCAT(s1, s2)
#define mrc_strncat(s1, s2, n)         MR_STRNCAT(s1, s2, n)
#define mrc_memcmp(s1, s2, n)      MR_MEMCMP(s1, s2, n)
#define mrc_strcmp(s1, s2)              MR_STRCMP(s1, s2)
#define mrc_strncmp(s1, s2, n)         MR_STRNCMP(s1, s2, n)
#define mrc_strcoll(s1, s2)               MR_STRCOLL(s1, s2)

#define mrc_memchr(s1, c, n)          MR_MEMCHR(s1, c, n)
#define mrc_memset(s1, c, n)          MR_MEMSET(s1, c, n)
#define mrc_strlen(s)                       MR_STRLEN(s)
#define mrc_strstr(s1, s2)               MR_STRSTR(s1, s2)
#define mrc_sprintf                         MR_SPRINTF
#define mrc_atoi(s1)                       MR_ATOI(s1)
#define mrc_strtoul(s1, s2, n)         MR_STRTOUL(s1, s2, n)

#define MR_MALLOC(size) mrc_malloc(size)
#define MR_FREE(p, size) mrc_free(p)

char *mrc_getCharBitmap(uint16 ch, uint16 fontSize, int *width, int *height);

int32 mrc_ferrno(void);

int32 mrc_open(const char* filename,  uint32 mode);

int32 mrc_close(int32 f);

int32 mrc_fileState(const char* filename);

int32 mrc_write(int32 f,void *p,uint32 l);

int32 mrc_read(int32 f,void *p,uint32 l);

void* mrc_readAll(const char* filename, uint32 *len);

int32 mrc_seek(int32 f, int32 pos, int method);

int32 mrc_getLen(const char* filename);

int32 mrc_remove(const char* filename);

int32 mrc_rename(const char* oldname, const char* newname);

int32 mrc_mkDir(const char* name);

int32 mrc_rmDir(const char* name);

int32 mrc_findStart(const char* name, char* buffer, uint32 len);

int32 mrc_findGetNext(int32 search_handle, char* buffer, uint32 len);

int32 mrc_findStop(int32 search_handle);

int32 mrc_drawChar(uint8* chr, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, uint16 font);

int32 mrc_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);

int32 mrc_drawTextEx(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font);

int32 mrc_drawTextLeft(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font);

int32 mrc_drawTextRight(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font);

uint16* mrc_c2u(char *cp, int32 *err, int32 *size);

uint16* mrc_c2uVM(char *cp, int32 *err, int32 *size);

int32 mrc_textWidthHeight(char* pcText, int is_unicode, uint16 font, int32* w, int32* h);

int32 mrc_unicodeTextWidthHeight(uint16* pcText, uint16 font, int32* w, int32* h);

int32 mrc_unicodeTextRow(uint16* pcText, uint16 font, int32 w);

int32 mrc_getScreenInfo(mr_screeninfo * screeninfo);

void mrc_drawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);

void mrc_drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 r, uint8 g, uint8 b);

void mrc_drawPoint(int16 x, int16 y, uint16 nativecolor);

void mrc_drawPointEx(int16 x, int16 y, int32 r, int32 g, int32 b);

void mrc_clearScreen(int32 r, int32 g, int32 b);

void mrc_refreshScreen(int16 x, int16 y, uint16 w, uint16 h);

void mrc_EffSetCon(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb);

int32 mrc_bitmapLoad(uint16 i, char* filename, int16 x, int16 y, uint16 w, uint16 h, uint16 max_w);

int32 mrc_bitmapShow(uint16 i, int16 x, int16 y, uint16 rop, int16 sx, int16 sy, int16 w, int16 h);

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


int mrc_bitmapShowExTrans(uint16* p,
   int16 x,
   int16 y,
   int16 mw,
   int16 w,
   int16 h,
   uint16 rop,
   int16 sx,
   int16 sy,
   uint16 transcolor
);

int mrc_bitmapNew(
   uint16 i,
   uint16 w,
   uint16 h
);

int mrc_bitmapDraw(
   uint16 di,
   int16 dx,
   int16 dy,
   uint16 si,
   int16 sx,
   int16 sy,
   uint16 w,
   uint16 h,
   int16 A,
   int16 B,
   int16 C,
   int16 D,
   uint16 rop
);

int mrc_bitmapGetScreen(
   uint16 i
);

int32 mrc_spriteSet(
   uint16 i,
   uint16 h);

int32 mrc_spriteDraw(
   uint16 i,
   uint16 spriteindex,
   int16 x,
   int16 y,
   uint16 mod
);

int32 mrc_spriteDrawEx(
   uint16 i,
   uint16 spriteindex,
   int16 x,
   int16 y,
   int16 A,
   int16 B,
   int16 C,
   int16 D
);

int32 mrc_spriteCheck(uint16 i, uint16 spriteindex, int16 x, int16 y,uint8 r, uint8 g, uint8 b);


int32 mrc_tileInit(
   uint16 i,
   int16 x,
   int16 y,
   uint16 w,
   uint16 h,
   uint16 tileh
   );

int32 mrc_tileSetRect(
   uint16 i,
   int16 x1,
   int16 y1,
   int16 x2,
   int16 y2
);

int32 mrc_tileDraw(
   uint16 i
);

int32 mrc_tileShift(
   uint16 i,
   uint16 mode
);

int32 mrc_tileLoad(
   uint16 i,
   char* filename
);

int32 mrc_tileLoadEx(
   uint16 i,
   uint8* buf,
   int32 len
);

int32 mrc_tileGetTile(
   uint16 i,
   uint16 x,
   uint16 y
);

int32 mrc_tileSetTile(
   uint16 i,
   uint16 x,
   uint16 y,
   uint16 v
);




int32 mrc_menuCreate(const char* title, int16 num);

int32 mrc_menuSetItem(int32 menu, const char *text, int32 index);

int32 mrc_menuShow(int32 menu);

int32 mrc_menuRelease(int32 menu);

int32 mrc_menuRefresh(int32 menu);

int32 mrc_dialogCreate(const char * title, const char * text, int32 type);

int32 mrc_dialogRelease(int32 dialog);

int32 mrc_dialogRefresh(int32 dialog, const char * title, const char * text, int32 type);

int32 mrc_textCreate(const char * title, const char * text, int32 type);

int32 mrc_textRelease(int32 text);

int32 mrc_textRefresh(int32 handle, const char * title, const char * text);

int32 mrc_editCreate(const char * title, const char * text, int32 type, int32 max_size);

int32 mrc_editRelease(int32 edit);

const char* mrc_editGetText(int32 edit);

int32 mrc_winCreate(void);

int32 mrc_winRelease(int32 win);




int32 mrc_getNetworkID(void);

typedef int32 (*MR_INIT_NETWORK_CB)(int32 result);

int32 mrc_initNetwork(MR_INIT_NETWORK_CB cb, const char *mode);

int32 mrc_closeNetwork(void);

typedef int32 (*MR_GET_HOST_CB)(int32 ip);

int32 mrc_getHostByName(const char *name, MR_GET_HOST_CB cb);

int32 mrc_socket(int32 type, int32 protocol);

int32 mrc_connect(int32 s, int32 ip, uint16 port, int32 type);

int32 mrc_getSocketState(int32 s);

int32 mrc_closeSocket(int32 s);

int32 mrc_recv(int32 s, char *buf, int len);

int32 mrc_recvfrom(int32 s, char *buf, int len, int32 *ip, uint16 *port);

int32 mrc_send(int32 s, const char *buf, int len);

int32 mrc_sendto(int32 s, const char *buf, int len, int32 ip, uint16 port);


int32 mrc_soundSet(uint16 i, char * filename, int32 type);

int32 mrc_soundPlay(
   uint16 i,
   int32 loop
);

int32 mrc_soundStop(
   uint16 i
);

int32 mr_playSoundEx(int type, const void* data, uint32 dataLen, int32 loop);


int32 mr_stopSoundEx(int type);


int32 mrc_timerCreate (void);

void mrc_timerDelete (int32 t);

void mrc_timerStop (int32 t);

typedef void (*mrc_timerCB)(int32 data);

int32 mrc_timerStart (int32 t, int32 time, int32 data, mrc_timerCB f, int32 loop);


int32 mrc_timerSetTimeEx (int32 t, int32 time);


void mrc_exit(void);

int32 mrc_getUptime(void);


int32 mrc_getDatetime(mr_datetime* datetime);

uint32 mrc_getMemoryRemain(void);

int32 mrc_startShake(int32 ms);

int32 mrc_getVersion(void);

char* mrc_getPackName(void);

int32 mrc_setPackName(char * name);

int32 mrc_getSysMem(void);

extern int32 mrc_init(void);

extern int32 mrc_event(int32 code, int32 param0, int32 param1);
#define mrc_appEvent mrc_event
extern int32 mrc_pause(void);

extern int32 mrc_resume(void);

extern int32 mrc_exitApp(void);  

extern int32 mrc_eventEx(int32 code, int32 param0, int32 param1, int32 param2, int32 param3);

int32 mrc_readFileFromMrpEx(char* packname, const char* filename, 
               uint8 ** filebuf, int32 *filelen, int32 lookfor);

int32 mrc_readFileFromMrpExA(char* packname, const char* filename, 
               uint8** filebuf, int32 *filelen, int32 lookfor);

int32 mrc_readFileFromMrpExPart(char* packname, const char* filename, 
               uint8 ** filebuf, int32 *filelen, int32 offset, int32 read_len);

void * mrc_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor);

void mrc_freeFileData(void* data, int32 filelen);

void mrc_freeOrigin(void* add, int32 size);

uint32 mrc_updcrc(uint8 *s, uint32 n);


void mrc_refreshScreenTrigger(int16 x, int16 y, uint16 w, uint16 h);
void mrc_refreshScreenUnTrigger(void);


typedef unsigned char md5_byte_t;
typedef unsigned int md5_word_t;

typedef struct md5_state_s {
    md5_word_t count[2];
    md5_word_t abcd[4];
    md5_byte_t buf[64];
} md5_state_t;

extern void mrc_md5_init(md5_state_t *pms);

extern void mrc_md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);

extern void mrc_md5_finish(md5_state_t *pms, md5_byte_t digest[16]);

extern int32 mrc_platDrawTextFlag;

extern void mrc_platDrawChar(uint16 ch, int32 x, int32 y, int32 color);

extern int32 mrc_exRamSSBInit(void);

extern int32 mrc_exRamInit(void);

extern int32 mrc_exRamRelease(void);

extern int32 mrc_exRamSSBRelease(void);

extern void* mrc_exRamMalloc(int size);

extern void mrc_exRamFree(void *address);

extern void* mrc_exRamMallocOnly(int size);

extern void mrc_exRamFreeOnly(void *address);

extern int32 mrc_exRamStore(void);

extern int32 mrc_exRamLoad(void);

extern int mrc_getMemStatus(int * mainUsed, int * mainLeft, 
					 int * ssbUsed, int * ssbLeft,  
					 int * sbasmUsed, int * sbasmLeft);


#endif
