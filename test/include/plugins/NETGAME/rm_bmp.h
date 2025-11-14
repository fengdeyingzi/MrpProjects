
#ifndef _RM_BMP_H
#define _RM_BMP_H

#include "mrc_base.h"
//#include "ext.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	/*FOR FREE ADJ*/
#define RES_INTERNAL_PRIO 255//don't change to other value
#define RES_MEMSTREAM_PRIO 254//?OK?
	
#define RES_MRP_FILENAME_MAXLEN (32)//(MPS_FP_FILENAME_LEN)
#define RES_RES_FILENAME_MAXLEN (12)
	
#define RES_BMPID_BASE	0
#define RES_BMPID_SYS_BASE	(RES_BMPID_BASE+0)
#define RES_BMPID_SYS_MAX 500
#define RES_BMPID_APPS_BASE	(RES_BMPID_BASE+1000)
	
	typedef struct
	{// do not change follow numbers order, must corresponding to struct - resIdSt
		uint16 *memPtr;//bmp内存地址指针
		uint16 beginId;//起始bmp id
		uint16 endId;//结束bmp id
		uint16 w;//bmp单元宽
		uint16 h;//bmp单元高
		
		uint8 *auxPtr;//bmp内存释放地址指针,若赋成NULL，则释放时以*memPtr为准
	}resBmpSt;
	/*
	约束:
	1/不允许一张bmp中的一组id从中间被替换(取中间一个id单个注册)，必须整体替换。否则视为无效
	2/替换规则:1、beginId必须一致；只能是大id范围替换掉小的。
	3/每次使用bmp时都必须使用SGL_resLoadBmp(id)重新获取其内存ptr，因可能先前的bmp已经被释放或替换
	*/
	
	
	
	int32 mrc_Bmp_Init(void);
	
	/*
	注册bmp group(打包在同一mrp文件中)
	输入:	
	const char *mrpName: bmp所属mrp名称
	输出:
	返回:
	>0  -group handle
	其他	-操作失败
	*/
	int32 mrc_Bmp_RegGroup(const char *mrpName);
	
	
	/*
	向bmp管理池中注册bmp
	输入:	
	resBmpSt *regAtt: bmp相关属性,详见resBmpSt
	const char *mrpName: bmp所属mrp名称
	int32 group: RM_resRegBmpGroup() 返回的bmp group值
	uint8 prio :       bmp的加载优先级
	输出:
	返回:
	MR_SUCCESS		操作成功
	MR_FAILED		操作失败
	*/
	int32 mrc_Bmp_RegId(resBmpSt *regAtt, const char *bmpName, int32 group, uint8 prio);
	
	
	/*
	获取bmp 内存指针
	输入:	
	uint16 bmpID: bmp id
	uint8 prio: 优先级，主要用来区分app
	输出:
	int32* width:bmp width
	int32* height:bmp height
	int32* index:bmp管理池中的index索引，可用于RM_resGetBmpPtr()中
	可以默认填NULL，则不返回index
	返回:
	NULL	操作失败
	其他	bmp内存指针
	*/
	uint16* mrc_Bmp_Load(uint16 bmpID, int32* width, int32* height, int32* index, uint8 prio);
	
	/*
	警告!:使用本函数前必须调用mrc_exRamInitEx初始化扩展内存
	同rm_resLoadBmp, 这个函数不同点是先从扩展内存分配内存空间, 分配不到时才从主内存分配
	输入:	
	uint16 bmpID: bmp id
	uint8 prio: 优先级，主要用来区分app
	输出:
	int32* width:bmp width
	int32* height:bmp height
	int32* index:bmp管理池中的index索引，可用于RM_resGetBmpPtr()中
	可以默认填NULL，则不返回index
	返回:
	NULL	操作失败
	其他	bmp内存指针
	*/
	uint16* mrc_Bmp_LoadExmem(uint16 bmpID, int32* width, int32* height, int32* index, uint8 prio);
	
	
	/*
	辅助函数,根据RM_resLoadBmp()返回的index获取bmp的内存指针,可提高些获取速度。
	输入:	
	uint16 bmpID: bmp id
	uint16 index: bmp 在bmp管理池中的index索引
	输出:
	返回:
	NULL	操作失败
	其他	bmp内存指针
	*/
	uint16* mrc_Bmp_GetPtr(uint16 bmpID, uint16 index);
	
	/*
	释放bmp内存
	输入:	
	uint16 bmpID: bmp id
	uint8 unReg: 是否同时卸载bmpID在bmp管理池中注册信息
	(请谨慎使用,应用模块只允许释放自身reg的bmp)
	输出:
	返回:
	*/
	void mrc_Bmp_Free(uint16 bmpID, uint8 unReg);
	
	
	/*
	释放指定优先级的bmp内存
	输入:	
	uint8 prio: 
	输出:
	返回:
	*/
	void mrc_Bmp_FreeByPrio(uint8 prio);
	
	/*
	释放指定group的bmp内存
	输入:	
	int32 groupId
	输出:
	返回:
	*/
	void mrc_Bmp_FreeByGroup(int32 groupId);
	
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _RM_BMP_H */

