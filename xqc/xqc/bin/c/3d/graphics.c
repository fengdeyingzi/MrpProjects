/************************************************************
  FileName: graphics.c
  Description:           ͼ�κ�����ʵ��
***********************************************************/






#include <mrc_base.h>
#include <mrc_bmp.h>
#include "struct.h"
#include "graphics.h"
#include "x_graphics.h "


EACH_POINT  gr_p[MAX_POINT];


/* ����24λ����RGB888λR��G��B���*/
#define PIXEL888RED(pixel)        (uint8)(((pixel) >> 16) & 0xff)
#define PIXEL888GREEN(pixel)      (uint8)(((pixel) >> 8) & 0xff)
#define PIXEL888BLUE(pixel)       (uint8)((pixel) & 0xff)

/* 
 * ������ɫֵ 
 * mr_colourSt color;
 */
#define SET_COLOR(color, pixel)	\
	do{	\
		color.b = PIXEL888RED(pixel);	\
		color.g = PIXEL888GREEN(pixel);	\
		color.r = PIXEL888BLUE(pixel);	\
	}while(0)


mr_colourSt Color,FillColor;




void set_color(XM_COLOR color)      /* ������ɫ */
{
	SET_COLOR(Color,color);
}

void set_fillcolor(XM_COLOR color)       /* ���������ɫ */
{
	SET_COLOR(FillColor,color);
}

/*************************************************
  Function:				init_graph
  Description:			��ʼ��ͼ��ģʽ
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
  Description:			��һ����
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				x		��ĺ�����
                        y		���������
						color	ָ������ɫ
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
  Description:			��һ����
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				x1		��һ���˵�ĺ�����
                        y1		��һ���˵��������
						x2		�ڶ����˵�ĺ�����
						y2		�ڶ����˵��������
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
  Description:			��ָ�������е�����
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				ps		�������ͷָ��
                        n		��ĸ���
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
  Description:			��һ�����Ķ����
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				ps		���������ͷָ��
                        n		����ĸ���
						color	�����ɫ
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
  Description:			��һ������
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				left		��߽�
                        top			�ϱ߽�
						right		�ұ߽�
						bottom		�±߽�
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
  Description:			���òü�����
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				left		��߽�
                        top			�ϱ߽�
						right		�ұ߽�
						bottom		�±߽�
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
  Description:			�ѻ�������������ʾ����Ļ��
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
