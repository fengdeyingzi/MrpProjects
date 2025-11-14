//mrc_opensky.h
//本文件包括所有OPENSKY模块的入口函数定义


#ifndef __MRC_OPENSKY_COMMON__
#define __MRC_OPENSKY_COMMON__

#include "mrc_base.h"

#ifdef __cplusplus
extern "C" {
#endif

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////宏定义//////////////////////////////////////////
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//netPK 模块
#define MAX_PREFIXDATA_COUNT   20

#define MAX_NICKNAME_LEN  12
#define MAX_TITLE_LEN     12
#define MAX_IDENTIFY_LEN 32
#define MAX_SERVERNAME_LEN	12

//入口模块
#define MRC_OPENSKY_ENTRY_START_TYPE_PARAM 100

typedef enum
{
	MR_OPENSKY_CLOUD_STORAGE		=	0,			//云存贮
	MR_OPENSKY_CLOUD_STORE			=	1,		//道具商城
	MR_OPENSKY_CLOUD_ACHIEVEMENT	=	2,		//成就
	MR_OPENSKY_CLOUD_FEED			=	3,		//feed
	MR_OPENSKY_CLOUD_NETPK			=	4,		//联网PK
	MR_OPENSKY_CLOUD_ENTRY			=	5,		//入口

	MR_OPENSKY_CLOUD_MAX
	
}MR_OPENSKY_CLOUD_TYPE;

typedef enum
{
	//common def
	MR_OPENSKY_FAILED			=	MR_FAILED,		//-1, 失败
	MR_OPENSKY_SUCCESS		=	MR_SUCCESS,	//0, 成功
	MR_OPENSKY_IGNORE		=	MR_IGNORE,		//1, 忽略
	MR_OPENSKY_WAITING		=	MR_WAITING,		//2, waiting
	MR_OPENSKY_PARAM_ERROR	=	5, 				//5, 传入参数错误		
	MR_OPENSKY_MALLOC_FAILED,					//6, malloc失败

	MR_OPENSKY_FORCE_UPDATE,					//强制更新
	MR_OPENSKY_SUGG_UPDATE,						//建议更新
	MR_OPENSKY_INVAILD_USER,						//错误的用户名或密码
	MR_OPENSKY_SERVER_BUSY,						//服务器忙

	//云存贮
	MR_OPENSKY_CLOUD_REJECT	=	0x080 			//已有语句正在执行中，不能同时执行另一语句
	
	

	
}MR_OPENSKY_RETCODE;


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////云存贮(cloude storage)模块///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_CLOUDE_STORAGE	//分割符，无实际意义
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//由于云存贮是一个插件，为保持版本兼容性，返回值定义保持不变
typedef enum
{
	MR_CLOUD_SUCCESS		 = 0,	//成功
	MR_CLOUD_INVAILD_PARAM	 = 1,	//参数错误
	MR_CLOUD_BLACK_LIST		 = 2,	//黑名单用户
	MR_CLOUD_FORCE_UPDATE	 = 3,	//强制更新
	MR_CLOUD_SUGG_UPDATE	 = 4,	//建议更新
	MR_CLOUD_INVAILD_USER	 = 5,	//错误的用户名或密码
	MR_CLOUD_SERVER_BUSY	 = 6,	//服务器忙
	MR_CLOUD_OTHER_FAILURE	 = 7	//其他错误,一般由本地错误引起,如联网失败等
}MR_cloudStorage_errcode_E;

typedef enum
{
	MR_SQLTYPE_BYTEA	= 2,	//bytea			多字节
	MR_SQLTYPE_BOOLEAN	= 1,	//boolean		单字节
	MR_SQLTYPE_INTERGER	= 4,	//interger		四字节
	MR_SQLTYPE_BIGINT	= 0,	//bigint		八字节
	MR_SQLTYPE_TIMESTP	= 5,	//timestamp		八字节
	MR_SQLTYPE_VCHAR	= 3,	//char varying	不定长
	MR_SQLTYPE_UNIVCHAR = 6		//unicode varChar不定长
}MR_sqlType_E;

typedef enum
{
	MR_CLOUD_RESULT_TYPE_MODIFY = 1,	//结果集类型：修改
	MR_CLOUD_RESULT_TYPE_QUERY  = 2		//结果集类型：查询
}MR_cloudStorage_RsType_E;

typedef struct  
{
	int32 responseCode;			//服务器返回的code
	uint16* responseMsg;		//执行结果的错误提示utf-16BE编码（非必须，可能是NULL）
	int32 resultSetNum;			//执行结果的结果集的g个数（非必须，如果是查询这个信息就没用了）
}MR_SQL_EXE_INFO_T;

typedef struct  
{
	MR_cloudStorage_RsType_E rsType;	//执行结果的状态码，取值为MR_cloudStorage_errcode_E枚举中的数据
	int32 resultCode;
	uint16* errorMsg;
	int32 columnCount;
	int32 rowCount;
}MR_SQL_RS_INFO_T;

typedef struct  
{
	int16 year;
	int16 month;
	int16 day;
	int16 hour;
	int16 minute;
	int16 seconds;
	int16 milliSecond;
}MR_SQL_DATE_T;

/*执行mrc_sql_execute后返回结果的回调函数*/
typedef void (*MR_SQL_EXE_CB)(int32 exeResult, MR_SQL_EXE_INFO_T mr_sqlExeInfo);	//执行SQL语句完成后的回调函数

//执行mrc_sql_start后的回调函数
typedef void (*MR_Complete_CB)(int32 dlResult);

/*函数名：mrc_sql_start
	功能：初始化云存储
	输出：MR_WAITING：版本正在更新
		  MR_SUCCESS 加载成功
		  MR_FAILED 加载失败
*/
int32 mrc_sql_start(MR_Complete_CB cb);

/****************************************************************************************************
	函数名：mrc_sql_setSecretStr
	功能：设置cp 开发应用的secret字符串
	输出：无
****************************************************************************************************/
void mrc_sql_setSecretStr(char* secretStr);

/****************************************************************************************************
	函数名：mrc_sql_appendStatement
	功能：设置好将要执行的sql语句，可以append多次，以打包多句sql语句，注意参数一般也需要设置多次
	format说明：%A MR_SQLTYPE_BYTEA 注意!!!这个参数需对应两个变量，一个是bytea的首地址，另一个是bytea的长度!!!
					例："...%A...", ...,byteaAddr,byteaSize,... 也就是在byteaAddr额外附加上bytea的长度，
					这个长度变量不需要任何%对应，其他格式无特殊要求，一一对应即可
				%O MR_SQLTYPE_BOOLEAN
				%I MR_SQLTYPE_INTERGER
				%B MR_SQLTYPE_BIGINT
				%T MR_SQLTYPE_TIMESTP
				%V MR_SQLTYPE_VCHAR
	其他注意事项：数据传入时需要以变量形式传入，例如在传bigInt类型时，如果直接写常数4，则编译器默认将其当int32处理，
				  导致后续参数堆栈数据读取错误，但传入变量可避免此问题
	输入：
		  sqlStatement 需要打包的sql语句
		  paramFormat 参数格式字符串
		  ...		  附带的参数	
	输出：MR_SUCCESS 成功
		  MR_FAILED  失败		  
****************************************************************************************************/
int32 mrc_sql_appendStatement(const char* sqlStatement, const char* paramFormat, ...);

/*
  与mrc_sql_appendStatement类似，第三个参数改为void**结构，需要传入应用自行设置的数组指针
  数组内容必须与format中的一一对应，format格式与mrc_sql_appendStatement中完全一致
*/
int32 mrc_sql_appendStatementEx(const char* sql, const char* format, void** arr);

/****************************************************************************************************
	函数名：mrc_sql_execute
	功能：执行设置好的sql语句
	输入：query_cb 通知调用者的回调函数
		  skyId: 获取的skyId，没有则填0
	输出：MR_WAITING
****************************************************************************************************/
int32 mrc_sql_execute(MR_SQL_EXE_CB query_cb, int32 skyid);

/****************************************************************************************************
	函数名：mrc_sql_getResultSetInfo
	功能：获得结果集的相关信息
	输入：resultSetIndex 结果集索引号
	输出：mr_sqlRsInfo：获得的信息
	返回：MR_SUCCESS 成功
		  MR_FAILED  成功
****************************************************************************************************/
int32 mrc_sql_getResultSetInfo(int32 resultSetIndex, MR_SQL_RS_INFO_T* mr_sqlRsInfo);

/****************************************************************************************************
	函数名：mrc_sql_resolveRS
	功能：通用解析数据函数，可解析获得所有SQLtype支持类型的数据
	输入：resultSetIndex 结果集索引号
		  row 目标数据的所在行
		  column 目标数据所在列
	输出：pData：存放获得的数据
		  len：
	返回：MR_SUCCESS 成功
		  MR_FAILED  成功
****************************************************************************************************/
int32 mrc_sql_resolveRS(int32 resultSetIndex, int32 row, int32 column, void* pData, int32* len);

/****************************************************************************************************
	函数名：mrc_sql_releaseResource
	功能：释放所有资源，结束sql操作
	输入：无
	输出：无
****************************************************************************************************/
void  mrc_sql_releaseResource(void);	//释放被占用的资源

/*主功能接口结束*/




/*一些辅助接口*/
/*********************************************************
	方便读取数据而增加的接口，以下接口可用
	通用解析函数mrc_sql_resolveRS实现相同功能
	如果习惯使用mrc_sql_resolveRS下面函数可忽略
**********************************************************/
/****************************************************************************************************
	函数名：mrc_sql_resolve_boolen
	功能：解析boolen数据
	输入：resultSetIndex 结果集索引号
		  row 目标数据的所在行
		  column 目标数据所在列
		  len 
	返回：解析获得的boolen数据
****************************************************************************************************/
uint8 mrc_sql_resolve_boolen(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	函数名：mrc_sql_resolve_int
	功能：解析int数据
	输入：resultSetIndex 结果集索引号
		  row 目标数据的所在行
		  column 目标数据所在列
	返回：解析获得的int数据
****************************************************************************************************/
uint32 mrc_sql_resolve_int(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	函数名：mrc_sql_resolve_bigInt
	功能：解析bigInt数据
	输入：resultSetIndex 结果集索引号
		  row 目标数据的所在行
		  column 目标数据所在列
	返回：解析获得的bigInt数据
****************************************************************************************************/
uint64 mrc_sql_resolve_bigInt(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	函数名：mrc_sql_resolve_timeStamp
	功能：解析时间日期数据
	输入：resultSetIndex 结果集索引号
		  row 目标数据的所在行
		  column 目标数据所在列
	返回：解析获得的timeStamp数据
****************************************************************************************************/
MR_SQL_DATE_T mrc_sql_resolve_timeStamp(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	函数名：mrc_sql_resolve_byte
	功能：解析byte数组数据
	输入：resultSetIndex 结果集索引号
		  row 目标数据的所在行
		  column 目标数据所在列
	输出：len 获得的数据长度
	返回：解析获得的byte数据的首指针
****************************************************************************************************/
const uint8* mrc_sql_resolve_bytea(int32 resultSetIndex, int32 row, int32 column, int32* len);

/****************************************************************************************************
	函数名：mrc_sql_resolve_byte
	功能：解析varchar数据
	输入：resultSetIndex 结果集索引号
		  row 目标数据的所在行
		  column 目标数据所在列
	输出：len 获得的字符串的长度
	返回：解析获得的char数据的首指针
****************************************************************************************************/
const char* mrc_sql_resolve_varChar(int32 resultSetIndex, int32 row, int32 column, int32* len);

/************************************************************************/
/*
	获取服务器时间的回调
	输出：
		result：是否成功
		date：时间戳，仅当result==MR_OPENSKY_SUCCESS时有效
*/
/************************************************************************/
typedef void (*MR_SQL_GETTIME_CB)(int32 result, MR_SQL_DATE_T date);

/************************************************************************/
/*
	获取服务器时间
	输入：
		cb：回调函数
	返回：
		MR_OPENSKY_RETCODE，当MR_OPENSKY_WAITING时需要等待回调
*/
/************************************************************************/
int32 mrc_sql_getServerTime(MR_SQL_GETTIME_CB cb);

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////道具商城(prop store)模块///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_PROP_STORE	//分割符，无实际意义
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef enum
{
	OPENSKY_STORE_WAP_PAGE_INDEX = 1,
	OPENSKY_STORE_WAP_PAGE_ADD_PACK = 2
}MR_OPENSKY_STORE_WAP_PAGE_TYPE;

typedef enum{
	MR_PROPSTORE_FAILED  = -1,	//失败
	MR_PROPSTORE_SUCCESS = 0,	//成功
	MR_PROPSTORE_EXPIRED,		//商品过期
	MR_PROPSTORE_INVAILD,		//商品不存在
	MR_PROPSTORE_LIMIT,			//商品超过数量
	MR_PROPSTORE_INVAILD_PARAM,	//参数错误
	MR_PROPSTORE_BLACK_LIST	,	//黑名单用户
	MR_PROPSTORE_FORCE_UPDATE,	//强制更新
	MR_PROPSTORE_SUGG_UPDATE,	//建议更新
	MR_PROPSTORE_INVAILD_USER,	//错误的用户名或密码
	MR_PROPSTORE_SERVER_BUSY,	//服务器忙
	MR_PROPSTORE_OTHER_FAILURE	//其他错误,一般由本地错误引起,如联网失败等
}MR_ResultCode_E;


typedef void (*MR_PROPSTORE_WAP_CB)(int32 resultCode);

typedef void (*MR_PROPSTORE_CB)(MR_ResultCode_E resultCode, int32 packVolumn, int32 itemCount, uint16* serverMsg);

typedef void (*MR_PROPSTORE_DEL_CB)(MR_ResultCode_E resultCode, int8* exeResult, uint16* serverMsg);

/********************************************************
					道具相关接口
********************************************************/
/****************************************************************************************************
	函数名：mrc_propStore_queryUserPack
	功能：  查询用户背包，用于返回当前活跃用户背包内的所有道具
	输入：
			columnName 配置返回的列名，可选字段：specid,descid,remain,period,num,attr 中间以逗号隔开。
			各字段含义: 
			         specid：实例号，在服务端的具体编号，用来操作delete等操作
				  descid:  CP提交给斯凯的道具编号，用来游戏内部的道具ID	
				  remain：表示道具剩余的耐久度，即还可使用多少天
				  period:   表示道具总的耐久度
				  num:   道具数量
				  attr:   道具属性结构
			queryCb    查询回调
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
	回调输出：
			result	   执行结果 成功：MR_SUCCESS 失败：MR_FAILED
			resultCode 无效
			packVolumn 背包总容量
			itemCount  商品数量
			serverMsg  服务器端返回的消息，可能为NULL		
****************************************************************************************************/
int32 mrc_propStore_queryUserPack(char* columnName, MR_PROPSTORE_CB queryCb);

/****************************************************************************************************
	函数名：mrc_propStore_consumeGoods
	功能：	消耗消耗类商品
	输入：
			id		   消耗商品的ID号
			num		   消耗的数量
			queryCb    查询回调
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
	回调输出：
			result	   执行结果 成功：MR_SUCCESS 失败：MR_FAILED
			resultCode 见MR_ResultCode_E定义
			packVolumn 无效
			itemCount  无效
			serverMsg  服务器端返回的消息，可能为NULL
****************************************************************************************************/
int32 mrc_propStore_consumeGoods(int32 id, int32 num, MR_PROPSTORE_CB queryCb);

/****************************************************************************************************
	函数名：mrc_propStore_deleteGoods
	功能：	删除商品，同时用于转化服务器端背包到本地，使用非消耗类道具，可同时进行多个删除，以数组的形式传入
	输入：
			idArray		删除商品的ID号数组
			itemCount	商品个数
			queryCb     查询回调
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
	回调输出：
			result		执行结果
			exeResult   包含成功信息的数组，与idArray一一对应，成功为MR_SUCCESS，不成功为MR_FAILED
			serverMsg	服务器返回的信息
****************************************************************************************************/
int32 mrc_propStore_deleteGoods(int32* idArray, int32 itemCount, MR_PROPSTORE_DEL_CB queryCb);

/****************************************************************************************************
	函数名：mrc_propStore_getGoods
	功能：	根据key获取商品
	输入：
			getKey		获得商品的key，类似于一个宝箱ID，boss的ID等
			columnName	配置返回的列名，可选字段：specid,descid,remain,period,num,attr 中间以逗号隔开
			queryCb     查询回调
			isGetPack	是返回获得道具后的整个背包还是仅返回此次获得的道具，0返回获得的道具，1返回整个背包
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
	回调输出：
			result	   执行结果 成功：MR_SUCCESS 失败：MR_FAILED
			resultCode 见MR_ResultCode_E定义
			packVolumn isGetPack = 0 时无效，isGetPack = 1 时返回背包容量
			itemCount  获得的商品数
			serverMsg  服务器端返回的消息，可能为NULL		
****************************************************************************************************/
int32 mrc_propStore_getGoods(int32 getKey, char* columnName, MR_PROPSTORE_CB queryCb, int32 isGetPack);

/****************************************************************************************************
	函数名：mrc_propStore_enterWap
	功能：调用浏览器，进入wap页面，在线查看用户背包等信息，并购买可在线道具
	输入：
			page	需要打开的页面取值如下：
					取值为：OPENSKY_STORE_WAP_PAGE_INDEX		打开用户在线背包首页
					取值为：OPENSKY_STORE_WAP_PAGE_ADD_PACK	打开背包扩容页面
			wapCb	调用浏览器的回调函数
					回调中传回的resultCode参数取值为：
							MR_SUCCESS			调用浏览器成功
							MR_FAILED			调用失败
****************************************************************************************************/
int32 mrc_propStore_enterWap(int32 page, MR_PROPSTORE_WAP_CB wapCb);

/********************************************************
				解析接口，解析类型不限
********************************************************/

/****************************************************************************************************
	函数名：mrc_propStore_resolve
	功能：  数据解析器
	输入：
			row		解析数据所在行
			column	解析数据所在列
			pData	解析后的数据
			len		解析获得的数据的占用字节长度 0则说明数据库中对应为NULL
	输出：
			MR_SUCCESS 成功
			MR_FAILED  失败
****************************************************************************************************/
int32 mrc_propStore_resolve(int32 row, int32 column, void* pData, int32* len);

/********************************************************
				释放资源接口
********************************************************/
void mrc_propStore_release(void);

/**
* 上传道具回调
* resultCode:上传结果
*      MR_FAILED 失败
*      MR_SUCCESS 成功
*      其他:
*          2:商品过期
*          3:商品不存在
*          4:商品超过数量
*          5:上传道具版本错误
*          6:上传道具属性可疑，道具入库，标识为可疑属性
*          7:上传道具属性数目不匹配
*          8:非法道具上传成功，道具入库，标识为非法版本上传
*          9:可疑道具上传成功，道具入库，标识为可疑版本上传，3次可疑版本上传后，则认为是一次非法上传
* ver:下一次上传服务端期望的版本号，当resultCode==MR_FAILED时，此值不确定
* 注意：失败时请至少重新再请求一次
**/
typedef void (*mrc_propStore_uploadItem_cb)(int32 resultCode, int32 ver);
/**
* 上传道具前置-设置道具属性
* 注意：最多设置256个属性
* order:属性编号
* value:属性值
* 以上参数由应用与服务端共周商议, 本插件只做一个转发
* 注意：属性设置在调用 mrc_propStore_uploadItem 后将被清空，如需再上传，需要重新调用本接口
* 返回:
*		MR_OPENSKY_SUCCESS:成功添加属性
*		MR_OPENSKY_IGNORE:已经设置超过256,该属性被忽略,上传属性后会清空所有属性
**/
int32 mrc_propStore_uploadItem_setItemProperty(int32 order, int32 value);
/**
* 上传道具，使用本接品前请先登录通行证
* ver:道具版本号
* cpGoodsId:道具ID(由CP自行定义)，道具的属性请参见 mrc_propStore_uploadItem_setItemProperty 接口
* num:道具数量
* cb:回调函数，不能为NULL
* 注意：在调用本函数前，应用应当暂停自己的逻辑（特别是必须要停止界面刷新），直到cb回调通知结果才能继续应用逻辑
* 无须关心返回值
**/
int32 mrc_propStore_uploadItem(int32 ver, int32 cpGoodsId, int32 num, mrc_propStore_uploadItem_cb cb);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////成就系统(achievement)模块///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_ACHIEVEMENT	//分割符，无实际意义
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef enum
{
	MR_ACHIEVEMENT_ERR_FORCE_UPDATE		 = 3,	//强制更新
	MR_ACHIEVEMENT_ERR_RECOMMEND_UPDATE	 = 4,	//建议更新
	MR_ACHIEVEMENT_ERR_NOSKYID				 = 5,	//skyid不存在
	MR_ACHIEVEMENT_ERR_NOID					 = 8,	//成就id不存在
	MR_ACHIEVEMENT_ERR_UNLOCKED			 = 9,	//已经解锁(重复解锁)
	MR_ACHIEVEMENT_ERR_SERVERERROR			 = 99	//服务器错误
}MR_Opensky_Achievement_errcode_E;
//其他定义请参考云插件返回值


/**
 * 启动浏览器回调函数
 * 输出:
 * result   回调结果
 *          MR_SUCCESS 成功
 *          其他 错误代码
 */
typedef void (*MR_ACHIEVEMENT_STARTBROWSER_CB)(int32 result/*ErrorCode*/, int32 var1, int32 var2, int32 var3, int32 var4, int32 var5);

/**
 * 插件下载回调函数
 * 输出:
 * result   回调结果
 *          MR_SUCCESS 成功
 *          MR_FAILED  失败
 */
typedef void (*MR_ACHIEVEMENT_DL_CB)(int32 result);

/**
 * 解锁成就的回调函数(当且仅当mrc_achievement_unlock返回为MR_SUCCESS会被调用)
 * 输出:
 * result   回调结果
 *          MR_SUCCESS 成功
 *          MR_FAILED  失败
 *          MR_Opensky_Achievement_errcode_E 枚举的  其它错误值
*/
typedef void (*MR_ACHIEVEMENT_UNLOCK_CB)(int32 result);

/****************************************************************************************************
 * 插件初始化
 * 输入:
 * dlcb     插件下载回调函数
 * 返回:
 *          MR_SUCCESS 成功
 *          MR_FAILED 失败
 *          MR_WAITING 等待中, 等待插件下载回调
 *          请等待回调函数返回
****************************************************************************************************/
int32 mrc_achievement_init(MR_ACHIEVEMENT_DL_CB dlcb);

/****************************************************************************************************
 * 插件反初始化, 反初始化后, 若要重新工作, 必须再次调用mrc_achievement_init
 * 返回:
 *          MR_SUCCESS 成功
 *          MR_FAILED  失败
****************************************************************************************************/
int32 mrc_achievement_uninit(void);

/****************************************************************************************************
 * 解锁成就(必须先登录)
 * 输入:
 * cb              回调函数
 * achievementId   成就点标识(通过成就系统后台进行查询)
 * costTime        (选填)此项为选填项,标识解锁成就所花费的时间(以秒(待定)为单位)
 * 返回:
 * MR_SUCCESS   成功
 * MR_FAILED    失败(请确认已经登录)
****************************************************************************************************/
int32 mrc_achievement_unlock(MR_ACHIEVEMENT_UNLOCK_CB cb, int32 achievementId, int32 costTime);

/****************************************************************************************************
 * 启动浏览器, 进入成就系统
 * 输入:
 * cb              回调函数
 * 返回:
 * MR_SUCCESS: 浏览器成功运行后退出
 * MR_FAILED: 浏览器未能成功运行
****************************************************************************************************/
int32 mrc_achievement_startBrowser(MR_ACHIEVEMENT_STARTBROWSER_CB cb);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////FEED和好友(feed)模块///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_FEED	//分割符，无实际意义
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

typedef enum
{
	MR_FEED_SUCCESS		 = 0,	//成功
	MR_FEED_INVAILD_PARAM	 = 1,	//参数错误
	MR_FEED_BLACK_LIST		 = 2,	//黑名单用户
	MR_FEED_FORCE_UPDATE	 = 3,	//强制更新
	MR_FEED_SUGG_UPDATE	 = 4,	//建议更新
	MR_FEED_INVAILD_USER	 = 5,	//错误的用户名或密码
	MR_FEED_SERVER_BUSY	 = 6,	//服务器忙
	MR_FEED_OTHER_FAILURE	 = 7	//其他错误,一般由本地错误引起,如联网失败等
}MR_OPENSKY_feedAndBuddy_errcode_E;

/************************************************************************/
/*
	执行独立feed以及好友操作的回调
	in:
		resultCode:详见MR_feedAndBuddy_errcode_E
		resultMsg:上传后的由服务端返回的提示字符串
*/
/***********************************************************************/
typedef void (*mrc_feedAndBuddy_cb)(int32 resultCode,uint16* resultMsg);

/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_uploadFeed
	功能:上传本地用户的feed
	in:
		skyid:要上传feed的玩家的skyid
		feed:feed文本，utf16be编码
		category:feed上传的动态类别
		cb:mrc_feedAndBuddy_cb类型的回调
	out:
		MR_SUCCESS:正在上传feed
		MR_WAITING:正在下载云存储插件
		MR_FAILED:传入参数错误或插件加载失败
	cb:
		mrc_feedAndBuddy_cb类型回调
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_uploadFeed(int32 skyid,uint16* feed,uint16* category,mrc_feedAndBuddy_cb cb);


/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_uploadFeedEx
	功能:上传本地用户的feed,并从服务端获取feed
	in:
		skyid:要上传feed的玩家的skyid
		feed:feed文本，utf16be编码
		category:feed上传的动态类别
		type:type为1表示获取的是应用的feed，type为2表示获取的是好友的feed
		startOffset:feed的偏移量
		limit:feed获取的条数
		getCategory:获取的feed的动态类别
		cb:mrc_feedAndBuddy_cb类型的回调
	out:
		MR_SUCCESS:正在与服务端交互
		MR_WAITING:正在下载云存储插件
		MR_FAILED:传入参数错误或插件加载失败
	cb:
		mrc_feedAndBuddy_cb类型回调
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_uploadFeedEx(int32 skyid,uint16* feed,uint16* category,int32 type,int32 startOffset,int32 limit,uint16* getCategory,mrc_feedAndBuddy_cb cb);


/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_getFriendFeed
	功能:获取本地用户的好友feed
	in:
		skyid:要获取feed的玩家的skyid
		startOffset:feed的偏移量
		limit:feed获取的条数
		category:获取的feed的动态类别
		cb:mrc_feedAndBuddy_cb类型的回调
	out:
		MR_SUCCESS:正在获取feed
		MR_WAITING:正在下载云存储插件
		MR_FAILED:传入参数错误或插件加载失败
	cb:
		mrc_feedAndBuddy_cb类型回调
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_getFriendFeed(int32 skyid,int32 startOffset,int32 limit,uint16* category,mrc_feedAndBuddy_cb cb);

/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_getAppFeed
	功能:获取应用feed
	in:
		startOffset:feed的偏移量
		limit:feed获取的条数
		category:获取的feed的动态类别
		cb:mrc_feedAndBuddy_cb类型的回调
	out:
		MR_SUCCESS:正在获取feed
		MR_WAITING:正在下载云存储插件
		MR_FAILED:传入参数错误或插件加载失败
	cb:
		mrc_feedAndBuddy_cb类型回调
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_getAppFeed(int32 startOffset,int32 limit,uint16* category,mrc_feedAndBuddy_cb cb);

/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_addFriend
	功能:加好友
	in:
		curSkyid:当前有效的skyid
		recvSkyid:接收好友请求的skyid
		cb:mrc_feedAndBuddy_cb类型的回调
	out:
		MR_SUCCESS:正在获取feed
		MR_WAITING:正在下载云存储插件
		MR_FAILED:传入参数错误或插件加载失败
	cb:
		mrc_feedAndBuddy_cb类型回调
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_addFriend(int32 curSkyid,int32 recvSkyid,mrc_feedAndBuddy_cb cb);


/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_sendMsg
	功能:向特定skyid发送消息
	in:
		curSkyid:当前有效的skyid
		recvSkyid:接收消息的skyid
		content:发送的消息内容，utf16be编码
		cb:mrc_feedAndBuddy_cb类型的回调
	out:
		MR_SUCCESS:正在获取feed
		MR_WAITING:正在下载云存储插件
		MR_FAILED:传入参数错误或插件加载失败
	cb:
		mrc_feedAndBuddy_cb类型回调
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_sendMsg(int32 curSkyid,int32 recvSkyid,uint16* content,mrc_feedAndBuddy_cb cb);



/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_getNoticeNum(同步接口)
	in:
		无
	out:
		能获取到的有效的公告条数，0表示没有
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_getNoticeNum(void);


/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_getNotice(同步接口)
	in:
		index:获取公告的index，从0至(mrc_feedAndBuddy_getNoticeNum-1)
	out:
		获取到的公告，utf16be编码，如果获取不到，则返回NULL
*/
/***********************************************************************/
uint16* mrc_feedAndBuddy_getNotice(int32 index);

/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_getFeedNum(同步接口)
	in:
		无
	out:
		能获取到的有效的feed条数，0表示没有
*/
/***********************************************************************/
int32 mrc_feedAndBuddy_getFeedNum(void);

/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_getFeed(同步接口)
	in:
		index:获取公告的index，从0至(mrc_feedAndBuddy_getFeedNum-1)
	out:
		获取到的feed，utf16be编码，如果获取不到，则返回NULL
*/
/***********************************************************************/
uint16* mrc_feedAndBuddy_getFeed(int32 index);

/***********************************************************************/
/*	
	函数名:mrc_feedAndBuddy_releaseResource(同步接口)
	说明:所释放的资源包括，由该feedAndBuddy模块分配的用户存储feed以及notice的内存
	in:
		无
	out:
		MR_SUCCESS:释放成功
*/
/***********************************************************************/
void mrc_feedAndBuddy_releaseResource(void);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////联网PK(netPK)模块///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_NEKPK	//分割符，无实际意义
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/*
  游戏角色	
*/
typedef struct 
{	
	uint32 ctrl;                            /*0: 终端控制PK结果奖励  1: 服务端控制PK结果奖励*/
	uint32 type;							/*当作为获取到的角色信息，0：普通玩家；1：官方BOSS*/
	int32  win;                             /*胜场数量*/ 
	int32  lost;                            /*败场数量*/
	int32  StrengthValue;	                /*实力指数*/
	int32 sex;								/*1表示男，2表示女,0表示性别不明*/
	int32 bitmap;							/*用于是否有某个bitmap属性，1表示有，0表示没有*/
	int32 day_win;							/*当日所赢场次*/
	int32 week_win;							/*本周所赢场次*/
	/*************** 预定单元格属性格式 ****************/
	/* MAX_PREFIXDATA_COUNT 个属性值                   
	VALUE1,VALUE2,VALUE3,...                         
	由游戏自己定义具体的单元为何种含义,如定义:               
	  prefixData[0] 存放体力                         
	  prefixData[1] 存放等级                            
	  各个单元的缺省值为0                            
	  单元格可以用于从服务端查询特定的游戏角色       */
	/***************************************************/

	int32 prefixData[MAX_PREFIXDATA_COUNT];
    
	/************** 自定义单元格的格式 *****************/ 
	/* 预定单元格只能存放int32的数值，无法存放如字符串 
	 或者字节数组这样类型的数据，customData用于存放由
	 游戏自己组织的数据格式,存放在自定义数据区的数据无
	 法用于查询。                                    */ 
	/***************************************************/
	uint8 *customData;      /*自定义数据*/            
	int32 dataLen;			/*自定义数据长度*/ 
	uint32 skyid;			/*skyid*/
	uint16 nickName[MAX_NICKNAME_LEN+1];    /*昵称*/ 
	uint16 title[MAX_TITLE_LEN+1];  		/*头衔*/
	char  identify[MAX_IDENTIFY_LEN];      /*服务器描述*/

}MR_Character;

/*角色信息列表*/
typedef struct tagCharacterList
{
	MR_Character *pCharacter;
	struct tagCharacterList *pNext;
}MR_CharacterList;


/*预设过滤条件*/
typedef enum
{
   MR_PKFilter_STRENGTH = 0,	     /*综合实力最强*/
   MR_PKFilter_SIMILAR,	             /*实力相近*/
   MR_PKFilter_FIGHTWITHME,	         /*最近挑战过自己*/
   MR_PKFilter_FEMALE,	             /*女性玩家*/
   MR_PKFilter_DAY_CHAMPION,	     /*日冠军*/
   MR_PKFilter_WEEK_CHAMPION,	     /*周冠军*/
   MR_PKFilter_CUSTOM,   	         /*自定义*/ 
   MR_PKFilter_BUDDY				/*获取好友的角色列表*/
}MR_PKFilter;

/*错误码*/
typedef enum
{
	MR_PKErrorCode_SUCCESS = 0,			/* 成功           */
	MR_PKErrorCode_UserCancel,			/* 用户取消       */
	MR_PKErrorCode_DIAL,				/* 拨号错误       */
	MR_PKErrorCode_Network,				/* 网络错误       */
	MR_PKErrorCode_InvalidateUser,		/* 无效用户       */
	MR_PKErrorCode_BlackListUser,		/* 黑名单用户     */ 
	MR_PKErrorCode_FormatError,			/*格式错误			*/
	MR_PKErrorCode_NotEnoughMem,		 /*内存不够			*/
	MR_PKErrorCode_OtherError,
	MR_PKErrorCode_ForceUpdate,			/*强制更新*/
	MR_PKErrorCode_RecommendedUpdate	/*建议更新*/	
}MR_PKErrorCode;



typedef enum
{
	MR_PKPageFlag_noPage,	//获取到的列表没有上一页，也没有下一页
	MR_PKPageFlag_next,		//获取到的列表只有上一页
	MR_PKPageFlag_previous,	//获取到的列表只有下一页
	MR_PKPageFlag_both		//获取到的列表有上一页也有下一页
}MR_PKPageFlag;

/*PK结果*/
typedef struct 
{
	uint32 winner;    /*赢家SKYID*/ 
	uint32 loster;    /*输家SKYID*/ 
}MR_PKResult;

/*PK分区服务器信息*/
typedef struct  
{
	int32 serverID;							//分区服务器ID
	uint16 serverName[MAX_SERVERNAME_LEN+1];	//分区服务器名称
	int32 ServerUserTotal;					//分区服务器能容纳的人数
	int32 serverStatus;						//分区服务器的状态
	int32 femaleNum;						//分区服务器目前的女性玩家数量
}MR_PKServerInfo;

/*
	联网PK回调函数原型：
		typedef void (*mr_safeNetpk_cb)(int32 resultCode,uint16* resultMsg,void* extraInfo);
	参数：
		resultCode：错误码，MR_PKErrorCode_SUCCESS表示云插件下载成功
					MR_PKErrorCode_OtherError表示下载失败
		resultMsg：服务端返回的错误提示字符串

*/
typedef void (*mr_safeNetpk_cb)(int32 resultCode,uint16* resultMsg);

/************************************/
/*mrc_netpk_commit_pkresult的回调原型 
errCode:   错误码                
respData:  从服务端反馈的数据，不需要应用释放，目前为utf16be编码的文字     
len:       respData的长度        */  
/************************************/
typedef void (*MR_Netpk_commit_PKResult_CB)(MR_PKErrorCode errCode,const char* respData,int32 len);

/****************************************/
/*mrc_netpk_upload_appdata的回调原型 
errCode:   错误码                
respData:  从服务端反馈的数据,不需要应用释放，目前为utf16be编码的文字    
len:       respData的长度				*/  
/****************************************/
typedef void (*MR_Netpk_upload_appdata_CB)(MR_PKErrorCode errCode,const char* respData,int32 len);

/*****************************************************/
/* mrc_getPkList的回调原型                           
 errCode: 错误码                                    
 lst:  PK角色列表,仅当errCode为MR_PKErrorCode_SUCCESS时有效.                     
		 lst释放说明：
			 lst是一个链表结构，最后一个节点指向NULL。应用负责释放每个节点中pCharacter所指向的内存，
			 以及pNext所指向的内存（如果pNext不为NULL）。特别注意，MR_Character中customData指针所指向的
			 内存也需由应用释放。
			 典型的释放如下：
		 void demo_freeList(MR_CharacterList* lst)
		 {
			 MR_CharacterList* pNextNode;
			 if(lst != NULL)
			 {
				 pNextNode = lst->pNext;
				 while(pNextNode)
				 {	
					 lst->pNext = pNextNode->pNext;
					 if(pNextNode->pCharacter->customData != NULL)
					 {
						 free(pNextNode->pCharacter->customData);
						 pNextNode->pCharacter->customData =NULL;
					 }
					 free(pNextNode->pCharacter);
					 pNextNode->pCharacter = NULL;
					 free(pNextNode);
					 pNextNode = pNextNode->pNext;
				 }
				 if(lst->pCharacter->customData != NULL)
				 {
					 free(lst->pCharacter->customData);
					 lst->pCharacter->customData = NULL;
					 
				 }
				 if(lst->pCharacter!=NULL)
				 {
					 free(lst->pCharacter);
					 lst->pCharacter = NULL;
				 }
				 free(lst);
				 lst = NULL;
			 }
		  }								               
*/
/*****************************************************/
typedef void (*MR_GetPKList_CB)(MR_PKErrorCode errCode,MR_CharacterList*  lst,MR_PKPageFlag pageFlag);

/****************************************************************************************************/
/*	mrc_netpk_openWap的回调原型																		
	result:																							
		MR_SUCCESS:浏览器成功运行后退出																
		MR_FAILED:浏览器未能成功运行																*/
/****************************************************************************************************/
typedef void (*MR_Netpk_openWap_CB)(int32 result);


/****************************************************************************************/
/*    根据筛选条件得到PK列表                                                           
    输入:                                                                             
        nTop:    得到符合条件的前几个记录                                             
        filter:  预设过滤条件,当预设过滤条件不为PKFilter_CUSTOM时,customFilter无效   
        customFilter: 自定义过滤条件,仅当filter为PKFilter_CUSTOM时有效             
                           customFilter以mrc_character中prefixData为筛选单位           
						customFilter为简单规则的筛选表达式,支持下面的规则:		       					
							   C(N) = Value     等于                                            
							   C(N) <> Value    不等于                                 
							   C(N) > Value     大于                                    
							   C(N) < Value     小于                                   
							   orderby C(N)  desc(asc)   排序                          
							   desc: 降序                                               
							   asc:升序                                                 
							   and  并且                                                
							   or   或者                                               
							   (..) 逻辑判断优先级                                     
                                                                                    
							Value 为int32类型值                                        
							1<=N<MAX_PREFIXDATA_COUNT                                  
                                                                                     
			例:   C1=1000 and (C2>241 or C5<>0)  orderby C1 desc                        
			含义可以为: 体力为1000并且(等级大于241或者魔法不等于0)的记录筛选出来，      
			最后以体力的降序排列来筛选													                    
        nOffset: 选择记录集的第nOffset开始                                             
        nCount:    得到nOffset之后的nCount条记录
		flag:	在获取列表的时候是否同时也获取自身数据，如果同时获取自身数据，则回调函数的第一个结点中
				的角色信息是本地登录角色。flag为TRUE表示需要带携带自身数据，在这种情况下，获得的角色
				会比nCount多一个；flag为FALSE表示不需要携带自身数据.
        cb:      得到PK角色的回调                                                   
                                                                                      
     返回:                                                                            
                                                                                       
        MR_FAILED:    失败,参数传递错误
		MR_WAITING:	  正在下载插件
        MR_SUCCESS:   等待                                                            */
/****************************************************************************************/    
int32 mrc_netpk_getPKList(MR_PKFilter filter,
						  const char* customFilter,
						  int32 nOffset,
						  int32 nCount,
						  int32 flag,
						  MR_GetPKList_CB cb);


/****************************************************************************************
	说明：
		在获取列表的同时上传自身属性
	函数原型：
	int32 mrc_netpk_getPKListWithAppdata(MR_PKFilter filter,
										const char* customFilter,
										int32 nOffset,
										int32 nCount,
										int32 flag,
										MR_Character* owner,
										uint16* feed_desc,
										int32 propNum,
										uint16** propStr,
										MR_GetPKList_CB cb);
	参数：
		输入：
			filter：预定义的筛选条件，详见：MR_PKFilter
			customFilter：只有在filter为MR_PKFilter_CUSTOM时该参数有效。
			nOffset：查询到的结果中的偏移量
			nCount：总共要获取的总条数
			flag：	在获取列表的时候是否同时也获取自身数据，如果同时获取自身数据，则回调函数的第一个结点中
					的角色信息是本地登录角色。flag为TRUE表示需要带携带自身数据，在这种情况下，获得的角色
					会比nCount多一个；flag为FALSE表示不需要携带自身数据.
			owner：本地角色信息
			feed_desc：欲上传的字符串，utf16be编码
			propNum：上传的自定义字符串的条数，最大5条，将在wap短显示
			propStr：上传的字符串数组
			cb：得到PK角色的回调
		输出：	MR_SUCCESS: 正在进行联网操作
				MR_WAITING: 正在下载云存储插件
				MR_FAILED:	失败
****************************************************************************************/    
int32 mrc_netpk_getPKListWithAppdata(MR_PKFilter filter,
									 const char* customFilter,
									 int32 nOffset,
									 int32 nCount,
									 int32 flag,
									 MR_Character* owner,
									 uint16* feed_desc,
									 int32 propNum,
									 uint16** propStr,
									 MR_GetPKList_CB cb);


/****************************************************************************************************/
/*  向联网PK服务端发送角色信息，以更新该角色信息                                                  
    如果有需要的话，携带一个FEED 文本信息,FEED信息将会在WAP站点等展现。                       
															                                     
   FEED 文本信息格式为UTF16BE文本串,包含的一些将被服务器端替换的标识:						                                                       
     标识:  ${SKYID:<XXX>}  角色ID  将被替换成玩家昵称									
            ${APPID:<XXX>} 应用ID 将被替换成游戏名称											
     如: "${SKYID:<4503044>}在${APPID:<80345>}中战胜了${SKYID:<4503044>},等级提升到5级"    
	在服务端会被替换成:                                                                            
		 "江南游侠在龙站中战胜了北国飞鹰,等级提升到5级"																					
   输入:																						
         owner	:     需更新的角色信息															
         feed_desc:     Feed信息(UTF16BE)															
   返回:																							
          MR_FAILED:   失败，参数传递错误															
          MR_SUCCESS:  等待																		
   此函数可以在如下场合使用:                                                                      
          1. 更新当前游戏角色信息																	
		  2. 上传feed以进行广播												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_upload_appdata(MR_Character* owner,const uint16* feed_desc,MR_Netpk_upload_appdata_CB cb);


/****************************************************************************************************/
/*  向联网PK服务端发送角色信息，以更新该角色信息                                                  
	如果有需要的话，携带一个FEED 文本信息,FEED信息将会在WAP站点等展现。该接口同时还可上传若干字符串到服务器端，
	目前可上传道具信息也可作为游戏自定义字符串（utf16-be编码）使用，该字符串属性属于本地登录的角色，如果上传的
	角色中没有本地角色那么不上传该字符串。                      
															                                     
   FEED 文本信息格式为UTF16BE文本串,包含的一些将被服务器端替换的标识:						                                                       
     标识:  ${SKYID:<XXX>}  角色ID  将被替换成玩家昵称									
            ${APPID:<XXX>} 应用ID 将被替换成游戏名称											
     如: "${SKYID:<4503044>}在${APPID:<80345>}中战胜了${SKYID:<4503044>},等级提升到5级"    
	在服务端会被替换成:                                                                            
		 "江南游侠在龙站中战胜了北国飞鹰,等级提升到5级"																					
   输入:																						
         owner	:     需更新的角色信息															
         feed_desc:     Feed信息(UTF16BE)
		 propNum：	上传道具或游戏自定义字符串的数量
		 propStr：  指向道具或游戏自定义字符串数组的指针，字符串用utf16-be编码
   返回:																							
          MR_FAILED:   失败，参数传递错误															
          MR_SUCCESS:  等待																		
   此函数可以在如下场合使用:                                                                      
          1. 更新当前游戏角色信息																	
		  2. 上传feed以进行广播												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_upload_appdataEx(MR_Character* owner,
								 const uint16* feed_desc,
								 int32 propNum,
								 uint16** propStr,
								 MR_Netpk_upload_appdata_CB cb);


/****************************************************************************************************/
/*  向联网PK服务端PK结果																		
    如果有需要的话，携带一个FEED 文本信息,FEED信息将会在WAP站点等展现。                          
															                                      
   FEED 文本信息格式为UTF16BE文本串,包含的一些将被服务器端替换的标识:						                                                            
     标识:  ${SKYID:<XXX>}  角色ID  将被替换成玩家昵称									
            ${APPID:<XXX>} 应用ID 将被替换成游戏名称											
     如: "${SKYID:<4503044>}在${APPID:<80345>}中战胜了${SKYID:<4503044>},等级提升到5级"     
	在服务端会被替换成:                                                                             
		 "江南游侠在龙站中战胜了北国飞鹰,等级提升到5级"																					
   输入:																						
		   result	:	  包含PK中的winner以及loster的用户名，其所指向的内存由应用自己释放。									
         owner	:     需更新的角色信息															
		   count	:	  指明需要更新多少个角色信息,目前的取值应为0，1，2						
		   lstHead	:	  需要更新的角色的链表，lstHead所指向的内存由应用自己释放													
         feed_desc:     Feed信息(UTF16BE)，其所指向的内存由应用自己释放	
   返回:																						
          MR_FAILED:   失败，参数传递错误														
          MR_SUCCESS:  等待																	 
   此函数可以在如下场合使用:                                                                      
			1. PK后对觉得进行更新																	
			2. 上传feed以进行广播												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_commit_pkresult(MR_PKResult* result,
								int32 count,
								MR_Character* lstHead,
								const uint16* feed_desc,
								MR_Netpk_commit_PKResult_CB cb);




/****************************************************************************************************/
/*  向联网PK服务端PK结果																		
    如果有需要的话，携带一个FEED 文本信息,FEED信息将会在WAP站点等展现。该接口同时还可上传若干字符串到服务器端，
	目前可上传道具信息也可作为游戏自定义字符串（utf16-be编码）使用，该字符串属性属于本地登录的角色，如果上传的
	角色中没有本地角色那么不上传该字符串。
															                                      
   FEED 文本信息格式为UTF16BE文本串,包含的一些将被服务器端替换的标识:						                                                            
     标识:  ${SKYID:<XXX>}  角色ID  将被替换成玩家昵称									
            ${APPID:<XXX>} 应用ID 将被替换成游戏名称											
     如: "${SKYID:<4503044>}在${APPID:<80345>}中战胜了${SKYID:<4503044>},等级提升到5级"     
	在服务端会被替换成:                                                                             
		 "江南游侠在龙站中战胜了北国飞鹰,等级提升到5级"																					
   输入:																						
		   result	:	  包含PK中的winner以及loster的用户名，其所指向的内存由应用自己释放。									
         owner	:     需更新的角色信息															
		   count	:	  指明需要更新多少个角色信息,目前的取值应为0，1，2						
		   lstHead	:	  需要更新的角色的链表，lstHead所指向的内存由应用自己释放													
         feed_desc:     Feed信息(UTF16BE)，其所指向的内存由应用自己释放				
		 propNum：	上传道具或游戏自定义字符串的数量
		 propStr：  指向道具或游戏自定义字符串数组的指针，字符串用utf16-be编码
   返回:																						
          MR_FAILED:   失败，参数传递错误														
          MR_SUCCESS:  等待																	 
   此函数可以在如下场合使用:                                                                      
          1. PK后对觉得进行更新																	
			2. 上传feed以进行广播												                    */
/****************************************************************************************************/ 
int32 mrc_netpk_commit_pkresultEx(MR_PKResult* result,
								  int32 count,
								  MR_Character* lstHead,
								  const uint16* feed_desc,
								  int32 propNum,
								  uint16** propStr,
								  MR_Netpk_commit_PKResult_CB cb);


/****************************************************************************************************/
/*  访问当前游戏的联网PK专区																	
	输入:无																							
	输出:                                                                                           
      MR_SUCCESS:等待                                                                                                        
		MR_FAILED: 失败，参数传递错误 																
此函数可以在如下场合使用:																			                                                                     
          1. 用户访问wap端的当前游戏的联网PK专区													
			2. 查看各类feed信息																		*/		
/****************************************************************************************************/ 
int32 mrc_netpk_openWap(MR_Netpk_openWap_CB cb);


/********************************************************************/
/*
	获取当前可用公告条数
	输入：无
	输出：
		返回当前可用的公告条数，暂不限制公告条数
	此函数用于如下场合：
		1.在获取公告消息前获取公告条数
*/
/********************************************************************/
int32 mrc_netpk_getNoticeNum(void);


/********************************************************************************************/
/*	获取当前可用的第index条公告（index从0开始取，取值范围在（0，mrc_netpk_getNoticeNum-1）之间）
	输入：无
	输出：
		NULL：无公告可用或index出错
		字符串：utf16-be编码，以\x00\x00结尾。
此函数可以用在如下场合：
	1.用户获取当前可用的公告
*/
/******************************************************************************************/
uint16* mrc_netpk_getNotice(int32 index);


/********************************************************************************************/
/*
	用于确保联网PK插件在plugins目录下：云存储插件
	输入：mr_safeNetpk_cb
	输出：
		MR_SUCCESS:通过了本地插件版本检查
		MR_WAITING:正在下载云存储插件
		MR_FAILED:失败
*/
/******************************************************************************************/
int32 mrc_netpk_initVersion(mr_safeNetpk_cb cb);

/********************************************************************************************/
/*
	在下载完成自身的游戏后，将从原游戏路径下加载同名mrp，使原游戏重启。
	输入：无
	输出：
		MR_FAILED:下载的游戏路径不在原游戏路径下
		MR_SUCCESS:正在加载游戏
	此函数可以用在如下场合：
		1.在游戏自更新之后，对游戏进行重启
*/
/******************************************************************************************/
int32 mrc_netpk_restartSelf(void);

/*
	释放所占的内存空间
*/
int32 mrc_netpk_releaseRes(void);



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////入口(entry)模块///////////////////////////////////////////////////////////////////////////////////
#ifdef OPENSKY_ENTRY	//分割符，无实际意义
#endif
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



/******************************************************************************************
 * 初始化回调
 * 当 result != MR_OPENSKY_SUCCESS 表示插件加载失败，
 *     失败原因可以根据result的值，对应MR_OPENSKY_RETCODE查询
 *     此时插件无法工作，应用应当自行处理此结果以返回应用逻辑。
 * 当 result == MR_OPENSKY_SUCCESS 表示插件已经在正常工作，
 *     应用需要暂停自己的界面刷新，以等待其他回调函数通知。
******************************************************************************************/
typedef void (*mrc_opensky_entry_init_cb)(int32 result);

/******************************************************************************************
 * 退出回调
 * 当 result != MR_OPENSKY_SUCCESS && result != MRC_OPENSKY_ENTRY_START_TYPE_PARAM 时表示插件异常退出，
 *     异常原因可以根据result对应的MR_OPENSKY_RETCODE值查询
 * 当 result == MR_OPENSKY_SUCCESS 时表示插件正常退出
 * 当 result == MRC_OPENSKY_ENTRY_START_TYPE_PARAM 时表示插件将返回给应用一个启动参数
 *     此启动参数可以由列表编辑器填写,具体含义由应用决定,应用如果需要保存此内容,请自行memcpy
 * 这个回调函数用以通知OpenSKY插件已经退出，应用可以继续执行本身的逻辑。
******************************************************************************************/
typedef void (*mrc_opensky_entry_exit_cb)(int32 result, const char* data);

/******************************************************************************************
 * 初始化并进入OSE
 * 本接口将初始化OpenSKY入口插件，如果插件不存在则显示下载界面。
 * 应用在调用此接口前应当暂停界面刷新，直到回调函数返回才能继续应用逻辑。
 * 参数输入： 
 *     secretStr：Secret校验码，向斯凯申请得到
 *     initCB：初始化回调
 *     exitCB：退出回调
 * 返回：
 *     无须关心
******************************************************************************************/
int32 mrc_opensky_entry_initEx(int8 isSupprotIM, char* secretStr, mrc_opensky_entry_init_cb initCB, mrc_opensky_entry_exit_cb exitCB);

#define mrc_opensky_entry_init(str,initCB,exitCB) mrc_opensky_entry_initEx(1,(char*)(str),(mrc_opensky_entry_init_cb)(initCB),(mrc_opensky_entry_exit_cb)(exitCB))

/******************************************************************************************
 * 加载URL回调
 * 当 result != MR_OPENSKY_SUCCESS 时表示插件加载失败，此时插件无法工作，应用应当自行处理此结果以返回应用逻辑。
 * 当 result == MR_OPENSKY_SUCCESS 时表示插件已经正常退出浏览器，应用可以继续执行本身的逻辑。
******************************************************************************************/
typedef void (*mrc_opensky_entry_loadURL_cb)(int32 result);

/******************************************************************************************
 * 打开OSE浏览器并加载指定的URL
 * 本接口将调用OpenSKY入口插件的内置浏览器来打开一个URL网页，如果插件不存在则显示下载界面。
 * 应用在调用此接口前应当暂停界面刷新，直到回调函数返回才能继续应用逻辑。
 * 使用本接口前请勿调用 mrc_opensky_entry_init
 * 参数输入：
 *     secretStr：Secret校验码，向斯凯申请得到
 *     url：网页的URL
 *     cb：加载URL回调
 * 返回：
 *     无须关心
 ******************************************************************************************/
int32 mrc_opensky_entry_loadURLEx(int8 isSupprotIM, char* secretStr, const char* url, mrc_opensky_entry_loadURL_cb cb);

#define mrc_opensky_entry_loadURL(str,url,cb) mrc_opensky_entry_loadURLEx(1,(char*)(str),(const char*)(url),(mrc_opensky_entry_loadURL_cb)(cb))

/******************************************************************************************
 * 查询IM（迷你版）是否自动启动
 * 返回：
 *     0 - 非自动启动
 *     1 - 自动启动
 * 如果IM被设置为自动启动，应用需要自行调用IM的相关接口来获取IM的状态并作出相关操作，
 * 本接口仅仅是用于查询在OSE插件运行时，用户是否启用了IM
******************************************************************************************/
int32 mrc_opensky_isIMStarted(void);


#ifdef __cplusplus
}
#endif

#endif // __MRC_OPENSKY_COMMON__
