#include "mrc_base.h"
#include "mrc_exb.h"
#include "res_str.h"
#include "Funcs.h"
#include "AppMain.h"

//#define CONFIG_SPR

uint16 *myrcOffset;//RC偏移
char *myrcData;//RC内容
long wstrlen(char*);

extern int32 mrc_sleep(uint32 ms);//系统API，找不到头文件了。。。

//释放图标文件到冒泡图标文件夹
void ReleaseIco(void)
{
	char *IcoFile=NULL, *IcoData=NULL;
	int32 fHwnd, fLen;

    mr_screeninfo sInfo;//屏幕信息
    mrc_getScreenInfo(&sInfo);

	IcoFile = (char*)mrc_malloc(128);
	mrc_memset(IcoFile, 0, 128);
	mrc_sprintf(IcoFile, "app%d%d\\res", sInfo.width, sInfo.height);

	if (MR_IS_DIR == mrc_fileState((const char*)IcoFile))//判断文件夹是否存在
	{
		mrc_strcat(IcoFile, "\\7MUO7.bmp");
	    mrc_remove(IcoFile);
		fHwnd = mrc_open(IcoFile, MR_FILE_RDWR | MR_FILE_CREATE);

		if (fHwnd)//判断图标文件是否打开成功
		{
			IcoData = (char*)mrc_readFileFromMrp((const char*)"icon.bmp", &fLen, 0);
			if (IcoData)//判断图标读取是否成功
			{
				mrc_seek(fHwnd, 0, MR_SEEK_SET);
				mrc_write(fHwnd, IcoData, fLen);
				mrc_freeFileData(IcoData, fLen);
			}
			mrc_close(fHwnd);
		}
	}
	mrc_free(IcoFile);
}

//显示LOGO
void ShowLOGO(void)
{
    const int16 pw = 128, ph = 128;//LOGO宽高

#ifdef CONFIG_SPR //展讯图像
	const uint32 CRCStand = 0xECCE2245;//CRC校验值 + 1
#else //CONFIG_MTK
	const uint32 CRCStand = 0x8A4E45E3;//CRC校验值 + 1
#endif

    char picName[20];
    int32 rValue, fLen;
	uint8 *PicBuffer;
	uint32 CRCout;

    mr_screeninfo sInfo;//屏幕信息
    mrc_getScreenInfo(&sInfo);

    UniToGB(GetString(STR_LOGONAME), picName, 20);

	PicBuffer = (uint8*)mrc_readFileFromMrp(picName, &fLen, 0);
	if (PicBuffer)
	{
		mrc_updcrc(NULL, 0);
		mrc_updcrc(PicBuffer, fLen);
		CRCout = mrc_updcrc(PicBuffer, 0) + 1;
		if (CRCStand == CRCout)//CRC校验
		{
			rValue = mrc_bitmapShowEx((uint16*)PicBuffer, (int16)(sInfo.width - pw)/2, (int16)(sInfo.height - ph)/2, pw, pw, ph, BM_TRANSPARENT, 0, 0);
		}
		else
		{
			ShowText(GetString(STR_NOLOGO), 0);
            ReleaseAllStrings();
		}
		mrc_free(PicBuffer);
	}
	else
	{
		ShowText(GetString(STR_NOLOGO), 0);
        ReleaseAllStrings();
	}

	mrc_refreshScreen(0, 0, (uint16)sInfo.width, (uint16)sInfo.height);
	mrc_sleep(1500);//暂停1.5秒
	DrawAll();
}

/*--------------------------------------------------------------------------------------*/
//载入本程序res_lang0.rc文件
void LoadAllStrings(void)
{
	int32 fLen;
	//const uint32 CRCStand = 0xDAF91335;//CRC值-1
	const uint32 CRCStand = -1648944000;
	uint32 CRCout;

	myrcOffset = (uint16*)mrc_readFileFromMrp("res_lang0.rc", &fLen, 0);
	if (NULL != myrcOffset)
	{
		myrcData = (char*)myrcOffset + RES_STRING_COUNT * 2;
		mrc_updcrc(NULL, 0);
		mrc_updcrc((uint8*)myrcOffset, fLen);
		CRCout = mrc_updcrc((uint8*)myrcOffset, 0) - 1;
		//WLog("CRC效验码:",CRCout);
		if (CRCStand != CRCout)//CRC校验
		{	//"警告：程序被非法修改，继续使用将会死机，请迅速退出！"
			TextToBuffer("\x8b\x66\x54\x4a\xff\x1a\x7a\xb\x5e\x8f\x88\xab\x97\x5e\x6c\xd5\x4f\xee\x65\x39\xff\xc\x7e\xe7\x7e\xed\x4f\x7f\x75\x28\x5c\x6\x4f\x1a\x6b\x7b\x67\x3a\xff\xc\x8b\xf7\x8f\xc5\x90\x1f\x90\x0\x51\xfa\xff\x1");
			ReleaseAllStrings();
		}
		
	}
}

//释放本程序res_lang0.rc所用资源
void ReleaseAllStrings(void)
{
	if (NULL!=myrcOffset)
	{
		mrc_free(myrcOffset);
		myrcOffset = NULL;
		myrcData = NULL;
	}
}

//取得本程序字符串
char* GetString(uint16 Index)
{
	uint16 rOffset;

	if (Index > RES_STRING_COUNT)
		return NULL;

	rOffset = *(myrcOffset + Index);

	return (myrcData + rOffset);
}

/*----------------------------------------------------------------------------------------*/
//查找位置
//参数：源数据，搜索数据，源大小，搜索大小
//返回：在源数据找到位置的指针
char *FindPos(char *StrS, char *StrF, int32 SLen, int32 FLen)
{
	char *Str1, *Str2;
	int32 Found, i, j;

	for(i = 0; i < SLen; i++)
	{
		Found = 1;
		for(j = 0; j < FLen; j++)
		{
			Str1 = StrS + i + j;
            Str2 = StrF + j;
			if (*Str1 != *Str2)
			{
				Found = 0;
				break;
			}
		}
		if (1 == Found) return StrS + i;
	}
	return NULL;
}

//转换Unicode字符串为数值
int32 watoi(char *UniStr)
{
    int32 sLen, i, rValue;
    char *asciiStr;

    sLen = mrc_wstrlen(UniStr);
    asciiStr = mrc_malloc(sLen + 1);
    mrc_memset(asciiStr, 0, sLen + 1);

    for(i = 0; i < sLen; i++)
    {
        asciiStr[i] = UniStr[i * 2 + 1];
    }

    rValue = mrc_atoi(asciiStr);
    mrc_free(asciiStr);
    return rValue;
}

//转换GB2312为Unicode
//参数：GB2312字符串，Unicode缓冲区，缓冲区大小
void GBToUni(const char *gbStr, char *uniBuffer, int32 bufSize)
{
    char *uniStr;
    int32 uniSize, errPos;

    mrc_memset(uniBuffer, 0, bufSize);
    uniStr = (char*)mrc_c2u((char*)gbStr, &errPos, &uniSize);
    if (uniStr)
    {
        mrc_memcpy(uniBuffer, uniStr, MIN(bufSize - 2, uniSize));
        mrc_free(uniStr);
    }
}

//转换Unicode为GB2312
//参数：Unicode字符串，GB2312缓冲区，缓冲区大小
void UniToGB(const char *uniStr, char *gbBuffer, int32 bufSize)
{
    uint8 *gbStr;
    int32 uniLen = mrc_wstrlen((char*)uniStr) + 2;
    int32 gbSize = bufSize, rValue;

    mrc_memset(gbBuffer, 0, bufSize);
	gbStr = (uint8*)mrc_malloc(bufSize);//输出缓冲
	mrc_memset(gbStr, 0, bufSize);
	rValue = mrc_unicodeToGb2312((uint8*)uniStr, uniLen, &gbStr, &gbSize);//转换为GB2312
    if (MR_SUCCESS == rValue)
    {
	    mrc_memcpy(gbBuffer, gbStr, MIN(bufSize - 1, gbSize));
	    mrc_free(gbStr);
    }
}

//记录日志
void WLog(char* Text, int32 Value)
{
	int32 fHwnd;
	char tStr[64];

	fHwnd=mrc_open("log.txt", MR_FILE_RDWR | MR_FILE_CREATE);
	mrc_seek(fHwnd, 0, MR_SEEK_END);
    mrc_write(fHwnd, Text, mrc_strlen(Text));
	mrc_sprintf(tStr, "%d\r\n", Value);
	mrc_write(fHwnd, tStr, mrc_strlen(tStr));
	mrc_close(fHwnd);
}


//记录日志(unicode)
void WLogu(char* Text, char *text2)
{
	int32 fHwnd;
	char tStr[64];
	char *gbtext=malloc(90); 

	fHwnd=mrc_open("log.txt", MR_FILE_RDWR | MR_FILE_CREATE);
	mrc_seek(fHwnd, 0, MR_SEEK_END);
    mrc_write(fHwnd, Text, mrc_strlen(Text));
     UniToGB(text2, gbtext, mrc_wstrlen(text2));//转换为GB2312
	mrc_sprintf(tStr, "%s%d\n", gbtext,mrc_wstrlen(text2));
	mrc_write(fHwnd, tStr, mrc_strlen(tStr));
	mrc_close(fHwnd);
}
//
////内存信息
//void GetMemInfo(char *DescText)
//{
//    int mUsed, mLeft, mMax;
//
//    mMax = mrc_getMemStatus(&mUsed, &mLeft, NULL, NULL, NULL, NULL);
//    WLog(DescText, 0);
//    WLog("内存峰值：", mMax);
//    WLog("使用内存：", mUsed);
//    WLog("剩余内存：", mLeft);
//}