#include "mrc_base.h"
#include "verdload.h"

#ifndef _VERCHECK_LIB_H_
#define _VERCHECK_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif


#define VER_CHECK_MAX_MRP_NAME     64

enum
{
	PLATFORM_INVALID	= 0,
	PLATFORM_MTK		= 21,		/* 由于历史原因, 为避免和之前的phoneType冲突, 从21开始定义平台 */
	PLATFORM_SPR		= 22,
	PLATFORM_SYMBIAN	= 23,
	PLATFORM_VIA		= 24,
	PLATFORM_MSTAR		= 25,
	/* append platform define above */
	PLATFORM_MAX
};

/*******************************************************************************
* product screen size define
*******************************************************************************/
enum
{
	SCREEN_INVALID		= 0,
	SCREEN_240X320		= 1,
	SCREEN_240X400		= 2,
	SCREEN_176X220		= 3,
	/* append screen size define above */
	SCREEN_MAX
};

/*******************************************************************************
* plugins type define
*******************************************************************************/
enum
{
	PLUGIN_INVALID 		= 0,
		PLUGIN_BASEMODULE 	= 1,	//基础插件
		PLUGIN_APPMODULE 	= 2,	//应用插件
		PLUGIN_BRWMODULE 	= 3,	//浏览器插件
		PLUGIN_BRWAPP	 	= 4,	//浏览器应用插件
		PLUGIN_MAX
};

typedef struct
{
	uint16	nMsgCode;		//消息ID
	uint16	PlatType;		//平台类型
	uint16  nScrType;		//屏幕类型
	uint32 	appID;			//appID
}REQ_PLUGIN_VER_PACKET;

typedef struct
{
	uint32 nPluginId;			//插件的ID
	uint32 nVersion;			//插件版本号
}PLUGIN_VER_REC;

typedef struct  
{
	uint16 nPluginNum;	//依赖插件信息
	PLUGIN_VER_REC *nPluginVer;
}PLUGIN_VER_DEPEND_INFO;

typedef struct __tagPluginRspInfo
{
	uint32 	appID;		//插件ID
	uint32 	appVersion;	//插件版本
	uint16 	appType;	//插件类型
	uint16 	rebootFlag;	//重启标识
	uint16 	updateFlag;	//更新标识
	uint16  appNameLen;	//app名称长度
	uint8  	mrpName[VER_CHECK_MAX_MRP_NAME];	//MRP文件名称
	
}PLUGIN_VERSION_RSPINFO;


typedef struct __tagUpdatePluginInfo
{
	uint16	nMsgCode;		//消息ID
	uint16 	nPluginCount;	//插件个数
	PLUGIN_VERSION_RSPINFO *VerRspInfo;	//插件详细信息
	
}UPDATE_PLUGIN_INFO;

typedef void (*update_plugin_cb)(UPDATE_PLUGIN_INFO *pUpPlugInfo);

/*
* 发送版本信息请求,用于从服务器获取当前MRP依赖的模块信息
* 输入参数:
* 	info: 当前MRP信息,应用ID，平台类型，屏幕类型;消息ID可以为零;
*  pDependInfo: 依赖的插件信息，插件个数，插件ID，插件版本号;
*  cb:   处理结果的回调函数.
*  在回调处理函数，用户可以保存需要的版本更新信息;并调用下载模块接口下载相应的插件.
*  如果回调函数的函数参数pUpPlugInfo为NULL，则表示没有收到版本信息响应或者网络错误.
* typedef void (*update_plugin_cb)(UPDATE_PLUGIN_INFO *pUpPlugInfo);
* 在完成版本更新下载处理后，需要调用mrc_vercheck_verInfo_Release来释放相关资源.
 */
int32 mrc_vercheck_verInfo_sendReq(REQ_PLUGIN_VER_PACKET *info,PLUGIN_VER_DEPEND_INFO *pDependInfo,update_plugin_cb cb);

/*
* 释放调用 mrc_vercheck_verInfo_sendReq 占用的资源;
* 确保在一些网络异常时，内存资源泄漏;
* 如果获取更新插件后,在开始下载更新插件之前，必须调用该接口释放内存和网络资源
 */
int32 mrc_vercheck_verInfo_Release(void);

/* 下载更新插件,相应的更新信息通过调用接口mrc_vercheck_verInfo_sendReq中配置的回调函数来获得;
* 需要注意的是，在获得版本更新信息后，必须调用mrc_vercheck_verInfo_Release释放相应的网络资源;
* 输入参数:
* 	pVerRspInfo: 需要更新的插件信息
*  cb:			 下载状态提示的回调函数
* 在回调函数里，需要对异常的网络下载状态进行必要的处理.
 */

int32 mrc_vercheck_verInfo_UpdatePlugin(PLUGIN_VERSION_RSPINFO *pVerRspInfo,MR_DL_CB cb);
/*
* 更新本地插件接口
* 从当前MRP文件中读取plugins.lst文件,从plugins.lst文件中获取相应的MRP版本号
* 检查本地目录是否存在对应的MRP文件，如果不存在的话，则从当前MRP文件中解压到本地目录
* 如果本地目录存在对应的MRP文件，且版本号比当前MRP中的版本号要小，即当前MRP中打包的MRP版本较新，
* 则从当前MRP文件中解压打包的mrp文件覆盖到本地目录中的文件.
 */
int32 mrc_vercheck_verInfo_UpdateMRP(void);
/*
 * 扫描扩展内存
 */
int32 mrc_vercheck_MemoryScan(void);

/* unicode字符串连接 */
//int32 mrc_vercheck_uniStrcat(char *dest, char *src);

/* unicode 字符串拷贝 */
//int32 mrc_vercheck_uniStrcpy(char * dest, char * src);

/* 格式化输出字符串 */
//int32 mrc_vercheck_unisprintf(char * dst,  const char * format, ...);

/* 数据解包函数 */
//int32 mrc_vercheck_unpack(char *in, int inLen, const char* fmt, ...);

//int32 mrc_vercheck_pack(char *out, const char* fmt, ...);
/*
用于去除输入中的空格和回车的非法输入
*/
//char * mrc_vercheck_uniStrtrim(char* string);

/* 过滤字符串的回车换行字符 */
//char * mrc_vercheck_uniStrtrimLF(char* string);

int32 mrc_vercheck_unloadVerd(void);
int32 mrc_vercheck_loadVerd(void);
int32 mrc_vercheck_preconfig_net(int32 pre_cfgnet);


#ifdef __cplusplus
}
#endif

#endif
