#ifndef __FONT_H__
#define __FONT_H__
extern int32 FONT;

//初始化
void fontinit(void);
//载入字库  12或16
int32 openfont(int font);
//释放字库
int32 closefont(void);

//测试宽度高度
int32 fontwh(char* pcText, int is_unicode, uint16 font, int32* w, int32* h);
//显示字符串
int gbsez(char *a,int x,int y,int r,int g,int b,int un,int font);
//不载入字库直接显示
int gbsezex(char *a,int x,int y,int r,int g,int b,int un,int font);
int32 gbsez2h(char* pcText, int16 w,int flag,uint16 font);
//换行文字
int32 gbsez2(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font);
//换行文字
int32 gbsezex2(char* pcText, int16 x, int16 y, int w,int h,int r,int g,int b, int flag, uint16 font);

#endif
