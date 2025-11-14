/*******************************************************************************
 * CopyRight (c) 2005-2010 SKY-MOBI Ltd. All rights reserved.
 * Filename:  dpnouilib.h
 * Creator:   lynn.mo
 * Version:   V1001
 * Date:      2011/04/11
 * Description:			
*******************************************************************************/
#ifndef __DPNOUILIB_H__
#define __DPNOUILIB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct  
{
	char*   p_merchantId;			//商户号			
	char*	p_merchantPasswd;		//商户号密码
	uint16	instanceID;				//cp的服务端模块号
	char*	p_orderID;				//订单号
	char*	p_productId;			//商品号 
	char*	p_productName;			//商品名称 
	/*
	以上信息由cp提供
	p_merchantId					cp的应用上线前由斯凯分配
	p_merchantPasswd				cp的应用上线前由斯凯分配
	p_orderID						由cp生成(字符串内容只能为数字)
	p_productId						由cp定义(字符串内容只能为数字) 
	p_productName					由cp定义(utf16-be编码)
	*/

	char*	p_cardNum;				//卡号
	char*	p_cardPasswd;			//密码
	uint16	cardType;				/*
									充值类别
									10		代表移动神州行充值卡
									15		代表联通充值卡
									21		代表电信充值卡
									11		代表盛大游戏充值卡
									12		代表征途游戏充值卡
									13		代表骏网游戏充值卡
									14		代表QQ游戏充值卡
									34		代表冒泡卡
									*/
	uint16	amount;					//充值金额,整型数据，以分为单位
	//以上信息需要用户填写

}dpnouiChargeInfo_T;

/*
*卡密支付接口
*使用说明:	
输入：
		pDpnouiChargeInfo	: 指向dpnouiChargeInfo_T结构体的指针，它的创建和销毁均由cp完成
		p1~p5				:保留字段，暂时无用，可填入任意值

返回值：
		MR_FAILED 失败
		MR_SUCCESS 成功
使用示例：
#include "dpnouiLib.h"
void  testFunc(void)
{
	int32 ret = MR_FAILED;
	dpnouiChargeInfo_T *pdpnouiChargeInfo = NULL;

	pdpnouiChargeInfo = (dpnouiChargeInfo_T*)malloc(sizeof(dpnouiChargeInfo_T));
	pdpnouiChargeInfo->p_merchantId ="10180" ;
	pdpnouiChargeInfo->p_merchantPasswd ="kodfdsafdasfang123";
	pdpnouiChargeInfo->instanceID = 0x64d0;
	pdpnouiChargeInfo->p_orderID = "123";
	pdpnouiChargeInfo->p_productId = "1";			
	pdpnouiChargeInfo->p_productName = "ddd";
	pdpnouiChargeInfo->cardType = 10;
	pdpnouiChargeInfo->p_cardNum = "12123214213412";
	pdpnouiChargeInfo->p_cardPasswd = "23421342134";
	pdpnouiChargeInfo->amount = 20;
	
	ret = mrc_dpnoui_sendChargeInfo(pdpnouiChargeInfo, 0, 0, 0, 0, 0);
	free(pdpnouiChargeInfo);
	pdpnouiChargeInfo = 0;
			
	return;
}			
*/
int32 mrc_dpnoui_sendChargeInfo(int32 pDpnouiChargeInfo, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DPNOUILIB_H__ */

