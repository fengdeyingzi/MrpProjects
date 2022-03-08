#ifndef _APS_MAIN_H_
#define _APS_MAIN_H_


#include "mrc_base_i.h"
#ifdef __cplusplus
extern "C" {
#endif


#define APS_EXT_ID_MAINMODULE    1   
#define APS_EXT_ID_GRAPHICS      10008
#define APS_EXT_ID_COMMENT	     10045
#define APS_EXT_ID_APP           255
/***************************************************
�����߿�Ϊ�Լ��Ĳ����������ϵĲ��ID�������ID��
��ѭ��ԭ����ͬһӦ�����õ��Ĳ��ID�����ظ�
 ***************************************************/

// ��appģ����� mrc_exit()ʱ������ֱ�ӵ����������mrc_exit()��
//��������ģ�鷢��һ����Ϣ��
#define APS_MAIN_CODE_EXITAPP          0
#define APS_MAIN_CODE_RUNMRP           1
#define APS_MAIN_CODE_PAUSE           34
#define APS_MAIN_CODE_RESUME          35
#define APS_MAIN_CODE_REG_ENTRY       36
#define APS_MAIN_CODE_DUMP_LOAD           41
#define APS_MAIN_CODE_DUMP_UNLOAD         42
#define APS_MAIN_CODE_FWD_CALLBACK        44
#define APS_MAIN_CODE_SWITCH_CDR          45
#define APS_MAIN_CODE_RESTORE_CDR         46
#define APS_MAIN_CODE_PAUSE_EXCEPT        47
#define APS_MAIN_CODE_RESUME_EXCEPT       48
#define APS_MAIN_CODE_VM_MALLOC           180
#define APS_MAIN_CODE_VM_FREE             181

/* ��ȡĳһ��mrp��appid��appVer��shortName */
#define APS_MAIN_CODE_GETMRPINFO               201


typedef void (*loadExt_dump_cb)(int32 result);
#ifdef SDK_MOD
	typedef int32 (*mrc_init_t)(void);
	typedef int32 (*mrc_event_t)(int32 code, int32 param0, int32 param1);
	typedef int32 (*mrc_pause_t)(void);
	typedef int32 (*mrc_resume_t)(void);
	typedef int32 (*mrc_exitApp_t)(void);
	
	typedef struct
	{
		mrc_init_t		mrc_init_f;
		mrc_event_t		mrc_event_f;
		mrc_pause_t		mrc_pause_f;
		mrc_resume_t	mrc_resume_f;
		mrc_exitApp_t	mrc_exitApp_f;
		loadExt_dump_cb load_cb;/*����ʧ��ʱ��Ҫ�˻ص�*/
	}mrcEnterFuncSt;
#endif

#ifndef SDK_MOD
/*
  aps_main_loadExt_dump
  ����extIDָ���Ĳ��
*/
#define aps_main_loadExt_dump(extID,extname,cb) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_DUMP_LOAD, (int32)(extID),0,0,(int32)(extname),(int32)cb)
#else
#define aps_main_loadExt_dump(extID,extname,enterFunc) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_DUMP_LOAD, (int32)(extID),0, 0,(int32)(extname),(int32)enterFunc)
#endif

/*ж�ز��*/
#define aps_main_unloadExt_dump(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_DUMP_UNLOAD,(int32)(extID),0,0,0,0)


#define aps_main_pauseExt(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_PAUSE,(int32)(extID),0,0,0,0)

#define aps_main_resumeExt(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_RESUME,(int32)(extID),0,0,0,0)


#define aps_main_runMrpLater(mrp_name,parameter) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RUNMRP,(int32)(mrp_name),(int32)(parameter),0,0,0)

#define aps_main_switchCDR(oldDirBuffer) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_SWITCH_CDR,(int32)(oldDirBuffer),0,0,0,0)

#define aps_main_restoreCDR(oldDirBuffer) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RESTORE_CDR,(int32)(oldDirBuffer),0,0,0,0)


/*��ͣ�����������������extID֮��*/
#define aps_main_pauseExt_except(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_PAUSE_EXCEPT,(int32)(extID),0,0,0,0)

/*�ָ������������������extID֮�� */
#define aps_main_resumeExt_except(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RESUME_EXCEPT,(int32)(extID),0,0,0,0)

/*�������ں���ԭ��*/
/* p0:  ��Ϣ�� */
/* p1~p5: ���� */
typedef int32 (*ExtEntry)(int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

/*ע��������ں���*/
#define aps_main_regEntry(selfExtid, entryAddr) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_REG_ENTRY,(int32)(selfExtid),(int32)(entryAddr),0,0,0)

/* ��ȡĳһ��mrp��appid��appVer��shortName */
#define aps_main_getMrpInfo(packName,appID,appVer,shortName) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_GETMRPINFO, (int32)packName, (int32)appID, (int32)appVer,(int32)shortName,0)
	


			
typedef struct _tagCBParam
{
	int32 p0;
	int32 p1; 
	int32 p2; 
	int32 p3; 
	int32 p4; 
	int32 p5;
}APS_CBParam;

/*************************************************************
  aps_main_fwd_cb����ί����ģ���첽�ص��ûص�����,�ڵ��ô˺���ǰ
  Ӧ����aps_main_unloadExt_dump
  func:   mpsFpFuncType                                    
  param:  APS_CBParam*    ��Ҫָ��һ�鶯̬�ڴ�,�ڻص�ִ�к�����
                          �ͷ�
*************************************************************/
#define aps_main_fwd_cb(func,param) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_FWD_CALLBACK,(int32)func,(int32)param,0,0,0)

/***********************************************************************
aps_vm_malloc��aps_vm_free����������ͷŲ���DUMPװ���ڴ�������Ծɰ�ȫ
���ڴ�ռ䡣

��A������ͷ��Լ�֮ǰ����ʹ��aps_vm_malloc������һ���ڴ棬���������Ҫ��
Game.Ext����Ϣ����ʹ�����ͷ�֮���Game.Ext��˵������ڴ��Ծ��ǰ�ȫ�ġ�

���Ա���������aps_vm_malloc���ڴ�ǳ�����,ֻ����������С����ڴ档
*************************************************************************/
#define aps_vm_malloc(size)\
	(void*)mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_VM_MALLOC,(int32)(size),0,0,0,0)

#define aps_vm_free(p)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_VM_FREE,(int32)(p),0,0,0,0)


#ifdef __cplusplus
}
#endif

#endif
