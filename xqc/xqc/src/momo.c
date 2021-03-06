#include "momo.h"
#include "mrc_network.h"
#include "platform.h"

#define DIRLEN  18
char STOREDIR[DIRLEN];
int32 func_state;
char enable_smallfont;
char FONT_H;
char FONT_W;
extern uint16 SCREEN_WIDTH;
extern uint16 SCREEN_HEIGHT;
 static STACKDATA STACK;

int32 RdWrData(uint32 item, void* data, uint32 len, int32 isRead)
{
    char filename[64];
    int32 f;

    mrc_sprintf(filename,"%s%d.tmp",STOREDIR,item);
    if(isRead)//??ȡ
    {
        f=mrc_open(filename,MR_FILE_RDONLY);
        if(!f) return -1;
        mrc_read(f,data,len);
        mrc_close(f);
    }
    else
    {
        mrc_remove(filename);
        f=mrc_open(filename,MR_FILE_CREATE|MR_FILE_RDWR);
        if(!f) return -1;
        mrc_write(f,data,len);
        mrc_close(f);
    }
    return 0;
}

int32 RdWrCfg(uint32 item, int32 data, int32 isRead)
{
    char filename[64];
    int32 handle;
    uint32 len;
    int32 nil=0; //????д????

    mrc_sprintf(filename,"%scfg.dat",STOREDIR);
    len = mrc_getLen(filename);
    if(len%4 != 0)
    {
        mrc_remove(filename);
        len=0;
    }
    handle  = mrc_open(filename,MR_FILE_CREATE|MR_FILE_RDWR);
 
    if(handle)
    {
        if(len < (item+1)*sizeof(int32))
        {
            mrc_seek(handle,0,MR_SEEK_END);
            for(len = (item+1)*sizeof(int32)-len;  len;  len-=sizeof(int32))
                mrc_write(handle,&nil,sizeof(int32));
        }
        mrc_seek(handle,item*sizeof(int32),MR_SEEK_SET);
        if(isRead)
            mrc_read(handle, &data, sizeof(int32));
        else
            mrc_write(handle, &data, sizeof(int32));
        mrc_close(handle);
        return data;
    }
    return 0;
}


int CopyFile(const char* dst,const char* src)
{
    int32 dstf,srcf,len,t;
    uint8 *buf;
    
    if(!mrc_strcmp(dst,src)) goto err;
    t=mrc_getMemoryRemain()/2;
    buf = mrc_malloc(t);
    if(!buf) goto err;
    mrc_remove(dst);
    dstf = mrc_open(dst,MR_FILE_CREATE|MR_FILE_RDWR);
    if(!dstf) 
    {
        free(buf);
        goto err;
    }
    len = mrc_getLen(src);
    srcf = mrc_open(src,MR_FILE_RDONLY);
    if(!srcf || !len)
    {
        free(buf);
        mrc_close(dstf);
        goto err;
    }
    while(len)
    {
        if(len > t)
        {
            len -= t;
        }
        else
        {
            t = len;
            len ^= len;
        }
        mrc_read(srcf,buf,t);
        mrc_write(dstf,buf,t);
    }
    free(buf);
    mrc_close(dstf);
    mrc_close(srcf);

    return 0;
err:
    return -1;
}

void StackInit(void)
{
    memset(&STACK,0,sizeof(STACK));
}


void push(int16 data)
{
    STACK.stack[STACK.sp] = data;
     if(STACK.sp < 63)//δ????ջ??
        STACK.sp++;
     else
         STACK.sp=0;
}

int16 pop(void)
{
     if(STACK.sp > 0)//δ????ջ??
         STACK.sp++;
     else
         STACK.sp=0;

    STACK.sp--;
    return STACK.stack[STACK.sp];
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
    if(fullName[dirNameLen-1] != '/')//׼??ƴ??Ŀ¼?µ????ļ?????Ŀ¼
    {
        fullName[dirNameLen] = '/';
        dirNameLen++;
    }

    if(mrc_fileState(dirName) == MR_IS_DIR)
    {
        int32 fd;
        int32 result = MR_FAILED;

        fd = mrc_findStart(dirName, fileName, sizeof(fileName));
        if(fd > 0)//??????????Ч
        {
            result = MR_SUCCESS;
            while(result == MR_SUCCESS)
            {
                //????fileNameȡֵΪ??.????ʶ??????ֵ??Ч???????????δ?????
                if(fileName[0] && fileName[0] != '.')
                {
                    int32 length = strlen(fileName);
                    memcpy(&fullName[dirNameLen], fileName, length);
                    fullName[dirNameLen+length] = 0;

                    length = mrc_fileState(fullName);
                    if(length == MR_IS_FILE)//?ļ????ͣ?ɾ???ļ?
                        mrc_remove(fullName);                  
                    else if(length == MR_IS_DIR)//?ļ??????ͣ??ݹ?ɾ?????ļ???
                        removeDir(fullName);
                }
                memset(fileName, 0, sizeof(fileName));
                result = mrc_findGetNext(fd, fileName, sizeof(fileName));
            }
            mrc_findStop(fd);
        }
        //????һ?μ?????,????????ɾ???ļ??е?????
        fd = mrc_findStart("",fileName,sizeof(fileName));
        mrc_findStop(fd);
        ret = mrc_rmDir(dirName);
    }
    return ret;
}

void copymen(COPYMENDATA *dest, COPYMENDATA *src)
{
	int xi,yi;

	if(dest->w>src->w)
		dest->w=src->w;
	if(dest->h>src->h)
		dest->h=src->h;
	for(yi=0; yi<dest->h ; yi++)
	{
		for(xi=0; xi<dest->w; xi++)
		{
			*dest->arry[dest->y+yi][dest->x+xi] = *src->arry[src->y+yi][src->x+xi];
		}
	}
}
//
//int32 skyfontWidthHeight(char *szText, int32 *width, int32 *height)
//{
//    skyfont_screenRect_t rect = {0,0,-1,-1};
//
//    return mrc_skyfont_textWidthHeight(szText,0,-1,0,0,rect,0,width, height);
//}
//int32 skyfontdraw(char* szText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, uint16 flag)
//{
//    skyfont_screenRect_t rect1;
//
//    memcpy(&rect1,&rect,sizeof(rect));
//    return mrc_skyfont_drawTextLeft(szText, 0, -1, x, y, rect1, colorst, flag);
//}
char *stred(char *str, int len)
{
    int i;

    for(i=0; i<len; i++)
    {
        str[i] ^= i-1;
    }
    return str;
}

int readline(int32 f, char out[256])
{
    int i=0,tmp=0;

    do{
        if(1>mrc_read(f,&tmp,1))
            goto AA;
    }while(tmp=='\n' || tmp=='\r');
    do{
        out[i]=tmp;
        i++;
        if(1>mrc_read(f,&tmp,1))
        {
AA:         out[i]=0;
            return -1;
        }
        if(tmp=='\n' || tmp=='\r')
            break;
    }while(i<255 && tmp);
    if(i==255) mrc_seek(f,-1,MR_SEEK_CUR);
    return out[i]=0;
}

void ShadeLine(int x, int y, int w, uint32 pixelA, uint32 pixelB)
{
    int i=0,r,g,b,r1,g1,b1;

    r1=PIXEL888R(pixelA);
    g1=PIXEL888G(pixelA);
    b1=PIXEL888B(pixelA);
    r=PIXEL888R(pixelB)-r1;
    g=PIXEL888G(pixelB)-g1;
    b=PIXEL888B(pixelB)-b1;
    while(i<=w)
    {
        mrc_drawPointEx(x+i,y,r1+r*i/w,g1+g*i/w,b1+b*i/w);
        i=i+1;
    }
}
void MidShadeLine(int x, int y, int w, uint32 pixelA, uint32 pixelB)
{
    w--;
    ShadeLine(x,y,w/2,pixelA,pixelB);
    ShadeLine(x+w/2,y,w-w/2,pixelB,pixelA);
}

int32 GetMRPFileInfo(int32 MRPhandle, char *filename, int32 *filepos, int32 *filelen, int32 *iszip)
{
	int32 unfilelen;
	int32 unfilepos;
	int32 fileStart;
	int32 fileStop;	
	uint32 filenamelen;
    char name[128]={0,};

	//////////////////////////////////////////////////////////////////////////

    if(!filename || !filepos || !filelen || !iszip || !MRPhandle) return -1;

    //ȷ????ʼ????ֹλ??
	mrc_seek(MRPhandle,4,MR_SEEK_SET);
	mrc_read(MRPhandle, &fileStop,4);
	fileStop += 8;//?ļ??б???ֹ??λ??
	mrc_seek(MRPhandle,12,MR_SEEK_SET);
	mrc_read(MRPhandle, &fileStart,4);//?б???ʼλ??

	while(fileStart < fileStop)
	{
		mrc_seek(MRPhandle,fileStart,MR_SEEK_SET);  //?Ƶ????б???
		mrc_read(MRPhandle, &filenamelen,4);        //?ļ???????
		mrc_read(MRPhandle,name,filenamelen);       //?ļ???
		mrc_read(MRPhandle,&unfilepos,4);           //?ļ?λ??
		mrc_read(MRPhandle,&unfilelen,4);           //?ļ?????
		fileStart = fileStart + filenamelen + 16;   //??λ???Ƶ??¸??б???
        if(!mrc_strcmp(name,filename))              //???Ƿ???Ҫ?ҵ??ļ?
        {
            *filepos = unfilepos;
            *filelen = unfilelen;
            mrc_seek(MRPhandle,unfilepos,MR_SEEK_SET);
            mrc_read(MRPhandle,&unfilelen,4);
            if(unfilelen == 0x00088B1F)//?ж??Ƿ?Ϊѹ???ļ?
                *iszip = TRUE;
            else
                *iszip = FALSE;
            return 0;
        }
	}
	return -1;
}

char *strrtrim(char *str)
{
	char *p = str + strlen(str) - 1;

	while(p != str)
	{
		if((*p != ' ') && (*p != '\r') && (*p != '\n') && (*p != '\t'))
			break;
		p--;
	}
	p[1] = '\0';
	return str;
}

char *strtrim(char *s)
{
    char *p = s, *q = s, *o =s;

    while(*s == ' ') s++;
    while(*s)
    {
        if((*p++ = *s++) != ' ') q=p;
    }
    *q = 0;
    return o;
}

int32 MakePath(char *path)
{
    int32 i,t;
    char *dir=NULL;
    char *dir2=NULL;

    if(!path) return -1;
    i = strlen(path);
    if(i>127) return -1;
    dir=(char*)mrc_malloc(256);
    dir2=&dir[128];
    memset(dir,0,256);
    do
    {
        for(t=i=0; *path; i++,path++)
        {
            if(*path == '/')
                {t=1;path++;break;}
            dir[i]=*path;
        }
        dir[i]=0;
        if(t && *dir)
        {
            mrc_strcat(dir2,dir);
            mrc_mkDir(dir2);
            mrc_strcat(dir2,"/");
        }
    }while(*path);
    free(dir);
    return 0;
}

int32 TestStr(char *str, int32 len)
{
    char *t=str;
    int32 i;
    
    if(!str) return 0;
    for(i=0; *str && i<len; i++)
    {
        if(*str & 0x80)
        {
            if(*(str+1) & 0x80) str+=2;
            else str++;
        }
        else str++;
    }
    return str-t;
}

int32 StrNum(char *str)
{
    int32 ret=0;
    int32 i;
    
    for(i=0; str[ret]; i++)
    {
        if(str[ret] & 0x80)
        {
            if(str[ret+1] & 0x80) ret+=2;
            else ret++;
        }
        else ret++;
    }
    return i;
}

char* GetFileExt(const char* path)
{
	int32 len;
	char* extname, *position;

    position = mrc_strrchr(path, '.');
	if(position != NULL)
    {
        len = mrc_strlen(position);
        len+=1;
	    extname = (char*)mrc_malloc(len);
	    mrc_memset(extname, 0, len);
		mrc_strcpy(extname, position + 1);
	    return extname;
    }
    else
        return NULL;
}



int32 strW(char* str, int is_uincode, uint16 font)
{
	int32 charwidth = 0;
	int32 charheight = 0;
	mrc_textWidthHeight(str,  is_uincode, font, &charwidth, &charheight);
	return charwidth;
}

int32 strH(char* str, int is_uincode, uint16 font)
{
	int32 charwidth = 0;
	int32 charheight = 0;
	mrc_textWidthHeight(str, is_uincode, font, &charwidth, &charheight);
	return charheight;
}

void TextScreen(const char *text, int8 is_unicode)
{
	mr_screenRectSt rect={0,0};
    mr_colourSt color;

	rect.w = SCREEN_WIDTH, rect.h = SCREEN_HEIGHT; //???־??ο?
	color.r = 255, color.g = 55, color.b = 92;   //??????ɫ

	mrc_EffSetCon(0,0,SCREEN_WIDTH, SCREEN_HEIGHT,-180,-180,-180);
	mrc_EffSetCon(0,0,SCREEN_WIDTH, SCREEN_HEIGHT,-180,-180,-180);
	mrc_EffSetCon(0,0,SCREEN_WIDTH, SCREEN_HEIGHT,180,180,180);
	mrc_drawTextEx((char*)text, rect.x ,rect.y ,rect , color, is_unicode|2, 0);
	mrc_refreshScreen(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
}

char *AddWhite(char *str, int32 len)
{
    if(str)
    {
        while(len > 0)
        {
            mrc_strcat(str," ");  
            len--;
        }
        return str;
    }
    else return NULL;
}

int32 SwitchDisk(const char* strDisk)
{
    int32 ret = MR_FAILED;	
    uint8* output = NULL;
    int32 output_len = 0;

#ifdef WIN32   
	return FALSE;
#endif

    ret = mrc_platEx(1204, (uint8*)strDisk, mrc_strlen(strDisk), &output, &output_len, NULL);
	
    return ret;
}

void DrawHatching(int x, int y, int w, int h)
{
/*	uint8 color=255;

	for(; h>0; h--,color--)
	{
		mrc_EffSetCon(x, y+h, x+w, 1, color,color,color);//??????
	}
*/
	mrc_EffSetCon(x+w,y+2,2,h-2	 ,180,180,180);//??????

	mrc_EffSetCon(x+2,y+h,w,3,180,180,180);//????

}

void DrawRectangle(int x, int y, int w, int h, uint32 pixel)
{
	mr_colourSt clr;
	int16 r = x + w - 1;
	int16 b = y + h - 1;

	clr.r = (uint8)(pixel >> 16) & 0xff;
	clr.g = (uint8)(pixel >> 8) & 0xff;
	clr.b = (uint8)(pixel ) & 0xff;

	mrc_drawLine(x, y, r, y, clr.r,clr.g,clr.b);
	mrc_drawLine(x, b, r, b, clr.r,clr.g,clr.b);
	mrc_drawLine(x, y, x, b, clr.r,clr.g,clr.b);
	mrc_drawLine(r, y, r, b, clr.r,clr.g,clr.b); 

}

void DrawRectangle2(int x, int y, int w, int h, int line, uint32 pixel)
{
	int32 i;
	for(i = 0; i < line; i++)
		DrawRectangle(x+i, y+i, w-2*i, h-2*i, pixel);
}

void BMPScreen(const char *name, int16 w, int16 h,uint16 Mode, int32 BackColor)
{	
	mrc_clearScreen(PIXEL888R(BackColor),PIXEL888G(BackColor),PIXEL888B(BackColor));
	mrc_bitmapNew(0,w,h);
	mrc_bitmapLoad(0,(char*)name,0,0,w,h,w);
	mrc_bitmapShow(0,(int16)((SCREEN_WIDTH-w)>>1),(int16)((SCREEN_HEIGHT-h)>>1),Mode,0,0,w,h);
	mrc_bitmapLoad(0,"*",0,0,w,h,w);
	mrc_refreshScreen(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
}

void GetLocalTimer(char output[6])
{
	mr_datetime time;

	mrc_getDatetime(&time);
	if(time.minute < 10)  //????????С??9????ǰ׺0
		mrc_sprintf(output,"%d%s%d",time.hour,":0",time.minute);
	else 
		mrc_sprintf(output,"%d%s%d",time.hour,":",time.minute);
}

void DrawHatchingText(const char *Text, int16 x, int16 y, int is_unicode)
{
	mrc_drawText((char*)Text,x+1,y,0,0,0,is_unicode,1);
	mrc_drawText((char*)Text,x+1,y+1,0,0,0,is_unicode,1);
	mrc_drawText((char*)Text,x,y,255,255,255,is_unicode,1);
}
int32 PrintScreen(const char *bmpname)
{

    mrc_remove(bmpname);
	return mrc_PrintScreen((char *)bmpname);
}
void TextMiddle(const char *text, int8 is_unicode, int8 show)
{
    int32 w,h;

    mrc_textWidthHeight((char*)text,is_unicode,1,&w,&h);
	mrc_drawText((char*)text,(int16)(SCREEN_WIDTH - w)>>1,(int16)(SCREEN_HEIGHT - h)>>1, 255,255,255, is_unicode, 1);
	if(show)mrc_refreshScreen(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
}

char* ExtractFileName(char* path)
{
    char* name;
    int32 len;
    char *str;

    str = mrc_strrchr(path,'/');
    if(NULL != str)
        str++;
    else
        str = path;
    len = mrc_strlen(str)+1;
    name = (char*)mrc_malloc(len);
    mrc_memset(name, 0, len);
    mrc_memcpy(name, str, len-1);

    return name;
}
int32 GetCurSystemPath(char * out)
{
    int32 len, ret = MR_FAILED;
    uint8 *pDir = NULL;

    if(!out) return MR_FAILED;

#ifdef WIN32
    mrc_memcpy(out, "C:/mythroad/", 12);
    return MR_FAILED;
#endif

    if(MR_SUCCESS == mrc_platEx(1204, (uint8*)"Y", 1, &pDir, &len, NULL))
    {
        mrc_memset(out,0,len+1);
        mrc_memcpy((void*)out, (void*)pDir, len);
        ret = len;
    }else
        ret = MR_FAILED;

    return ret;
}
int32 GetDefSystemPath(char * out)
{
    int32 len, ret = MR_FAILED;
    uint8 *pDir = NULL;

    if(!out) return MR_FAILED;

#ifdef WIN32
    return MR_FAILED;
#endif

    ret = mrc_platEx(1204, (uint8*)"Z", 1, &pDir, &len, NULL);
    mrc_memcpy((void*)out, (void*)pDir, len);

    return ret;
}

int32 getDiskSpace(void)
{
    int32 ret;
    uint32 totalKSpace = 0, freeKSpace = 0, freeBSpace;

#ifdef WIN32
    return 0;
#endif

    freeBSpace = mrc_GetDiskFreeSpace32((uint8*)"C", &totalKSpace, &freeKSpace);
    if(freeBSpace > 2048)//??????Ҫ2M?ռ????ܷ??سɹ?
        ret=MR_SUCCESS;
    else
    {
        TextScreen("???̿ռ䲻?㣡????",FALSE);
        mrc_sleep(1000);
        ret=MR_FAILED;
    }
    return ret;
}
 //static void my_draw_str(char *str, int16 x, int16 y, int32 fg_color, int32 bg_color)
 //{
 //    mrc_drawText(str,x,y+1,PIXEL888RED(bg_color),PIXEL888GREEN(bg_color),PIXEL888BLUE(bg_color),0,1);
 //    mrc_drawText(str,x+1,y,PIXEL888RED(bg_color),PIXEL888GREEN(bg_color),PIXEL888BLUE(bg_color),0,1);
 //    mrc_drawText(str,x+2,y+1,PIXEL888RED(bg_color),PIXEL888GREEN(bg_color),PIXEL888BLUE(bg_color),0,1);
 //    mrc_drawText(str,x+1,y+2,PIXEL888RED(bg_color),PIXEL888GREEN(bg_color),PIXEL888BLUE(bg_color),0,1);
 //    mrc_drawText(str,x+1,y+1,PIXEL888RED(fg_color),PIXEL888GREEN(fg_color),PIXEL888BLUE(fg_color),0,1);
 //}

int32 Check(char *key)
{
	int32 ret = MR_FAILED;
	uint8 md5[17] = {0};
	uint8 cor[17] = {0};
	int32 fd, len;
	char* name = mrc_getPackName();
    char r;

#ifndef WIN32
    r=0;
#else
    r=1;
#endif

    if(r)
        len = mrc_getLen(name) - 240;
    else 
	    len = mrc_getLen(name) - 256;
	fd = mrc_open(name, MR_FILE_RDWR);
	if(fd > 0)
	{
		mrc_seek(fd, -16, MR_SEEK_END);
		mrc_read(fd, cor, 16);
		if(MR_SUCCESS == md5MakeFromFileHandle(key, fd, 240, len, md5))
			ret = !mrc_memcmp(md5, cor, 16) ? MR_SUCCESS : MR_FAILED;
		else
			ret = MR_FAILED;
        if(r)
        {
		    mrc_seek(fd, 0, MR_SEEK_END);
            mrc_write(fd,md5,16);
        }
	    mrc_close(fd);
	}
    else ret = MR_FAILED;

	return ret;
}
