#include "mrc_base.h"
#include "mrc_exb.h"
#include "mrc_win.h"
#include "FileView.h"
/*
typedef struct 
{
	int32 dft_list;
	char list_path[260];  //gb����·��
}MR_T_FMGR_REQ;*/

extern int32 mrc_platEx(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB* cb);

//��ʼ���ļ���������أ�API���
int32 FV_Init(void)
{
	return mrc_platEx(1401, NULL, 0, NULL, NULL, NULL);
}

//������ʾ���ͣ����������ʹ��ţ��Ƿ���ʾ�����أ�API���
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

//��ʼ�������������ʼ·����ϵͳ·�����ţ����ȣ������أ�API���
int32 FV_BeginView(const char *Path, int32 SysPath)
{
	MR_T_FMGR_REQ vPath;

	vPath.dft_list = SysPath;
	mrc_memset(vPath.list_path, 0, sizeof(vPath.list_path));
	mrc_memcpy(vPath.list_path, Path, mrc_strlen(Path));

	return mrc_platEx(1404, (uint8*)&vPath, sizeof(vPath), NULL, NULL, NULL);
}

//ȡ��ѡ�����ļ�·�������������ص��ַ��������أ�API���
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

//������������أ�API���
int32 FV_EndView(void)
{
	return mrc_platEx(1406, NULL, 0, NULL, NULL, NULL);
}

/*------------------------------------------------------------------------------------------*/
