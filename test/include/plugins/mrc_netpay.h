#ifndef __MRC_NETPAY_H__
#define __MRC_NETPAY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
	SMS_APP_PRE_NETWORK_NONE = 0,		/*无预先网络配置*/
	SMS_APP_PRE_NETWORK_CMWAP = 1,		/*预先网络CMWAP */
	SMS_APP_PRE_NETWORK_CMNET = 3		/*预先网络CMNET */
}SMS_APP_PRE_NETWORK;

typedef enum pay_type
{
	PAY_MSG_TYPE_REGISTER = 0,
	PAY_MSG_TYPE_MAGIC = 1,
	PAY_MSG_TYPE_REREG = 5
}SMS_PAY_TYPE;

typedef enum pay_result
{
	PAY_SUCCESS = 0,
	PAY_FAILED = -1,
	PAY_REREG = -2,
	PAY_ERROR = -101
}SMS_PAY_RESULT;

/*
付费模块初始化函数，启动时调用一次
如果返回MR_FAILED，则表示目前无法付费
返回值：
MR_SUCCESS：0 初始化成功
MR_FAILED：-1 初始化失败
*/
int32 mrc_netPayInit(void);

/*
int32 result: 
MR_SUCCESS：0	付费成功
MR_FAILED：	-1	付费失败
MR_NOTFULL：-2	付费部分成功
MR_BADERRPR：-101	严重错误，在回调中不能再进行付费相关的操作
*/
#define MR_NOTFULL	-2
#define MR_BADERRPR	-101
typedef int32 (*mrc_netPayCB)(int32 result, int32 retlen, char *retdata);

/*
tag			字段名			字段类型		说明 
10100		priceTotal		int32			付费总金额，单位分 
10101		pricePaid		int32			已付费金额，单位分 
10102		priceServ		int32			服务器支持金额，单位分 
												举例说明：priceTotal=500，而目前只有2元通道，则priceServ=400，发送两条短信即算成功
10200		payType			int32			付费类型
											  0 = 注册
											  1 = 道具
											  3 = 积分
											  4 = 充值
											  5 = 补注册

											  10 = 包月（BAOYUE）
											  11 = 包次（BAOCI）
											  12 = 点播（DIANBO）
										说明：0 = 注册，有短信发送信息保存，支持补发，如果存在上次未发送完短信，则接着发送完毕
										      1 = 道具，没有短信发送信息保存，每次都是从新获取道通进行付费
											  5 = 补注册，使用保存的短信通道信息付费
10201		payPoint		int32			计费点编号(商品编号)
10202		payDesc			char			计费点说明(商品名称)
											  a-z,0-9
											  ASCII编码，不要超过10个字节 
10203		hintTitle		char			计费提示页标题 (unicode编码)
10204		hintText		char			计费提示页内容 (unicode编码)
10205		userdata		bin				计费文件用户自定义信息
10206		paytime			char			付费时间，格式为YYYYMMDDHHMiSS，
											  ASCII编码，例如：
											  20100102193015
10207		netInit			int32			网络连接类型
											  0 = 没有连接
											  1 = 已经连接(CMWAP)
											  2 = 已经连接(CMNET，应用需要断GPRS)
											  3 = 已经连接(CMNET，应用不需要断GPRS)
										说明：1、应用本身没有连接网络，					netInit=0
											  2、应用已经连接网路且拨号方式为CMWAP		netInit=1
											  3、应用已经连接网路且拨号方式为CMNET		netInit=2
											     (此种情况，应用付费前需断开网络，付费回调后，再恢复网络)
											  4、应用已经连接网路且拨号方式为CMNET		netInit=3
											     (此种情况，应用付费前不需要断开网络)
10208		appId			int32			计费的应用ID
10209		appName			char			计费的应用短名称
10210		appVer			int32			计费的应用版本号
10211		transcationId	char			订单号
10212		skyid			char			用户的skyid
10213		productnum		int32			计费点对应的数量(商品数量) >=0
10214		productdate		char			计费点对应的商品的过期时间，格式为YYYYMMDD
10215		businessId		int32			业务编号：
											  100 = 阅读
											  200 = 音乐
											  300 = 单机游戏
											  400 = 网游
											  500 = 棋牌
											  600 = SNS
10216		businesName		char			业务名称，ASCII编码
10217		portalId		int32			入口编号，只有WAP付费时填写(值为100)
10218		portalName		char			入口名称，ASCII编码，只有WAP付费时填写(值为WAP)
10219		priceUint		int32			付费单位：
											  10 人民币
											  11 美元
											  12 KB
10220		channelId		int32			渠道编号
											  100 = OTA
											  200 = T卡预置
											  300 = 固化
10221		channelName		char			渠道名称
10300		cbdata			bin				回调函数用户自定义信息 
10400		createOldSid	int32			使用新应用接口是否生成游戏SID文件
											0	不生成
											1	生成
10401		deleteOldSid	int32			删除应用付费信息时，是否删除游戏SID文件
											0	不删除
											1	删除
*/

#define TAG_PRICETOTAL		10100
#define TAG_PRICEPAID		10101
#define TAG_PRICESERV		10102

#define TAG_PAYTYPE			10200
#define TAG_PAYPOINT		10201
#define TAG_PAYDESC			10202
#define TAG_PAYHINT_TITLE	10203
#define TAG_PAYHINT_TEXT	10204
#define TAG_PAYUSERDATA		10205
#define TAG_PAYTIME			10206
#define TAG_PAYNETINIT		10207
#define TAG_APPID			10208
#define TAG_APPNAME			10209
#define TAG_APPVER			10210

#define TAG_CBDATA			10300
#define TAG_CREATEOLDSID	10400
#define TAG_DELETEOLDSID	10401

/*
发起付费，参数中需要包含的tag:
10100,10200,10201,10202,10203,10204,10205,10206,10207,10300
回调中包含的tag:
10100,10101,10102,10206,10300
*/
int32 mrc_netPayUI(int32 varlen, char *vardata, mrc_netPayCB cb);

/*
检查付费，参数中需要包含的tag:
10201,10300
回调中包含的tag:
10100,10101,10102,10205,10206,10300
*/
int32 mrc_netCheckCharge(int32 varlen, char *vardata, mrc_netPayCB cb);

/*
删除付费，参数中需要包含的tag:
10201
回调中包含的tag:

说明：如果希望重新注册，而又不希望是补发，可以先调用此接口，删除短信通道文件

cb中result值
MR_SUCCESS：0	删除文件成功
MR_FAILED：	-1	删除文件失败
MR_NOTFULL：-2	文件不存在
*/
int32 mrc_netRemoveSMSFile(int32 varlen, char *vardata, mrc_netPayCB cb);

/*
应用在付费前设置已经使用GPRS拨号方式，类型为SMS_APP_PRE_NETWORK
调用此函数设置网络后，付费时，不能断开原来的GPRS，且回调后不需要再重新GPRS

注：
networkType	方式和mrc_netPayUI中的的10207		netInit要对应
*/
void mrc_netSetAppNetworkType(SMS_APP_PRE_NETWORK networkType);

/*
单机应用获取注册时已付费的金额
param	varlen 保留，暂时没用
	vardata 保留，暂时没用
return 	payPrice已付费的金额，单位：分
	MR_FAILED 获取付费金额失败
	!MR_FAILED 获取到的付费金额
*/
int32 mrc_appGameGetPayPrice(int32 varlen, char *vardata);

/*
单机应用删除注册时的保存的付费信息
param	varlen 保留，暂时没用
	vardata 保留，暂时没用
return 	是否删除成功
	MR_FAILED 失败
	MR_SUCCESS 成功
*/
int32 mrc_appGameRemovePayFile(int32 varlen, char *vardata);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MRC_NETPAY_H__ */