#include "mrc_base.h"
#include "mrc_exb.h"
#include "mrc_win.h"
#include "FileView.h"
/*
typedef struct 
{
	int32 dft_list;
	char list_path[260];  //gb绝对路径
}MR_T_FMGR_REQ;*/

extern int32 mrc_platEx(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB* cb);

//初始化文件浏览，返回：API结果
int32 FV_Init(void)
{
	return mrc_platEx(1401, NULL, 0, NULL, NULL, NULL);
}

//设置显示类型，参数：类型代号，是否显示；返回：API结果
int32 FV_SetType(uint8 vType, uint8 IsView)
{
	if (1 == IsView)
	{
		return mrc_platEx(1402, &vType, 0, NULL, NULL, NULL);
	}
	else
	{
		return mrc_platEx(1403, &vType, 0, NULL, NULL, NULL);
	}
}

//开始浏览，参数：初始路径，系统路径代号（优先）；返回：API结果
int32 FV_BeginView(const char *Path, int32 SysPath)
{
	MR_T_FMGR_REQ vPath;

	vPath.dft_list = SysPath;
	mrc_memset(vPath.list_path, 0, sizeof(vPath.list_path));
	mrc_memcpy(vPath.list_path, Path, mrc_strlen(Path));

	return mrc_platEx(1404, (uint8*)&vPath, sizeof(vPath), NULL, NULL, NULL);
}

//取得选定的文件路径，参数：返回的字符串；返回：API结果
int32 FV_GetPath(char *Path, int32 pathLen)
{
	int32 rValue, sLen;
	uint8 *sStr=NULL;

	rValue = mrc_platEx(1405, NULL, 0, &sStr, &sLen, NULL);

	if (MR_SUCCESS == rValue)
	{
		if (pathLen > sLen)
		{
			mrc_memset(Path, 0, pathLen);
			mrc_memcpy(Path, sStr, sLen);
		}
		mrc_free(sStr);
	}

	return rValue;
}

//结束浏览，返回：API结果
int32 FV_EndView(void)
{
	return mrc_platEx(1406, NULL, 0, NULL, NULL, NULL);
}

/*------------------------------------------------------------------------------------------*/
