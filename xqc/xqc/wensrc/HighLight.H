#ifndef _HIGHLIGHT_H
#define _HIGHLIGHT_H
//储存颜色块
typedef struct
{
	mr_colourSt keyword;//关键字
	mr_colourSt annotation;//注释
	mr_colourSt text;//文本
	mr_colourSt character;//字符
	mr_colourSt macro;//宏
	mr_colourSt variate;//变量
	mr_colourSt function;//函数名
}HIGHLIGHTCOR;
//初始化高光
void HighLightInit(void);
//显示一行高光文字
void DrawTextKeyWord(char *text,int32 x,int32 y);


#endif