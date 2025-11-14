#ifndef _SKY_FONT16_H_
#define _SKY_FONT16_H_

//#include "mpc.h"

//编译时用sky_font16_mtk.lib，注意sky_font16_mtkt.lib含t的是thumb模式的，（vs2005调试时项目属性链接器用sky_font16_WIN32.lib）
/*
16号字体文件位于mythroad/system/gb16.uc2


原创：风的影子
优化：无尽时空
技术支持：小蟀
20240908
绘文字增加rect参数
2021.6.3
两个drawText绘文字函数取消了sez16_sky无用操作
drawText_un取消了200长度限制，
增加了超出SCRH的部分不绘制
drawText增加了超出SCRW的部分不绘制

利用5S理念 力争保证每个代码都有实际的用途


*/


int xl_font_sky16_init(void);//...........................................................................................初始化字库,返回MR_SUCCESS表示加载字库成功，MR_FAILED失败

int xl_font_sky16_close(void);//..........................................................................................退出前卸载字库

void xl_font_sky16_textWidthHeight(char*text,int isunicode,int32*width,int32*height);//...................................获取unicode文本的宽高

void xl_font_sky16_drawText_un(char*text0,int16 x0,int16 y0,mr_screenRectSt rect,mr_colourSt color,int flag,int fontsize);//.绘unicode文字，支持换行

void xl_font_sky16_drawText(char*text,int16 x,int16 y,mr_screenRectSt rect,mr_colourSt color,int isunicode,int fontsize);//..可以绘gb文字，支持换行

#endif

/*
extern int32 mrc_sleep(uint32 ms);

if (xl_font_sky16_init()==MR_FAILED)
{
mrc_clearScreen(0,0,0);
mrc_drawText("请下载gb16.uc2字库文件",0,0,255,255,255,0,1);
mrc_drawText("放在本地mythroad/system内",0,30,255,255,255,0,1);
mrc_drawText("sohehe4.ys168.com",0,60,255,255,255,0,1);
mrc_refreshScreen(0,0,240,320);
mrc_sleep(3000);
mrc_exit();
}

*/