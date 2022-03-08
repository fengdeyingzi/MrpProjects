#include "mrc_base.h"
#include "tcc.h"
#include "main.h"

#define  PRINTF_NAME  "c/print.txt"
void *o_memcpy(void * s1, const void * s2, int n)
{
	return mrc_memcpy(s1,s2,n);
}

void *o_memmove(void * s1, const void * s2, int n)
{
	return mrc_memmove(s1,s2,n);
}

char *o_strcpy(char * s1, const char * s2)
{
	return mrc_strcpy(s1,s2);
}

char *o_strncpy(char * s1, const char * s2, int n)
{
	return mrc_strncpy(s1,s2,n);
}

char *o_strcat(char * s1, const char * s2)
{
	return mrc_strcat(s1,s2);
}

char *o_strncat(char * s1, const char * s2, int n)
{
	return mrc_strncat(s1,s2,n);
}

int o_memcmp(void * s1, const void * s2, int n)
{
	return mrc_memcmp(s1,s2,n);
}

int o_strcmp(char * s1, const char * s2)
{
	return mrc_strcmp(s1,s2);
}

int o_strncmp(char * s1, const char * s2, int n)
{
	return mrc_strncmp(s1,s2,n);
}

int o_strcoll(const char * s1, const char * s2)
{
	return mrc_strcoll(s1,s2);
}
void *o_memchr(const void * s, int c, int n)
{
	return mrc_memchr(s,c,n);
}

void *o_memset(void * s, int c, int n)
{
	return mrc_memset(s,c,n);
}

int o_strlen(const char * s)
{
	return mrc_strlen(s);
}

char *o_strstr(const char * s1,const char * s2)
{
	return mrc_strstr(s1,s2);
}

int o_atoi(const char * s)
{
	return mrc_atoi(s);
}

unsigned long int o_strtoul(const char * nptr, char ** endptr, int base)
{
	return mrc_strtoul(nptr,endptr,base);
}

int o_sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int n;
	if(!buf)
		return -1;
	va_start(args, fmt);
	n = vsprintf(buf, fmt, args);
	va_end(args);
	buf[n]=0;
	return n;
}

// 输出信息
void printfunc(const char *msg)
{
	int32 fd;

	fd = mrc_open(PRINTF_NAME, MR_FILE_RDWR | MR_FILE_CREATE);
	if (0 != fd)
	{
		mrc_seek(fd, 0, MR_SEEK_END);
		mrc_write(fd, (void*)msg, strlen(msg));
		mrc_close(fd);
	}
}

int o_printf( const char *fmt, ...)
{
	va_list args;
	int n;
	char buf[512];
	va_start(args, fmt);
	n = vsprintf(buf, fmt, args);
	va_end(args);
	printfunc(buf);
	return n;
}
int32 l_base_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	mrc_remove(PRINTF_NAME);
	/********************************C库函数********************************/

	tcc_add_symbol(s,"MR_MEMCPY", (void*)o_memcpy);
	tcc_add_symbol(s,"MR_MEMMOVE", (void*)o_memmove);
	tcc_add_symbol(s,"MR_STRCPY", (void*)o_strcpy);
	tcc_add_symbol(s,"MR_STRNCPY", (void*)o_strncpy);
	tcc_add_symbol(s,"MR_STRCAT", (void*)o_strcat);
	tcc_add_symbol(s,"MR_STRNCAT", (void*)o_strncat);
	tcc_add_symbol(s,"MR_MEMCMP", (void*)o_memcmp);
	tcc_add_symbol(s,"MR_STRCMP", (void*)o_strcmp);
	tcc_add_symbol(s,"MR_STRNCMP", (void*)o_strncmp);
	tcc_add_symbol(s,"MR_STRCOLL", (void*)o_strcoll);
	tcc_add_symbol(s,"MR_MEMCHR", (void*)o_memchr);
	tcc_add_symbol(s,"MR_MEMSET", (void*)o_memset);
	tcc_add_symbol(s,"MR_STRLEN", (void*)o_strlen);
	tcc_add_symbol(s,"MR_STRSTR", (void*)o_strstr);
	//定义在clibrary.c中的函数
	tcc_add_symbol(s,"MR_SPRINTF", (void*)o_sprintf);
	tcc_add_symbol(s,"mrc_printf", (void*)o_printf);
	tcc_add_symbol(s,"mrc_atoi", (void*)o_atoi);
	tcc_add_symbol(s,"mrc_strtoul", (void*)o_strtoul);
	tcc_add_symbol(s,"mrc_malloc", (void*)mrc_malloc );
	tcc_add_symbol(s,"mrc_free", (void*)mrc_free);
	tcc_add_symbol(s,"mrc_strchr", (void*)mrc_strchr);
	tcc_add_symbol(s,"mrc_wstrlen", (void*)mrc_wstrlen);
	/********************************C库函数结束********************************/
	tcc_add_symbol(s,"mrc_getCharBitmap", (void*)mrc_getCharBitmap);
	/********************************文件接口********************************/
	tcc_add_symbol(s,"mrc_ferrno", (void*)mrc_ferrno);
	tcc_add_symbol(s,"mrc_open", (void*)mrc_open);
	tcc_add_symbol(s,"mrc_close", (void*)mrc_close);
	tcc_add_symbol(s,"mrc_fileState", (void*)mrc_fileState);
	tcc_add_symbol(s,"mrc_write", (void*)mrc_write);
	tcc_add_symbol(s,"mrc_read", (void*)mrc_read);
	tcc_add_symbol(s,"mrc_readAll", (void*)mrc_readAll);
	tcc_add_symbol(s,"mrc_seek", (void*)mrc_seek);
	tcc_add_symbol(s,"mrc_getLen", (void*)mrc_getLen);
	tcc_add_symbol(s,"mrc_remove", (void*)mrc_remove);
	tcc_add_symbol(s,"mrc_rename", (void*)mrc_rename);
	tcc_add_symbol(s,"mrc_mkDir", (void*)mrc_mkDir);
	tcc_add_symbol(s,"mrc_rmDir", (void*)mrc_rmDir);
	tcc_add_symbol(s,"mrc_findStart", (void*)mrc_findStart);
	tcc_add_symbol(s,"mrc_findGetNext", (void*)mrc_findGetNext);
	tcc_add_symbol(s,"mrc_findStop", (void*)mrc_findStop);
	/********************************绘图接口********************************/
	tcc_add_symbol(s,"mrc_drawChar", (void*)mrc_drawChar);
	tcc_add_symbol(s,"mrc_drawText", (void*)mrc_drawText);
	tcc_add_symbol(s,"mrc_drawTextEx", (void*)mrc_drawTextEx);
	tcc_add_symbol(s,"mrc_drawTextLeft", (void*)mrc_drawTextLeft);
	tcc_add_symbol(s,"mrc_drawTextRight", (void*)mrc_drawTextRight);
	tcc_add_symbol(s,"mrc_c2u", (void*)mrc_c2u);
	tcc_add_symbol(s,"mrc_c2uVM", (void*)mrc_c2uVM);
	tcc_add_symbol(s,"mrc_textWidthHeight", (void*)mrc_textWidthHeight);
	tcc_add_symbol(s,"mrc_unicodeTextRow", (void*)mrc_unicodeTextRow);
	tcc_add_symbol(s,"mrc_getScreenInfo", (void*)mrc_getScreenInfo);
	tcc_add_symbol(s,"mrc_drawRect", (void*)mrc_drawRect);
	tcc_add_symbol(s,"mrc_drawLine", (void*)mrc_drawLine);
	tcc_add_symbol(s,"mrc_drawPoint", (void*)mrc_drawPoint);
	tcc_add_symbol(s,"mrc_drawPointEx", (void*)mrc_drawPointEx);
	tcc_add_symbol(s,"mrc_clearScreen", (void*)mrc_clearScreen);
	tcc_add_symbol(s,"mrc_refreshScreen", (void*)mrc_refreshScreen);
	tcc_add_symbol(s,"mrc_EffSetCon", (void*)mrc_EffSetCon);
	tcc_add_symbol(s,"mrc_bitmapLoad", (void*)mrc_bitmapLoad);
	tcc_add_symbol(s,"mrc_bitmapShow", (void*)mrc_bitmapShow);
	tcc_add_symbol(s,"mrc_bitmapShowEx", (void*)mrc_bitmapShowEx);
	tcc_add_symbol(s,"mrc_bitmapShowExTrans", (void*)mrc_bitmapShowExTrans);
	tcc_add_symbol(s,"mrc_bitmapNew", (void*)mrc_bitmapNew);
	tcc_add_symbol(s,"mrc_bitmapDraw", (void*)mrc_bitmapDraw);
	tcc_add_symbol(s,"mrc_bitmapGetScreen", (void*)mrc_bitmapGetScreen);
	tcc_add_symbol(s,"mrc_spriteSet", (void*)mrc_spriteSet);
	tcc_add_symbol(s,"mrc_spriteDraw", (void*)mrc_spriteDraw);
	tcc_add_symbol(s,"mrc_spriteDrawEx", (void*)mrc_spriteDrawEx);
	tcc_add_symbol(s,"mrc_spriteCheck", (void*)mrc_spriteCheck);
	tcc_add_symbol(s,"mrc_tileInit", (void*)mrc_tileInit);
	tcc_add_symbol(s,"mrc_tileSetRect", (void*)mrc_tileSetRect);
	tcc_add_symbol(s,"mrc_tileDraw", (void*)mrc_tileDraw);
	tcc_add_symbol(s,"mrc_tileShift", (void*)mrc_tileShift);
	tcc_add_symbol(s,"mrc_tileLoad", (void*)mrc_tileLoad);
	tcc_add_symbol(s,"mrc_tileLoadEx", (void*)mrc_tileLoadEx);
	tcc_add_symbol(s,"mrc_tileGetTile", (void*)mrc_tileGetTile);
	tcc_add_symbol(s,"mrc_tileSetTile", (void*)mrc_tileSetTile);
	/********************************本地化UI接口********************************/
	tcc_add_symbol(s,"mrc_menuCreate", (void*)mrc_menuCreate);
	tcc_add_symbol(s,"mrc_menuSetItem", (void*)mrc_menuSetItem);
	tcc_add_symbol(s,"mrc_menuShow", (void*)mrc_menuShow);
	tcc_add_symbol(s,"mrc_menuRelease", (void*)mrc_menuRelease);
	tcc_add_symbol(s,"mrc_menuRefresh", (void*)mrc_menuRefresh);
	tcc_add_symbol(s,"mrc_dialogCreate", (void*)mrc_dialogCreate);
	tcc_add_symbol(s,"mrc_dialogRelease", (void*)mrc_dialogRelease);
	tcc_add_symbol(s,"mrc_dialogRefresh", (void*)mrc_dialogRefresh);
	tcc_add_symbol(s,"mrc_textCreate", (void*)mrc_textCreate);
	tcc_add_symbol(s,"mrc_textRelease", (void*)mrc_textRelease);
	tcc_add_symbol(s,"mrc_textRefresh", (void*)mrc_textRefresh);
	tcc_add_symbol(s,"mrc_editCreate", (void*)mrc_editCreate);
	tcc_add_symbol(s,"mrc_editRelease", (void*)mrc_editRelease);
	tcc_add_symbol(s,"mrc_editGetText", (void*)mrc_editGetText);
	tcc_add_symbol(s,"mrc_winCreate", (void*)mrc_winCreate);
	tcc_add_symbol(s,"mrc_winRelease", (void*)mrc_winRelease);
	/********************************网络接口********************************/
	tcc_add_symbol(s,"mrc_getNetworkID", (void*)mrc_getNetworkID);
	tcc_add_symbol(s,"mrc_initNetwork", (void*)mrc_initNetwork);
	tcc_add_symbol(s,"mrc_closeNetwork", (void*)mrc_closeNetwork);
	tcc_add_symbol(s,"mrc_getHostByName", (void*)mrc_getHostByName);
	tcc_add_symbol(s,"mrc_socket", (void*)mrc_socket);
	tcc_add_symbol(s,"mrc_connect", (void*)mrc_connect);
	tcc_add_symbol(s,"mrc_getSocketState", (void*)mrc_getSocketState);
	tcc_add_symbol(s,"mrc_closeSocket", (void*)mrc_closeSocket);
	tcc_add_symbol(s,"mrc_recv", (void*)mrc_recv);
	tcc_add_symbol(s,"mrc_recvfrom", (void*)mrc_recvfrom);
	tcc_add_symbol(s,"mrc_send", (void*)mrc_send);
	tcc_add_symbol(s,"mrc_sendto", (void*)mrc_sendto);
	/********************************声音接口********************************/
	tcc_add_symbol(s,"mrc_soundSet", (void*)mrc_soundSet);
	tcc_add_symbol(s,"mrc_soundPlay", (void*)mrc_soundPlay);
	tcc_add_symbol(s,"mrc_soundStop", (void*)mrc_soundStop);
	tcc_add_symbol(s,"mr_playSoundEx", (void*)mr_playSoundEx);
	tcc_add_symbol(s,"mr_stopSoundEx", (void*)mr_stopSoundEx);
	/********************************定时器接口********************************/
	tcc_add_symbol(s,"mrc_timerCreate", (void*)mrc_timerCreate);
	tcc_add_symbol(s,"mrc_timerDelete", (void*)mrc_timerDelete);
	tcc_add_symbol(s,"mrc_timerStop", (void*)mrc_timerStop);
	tcc_add_symbol(s,"mrc_timerStart", (void*)mrc_timerStart);
	tcc_add_symbol(s,"mrc_timerSetTimeEx", (void*)mrc_timerSetTimeEx);
	/********************************其他接口********************************/
	tcc_add_symbol(s,"mrc_exit", (void*)tcc_exitApp);
	tcc_add_symbol(s,"mrc_getUptime", (void*)mrc_getUptime);
	tcc_add_symbol(s,"mrc_getDatetime", (void*)mrc_getDatetime);
	tcc_add_symbol(s,"mrc_getMemoryRemain", (void*)mrc_getMemoryRemain);
	tcc_add_symbol(s,"mrc_startShake", (void*)mrc_startShake);
	tcc_add_symbol(s,"mrc_getVersion", (void*)mrc_getVersion);
	tcc_add_symbol(s,"mrc_getPackName", (void*)mrc_getPackName);
	tcc_add_symbol(s,"mrc_setPackName", (void*)mrc_setPackName);
	tcc_add_symbol(s,"mrc_getSysMem", (void*)mrc_getSysMem);
	/*******************************入口函数**********************************/
	tcc_add_symbol(s,"mrc_readFileFromMrpEx", (void*)mrc_readFileFromMrpEx);
	tcc_add_symbol(s,"mrc_readFileFromMrpExA", (void*)mrc_readFileFromMrpExA);
	tcc_add_symbol(s,"mrc_readFileFromMrpExPart", (void*)mrc_readFileFromMrpExPart);
	tcc_add_symbol(s,"mrc_readFileFromMrp", (void*)mrc_readFileFromMrp);
	tcc_add_symbol(s,"mrc_freeFileData", (void*)mrc_freeFileData);
	tcc_add_symbol(s,"mrc_freeOrigin", (void*)mrc_freeOrigin);
	tcc_add_symbol(s,"mrc_updcrc", (void*)mrc_updcrc);
	tcc_add_symbol(s,"mrc_refreshScreenTrigger", (void*)mrc_refreshScreenTrigger);
	tcc_add_symbol(s,"mrc_refreshScreenUnTrigger", (void*)mrc_refreshScreenUnTrigger);


	tcc_add_symbol(s,"mrc_md5_init", (void*)mrc_md5_init);
	tcc_add_symbol(s,"mrc_md5_append", (void*)mrc_md5_append);
	tcc_add_symbol(s,"mrc_md5_finish", (void*)mrc_md5_finish);
	tcc_add_symbol(s,"mrc_platDrawChar", (void*)mrc_platDrawChar);
	tcc_add_symbol(s,"mrc_exRamInit", (void*)mrc_exRamInit);
	tcc_add_symbol(s,"mrc_exRamRelease", (void*)mrc_exRamRelease);
	tcc_add_symbol(s,"mrc_exRamMalloc", (void*)mrc_exRamMalloc);
	tcc_add_symbol(s,"mrc_exRamFree", (void*)mrc_exRamFree);
	tcc_add_symbol(s,"mrc_exRamMallocOnly", (void*)mrc_exRamMallocOnly);
	tcc_add_symbol(s,"mrc_exRamFreeOnly", (void*)mrc_exRamFreeOnly);
	tcc_add_symbol(s,"mrc_exRamStore", (void*)mrc_exRamStore);
	tcc_add_symbol(s,"mrc_exRamLoad", (void*)mrc_exRamLoad);
	tcc_add_symbol(s,"mrc_getMemStatus", (void*)mrc_getMemStatus);


	return 0;
}