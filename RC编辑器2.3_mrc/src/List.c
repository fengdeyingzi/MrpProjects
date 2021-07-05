#include "mrc_base.h"
#include "List.h"
#include "Funcs.h"
#include "res_str.h"

static PRC_ITEM FirstItem;//链表首项，当前项
static uint8 AddrSize; //偏移所用字节数，<64K时为2位，>64K为4位
static uint8 RcStrCode; //RC字符串编码，0 = Unicode Big，1 = GB2312

extern int16 RcCount;//字符串总数

PRC_ITEM MakeRcList(const char* rFile, int16 listCount);

/*----------------------------------------------------------------------------------------------*/
//读取RC文件并保存入双向链表
//参数：文件路径
//返回：字符串数目
int32 ReadRc(const char* rFile)
{
    ClearItems();
    //WLog("清除链表成功！ ", 0);
    RcCount = CalcListCount(rFile);
    WLog("分析数目成功！为：", RcCount);

	if (RcCount > 0)//字符串数>0
	{
        FirstItem = MakeRcList(rFile, RcCount);
        //WLog("读取链表成功！ ", 0);
        RcStrCode = JudgeCode(FirstItem);
        //WLog("判断编码成功！为：", RcStrCode);

        return MR_SUCCESS;
	}

	return MR_FAILED;
}

//保存RC数据
//参数：文件名
//返回：保存的文件大小
int32 SaveRc(const char *rFile)
{
	PRC_ITEM tempItem;//链表项
	int32 fHwnd, wPos=0, wPos2=0;//文件句柄，偏移写入地址，数据写入地址
	int32 rSize, rOffset;//数据大小，数据偏移
	char temp[300];
	sprintf(temp,"保存文件:%s",rFile);
WLog(temp,9999); 
	if (MR_IS_FILE == mrc_fileState(rFile))
	  { 
	  WLog("删除文件",0); 
		mrc_remove(rFile);
		} 

	fHwnd = mrc_open(rFile, MR_FILE_RDWR | MR_FILE_CREATE);
	WLog("打开文件:",fHwnd);
	WLog("字符串总数",AddrSize);
	WLog("rc文件偏移长度",RcCount);
	 
	if (0 != fHwnd) 
	{
		tempItem = FirstItem->Next;
		wPos = 0; rOffset = 0;
		wPos2 = RcCount * AddrSize;

		while(NULL != tempItem)//遍历链表
		{
			rSize = tempItem->Size;
			mrc_seek(fHwnd, wPos, MR_SEEK_SET);
			mrc_write(fHwnd, &rOffset, AddrSize);//写偏移
			mrc_seek(fHwnd, wPos2, MR_SEEK_SET);
			mrc_write(fHwnd, tempItem->DataStr, rSize);//写字符串数据
			wPos += AddrSize;
			wPos2 += rSize;
			rOffset += rSize;
			tempItem = tempItem->Next;
		}
		mrc_close(fHwnd);
	WLog("close...",0);
	}
	

	return wPos2;
}

//保存为文本文件
//参数：文件名
//返回：保存的文件大小
int32 SaveToText(const char *rFile)
{
	PRC_ITEM tempItem;//链表项
	int32 fHwnd, rSize;//文件句柄,数据大小
	int32 bufSize = 1024 * 8;//
    char *sBuffer;

    if (NULL == FirstItem)
        return -1;

	if (MR_IS_FILE == mrc_fileState(rFile))
		mrc_remove(rFile);

	fHwnd = mrc_open(rFile, MR_FILE_RDWR | MR_FILE_CREATE);
	if (0 != fHwnd) 
	{
		tempItem = FirstItem->Next;
        sBuffer = (char*)mrc_malloc(bufSize);

        if (0 == RcStrCode) //0 - Unicode，1 - GB2312
        {
            mrc_seek(fHwnd, 0, MR_SEEK_END);
		    mrc_write(fHwnd, "\xfe\xff", 2);//写BOM
        }

		while(NULL != tempItem)//遍历链表
		{
            if (1 == RcStrCode) //0 - Unicode，1 - GB2312
            {
                rSize = mrc_strlen(tempItem->DataStr);
			    mrc_seek(fHwnd, 0, MR_SEEK_END);
			    mrc_write(fHwnd, tempItem->DataStr, rSize);//写字符串数据
                mrc_seek(fHwnd, 0, MR_SEEK_END);
			    mrc_write(fHwnd, "\r\n", 2);//写换行符
            }
            else
            {
                rSize = mrc_wstrlen(tempItem->DataStr);
			    mrc_seek(fHwnd, 0, MR_SEEK_END);
			    mrc_write(fHwnd, tempItem->DataStr, rSize);//写字符串数据
                mrc_seek(fHwnd, 0, MR_SEEK_END);
			    mrc_write(fHwnd, "\x0\xd\x0\xa", 4);//写换行符
            }
			tempItem = tempItem->Next;
		}

        mrc_free(sBuffer);
	}

	mrc_close(fHwnd);

	return 0;
}

/*----------------------------------------------------------------------------------------------*/
//分析RC文件项目数
//参数：文件路径
//返回：项目数
int16 CalcListCount(const char* rFile)
{
    int16 i=0;//计数
	int32 lOffset=0, rOffset=0;//上个偏移，字符串偏移
    int32 fHwnd=0, RcSize=0;//文件句柄，文件大小
    char *oBuffer=NULL, *oPos=NULL;

    RcSize = mrc_getLen(rFile);//根据文件大小确定偏移量数据类型
	if (RcSize < 65536)
	{
		AddrSize = 2;
	}
	else if (RcSize < 204800)
	{
		AddrSize = 4;
	}
	else
	{
		return 0;
	}

    fHwnd = mrc_open(rFile, MR_FILE_RDONLY);
    if (0 == fHwnd)
    {
        return 0;
    }
    oBuffer = (char*)mrc_malloc(RcSize);
    mrc_read(fHwnd, oBuffer, RcSize);
    mrc_close(fHwnd);

	while(1)//取得偏移列表大小
	{
        rOffset = 0;
        oPos = oBuffer + i * AddrSize;
		mrc_memcpy(&rOffset, oPos, AddrSize);

		if (0 == i)
		{
			if (0 != rOffset) break;//检验首个偏移是否是0，正常是0。
		}
		else
		{
			if (rOffset < lOffset) break;//检验偏移是否比上一个大，正常要大。
		}

		if (rOffset > (RcSize - i * AddrSize)) break;//检验偏移是否超过文件。

		lOffset = rOffset;
		i++;
	}

    mrc_free(oBuffer);

    return i;
}

//从文件读取RC并生成链表
//参数：文件路径，RC项目数
//返回：链表头
PRC_ITEM MakeRcList(const char* rFile, int16 listCount)
//void MakeRcList(const char* rFile, int16 listCount)
{
    int16 i = 0;//计数
    int32 fHwnd = 0, RcSize = 0, oSize = 0;//文件句柄，文件大小
    int32 lOffset = 0, rOffset = 0, rSize = 0;//上个偏移，字符串偏移、大小
    char *oBuffer = NULL,  *sBuffer = NULL, *itemData = NULL;//内存空间
    char *sPos = NULL;//内存指针
    PRC_ITEM headItem, tempItem;//临时项，用于生成链表

    RcSize = mrc_getLen(rFile);
    fHwnd = mrc_open(rFile, MR_FILE_RDONLY);
    if (0 == fHwnd)
    {
        return NULL;
    }

    oSize = listCount * AddrSize;

    //读取偏移列表
    oBuffer = (char*)mrc_malloc(oSize);
    mrc_seek(fHwnd, 0, MR_SEEK_SET);
	mrc_read(fHwnd, oBuffer, oSize);

    //读取内容
    sBuffer = (char*)mrc_malloc(RcSize - oSize);
    mrc_seek(fHwnd, oSize, MR_SEEK_SET);
	mrc_read(fHwnd, sBuffer, RcSize - oSize);

    mrc_close(fHwnd);

    sPos = sBuffer;

    //初始化链表头
	headItem = (PRC_ITEM)mrc_malloc(sizeof(RC_ITEM));
    headItem->Index = 0;
    headItem->Size = 0;
    headItem->DataStr = NULL;
    headItem->Next = NULL;
	tempItem = headItem;

	for (i = 1; i <= listCount; i++)//构造链表
	{
		if (i < listCount)//前n-1项
		{
            rOffset = 0;
			mrc_memcpy(&rOffset, oBuffer + (int32)(i * AddrSize), AddrSize);
		}
		else//最后一项
		{
			rOffset = RcSize - oSize;//读取文件所有剩余数据
		}

		rSize = rOffset - lOffset;
		itemData = (char*)mrc_malloc(rSize + 2);
        if (NULL == itemData)
        {
            WLog("链表生成错误1，序号：", i);
            break;
        }
        mrc_memset(itemData, 0, rSize + 2);
        mrc_memcpy(itemData, sPos, rSize);

		sPos += rSize;
		lOffset = rOffset;

        tempItem->Next = (PRC_ITEM)mrc_malloc(sizeof(RC_ITEM));
        tempItem = tempItem->Next;
        if (NULL == tempItem)
        {
            WLog("链表生成错误2，序号：", i);
            break;
        }
        tempItem->Index = i;
        tempItem->Size = (uint16)rSize;
		tempItem->DataStr = itemData;
        tempItem->Next = NULL;
	}

    mrc_free(oBuffer);
    mrc_free(sBuffer);

    return headItem;
}

//判断字符串编码
//参数：链表头
//返回：0 - Unicode，1 - GB2312
uint8 JudgeCode(PRC_ITEM headItem)
{
    PRC_ITEM tempItem;
    uint8 rCode = 0;

    tempItem = headItem->Next;

    while(tempItem)
    {
        if (1 == (tempItem->Size % 2)) //大小是奇数，肯定是G2312
        {
            rCode = 1;
            break;
        }
        //if (0 != *(tempItem->DataStr + tempItem->Size - 2)) //倒数第二字节不为零，不是Unicode
        //{
        //    rCode = 1;
        //    break;
        //}
        tempItem = tempItem->Next;
    }

    return rCode;
}

//清除链表（释放内存）
void ClearItems(void)
{
	PRC_ITEM tempItem, p1 = NULL;
	tempItem = FirstItem;

	while(tempItem)
	{
		if(tempItem->DataStr)
		    mrc_free(tempItem->DataStr);
		p1 = tempItem->Next;
		mrc_free(tempItem);
		tempItem = p1;
	}

	RcCount = 0;
}

//取得指定项数据
//参数：序号，返回缓冲区，缓冲区大小
//返回：字符串原始数据大小
uint16 GetItemData(int16 Index, char *sBuffer, uint16 bufSize)
{
    uint8 IsFind = 0;
    PRC_ITEM tempItem;

    WLog("开始查找项目，序号：", Index);

    tempItem = FirstItem->Next;

    while(tempItem)
	{
        if (tempItem->Index == Index)
        {
            IsFind = 1;
            break;
        }
		tempItem = tempItem->Next;
	}

    if (0 == IsFind)
    {
        WLog("未找到项目，序号：", tempItem->Index);
        return 0;
    }

    WLog("查找项目成功！序号：", tempItem->Index);

    if (0 == RcStrCode) //0 - Unicode，1 - GB2312
    {
        mrc_memset(sBuffer, 0, bufSize);
        if (tempItem->DataStr)
        {
            mrc_memcpy(sBuffer, tempItem->DataStr, MIN(bufSize - 2, tempItem->Size));
            WLog("Unicode内容获取成功！大小：", tempItem->Size);
        }
    }
    else
    {
        GBToUni(tempItem->DataStr, sBuffer, bufSize);
        WLog("GB2312内容获取成功！大小：", tempItem->Size);
    }

	return tempItem->Size;
}

//保存当前项数据，参数：新数据，返回：新数据大小
uint16 SaveItemData(uint16 Index, char *NewData)
{
    uint8 IsFind = 0;
	uint16 rSize;
	char *rBuffer;
    PRC_ITEM tempItem;

    WLog("开始保存项目，序号：", Index);

    tempItem = FirstItem->Next;

    while(tempItem)
	{
        if (tempItem->Index == Index)
        {
            IsFind = 1;
            break;
        }
		tempItem = tempItem->Next;
	}

    if (0 == IsFind)
    {
        WLog("未找到要保存项目，序号：", Index);
        return 0;
    }

    if (tempItem->DataStr)
	    mrc_free(tempItem->DataStr);

    rSize = (uint16)(mrc_wstrlen(NewData) + 2);
	rBuffer = (char*)mrc_malloc(rSize);

    if (0 == RcStrCode) //0 - Unicode，1 - GB2312
    {
	    mrc_memset(rBuffer, 0, rSize);
	    mrc_memcpy(rBuffer, NewData, rSize - 2);
    }
    else
    {
        UniToGB(NewData, rBuffer, rSize);
        rSize = mrc_strlen(rBuffer) + 1;
    }

    tempItem->Size = rSize;
	tempItem->DataStr = rBuffer;

    WLog("保存项目成功！", 0);

	return rSize;
}
