#ifndef __MRC_FEED_H_
#define __MRC_FEED_H_
#include "mrc_base.h"
#include "mrc_base_i.h"
#include "verdload.h"


typedef enum
{
	MR_FEEDErrorCode_SUCCESS = 0,			/* 成功           */
	MR_FEEDErrorCode_UserCancel,			/* 用户取消       */
	MR_FEEDErrorCode_Network,				/* 网络错误       */
	MR_FEEDErrorCode_FormatError,			/*格式错误		  */
	MR_FEEDErrorCode_BlackListUser,			/* 黑名单用户     */ 
	MR_FEEDErrorCode_updatePlugins,			/*更新插件		  */
// 	MR_FEEDErrorCode_updateLst,				/*更新列表		  */
	MR_FEEDErrorCode_invalidUser=7,			/*用户不存在	  */
	MR_FEEDErrorCode_serverBusy,			/*服务器忙	   	  */
	MR_FEEDErrorCode_otherError				/*其他错误		  */
}MR_FEEDErrorCode;

/*
	errorCode:详见MR_FEEDErrorCode
	resultMsg:服务端返回的提示字符串	
*/
typedef void (*MR_FEED_CB)(int32 errorCode,uint16* resultMsg);

/*
	初始化feed分享插件
	输入:
		MR_DL_CB cb:下载完成的回调
	输出:	
		无
	返回值：
		MR_FAILED 失败，回调为NULL
		MR_WAITING	等待，正在下载插件
		MR_SUCCESS 成功，通过本地版本检查
*/
int32 mrc_feed_initVersion(MR_DL_CB cb);

/*
	调用feed分享插件，带UI
	输入:
		int32 isDump:是否用dump模式加载，0表示用非dump模式加载
						1表示用dump模式加载
		MR_FEED_CB cb:退出插件的回调
		char* templates:模板，用ascii编码，包含格式化输入符
		可变参数:
			%d	Int32类型的格式化输入
			%s	Ascii编码字符串
			在输入可变参数时，需同时输入一个参数长度，一个格式化输入参数示例如下：
		示例:
		int32 data;
		char* wStr = "\x00\x73\x00\x6a\x00\x66\x00\x6b\x00\x6a\x00\x73\x00\x6b\x00\x6c\x00\x64\x00\x6a\x00\x66\x00\x00";
		data = 2;
		mrc_feed_share(0,feed_cb,"hello:%d%s",4,&data,mrc_wstrlen(wStr),wStr);
		示例说明：模板中的"%d"对应4以及&data，4表示data数据的长度，&data是数据区;模板中的"%s"对应的参数是
		mrc_wstrlen(wStr)以及wStr,mrc_wstrlen(wStr)
		是wStr的长度，wStr是数据区
	输出:	
		无
	回调：
	typedef void (*MR_FEED_CB)(int32 errorCode,uint16* resultMsg)
		int32 errorCode:MR_FEEDErrorCode错误码
		resultMsg:utf16be编码的提示字符串
	返回值：
		MR_SUCCESS 成功启动feed分享插件
		MR_WAITING 正在下载feed分享插件
		MR_FAILED	回调为空
*/
int32 mrc_feed_share(int32 isDump,MR_FEED_CB cb,char* templates,...);

/*
	调用feed分享插件，可以选择是否带UI
	输入:
		int32 isDump:是否用dump模式加载，0表示用非dump模式加载
						1表示用dump模式加载
		int32 isShow:是否带UI
		int32 target:指定分享要什么地方，如果不带UI，则target由应用指定；
					如果带UI，则全选。
					Bit-map：
						第0位：是否分享到冒泡空间
						第1位：是否分享到冒泡官网
						第2位：是否分享到新浪微博

		MR_FEED_CB cb:退出插件的回调
		char* templates:模板，用ascii编码，包含格式化输入符
		可变参数:
			%d	Int32类型的格式化输入
			%s	Ascii编码字符串
			在输入可变参数时，需同时输入一个参数长度，一个格式化输入参数示例如下：
		示例:
		int32 data;
		char* wStr = "\x00\x73\x00\x6a\x00\x66\x00\x6b\x00\x6a\x00\x73\x00\x6b\x00\x6c\x00\x64\x00\x6a\x00\x66\x00\x00";
		data = 2;
		mrc_feed_share(0,feed_cb,"hello:%d%s",4,&data,mrc_wstrlen(wStr),wStr);
		示例说明：模板中的"%d"对应4以及&data，4表示data数据的长度，&data是数据区;模板中的"%s"对应的参数是
		mrc_wstrlen(wStr)以及wStr,mrc_wstrlen(wStr)
		是wStr的长度，wStr是数据区
	输出:	
		无
	回调：
	typedef void (*MR_FEED_CB)(int32 errorCode,uint16* resultMsg)
		int32 errorCode:MR_FEEDErrorCode错误码
		resultMsg:utf16be编码的提示字符串
	返回值：
		MR_SUCCESS 成功启动feed分享插件
		MR_WAITING 正在下载feed分享插件
		MR_FAILED	回调为空
*/
int32 mrc_feed_shareEx(int32 isDump,int32 isShow,int32 target,MR_FEED_CB cb,char* templates,...);


/*
	加好友为关注，无UI
	输入:
		int32 desSkyid:目标skyid，即想加的好友的skyid
		MR_FEED_CB cb:退出插件的回调
	输出:	
		无
	回调：
	typedef void (*MR_FEED_CB)(int32 errorCode,uint16* resultMsg)
		int32 errorCode:MR_FEEDErrorCode错误码
		resultMsg:utf16be编码的提示字符串
	返回值：
		MR_SUCCESS 成功启动feed分享插件
		MR_WAITING 正在下载feed分享插件
		MR_FAILED	回调为空
*/
int32 mrc_feed_attend(int32 desSkyid,MR_FEED_CB cb);


#endif