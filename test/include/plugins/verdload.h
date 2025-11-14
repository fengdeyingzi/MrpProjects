#ifndef _VERDLOAD_H_
#define _VERDLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* 下载状态 */
typedef enum
{
	MR_DL_STATUS_DIALING = 0,              /* 正在拨号*/
    MR_DL_STATUS_CONNECTING,               /* 正在连接下载服务器 */
	MR_DL_STATUS_DOWNLOADING,              /* 正在下载数据 */
    MR_DL_STATUS_COMPLETE,                 /*下载完成     */

	MR_DL_STATUS_EXCEPTION                /*下载异常     */
}MR_DL_STATUS;

typedef enum
{
	MR_DL_ERROR_REASON_NONE = 0,          /* 正常    */ 
    MR_DL_ERROR_REASON_DIAL,              /*拨号错误 */
	MR_DL_ERROR_REASON_CONNECT,           /*连接下载服务器错误*/
	MR_DL_ERROR_REASON_NOT_FOUND,         /*服务器未找到需下载资源*/
	MR_DL_ERROR_REASON_TIMEOUT,           /*下载超时*/
	MR_DL_ERROR_REASON_FILE,              /*文件错误*/

	MR_DL_ERROR_REASON_CANCEL               /*用户中止下载*/                
}MR_DL_ERROR_REASON;


#define MR_DL_FLAG_UIBYAPP   0x0001    /* 由client来绘画下载过程UI*/
#define MR_DL_FLAG_UIHINT    0x0002    /* 在由verdload来绘画下载过程时，是否需要下载提示界面 */
#define MR_DL_FIAG_HINT_EX	 0x0006	   /* 使用确认对话框背景透明效果*/


/*配置预先网络拨号信息，应用如果已经拨号,则不需要进行拨号*/
typedef enum
{
	DL_PRECONFIG_NETWORK_NONE = 0,             /*无预先网络配置*/
	DL_PRECONFIG_NETWORK_CMWAP,                /*预先网络CMWAP */
	DL_PRECONFIG_NETWORK_CMNET                 /*预先网络CMNET */ 
}MR_DL_PRECONFIG_NETWORK;

/*下载过程回调定义
  
   status:    当前通知的状态
   reason:    错误原因,仅在 MR_DL_STATUS_EXCEPTION时有效，其它状态下为MR_DL_ERROR_REASON_NONE
   p1:        status == MR_DL_STATUS_DOWNLOADING 时为总下载百分比值,如20%,p1=20,
              其它status状态下为备用参数
   (p2-p3):   status == MR_DL_STATUS_DOWNLOADING 时p2 为当前已下载数据，p3为应下载全部数据长度
              其它status状态下目前作为备用参数
	!!!!!!!!!注意dump模式加载的下载插件不支持实时进度的通知!!!!!!!!!!!!
*/
typedef void (*MR_DL_CB)(MR_DL_STATUS status,MR_DL_ERROR_REASON reason,int32 p1,int32 p2,int32 p3);

/*
    初始化下载任务,当前仅支持一个任务的下载,不能在下载任务未完成之前发起
 另一个下载任务。

    输入: 
	  srcid:      被下载的资源ID,此ID能够被用来唯一地标识一个资源
	  pathName:   下载的文件被保存的目的绝对路径(路径+文件名)
      cb:         下载通知回调

	  ui:          ui的取值有三种可能：
		           0                       下载进度界面由下载插件负责,下载前不让用户确认。
				   VERDLOAD_FLAG_HINT      下载进度界面由下载插件负责，下载前让用户确认。
				   VERDLOAD_FLAG_UIBYAPP   下载界面由APP负责, 下载插件不负责任何的UI。
				   
	  title:       下载界面标题,ui & VERDLOAD_FLAG_UIBYAPP == 0 时需要提供
	  hint:        下载确认界面的提示,ui =VERDLOAD_FLAG_HINT 时需要提供
	输出:
	  MR_SUCCESS:  成功
	  MR_FAILED:   失败


	***NOTE********************************************************************
	 如果 返回值为MR_SUCCESS 并且 ui 为 0 或者 VERDLOAD_FLAG_HINT, 
               那么应用必须停止自身的界面刷新， 否则会和下载插件的UI冲突。
    ****************************************************************************
*/

int32 mrc_dl_file_start(int32 fileid,const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
  功能与 mrc_dl_file_start一致,差别在于使用dump方式加载
*/
int32 mrc_dl_file_startDump(int32 fileid,const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
  功能与 mrc_dl_file_start一致,差别在于增加了预设网络的配置,用来指示在下载过程中需不需要重新拨号
  输入:
	precfg_network:  预先的拨号环境,如果不为DL_PRECONFIG_NETWORK_NONE，则下载过程中将不进行重新拨号动作	
  输出:
	 MR_SUCCESS:  成功
	 MR_FAILED:   失败
*/
int32 mrc_dl_file_startEx(int32 fileid,const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint,MR_DL_PRECONFIG_NETWORK precfg_network);

/*
  功能与 mrc_dl_file_startEx一致,差别在于使用dump方式加载
*/
int32 mrc_dl_file_startExDump(int32 fileid,const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint,MR_DL_PRECONFIG_NETWORK precfg_network);

/*
    功能与 mrc_dl_file_start一致，差别在于下载的文件通过代理到曲奇下载
	输入：
	srcVer:		被下载资源的目标版本号
	reqVer:		被下载资源的最低版本号
	其它参数与mrc_dl_file_start一致
*/
int32 mrc_dl_fileFromCookie_start(int32 appid,int32 srcVer, int32 reqVer, const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
  功能与 mrc_dl_fileFromCookie_start一致,差别在于使用dump方式加载
*/
int32 mrc_dl_fileFromCookie_startDump(int32 appid,int32 srcVer, int32 reqVer, const char *pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
功能与 mrc_dl_file_start一致，差别在于下载的文件通过代理到超大下载
输入：
fileName:	下载文件名
cpCode:		CP唯一的下载码
其它参数与mrc_dl_file_start一致
*/
int32 mrc_dl_fileByFileName_start(char* fileName, int16 cpCode, const char* pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
  功能与 mrc_dl_fileByFileName_start一致,差别在于使用dump方式加载
*/
int32 mrc_dl_fileByFileName_startDump(char* fileName, int16 cpCode, const char* pathName, MR_DL_CB cb,int32 ui,uint16 *title,uint16 *hint);

/*
    取消当前下载任务
	输出:
	  MR_SUCCESS: 成功
	  MR_FAILED: 失败
*/
int32 mrc_dl_file_cancel(void);

/*
	设置调用者的extId，以区分调用者
	输入：调用插件的extID
	输出：无
*/
void mrc_dl_set_ExtID(int32 extID);

#ifdef __cplusplus
}
#endif
#endif