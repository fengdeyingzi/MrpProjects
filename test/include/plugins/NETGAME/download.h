#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 加载download绘图模块 */
int32 mrc_download_load(void);

/* 卸载download绘图模块 */
int32 mrc_download_unload(void);

int32 mrc_dl_init_network(void);
//download status or result, report to user
typedef enum
{
	/*for update phase*/
	DLOAD_GETVERSION_FAILED = 0,//获取文件版本信息失败，busy, or send appid failed, or recv timeout, or recv proc failed.
	DLOAD_GETVERSION_SUCCESS,//获取文件版本信息成功。
	DLOAD_GETVERSION_NONE,//当前已是最新版本。
	/*for fileload phase*/
	DLOAD_ALOAD_START,//某一文件开始下载, +fileID
	DLOAD_DATA_COMING,//某一文件有数据到来,+fileID
	DLOAD_ALOAD_END,//某一文件下载完毕,+fileID
	DLOAD_ALLLOAD_END,//全部文件下载完毕
	DLOAD_LOAD_FAILED,//某一文件下载失败,+fileID +suberror
	
	DLOAD_RET_MAX
}DloadStatusEn;

//sub error code for DLOAD_LOAD_FAILED, reposrt to user
typedef enum
{
	DLOAD_NO_ERROR = 0,
	DLOAD_FAILED_FILE_ERROR = 1,//文件内容校验失败
	DLOAD_FAILED_FFS_ERROR = 2,//文件操作失败
	DLOAD_FAILED_NET_ERROR = 3,//网络超时等错误
	DLOAD_FAILED_SERVER_ERROR = 4,//服务错误

    DLOAD_FAILED_CHK2REP_REMMRP_ERROR = 5,
    DLOAD_FAILED_CHK2REP_RENAME_ERROR = 6,
    DLOAD_FAILED_CHK2REP_REMCFG_ERROR = 7,
    DLOAD_FAILED_CHK2REP_WRONG_STATE = 8,

	DLOAD_FAILED_FILE_NOT_FOUND = 9,//文件未找到
	
	DLOAD_FAILED_MAX
}DloadSubErrEn;

//result code for: c <-> s
enum
{
	DL_RESULT_PARAM_INVALID = -3, //参数错误
	DL_RESULT_TIMEOUT = -2, //操作超时
	DL_RESULT_FAILED = -1, //操作失败
	DL_RESULT_SUCCESS = 0,	//成功
	DL_RESULT_NO_NEWVERS = 1 //服务器无新版本或无对应文件，(及任何待下载的其他plugs ?)
};

#define DLOAD_FILES_MAX		64//下载队列中可以存在待下载文件的最大个数
#define DLOAD_FULLPATH_MAX_LEN 64//路径+文件名的最大允许长度

typedef int32  DloadResult;
//typedef int32 (*DloadResult)(DloadStatusEn status, uint32 fileId, uint32 param, int32 p3, int32 p4, int32 p5);

typedef struct
{
	uint32 fileId; 			//网络为每个部署文件分配唯一的ID号。
	uint32 fileLen; 			//文件总长度 
	uint32 curLen; 			//当前已下载长度 
	uint8 filenameLen;		//文件名长度
	char *filename;		//gb,
}DloadFileAttSt;

typedef struct
{
	uint32 num;							//文件数目
	uint32 fileIds[DLOAD_FILES_MAX]; 		//网络为每个部署文件分配唯一的ID号。
}DloadFileids;

typedef void (*DL_CB)(int32 result, int32 file, int32 param, int32 FileNum, int32 p4, int32 p5);


/*
注册状态处理函数
输入:	
		int32 extHandle:应用模块的ext句柄
		DloadResult resultCb: 状态处理回调
输出:
返回:
		MR_FAILED				操作失败
		MR_SUCCESS		 		注册成功
*/
int32 mrc_DloadRegResultCb(int32 extId, DL_CB dl_callback);

/*
注销状态处理函数,本函数不做任何操作
输入:	
		handle		mrc_DloadUnRegResultCb()得到的句柄
输出:
返回:
		MR_SUCCESS		操作成功
*/
int32 mrc_DloadUnRegResultCb(int32 handle);



/*
发起开始文件数据下载的请求
输入:	
		int32 flag: 是否是后台下载,1 是，0否。
输出:
返回:
		MR_SUCCESS		操作成功
		其他				操作失败
*/
int32 mrc_DloadStartDownloadReq(int32 flag);


/*
获取某一文件的下载状态
输入:	
		uint32 fileId: 网络为每个部署文件分配唯一的ID号。
输出:
返回:
		NULL		操作失败
		其他		操作成功		
*/
const DloadFileAttSt* mrc_DloadGetStatus(uint32 fileId);

/*
获取当前下载队列中所有的fileid
输入:	
		DloadFileids* fileids:  
输出:
		DloadFileids* fileids: 当前所有待下载的fileid
返回:
		MR_SUCCESS		操作成功
		其他				操作失败
*/
int32 mrc_DloadGetFileids(DloadFileids* fileids);

/*
检查是否有临时下载的文件需要替换
输入:	
输出:
返回:
		MR_SUCCESS		操作成功
		其他				操作失败
*/
int32 mrc_DloadTdCheck2Replace(void);


/*
设置文件保存目录
输入:	
输出:
返回:
MR_SUCCESS		操作成功
其他				操作失败
注意：输入路径必须如"gwy/"所示，后面带斜线
*/
int32 mrc_DloadSetSaveFilePath(char * path);

/*
设置下载文件类型
输入:	
输出:
返回:
*/
void mrc_DloadSetDownloadFileType(int isMrpFile);

/*
取消文件下载
输入:	
输出:
返回:
*/
void mrc_DloadCancel(void);

/*
下载文件列表
输入:	
输出:
返回:
MR_SUCCESS		操作成功
其他				操作失败
*/
int32 mrc_DloadFileListInd(char *data, int32 len);


/*
获取文件信息
输入:	uint16	modInstId 下载模块的module Instance id 
		char ** fileNames 文件名列表
	    int		count	  文件数
example:
char * fileNames[] = {"dwol.gif", "xxx.gif", "yyy.gif"};
		
DloadSetSaveFilePath("test/"); // 先设置下载目录 
//注册下载回调，downloadCBShell是EXT_ID_ROOMLIST模块的函数
DloadRegResultCb(EXT_ID_ROOMLIST, downloadCBShell); 
DloadGetFileInfoReq(0xBB01, fileNames, 3);  // 下载这3个文件


输出:
返回:
MR_SUCCESS			操作成功
其他				操作失败
*/
int32 mrc_DloadGetFileInfoReq(uint16 modInstId, char * fileNames, int count);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif