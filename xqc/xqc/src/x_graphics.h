#ifndef _X_GRAPHICS_H__
#define _X_GRAPHICS_H__

#define  MAX_POINT 10   //多边形最多边数

/**
 * \距形区域定义
 */
typedef struct _RECT
{
    int left;		/*矩形的左上角X坐标*/
    int top;		/*矩形的左上角Y坐标*/
    int width;		/*矩形的宽度*/
    int height;		/*矩形的高度*/

} RECT, *PRECT;
//二维结构
typedef struct 
{ 
	int x; 
	int y; 
}EACH_POINT;
//多边形结构
typedef struct 
{ 
	int y_top; 
	float x_int; 
	int delta_y; 
	float x_change_per_scan;
}EACH_ENTRY; 




//矩形赋值
void RectRe(RECT *rect,int x,int y,int w,int h);
//用于划线判断，自动纠正反向绘制
void Rectlinepd(RECT *rect);
//判断psrcs1与psrcs2是否相交
int Intersect(PRECT psrcs1, PRECT psrcs2);
//一个点是否在屏幕内
int pointpd(int x,int y);
//pdst与psrc1相交区域赋值给psrc2
void sectRect(PRECT pdst, PRECT psrc1, PRECT psrc2);
//16位转24位
void rgb24(uint16 n,mr_colourSt *r);
//清空屏幕
void gr_cls(mr_colourSt color);
//画点
void gr_point(int x,int y,uint16 color);
//与gr_point相似，不同在于颜色参数
void gr_pointex(int x,int y,mr_colourSt color);
//画线
void gr_line(int x,int y,int x1,int y1,mr_colourSt color);
//画矩形
void gr_rect(int x,int y,int w,int h,mr_colourSt color);
//画多边形
void gr_fill_area(EACH_POINT *zb,int count,mr_colourSt color);
//所有点相连
void gr_bigger(EACH_POINT *zb,int n,mr_colourSt color) ;
//画圆
void gr_Circle(int r,int x0,int y0,mr_colourSt colors);
//使用中点算法画椭圆
void gr_Ellipse(int x0,int y0,int a,int b,mr_colourSt colors);
//获取屏幕一个位置的颜色
uint16 getpixel(int x,int y);
//对区域使用给定颜色进行填充 四方向的Flood算法 用的递归色块不要太大
void gr_Flood_Fill_4( int x, int y, uint16 old_color, uint16 new_color);

//设置裁剪区域
void gr_set_clip_rect(int left, int top, int right, int botton);
//把缓冲区的内容显示在屏幕上
void gr_graph();
//图形库初始化
void gr_init(void);
//关闭图形库
void gr_close();
#endif
