#ifndef MRC_PLATREQ_H
#define MRC_PLATREQ_H


#define MRC_PLATFROM_REQUEST_TYPE_SCORE   0x1   // 连接到游戏成绩排行页面。
#define MRC_PLATFROM_REQUEST_TYPE_REMARK  0x2   // 跳转到游戏评论的页面。
#define MRC_PLATFROM_REQUEST_TYPE_CPZONE  0x3   // 跳转到CP专区 （即将支持）
#define MRC_PLATFROM_REQUEST_TYPE_GUIDES  0x4	// 跳转到游戏攻略


/*
 * 在需要异步过程的情况下通知应用结果。
 */
typedef void (*mrc_platreq_cb_t)(int32 result/*ErrorCode*/, int32 var1, int32 var2, int32 var3, int32 var4, int32 var5);

/*
 *  函数： mrc_platformRequest
 *
 * Parameters :
 *        reqType : 请求的类型，目前可以支持  成绩排行，游戏评论，cp专区（即将支持）。
 *     
 *        resultCB: 在异步的情况下通知结果的。
 *
 *        hintStr:       (const char*),  如果需要从网络下载功能插件，显示给用户的提示语。 可以为NULL。  utf16be encoded.
 *
 *        MRC_PLATFROM_REQUEST_TYPE_SCORE:
 *                 var1 : int32, 游戏的积分。
 * 
 * Returns : 
 *       MR_SUCCESS:  成功， 应用不需要做什么。 
 *                    将通过resultCB来通知应用实际的结果。目前可能的result有：
 *
 *      ********************************************************************************************************
 *              NOTE :  如果返回值为MR_SUCCESS, 那么应用需要停止自己的刷屏。否则可能和会和下载插件冲突。
 *      ********************************************************************************************************                               
 *                        
 *      
 *       ELSE:   失败， 应用需要做一个UI提示用户。 提示内容可以为： “调用平台功能出错。错误代码：%x”
 *       
 * 异步回调结果
 *    result: 
 *		MR_SUCCESS:浏览器成功运行后退出
 *		MR_FAILED:浏览器未能成功运行
 *
 */

extern int32 mrc_platformRequest(int32 reqType, mrc_platreq_cb_t resultCB, const char* hintStr, int32 var1, int32 var2, int32 var3);


/* 函数mrc_startBrowser
 * Parameters:
 *    url1:首选加载的url，不能为NULL
 *    url2:在url1加载失败时加载url2，可以为NULL
 *    resultCB:在异步的情况下通知结果
 *    hintStr:       (const char*),  如果需要从网络下载功能插件，显示给用户的提示语。 可以为NULL。  utf16be encoded.
 *
 * Return:
 *       MR_SUCCESS:  成功， 应用不需要做什么。 
 *                    将通过resultCB来通知应用实际的结果。
 *       ELSE:   失败， 应用需要做一个UI提示用户。
 *					提示内容可以为： “浏览器启动失败。错误代码：%x”
 *       
 *
 * 异步回调结果
 *    result: 
 *		MR_SUCCESS:浏览器成功运行后退出
 *		MR_FAILED:浏览器未能成功运行
 */
extern int32 mrc_startBrowser(const char *url1, const char *url2, mrc_platreq_cb_t resultCB, const char* hintStr);


/*设置浏览器的工作路径
*  浏览器默认工作路径是启动它的app的路径
*  Parameters:
*  	pszwkdir:工作路径
*  返回值:
*  result: 
*	   MR_SUCCESS:设置成功
*	   MR_FAILED:设置失败
*/
extern int32 mrc_setwkDir(char * pszwkdir);

#endif
