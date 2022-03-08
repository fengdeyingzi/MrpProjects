#include "mrc_base.h"
#include "mrc_bmp.h "
#include "math.h "
#include "x_graphics.h "


/**
 * \宏返回的RECT正确位置
 */
#define RECT_RIGHT(r) (r->left + r->width)

/**
 * \宏返回的矩形底部位置
 */
#define RECT_BOTTOM(r) (r->top + r->height)


//全局变量
RECT scrrect;
int scrrectd;
mr_screeninfo scrpt;
uint16 *SCR_P;



//矩形赋值
void RectRe(RECT *rect,int x,int y,int w,int h)
{
	rect->left=x;
	rect->top=y;
	rect->width=w;
	rect->height=h;
}
//一个点是否在屏幕内
int pointpd(int x,int y)
{
	return x>=scrrect.left && x<scrrect.left+scrrect.width && y>=scrrect.top && y<scrrect.top+scrrect.height;
}
//用于划线判断，自动纠正反向绘制
void Rectlinepd(RECT *rect)
{
	if((rect->width) < 0)
	{
		rect->left+=rect->width;rect->width=-rect->width;
	}
	if((rect->height) < 0)
	{
		rect->top+=rect->height;rect->height -=rect->height;
	}
}
//判断psrcs1与psrcs2是否相交
int Intersect(PRECT psrcs1, PRECT psrcs2)
{
	int left, top, right, bottom;

	RECT psrc1, psrc2;
	psrcs1=memcpy(&psrc1,psrcs1,sizeof(RECT));
	psrcs2=memcpy(&psrc2,psrcs2,sizeof(RECT));
	Rectlinepd(&psrc1);
	Rectlinepd(&psrc2);
	left = (psrc1.left > psrc2.left) ? psrc1.left : psrc2.left;
	top  = (psrc1.top > psrc2.top) ? psrc1.top : psrc2.top;
	right = (RECT_RIGHT(psrcs1) < RECT_RIGHT(psrcs2)) ? RECT_RIGHT(psrcs1) : RECT_RIGHT(psrcs2);
	bottom = (RECT_BOTTOM(psrcs1) < RECT_BOTTOM(psrcs2)) 
		? RECT_BOTTOM(psrcs1) : RECT_BOTTOM(psrcs2);

	if(left > right || top > bottom)
		return FALSE;

	return TRUE;
}
//pdst与psrc1相交区域赋值给psrc2
void sectRect(PRECT pdst, PRECT psrc1, PRECT psrc2)
{
	int left, top, right, bottom;

	left = (psrc1->left > psrc2->left) ? psrc1->left : psrc2->left;
	top  = (psrc1->top > psrc2->top) ? psrc1->top : psrc2->top;
	right = (RECT_RIGHT(psrc1) < RECT_RIGHT(psrc2)) ? RECT_RIGHT(psrc1) : RECT_RIGHT(psrc2);
	bottom = (RECT_BOTTOM(psrc1) < RECT_BOTTOM(psrc2)) 
		? RECT_BOTTOM(psrc1) : RECT_BOTTOM(psrc2);

	pdst->left = left;
	pdst->top = top;
	pdst->width = right - left + 1;
	pdst->height = bottom - top + 1;
}



//判断rect是否在屏幕内
#define PD_SCRRECT(rect) Intersect(&scrrect,rect)
//纠正负面矩形
#define SET_SMRECT(pdst) sectRect(pdst,&scrrect,pdst)



#define rgb32(r,g,b) (0xff000000|(b<<16))|(g<<8)|r

void rgb24(uint16 n,mr_colourSt *r)
{
	uint8 m;
	m=n>>8&0xf8;
	r->b=m|(m>>5);
	m=n>>3&0xfe;
	r->g=m|(m>>6);
	m=n<<3&0xf8;
	r->r=m|(m>>5);
}
#define rgb16(r,g,b) (r>>3<<11)+(g>>2<<5)+(b>>3)
//清空屏幕
void gr_cls(mr_colourSt color)
{
	mrc_drawRect(scrrect.left, scrrect.top, scrrect.width, scrrect.height,color.r,color.g,color.b);
}
//画点
void gr_point(int x,int y,uint16 color)
{
	RECT rect;
	if(scrrectd==1)
	{
	RectRe(&rect,x,y,1,1);
	if(!PD_SCRRECT(&rect))
		return;
	}
	mrc_drawPoint(x,y,color);
}
//与gr_point相似，不同在于颜色参数
void gr_pointex(int x,int y,mr_colourSt color)
{
	gr_point(x,y,rgb16(color.r,color.g,color.b));
}
//画线
void gr_line(int x,int y,int x1,int y1,mr_colourSt color)
{
	RECT rect;
	RectRe(&rect,x,y,x1-x,y1-y);

	if(!PD_SCRRECT(&rect))
		return;

	SET_SMRECT(&rect);
	mrc_drawLine(rect.left,rect.top,rect.width+rect.left,rect.height+rect.top,color.r,color.g,color.b);
}
//画矩形
void gr_rect(int x,int y,int w,int h,mr_colourSt color)
{
	RECT rect;
	RectRe(&rect,x,y,w,h);

	if(!PD_SCRRECT(&rect))
		return;

	SET_SMRECT(&rect);
	mrc_drawRect(rect.left,rect.top,rect.width+rect.left,rect.height+rect.top,color.r,color.g,color.b);
}


//画多边形开始

EACH_ENTRY sides[MAX_POINT]; 


int side_count,first_s,last_s,bottomscan,scan,x_int_count; 
//判断多边形是否在屏幕内
int polygonrect(EACH_POINT *zb,int n)
{
	RECT rect;int i;
	rect.left=zb[1].x;
	for(i=2;i<=n;i++)
		if(zb[i].x<rect.left)
			rect.left=zb[i].x;

	rect.width=zb[1].x;
	for(i=2;i<=n;i++)
		if(zb[i].x>rect.width)
			rect.width=zb[i].x;

	rect.top=zb[1].y;
	for(i=2;i<=n;i++)
		if(zb[i].y<rect.top)
			rect.top=zb[i].y;

	rect.height=zb[1].y;
	for(i=2;i<=n;i++)
		if(zb[i].y>rect.height)
			rect.height=zb[i].y;
	if(rect.left<0)
		i=0;
	rect.width-=rect.left;
	rect.height-=rect.top;
	Rectlinepd(&rect);

	return PD_SCRRECT(&rect);
}

void update_sides_list()
{ 
	int k;   
	for(k=first_s;k<last_s+1;k++) 
	{
		if(sides[k].delta_y>0)    
		{
			sides[k].delta_y--;
			sides[k].x_int-=sides[k].x_change_per_scan;   
		} 
	} 
} 
void draw_lines(int scan,int x_int_count,int index,mr_colourSt color) 
{      
	int k,x1,x2;
	for(k=1;k<(int)(x_int_count/2+1.5);k++) 
	{
		while(sides[index].delta_y==0)    
			index++;
		x1=(int)(sides[index].x_int+0.5);
		index++;
		while(sides[index].delta_y==0)    
			index++;    
		x2=(int)(sides[index].x_int+0.5);    

		gr_line(x1,scan,x2,scan,color);
		index++; 
	} 
} 
void swap1(EACH_ENTRY *x,EACH_ENTRY *y) 
{
	int i_temp;
	float f_temp; 
	i_temp=x->y_top;
	x->y_top=y->y_top;
	y->y_top=i_temp; 
	f_temp=x->x_int;
	x->x_int=y->x_int;y->x_int=f_temp; 
	i_temp=x->delta_y;x->delta_y=y->delta_y;
	y->delta_y=i_temp; 
	f_temp=x->x_change_per_scan;
	x->x_change_per_scan=y->x_change_per_scan; 
	y->x_change_per_scan=f_temp; 
} 

void sort_on_x(int entry,int first_s) 
{
	while((entry>first_s)&&(sides[entry].x_int<sides[entry-1].x_int)) 
	{
		swap1(&sides[entry],&sides[entry-1]);
		entry--;
	} 
} 

void process_x_intersections(int first_s,int last_s) 
{
	int k;
	x_int_count=0; 
	for(k=first_s;k<last_s+1;k++) 
	{
		if(sides[k].delta_y>0)    
		{
			x_int_count++;    
			sort_on_x(k,first_s);    
		} 
	} 
} 
void update_first_and_last(int count,int scan) 
{ 
	while((sides[last_s+1].y_top>=scan)&&(last_s<count))    
		last_s++;
	while(sides[first_s].delta_y==0)   
		first_s++; 
} 

void put_in_sides_list(int entry,int x1,int y1,int x2,int y2,int next_y) 
{
	int maxy; float x2_temp,x_change_temp;
	x_change_temp=(float)(x2-x1)/(float)(y2-y1); 
	x2_temp=x2;
	if((y2>y1)&&(y2<next_y)) 
	{
		y2--;
		x2_temp-=x_change_temp;
	}
	else
	{
		if((y2<y1)&&(y2>next_y))
		{
			y2++;
			x2_temp+=x_change_temp;
		}
	}
	maxy=(y1>y2)? y1:y2;
	while((entry>1)&&(maxy>sides[entry-1].y_top))
	{
		sides[entry]=sides[entry-1];
		entry--;
	}
	sides[entry].y_top=maxy; 
	sides[entry].delta_y=abs(y2-y1)+1; 
	if(y1>y2)
		sides[entry].x_int=x1; 
	else
		sides[entry].x_int=x2_temp; 
	sides[entry].x_change_per_scan=x_change_temp; 
} 

void sort_on_bigger_y(EACH_POINT *zb,int n,mr_colourSt color) 
{
	int k,x1,y1;int tem; 
	side_count=0;
	y1=zb[n].y; 
	x1=zb[n].x;
	bottomscan=zb[n].y; 
	for(k=1;k<n+1;k++) 
	{
		if(y1!=zb[k].y)
		{
			side_count++;
			if(k!=n)
				tem=zb[k+1].y;
			else
				tem=zb[1].y;
			put_in_sides_list(side_count,x1,y1,zb[k].x,zb[k].y,tem); 

		}
		else
		{
			gr_line(x1,y1,zb[k].x,zb[k].y,color);
		}
		if(zb[k].y<bottomscan)
			bottomscan=zb[k].y;
		y1=zb[k].y;x1=zb[k].x;
	} 
} 



//画多边形主函数
void gr_fill_area(EACH_POINT *zb,int count,mr_colourSt color) 
{ 
	if(count<3)
		return;
	if(!polygonrect(zb,count))
		return;
	memset(sides,0,sizeof(EACH_ENTRY)*MAX_POINT);
	sort_on_bigger_y(zb,count,color);
	first_s=1; last_s=1; 
	for(scan=sides[1].y_top;scan>=bottomscan;scan--) 
	{ 
		update_first_and_last(count,scan);    
		process_x_intersections(first_s,last_s);    
		draw_lines(scan,x_int_count,first_s,color);    
		update_sides_list();
	}
} 
//画多边形结束

//所有点相连
void gr_bigger(EACH_POINT *zb,int n,mr_colourSt color) 
{
	int k,x1,y1; 
	side_count=0;
	y1=zb[n].y; 
	x1=zb[n].x;
	bottomscan=zb[n].y; 
	for(k=1;k<n+1;k++) 
	{
		gr_line(x1,y1,zb[k].x,zb[k].y,color);
		y1=zb[k].y;x1=zb[k].x;
	} 
} 


void gr_Circle(int r,int x0,int y0,mr_colourSt colors)
{
	//使用正负法画圆
	int x, y, f;//(x,y)为当前坐标 f为误差值
	uint16 color;
	color=rgb16(colors.r,colors.g,colors.b);
	x = x0;
	y = y0 + r;
	f = 0;

	while(y >= y0){//从圆周的右上部分开始
		gr_point( x, y,color);//对称地画出四个象限内的坐标点
		gr_point( 2 * x0 - x, y,color);
		gr_point( x, 2 * y0 -y,color);
		gr_point( 2 * x0 - x, 2 * y0 -y,color);

		if(f > 0){
			f = f - 2 * (y - y0) + 1;
			y--;//向圆内走
		}
		else{
			f = f + 2 * (x - x0) + 1;
			x++;//向圆外走
		}
	}

	if(y == y0)
		gr_point( x, y,color);
}

void gr_Ellipse(int x0,int y0,int a,int b,mr_colourSt colors)
{
	//使用中点算法画椭圆
	int a1 = a * a, b1 = b * b;
	int a2 = 2 * a1, b2 = 2 * b1;
	int x = 0, y = b;
	int d;
	int dx = 0, dy = a2 * y;
	uint16 color;
	color=rgb16(colors.r,colors.g,colors.b);
	d = (int)(b1 + a1 * (-b + 0.25) + 0.5);

	gr_point( x0 + x, y0 + y,color);
	gr_point( x0 - x, y0 + y,color);
	gr_point( x0 + x, y0 - y,color);
	gr_point( x0 - x, y0 - y,color);

	while(dx < dy){
		x++;
		dx += b2;
		if(d < 0)
			d += b1 + dx;
		else{
			dy -= a2;
			d += b1 + dx - dy;
			y--;
		}
		gr_point( x0 + x, y0 + y,color);
		gr_point( x0 - x, y0 + y,color);
		gr_point( x0 + x, y0 - y,color);
		gr_point( x0 - x, y0 - y,color);
	}
	d =(int)(b1 * (x + 0.5) * (x + 0.5) + a1 * (y - 1) * (y - 1) - a1 * b1 - 0.5);

	while(y > 0)
	{
		y--;
		dy -= a2;
		if(d > 0)
			d += a1 - dy;
		else{
			x++;
			dx += b2;
			d += a1 - dy + dx;
		}
		gr_point( x0 + x, y0 + y,color);
		gr_point( x0 - x, y0 + y,color);
		gr_point( x0 + x, y0 - y,color);
		gr_point( x0 - x, y0 - y,color);
	}
}

//获取屏幕一个位置的颜色
uint16 getpixel(int x,int y)
{
	if(x>=0&&y>=0&&x<(int)(scrpt.width)&&y<(int)(scrpt.height))
		return *(SCR_P+y*scrpt.width+x);
	else
		return -1;
}

void gr_Flood_Fill_4( int x, int y, uint16 old_color, uint16 new_color)
{
	if(!pointpd(x,y))
		return;
	//对区域使用给定颜色进行填充 四方向的Flood算法
	if(getpixel( x, y) == old_color){
		mrc_drawPoint( x, y, new_color);//如果当前像素点是旧颜色 则将其改变为欲填充颜色

		gr_Flood_Fill_4( x, y + 1, old_color, new_color);//递归地填充与当前像素相邻的四个点
		gr_Flood_Fill_4( x, y - 1, old_color, new_color);
		gr_Flood_Fill_4( x - 1, y, old_color, new_color);
		gr_Flood_Fill_4( x + 1, y, old_color, new_color);
	}
}
//设置裁剪区域
void gr_set_clip_rect(int left, int top, int right, int botton)
{
	RectRe(&scrrect,left, top, right, botton);
	if(left!=0||top!=0||right!=scrpt.width||botton!=scrpt.height)
		scrrectd=1;
	else
		scrrectd=0;

}
//把缓冲区的内容显示在屏幕上
void gr_graph()
{
	mrc_refreshScreen(scrrect.left, scrrect.top, scrrect.width, scrrect.height);
}

//图形库初始化
void gr_init()
{
	if(mrc_getScreenInfo(&scrpt)==MR_FAILED)
	{
		scrpt.bit=16;
		scrpt.height=320;scrpt.width=240;
	}
	RectRe(&scrrect,0,0,scrpt.width,scrpt.height);
	scrrectd=0;
	SCR_P=w_getScreenBuffer();
}

//关闭图形库
void gr_close()
{

}

