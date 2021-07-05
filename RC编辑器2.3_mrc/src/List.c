#include "mrc_base.h"
#include "List.h"
#include "Funcs.h"
#include "res_str.h"

static PRC_ITEM FirstItem;//���������ǰ��
static uint8 AddrSize; //ƫ�������ֽ�����<64KʱΪ2λ��>64KΪ4λ
static uint8 RcStrCode; //RC�ַ������룬0 = Unicode Big��1 = GB2312

extern int16 RcCount;//�ַ�������

PRC_ITEM MakeRcList(const char* rFile, int16 listCount);

/*----------------------------------------------------------------------------------------------*/
//��ȡRC�ļ���������˫������
//�������ļ�·��
//���أ��ַ�����Ŀ
int32 ReadRc(const char* rFile)
{
    ClearItems();
    //WLog("�������ɹ��� ", 0);
    RcCount = CalcListCount(rFile);
    WLog("������Ŀ�ɹ���Ϊ��", RcCount);

	if (RcCount > 0)//�ַ�����>0
	{
        FirstItem = MakeRcList(rFile, RcCount);
        //WLog("��ȡ����ɹ��� ", 0);
        RcStrCode = JudgeCode(FirstItem);
        //WLog("�жϱ���ɹ���Ϊ��", RcStrCode);

        return MR_SUCCESS;
	}

	return MR_FAILED;
}

//����RC����
//�������ļ���
//���أ�������ļ���С
int32 SaveRc(const char *rFile)
{
	PRC_ITEM tempItem;//������
	int32 fHwnd, wPos=0, wPos2=0;//�ļ������ƫ��д���ַ������д���ַ
	int32 rSize, rOffset;//���ݴ�С������ƫ��
	char temp[300];
	sprintf(temp,"�����ļ�:%s",rFile);
WLog(temp,9999); 
	if (MR_IS_FILE == mrc_fileState(rFile))
	  { 
	  WLog("ɾ���ļ�",0); 
		mrc_remove(rFile);
		} 

	fHwnd = mrc_open(rFile, MR_FILE_RDWR | MR_FILE_CREATE);
	WLog("���ļ�:",fHwnd);
	WLog("�ַ�������",AddrSize);
	WLog("rc�ļ�ƫ�Ƴ���",RcCount);
	 
	if (0 != fHwnd) 
	{
		tempItem = FirstItem->Next;
		wPos = 0; rOffset = 0;
		wPos2 = RcCount * AddrSize;

		while(NULL != tempItem)//��������
		{
			rSize = tempItem->Size;
			mrc_seek(fHwnd, wPos, MR_SEEK_SET);
			mrc_write(fHwnd, &rOffset, AddrSize);//дƫ��
			mrc_seek(fHwnd, wPos2, MR_SEEK_SET);
			mrc_write(fHwnd, tempItem->DataStr, rSize);//д�ַ�������
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

//����Ϊ�ı��ļ�
//�������ļ���
//���أ�������ļ���С
int32 SaveToText(const char *rFile)
{
	PRC_ITEM tempItem;//������
	int32 fHwnd, rSize;//�ļ����,���ݴ�С
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

        if (0 == RcStrCode) //0 - Unicode��1 - GB2312
        {
            mrc_seek(fHwnd, 0, MR_SEEK_END);
		    mrc_write(fHwnd, "\xfe\xff", 2);//дBOM
        }

		while(NULL != tempItem)//��������
		{
            if (1 == RcStrCode) //0 - Unicode��1 - GB2312
            {
                rSize = mrc_strlen(tempItem->DataStr);
			    mrc_seek(fHwnd, 0, MR_SEEK_END);
			    mrc_write(fHwnd, tempItem->DataStr, rSize);//д�ַ�������
                mrc_seek(fHwnd, 0, MR_SEEK_END);
			    mrc_write(fHwnd, "\r\n", 2);//д���з�
            }
            else
            {
                rSize = mrc_wstrlen(tempItem->DataStr);
			    mrc_seek(fHwnd, 0, MR_SEEK_END);
			    mrc_write(fHwnd, tempItem->DataStr, rSize);//д�ַ�������
                mrc_seek(fHwnd, 0, MR_SEEK_END);
			    mrc_write(fHwnd, "\x0\xd\x0\xa", 4);//д���з�
            }
			tempItem = tempItem->Next;
		}

        mrc_free(sBuffer);
	}

	mrc_close(fHwnd);

	return 0;
}

/*----------------------------------------------------------------------------------------------*/
//����RC�ļ���Ŀ��
//�������ļ�·��
//���أ���Ŀ��
int16 CalcListCount(const char* rFile)
{
    int16 i=0;//����
	int32 lOffset=0, rOffset=0;//�ϸ�ƫ�ƣ��ַ���ƫ��
    int32 fHwnd=0, RcSize=0;//�ļ�������ļ���С
    char *oBuffer=NULL, *oPos=NULL;

    RcSize = mrc_getLen(rFile);//�����ļ���Сȷ��ƫ������������
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

	while(1)//ȡ��ƫ���б��С
	{
        rOffset = 0;
        oPos = oBuffer + i * AddrSize;
		mrc_memcpy(&rOffset, oPos, AddrSize);

		if (0 == i)
		{
			if (0 != rOffset) break;//�����׸�ƫ���Ƿ���0��������0��
		}
		else
		{
			if (rOffset < lOffset) break;//����ƫ���Ƿ����һ��������Ҫ��
		}

		if (rOffset > (RcSize - i * AddrSize)) break;//����ƫ���Ƿ񳬹��ļ���

		lOffset = rOffset;
		i++;
	}

    mrc_free(oBuffer);

    return i;
}

//���ļ���ȡRC����������
//�������ļ�·����RC��Ŀ��
//���أ�����ͷ
PRC_ITEM MakeRcList(const char* rFile, int16 listCount)
//void MakeRcList(const char* rFile, int16 listCount)
{
    int16 i = 0;//����
    int32 fHwnd = 0, RcSize = 0, oSize = 0;//�ļ�������ļ���С
    int32 lOffset = 0, rOffset = 0, rSize = 0;//�ϸ�ƫ�ƣ��ַ���ƫ�ơ���С
    char *oBuffer = NULL,  *sBuffer = NULL, *itemData = NULL;//�ڴ�ռ�
    char *sPos = NULL;//�ڴ�ָ��
    PRC_ITEM headItem, tempItem;//��ʱ�������������

    RcSize = mrc_getLen(rFile);
    fHwnd = mrc_open(rFile, MR_FILE_RDONLY);
    if (0 == fHwnd)
    {
        return NULL;
    }

    oSize = listCount * AddrSize;

    //��ȡƫ���б�
    oBuffer = (char*)mrc_malloc(oSize);
    mrc_seek(fHwnd, 0, MR_SEEK_SET);
	mrc_read(fHwnd, oBuffer, oSize);

    //��ȡ����
    sBuffer = (char*)mrc_malloc(RcSize - oSize);
    mrc_seek(fHwnd, oSize, MR_SEEK_SET);
	mrc_read(fHwnd, sBuffer, RcSize - oSize);

    mrc_close(fHwnd);

    sPos = sBuffer;

    //��ʼ������ͷ
	headItem = (PRC_ITEM)mrc_malloc(sizeof(RC_ITEM));
    headItem->Index = 0;
    headItem->Size = 0;
    headItem->DataStr = NULL;
    headItem->Next = NULL;
	tempItem = headItem;

	for (i = 1; i <= listCount; i++)//��������
	{
		if (i < listCount)//ǰn-1��
		{
            rOffset = 0;
			mrc_memcpy(&rOffset, oBuffer + (int32)(i * AddrSize), AddrSize);
		}
		else//���һ��
		{
			rOffset = RcSize - oSize;//��ȡ�ļ�����ʣ������
		}

		rSize = rOffset - lOffset;
		itemData = (char*)mrc_malloc(rSize + 2);
        if (NULL == itemData)
        {
            WLog("�������ɴ���1����ţ�", i);
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
            WLog("�������ɴ���2����ţ�", i);
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

//�ж��ַ�������
//����������ͷ
//���أ�0 - Unicode��1 - GB2312
uint8 JudgeCode(PRC_ITEM headItem)
{
    PRC_ITEM tempItem;
    uint8 rCode = 0;

    tempItem = headItem->Next;

    while(tempItem)
    {
        if (1 == (tempItem->Size % 2)) //��С���������϶���G2312
        {
            rCode = 1;
            break;
        }
        //if (0 != *(tempItem->DataStr + tempItem->Size - 2)) //�����ڶ��ֽڲ�Ϊ�㣬����Unicode
        //{
        //    rCode = 1;
        //    break;
        //}
        tempItem = tempItem->Next;
    }

    return rCode;
}

//��������ͷ��ڴ棩
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

//ȡ��ָ��������
//��������ţ����ػ���������������С
//���أ��ַ���ԭʼ���ݴ�С
uint16 GetItemData(int16 Index, char *sBuffer, uint16 bufSize)
{
    uint8 IsFind = 0;
    PRC_ITEM tempItem;

    WLog("��ʼ������Ŀ����ţ�", Index);

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
        WLog("δ�ҵ���Ŀ����ţ�", tempItem->Index);
        return 0;
    }

    WLog("������Ŀ�ɹ�����ţ�", tempItem->Index);

    if (0 == RcStrCode) //0 - Unicode��1 - GB2312
    {
        mrc_memset(sBuffer, 0, bufSize);
        if (tempItem->DataStr)
        {
            mrc_memcpy(sBuffer, tempItem->DataStr, MIN(bufSize - 2, tempItem->Size));
            WLog("Unicode���ݻ�ȡ�ɹ�����С��", tempItem->Size);
        }
    }
    else
    {
        GBToUni(tempItem->DataStr, sBuffer, bufSize);
        WLog("GB2312���ݻ�ȡ�ɹ�����С��", tempItem->Size);
    }

	return tempItem->Size;
}

//���浱ǰ�����ݣ������������ݣ����أ������ݴ�С
uint16 SaveItemData(uint16 Index, char *NewData)
{
    uint8 IsFind = 0;
	uint16 rSize;
	char *rBuffer;
    PRC_ITEM tempItem;

    WLog("��ʼ������Ŀ����ţ�", Index);

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
        WLog("δ�ҵ�Ҫ������Ŀ����ţ�", Index);
        return 0;
    }

    if (tempItem->DataStr)
	    mrc_free(tempItem->DataStr);

    rSize = (uint16)(mrc_wstrlen(NewData) + 2);
	rBuffer = (char*)mrc_malloc(rSize);

    if (0 == RcStrCode) //0 - Unicode��1 - GB2312
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

    WLog("������Ŀ�ɹ���", 0);

	return rSize;
}
