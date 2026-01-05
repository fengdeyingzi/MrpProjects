#include "base.h"
#include "xl_bmp.h"


char *assets_dir;

//extern int32 mrc_getLen(const char* filename);

 char* getAssetsDir(void){
	return assets_dir;
}
 void setAssetsDir(char *dir){
	assets_dir = dir;
}

#ifdef C_RUN
void *mrc_readFileFromAssets(const char *filename, int32 *len){
 int32 f = 0;
 char path[300];
 void *buf = NULL;
 sprintf(path,"%s/%s",getAssetsDir(),filename);
//  debug_printf("¶ÁÈ¡assets");
//  debug_printf(path);
 *len = (int32)getlen((const char*)path);
 buf = (void*)malloc(*len);
 f = mrc_open(path, 1);
 if(f>0){
 mrc_read(f, buf, *len);
 mrc_close(f);
 return buf;
 }
 return NULL;
}

void mrc_freeFileFromAssets(void *data,int32 len){
	free(data);
}

#else

void *mrc_readFileFromAssets(const char *filename, int32 *len){

//  re = mrc_readFileFromMrpEx(NULL,(const char*)filename,&buf,len,0);
 void* temp = mrc_readFileFromMrp((const char*)filename, len,0);



 if(temp){
	 return temp;
 }
 return NULL;
}

void mrc_freeFileFromAssets(void *data,int32 len){
	mrc_freeFileData(data, len);
}
#endif