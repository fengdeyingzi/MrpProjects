#include "string.h"

#include "mrc_exb.h"
#include "smp.h"

#ifndef WIN32
PSTR tolower(PSTR str)
{
	int i = 0;

	for(; str[i]; i++)
	{
		if(str[i] >= 'A' && str[i] <= 'Z') 
			str[i] += 'a' - 'A';
	}

	return str;
}


PSTR toupper(PSTR str)
{
	int i = 0;

	for(; str[i]; i++)
	{
		if(str[i] >= 'a' && str[i] <= 'z') 
			str[i] += 'A' - 'a';
	}

	return str;
}
#endif

////////////////////自定义函数//////////////////////////////
/**********************unicode转GB编码函数*****************/
uint8* UnicodeToChar(PWSTR unicode)
{
	PWSTR gb;
	int32 inLen,outLen;

	inLen = (int32)wstrlen((PCWSTR)unicode);
	outLen = inLen*2 + 1;
	gb = (PWSTR)mrc_malloc((int)outLen);
	mrc_memset(gb, 0, outLen);
	mrc_unicodeToGb2312((uint8*)unicode, inLen, (uint8**)&gb, &outLen);
	return gb;
}

/*********************GB转Unicode编码函数*****************/
uint16* CharToUnicode(PSTR gb)
{
	int32  unilen ; /* 用于存放Unicode长度，函数自己获得长度不需要使用者计算 */
	uint16* unicode;
	unicode = mrc_c2uVM(gb, NULL, &unilen);  //size存放输出的Unicode的长度，函数
	return unicode;
}

VOID CharToUnicode2(char* in, uint16** out, int32* outlen)
{
	//输出unicode指针及长度
	*out = mrc_c2uVM(in, NULL, outlen);  
}



Sint32 stricmp(PCSTR dst, PCSTR src)
{
    CHAR f,l;
    
    do{
        if (((f = *(dst++)) >= 'A') && (f <= 'Z'))
            f -= ('A' - 'a');
        
        if (((l = *(src++)) >= 'A') && (l <= 'Z'))
            l -= ('A' - 'a');
    } while (f && (f == l));
    
    return (f - l);
}


PSTR trimleft(PSTR str)
{
	SGL_ASSERT(FALSE && "NOT IMPLEMENTED YET!");
	return str;
}


PSTR trimright(PSTR str)
{
	SGL_ASSERT(FALSE && "NOT IMPLEMENTED YET!");
	return str;
}


PSTR trim(PSTR str)
{
    int i;
    PSTR porg = str;
    int len = wstrlen((PCWSTR)str);

    //trim left
    for(;str[0] == 0 && str[1] == ' '; str += 2);

    //trim right
    for(i = len -1; porg[i -1] == 0 && porg[i] == ' '; i -= 2)
        porg[i] = 0;

    if(porg != str) 
		wstrcpy((PWSTR)porg, (PCWSTR)str);

    return porg;
}



Sint32 split(PSTR str, CHAR sep, PSTR value[], Sint32 size)
{
	int i;	
	for(i = 0; i < size; i++)
	{
		while(*str && *str == sep) str++; //test space
		if(!*str) break;
		value[i] = str;
		while(*str && *str != sep) str++;
		if(!*str) return i+1;
		*str++ = 0;
	}	

	return i;
}

//获得Unicode字符串长度 
Sint32 wstrlen(PCWSTR str)
{
	int i = 0;
	while(str[i] || str[i+1]) 
		i+=2;
	return i;
}

//复制Unicode字符
PWSTR wstrcpy(PWSTR dst, PCWSTR src)
{
	int i = 0;
	while(src[i] || src[i+1]) 
	{
		dst[i] = src[i];
		dst[i+1] = src[i+1];
		i+=2;
	}

	dst[i] = dst[i+1] = 0;
	return dst;
}

//拼接Unicode字符
PWSTR wstrcat(PWSTR dst, PCWSTR src)
{
	int len = wstrlen(dst);
	return wstrcpy(dst + len, src);
}

//复制size个Unicode到目标字串
PWSTR wstrncpy(PWSTR dst, PCWSTR src, Sint32 size)
{
	int i = 0;
	size -= 2;

	while(src[i] || src[i+1]) 
	{
		if(i >= size) break;

		dst[i] = src[i];
		dst[i+1] = src[i+1];
		i+=2;
	}

	dst[i] = dst[i+1] = 0;
	return dst;
}


PCWSTR wstrchr(PCWSTR str, UCHAR chr1, UCHAR chr2)
{
	while(str[0] || str[1]) 
	{	
		if(str[0] == chr1 && str[1] == chr2)
			return str;
		str += 2;
	}
	
	return NULL;
}

//比较两个字符串Unicode
Sint32 wstrcmp(PCWSTR str1, PCWSTR str2)
{
	while((str1[0] || str1[1]) && str1[0] == str2[0] && str1[1] == str2[1])
	{
		str1+=2; 
		str2+=2;
	}

	return str1[0]==str2[0]? str1[1]-str2[1] : str1[0]-str2[0];
}


PWSTR wtrimleft(PWSTR str)
{
	SGL_ASSERT(FALSE && "NOT IMPLEMENTED YET!");
	return str;
}


PWSTR wtrimright(PWSTR str)
{
	SGL_ASSERT(FALSE && "NOT IMPLEMENTED YET!");
	return str;
}


PWSTR wtrim(PWSTR str)
{
	int i;
	PWSTR porg = str;
	int len = wstrlen(str);

	//trim left
	for(;str[0] == 0 && str[1] == ' '; str += 2);

	//trim right
	for(i = len -1; porg[i -1] == 0 && porg[i] == ' '; i -= 2)
		porg[i] = 0;

	if(porg != str) wstrcpy(porg, str);

	return porg;
}

//将 UTF8 转为 Unicode
Sint32 str2wstr(PCSTR utf8, PWSTR unicode, Sint32 size)
{
	int i = 0, u = 0;
	
	SGL_MEMSET(unicode, 0, size); 
	while(utf8[i] && u < size - 2)
	{
		if((utf8[i] & 0x80) == 0)
		{
			// one byte 0...
			unicode[u+1] = utf8[i++];
		}else if((utf8[i] & 0x20) == 0){
			// two bytes 110... 10...
			unicode[u] = (UCHAR)((utf8[i]) & 0x1f) >> 2;
			unicode[u+1] = ((UCHAR)((utf8[i]) & 0x1f) << 6) | (utf8[i+1] & 0x3f);
			i+=2;
		}else{
			// three bytes 1110... 10... 10...
			unicode[u] = ((utf8[i] & 0x0f) << 4) | ((utf8[i+1] & 0x3f) >> 2);
			unicode[u+1] = (((utf8[i+1] & 0x3f) << 6)) | (utf8[i+2] & 0x3f);
			i+=3;
		}

		u+=2;
	}

	return u;
}

//将Unicode转为UTF8
Sint32 wstr2str(PCWSTR unicode, PSTR utf8, Sint32 size)
{
	int u = 0, i = 0;

	SGL_MEMSET(utf8, 0, size);
	while((unicode[u] || unicode[u+1]) && i < size-1)
	{
		if(unicode[u] == 0 && unicode[u+1] < 128)
		{
			// 0 - 7 bits
			utf8[i++] = unicode[u+1];
		}else if((unicode[u] & 0xf8) == 0){
			// 8 - 11 bits
			utf8[i++] = 0xc0 | (unicode[u] << 2) | (unicode[u+1] >> 6);
			utf8[i++] = 0x80 | (unicode[u+1] & 0x3f);
		}else{
			// 12 - 16 bits
			utf8[i++] = 0xe0 | (unicode[u] >> 4);
			utf8[i++] = 0x80 | ((unicode[u] & 0x0f) << 2) | (unicode[u+1] >> 6);
			utf8[i++] = 0x80 | (unicode[u+1] & 0x3f);
		}

		u+=2;
	}
	
	return i;
}


char *strDup(char *pcStr)
{
    char *pcTmp = NULL;
    int iSize = 0;
    
    if(NULL == pcStr)
    {
        return NULL;
    }
    
    iSize = strlen(pcStr) + 1;
    pcTmp = malloc(iSize);
    if(NULL == pcTmp)
    {
        return NULL;
    }
    memset(pcTmp, 0, iSize);
    strcpy(pcTmp, pcStr);

    return pcTmp;
}


char *strChrEx(const char *src,int c)
{
    char *pTmp = NULL;
    
    if(NULL == src)
    {
        return NULL;
    }

    while(*src != 0)
    {
        if(*src == c)
        {
            pTmp = (char *)src;
            break;
        }
        src++;
    }
    return pTmp;
}

/*
*    CR:0x00 0x0d
*    LF: 0x00 0x0a
*    remove all CRLF
*/
PWSTR trimCRLF(PWSTR str)
{
    int iSize = 0;
    int i = 0;
   // int j = 0;

    if(NULL == str)
    {
        return NULL;
    }
    iSize = wstrlen((PCWSTR) str);
    for(i = 0; i < iSize;)
    {
        if(((0x00 == str[i]) && (0x0d == str[i+1])) || ((0x00 == str[i]) && (0x0a == str[i+1])))
        {
            memcpy(str+i, str+i+2, (iSize - i));
            iSize -= 2;
        }
        else
        {
            i+=2;
        }
    }

    return str;
}


PWSTR wstrdup(PCWSTR str)
{
    int len = 0;
    PWSTR pDest = 0;
    
    if(0 == str)
    {
        return NULL;
    }
    len = wstrlen(str) + 2;
    
    pDest = malloc(len);
    if(NULL == pDest)
    {
        return NULL;
    }

    memcpy(pDest, str, len);
    return pDest;
}
//屏幕中央显示文字函数
void DrawTextMidpoint(char* text,uint8 R,uint8 G,uint8 B,int8 is_unicode) 
{ 
    int16 x,y; 
    int32 w,h;

    mrc_textWidthHeight( text, is_unicode, MR_FONT_BIG, &w, &h); 
    x = (int16)(SCREEN_WIDTH - w )/2; 
    y =  (int16)(SCREEN_HEIGHT- h )/2; 
    mrc_drawText(text,  x,  y,  R,G,B, is_unicode, MR_FONT_MEDIUM); 
}
