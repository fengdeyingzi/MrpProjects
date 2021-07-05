#include "mrc_base.h"

#include "FileRW.h"
#include "List.h"
#include "Funcs.h"

/*---------------------------------------------------------------------------------------------*/
//��Mrp��ȡrc�ļ���������Mrp�ļ���
int32 ReadMrp(const char *MrpFile)
{
	int32 RcSize=0, rValue=0, fHwnd=0;
	uint8 *RcData=NULL;
    const char *tempFile = "temp.rc";
WLog("��mrp����ļ�",0); 
	rValue = mrc_readFileFromMrpEx((char*)MrpFile, "res_lang0.rc", &RcData, &RcSize, 0);
	if (MR_SUCCESS == rValue)
	{
		WLog ("�ж�mrp�ļ��ɹ�",0); 
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

//����rc��Mrp�ļ���������Mrp�ļ���
int32 SaveMrp(const char *MrpFile)
{
	int32 fHwnd;
	int32 MrpSize, NameLen, NewOffset;
	uint32 HeadSize,NewRcSize;
	char *HeadData, *RcData, *pPos, *RcName, *tempFile;

	RcName = "res_lang0.rc";
	NameLen = 13;

	//����res_lang0.rc�����¶�ȡ
	tempFile = "temp.rc";
	NewRcSize = SaveRc(tempFile);
	RcData = (char*)mrc_readAll(tempFile, &NewRcSize);
	mrc_remove(tempFile);
	if (NULL == RcData) return 0;

	fHwnd = mrc_open(MrpFile, MR_FILE_RDWR);
	if (0 == fHwnd) return 0;

	//ȡ��Mrp�ܴ�С
	mrc_seek(fHwnd, 8, MR_SEEK_SET);
	mrc_read(fHwnd, &MrpSize, 4);

	//ȡ����Ϣ���б��С
	mrc_seek(fHwnd, 4, MR_SEEK_SET);
	mrc_read(fHwnd, &HeadSize, 4);
	HeadSize += 8;

	//ȡ����Ϣ���б�����
	HeadData = (char*)mrc_malloc(HeadSize);
	mrc_seek(fHwnd, 0, MR_SEEK_SET);
	mrc_read(fHwnd, HeadData, HeadSize);

	//����res_lang0.rc
	pPos = FindPos(HeadData, RcName, HeadSize, NameLen);
	if (pPos) 
	{
		//�޸���res_lang0.rcƫ��
		NewOffset = MrpSize+NameLen + 8;
		mrc_memcpy(pPos+13, &NewOffset, 4);

		//�޸���res_lang0.rc��С
		mrc_memcpy(pPos+17, &NewRcSize, 4);

		//�޸�Mrp�ܴ�С
		MrpSize = MrpSize + NewRcSize + NameLen + 8;
		mrc_memcpy(HeadData+8, &MrpSize, 4);

		//д��Mrp�ļ�
		mrc_seek(fHwnd, 0, MR_SEEK_SET);
		mrc_write(fHwnd, HeadData, HeadSize);

		//д��res_lang0.rc��Ϣ��ĩβ
		mrc_seek(fHwnd, 0, MR_SEEK_END);
		mrc_write(fHwnd, &NameLen, 4);
		mrc_seek(fHwnd, 0, MR_SEEK_END);
		mrc_write(fHwnd, RcName, NameLen);
		mrc_seek(fHwnd, 0, MR_SEEK_END);
		mrc_write(fHwnd, &NewRcSize, 4);

		//д��res_lang0.rc���ݵ�ĩβ
		mrc_seek(fHwnd, 0, MR_SEEK_END);
		mrc_write(fHwnd, RcData, NewRcSize);
	}

	mrc_free(RcData);
	mrc_free(HeadData);
	mrc_close(fHwnd);

	return MrpSize;
}
