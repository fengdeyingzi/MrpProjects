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
		mrc_memcpy(pPos+strlen(RcName)+1, &NewOffset, 4);

		//�޸���res_lang0.rc��С
		mrc_memcpy(pPos+strlen(RcName)+1+4, &NewRcSize, 4);

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

// ��ȡ�ļ���
 static char* getName(char *path) {
	 int i = 0;
		for ( i = mrc_strlen(path) - 1; i >= 0; i--) {
			if (path[i] == '/' || path[i] == '\\') {
				return path+i + 1;
			}
		}
		return path;
	}

//��Ҫfree
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

//�滻mrp��ָ���ļ�
int32 mrpReplaceFile(const char *MrpFile,char *filename)
{
	int32 fHwnd;
	int32 MrpSize, NameLen, NewOffset;
	uint32 HeadSize,NewRcSize;
	char *HeadData, *RcData, *RcName, *pPos;

	RcName = getName(filename);
	NameLen = 13;

	//����res_lang0.rc�����¶�ȡ
	NewRcSize = mrc_getFileLen(filename);
	RcData = (char*)mrc_readAll(filename, &NewRcSize);
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
		mrc_memcpy(pPos+strlen(RcName)+1, &NewOffset, 4);

		//�޸���res_lang0.rc��С
		mrc_memcpy(pPos+strlen(RcName)+1+4, &NewRcSize, 4);

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
    if(fullName[dirNameLen-1] != '/')//׼��ƴ��Ŀ¼�µ����ļ�����Ŀ¼
    {
        fullName[dirNameLen] = '/';
        dirNameLen++;
    }

    if(mrc_fileState(dirName) == MR_IS_DIR)
    {
        int32 fd;
        int32 result = MR_FAILED;

        fd = mrc_findStart(dirName, fileName, sizeof(fileName));
        if(fd > 0)//���������Ч
        {
            result = MR_SUCCESS;
            while(result == MR_SUCCESS)
            {
                //���fileNameȡֵΪ��.����ʶ������ֵ��Ч����������δ���
                if(fileName[0] && fileName[0] != '.')
                {
                    int32 length = strlen(fileName);
                    memcpy(&fullName[dirNameLen], fileName, length);
                    fullName[dirNameLen+length] = 0;

                    length = mrc_fileState(fullName);
                    if(length == MR_IS_FILE)//�ļ����ͣ�ɾ���ļ�
                        mrc_remove(fullName);                  
                    else if(length == MR_IS_DIR)//�ļ������ͣ��ݹ�ɾ�����ļ���
                        removeDir(fullName);
                }
                memset(fileName, 0, sizeof(fileName));
                result = mrc_findGetNext(fd, fileName, sizeof(fileName));
            }
            mrc_findStop(fd);
        }
        //����һ�μ�����,�������ɾ���ļ��е�����
        fd = mrc_findStart("",fileName,sizeof(fileName));
        mrc_findStop(fd);
        ret = mrc_rmDir(dirName);
    }
    return ret;
}

//����mrp����ʾ����
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

//��mrp������ļ�
int32 mrpAddFile(const char *MrpFile, char *filename){
	char *cacheDir = "mrpbuilder";
	char *MrpFile_new = mrc_malloc(512);
	char *displayname = NULL;
	strcpy(MrpFile_new, MrpFile);
	strcat(MrpFile_new, ".mrp");
	//�������Ŀ¼������
	removeDir(cacheDir);
	mrc_mkDir(cacheDir);
	//���
	ExtractAll(MrpFile, cacheDir, 0);
	//���
    // PackageMrp(mrpinfo, MrpFile, cacheDir, MrpFile_new);
    ToMrp(cacheDir,MrpFile,20480,&mrpinfo, NULL);
	displayname = getDirName(filename);
    mrpSetDisplayName(MrpFile,displayname);
	mrc_free(MrpFile_new);
	mrc_remove(MrpFile);
	mrc_rename(MrpFile_new, MrpFile);
	return MR_SUCCESS;
}

