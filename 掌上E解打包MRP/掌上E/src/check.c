#include "mrc_base.h"
#include "mrc_exb.h"
#include "check.h"

int32 Check(void)
{
	int32 f=0;
	int32 mrpLen=0;
	int32 ret=0;
	uint8 *md516=NULL,
	      *md532=NULL;

	char *filename=NULL,
	     *correct=NULL;

	md516 = (uint8*)mrc_malloc(16);
		mrc_memset(md516, 0, 16);
	md532 = (uint8*)mrc_malloc(32);
		mrc_memset(md532, 0, 32);
	correct = (char*)malloc(32);
		mrc_memset(correct,0,32);
	filename = mrc_getPackName();

	if(f)//此处下断点用于取得正确的MD5
	{
		mrpLen = mrc_getLen(filename);
	}
	else
	{
		mrpLen = mrc_getLen(filename) - 32;
	}
	f = mrc_open(filename,MR_FILE_RDONLY);
	mrc_seek(f,mrpLen,MR_SEEK_SET);
	mrc_read(f,(void*)correct,32);

	ret = md5MakeFromFileHandle("\n",f,0,mrpLen,md516);
	mrc_close(f);
	if(ret == MR_FAILED)
	{
		mrc_free(md516);
		mrc_free(md532);
		mrc_free(correct);
		mrc_free(filename);
		return MR_FAILED;
	}
	md5ToString(md516,md532);
	f = 0;
	if(f)//此处下断点用于写出正确的MD5
	{
		f=mrc_open("1",MR_FILE_CREATE | MR_FILE_RDWR);
		mrc_write(f,md532,32);
		mrc_close(f);
	}
	ret = mrc_strncmp((char*)md532,correct,32);
	mrc_free(md516);
	mrc_free(md532);
	mrc_free(correct);
	mrc_free(filename);
	if(ret == 0)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}