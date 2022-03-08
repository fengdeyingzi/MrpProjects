#ifndef __BASE_H__
#define __BASE_H__

// 以下定义未在虚拟机定义，一般可根据需要修改或添加
// 部分类型在虚拟机使用，切勿修改（typedef类型）

#define MR_SUCCESS 0
#define MR_FAILED  -1
#define MR_IGNORE  1
#define MR_WAITING 2

#define IS_FILE    1
#define IS_DIR     2
#define IS_INVALID 8

#define _RDONLY    1
#define _WRONLY    2
#define _RDWR      4
#define _CREATE    8
#define _SHARE     16

enum{
_0,
_1,
_2,
_3,
_4,
_5,
_6,
_7,
_8,
_9,
_STAR,    //*
_POUND,   //#
_UP,      //上
_DOWN,    //下
_LEFT,    //左
_RIGHT,   //右
_POWER,   //电源键
_SLEFT,   //左软键
_SRIGHT,  //右软键
_SEND,    //接听键
_SELECT,  //确认/选择
_VUP,     //音量+
_VDOWN,   //音量-
_KEY_MAX
};

enum{
KY_DOWN,
KY_UP,
MS_DOWN,
MS_UP,
MN_SLT,
MN_RET,
MR_DIALOG,
MR_LOCALUI,
MS_MOVE = 12
};

enum{
_NET_MOBILE,
_NET_CN,
_NET_CDMA,
_NET_NONE
};

enum{
_EDIT_ANY,
_EDIT_NUMERIC,
_EDIT_PASSWORD
};

enum{
DLG_OK,
DLG_CANCEL
};

enum{
SEEK_SET,
SEEK_CUR,
SEEK_END
};

enum{
FONT_SMALL,
FONT_MEDIUM,
FONT_BIG
};

enum{
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

typedef signed char    int8;
typedef unsigned char  uint8;
typedef signed short   int16;
typedef unsigned short uint16;
typedef signed long    int32;
typedef unsigned long  uint32;

typedef struct{
int16 x;
int16 y;
uint16 w;
uint16 h;
}rectst;

typedef struct{
uint8 r;
uint8 g;
uint8 b;
}colorst;

typedef struct{
uint16 year;
uint8 month;
uint8 day;
uint8 hour;
uint8 minute;
uint8 second;
}mr_datetime;

#endif