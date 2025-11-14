#ifndef _MRC_JGRAPHICS_H
#define _MRC_JGRAPHICS_H

#include "mrc_base.h"
#include "mrc_bmp.h"



#define TOP_Graphics            16
#define BASELINE_Graphics       64
#define BOTTOM_Graphics         32
#define VCENTER_Graphics        0x2


#define LEFT_Graphics           0x4
#define HCENTER_Graphics        0x1
#define RIGHT_Graphics          0x8


#define AP_V_MASK_Graphics      (TOP_Graphics|BASELINE_Graphics|BOTTOM_Graphics|VCENTER_Graphics)
#define AP_H_MASK_Graphics      (LEFT_Graphics|HCENTER_Graphics|RIGHT_Graphics)




#define TRANS_NONE_Sprite          0
#define TRANS_MIRROR_ROT180_Sprite 1
#define TRANS_MIRROR_Sprite        2
#define TRANS_ROT180_Sprite        3

#define TRANS_MIRROR_ROT270_Sprite 4
#define TRANS_ROT90_Sprite         5
#define TRANS_ROT270_Sprite        6
#define TRANS_MIRROR_ROT90_Sprite  7

typedef struct _mrc_jgraphics_context_t {    
    struct {

        /*
         *  Clip 变量： 坐标值均为经过translate处理过的。
         *  相对于 ScreenBuffer的坐标。
         */
        int clipX;
        int clipY;
        int clipWidth;
        int clipHeight;
        int clipXRight;    /* = clipX + clipWidth， 为了速度而设置的冗余值 */
        int clipYBottom;   /* = clipY + clipBottom，为了速度而设置的冗余值 */
        
        int translateX;
        int translateY;
        
        int font;
        
        uint8  color_R, color_G, color_B;
        uint16 color_565;
    }
    mutableValues;
    
    
    uint16 * __SJC_SCREEN_BUFFER__; //假的屏幕缓冲(缓存)
    int __SJC_SCREEN_WIDTH__; //假的屏幕宽(缓存宽)
    int __SJC_SCREEN_HEIGHT__; //假的屏幕高(缓存高)
	int flag; //标识是否是屏幕buffer，0-否，1-是
	uint16 * __REAL_SJC_SCREEN_BUFFER__; //真正的屏幕冲缓
	int __REAL_SJC_SCREEN_WIDTH__; //真正的屏幕宽
    int __REAL_SJC_SCREEN_HEIGHT__; //真正的屏幕高
}
mrc_jgraphics_context_t;


typedef struct {
	uint16*  data;
	uint16  width;
	uint16	height;
	uint16   trans; // 0 - 图片中不包含透明色，1： 图片左上角第一个像素为透明色; 2: transcolor指定。
	uint16  transcolor;  //如果包含透明色， 那么transcolor的565数值。
} mrc_jImageSt;

typedef 

struct _mrc_jgraphics_func_table_t {
    void (*setClip)(mrc_jgraphics_context_t * gContext, int x, int y, int width, int height);
    void (*clipRect)(mrc_jgraphics_context_t * gContext, int x, int y, int width, int height);
    void (*drawLine)(mrc_jgraphics_context_t * gContent, int x1, int y1, int x2, int y2);
    void (*drawImage)(mrc_jgraphics_context_t * gContext, mrc_jImageSt * img, int x, int y, int anchor);
    void (*drawString)(mrc_jgraphics_context_t * gContext, char* str, int x, int y, int anchor);
    void (*fillRect)(mrc_jgraphics_context_t* gContext, int x, int y, int width, int height);
    void (*drawRect)(mrc_jgraphics_context_t * gContext, int x, int y, int width, int height);
    void (*drawRegion)(mrc_jgraphics_context_t * gContext, mrc_jImageSt * src, int x_src, int y_src, int width,   int height,     int transform,    int x_dest,     int y_dest,   int anchor);
	void (*copyArea)(mrc_jgraphics_context_t * context, int x_src, int y_src, int width, int height, int x_dest, int y_dest, int anchor);
} mrc_jgraphics_func_table_t;

extern mrc_jgraphics_func_table_t mrc_jgraphics_func_table;

extern mrc_jgraphics_context_t * mrc_jgraphics_context;

/*
 * 函数名：mrc_jgraphics_init
 * 功能  ：初始化sjc_Graphics，获取屏幕的高和宽
 * 初始化裁剪区域，初始化平移参数，获取屏幕缓存
 * 在绘制开始前最先调用，初始化绘图。
 *
 *   参数：
 *      mode : 0 - 普通模式
 *           : 1 - 加速模式
 *       
 *   返回值：
 *             0 - 普通模式加载成功
 *             1 - 加速模式加载成功
 *
 */
extern int32 mrc_jgraphics_init(int32 mode);

/* 测试是否支持加速模式
 * 1-支持 0-不支持
 */
extern int mrc_jgraphics_supportSpeed(void);

/*函数名：mrc_jgraphics_resetContext
 *功能  ：重新初始化裁剪区域（全屏幕），
 *		重新初始化平移参数（0，0）
 *		可在渲染绘制循环开始时调用
 * 参数：
 *	context mrc_jgraphics_context_t指针
*/
extern void mrc_jgraphics_resetContext(mrc_jgraphics_context_t * context);

/*
 * 函数名	：mrc_jgraphics_translate
 * 功能	：平移当前坐标系
 * 参数：
 * context mrc_jgraphics_context_t指针
 */
extern void mrc_jgraphics_translate(mrc_jgraphics_context_t * context, int x, int y);

/*
 * 函数名	：mrc_jgraphics_getTranslateX
 * 功能	：获取当前
 * 参数：
 *	context mrc_jgraphics_context_t指针
*/
extern int mrc_jgraphics_getTranslateX(mrc_jgraphics_context_t * context);

/*
 * 函数名	：mrc_jgraphics_getTranslateY
 * 功能	：平移当前坐标系
 * 参数：
 *	context mrc_jgraphics_context_t指针
*/
extern int mrc_jgraphics_getTranslateY(mrc_jgraphics_context_t * context);

/* 函数名	：mrc_jgraphics_setClip
 * 功能	：重设当前裁剪矩形框,受translate影响
 * 参数：
 *	context mrc_jgraphics_context_t指针
 */
#define mrc_jgraphics_setClip(context, x, y, width, height) \
	mrc_jgraphics_func_table.setClip((context), (x), (y), (width), (height))

/*
 * 函数名	：mrc_jgraphics_clipRect
 * 功能	：设置裁剪矩形。
 * 输入参数：
 *			context mrc_jgraphics_context_t指针
 *			x		--当前坐标系下所要设置裁剪矩形左上点的x坐标
 *			y		--当前坐标系下所要设置裁剪矩形左上点的y坐标
 *			width	--所要设置裁剪矩形的宽
 *			height	--所要设置裁剪矩形的高
 * !!所要设置的裁剪矩形超出当前裁剪矩形的部分将被当前裁剪矩形裁剪*/
#define mrc_jgraphics_clipRect(context, x,y,width,height) \
	mrc_jgraphics_func_table.clipRect((context), (x), (y), (width), (height))

 /*
 *函数名	：mrc_jgraphics_getClipHeight
 *功能	：获取当前裁剪矩形的高
 * 参数：
 *	context mrc_jgraphics_context_t指针
 */
extern int 	mrc_jgraphics_getClipHeight(mrc_jgraphics_context_t * context);

/*
 * 函数名	：mrc_jgraphics_getClipWidth
 * 功能	：获取当前裁剪矩形的宽
 * 参数：
 *	context mrc_jgraphics_context_t指针
 */
extern int 	mrc_jgraphics_getClipWidth(mrc_jgraphics_context_t * context);

/*
 * 函数名	：mrc_jgraphics_getClipX
 * 功能	：获取当前裁剪矩形的左上点在屏幕坐标系下的x坐标
 * 参数：
 *	context mrc_jgraphics_context_t指针
 */
extern int 	mrc_jgraphics_getClipX(mrc_jgraphics_context_t * context);

/*
 * 函数名	：mrc_jgraphics_getClipY
 * 功能	：获取当前裁剪矩形的左上点在屏幕坐标系下的y坐标
 * 参数：
 *	context mrc_jgraphics_context_t指针
 */
extern int 	mrc_jgraphics_getClipY(mrc_jgraphics_context_t * context); 





/*
 * 函数名	：mrc_jgraphics_setFont
 * 功能	：设置当前字体
 *		MR_FONT_SMALL,
 *		MR_FONT_MEDIUM,
 *		MR_FONT_BIG
 * 参数：
 *	context mrc_jgraphics_context_t指针
 */
extern void mrc_jgraphics_setFont(mrc_jgraphics_context_t * context, uint16 font);

/*
 * 函数名	：mrc_jgraphics_getFont
 * 功能	：获取当前字体
 * 参数：
 *	context mrc_jgraphics_context_t指针
*/
extern uint16 mrc_jgraphics_getFont(mrc_jgraphics_context_t * context);

/*
 * 函数名	：mrc_jgraphics_setColor
 * 功能	：设置当前颜色
 * 参数：
 *	context mrc_jgraphics_context_t指针
 * 参数    ：int RGB
 */
extern void mrc_jgraphics_setColor(mrc_jgraphics_context_t * context, int RGB);

/*
 * 函数名	：mrc_jgraphics_setColor_Ex
 * 功能	    ：设置当前颜色
 * 参数     ：int red, int green, int blue
 * 参数：
 *	context mrc_jgraphics_context_t指针
 */
extern void mrc_jgraphics_setColor_Ex(mrc_jgraphics_context_t * context, int red, int green, int blue);

/*
 * 函数名	：mrc_jgraphics_getColor
 * 功能	：获取当前颜色
 * 参数：
 *	context mrc_jgraphics_context_t指针
 * 返回    ：8/8/8 format pixel (0x00RRGGBB)
 */
extern int  mrc_jgraphics_getColor(mrc_jgraphics_context_t * context);



////////////////////////////////////////////////////////////////////////////////////////////////////
//   下面为真正的图形函数。                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * 函数名	：mrc_jgraphics_drawImage
 * 功能	：绘制图像
 *		typedef struct {
 *	uint8*  data; //源图片指针
 *	uint16  width;
 *	uint16	height;
 *	uint8   trans;//1:使用透明色;0:不使用
 *		} Image_s;
 */
#define mrc_jgraphics_drawImage(context, img, x, y, anchor) \
	mrc_jgraphics_func_table.drawImage((context),(img),(x),(y),(anchor))

/*
 * 函数名	：mrc_jgraphics_drawRegion
 * 功能	：绘制图像
 * 参数	：	src,					--图像	Image_s结构指针
 * 			x_src, y_src,			--源图像
 * 			width,height,			--源图像宽和高
 * 			transform,				--翻转参数：
 * 									TRANS_NONE_Sprite			不做变换
 * 									TRANS_MIRROR_Sprite			镜像
 * 									TRANS_ROT180_Sprite			旋转180
 * 									TRANS_MIRROR_ROT180_Sprite	镜像旋转180
 * 									TRANS_ROT90_Sprite 			旋转90
 * 									TRANS_MIRROR_ROT90_Sprite 	镜像旋转90
 * 									TRANS_ROT270_Sprite 		旋转270
 * 									TRANS_MIRROR_ROT270_Sprite 	镜像旋转270
 * 			x_dest,y_dest,			--屏幕上的位置
 *             anchor					--锚定参数（当前只支持为0（左上点））
 */
 
#define mrc_jgraphics_drawRegion(context, src,x_src,y_src,width,height,transform,x_dest,y_dest,anchor) \
	mrc_jgraphics_func_table.drawRegion((context),(src),(x_src),(y_src),(width),(height),(transform),(x_dest),(y_dest),(anchor))

/**
 * 函数名：mrc_jgraphics_newContext
 * 功能：新生成一个非屏幕缓冲的mrc_jgraphics_context
 * 参数：
 *		buf：缓冲内存地址
 *		width：缓冲宽
 *		height：缓冲高
 * 返回值：
 *		mrc_jgraphics_context_t 指针
 */
extern mrc_jgraphics_context_t * mrc_jgraphics_newContext(uint16 * buf, int32 width, int32 height);

/**
 * 函数名：mrc_jgraphics_deleteContext
 * 功能：销毁一个 mrc_jgraphics_context_t 指针
 */
extern void mrc_jgraphics_deleteContext(mrc_jgraphics_context_t * context);

/*
 * 函数名	：mrc_jgraphics_drawString
 * 功能	：绘制字符串,仅处理Unicode字符串输入
 */
#define mrc_jgraphics_drawString(context, str, x, y, anchor) \
	mrc_jgraphics_func_table.drawString((context), (str), (x), (y), (anchor))

/*
 * 函数名	：mrc_jgraphics_drawLine
 * 功能	：绘制直线
 * 参数    ：	x1:当前坐标系下所要绘制直线一端的x坐标
 *			y1:当前坐标系下所要绘制直线一端的y坐标
 *			x2:当前坐标系下所要绘制直线另一端的x坐标
 *			y2:当前坐标系下所要绘制直线另一端的y坐标
 */
#define mrc_jgraphics_drawLine(context, x1, y1, x2, y2) \
	mrc_jgraphics_func_table.drawLine((context), (x1), (y1), (x2), (y2))

/*
 * 函数名	：mrc_jgraphics_fillRect
 * 功能	：绘制填充矩形
 */
#define mrc_jgraphics_fillRect(context, x, y, width, height) \
	mrc_jgraphics_func_table.fillRect((context), (x), (y), (width), (height))

/*
 * 函数名	：mrc_jgraphics_drawRect
 * 功能	：绘制矩形框
 */
#define mrc_jgraphics_drawRect(context, x, y, width, height) \
	mrc_jgraphics_func_table.drawRect((context), (x), (y), (width), (height))

#define mrc_jgraphics_copyArea(context, x_src, y_src, width, height, x_dest, y_dest, anchor) \
	mrc_jgraphics_func_table.copyArea((context), (x_src), (y_src), (width), (height), (x_dest), (y_dest), (anchor))
#endif