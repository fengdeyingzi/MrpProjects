#include "mrc_base.h"

#include "FileRW.h"
#include "List.h"
#include "Funcs.h"

/*---------------------------------------------------------------------------------------------*/
//从Mrp读取rc文件，参数：Mrp文件名
int32 ReadMrp(const char *MrpFile)
{
	int32 RcSize=0, rValue=0, fHwnd=0;
	uint8 *RcData=NULL;
    const char *tempFile = "temp.rc";
WLog("从mrp里打开文件",0); 
	rValue = mrc_readFileFromMrpEx((char*)MrpFile, "res_lang0.rc", &RcData, &RcSize, 0);
	if (MR_SUCCESS == rValue)
	{
		WLog ("判断mrp文件成功",0); 
        mrc_remove(tempFile);
        fHwnd = mrc_open(tempFile, MR_FILE_RDWR | MR_FILE_CREATE);
        mrc_seek(fHwnd, 0, MR_SEEK_SET);
        mrc_write(fHwnd, RcData, RcSize);
        mrc_close(fHwnd);
        mrc_free(RcData);
		ReadRc(tempFile);
        mrc_remove(tempFile);

		return MR_SUCCESS;
	}
	else
	{
		return MR_FAILED;
	}
}

//保存rc到Mrp文件，参数：Mrp文件名
int32 SaveMrp(const char *MrpFile)
{
	int32 fHwnd;
	int32 MrpSize, NameLen, NewOffset;
	uint32 HeadSize,NewRcSize;
	char *HeadData, *RcData, *pPos, *RcName, *tempFile;

	RcName = "res_lang0.rc";
	NameLen = 13;

	//保存res_lang0.rc并重新读取
	tempFile = "temp.rc";
	NewRcSize = SaveRc(tempFile);
	RcData = (char*)mrc_readAll(tempFile, &NewRcSize);
	mrc_remove(tempFile);
	if (NULL == RcData) return 0;

	fHwnd = mrc_open(MrpFile, MR_FILE_RDWR);
	if (0 == fHwnd) return 0;

	//取得Mrp总大小
	mrc_seek(fHwnd, 8, MR_SEEK_SET);
	mrc_read(fHwnd, &MrpSize, 4);

	//取得信息和列表大小
	mrc_seek(fHwnd, 4, MR_SEEK_SET);
	mrc_read(fHwnd, &HeadSize, 4);
	HeadSize += 8;

	//取得信息和列表数据
	HeadData = (char*)mrc_malloc(HeadSize);
	mrc_seek(fHwnd, 0, MR_SEEK_SET);
	mrc_read(fHwnd, HeadData, HeadSize);

	//查找res_lang0.rc
	pPos = FindPos(HeadData, RcName, HeadSize, NameLen);
	if (pPos) 
	{
		//修改新res_lang0.rc偏移
		NewOffset = MrpSize+NameLen + 8;
		mrc_memcpy(pPos+13, &NewOffset, 4);

		//修改新res_lang0.rc大小
		mrc_memcpy(pPos+17, &NewRcSize, 4);

		//修改Mrp总大小
		MrpSize = MrpSize + NewRcSize + NameLen + 8;
		mrc_memcpy(HeadData+8, &MrpSize, 4);

		//写回Mrp文件
		mrc_seek(fHwnd, 0, MR_SEEK_SET);
		mrc_write(fHwnd, HeadData, HeadSize);

		//写入res_lang0.rc信息到末尾
		mrc_seek(fHwnd, 0, MR_SEEK_END);
		mrc_write(fHwnd, &NameLen, 4);
		mrc_seek(fHwnd, 0, MR_SEEK_END);
		mrc_write(fHwnd, RcName, NameLen);
		mrc_seek(fHwnd, 0, MR_SEEK_END);
		mrc_write(fHwnd, &NewRcSize, 4);

		//写入res_lang0.rc数据到末尾
		mrc_seek(fHwnd, 0, MR_SEEK_END);
		mrc_write(fHwnd, RcData, NewRcSize);
	}

	mrc_free(RcData);
	mrc_free(HeadData);
	mrc_close(fHwnd);

	return MrpSize;
}
