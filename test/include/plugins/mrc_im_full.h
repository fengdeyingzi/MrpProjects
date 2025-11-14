#ifndef __MRC_IM_FULL_H__
#define __MRC_IM_FULL_H__

#include "mrc_IM_mini.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	IM_FULL_EVENT_QUIT_SUC,//正常退出
	IM_FULL_EVENT_LOAD_ERR,//加载失败
	IM_FULL_EVENT_DLOAD_ERR,//未下载完成
	IM_FULL_EVENT_MAX
}IM_CB_EVEVT;

/*
完整版IM的回调函数
code:回调事件通知
errcode:错误类型，当code=IM_FULL_EVENT_QUIT_ERR时有效
*/

typedef void  (*full_cb)(IM_CB_EVEVT code);


typedef void (*msgStatusNotifyCb)(im_event event);


/*
启动完整版的IM,退出时精简版仍然运行
输入参数:

  cb 		退出IM时回调通知函数
  status_cb 	网络状态通知

返回
*/
void mrc_im_full_start(full_cb cb, msgStatusNotifyCb status_cb);


/*
启动完整版的IM,退出时精简版也卸载
输入参数:

  cb 退出IM时回调通知函数

返回
*/
void mrc_im_full_start_alone(full_cb cb);


void start_by_mini(int32 skyid,int32 gadtype,int32 eventIndex,full_cb cb);



#ifdef __cplusplus
}
#endif

#endif