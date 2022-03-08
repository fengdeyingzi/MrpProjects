#include "haning.h"
#include "interpreter.h"
#include "mrc_base.h"
#include "mrc_exb.h"
#include "mrc_base_i.h"
#include "mrc_bmp.h"
#include "mrc_sound.h"
#include "momo.h"
#include "font.h"
#include "mrc_image.h"



struct timerST{
    int32 timer;
    int32 data;
    char *callback;
    struct timerST* next;
};
struct timerST *timerList=NULL;

int32 my_readAllEx(char* filename, char* buf, int32 *len)
{
	int32 ret,filelen,f,oldlen;

	ret = mrc_fileState(filename);
	if((ret != MR_IS_FILE))
		return MR_FAILED;

	filelen = mrc_getLen(filename);
	if (filelen <= 0)
		return MR_FAILED;
	filelen = (filelen > *len)? *len:filelen;

	f = mrc_open(filename, MR_FILE_RDONLY );
	if (f == 0)
		return MR_FAILED;

	oldlen = 0;
	while(oldlen < filelen){
		ret = mrc_read(f, (char*)buf+oldlen, filelen-oldlen);
		if (ret <= 0)
		{
			mrc_close(f);
			return MR_FAILED;
		}
		oldlen = oldlen + ret;
	}
	mrc_close(f);
	*len = filelen;
	return MR_SUCCESS;
}

int DrawIMG(char* filename, int x,int y)
{
	PMRAPP_IMAGE_WH p;
	MRAPP_IMAGE_ORIGIN_T t_imgsizeInfo;
	T_DRAW_DIRECT_REQ input;

	//获取图片尺寸信息
	t_imgsizeInfo.src = (char*)filename;
	t_imgsizeInfo.len = mrc_strlen(filename);
	t_imgsizeInfo.src_type = SRC_NAME;
	mrc_getImageInfo(&t_imgsizeInfo, (uint8**)&p);

	//设置传到移植层得参数
	input.src_type = 0;
	input.ox = x;
	input.oy = y;
	input.w = p->width;
	input.h = p->height;
	input.src = (char*)filename;
	input.src_len = mrc_strlen(filename);

	return mrc_platEx(3010, (uint8*)&input, sizeof(T_DRAW_DIRECT_REQ), NULL, NULL, NULL);
}

enum
{
	SHADE_UPDOWN,		//从上到下
	SHADE_LEFTRIGHT,	//从左到右
	SHADE_DOWNUP,		//从下到上
	SHADE_RIGHTLEFT		//从右到左
};


void ShadeRect(int x, int y, int w, int h, int AR, int AG, int AB, int BR, int BG, int BB, int mode)
{
	int16 i,j,t;

	BR-=AR; BG-=AG; BB-=AB;
	switch(mode)
	{
	case SHADE_UPDOWN:
		t=x+w-1;
		for(i=0;     i<h;     i++)
			mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*i/h),(uint8)(AG+BG*i/h),(uint8)(AB+BB*i/h));
		return;
	case SHADE_DOWNUP:
		t=x+w-1;
		for(i=h-1,j=0;    i>=0;    i--,j++)
			mrc_drawLine(x, y+i, t, y+i, (uint8)(AR+BR*j/h),(uint8)(AG+BG*j/h),(uint8)(AB+BB*j/h));
		return;
	case SHADE_LEFTRIGHT:
		t=y+h-1;
		for(i=0;     i<w;     i++)
			mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*i/w),(uint8)(AG+BG*i/w),(uint8)(AB+BB*i/w));
		return;
	case SHADE_RIGHTLEFT:
		t=y+h-1;
		for(i=w-1,j=0;    i>=0;    i--,j++)
			mrc_drawLine(x+i, y, x+i, t, (uint8)(AR+BR*j/w),(uint8)(AG+BG*j/w),(uint8)(AB+BB*j/w));
		return;
	}
}

void DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode)
{
	//RGBA+(RGBB-RGBA)*N/Step
	int32 AR,AG,AB;
	int32 BR,BG,BB;

	AR = (uint8)(pixelA >> 16) & 0xff;
	AG = (uint8)(pixelA >> 8) & 0xff;
	AB = (uint8)(pixelA) & 0xff;

	BR = (uint8)((pixelB >> 16) & 0xff);
	BG = (uint8)((pixelB >> 8) & 0xff);
	BB = (uint8)((pixelB) & 0xff);
	ShadeRect(x,y,w,h,AR,AG,AB,BR,BG,BB,mode);

}

void Lib_Malloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_malloc(Param[0]->Val->Integer);
}

void Lib_Free(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_free(Param[0]->Val->Pointer);
}

void Lib_U2c(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_unicodeToGb2312(Param[0]->Val->Pointer,Param[1]->Val->Integer,Param[2]->Val->Pointer,Param[3]->Val->Pointer);
}

void Lib_Memcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_memcpy(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void Lib_Memmove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_memmove(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->UnsignedInteger);
}

void Lib_Strcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strcpy(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_Strncpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strncpy((char *)Param[0]->Val->Pointer, (char *)Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void Lib_Strcat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strcat((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer);
}

void Lib_Strncat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strncat((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_Memcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{   
    ReturnValue->Val->Integer = mrc_memcmp(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_Strcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_strcmp((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer);
}

void Lib_Strncmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{ 
    ReturnValue->Val->Integer = mrc_strncmp((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_Memchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_memchr(Param[0]->Val->Pointer, Param[1]->Val->Character, Param[2]->Val->UnsignedInteger);
}

void Lib_Memset(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_memset(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->UnsignedInteger);
}

void Lib_Strlen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{    
    ReturnValue->Val->UnsignedInteger = mrc_strlen((char *)Param[0]->Val->Pointer);
}

void Lib_Strstr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strstr((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer);
}

void Lib_Atoi(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_atoi((const char*)Param[0]->Val->Pointer);
}

void Lib_Strtoul(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->UnsignedLongInteger = mrc_strtoul((const char*)Param[0]->Val->Pointer, Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_StrrChr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = (void *)mrc_strrchr((char *)Param[0]->Val->Pointer,Param[1]->Val->Integer);
}

int32 re_exit;
void Lib_Set_Exit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	if(Param[0]->Val->LongInteger)
		re_exit=1;
	else
		re_exit=0;
}

void Lib_Exit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	if(re_exit==1)
		PicocParse(" ","exitapp();", strlen("exitapp();"), TRUE, TRUE, FALSE);
    PlatformExit(0);
//     mrc_exit();
}

void Lib_Open(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_open(Param[0]->Val->Pointer,Param[1]->Val->UnsignedLongInteger);
}

void Lib_Close(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_close(Param[0]->Val->LongInteger);
}

void Lib_FileState(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_fileState(Param[0]->Val->Pointer);
}

void Lib_Write(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_write(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->UnsignedLongInteger);
}

void Lib_Read(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_read(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->UnsignedLongInteger);
}

void Lib_ReadAll(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= my_readAllEx(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}

void Lib_Seek(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_seek(Param[0]->Val->LongInteger,Param[1]->Val->LongInteger,Param[2]->Val->Integer);
}

void Lib_GetLen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_getLen((const char*)Param[0]->Val->Pointer);
}

void Lib_Remove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_remove((const char*)Param[0]->Val->Pointer);
}

void Lib_Rename(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_rename((const char*)Param[0]->Val->Pointer, (const char*)Param[1]->Val->Pointer);
}

void Lib_MkDir(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_mkDir(Param[0]->Val->Pointer);
}

void Lib_RmDir(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_rmDir(Param[0]->Val->Pointer);
}

void Lib_FindStart(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_findStart(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->UnsignedLongInteger);
}

void Lib_FindGetNext(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_findGetNext(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->UnsignedLongInteger);
}

void Lib_FindStop(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_findStop(Param[0]->Val->LongInteger);
}

void Lib_DrawTextEx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
//     ReturnValue->Val->LongInteger= mrc_drawTextEx(Param[0]->Val->Pointer,
//         Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, 
//         *(mr_screenRectSt *)&Param[3]->Val->Character,*(mr_colourSt*)&Param[4]->Val->Character, 
//         Param[5]->Val->Integer, Param[6]->Val->Integer);
//如果上面的使用方法存在BUG则使用下面的指针法
	openfont(Param[6]->Val->Integer);
    ReturnValue->Val->LongInteger = gbsez2(Param[0]->Val->Pointer,
        Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, 
        *(mr_screenRectSt*)Param[3]->Val->Pointer,*(mr_colourSt*)Param[4]->Val->Pointer, 
        Param[5]->Val->Integer, Param[6]->Val->Integer);
	closefont();
}

void Lib_DrawTextRight(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_drawTextRight(Param[0]->Val->Pointer, 
        Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, 
        *(mr_screenRectSt*)Param[3]->Val->Pointer,*(mr_colourSt*)Param[4]->Val->Pointer, 
        Param[5]->Val->Integer, Param[6]->Val->Integer);
}

void Lib_C2u(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_c2u((char*)Param[0]->Val->Pointer, (int32*)Param[1]->Val->Pointer, (int32*)Param[2]->Val->Pointer);
}

void Lib_TextWidthHeight(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= fontwh(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->UnsignedShortInteger, (int32*)Param[3]->Val->Pointer, (int32*)Param[4]->Val->Pointer);
}

void Lib_EffSetCon(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_EffSetCon(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger,
        Param[4]->Val->ShortInteger, Param[5]->Val->ShortInteger, Param[6]->Val->ShortInteger);
}

void Lib_WstrLen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_wstrlen(Param[0]->Val->Pointer);
}
void Lib_UnicodeTextRow(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_unicodeTextRow(Param[0]->Val->Pointer,Param[1]->Val->UnsignedShortInteger,Param[2]->Val->LongInteger);
}

void Lib_MenuCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_menuCreate(Param[0]->Val->Pointer,Param[1]->Val->ShortInteger);
}

void Lib_MenuSet(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_menuSetItem(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->LongInteger);
}

void Lib_MenuShow(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_menuShow(Param[0]->Val->LongInteger);
}
void Lib_MenuDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_menuRelease(Param[0]->Val->LongInteger);
}

void Lib_MenuRef(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger=  mrc_menuRefresh(Param[0]->Val->LongInteger);
}

void Lib_DlgCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_dialogCreate(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->LongInteger);
}

void Lib_DlgDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_dialogRelease(Param[0]->Val->LongInteger);
}

void Lib_DlgRef(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_dialogRefresh(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->Pointer,Param[3]->Val->LongInteger);
}

void Lib_TextCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_textCreate(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->LongInteger);
}

void Lib_TextDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_textRelease(Param[0]->Val->LongInteger);
}

void Lib_TextRef(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_textRefresh(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}

void Lib_EditCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_editCreate(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->LongInteger,Param[3]->Val->LongInteger);
}

void Lib_EditDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_editRelease(Param[0]->Val->LongInteger);
}

void Lib_EditGet(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer= (char*)mrc_editGetText(Param[0]->Val->LongInteger);
}

void Lib_GetUptime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_getUptime();
}

void Lib_Sleep(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_sleep(Param[0]->Val->UnsignedLongInteger);
}

void Lib_GetDateTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_getDatetime(Param[0]->Val->Pointer);
}

void Lib_GetMemRemain(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->UnsignedLongInteger= mrc_getMemoryRemain();
}

void Lib_Shake(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_startShake(Param[0]->Val->LongInteger);
}

void Lib_GetSysMem(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_getSysMem();
}

void Lib_RunMrp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_runMrp(Param[0]->Val->Pointer,Param[1]->Val->Pointer,NULL);
}

void Lib_GetParentPath(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	
    ReturnValue->Val->Pointer=NULL;// mrc_GetParentPath();
}

void Lib_Sand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_sand(Param[0]->Val->UnsignedLongInteger);
}

void Lib_Rand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->UnsignedLongInteger= mrc_rand();
}

void Lib_SendSms(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    extern int32 mrc_sendSms(char* pNumber, char*pContent, int32 flags);
    ReturnValue->Val->Integer= mrc_sendSms(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_Lcd(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_LCDCanSleep(Param[0]->Val->Character);
}

void Lib_GetScrBuf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer= w_getScreenBuffer();
}

void Lib_SetScrBuf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    w_setScreenBuffer(Param[0]->Val->Pointer);
}

void Lib_GetScrSize(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_getScreenSize(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_SetScrSize(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_setScreenSize(Param[0]->Val->Integer,Param[1]->Val->Integer);
}

void Lib_BmpShowFlip(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger= mrc_bitmapShowFlip(Param[0]->Val->Pointer,
		Param[1]->Val->ShortInteger,
		Param[2]->Val->ShortInteger,
		Param[3]->Val->UnsignedShortInteger,
		Param[4]->Val->UnsignedShortInteger,
		Param[5]->Val->UnsignedShortInteger,
		Param[6]->Val->UnsignedShortInteger,
		Param[7]->Val->ShortInteger,
		Param[8]->Val->ShortInteger,
		Param[9]->Val->UnsignedShortInteger);
}

void Lib_bitmapShowEx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger= mrc_bitmapShowEx(Param[0]->Val->Pointer,
		Param[1]->Val->ShortInteger,
		Param[2]->Val->ShortInteger,
		Param[3]->Val->UnsignedShortInteger,
		Param[4]->Val->UnsignedShortInteger,
		Param[5]->Val->UnsignedShortInteger,
		Param[6]->Val->UnsignedShortInteger,
		Param[7]->Val->ShortInteger,
		Param[8]->Val->ShortInteger);
}

void Lib_Wap(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    extern void mrc_connectWAP(char* wap);
    mrc_connectWAP(Param[0]->Val->Pointer);
}

void Lib_FreeFileData(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_freeFileData(Param[0]->Val->Pointer,Param[1]->Val->LongInteger);
}

void Lib_FreeOrigin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_freeOrigin(Param[0]->Val->Pointer,Param[1]->Val->LongInteger);
}

void TimerCb(int32 data)//所有定时器的回调,data为使用的定时器
{
    struct timerST *list=timerList;
    char Source[256];
    char *ptr=NULL;

    while(list != NULL)
    {
        if(list->timer == data)
        {
            FuncRet=list->data;//FuncRet就是内部符号_ret
            ptr = list->callback;
            break;
        }
        list=list->next;
    }
    if(CheckBaseCall(ptr,&VoidType,1))
    {
        mrc_timerStop(data);
        return;
    }
    mrc_sprintf(Source,"%s(_ret);",ptr);
    PicocParse(" ", Source, strlen(Source), TRUE, TRUE, FALSE);
}
void Lib_TimerCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    int32 timer;
    timer = mrc_timerCreate();
    if(timer)
    {
        struct timerST *NewNode;
        
        NewNode=mrc_malloc(sizeof(struct timerST));
        NewNode->next=timerList;
        NewNode->timer=timer;
        NewNode->callback=NULL;
        timerList=NewNode;
    }
    ReturnValue->Val->LongInteger = timer;
}

void Lib_TimerDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{   
    int32 timer=Param[0]->Val->LongInteger;
    struct timerST *list=timerList;
    struct timerST *prev=timerList;

    while(list != NULL)
    {
        if(list->timer == timer)
        {
            if(list==timerList)
                timerList=timerList->next;
            else
                prev->next=list->next;
            if(list->callback) mrc_free(list->callback);
            mrc_free(list);
            break;
        }
        prev=list;
        list=list->next;
    }
    mrc_timerDelete(timer);
}

void Lib_TimerStop(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_timerStop(Param[0]->Val->LongInteger);
}

void Lib_TimerStart(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    int32 timer = Param[0]->Val->LongInteger;
    char *callback=Param[3]->Val->Pointer;
    struct timerST *list=timerList;

    while(list != NULL)
    {
        if(list->timer == timer)
        {
            list->data = Param[2]->Val->LongInteger;
            list->callback=mrc_malloc(strlen(callback)+1);
            mrc_strcpy(list->callback,callback);
            break;
        }
        list=list->next;
    }
    ReturnValue->Val->LongInteger= mrc_timerStart(timer,Param[1]->Val->LongInteger,timer,TimerCb,Param[4]->Val->LongInteger);
}

void Lib_TimerSetTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_timerSetTimeEx(Param[0]->Val->LongInteger,Param[1]->Val->LongInteger);
}

void Lib_DrawImg(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer= DrawIMG(Param[0]->Val->Pointer,Param[1]->Val->Integer,Param[2]->Val->Integer);
}

void Lib_ShadeRect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    DrawShadeRect(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer, Param[3]->Val->Integer,Param[4]->Val->LongInteger,Param[5]->Val->LongInteger,Param[6]->Val->Integer);
}
/*
void Lib_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
ReturnValue->Val->LongInteger= mrc_fileState(Param[0]->Val->Pointer);
}

void Lib_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
ReturnValue->Val->LongInteger= mrc_fileState(Param[0]->Val->Pointer);
}
*/
void Lib_ReadFileFromMrp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_readFileFromMrpEx(Param[0]->Val->Pointer,
        Param[1]->Val->Pointer,
        Param[2]->Val->Pointer,
        Param[3]->Val->Pointer,
        Param[4]->Val->LongInteger);
}

void Lib_ClearScreen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_clearScreen(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer);
}

void Lib_refreshScreen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_refreshScreen(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger);
}

void Lib_drawRect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_drawRect(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger,
        Param[4]->Val->Character, Param[5]->Val->Character, Param[6]->Val->Character);
}

void Lib_drawLine(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_drawLine(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger,
        Param[4]->Val->Character, Param[5]->Val->Character, Param[6]->Val->Character);
}

void Lib_DrawPoint(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_drawPoint(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, 
        Param[2]->Val->ShortInteger);
}

void Lib_drawPointEx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_drawPointEx(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, 
        Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger, Param[4]->Val->ShortInteger);
}

void Lib_drawText(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	openfont(Param[7]->Val->UnsignedShortInteger);
    gbsezex((char*)Param[0]->Val->Pointer, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, 
        Param[3]->Val->Character, Param[4]->Val->Character, Param[5]->Val->Character,
        Param[6]->Val->Integer, Param[7]->Val->UnsignedShortInteger);
	closefont();
}

void Lib_ReadMrpFile(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	 ReturnValue->Val->Pointer=(void *)filenei((char*)Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void Lib_BmpShowFlipMrp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	uint32 len;
	uint16 *p=(uint16 *)filenei((char*)Param[0]->Val->Pointer,&len);
	if(p==NULL)
	{
		ReturnValue->Val->LongInteger=-1;
		return;
	}
	ReturnValue->Val->LongInteger= mrc_bitmapShowFlip(p,
		Param[1]->Val->ShortInteger,
		Param[2]->Val->ShortInteger,
		Param[3]->Val->UnsignedShortInteger,
		Param[4]->Val->UnsignedShortInteger,
		Param[5]->Val->UnsignedShortInteger,
		Param[6]->Val->UnsignedShortInteger,
		Param[7]->Val->ShortInteger,
		Param[8]->Val->ShortInteger,
		Param[9]->Val->UnsignedShortInteger);
	free(p);
}
void Lib_BmpShowFlipMrpEx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	uint32 len;
	uint16 *p=(uint16 *)filenei((char*)Param[0]->Val->Pointer,&len);
	if(p==NULL)
	{
		ReturnValue->Val->LongInteger=-1;
		return;
	}
	ReturnValue->Val->LongInteger= mrc_bitmapShowEx(p,
		Param[1]->Val->ShortInteger,
		Param[2]->Val->ShortInteger,
		Param[3]->Val->UnsignedShortInteger,
		Param[4]->Val->UnsignedShortInteger,
		Param[5]->Val->UnsignedShortInteger,
		Param[6]->Val->UnsignedShortInteger,
		Param[7]->Val->ShortInteger,
		Param[8]->Val->ShortInteger);
	free(p);
}

void Lib_PrintScreen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	mrc_remove(Param[0]->Val->Pointer);
	ReturnValue->Val->LongInteger=mrc_PrintScreen((char*)Param[0]->Val->Pointer);
}

void Lib_removeDir(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	ReturnValue->Val->LongInteger=removeDir((char*)Param[0]->Val->Pointer);
}

void Lib_SwitchDisk(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	ReturnValue->Val->LongInteger=SwitchDisk((char*)Param[0]->Val->Pointer);
}




////////////////////接口类///////////////////////////






////////////////////////////////////////////////////////////////////
const  char base_Defs[]= "\
#define _VERSION 1001\r\n\
#define _DEBUG 1\r\n\
#define MR_SUCCESS 0\r\n\
#define MR_FAILED -1\r\n\
#define MR_IGNORE 1\r\n\
#define MR_WAITING 2\r\n\
#define _RDONLY 1\r\n\
#define _WRONLY 2\r\n\
#define _RDWR 4\r\n\
#define _CREATE 8\r\n\
#define _SHARE 16\r\n\
typedef unsigned short uint16;\n\
typedef unsigned long uint32;\n\
typedef long int32;\n\
typedef unsigned char uint8;\n\
typedef char int8;\n\
typedef short int16;\n\
typedef struct{int16 x;int16 y;uint16 w;uint16 h;}rectst;\n\
typedef struct{uint8 r;uint8 g;uint8 b;}colorst;\n\
typedef struct{uint16 year;uint8 month;uint8 day;uint8 hour;uint8 minute;uint8 second;}mr_datetime;\n\
enum{KY_DOWN,KY_UP,MS_DOWN,MS_UP,MN_SLT,MN_RET,MR_DIALOG,MR_LOCALUI,MS_MOVE=12};\n\
enum{_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_STAR,_POUND,_UP,_DOWN,_LEFT,_RIGHT,_POWER,_SLEFT,_SRIGHT,_SEND,_SELECT,_VUP,_VDOWN,_KEY_MAX};\n\
enum{_NET_MOBILE,_NET_CN,_NET_CDMA,_NET_NONE};\
enum{SEEK_SET,SEEK_CUR,SEEK_END};\n\
enum{IS_FILE=1,IS_DIR=2,IS_INVALID=8};\n\
enum{DLG_OK,DLG_CANCEL};\n\
enum{_EDIT_ANY,_EDIT_NUMERIC,_EDIT_PASSWORD};\
enum{FONT_SMALL,FONT_MEDIUM,FONT_BIG};\
enum{BM_OR,BM_XOR,BM_COPY,BM_NOT,BM_MERGENOT,BM_ANDNOT,BM_TRANS,BM_AND,BM_GRAY,BM_REVERSE};";





int32 Base_SetupFunc(void)
{
	Haninit(&base_Defs[0]);
	fontinit();

	re_exit=0;

	AddString(Lib_Malloc, "void* malloc(int);");
	AddString(Lib_Free,"void free(void*);");
	AddString(Lib_U2c,"int u2c(char*,int,char**,int*);");
	AddString(Lib_Memcpy,"void* memcpy(void*,void*,uint32);");
	AddString(Lib_Memmove,"void* memmove(void *,void*,uint32);");
	AddString(Lib_Strcpy,"char* strcpy(char*,char*);");
	AddString(Lib_Strncpy,"char* strncpy(char*,char*,int);");
	AddString(Lib_Strcat,"char* strcat(char*,char*);");
	AddString(Lib_Strncat,"char* strncat(char*,char*,int);");
	AddString(Lib_Memcmp,"int memcmp(void*,void*,uint32);");
	AddString(Lib_Strcmp,"int strcmp(char*,char*);");
	AddString(Lib_Strncmp,"int strncmp(char*,char*,int);");
	AddString(Lib_Memchr,"void *memchr(void*,char,uint32);");
	AddString(Lib_Memset,"void *memset(void*,int,uint32);");
	AddString(Lib_Strlen,"uint32 strlen(char*);");
	AddString(Lib_Strstr,"char* strstr(char*,char*);");
//定义在clibrary.c中的函数
	AddString(LibPrintf,"void printf(char*,...);");
	AddString(Lib_Atoi,"int atoi(char*);");
	AddString(Lib_Strtoul,"long strtoul(char*,char**,int);");
	AddString(Lib_StrrChr,"char *strrchr(char*,char);");
	AddString(Lib_Set_Exit,"void set_exit(int32);");
	AddString(Lib_Exit,"void exit();");
	AddString(Lib_ClearScreen,"void cls(int,int,int);");
	AddString(Lib_refreshScreen,"void ref(int,int,int,int);");
	AddString(Lib_drawRect,"void drect(int,int,int,int,int,int,int);");
	AddString(Lib_drawLine,"void dline(int,int,int,int,int,int,int);");
	AddString(Lib_drawPointEx,"void dpointex(int,int,int,int,int);");
	AddString(Lib_drawText,"int dtext(char*,int,int,int,int,int,int,int);");
//定义在clibrary.c中的函数
	AddString(LibSPrintf,"char *sprintf(char*,char*,...);");
	AddString(Lib_Open,"int open(char*,int);");
	AddString(Lib_Close,"int close(int);");
	AddString(Lib_FileState,"int filestate(char*);");
	AddString(Lib_Write,"int write(int,void*,int);");
	AddString(Lib_Read,"int read(int,void*,int);");
	AddString(Lib_ReadAll,"int readall(char*,char*,int*);");
	AddString(Lib_Seek,"int seek(int,int,int);");
	AddString(Lib_GetLen,"int getlen(char*);");
	AddString(Lib_Remove,"int remove(char*);");
	AddString(Lib_Rename,"int rename(char*,char*);");
	AddString(Lib_MkDir,"int mkdir(char*);");
	AddString(Lib_RmDir,"int rmdir(char*);");
	AddString(Lib_FindStart,"int findstart(char*,char*,int);");
	AddString(Lib_FindGetNext,"int findnext(int,char*,int);");
	AddString(Lib_FindStop,"int findstop(int);");
	AddString(Lib_DrawTextEx,"int dtextex(char*,short,short,rectst*,colorst*,int,short);");
	AddString(Lib_DrawTextRight,"int dtextright(char*,short,short,rectst*,colorst*,int,short);");
	AddString(Lib_C2u,"short* c2u(char*,int*,int*);");
	AddString(Lib_TextWidthHeight,"int textwh(char*,int,int,int*,int*);");
	AddString(Lib_DrawPoint,"void dpoint(int,int,int);");
	AddString(Lib_EffSetCon,"void effsetcon(int,int,int,int,int,int,int);");
	AddString(Lib_WstrLen,"int wstrlen(char*);");
	AddString(Lib_UnicodeTextRow,"int unitextrow(uint16*,int,int);");
	AddString(Lib_MenuCreate,"int menucreate(char*, int16);");
	AddString(Lib_MenuSet,"int menuset(int,char*,int);");
	AddString(Lib_MenuShow,"int menushow(int);");
	AddString(Lib_MenuDel,"int menudel(int);");
	AddString(Lib_MenuRef,"int menuref(int);");
	AddString(Lib_DlgCreate,"int dlgcreate(char*,char*,int);");
	AddString(Lib_DlgDel,"int dlgdel(int);");
	AddString(Lib_DlgRef,"int dlgref(int,char*,char*,int);");
	AddString(Lib_TextCreate,"int textcreate(char*,char*,int);");
	AddString(Lib_TextDel,"int textdel(int);");
	AddString(Lib_TextRef,"int textref(int,char*,char*);");
	AddString(Lib_EditCreate,"int editcreate(char*,char*,int,int);");
	AddString(Lib_EditDel,"int editdel(int);");
	AddString(Lib_EditGet,"char* editget(int);");
	AddString(Lib_GetUptime,"int getuptime();");
	AddString(Lib_Sleep,"int sleep(uint32);");
	AddString(Lib_GetDateTime,"int getdatetime(void*);");
	AddString(Lib_GetMemRemain,"uint32 getmemremain();");
	AddString(Lib_Shake,"int shake(int);");
	AddString(Lib_GetSysMem,"int getsysmem();");
	AddString(Lib_RunMrp,"int runmrp(char*,char*);");
	AddString(Lib_GetParentPath,"char* getparentpath();");
	AddString(Lib_Sand,"void sand(uint32);");
	AddString(Lib_Rand,"uint32 rand();");
	AddString(Lib_SendSms,"int sms(char*,char*,int);");
	AddString(Lib_Lcd,"int lcd(char);");
	AddString(Lib_GetScrBuf,"uint16 *getscrbuf();");
	AddString(Lib_SetScrBuf,"void setscrbuf(uint16*);");
	AddString(Lib_GetScrSize,"void getscrsize(int*,int*);");
	AddString(Lib_SetScrSize,"void setscrsize(int,int);");
	AddString(Lib_BmpShowFlip,"int bmpshowflip(uint16*,int16,int16,uint16,uint16,uint16,uint16,int16,int16,uint16);");
	AddString(Lib_bitmapShowEx,"int bitmapshowex(uint16*,int16,int16,uint16,uint16,uint16,uint16,int16,int16);");
	AddString(Lib_Wap,"void wap(char*);");
	AddString(Lib_ReadFileFromMrp,"int readfilefrommrp(char*,char*,uint8**,long*,int);");
	AddString(Lib_FreeFileData,"void freefiledata(void*,int);");
	AddString(Lib_FreeOrigin,"void freeorigin(void*,int);");
	AddString(Lib_TimerCreate,"long timercreate();");
	AddString(Lib_TimerDel,"void timerdel(long);");
	AddString(Lib_TimerStop,"void timerstop(long);");
	AddString(Lib_TimerStart,"int timerstart(long,long,long,char*,int);");
	AddString(Lib_TimerSetTime,"int timersettime(long,long);");
	AddString(Lib_DrawImg,"int img(char*,int,int);");
	AddString(Lib_ShadeRect,"void shaderect(int,int,int,int,long,long,int);");
	AddString(Lib_ReadMrpFile,"void* readmrpfile(char*,uint32*);");
	AddString(Lib_BmpShowFlipMrp,"int32 bmpshowflipmrp(char*,int16,int16,uint16,uint16,uint16,uint16,int16,int16,uint16);");
	AddString(Lib_BmpShowFlipMrpEx	,"int32 bmpshowflipmrpex(char*,int16,int16,uint16,uint16,uint16,uint16,int16,int16);");
	AddString(Lib_PrintScreen	,"int32 printscreen(char*);");
	AddString(Lib_removeDir	,"int32 removedir(char*);");
	AddString(Lib_SwitchDisk	,"int32 switchdisk(char*);");

	return hanclose();

}

