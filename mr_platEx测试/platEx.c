#include <mrc_base.h>
#include <mrc_exb.h>
#include <mrc_base_i.h>
typedef void (*MR_PLAT_EX_CB)(uint8* output, int32 output_len);
extern int32 mrc_platEx(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb);
extern int32 mrc_plat(int32 code, int32 param);

//led背光开关
void ledSet(int isOpen){
    int32 output_len = 0;
    if(isOpen){
        mrc_platEx(1222,NULL,0,NULL,&output_len,NULL);
    }else{
        mrc_platEx(1223,NULL,0,NULL,&output_len,NULL);
    }
}
//获取短信中心号码
void getSmsCenter(void){
    mrc_plat(1106, 0);
}
//检测是否触屏
int32 isTouchScreen(void){
    int32 temp = mrc_plat(1205,0);
    if(temp != MR_FAILED && temp!=MR_IGNORE){
        return temp - MR_PLAT_VALUE_BASE;
    }
    return temp;
}
//获得手机语言
int32 getLanguage(void){
    int32 temp = mrc_plat(1206,0);
    if(temp != MR_FAILED && temp!=MR_IGNORE){
        return temp - MR_PLAT_VALUE_BASE;
    }
    return temp;
}
//查询t卡状态 已存在
int32 getTSDCardState(void){
    return mrc_plat(1218,0);
}
//获得当前情景模式
int32 getModel(){
    return mrc_plat(1213,0);
}
//设置播放音量
int32 setVolue(int32 volue){
    return mrc_plat(1302,volue);
}