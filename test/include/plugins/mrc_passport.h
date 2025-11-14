#ifndef _MRC_PASSPORT_H_
#define _MRC_PASSPORT_H_

//#include "aps_verdload.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PASSPORT_LAST_VER       17  //当前版本

#define PASSPORT_MRP_NAME	 "passpt.mrp"	
#define PASSPORT_MRP_FOLDER	"plugins"

//保存用户信息个数
#define MAX_SAVE_USR_COUNT		3 		

#define MAX_PWD_LEN             12*2
#define MAX_USERNAME_LEN        63
#define MAX_NICKNAME_LEN		12  //12个字符
//加一字节密文类型
#define MAX_PWD_CONTENT_LEN     1+32 

//#define ENCRYPT_IMSI_LEN        24
#define MAX_IMSI_LEN 16

typedef enum
{
	// 统一通行证服务器返回码
	E_PASSPORT_SUCCESS  = 200,	//成功
    E_PASSPORT_FORCE_UPDATE = 505, //强制更新
	E_PASSPORT_OPTIONAL_UPDATE = 506 //可选更新(已经请求处理成功)
	//其他失败
}PASSPORT_SERVER_RESULT_CODE;

//本地历史记录中的账户信息
typedef struct
{
    uint8  imsi1[MAX_IMSI_LEN];
	uint8  imsi2[MAX_IMSI_LEN];
	uint8  isAvailable;			// 是否可用，0x01可用，此字段在获取账号历史记录后，用于判断账号是否可用；更新账号时无用。
    uint8  pwdLen;               // 用户实际的密码长度，注意：在更新账号历史文件时pwdLen为0，则不更新pwdLen，否则会更新pwdLen。
    uint8  isSavePwd;			// 是否记住密码
	uint8  isAutoLogin;			// 自动登录
    uint16 userName[MAX_USERNAME_LEN+1];		// 账户名 utf-16-be
	uint8  pwdContentLen;		// pwdContentLen 长度
    uint8  pwdContent[MAX_PWD_CONTENT_LEN];		// 由服务端下发的密码加密串。
    uint8  reserved[16];			// 保留
}AccountInfoSt;

//存在DSM中的，登录成功的用户的信息
typedef struct
{
	uint32 skyid;
	uint8  token[8];	
	uint16 loginDate[6]; //Year(2),month(2),day(2),hour(2),minute(2),second(2)
	uint16 userName[MAX_USERNAME_LEN+1];
	uint16 nickName[MAX_NICKNAME_LEN+1];
	uint8  gender;
	uint8  age;
	//uint8  star;		
	uint8  portrait[16+1];
    uint8  flags;	//1 bit 标识是否设置了安全问题
	uint8  mobile[11+1];
}LOGININFO;

#define DSM_USERINFO LOGININFO

//登录需要的account信息
typedef struct
{  
	uint8  isSavePwd;//是否保存密码
	uint8  isAutoLogin;//是否自动登录
	uint16 userName[MAX_USERNAME_LEN+1];//用户名，\x00\x00结束						
	int8   pwdContentLen; //当pwdContentLen为0时，表示明文；否则为密文
	uint8  pwdContent[MAX_PWD_CONTENT_LEN];  //密码明文或密文
}ACCOUNTINFO;	


typedef enum //accesspoint，用于数据分析
{
	ACCESSPOINT_ENTER_PASSPORT,
	ACCESSPOINT_ENTER_REG,	           
	ACCESSPOINT_ENTER_NORMAL_REG,		
	ACCESSPOINT_ENTER_MOBILE_REG	
}ACCESSPOINT;


/*
mrc_passport回调函数: 调用函数后，不论函数调用成功失败，回调函数都会被调用。																
	result:	200 成功，其它失败							    
	resultMsg: resultMsg会有具体的提示信息(获取安全问题成功时，此字段放安全问题)。
	extraInfo: 自更新提示，根据result判断是强制更新还是可选更新(E_PASSPORT_FORCE_UPDATE为强制，否则为可选)。
			   NULL  无效
			   非零  提示语
				
*/
typedef void (*MR_Passport_CB)(int32 result, uint16 * resultMsg, void * extraInfo);


/*
mrc_passport window回调函数:																	
	errCode: 0 成功，非0 失败							    				
*/
typedef void (*MR_Passport_WIN_CB)(int32 errCode);


/*
检查版本																
返回值:
	MR_FAILED 失败，需要更新
	MR_SUCCESS 成功，已经是最新的
*/
int32 mrc_passport_checkVersion(void);

/*下载过程回调定义(这个回调同MR_DL_CB，更详细的定义请参考verdload.h)

  status:    当前通知的状态
  reason:    错误原因,仅在 MR_DL_STATUS_EXCEPTION时有效，其它状态下为MR_DL_ERROR_REASON_NONE
  p1:        status == MR_DL_STATUS_DOWNLOADING 时为总下载百分比值,如20%,p1=20,
  其它status状态下为备用参数
  (p2-p3):   status == MR_DL_STATUS_DOWNLOADING 时p2 为当前已下载数据，p3为应下载全部数据长度
  其它status状态下目前作为备用参数
*/
typedef void (*MR_Passport_DL_CB)(int32 status,int32 reason,int32 p1,int32 p2,int32 p3);

/*
调用verdload模块进行下载，应用需要根据下载回调，绘制下载界面。																
返回值:
MR_FAILED 调用下载模块失败
MR_SUCCESS 调用下载模块成功
*/
int32 mrc_passport_verDownload(MR_Passport_DL_CB cb);

/*
用户正常注册																
输入：																					
	uint16 * userName:用户名						
	uint16 * pwd:密码明文
	MR_Passport_CB cb:回调，操作完成后被调用
返回值:
	MR_FAILED 失败
	MR_SUCCESS 成功
*/ 
int32 mrc_passport_reg(uint16 * userName, uint16 * pwd, MR_Passport_CB cb);

/*
快速注册																
输入:																					
	MR_Passport_CB cb:回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/ 
int32 mrc_passport_quickReg(MR_Passport_CB cb);

/*
用户登录
输入:																					
	ACCOUNTINFO * accountInfo:账户信息  
	MR_Passport_CB cb:回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/
int32 mrc_passport_login(ACCOUNTINFO * accountInfo, MR_Passport_CB cb);
/*
用户登出																
输入:																					
	MR_Passport_CB cb:回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/ 
int32 mrc_passport_logout(MR_Passport_CB cb);

/*
设置安全问题																
输入:						
	uint16 * pwd:密码明文
	uint16 * secretq:安全问题
	uint16 * secreta:安全问题答案
	MR_Passport_CB cb:回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/
int32 mrc_passport_setSecretq(uint16 * pwd,
                              uint16 * secretq,
                              uint16 * secreta, 
                              MR_Passport_CB cb);
                              
/*
获取安全问题																
输入:																					
	uint16 * userName:用户名								
	MR_Passport_CB cb:回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
  
CB参数:
	当result为200时，获取安全问题成功，安全问题填入resultMsg。
*/                              
int32 mrc_passport_getSecretq(uint16 * userName, MR_Passport_CB cb);

/*
重置密码																
输入:																					
	uint16 * userName:用户名
	uint16 * pwd:重置后的明文密码						
	uint16 * secretq:安全问题
	uint16 * secreta:安全问题答案	
	MR_Passport_CB cb: 回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/
int32 mrc_passport_resetPwd(uint16 * userName, 
                            uint16 * pwd,
                            uint16 * secretq, 
                            uint16 * secreta, 
                            MR_Passport_CB cb);                            

/*
设置用户密码
输入:																					
	uint16 * pwd:旧密码明文
	uint16 * newPwd:新密码明文
	MR_Passport_CB cb: 回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/                            
int32 mrc_passport_setPwd(uint16 * pwd, uint16 * newPwd, MR_Passport_CB cb);

/*
设置用户信息，只提供设置基本信息，更多信息请启动浏览器通过WAP设置
输入:																					
	uint16 * nickName:用户昵称
	uint8 gender:性别
	MR_Passport_CB cb: 回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/
int32 mrc_passport_setUserInfo(uint16 * niceName, uint8 gender, MR_Passport_CB cb);    

/*
用户验证
输入:																					
	MR_Passport_CB cb:回调，操作完成后被调用
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/ 
int32 mrc_passport_checkLogin(MR_Passport_CB cb);


/*
获取DSM中保存的登录状态信息
输入:					
	LOGININFO* userinfo							
输出:
	*userinfo
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/
int32 mrc_passport_getLoginInfo(LOGININFO * loginInfo);

/*
清空DSM中保存的登录状态信息，不进行登出操作
输入:
	无					
输出:
	无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/
int32 mrc_passport_clearLoginInfo(void);

/*
取消之前的请求
输入:
	无					
输出:
	无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功
*/
int32 mrc_passport_cancelReq(void);

/*
设置拨号类型
输入:
uint8 dialType: 拨号类型(1 CMWAP, 2 CMNET)
输出:
无
返回值：
MR_FAILED 失败
MR_SUCCESS 成功
*/
int32 mrc_passport_setDialType(uint8 dialType);

/*
设置应用拨号状态
输入:
uint8 status: 拨号状态(0 未拨号, 1 已拨号)
输出:
无
返回值：
MR_FAILED 失败
MR_SUCCESS 成功
*/
int32 mrc_passport_setAppDialStatus(uint8 status);

/*
(如果是从插件调用通行证的，且插件希望对从这个插件进入通行证的数据进行统计，可以调用这个接口;如果是从应用(非插件方式)调用的，不需要调用这个函数设置)
应用访问统计
输入:
uint32 accessPoint:应用来源(为了统计方便，可以填写插件的APPID)
输出:
无
返回值：
MR_FAILED 失败
MR_SUCCESS 成功
*/
int32 mrc_passport_access(uint32 accessPoint);

/*
设置UI参数
输入:
int32 regType: 注册类型(0 默认、1 手机注册、2 普通注册、3 快速注册)
int32 notShowSuccWin: 是否显示登录成功界面(0 显示、1 不显示)
输出:
无
返回值：
*/
void mrc_passport_setUIArgs(int32 regType, int32 notShowSuccWin, int32 p3, int32 p4);


/*
从本地历史文件中获取历史登录信息																
输入:																							
	AccountInfoSt accountInfo[MAX_SAVE_USR_COUNT]，指向保存记录的结构
输出：
    accountInfo[MAX_SAVE_USR_COUNT]
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
int32 mrc_passport_getLoginHistory(AccountInfoSt accountInfo[MAX_SAVE_USR_COUNT]);


/*
手机快速注册																
输入：																					
	MR_Passport_CB cb:回调，操作完成后被调用
返回值:
	MR_FAILED 失败
	MR_SUCCESS 成功
*/ 
int32 mrc_passport_mobile(MR_Passport_CB cb);

/*
手机重置密码																
输入：																					
	uint16 * userName:用户名
	uint16 * pwd:新密码
	MR_Passport_CB cb:回调，操作完成后被调用
返回值:
	MR_FAILED 失败
	MR_SUCCESS 成功
*/ 
int32 mrc_passport_mobResetPwd(uint16 * userName, uint16 * pwd, MR_Passport_CB cb);


/*
绑定手机																
输入：																					
	uint16 * pwd:密码
	MR_Passport_CB cb:回调，操作完成后被调用
返回值:
	MR_FAILED 失败
	MR_SUCCESS 成功
*/ 
int32 mrc_passport_bindMob(uint16 * pwd, MR_Passport_CB cb);

/*
手机解绑																
输入：																					
	uint16 * pwd:密码
	MR_Passport_CB cb:回调，操作完成后被调用
返回值:
	MR_FAILED 失败
	MR_SUCCESS 成功
*/ 
int32 mrc_passport_unbindMob(uint16 * pwd, MR_Passport_CB cb);

enum {
	PASSPORT_ID_ENTRY_WIN_DUMP = 1,
    PASSPORT_ID_ENTRY_WIN, 
	PASSPORT_ID_LOGIN_WIN_DUMP,
	PASSPORT_ID_LOGIN_WIN,
	PASSPORT_ID_REG_WIN_DUMP,
	PASSPORT_ID_REG_WIN,
	PASSPORT_ID_MODIFYUSERINFO_WIN_DUMP,
	PASSPORT_ID_MODIFYUSERINFO_WIN
};
int32 mrc_passport_funcEntry(int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

/*
通行证入口，DUMP模式
输入:																							
	MR_Passport_WIN_CB cb:回调函数，从窗口返回后调用此回调
输出：
    无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
#define mrc_passport_entryWin_dump(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_ENTRY_WIN_DUMP,(int32)cb,0,0,0,0)

/*
通行证入口，非DUMP模式
输入:																							
	MR_Passport_WIN_CB cb:回调函数，从窗口返回后调用此回调
输出：
    无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
#define mrc_passport_entryWin(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_ENTRY_WIN,(int32)cb,0,0,0,0)

/*
前台登录(带UI)，DUMP模式
输入:																							
	MR_Passport_WIN_CB cb:回调函数，从窗口返回后调用此回调
输出：
    无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
#define mrc_passport_loginWIN_dump(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_LOGIN_WIN_DUMP,(int32)cb,0,0,0,0)

/*
前台登录(带UI)，非DUMP模式
输入:																							
	MR_Passport_WIN_CB cb:回调函数，从窗口返回后调用此回调
输出：
    无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
#define mrc_passport_loginWIN(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_LOGIN_WIN,(int32)cb,0,0,0,0)

/*
前台注册(带UI)，DUMP模式
输入:																							
	MR_Passport_WIN_CB cb:回调函数，从窗口返回后调用此回调
输出：
    无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
#define mrc_passport_regWIN_dump(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_REG_WIN_DUMP,(int32)cb,0,0,0,0)

/*
前台注册(带UI)，DUMP模式
输入:																							
	MR_Passport_WIN_CB cb:回调函数，从窗口返回后调用此回调
输出：
    无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
#define mrc_passport_regWIN(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_REG_WIN,(int32)cb,0,0,0,0)

/*
前台注册(带UI)，DUMP模式
输入:																							
	MR_Passport_WIN_CB cb:回调函数，从窗口返回后调用此回调
输出：
    无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
#define mrc_passport_modifyUserInfoWIN_dump(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_MODIFYUSERINFO_WIN_DUMP,(int32)cb,0,0,0,0)

/*
前台修改用户信息(带UI)，非DUMP模式
输入:																							
	MR_Passport_WIN_CB cb:回调函数，从窗口返回后调用此回调
输出：
    无
返回值：
	MR_FAILED 失败
	MR_SUCCESS 成功    
*/
#define mrc_passport_modifyUserInfoWIN(cb) \
	mrc_passport_funcEntry(PASSPORT_ID_MODIFYUSERINFO_WIN,(int32)cb,0,0,0,0)

#ifdef __cplusplus
}
#endif

#endif