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
		mrc_memcpy(pPos+strlen(RcName)+1, &NewOffset, 4);

		//修改新res_lang0.rc大小
		mrc_memcpy(pPos+strlen(RcName)+1+4, &NewRcSize, 4);

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

// 获取文件名
 static char* getName(char *path) {
	 int i = 0;
		for ( i = mrc_strlen(path) - 1; i >= 0; i--) {
			if (path[i] == '/' || path[i] == '\\') {
				return path+i + 1;
			}
		}
		return path;
	}

//需要free
	 static char* getDirName(char *path) {
	 int i = 0;
	 int end = mrc_strlen(path);
	 char *dirname = mrc_malloc(128);
	 int start = 0;
	 mrc_memset(dirname, 0, 128);
	 mrc_strcpy(dirname, path);
		for ( i = mrc_strlen(path) - 1; i >= 0; i--) {
			if (path[i] == '/' || path[i] == '\\') {
				end = i;
				break;
			}
		}
		for(i = end;i>=0;i--){
			if (path[i] == '/' || path[i] == '\\') {
				start = i+1;
				mrc_memcpy(dirname, path+start, end-start);
				break;
			}
		}
		return dirname;
	}

//替换mrp中指定文件
int32 mrpReplaceFile(const char *MrpFile,char *filename)
{
	int32 fHwnd;
	int32 MrpSize, NameLen, NewOffset;
	uint32 HeadSize,NewRcSize;
	char *HeadData, *RcData, *RcName, *pPos;

	RcName = getName(filename);
	NameLen = 13;

	//保存res_lang0.rc并重新读取
	NewRcSize = mrc_getFileLen(filename);
	RcData = (char*)mrc_readAll(filename, &NewRcSize);
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
		mrc_memcpy(pPos+strlen(RcName)+1, &NewOffset, 4);

		//修改新res_lang0.rc大小
		mrc_memcpy(pPos+strlen(RcName)+1+4, &NewRcSize, 4);

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

int32 removeDir(const char* dirName)
{
    char fullName[128];
    char fileName[64];
    int32 ret = -1;
    int32 dirNameLen;

    if(!dirName)
        return MR_FAILED;    
    memset(fullName, 0, sizeof(fullName));
    memset(fileName, 0, sizeof(fileName));

    dirNameLen = strlen(dirName);
    memcpy(fullName, dirName, dirNameLen);
    if(fullName[dirNameLen-1] != '/')//准备拼接目录下的子文件或子目录
    {
        fullName[dirNameLen] = '/';
        dirNameLen++;
    }

    if(mrc_fileState(dirName) == MR_IS_DIR)
    {
        int32 fd;
        int32 result = MR_FAILED;

        fd = mrc_findStart(dirName, fileName, sizeof(fileName));
        if(fd > 0)//搜索句柄有效
        {
            result = MR_SUCCESS;
            while(result == MR_SUCCESS)
            {
                //如果fileName取值为’.’标识该搜索值无效，请放弃本次处理。
                if(fileName[0] && fileName[0] != '.')
                {
                    int32 length = strlen(fileName);
                    memcpy(&fullName[dirNameLen], fileName, length);
                    fullName[dirNameLen+length] = 0;

                    length = mrc_fileState(fullName);
                    if(length == MR_IS_FILE)//文件类型，删除文件
                        mrc_remove(fullName);                  
                    else if(length == MR_IS_DIR)//文件夹类型，递归删除子文件夹
                        removeDir(fullName);
                }
                memset(fileName, 0, sizeof(fileName));
                result = mrc_findGetNext(fd, fileName, sizeof(fileName));
            }
            mrc_findStop(fd);
        }
        //进行一次假搜索,解决不能删除文件夹的问题
        fd = mrc_findStart("",fileName,sizeof(fileName));
        mrc_findStop(fd);
        ret = mrc_rmDir(dirName);
    }
    return ret;
}

//设置mrp的显示名称
int32 mrpSetDisplayName(const char *MrpFile,const char *displayname){
	int32 f = mrc_open(MrpFile,4);
	char *temp = mrc_malloc(22);
	mrc_memset(temp, 0, 22);
	strcpy(temp, displayname);
	if(f!=0){
	mrc_seek(f, 28, SEEK_SET);
	mrc_write(f, temp, 22);
	mrc_close(f);
	mrc_free(temp);	
	}
	return MR_SUCCESS;
}

//向mrp中添加文件
int32 mrpAddFile(const char *MrpFile, char *filename){
	char *cacheDir = "mrpbuilder";
	char *MrpFile_new = mrc_malloc(512);
	char *displayname = NULL;
	strcpy(MrpFile_new, MrpFile);
	strcat(MrpFile_new, ".mrp");
	//清除缓存目录内数据
	removeDir(cacheDir);
	mrc_mkDir(cacheDir);
	//解包
	ExtractAll(MrpFile, cacheDir, 0);
	//打包
    // PackageMrp(mrpinfo, MrpFile, cacheDir, MrpFile_new);
    ToMrp(cacheDir,MrpFile,20480,&mrpinfo, NULL);
	displayname = getDirName(filename);
    mrpSetDisplayName(MrpFile,displayname);
	mrc_free(MrpFile_new);
	mrc_remove(MrpFile);
	mrc_rename(MrpFile_new, MrpFile);
	return MR_SUCCESS;
}

