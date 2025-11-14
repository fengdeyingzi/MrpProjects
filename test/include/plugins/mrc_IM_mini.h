#ifndef _MRC_IM_MINI_H_
#define _MRC_IM_MINI_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	IM_EVENT_START_DIAL,				//开始拨号
	IM_EVENT_DIALED_DIAL_SUCCESS,		//拨号成功
	IM_EVENT_DIALED_DIAL_FAILED,		//拨号失败

	IM_EVENT_CONNECT_SUCCESS,			//连接服务器成功
	IM_EVENT_CONNECT_TRY_NEXT,			//服务器重定向, 连接下一个服务器
											
	IM_EVENT_DISCONNECTED,			//与服务器连接断开，会尝试重连服务器
	IM_EVENT_CONNECT_FAILED,			//连接服务器失败
	/***************************************************************************/
	IM_EVENT_LOGIN_SUCESS,					//IM登录验证成功
	IM_EVENT_LOGIN_FAILED,	 //IM登录验证失败或超时

	IM_EVENT_WIN_OPEN,		//聊天窗口开启，需要游戏配合绘制全屏
	IM_EVENT_WIN_CLOSE		//聊天窗口关闭

}im_event;


typedef enum
{	
	MSG_STA_CONNECTING,//未连上，连接中
	MSG_STA_REDIRECTING,//未连上，重定向中
	MSG_STA_CONNECTED,//连接保持中
	MSG_STA_RECONNECTING,//未连上，重连接中
	MSG_STA_DISCONNECTED//未连上，重连失败
}msgStatus;
/*********************************回调函数定义****************************************************************/

/*
  IM mini的回调通知函数
*/
typedef void (*msgStatusNotifyCb)(im_event event);


/*消息模块接收回调
pData:	消息内容，不需要释放
len:      消息长度
*/
typedef void (*msg_recv_cb)(uint8* pData,int32 len,uint16 srcid);


/************************IM_MINI**********************************/
/*
	函数名：mrc_IM_mini_start
	功能：启动IM mini，连接接入服务
	输入：statusCb，
	输出：返回值
	MR_SUCCESS,启动IM插件成功
	MR_FAILED,启动IM插件失败
*/
int32 mrc_IM_mini_start(msgStatusNotifyCb statusCb);



/*
	函数名：mrc_IM_mini_off_line_hint
	功能：绘制掉线提示，调用后只要掉线后会跳出窗口提示游戏重启
*/
int32 mrc_IM_mini_off_line_hint(void);


/*
	函数名：mrc_IM_mini_login
	功能：立即尝试一次好友登录，结果仍用mrc_IM_mini_start的回调来通知

*/
int32 mrc_IM_mini_login(void);


/* 
	函数名：mrc_IM_mini_set_draw_pos
	功能：设置消息提示图标绘制的位置
	输入：x,y 绘制的位置中心点位置坐标

    当x=-1，y=-1时表示不显示消息提示图标，并且不提示网络回调
	当x=-2，y=-2时表示不显示消息提示图标，但是会提示网络回调
	当x，y为正常坐标设置时会显示消息提示图标，并且提示网络回调
*/

int32 mrc_IM_mini_set_draw_pos(int32 x,int32 y);


/*
	函数名：mrc_IM_mini_isVisible
	功能：	获取当前的IM可见设置的状态
	输出：0，不可见且无网络回调
		  1，不可见但有网络回调
		  2，可见且有网络回调
*/
int32 mrc_IM_mini_isVisible(void);


/*  
	函数名：mrc_IM_mini_restart
	功能：重新建立网络连接，其他状态维持不变
*/

int32 mrc_IM_mini_restart(void);


/*  
	函数名：mrc_IM_mini_stop
	功能：关闭IM_mini
*/

int32 mrc_IM_mini_stop(void);



/************************MSG的接口使用************************/

/*向消息模块注册
输入参数 
extId :模块id
eventCB :事件回调函数
recvCB  :消息接收回调函数
返回  	MR_SUCCESS :注册成功
MR_FAILED			:注册失败*/
int32 mrc_msgRegist(int32 srcId, msg_recv_cb recvCB);


/*取消注册消息句柄
输入参数 
extId :模块id
返回  	MR_SUCCESS :取消成功
MR_FAILED     :取消失败*/
int32 mrc_msgUnRegist(int32 srcId);


/*添加发送消息
输入参数 
srcId   :终端的模块号
dstId	:服务端模块号
pData   :发送消息的内容指针，函数调用后可自行释放
len     :发送消息的长度
返回
MR_SUCCESS :添加成功
MR_FAILED  :添加失败*/
int32 mrc_msgSendMsg(int32 srcId,int32 dstId,uint8* pData, int32 len);



/*添加发送消息扩展
相比mrc_msgSendMsg
可以指定消息的标识位tag
*/
int32 mrc_msgSendMsgEx(int32 extId,int32 serId,uint8* pData, int32 len,int32 tag);

/*
打包消息预备空间

  输入参数:
	srcId   :终端的模块号
	dstId	:服务端模块号
	length  :发送消息的长度

  返回 存放消息地址
*/
uint8* mrc_msgPrepare(int32 srcId,int32  dstId, int32 length);


/*
发送打包消息
  输入参数：
  handle：没有用到
  pData：调用mrc_msgPrepare返回的存放消息地址
  seqId：没有用到
  priority：没有用到	

*/
int32 mrc_msgSendPacket(int32 handle, uint8* pData, int32 seqId, int32 priority);


/*
获取msg的状态
  输入参数：
返回：msg的当前状态	

*/

msgStatus mrc_msgGetStatus(void);

/*
暂停消息接收
请求msg存储n条消息
输入参数:	num 消息条数
srcId 终端模块号
返回
MR_SUCCESS :成功
MR_FAILED  :失败
*/
int32 mrc_msgPause(uint8 num,int32 srcId);

/*
恢复接收消息
输入参数:	srcId 终端模块号
如果期间有存储的消息，会立即发送
*/
int32 mrc_msgResume(int32 srcId);

#ifdef __cplusplus
}
#endif

#endif
