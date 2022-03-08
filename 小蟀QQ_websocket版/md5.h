#ifndef __MD5_H__
#define __MD5_H__

#include "mrc_exb.h"

//主函数添加
/*
	int32 w,h;
	if (Check_Authorized()==MR_FAILED)
	{
		mrc_textWidthHeight("文件损坏",0,MR_FONT_MEDIUM,&w,&h);
		mrc_clearScreen(0, 0, 0);
		mrc_drawText("文件损坏", (240-w)/2, (320-h)/2, 255, 255, 255, 0, MR_FONT_MEDIUM);
		mrc_refreshScreen(0, 0, 240, 320);
		mrc_sleep(3000);
		mrc_exit();	
	}
*/

//从文件名计算MD5值
int32 MD5_MakeFromFile(char *key, char *filename, int32 offset, int32 len, uint8* out);
//检查内部文件,编译一下,写在220偏移
int32 Check_Authorized(void);

int32 MD5_MakeFromFile(char *key, char *filename, int32 offset, int32 len, uint8* out)
{
	int32 flen = 0;
	char *src = NULL;
	md5_state_t context;

	//1.MD5初始化
	if(out == NULL) return MR_FAILED;

	mrc_memset(&context, 0, sizeof(context));
	mrc_md5_init(&context);

	//2.打开文件获取加密源
	flen = mrc_getLen(filename);
	if(MR_FAILED == flen) 
		return MR_FAILED;
	
	if(len > 0)	
	{
		int32 fd, read0;

		fd = mrc_open(filename, MR_FILE_RDONLY);
		if(!fd) 
			return MR_FAILED;

		if(len > flen) 
			len = flen;	//输入的长度小于文件总长则取文件长度
		src = mrc_malloc(len);
		if(!src)
		{
			mrc_close(fd);
			return MR_FAILED;
		}
		mrc_memset(src, 0, len);

		mrc_seek(fd, (offset < 0) ? 0 : offset, MR_SEEK_SET);
		read0 = mrc_read(fd, src, len);
		mrc_close(fd);
		mrc_md5_append(&context, (md5_byte_t*)src, read0);
	}
	
	//3.加入密钥
	if(key != NULL)
	{
		mrc_md5_append(&context, (md5_byte_t *)key, mrc_strlen(key));
	}

	//获取MD5
	mrc_md5_finish(&context, (md5_byte_t *)out);

	//释放分配内存
	if(src) mrc_free(src);

	return MR_SUCCESS;
}

int32 Check_Authorized(void)
{
	int32 ret = MR_FAILED;
	uint8 md5[17] = {0};
	uint8 cor[17] = {0};
	int32 len;
	int32 fd;
	char *name = mrc_getPackName();
/*
#ifdef WIN32
	return MR_SUCCESS;
#endif
*/
	len = mrc_getLen(name) - 240;
	fd = mrc_open(name, MR_FILE_RDONLY);
	if(fd > 0)
	{
		mrc_seek(fd, 220, MR_SEEK_SET);
		mrc_read(fd, cor, 16);
		mrc_close(fd);

		if(MR_SUCCESS == MD5_MakeFromFile((char*)"skymobi", name, 240, len, md5))
			ret = (mrc_memcmp(md5, cor, 16) == 0) ? MR_SUCCESS : MR_FAILED;
		else
			ret = MR_FAILED;
	}else
		ret = MR_FAILED;

	return ret;
}

#endif