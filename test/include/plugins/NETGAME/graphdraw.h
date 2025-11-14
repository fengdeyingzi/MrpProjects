#ifndef _GRAPHDRAW_H_
#define _GRAPHDRAW_H_

#include "mrc_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct  
{
	int16 x;	//绘图区域屏幕起始点的横坐标
	int16 y;	//绘图区域屏幕起始点的纵坐标
	int16 w;	//欲加载的图片宽度
	int16 h;	//欲加载的图片高度
	int16 mw;	//源图片的原始宽度
	int16 sx;	//从源图片加载的起始宽度
	int16 sy;	//从源图片加载的起始高度
}drawBmpParamSt;

typedef struct  
{
	int16 x;	//待显示的文本左上角横坐标
	int16 y;	//待显示的文本左上角纵坐标
	mr_screenRectSt rect;	//显示的字符限制范围
}drawTextParamSt;

typedef struct 
{
	int16 x;
	int16 y;
	uint16 mw;
	uint16 w;
	uint16 h;
	uint16 rop;
	int16 sx;
	int16 sy;
	uint16 transcolor;
}GifDecodeParaSt;

enum
{
	GIF2SCREEN,
	GIF2BMP16,
	GIF2BMP256
};

/* 加载graphdraw绘图模块,加载到cache */
int32 mrc_graphdraw_cacheLoad(void);

/* 加载graphdraw绘图模块 */
int32 mrc_graphdraw_load(void);

int32 mrc_graphdraw_reload(void);

/* 卸载graphdraw绘图模块 */
int32 mrc_graphdraw_unload(void);

/* 画点接口 */
void mrc_graphdraw_drawPoint(int32 x, int32 y, uint16 color565);

/* 画线接口,对画直线或等斜率作了优化,(x1,y1)-->(x2,y2); */
void mrc_graphdraw_drawLine(int32 x1, int32 y1, int32 x2, int32 y2, uint16 color565);

/* 画矩形区域,并用颜色填充区域,x,y为矩形左上角起始点位置,w,h分别为矩形宽度和高度 */
void mrc_graphdraw_drawRect(int32 x, int32 y, int32 w, int32 h, uint16 color565);

/* 画矩形区域,并用RGB颜色填充,x,y为矩形左上角起始点位置,w,h分别为矩形宽度和高度 */
void mrc_graphdraw_drawRectRGB(mr_screenRectSt* rect, uint8 r, uint8 g, uint8 b);
/* 
 * 画不填充的矩形函数
 * color565参数表示该参数需要RGB565格式的颜色,可以使用下面的宏来转换
 * 参见SGL_common.h
 * 1.RGB2PIXEL565
 * 2.COLOR2PIXEL565
 */

 /* 画矩形边框,边框颜色采用color565,x,y为矩形左上角起始点位置,w,h分别为矩形宽度和高度 */
void mrc_graphdraw_drawRectangle(int32 x, int32 y, int32 w, int32 h, uint16 color565);

/*
 * [是mrc_drawEffSetCon函数的加速版]
 * 将屏幕的左上角x,y，宽高为w,h的矩形区域内的r,g,b分别
 * 增强perr/256, perg/256, perb/256倍。
 * 若perr=256, perg=0, perb=0，将只保留矩形区域内的红色；若
 * perr=perg= perb=128，将使矩形区域内产生一种半透明的效果。
 * 输入:x,y,w,h	屏幕位置。perr, perg, perb	r,g,b变化的数值。
 */
void  mrc_graphdraw_drawEffSetCon(mr_screenRectSt *pRect, int32 perr, int32 perg, int32 perb);
/*
位图绘图接口,不是加速绘图接口

参数说明: 
p: 16位位图数据;
pRectParam: 绘图区域信息;
rop: 绘图操作类型:
BM_OR,			//SRC .OR. DST*   	半透明效果
BM_XOR, 		//SRC .XOR. DST*	异或操作
BM_COPY,		//DST = SRC*		覆盖/拷贝
BM_NOT,			//DST = (!SRC)*		取反操作
BM_MERGENOT,	//DST .OR. (!SRC)	或加取反操作
BM_ANDNOT,     	//DST .AND. (!SRC)	与加取反操作
BM_TRANSPARENT,	//透明色不显示，图片的第一个象素（左上角的象素）是透明色
BM_AND,        	//DST AND SRC		与操作
BM_GRAY,        //灰度绘图,相当于BM_TRANSPARENT＋灰度绘图：DST灰度 ＝ 30%R + 59%G + 11%B图片序号
BM_REVERSE,		//反向绘图,相当于BM_TRANSPARENT＋反向绘图(V1939)

当rop为 BM_COPY 或者 BM_TRANSPARENT 操作时，可以支持旋转操作
如 rop = BM_COPY | MTRANS_ROT90;表示顺时针旋转90度绘图.
rop = BM_TRANSPARENT | MTRANS_ROT90;表示旋转90度后作透明色处理并绘图.

transcolor: 透明色,默认将第一个像素点作为透明色.
*/
//下面三个宏定义只为保持版本前后兼容性
#define FLIP_NONE	0x0000
#define	FLIP_HORI	0x0100	//水平翻转
#define FLIP_VERT	0x0200	//垂直翻转

#define MTRANS_NONE             0x0000  //没有进行转动和翻转.
#define MTRANS_MIRROR			0x0100  //水平(左右)翻转<==>FLIP_HORI
#define MTRANS_MIRROR_ROT180    0x0200  //水平(左右)翻转后180度转动<==>FLIP_VERT垂直(上下)翻转
#define MTRANS_ROT90            0x0300  //90度转动
#define MTRANS_ROT180			0x0400  //180度转动
#define MTRANS_ROT270			0x0500  //270度转动
#define MTRANS_MIRROR_ROT90		0x0600  //水平(左右)翻转后90度转动
#define MTRANS_MIRROR_ROT270    0x0700  //水平(左右)翻转后270度转动

int32 mrc_graphdraw_drawBitmap(uint16* p, drawBmpParamSt *pRectParam, uint16 rop, uint16 transcolor);

int32 mrc_graphdraw_bitmapShow(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, int16 sx, int16 sy);

int32 mrc_graphdraw_bitmapShowTrans(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, int16 sx, int16 sy, uint16 transcolor);

/*
画字接口,从左到右绘字.

  pcText: 字符串缓冲区;
  pRectParam: 绘字区域信息;
  pColor:	字体颜色;
  flag: 画字标识,可以取如下值:
  #define DRAW_TEXT_EX_IS_UNICODE         1 //是否使用UNICODE码, 网络字节序
  #define DRAW_TEXT_EX_IS_AUTO_NEWLINE    2 //是否自动换行
  font: 字体大小类型: 
  MR_FONT_SMALL	//小字体
  MR_FONT_MEDIUM	//中型字体
  MR_FONT_BIG		//大字体
*/
int32 mrc_graphdraw_drawText(char* pcText, drawTextParamSt *pRectParam, mr_colourSt *pColor, int32 flag, uint16 font);

int32 mrc_graphdraw_drawTextLeft(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int32 flag, uint16 font);

/*
 参数跟上面的绘字接口一样,不同的是将字符串从右到左绘制到相应区域
*/
int32 mrc_graphdraw_drawTextR(char* pcText, drawTextParamSt *pRectParam, mr_colourSt *pColor, int32 flag, uint16 font);

int32 mrc_graphdraw_drawTextRight(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int32 flag, uint16 font);

/*
 * [初始化gif画图]
 * 如果批量画宽高相同的gif图片,只需调用一次graphics_startGif,然后可多次调用graphics_SpeedupgifDecode,最后需要调用一次graphics_endGif。
 * 如果处理不同宽高的gif图片,则需要每次都调用graphics_startGif和graphics_endGif.
 * 调用示例1:
 *		graphics_startGif(48,48);
 *		graphics_SpeedupgifDecode(&gifBuf1, 1234, GIF2SCREEN, &param1);
 *		graphics_endGif();
 *		graphics_startGif(36,48);
 *		graphics_SpeedupgifDecode(&gifBuf2, 2345, GIF2SCREEN, &param2);
 *		graphics_endGif();
 * 调用示例2:
 *		graphics_startGif(48,48);
 *		graphics_SpeedupgifDecode(&gifBuf1, 1234, GIF2SCREEN, &param1);
 *		graphics_SpeedupgifDecode(&gifBuf2, 2345, GIF2SCREEN, &param2);
 *		graphics_endGif();
 * 输入:
 *		gifWid: [输入]gif图片宽度
 *		gifHei: [输入]gif图片高度
 *		isLoadCache :[输入] 
 *			1: 解码中用到的buffer在cache中申请  
 *			0: 不在cache中申请
 */
int32 mrc_graphdraw_startGif(int16 gifWid, int16 gifHei);

/*
 * [结束gif画图]
 */
void mrc_graphdraw_endGif(void);

/*
 * [gif解码]
 * 参数:
 *		gifBuffer:		[输入]gif图片数据
 *		gifBufferLen:	[输入]gif数据字节数
 *		[直接画屏]
 *			outType=GIF2SCREEN
 *			pPara:[输入]GifDecodeParaSt类型指针,指定图片范围(暂时只支持: rop = BM_COPY, sx = 0, sy = 0)
 *		
 *		[gif转16位bmp]
 *			outType=GIF2BMP16
 *			pPara:[输出]16位bmp的buffer指针，需要用户自己申请空间并释放,空间大小为 (gifWid*gifHei*2)。
 *				  输出的16位bmp可以直接调用bitmapShow等接口进行画图。
 *				
 *		[gif转8位bmp]
 *			outType=GIF2BMP256
 *			pPara:[输出]8位bmp的buffer指针,	需要用户自己申请空间并释放,空间大小为 (gifWid*gifHei+769).
 *				  输出的8位bmp只能调用graphics_DrawBmp256接口进行画图。
 *	
 */
int32 mrc_graphdraw_SpeedupgifDecode(uint8 **gifBuffer, int32 gifBufferLen, int8 outType, void *pPara);

/*
 * [8位bmp画图]
 * 不能处理标准8位图,只能处理由graphics_SpeedupgifDecode输出的8位bmp,具体参看graphics_SpeedupgifDecode的[gif转8位bmp]
 * 参数:
 *		bmpBuffer:	[输入]8位bmp数据
 *		pPara:		[输入]GifDecodeParaSt类型指针,指定画图范围,支持sx,sy参数		
 */
void mrc_graphdraw_DrawBmp256(uint8* bmpBuffer, void *pPara);

/*
 * 与mrc_unicodeTextWidthHeight功能差不多，这个函数是加速版, 还支持12号小字体
 * 输入:
 * pcText:         待显示的字符串
 * font:           字体大小，可能的值是   
 *                  MR_FONT_SMALL		使用超大安装的12号小字体
 *                  MR_FONT_MEDIUM		
 *                  MR_FONT_BIG
 * 输出:
 * w：字符串的显示宽度
 * h：字符串的显示高度
 * 返回:
 *  MR_SUCCESS     操作成功
 *  MR_FAILED      操作失败
 */
int32 mrc_graphdraw_unicodeTextWidthHeight(uint16* pcText, uint16 font, int32* w, int32* h);

/*
 * 与mrc_unicodeTextRow功能差不多，这个函数是加速版, 还支持12号小字体
 * 仅处理Unicode字符串输入
 * 返回待显示字符串若显示在宽为w的区间里，
 * 需要的行数；
 * pcText:         待显示的字符串
 * font:              字体大小，可能的值是   
 *                      MR_FONT_SMALL		使用超大安装的12号小字体
 *                      MR_FONT_MEDIUM
 *                      MR_FONT_BIG
 * w                    待显示宽度
 * 返回:
 *    MR_FAILED      操作失败
 *    其他           字符串行数
 */
int32 mrc_graphdraw_unicodeTextRow(uint16* pcText, uint16 font, int32 w);

/*
* 打开小字体文件句柄
 */
int32 mrc_graphdraw_OpenSmallFont(void);

/*
* 关闭小字体文件句柄
 */
int32 mrc_graphdraw_CloseSmallFont(void);

/*
 * 开启小字体加速模式。
 * 输入:
 *	fontNum:	缓存的字体个数。每字需要20个字节, fontNum范围[1-10008]
 * 返回值:
 *	MR_SUCCESS:	成功
 *	MR_FAILED:	失败
 */
int32 mrc_graphdraw_smallFontCacheLoad(int32 fontNum);

/*
 * 关闭小字体加速模式。
 */
void mrc_graphdraw_smallFontCacheUnload(void);

/* 绘图加速接口 */
/* 绘图加速初始化接口
* 调用绘图加速接口之前必须调用此接口初始化,调用一次即可.
* 主要绘图加速接口: mrc_graphdraw_bitmapShowDAC,mrc_graphdraw_bitmapShowDACTrans,mrc_graphdraw_drawRectDAC
 */
int32 mrc_graphics_DACInit(void);

/* 位图绘制加速接口,参数详细参见上面的mrc_graphdraw_drawBitmap接口 
* mrc_graphdraw_bitmapShowDAC,当需要处理透明色时,使用第一个像素作为透明色
* mrc_graphdraw_bitmapShowDACTrans,当需要处理透明色时,使用transcolor作为透明色
*/

//绘图加速接口的函数原型
/*
int32 mrc_graphdraw_bitmapShowDAC(uint16* p, int16 x, int16 y, 
								  uint16 mw, uint16 w, uint16 h, uint16 rop, int16 sx, int16 sy);

int32 mrc_graphdraw_bitmapShowDACTrans(uint16* p, int16 x, int16 y, 
									   uint16 mw, uint16 w, uint16 h, uint16 rop, int16 sx, int16 sy, uint16 transcolor);

// 矩阵区域绘制加速接口,参数详细参见上面的mrc_graphdraw_drawRect接口
void mrc_graphdraw_drawRectDAC(int32 x, int32 y, int32 w, int32 h, uint8 r, uint8 g, uint8 b);
*/

typedef int32 (*bitmapShowType)(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
								int16 sx, int16 sy);
typedef int32 (*bitmapShowTransType)(uint16* p, int16 x, int16 y, uint16 mw, uint16 w, uint16 h, uint16 rop, 
									 int16 sx, int16 sy, uint16 transcolor);
typedef void (*drawRectType)(int32 x, int32 y, int32 w, int32 h, uint8 r, uint8 g, uint8 b);

extern bitmapShowType mrc_graphdraw_bitmapShowDAC;
extern bitmapShowTransType mrc_graphdraw_bitmapShowDACTrans;
extern drawRectType mrc_graphdraw_drawRectDAC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif