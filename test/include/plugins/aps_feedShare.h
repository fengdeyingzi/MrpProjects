#ifndef APS_FEEDSHARE_H_
#define APS_FEEDSHARE_H_


#include "mrc_base.h"
#include "mrc_base_i.h"
#include "aps_main.h"
#include "mrc_feedShare.h"
#ifdef __cplusplus
extern "C"{
#endif

/*
	分享冒泡官网以及冒泡空间、新浪微博
*/
#define FEEDSHARE_TARGET	7	

#define FEEDSHARE_MRP_NAME	"fedd.mrp"
#define FEEDSHARE_MRP_FOLDER	"plugins"
#define FEEDSHARE_EXT_NAME	"fedd.ext"


#define FEEDSHARE_APPID			 490092
#define FEEDSHARE_REQ_VERSION	 13
#define APS_EXT_ID_FEEDSHARE	 10092

// 插件功能号
enum
{
	APS_FEEDSHARE_SETSKYID,
	APS_FEEDSHARE_UPLOADFEED,
	APS_FEEDSHARE_ATTEND,
	APS_FEEDSHARE_SETUIFALG,
	APS_FEEDSHARE_SETTARGETS,
	APS_FEEDSHARE_UPLOADFEEDEX
};

enum
{
	E_PASSPORT_UPDATE_PLUGINS=4
};

typedef struct  
{
	uint8* paramData;
	int32 dataLen;
}feedShare_paramInfo;

#ifndef SDK_MOD

#define aps_feedShare_loadExt()\
	aps_main_loadExt(APS_EXT_ID_FEEDSHARE,FEEDSHARE_MRP_NAME,FEEDSHARE_MRP_FOLDER,FEEDSHARE_EXT_NAME)
#define aps_feedShare_loadExtDump()	\
	aps_main_loadExt_dump(APS_EXT_ID_FEEDSHARE,FEEDSHARE_MRP_NAME,FEEDSHARE_MRP_FOLDER,FEEDSHARE_EXT_NAME,NULL)
#else
	int32 aps_feedShare_loadExt();
	int32 aps_feedShare_loadExtDump();
#endif


/*立即卸载feedShare模块*/	
#define	aps_feedShare_unloadExt() \
aps_main_unloadExt(APS_EXT_ID_FEEDSHARE,0)

/*延迟卸载feedShare模块*/
#define aps_feedShare_delay_unloadExt() \
aps_main_unloadExt(APS_EXT_ID_FEEDSHARE,1)

/*卸载账号UI模块*/
#define aps_feedShare_unloadExtDump()  \
	aps_main_unloadExt_dump(APS_EXT_ID_FEEDSHARE)

/*暂停feedShare模块*/	
#define aps_feedShare_pauseExt() \
aps_main_pauseExt(APS_EXT_ID_FEEDSHARE)

/*继续feedShare模块*/	
#define aps_feedShare_resumeExt() \
aps_main_resumeExt(APS_EXT_ID_FEEDSHARE)

/*得到feedShare模块版本号*/
#define aps_feedShare_getExtVersion() \
	aps_main_getExtVersion(APS_EXT_ID_FEEDSHARE)


/*设置skyid*/
#define	aps_feedShare_setSkyid(skyid) \
	mrc_extSendAppEventEx(APS_EXT_ID_FEEDSHARE,APS_FEEDSHARE_SETSKYID,skyid,0,0,0,0)
	
/*上传feed*/
#define aps_feedShare_feedUpload(templates,parameters,netInfo,dumpMode,cb) \
	mrc_extSendAppEventEx(APS_EXT_ID_FEEDSHARE,APS_FEEDSHARE_UPLOADFEED,(int32)templates,(int32)parameters,(int32)netInfo,(int32)dumpMode,(int32)cb)

/*上传feed的增强接口*/
#define aps_feedShare_feedUploadEx(templates,parameters,netInfo,dumpMode,cb) \
	mrc_extSendAppEventEx(APS_EXT_ID_FEEDSHARE,APS_FEEDSHARE_UPLOADFEEDEX,(int32)templates,(int32)parameters,(int32)netInfo,(int32)dumpMode,(int32)cb)

/*设置是否带UI*/
#define aps_feedShare_setUIFlag(isShow)	\
	mrc_extSendAppEventEx(APS_EXT_ID_FEEDSHARE,APS_FEEDSHARE_SETUIFALG,(int32)isShow,0,0,0,0)

/*设置目标*/
#define aps_feedShare_setTargets(targets)	\
	mrc_extSendAppEventEx(APS_EXT_ID_FEEDSHARE,APS_FEEDSHARE_SETTARGETS,(int32)targets,0,0,0,0)

/*关注*/
#define aps_feedShare_attend(descSkyid,cb)	\
	mrc_extSendAppEventEx(APS_EXT_ID_FEEDSHARE,APS_FEEDSHARE_ATTEND,(int32)descSkyid,(int32)cb,0,0,0)

#ifdef __cplusplus
}
#endif
#endif