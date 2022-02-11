#ifndef __PLATEX_H__
#define __PLATEX_H__
#include <mrc_base.h>
//led背光开关
void ledSet(int isOpen);

//获取短信中心号码
void getSmsCenter(void);

//检测是否触屏
int32 isTouchScreen(void);

//获得手机语言
int32 getLanguage(void);

//查询t卡状态
int32 getTSDCardState(void);

//获得当前情景模式
int32 getModel(void);

//设置播放音量
int32 setVolue(int32 volue);

#endif
