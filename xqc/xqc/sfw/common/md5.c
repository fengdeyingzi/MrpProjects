#include "md5.h"

//  ��MD5�㷨ȡɢ��ֵ
//  ����:	szSour Դ�ַ���
//			iLen   Դ�ַ�������
//			szDest Ŀ�Ĵ�(16�ֽ�)
VOID MD5(BYTE *szSour, int iLen, BYTE *szDest)
{
	md5_state_t context;

	SGL_MEMSET(&context, 0, sizeof(context));
	mrc_md5_init(&context);
	mrc_md5_append(&context, (md5_byte_t *)szSour, iLen);
	mrc_md5_finish(&context, (md5_byte_t *)szDest);
}

