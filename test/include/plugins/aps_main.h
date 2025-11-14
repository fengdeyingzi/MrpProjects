#ifndef _APS_MAIN_H_
#define _APS_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif


#define APS_EC_INVALID_PARAM 1
#define APS_EC_OUT_OF_MEMORY 2
#define APS_EC_IO_ERROR      3
#define APS_EC_INVALID_EXT_STATE 4



//////////////////////////////////////////////////////////////////////////////
//   NOTE:
//
//   负责开发插件的同事 
//   插件ID (extID) , 插件mrp的appid， 插件mrp的文件路径名
//   必须向SDK开发小组(王泽芳/朱凯)申请。
//
//    以免防止冲突。
//
//////////////////////////////////////////////////////////////////////////////


#define APS_ID_EXTID_BASE     480000

/************************************************************************
 *  插件ID 分配表  (0-31)
 ************************************************************************/


#define APS_EXT_ID_MAINMODULE                1                                 /* 主模块 */   
#define APS_EXT_ID_VERDLOAD                  2                                 /* 下载   */
/* smsend.mrp  已经不使用， 回收中  */     /*3*/
#define APS_EXT_ID_BROWSER                   4                                 /* 浏览器 */
/*  原Java图形插件  回收中 */              /*5*/
/*  6  空闲 */                             /*6*/
/*  7  空闲 */                             /*7*/
#define APS_EXT_ID_SMCHECK		             8		                           /* 计费 -- 付费检查*/
#define APS_EXT_ID_SMSPAY		             9		                           /* 计费 */
#define APS_EXT_ID_NETPAY		            10		                           /* 计费 */
#define APS_EXT_ID_SMSNET		            11		                           /* 计费 短信预取 */
/*  12 原交叉营销插件,   回收中。 */      /*12*/
#define APS_EXT_ID_PASSPORT_LOCALHISTORY	13                                 /* 通行证 */
#define APS_EXT_ID_PASSPORT_UI		        14                                 /* 通行证 */
/*  15 空闲 */                            /*15*/
#define APS_EXT_ID_SMSHIDE		            16	                               /* 计费 后台短信付费插件*/
#define APS_EXT_ID_PASSPORT_NET 	        17                                 /* 通行证 */
#define APS_EXT_ID_SMSCHAR		            18		                           /* 计费:短信付费插件*/
#define APS_EXT_ID_OGMCHAR		            19		                           /* 计费: 网游付费插件*/
#define APS_EXT_ID_KBCHARG		            20		                           /* 计费: K币付费插件*/
#define APS_EXT_ID_BRWCORE		            21	                               /* 计费: 浏览器core插件*/
#define APS_EXT_ID_BRWNAVI		            22		                           /* 浏览器导航插件*/
#define APS_EXT_ID_BRWMAIN		            23		                           /* 浏览器主控插件*/
#define APS_EXT_ID_BRWCHLST		            24		                           /* 浏览器频道插件*/
#define APS_EXT_ID_DSMSRFM	 	            25		                           /* 计费: 延迟二次确认短信*/
#define APS_EXT_ID_BRWGUI		            26								   /* 浏览器gui插件*/
#define APS_EXT_ID_BRWSHELL		            27		                           /* 浏览器shell插件*/
/*   28  cplg.mrp */                      /*28*/                               /* 计费*/
/*   29  计费内营销插件 */                /*29*/                               /* 计费 营销*/
/*   30  空闲 */                          /*30*/
/*   31  空闲*/                           /*31*/


#define APS_EXT_ID_APP           255


#define APS_EXT_ID_GRAPHICS      10008
#define APS_EXT_ID_RECOMMEND     10044  //10044-10047
#define APS_EXT_ID_MESSAGE	     10048  //10048-10051





// 当app模块调用 mrc_exit()时,向主模块发送一个退出消息。
#define APS_MAIN_CODE_EXITAPP            0  
#define APS_MAIN_CODE_RUNMRP             1

#define APS_MAIN_CODE_LOAD            32
#define APS_MAIN_CODE_UNLOAD          33


#define APS_MAIN_CODE_BLOCK           34   //  *** block
#define APS_MAIN_CODE_UNBLOCK         35   //  *** unblock

#define APS_MAIN_CODE_REG_ENTRY       36



/******************************************************
 *  为了让新版本的主模块兼容老版本的插件, 防止某些老插件还继续调用这个CODE来检查版本.
 *   
 *   我们在新版本的主模版里仍然保留这个CODE
 *
 *   但是这个Code的功能我们调整为:  不会根据plugins.lst解压出插件。
 */
#define APS_MAIN_CODE_CHECK_EXT_VERSION   38


#define APS_MAIN_CODE_GET_EXT_VERSION     39
#define APS_MAIN_CODE_GET_EXT_PACKNAME    40
#define APS_MAIN_CODE_DUMP_LOAD           41
#define APS_MAIN_CODE_DUMP_UNLOAD         42
#define APS_MAIN_CODE_QUERY_LOAD_MODE     43
#define APS_MAIN_CODE_FWD_CALLBACK        44

#define APS_MAIN_CODE_SWITCH_CDR          45
#define APS_MAIN_CODE_RESTORE_CDR         46


#define APS_MAIN_CODE_BLOCK_EXCEPT        47    // *** block except
#define APS_MAIN_CODE_UNBLOCK_EXCEPT       48    // *** unblock except

#define APS_MAIN_CODE_QUERY_MAINEXTTYPE   49
#define APS_MAIN_CODE_GET_MAIN_VERSION    50

// 真正的pause/resume
#define APS_MAIN_CODE_PAUSE_REAL          51
#define APS_MAIN_CODE_RESUME_REAL         52

#define APS_MAIN_CODE_OVERRIDE_MEMFUNC   100
#define APS_MAIN_CODE_GETSMSCENTER       101

#define APS_MAIN_CODE_VM_MALLOC           180
#define APS_MAIN_CODE_VM_FREE             181
    
#define APS_MAIN_CODE_SET_UIFLAG               191
#define APS_MAIN_CODE_QUERY_NETWORK_TYPE       192
#define APS_MAIN_CODE_BLOCK_KEY_EVENT          193
#define APS_MAIN_CODE_TRANS_KEY_EVENT          194  


#define APS_MAIN_CODE_LOAD_VM				   196
#define APS_MAIN_CODE_RESET_LOAD			   197
#define APS_MAIN_CODE_RESET_UNLOAD			   198

/* 获取某一个mrp的appid，appVer，shortName */
#define APS_MAIN_CODE_GETMRPINFO               201

/* 读取某一个mrp里的plugins.lst */
#define APS_MAIN_CODE_GETPLUGINSLST             202

/* 从某一个MRP包提取一个文件, 写到T卡上 */
#define APS_MAIN_CODE_RETRIEVEFILEFROMMRP            203

/* 从某一个MRP包提取一个非压缩的文件, 写到T卡上 */
#define APS_MAIN_CODE_RETRIEVENOZIPFILEFROMMRP            204

/* 设置插件的刷屏优先级*/
#define APS_MAIN_CODE_SET_REFRESHSCREEN_PRIORITY		205

/* 按优先级刷屏	*/
#define APS_MAIN_CODE_REFRESHSCREEN_BY_PRIORITY		206

/* 暂停刷屏	*/
#define APS_MAIN_CODE_REFRESHSCREEN_SUSPEND		207

/* 恢复刷屏	*/
#define APS_MAIN_CODE_REFRESHSCREEN_UNSUSPEND	208

/*查询堆栈信息,仅对_r模式下的主模块有效*/
#define APS_MAIN_CODE_QUERYSTACK                         0xF0000000

typedef enum MainExtType
{
	MainExtType_NORMAL = 0,  /* 普通类型       */
	MainExtType_DUMP         /* 支持dump的类型 */
}MainExtType;

/*查询当前的主模块类型,返回值为MainExtType*/
#define aps_main_getMainExtType()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_QUERY_MAINEXTTYPE, 0,0,0,0,0)

/*查询当前主模块的版本号*/
#define aps_main_getMainVersion()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_GET_MAIN_VERSION,0,0,0,0,0)

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
	}mrcEnterFuncSt;
#endif

/*
 *   aps_main_loadExt
 *
 *   aps_main_loadExtAddr  可以指定加载的地址，主要是考虑使用图形加速功能。
 */
#ifndef SDK_MOD
#define  aps_main_loadExt(extID,mrpname,mrpfolder,extname) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),0)

/*预加载插件描述*/
typedef struct 
{
	void* addr;  /*预加载内存地址 */
	int32 len;   /*所提供的用于加载的内存尺寸, ==0 表示不指定长度,将使用插件实际大小内存*/  
}APS_PreLoadParam;

#define  aps_main_loadExtAddr(extID,mrpname,mrpfolder,extname,preLoadParam) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)(preLoadParam))


#else   
#define  aps_main_loadExt(extID,mrpname,mrpfolder,extname,enterFunc) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)enterFunc)
#endif

#ifndef SDK_MOD
#define aps_main_loadExtVm(extID,mrpname,mrpfolder,extname) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD_VM, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),0)
#else
#define aps_main_loadExtVm(extID,mrpname,mrpfolder,extname,enterFunc) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_LOAD_VM, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)enterFunc)
#endif

#define aps_main_loadExtRest(extID,mrpname,mrpfolder,extname) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_RESET_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),0)

typedef void (*loadExt_dump_cb)(int32 result);
#ifndef SDK_MOD
/*
  aps_main_loadExt_dump
  用来延迟加载extID指定的插件,并且尝试覆盖Game Ext的代码段做为插件使用的代码段内存，如果Game Ext的代码段不足已容纳插件的代码段，则
  行为与aps_main_loadExt一致，申请新的内存来加载。

  aps_main_loadExt_dump的操作得等待调用者的调用栈退出之后才能真正执行，所以，在调用者对于aps_main_loadExt_dump的所有后续(本次调用栈之内操作)
  对于主模块的所有操作,都将被延后到aps_main_loadExt_dump真正执行之后才能得到执行。
*/
#define aps_main_loadExt_dump(extID,mrpname,mrpfolder,extname,cb) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_DUMP_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)cb)
#else
#define aps_main_loadExt_dump(extID,mrpname,mrpfolder,extname,enterFunc) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_DUMP_LOAD, (extID),(int32)(mrpname),(int32)(mrpfolder),(int32)(extname),(int32)enterFunc)
#endif



#define APS_EXT_LOADMODE_NOTFOUND     0   /*插件未装载*/          
#define APS_EXT_LOADMODE_NORMAL       1   /*插件已正常装载*/
#define APS_EXT_LOADMODE_DUMP         2   /*插件已复用Game Ext代码段的方式装载*/
#define APS_EXT_LOADMODE_VM			  3   /*插件已加载到VM内存中*/
#define APS_EXT_LOADMODE_RESET		  4	  /*插件已通过reset方式加载*/	

/* 查询插件的装载模式 */
#define aps_main_query_loadMode(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_QUERY_LOAD_MODE,(extID),0,0,0,0)

/*isDelayUnload  0: 立即卸载 1: 延迟卸载*/
#define  aps_main_unloadExt(extID,isDelayUnload) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_UNLOAD,(int32)(extID),(int32)isDelayUnload,0,0,0)

/*卸载由Dump方式加载的插件*/
#define aps_main_unloadExt_dump(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_DUMP_UNLOAD,(int32)(extID),0,0,0,0)

/*************************************************
 *  这一组API 用来 block/unblock 插件
 *     block/unblock 会暂停游戏的定时器， 并且block按键事件.
 *     
 *     在模拟器模式下， 定时器不能被停止， 需要应用自己来停。 
 *************************************************/

#define aps_main_blockExt(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_BLOCK,(int32)(extID),0,0,0,0)

#define aps_main_unblockExt(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_UNBLOCK,(int32)(extID),0,0,0,0)

/*暂停所有其他插件，除了extID之外*/
#define aps_main_blockExt_except(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_BLOCK_EXCEPT,(int32)(extID),0,0,0,0)

/*恢复所有其他插件，除了extID之外 */
#define aps_main_unblockExt_except(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_UNBLOCK_EXCEPT,(int32)(extID),0,0,0,0)

/*************************************************
 *  这一组API 用来 pause/resume 插件
 *  pause/resume会暂停游戏的定时器， 并且组织案件事件。
 *  ** 受影响的插件会接收到 resmue/pause 事件
 * 
 *
 *    flags :
 *          0x1  :  受影响的目标插件集合 = 所有的插件的集合 - (extID1, extID2)
 *          
 *          0x2  :  受影响的目标插件集合 = 所有的插件的集合 - (extID1, extID2) - VM常驻插件
 *
 *
 **************************************************/

#define aps_main_pauseExtReal(flags,extID1,extID2) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_PAUSE_REAL,(int32)(flags),extID1,extID2,0,0)

#define aps_main_resumeExtReal(flags,extID1,extID2) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RESUME_REAL,(int32)(flags),extID1,extID2,0,0)


/*插件的入口函数原型*/
/* p0:  消息号 */
/* p1~p5: 参数 */
typedef int32 (*ExtEntry)(int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

/*注册插件的入口函数*/
#define aps_main_regEntry(selfExtid, entryAddr) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_REG_ENTRY,(int32)(selfExtid),(int32)(entryAddr),0,0,0)


/* 获取某一个mrp的appid，appVer，shortName */
#define aps_main_getMrpInfo(packName,appID,appVer,shortName) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_GETMRPINFO, (int32)packName, (int32)appID, (int32)appVer,(int32)shortName,0)

/* 读取某一个mrp里的plugins.lst */
#define aps_main_getAppVer_from_Pluginslst(mrpName,appID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_GETPLUGINSLST, (int32)mrpName,appID,0,0,0)

/* 从某一个MRP包提取一个文件, 写到T卡上 */
#define aps_main_retrieveFileFromMrp(srcMrpName,srcFileName, dstFolder, dstFileName) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_RETRIEVEFILEFROMMRP, (int32)srcMrpName, (int32)srcFileName, (int32)dstFolder, (int32)dstFileName, 0)

#if 0 /*此功能目前关闭状态*/
/* 从某一个MRP包提取一非压缩的文件, 写到T卡上，不需要耗费解压缩的内存 */
#define aps_main_retrieveNozipFileFromMrp(srcMrpName,srcFileName,dstFolder,dstFileName) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_RETRIEVENOZIPFILEFROMMRP, (int32)srcMrpName, (int32)srcFileName, (int32)dstFolder, (int32)dstFileName, 0)
#endif

#define aps_main_getExtVersion(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE, APS_MAIN_CODE_GET_EXT_VERSION, (extID),0,0,0,0)

#define aps_main_getExtPackName(extID) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_GET_EXT_PACKNAME, (extID), 0,0,0,0)

#define aps_main_runMrpLater(mrp_name,parameter) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RUNMRP,(int32)(mrp_name),(int32)(parameter),0,0,0)

#define aps_main_switchCDR(oldDirBuffer) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_SWITCH_CDR,(int32)(oldDirBuffer),0,0,0,0)

#define aps_main_restoreCDR(oldDirBuffer) \
    mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_RESTORE_CDR,(int32)(oldDirBuffer),0,0,0,0)

#define aps_main_getSMSCenter(smsCenter) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_GETSMSCENTER,(int32)(smsCenter),0,0,0,0)

#define aps_main_setUIFlag(onOffFlag) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_SET_UIFLAG,(int32)(onOffFlag),0,0,0,0)

#define aps_main_blockKeyEvent(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_BLOCK_KEY_EVENT,(int32)(extID),1,0,0,0)

#define aps_main_unblockKeyEvent(extID) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_BLOCK_KEY_EVENT,(int32)(extID),0,0,0,0)

#define aps_main_OnExtKeyEvent(extID,code,param0,param1) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_TRANS_KEY_EVENT,(int32)(extID),code,param0,param1,0)									

			
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
  aps_main_fwd_cb用来委托主模块异步地调用回调函数,在调用此函数前
  应调用aps_main_unloadExt_dump
  func:   mpsFpFuncType                                    
  param:  APS_CBParam*    需要指向一块动态内存,在回调执行函数中
                          释放
*************************************************************/
#define aps_main_fwd_cb(func,param) \
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_FWD_CALLBACK,(int32)func,(int32)param,0,0,0)


/***********************************************************************
aps_vm_malloc和aps_vm_free用来申请和释放采用DUMP装载内存情况下仍旧安全
的内存空间。

如A插件在释放自己之前可以使用aps_vm_malloc来申请一块内存，用来存放需要给
Game.Ext的信息，即使自身释放之后对Game.Ext来说，这块内存仍旧是安全的。

可以被用来进行aps_vm_malloc的内存非常有限,只能用来申请小块的内存。
*************************************************************************/
#define aps_vm_malloc(size)\
	(void*)mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_VM_MALLOC,(int32)(size),0,0,0,0)

#define aps_vm_free(p)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_VM_FREE,(int32)(p),0,0,0,0)


#define APS_NETWORK_STATUS_NONE    0xFF
#define APS_NETWORK_STATUS_CMWAP    1
#define APS_NETWORK_STATUS_CMNET    2
#define APS_NETWORK_STATUS_UNIWAP   3
#define APS_NETWORK_STATUS_UNINET   4          
#define APS_NETWORK_STATUS_UNKNOW   0xF1

#define aps_query_netowrkType()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_QUERY_NETWORK_TYPE,0,0,0,0,0)

#define aps_query_stackInfo(stackTop,stackLen,vmLeft, ctrlLeft)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_QUERYSTACK,(int32)(stackTop),(int32)(stackLen),(int32)vmLeft,(int32)ctrlLeft,0)


/*
 *  刷屏和事件优先级的设置  - 可以同时设置，也可以一次设置一个。
 *  priority -- 刷屏的优先级，  越高的值， 就越是最后刷
 *  eventPriority -- 事件队列，  越高的值， 就越是先受到事件。
 *  
 *      if (paint_func)  {   设置刷屏 }
 *      if (eventPriority != -1 )  {设置事件优先级}
 *  
 *   tgtExtID -- 目标插件， 主模块可以在插件被pause时不调用原来的刷屏函数。
 */

typedef void (*aps_asynPaint_func_t)(int32 x, int32 y, int32 w, int32 h);

#define aps_set_refreshScreen_priority(paint_func,priority, tgtExtID, eventPriority)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_SET_REFRESHSCREEN_PRIORITY,(int32)(paint_func),(int32)(priority),(int32)(tgtExtID),(int32)(eventPriority),0)
		
#define aps_refreshScreen_trigger(x,y,w,h)\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_REFRESHSCREEN_BY_PRIORITY,(int32)(x),(int32)(y),(int32)(w),(int32)(h),0)

#define aps_refreshScreen_suspend()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_REFRESHSCREEN_SUSPEND,0,0,0,0,0)

#define aps_refreshScreen_unsuspend()\
	mrc_extSendAppEventEx(APS_EXT_ID_MAINMODULE,APS_MAIN_CODE_REFRESHSCREEN_UNSUSPEND,0,0,0,0,0)



 
#ifdef __cplusplus
}
#endif



#endif
