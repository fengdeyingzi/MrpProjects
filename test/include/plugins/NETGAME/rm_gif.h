
#ifndef _RM_GIF_H
#define _RM_GIF_H

#include "mrc_base.h"
#include "verdload.h"
//#include "ext.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	
#ifndef GIF_FILE_NAME_LEN
#define GIF_FILE_NAME_LEN	64
#endif
	
//GIF 图片的存放目录
#define GIFFILE_DIR		"gwy/gifs/"

	typedef struct _gif_file_desc
	{
		char filename[GIF_FILE_NAME_LEN];
		uint16 version;
		uint32  fileLen;    
		uint32  fileId;
	}gif_file_desc_t;


	/*初始化*/
	int32 mrc_Gif_Init(void);
	
	/*
	加载图片，类似与SGL_resLoadBmp，会缓存图片
	输入:	
	int32 group:调用的模块ID
	char filename: gif图片文件名
	unit8 bNeedDownload:如果图片不存在，是否去服务器下载
	输出:
	uint16* width:bmp width
	uint16* height:bmp height
	返回:
	NULL	操作失败
	其他	bmp内存指针，图片会被缓存，如果释放需要调用rm_gifFreeByFileName或rm_gifFreeByGroup
	*/
	uint16* mrc_Gif_Load(int32 group, char *filename, uint16*width, uint16*height, uint8 bNeedDownload);
	
	/*
	根据图片名释放图片
	输入:	
	char filename: gif图片文件名
	输出:
	返回:
	*/
	int32 mrc_Gif_FreeByFileName(char * filename);
	
	/*
	根据模块ID释放图片
	输入:	
	int32 group: 模块ID
	输出:
	返回:
	*/
	int32 mrc_Gif_FreeByGroup(int32 group);
	
	
	/*
	显示gif图片,不对图片进行缓存。
	输入:
	char * filename: gif图片名
	int16 x:	起始位置x
	int16 y:    起始位置y
	unit8 bNeedDownload:如果图片不存在，是否去服务器下载
	输出:
	MR_SUCCESS 显示成功
	MR_FAILED  失败，会根据needDownLoadFlag去下载文件。
	返回:
	*/
	int32 mrc_Gif_Show(char * filename, int16 x, int16 y, uint16 *width, uint16 *height, uint8 bNeedDownload);
	
	/* 
		gif_file_desc_t* file: gif图片的信息
		MR_DL_CB DL_CB:下载回调函数
		ui:          ui的取值有三种可能：
		0                       下载进度界面由下载插件负责,下载前不让用户确认。
		MR_DL_FLAG_UIHINT      下载进度界面由下载插件负责，下载前让用户确认。
		MR_DL_FLAG_UIBYAPP   下载界面由APP负责, 下载插件不负责任何的UI。

		  title:       下载界面标题,ui & VERDLOAD_FLAG_UIBYAPP == 0 时需要提供
		  hint:        下载确认界面的提示,ui =VERDLOAD_FLAG_HINT 时需要提供
	*/
	int32 mrc_Gif_TryDownLoadFile(gif_file_desc_t* file,MR_DL_CB DL_CB,int32 ui,uint16 *title,uint16 *hint);
	int32 mrc_Gif_CancelDownLoad(void);
	/*
		释放bmpPool中所有的图片.
	*/
	int32 mrc_Gif_FreeAll(void);
	
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _RM_GIF_H */

