/************************************************************
  FileName: graphics.c
  Description:           图形函数的实现
***********************************************************/






#include <mrc_base.h>
#include <mrc_bmp.h>
#include "struct.h"
#include "graphics.h"
#include "x_graphics.h "


EACH_POINT  gr_p[MAX_POINT];


/* 返回24位像素RGB888位R，G或B组分*/
#define PIXEL888RED(pixel)        (uint8)(((pixel) >> 16) & 0xff)
#define PIXEL888GREEN(pixel)      (uint8)(((pixel) >> 8) & 0xff)
#define PIXEL888BLUE(pixel)       (uint8)((pixel) & 0xff)

/* 
 * 设置颜色值 
 * mr_colourSt color;
 */
#define SET_COLOR(color, pixel)	\
	do{	\
		color.b = PIXEL888RED(pixel);	\
		color.g = PIXEL888GREEN(pixel);	\
		color.r = PIXEL888BLUE(pixel);	\
	}while(0)


mr_colourSt Color,FillColor;




void set_color(XM_COLOR color)      /* 设置颜色 */
{
	SET_COLOR(Color,color);
}

void set_fillcolor(XM_COLOR color)       /* 设置填充颜色 */
{
	SET_COLOR(FillColor,color);
}

/*************************************************
  Function:				init_graph
  Description:			初始化图形模式
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				
  Output:         
  Return:         
  Others:         
*************************************************/


void init_graph()
{
	gr_init();
}
void close_graph()
{

	gr_close();
}

/*************************************************
  Function:				put_pixel
  Description:			绘一个点
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				x		点的横坐标
                        y		点的纵坐标
						color	指定的颜色
  Output:         
  Return:         
  Others:         
*************************************************/
void put_pixel(int x, int y, XM_COLOR color)
{
	gr_pointex( x, y, Color);
}




/*************************************************
  Function:				put_line
  Description:			绘一条线
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				x1		第一个端点的横坐标
                        y1		第一个端点的纵坐标
						x2		第二个端点的横坐标
						y2		第二个端点的纵坐标
  Output:         
  Return:         
  Others:         
*************************************************/
void put_line(int x1, int y1, int x2, int y2)
{
	gr_line(x1, y1, x2,y2,Color);
}





/*************************************************
  Function:				put_polyline
  Description:			把指定的所有点相连
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				ps		点数组的头指针
                        n		点的个数
  Output:         
  Return:         
  Others:         
*************************************************/
void put_polyline(XM_POINT2D *ps, int n)
{
	int i;
	if(n<3)
		return;

	for (i = 0; i < n; i++)
	{
		gr_p[i+1].x = ps[i].x;
		gr_p[i+1].y = ps[i].y;
	}
	gr_bigger(gr_p, n,Color);
}




/*************************************************
  Function:				put_polygon
  Description:			绘一个填充的多边形
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				ps		顶点数组的头指针
                        n		顶点的个数
						color	填充颜色
  Output:         
  Return:         
  Others:         
*************************************************/
void put_polygon(XM_POINT2D *ps, int n)
{
	int i;


	for (i = 0; i < n; i++)
	{
		gr_p[i+1].x = ps[i].x;
		gr_p[i+1].y = ps[i].y;
	}
	
	gr_fill_area(gr_p, n,FillColor);
	gr_bigger(gr_p, n,Color);

}





/*************************************************
  Function:				put_rectangle
  Description:			绘一个矩形
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				left		左边界
                        top			上边界
						right		右边界
						bottom		下边界
  Output:         
  Return:         
  Others:         
*************************************************/
void put_rectangle(int left, int top, int right, int bottom)
{
	gr_rect( left, top, right, bottom,Color);
}




/*************************************************
  Function:				set_clip_rectangle
  Description:			设置裁剪区域
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				left		左边界
                        top			上边界
						right		右边界
						bottom		下边界
  Output:         
  Return:         
  Others:         
*************************************************/
void set_clip_rectangle(int left, int top, int right, int botton)
{
	gr_set_clip_rect(left, top, right, botton);
}

/*************************************************
  Function:				flush_graph
  Description:			把缓冲区的内容显示在屏幕上
  Calls:           
  Input:				
  Output:         
  Return:         
  Others:         
*************************************************/
void flush_graph()
{
	gr_graph();
}
