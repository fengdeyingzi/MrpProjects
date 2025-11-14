#ifndef _MRC_RECOMMEND_H_
#define _MRC_RECOMMEND_H_

#ifdef __cplusplus
extern "C" {
#endif

enum{
	RMD_USER_QUIT,
	RMD_DLOAD_FAILED,
	RMD_LOAD_FAILED	
};


/*交叉营销插件回调
code:返回状态:
	
	RMD_USER_QUIT:		用户退出插件返回游戏
	RMD_DLOAD_FAILED:	下载插件失败(如果游戏包里没有打入插件，会尝试下载)
	RMD_LOAD_FAILED:	加载插件失败	
	p1,p2,p3,p4,p5:没有用到，保留
*/

typedef void (*recommendGame_cb)(int32 code,int32 p1,int32 p2,int32 p3,int32 p4,int32 p5);

/*启动交叉营销插件
函数输入：
	cb:	回调函数
函数输出：	
*/

void mrc_recommendGame(recommendGame_cb cb);


#ifdef __cplusplus
}
#endif
#endif