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
struct timerST *mr_timerList=NULL;

int32 mr_my_readAllEx(char* filename, char* buf, int32 *len)
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

int mr_DrawIMG(char* filename, int x,int y)
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


void mr_ShadeRect(int x, int y, int w, int h, int AR, int AG, int AB, int BR, int BG, int BB, int mode)
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

void mr_DrawShadeRect(int16 x, int16 y, int16 w, int16 h, uint32 pixelA, uint32 pixelB, int8 mode)
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
	mr_ShadeRect(x,y,w,h,AR,AG,AB,BR,BG,BB,mode);

}

void Lib_mr_Malloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_malloc(Param[0]->Val->Integer);
}

void Lib_mr_Free(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_free(Param[0]->Val->Pointer);
}

void Lib_mr_U2c(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_unicodeToGb2312(Param[0]->Val->Pointer,Param[1]->Val->Integer,Param[2]->Val->Pointer,Param[3]->Val->Pointer);
}

void Lib_mr_Memcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_memcpy(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void Lib_mr_Memmove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_memmove(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->UnsignedInteger);
}

void Lib_mr_Strcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strcpy(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_mr_Strncpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strncpy((char *)Param[0]->Val->Pointer, (char *)Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void Lib_mr_Strchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = mrc_strchr((const char *)Param[0]->Val->Pointer,Param[1]->Val->Integer);
}

void Lib_mr_Strcat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = mrc_strcat((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer);
}

void Lib_mr_Strncat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strncat((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_mr_Memcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{   
    ReturnValue->Val->Integer = mrc_memcmp(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_mr_Strcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_strcmp((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer);
}

void Lib_mr_Strncmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{ 
    ReturnValue->Val->Integer = mrc_strncmp((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_mr_Memchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_memchr(Param[0]->Val->Pointer, Param[1]->Val->Character, Param[2]->Val->UnsignedInteger);
}

void Lib_mr_Memset(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_memset(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->UnsignedInteger);
}

void Lib_mr_Strlen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{    
    ReturnValue->Val->UnsignedInteger = mrc_strlen((char *)Param[0]->Val->Pointer);
}

void Lib_mr_Strstr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_strstr((char *)Param[0]->Val->Pointer,(char *)Param[1]->Val->Pointer);
}

void Lib_mr_Atoi(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = mrc_atoi((const char*)Param[0]->Val->Pointer);
}

void Lib_mr_Strtoul(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->UnsignedLongInteger = mrc_strtoul((const char*)Param[0]->Val->Pointer, Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_mr_StrrChr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = (void *)mrc_strrchr((char *)Param[0]->Val->Pointer,Param[1]->Val->Integer);
}

int32 mre_exit;
void Lib_mr_Set_Exit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	if(Param[0]->Val->LongInteger)
		mre_exit=1;
	else
		mre_exit=0;
}

void Lib_mr_Exit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	if(mre_exit==1)
		PicocParse(" ","exitapp();", strlen("exitapp();"), TRUE, TRUE, FALSE);
    PlatformExit(0);
//     mrc_exit();
}

void Lib_mr_Open(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_open(Param[0]->Val->Pointer,Param[1]->Val->UnsignedLongInteger);
}

void Lib_mr_Close(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_close(Param[0]->Val->LongInteger);
}

void Lib_mr_FileState(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_fileState(Param[0]->Val->Pointer);
}

void Lib_mr_Write(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_write(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->UnsignedLongInteger);
}

void Lib_mr_Read(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_read(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->UnsignedLongInteger);
}

void Lib_mr_ReadAll(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mr_my_readAllEx(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}

void Lib_mr_Seek(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_seek(Param[0]->Val->LongInteger,Param[1]->Val->LongInteger,Param[2]->Val->Integer);
}

void Lib_mr_GetLen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_getLen((const char*)Param[0]->Val->Pointer);
}

void Lib_mr_Remove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_remove((const char*)Param[0]->Val->Pointer);
}

void Lib_mr_Rename(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_rename((const char*)Param[0]->Val->Pointer, (const char*)Param[1]->Val->Pointer);
}

void Lib_mr_MkDir(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_mkDir(Param[0]->Val->Pointer);
}

void Lib_mr_RmDir(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_rmDir(Param[0]->Val->Pointer);
}

void Lib_mr_FindStart(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_findStart(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->UnsignedLongInteger);
}

void Lib_mr_FindGetNext(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_findGetNext(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->UnsignedLongInteger);
}

void Lib_mr_FindStop(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_findStop(Param[0]->Val->LongInteger);
}

void Lib_mr_DrawTextEx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
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

void Lib_mr_DrawTextRight(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_drawTextRight(Param[0]->Val->Pointer, 
        Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, 
        *(mr_screenRectSt*)Param[3]->Val->Pointer,*(mr_colourSt*)Param[4]->Val->Pointer, 
        Param[5]->Val->Integer, Param[6]->Val->Integer);
}

void Lib_mr_C2u(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = mrc_c2u((char*)Param[0]->Val->Pointer, (int32*)Param[1]->Val->Pointer, (int32*)Param[2]->Val->Pointer);
}

void Lib_mr_TextWidthHeight(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= fontwh(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->UnsignedShortInteger, (int32*)Param[3]->Val->Pointer, (int32*)Param[4]->Val->Pointer);
}

void Lib_mr_EffSetCon(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_EffSetCon(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger,
        Param[4]->Val->ShortInteger, Param[5]->Val->ShortInteger, Param[6]->Val->ShortInteger);
}

void Lib_mr_WstrLen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_wstrlen(Param[0]->Val->Pointer);
}
void Lib_mr_UnicodeTextRow(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_unicodeTextRow(Param[0]->Val->Pointer,Param[1]->Val->UnsignedShortInteger,Param[2]->Val->LongInteger);
}

void Lib_mr_MenuCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger = mrc_menuCreate(Param[0]->Val->Pointer,Param[1]->Val->ShortInteger);
}

void Lib_mr_MenuSet(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_menuSetItem(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->LongInteger);
}

void Lib_mr_MenuShow(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_menuShow(Param[0]->Val->LongInteger);
}
void Lib_mr_MenuDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_menuRelease(Param[0]->Val->LongInteger);
}

void Lib_mr_MenuRef(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger=  mrc_menuRefresh(Param[0]->Val->LongInteger);
}

void Lib_mr_DlgCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_dialogCreate(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->LongInteger);
}

void Lib_mr_DlgDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_dialogRelease(Param[0]->Val->LongInteger);
}

void Lib_mr_DlgRef(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_dialogRefresh(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->Pointer,Param[3]->Val->LongInteger);
}

void Lib_mr_TextCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_textCreate(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->LongInteger);
}

void Lib_mr_TextDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_textRelease(Param[0]->Val->LongInteger);
}

void Lib_mr_TextRef(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_textRefresh(Param[0]->Val->LongInteger,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}

void Lib_mr_EditCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_editCreate(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->LongInteger,Param[3]->Val->LongInteger);
}

void Lib_mr_EditDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_editRelease(Param[0]->Val->LongInteger);
}

void Lib_mr_EditGet(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer= (char*)mrc_editGetText(Param[0]->Val->LongInteger);
}

void Lib_mr_GetUptime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_getUptime();
}

void Lib_mr_Sleep(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_sleep(Param[0]->Val->UnsignedLongInteger);
}

void Lib_mr_GetDateTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_getDatetime(Param[0]->Val->Pointer);
}

void Lib_mr_GetMemRemain(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->UnsignedLongInteger= mrc_getMemoryRemain();
}

void Lib_mr_Shake(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_startShake(Param[0]->Val->LongInteger);
}

void Lib_mr_GetSysMem(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_getSysMem();
}

void Lib_mr_RunMrp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_runMrp(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}

void Lib_mr_GetParentPath(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	
    ReturnValue->Val->Pointer=NULL;// mrc_GetParentPath();
}

void Lib_mr_Sand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_sand(Param[0]->Val->UnsignedLongInteger);
}

void Lib_mr_Rand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->UnsignedLongInteger= mrc_rand();
}

void Lib_mr_SendSms(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    extern int32 mrc_sendSms(char* pNumber, char*pContent, int32 flags);
    ReturnValue->Val->Integer= mrc_sendSms(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Integer);
}

void Lib_mr_Lcd(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_LCDCanSleep(Param[0]->Val->Character);
}

void Lib_mr_GetScrBuf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer= w_getScreenBuffer();
}

void Lib_mr_getScreenInfo(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	 ReturnValue->Val->Integer=mrc_getScreenInfo(Param[0]->Val->Pointer);
}

void Lib_mr_SetScrBuf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	w_setScreenBuffer(Param[0]->Val->Pointer);
}

void Lib_mr_GetScrSize(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_getScreenSize(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_mr_SetScrSize(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_setScreenSize(Param[0]->Val->Integer,Param[1]->Val->Integer);
}

void Lib_mr_BmpShowFlip(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
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

void Lib_mr_bitmapShowEx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
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

void Lib_mr_Wap(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    extern void mrc_connectWAP(char* wap);
    mrc_connectWAP(Param[0]->Val->Pointer);
}

void Lib_mr_FreeFileData(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_freeFileData(Param[0]->Val->Pointer,Param[1]->Val->LongInteger);
}

void Lib_mr_FreeOrigin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_freeOrigin(Param[0]->Val->Pointer,Param[1]->Val->LongInteger);
}

void mr_TimerCb(int32 data)//所有定时器的回调,data为使用的定时器
{
    struct timerST *list=mr_timerList;
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
void Lib_mr_TimerCreate(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    int32 timer;
    timer = mrc_timerCreate();
    if(timer)
    {
        struct timerST *NewNode;
        
        NewNode=mrc_malloc(sizeof(struct timerST));
        NewNode->next=mr_timerList;
        NewNode->timer=timer;
        NewNode->callback=NULL;
        mr_timerList=NewNode;
    }
    ReturnValue->Val->LongInteger = timer;
}

void Lib_mr_TimerDel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{   
    int32 timer=Param[0]->Val->LongInteger;
    struct timerST *list=mr_timerList;
    struct timerST *prev=mr_timerList;

    while(list != NULL)
    {
        if(list->timer == timer)
        {
            if(list==mr_timerList)
                mr_timerList=mr_timerList->next;
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

void Lib_mr_TimerStop(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mrc_timerStop(Param[0]->Val->LongInteger);
}

void Lib_mr_TimerStart(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    int32 timer = Param[0]->Val->LongInteger;
    char *callback=Param[3]->Val->Pointer;
    struct timerST *list=mr_timerList;

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
    ReturnValue->Val->LongInteger= mrc_timerStart(timer,Param[1]->Val->LongInteger,timer,mr_TimerCb,Param[4]->Val->LongInteger);
}

void Lib_mr_TimerSetTime(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_timerSetTimeEx(Param[0]->Val->LongInteger,Param[1]->Val->LongInteger);
}

void Lib_mr_ShadeRect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    mr_DrawShadeRect(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer, Param[3]->Val->Integer,Param[4]->Val->LongInteger,Param[5]->Val->LongInteger,Param[6]->Val->Integer);
}
/*
void Lib_mr_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
ReturnValue->Val->LongInteger= mrc_fileState(Param[0]->Val->Pointer);
}

void Lib_mr_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
ReturnValue->Val->LongInteger= mrc_fileState(Param[0]->Val->Pointer);
}
*/
void Lib_mr_ReadFileFromMrp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->LongInteger= mrc_readFileFromMrpEx(Param[0]->Val->Pointer,
        Param[1]->Val->Pointer,
        Param[2]->Val->Pointer,
        Param[3]->Val->Pointer,
        Param[4]->Val->LongInteger);
}

void Lib_mr_ClearScreen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_clearScreen(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer);
}

void Lib_mr_refreshScreen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_refreshScreen(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger);
}

void Lib_mr_drawRect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_drawRect(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger,
        Param[4]->Val->Character, Param[5]->Val->Character, Param[6]->Val->Character);
}

void Lib_mr_drawLine(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_drawLine(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger,
        Param[4]->Val->Character, Param[5]->Val->Character, Param[6]->Val->Character);
}

void Lib_mr_DrawPoint(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_drawPoint(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, 
        Param[2]->Val->ShortInteger);
}

void Lib_mr_drawPointEx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    mrc_drawPointEx(Param[0]->Val->ShortInteger, Param[1]->Val->ShortInteger, 
        Param[2]->Val->ShortInteger, Param[3]->Val->ShortInteger, Param[4]->Val->ShortInteger);
}

void Lib_mr_drawText(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	openfont(Param[7]->Val->UnsignedShortInteger);
    gbsezex((char*)Param[0]->Val->Pointer, Param[1]->Val->ShortInteger, Param[2]->Val->ShortInteger, 
        Param[3]->Val->Character, Param[4]->Val->Character, Param[5]->Val->Character,
        Param[6]->Val->Integer, Param[7]->Val->UnsignedShortInteger);
	closefont();
}

void Lib_mr_ReadMrpFile(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
	 ReturnValue->Val->Pointer=(void *)filenei((char*)Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void Lib_mr_BmpShowFlipMrp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
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
void Lib_mr_BmpShowFlipMrpEx(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
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





////////////////////接口类///////////////////////////






////////////////////////////////////////////////////////////////////
const  char mrc_base_Defs[]= "\
#include <base.h>\n\
#define MR_FILE_RDONLY 1\r\n\
#define MR_FILE_WRONLY 2\r\n\
#define MR_FILE_RDWR 4\r\n\
#define MR_FILE_CREATE 8\r\n\
#define MR_FILE_RECREATE 16\r\n\
typedef struct{int16 x;int16 y;uint16 w;uint16 h;}mr_screenRectSt;\n\
typedef struct{uint8 r;uint8 g;uint8 b;}mr_colourSt;\n\
enum {MR_KEY_PRESS,MR_KEY_RELEASE,MR_MOUSE_DOWN,MR_MOUSE_UP,MR_MENU_SELECT,MR_MENU_RETURN,MR_DIALOG_EVENT,MR_EVENT01,MR_MOUSE_MOVE=12};\
enum {MR_KEY_0,MR_KEY_1,MR_KEY_2,MR_KEY_3,MR_KEY_4,MR_KEY_5,MR_KEY_6,MR_KEY_7,MR_KEY_8,MR_KEY_9,MR_KEY_STAR,MR_KEY_POUND,\
MR_KEY_UP,MR_KEY_DOWN,MR_KEY_LEFT,MR_KEY_RIGHT,MR_KEY_POWER,MR_KEY_SOFTLEFT,MR_KEY_SOFTRIGHT,MR_KEY_SEND,MR_KEY_SELECT,MR_KEY_VOLUME_UP,MR_KEY_VOLUME_DOWN,MR_KEY_NONE};\
enum {MR_NET_ID_MOBILE,MR_NET_ID_CN,MR_NET_ID_CDMA,MR_NET_ID_NONE};\
enum{MR_SEEK_SET,MR_SEEK_CUR,MR_SEEK_END};\
enum{MR_IS_FILE=1,MR_IS_DIR=2,MR_IS_INVALID=8};\n\
enum{MR_DIALOG_OK,MR_DIALOG_OK_CANCEL,MR_DIALOG_CANCEL};\
enum{MR_DIALOG_KEY_OK,MR_DIALOG_KEY_CANCEL};\n\
enum{MR_EDIT_ANY,MR_EDIT_NUMERIC,MR_EDIT_PASSWORD};\
enum{MR_FONT_SMALL,MR_FONT_MEDIUM,MR_FONT_BIG};\
enum{BM_TRANSPARENT=6};\
typedef struct{uint32 width;uint32 height;uint32 bit;}mr_screeninfo;";






int32 mr_Base_SetupFunc(void)
{
	Haninit(&mrc_base_Defs[0]);
	fontinit();

	mre_exit=0;

	AddString(Lib_mr_Malloc, "void* mrc_malloc(int);");
	AddString(Lib_mr_Free,"void mrc_free(void*);");
	AddString(Lib_mr_U2c,"int mrc_unicodeToGb2312(char*,int,char**,int*);");
	AddString(Lib_mr_Memcpy,"void* mrc_memcpy(void*,void*,uint32);");
	AddString(Lib_mr_Memmove,"void* mrc_memmove(void *,void*,uint32);");
	AddString(Lib_mr_Strcpy,"char* mrc_strcpy(char*,char*);");
	AddString(Lib_mr_Strncpy,"char* mrc_strncpy(char*,char*,int);");
	AddString(Lib_mr_Strchr,"char* mrc_strchr(char*,int);");
	AddString(Lib_mr_Strcat,"char* mrc_strcat(char*,char*);");
	AddString(Lib_mr_Strncat,"char* mrc_strncat(char*,char*,int);");
	AddString(Lib_mr_Memcmp,"int mrc_memcmp(void*,void*,uint32);");
	AddString(Lib_mr_Strcmp,"int mrc_strcmp(char*,char*);");
	AddString(Lib_mr_Strncmp,"int mrc_strncmp(char*,char*,int);");
	AddString(Lib_mr_Memchr,"void *mrc_memchr(void*,char,uint32);");
	AddString(Lib_mr_Memset,"void *mrc_memset(void*,int,uint32);");
	AddString(Lib_mr_Strlen,"uint32 mrc_strlen(char*);");
	AddString(Lib_mr_Strstr,"char* mrc_strstr(char*,char*);");
//定义在clibrary.c中的函数
	AddString(LibPrintf,"void mrc_printf(char*,...);");
	AddString(Lib_mr_Atoi,"int mrc_atoi(char*);");
	AddString(Lib_mr_Strtoul,"long mrc_strtoul(char*,char**,int);");
	AddString(Lib_mr_StrrChr,"char *mrc_strrchr(char*,char);");
	AddString(Lib_mr_Set_Exit,"void mrc_set_exit(int32);");
	AddString(Lib_mr_Exit,"void mrc_exit();");
	AddString(Lib_mr_ClearScreen,"void mrc_clearScreen(int,int,int);");
	AddString(Lib_mr_refreshScreen,"void mrc_refreshScreen(int,int,int,int);");
	AddString(Lib_mr_drawRect,"void mrc_drawRect(int,int,int,int,int,int,int);");
	AddString(Lib_mr_drawLine,"void mrc_drawLine(int,int,int,int,int,int,int);");
	AddString(Lib_mr_drawPointEx,"void mrc_drawPointEx(int,int,int,int,int);");
	AddString(Lib_mr_drawText,"int mrc_drawText(char*,int,int,int,int,int,int,int);");
//定义在clibrary.c中的函数
	AddString(LibSPrintf,"char *mrc_sprintf(char*,char*,...);");
	AddString(Lib_mr_Open,"int mrc_open(char*,int);");
	AddString(Lib_mr_Close,"int mrc_close(int);");
	AddString(Lib_mr_FileState,"int mrc_fileState(char*);");
	AddString(Lib_mr_Write,"int mrc_write(int,void*,int);");
	AddString(Lib_mr_Read,"int mrc_read(int,void*,int);");
	AddString(Lib_mr_ReadAll,"int mrc_readAll(char*,char*,int*);");
	AddString(Lib_mr_Seek,"int mrc_seek(int,int,int);");
	AddString(Lib_mr_GetLen,"int mrc_getLen(char*);");
	AddString(Lib_mr_Remove,"int mrc_remove(char*);");
	AddString(Lib_mr_Rename,"int mrc_rename(char*,char*);");
	AddString(Lib_mr_MkDir,"int mrc_mkDir(char*);");
	AddString(Lib_mr_RmDir,"int mrc_rmDir(char*);");
	AddString(Lib_mr_FindStart,"int mrc_findStart(char*,char*,int);");
	AddString(Lib_mr_FindGetNext,"int mrc_findGetNext(int,char*,int);");
	AddString(Lib_mr_FindStop,"int mrc_findStop(int);");
	AddString(Lib_mr_DrawTextEx,"int mrc_drawTextEx(char*,short,short,mr_screenRectSt*, mr_colourSt*,int,short);");
	AddString(Lib_mr_DrawTextRight,"int mrc_drawTextRight(char*,short,short,mr_screenRectSt*, mr_colourSt*,int,short);");
	AddString(Lib_mr_C2u,"short* mrc_c2u(char*,int*,int*);");
	AddString(Lib_mr_TextWidthHeight,"int mrc_textWidthHeight(char*,int,int,int*,int*);");
	AddString(Lib_mr_DrawPoint,"void mrc_drawPoint(int,int,int);");
	AddString(Lib_mr_EffSetCon,"void mrc_EffSetCon(int,int,int,int,int,int,int);");
	AddString(Lib_mr_WstrLen,"int mrc_wstrlen(char*);");
	AddString(Lib_mr_UnicodeTextRow,"int mrc_unicodeTextRow(uint16*,int,int);");
	AddString(Lib_mr_MenuCreate,"int mrc_menuCreate(char*, int16);");
	AddString(Lib_mr_MenuSet,"int mrc_menuSetItem(int,char*,int);");
	AddString(Lib_mr_MenuShow,"int mrc_menuShow(int);");
	AddString(Lib_mr_MenuDel,"int mrc_menuRelease(int);");
	AddString(Lib_mr_MenuRef,"int mrc_menuRefresh(int);");
	AddString(Lib_mr_DlgCreate,"int mrc_dialogCreate(char*,char*,int);");
	AddString(Lib_mr_DlgDel,"int mrc_dialogRelease(int);");
	AddString(Lib_mr_DlgRef,"int mrc_dialogRefresh(int,char*,char*,int);");
	AddString(Lib_mr_TextCreate,"int mrc_textCreate(char*,char*,int);");
	AddString(Lib_mr_TextDel,"int mrc_textRelease(int);");
	AddString(Lib_mr_TextRef,"int mrc_textRefresh(int,char*,char*);");
	AddString(Lib_mr_EditCreate,"int mrc_editCreate(char*,char*,int,int);");
	AddString(Lib_mr_EditDel,"int mrc_editRelease(int);");
	AddString(Lib_mr_EditGet,"char* mrc_editGetText(int);");
	AddString(Lib_mr_GetUptime,"int mrc_getUptime();");
	AddString(Lib_mr_Sleep,"int mrc_sleep(uint32);");
	AddString(Lib_mr_GetDateTime,"int mrc_getDatetime(void*);");
	AddString(Lib_mr_GetMemRemain,"uint32 mrc_getMemoryRemain();");
	AddString(Lib_mr_Shake,"int mrc_startShake(int);");
	AddString(Lib_mr_GetSysMem,"int mrc_getSysMem();");
	AddString(Lib_mr_RunMrp,"int mrc_runMrp(char*,char*,char*);"); 
	//AddString(Lib_mr_GetParentPath,"char* getparentpath();");
	AddString(Lib_mr_Sand,"void mrc_sand(uint32);");
	AddString(Lib_mr_Rand,"uint32 mrc_rand();");
	AddString(Lib_mr_SendSms,"int mrc_sendSms(char*,char*,int);");
	AddString(Lib_mr_Lcd,"int mrc_LCDCanSleep(char);");
	AddString(Lib_mr_getScreenInfo,"int32 mrc_getScreenInfo(void*);");
	AddString(Lib_mr_GetScrBuf,"uint16 *w_getScreenBuffer();");
	AddString(Lib_mr_SetScrBuf,"void w_setScreenBuffer(uint16*);");
	AddString(Lib_mr_GetScrSize,"void mrc_getScreenSize(int*,int*);");
	AddString(Lib_mr_SetScrSize,"void mrc_setScreenSize(int,int);");
	AddString(Lib_mr_BmpShowFlip,"int mrc_bitmapShowFlip(uint16*,int16,int16,uint16,uint16,uint16,uint16,int16,int16,uint16);");
	AddString(Lib_mr_bitmapShowEx,"int mrc_bitmapShowEx(uint16*,int16,int16,uint16,uint16,uint16,uint16,int16,int16);");
	AddString(Lib_mr_Wap,"void mrc_connectWAP(char*);");
	AddString(Lib_mr_ReadFileFromMrp,"int mrc_readFileFromMrpEx(char*,char*,uint8**,long*,int);");
	AddString(Lib_mr_FreeFileData,"void mrc_freeFileData(void*,int);");
	AddString(Lib_mr_FreeOrigin,"void mrc_freeOrigin(void*,int);");
	AddString(Lib_mr_TimerCreate,"long mrc_timerCreate();");
	AddString(Lib_mr_TimerDel,"void mrc_timerDelete(long);");
	AddString(Lib_mr_TimerStop,"void mrc_timerStop(long);");
	AddString(Lib_mr_TimerStart,"int mrc_timerStart(long,long,long,char*,int);");
	AddString(Lib_mr_TimerSetTime,"int mrc_timerSetTimeEx(long,long);");
	return hanclose();

}

