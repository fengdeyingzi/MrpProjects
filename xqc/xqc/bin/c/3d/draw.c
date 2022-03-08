
/************************************************************
  FileName: draw.c
  Author: 马海文       Version: 1.0         Date: 2005-11-3
  Description:         模型与场景的绘制  
  Function List:   
    1. draw_model
	2. draw_field
  History:         
      <author>  <date>       <version >   <desc>
      马海文     2005-11-3   1.0           模型与场景的绘制  
***********************************************************/




#include <mrc_base.h>
#include <math.h>

#include "struct.h"
#include "xmmath.h"
#include "graphics.h"
#include "draw.h"


/*************************************************
  Function:				draw_model
  Description:			绘制模型
  Calls:				point_prj
  Table Accessed:  
  Table Updated:   
  Input:				model: 指定的模型
						view: 指定的视点
						window: 指定的窗口
						style: 绘制的样式
  Output:         
  Return:         
  Others:         
*************************************************/
void draw_model(XM_MODEL *model, XM_VIEW *view, XM_WINDOW *window)
{
	XM_POINT_NODE *point_node_pt;         /* 点链表结点指针 */
	XM_LINE_NODE *line_node_pt;            /* 线链表结点指针 */
	XM_POLYGON_NODE *polygon_node_pt;     /* 多边形链表结点指针 */

	float plg_A, plg_B, plg_C, plg_D;    /* 多边形所在平面方程参数 */

	float pst_sign;             /* 视点相对视面的位置标志 */
	float point_sign;          /* 任一点相对视面的位置标志 */

	int offset_x, offset_y;              /* 屏幕窗口坐标的偏移量 */
    float unit_x, unit_y;                /* 屏幕窗口与视区的对应比例 */

	XM_POINT *p1;                 
	XM_POINT *p2;
	XM_POINT *p3;

	float t;

	XM_POINT temp_point;          /* 一临时点，用作直线与视面的交点 */

	XM_POINT2D ps[10];
	int point_count = 0;



	/****************************************************************/
	/*                        多边形的消隐                          */
	/****************************************************************/

	if ((model->style & DMS_HIDE) == DMS_HIDE)     /* 消隐 */
	{
		polygon_node_pt = model->polygon_head;
		while (polygon_node_pt->next != NULL)
		{
			polygon_node_pt = polygon_node_pt->next;
	
			/* 求多边形所在面的方程参数 */
			p1 = polygon_node_pt->polygon->point_head->next->point;
			p2 = polygon_node_pt->polygon->point_head->next->next->point;
			p3 = polygon_node_pt->polygon->point_head->next->next->next->point;

			plg_A = p1->y * (p2->z - p3->z) 
				  + p2->y * (p3->z - p1->z) 
				  + p3->y * (p1->z - p2->z);
			
			plg_B = p1->z * (p2->x - p3->x)
				  + p2->z * (p3->x - p1->x)
				  + p3->z * (p1->x - p2->x);
			
			plg_C = p1->x * (p2->y - p3->y)
				  + p2->x * (p3->y - p1->y)
				  + p3->x * (p1->y - p2->y);

			plg_D = - p1->x * (p2->y * p3->z - p3->y * p2->z)
				    - p2->x * (p3->y * p1->z - p1->y * p3->z)
					- p3->x * (p1->y * p2->z - p2->y * p1->z);
	
			/* 视点相对多边形所在面的位置标记 */
			pst_sign = plg_A * view->pst->x 
				     + plg_B * view->pst->y 
					 + plg_C * view->pst->z 
					 + plg_D;

			point_sign = plg_A * model->offset->x 
				       + plg_B * model->offset->y 
					   + plg_C * model->offset->z 
					   + plg_D;

			if (pst_sign <= 0.0)    /* 多边形可见 */
			{
				polygon_node_pt->polygon->visible = XM_TRUE;
			}
			else       /* 多边形不可见 */
			{
				polygon_node_pt->polygon->visible = XM_FALSE;
			}
		}   

	}   /* 多边形消隐结束 */



	/****************************************************************/
	/*                           投影                               */
	/****************************************************************/
	switch (model->style & (DMS_HIDE | DMS_CUT3D))
	{
	case 0:  /* 不消隐，不裁剪 */
		/* 全部点投影 */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			point_prj(view, point_node_pt->point);
		}
		break;

	case 2:  /* 不消隐，裁剪 */
		/* 设所有点为不可见 */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			point_node_pt->point->visible = XM_FALSE;
		}

		/* 对可见点投影 */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			
			point_sign = view->A * point_node_pt->point->x 
				       + view->B * point_node_pt->point->y
					   + view->C * point_node_pt->point->z 
					   + view->D;

			if (point_sign >= 0.0)
			{
				point_prj(view, point_node_pt->point);
				point_node_pt->point->visible = XM_TRUE;
			}
		}
		break;

	case 1:  /* 消隐，不裁剪 */
		/* 设所有点为不可见 */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			point_node_pt->point->visible = XM_FALSE;
		}

		/* 可见面对应的点投影 */
		polygon_node_pt = model->polygon_head;
		while (polygon_node_pt->next != NULL)
		{
			polygon_node_pt = polygon_node_pt->next;

			if (polygon_node_pt->polygon->visible == XM_TRUE)
			{
				point_node_pt = polygon_node_pt->polygon->point_head;
				while (point_node_pt->next != NULL)
				{
					point_node_pt = point_node_pt->next;
					if (point_node_pt->point->visible == XM_FALSE)
					{
						point_prj(view, point_node_pt->point);
						point_node_pt->point->visible = XM_TRUE;
					}
				}
			}
		}
		break;

	case 3:  /* 消隐，裁剪 */
		/* 设所有点为不可见 */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			point_node_pt->point->visible = XM_FALSE;
		}

		/* 可见面对应的点投影 */
		polygon_node_pt = model->polygon_head;
		while (polygon_node_pt->next != NULL)
		{
			polygon_node_pt = polygon_node_pt->next;

			if (polygon_node_pt->polygon->visible == XM_TRUE)
			{
				point_node_pt = polygon_node_pt->polygon->point_head;
				while (point_node_pt->next != NULL)
				{
					point_node_pt = point_node_pt->next;
					if (point_node_pt->point->visible == XM_FALSE)
					{
						point_prj(view, point_node_pt->point);
						point_node_pt->point->visible = XM_TRUE;
					}
				}
			}
		}

		/* 对可见点投影 */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			
			if (point_node_pt->point->visible == XM_TRUE)
			{
				point_sign = view->A * point_node_pt->point->x 
					       + view->B * point_node_pt->point->y
						   + view->C * point_node_pt->point->z 
						   + view->D;

				if (point_sign >= 0.0)
				{
					point_prj(view, point_node_pt->point);
				}
				else
				{
					point_node_pt->point->visible = XM_FALSE;
				}
			}
		}
		break;


	}   /* 投影结束 */




	/**********************************************************/
	/*                坐标映射参数初始化                      */
	/**********************************************************/

	offset_x = window->left + window->width / 2;
	offset_y = window->top + window->height / 2;
	unit_x = view->width / window->width;
	unit_y = view->height / window->height;


	

	set_color(model->bordercolor);
	/*********************************************************************/
	/*                              绘制模型                             */
	/*********************************************************************/

	switch (model->style & (DMS_LINE | DMS_FRAME | DMS_FILL))
	{

	/******************/
	/* 以线为单位绘制 */
	/******************/
	case DMS_LINE:         
		if ((model->style & DMS_CUT3D) != DMS_CUT3D)    /* 不裁剪 */
		{
			line_node_pt = model->line_head;
			while (line_node_pt->next != NULL)
			{
				line_node_pt = line_node_pt->next;

				put_line((int)(offset_x + line_node_pt->line->p1->prj_x / unit_x),
					     (int)(offset_y - line_node_pt->line->p1->prj_y / unit_y),
						 (int)(offset_x + line_node_pt->line->p2->prj_x / unit_x),
						 (int)(offset_y - line_node_pt->line->p2->prj_y / unit_y));
			}
		}
		else      /* 裁剪 */
		{
			line_node_pt = model->line_head;
			while (line_node_pt->next != NULL)
			{
				line_node_pt = line_node_pt->next;

				p1 = line_node_pt->line->p1;
				p2 = line_node_pt->line->p2;

				if (p1->visible != p2->visible)
				{
					t = - (view->A * p1->x + view->B * p1->y + view->C * p1->z + view->D)
					    / (view->A * (p2->x - p1->x) 
						 + view->B * (p2->y - p1->y) 
						 + view->C * (p2->z - p1->z));

					temp_point.x = (p2->x - p1->x) * t + p1->x;
					temp_point.y = (p2->y - p1->y) * t + p1->y;
					temp_point.z = (p2->z - p1->z) * t + p1->z;

					point_prj(view, &temp_point);

					if (p1->visible == XM_FALSE)
					{
						p1 = &temp_point;
					}
					else
					{
						p2 = &temp_point;
					}

				}
				else if(p1->visible == XM_FALSE && p2->visible == XM_FALSE)
				{
					continue;
				}

				put_line((int)(offset_x + p1->prj_x / unit_x),
					     (int)(offset_y - p1->prj_y / unit_y),
						 (int)(offset_x + p2->prj_x / unit_x),
						 (int)(offset_y - p2->prj_y / unit_y));
			}
		}
		break;

	/******************************/
	/* 以多边形为单位绘制，不填充 */
	/******************************/
	case DMS_FRAME:        
		if ((model->style & DMS_CUT3D) != DMS_CUT3D)    /* 不裁剪 */
		{
			polygon_node_pt = model->polygon_head;
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;

				point_count = 0;
				point_node_pt = polygon_node_pt->polygon->point_head;
				while (point_node_pt->next != NULL)
				{
					point_node_pt = point_node_pt->next;
					ps[point_count].x = (int)(offset_x + point_node_pt->point->prj_x / unit_x);
					ps[point_count].y = (int)(offset_y - point_node_pt->point->prj_y / unit_y);
					point_count++;
				}

				ps[point_count].x = ps[0].x;
				ps[point_count].y = ps[0].y;
				point_count++;

				put_polyline(ps, point_count);
			}
		}
		else      /* 裁剪 */
		{
			polygon_node_pt = model->polygon_head;
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;
				point_count = 0;
				point_node_pt = polygon_node_pt->polygon->point_head->next;
				p2 = point_node_pt->point;
				while (point_node_pt->next != NULL)
				{
					point_node_pt = point_node_pt->next;
					p1 = p2;
					p2 = point_node_pt->point;

					if (p1->visible != p2->visible)
					{
						t = - (view->A * p1->x + view->B * p1->y + view->C * p1->z + view->D)
						    / (view->A * (p2->x - p1->x) 
							 + view->B * (p2->y - p1->y) 
							 + view->C * (p2->z - p1->z));

						temp_point.x = (p2->x - p1->x) * t + p1->x;
						temp_point.y = (p2->y - p1->y) * t + p1->y;
						temp_point.z = (p2->z - p1->z) * t + p1->z;

						point_prj(view, &temp_point);

						ps[point_count].x = (int)(offset_x + temp_point.prj_x / unit_x);
						ps[point_count].y = (int)(offset_y - temp_point.prj_y / unit_y);
						point_count++;
					}

					if (p2->visible == XM_TRUE)
					{
						ps[point_count].x = (int)(offset_x + p2->prj_x / unit_x);
						ps[point_count].y = (int)(offset_y - p2->prj_y / unit_y);
						point_count++;
					}
				}
				p1 = p2;
				p2 = polygon_node_pt->polygon->point_head->next->point;
				if (p1->visible != p2->visible)
				{
					t = - (view->A * p1->x + view->B * p1->y + view->C * p1->z + view->D)
					    / (view->A * (p2->x - p1->x) 
						 + view->B * (p2->y - p1->y) 
						 + view->C * (p2->z - p1->z));

					temp_point.x = (p2->x - p1->x) * t + p1->x;
					temp_point.y = (p2->y - p1->y) * t + p1->y;
					temp_point.z = (p2->z - p1->z) * t + p1->z;

					point_prj(view, &temp_point);

					ps[point_count].x = (int)(offset_x + temp_point.prj_x / unit_x);
					ps[point_count].y = (int)(offset_y - temp_point.prj_y / unit_y);
					point_count++;
				}

				if (p2->visible == XM_TRUE)
				{
					ps[point_count].x = (int)(offset_x + p2->prj_x / unit_x);
					ps[point_count].y = (int)(offset_y - p2->prj_y / unit_y);
					point_count++;
				}

				ps[point_count].x = ps[0].x;
				ps[point_count].y = ps[0].y;
				point_count++;
				put_polyline(ps, point_count);
			}  /* 遍历多边形结束 */

		}   /* 线框绘制结束 */
		break;

	/****************************/
	/* 以多边形为单位绘制，填充 */
	/****************************/
	case DMS_FILL:         
		if ((model->style & DMS_CUT3D) != DMS_CUT3D)    /* 不裁剪 */
		{
			polygon_node_pt = model->polygon_head;
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;

				if (polygon_node_pt->polygon->visible == XM_TRUE)
				{
					point_count = 0;
					point_node_pt = polygon_node_pt->polygon->point_head;
					while (point_node_pt->next != NULL)
					{
						point_node_pt = point_node_pt->next;
						ps[point_count].x = (int)(offset_x + point_node_pt->point->prj_x / unit_x);
						ps[point_count].y = (int)(offset_y - point_node_pt->point->prj_y / unit_y);
						point_count++;
					}

					set_fillcolor(polygon_node_pt->polygon->color);
					put_polygon(ps, point_count);
				}
			}
		}
		else      /* 裁剪 */
		{
			polygon_node_pt = model->polygon_head;
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;

				if (polygon_node_pt->polygon->visible == XM_TRUE)
				{
					point_count = 0;
					point_node_pt = polygon_node_pt->polygon->point_head->next;
					p2 = point_node_pt->point;
					while (point_node_pt->next != NULL)
					{
						point_node_pt = point_node_pt->next;
						p1 = p2;
						p2 = point_node_pt->point;

						if (p1->visible != p2->visible)
						{
							t = - (view->A * p1->x + view->B * p1->y + view->C * p1->z + view->D)
							    / (view->A * (p2->x - p1->x) 
								 + view->B * (p2->y - p1->y) 
								 + view->C * (p2->z - p1->z));

							temp_point.x = (p2->x - p1->x) * t + p1->x;
							temp_point.y = (p2->y - p1->y) * t + p1->y;
							temp_point.z = (p2->z - p1->z) * t + p1->z;

							point_prj(view, &temp_point);

							ps[point_count].x = (int)(offset_x + temp_point.prj_x / unit_x);
							ps[point_count].y = (int)(offset_y - temp_point.prj_y / unit_y);
							point_count++;
						}

						if (p2->visible == XM_TRUE)
						{
							ps[point_count].x = (int)(offset_x + p2->prj_x / unit_x);
							ps[point_count].y = (int)(offset_y - p2->prj_y / unit_y);
							point_count++;
						}
					}
					p1 = p2;
					p2 = polygon_node_pt->polygon->point_head->next->point;
					if (p1->visible != p2->visible)
					{
						t = - (view->A * p1->x + view->B * p1->y + view->C * p1->z + view->D)
						    / (view->A * (p2->x - p1->x) 
							 + view->B * (p2->y - p1->y) 
							 + view->C * (p2->z - p1->z));

						temp_point.x = (p2->x - p1->x) * t + p1->x;
						temp_point.y = (p2->y - p1->y) * t + p1->y;
						temp_point.z = (p2->z - p1->z) * t + p1->z;

						point_prj(view, &temp_point);

						ps[point_count].x = (int)(offset_x + temp_point.prj_x / unit_x);
						ps[point_count].y = (int)(offset_y - temp_point.prj_y / unit_y);
						point_count++;
					}

					if (p2->visible == XM_TRUE)
					{
						ps[point_count].x = (int)(offset_x + p2->prj_x / unit_x);
						ps[point_count].y = (int)(offset_y - p2->prj_y / unit_y);
						point_count++;
					}

					set_fillcolor(polygon_node_pt->polygon->color);
					put_polygon(ps, point_count);
				}  /* if (polygon_node_pt->polygon->visible == XM_TRUE) 结束 */
			}  /* 遍历多边形结束 */

		}   /* 填充绘制结束 */
		break;

	}  /* 绘制模型结束 */

}








/*************************************************
  Function:				draw_field
  Description:			绘制场景
  Calls:				draw_model
  Table Accessed:  
  Table Updated:   
  Input:				field: 指定的场景
						view: 指定的视点
						window: 指定的窗口
						style: 绘制的样式
  Output:         
  Return:         
  Others:         
*************************************************/
void draw_field(XM_FIELD *field, XM_VIEW *view, XM_WINDOW *window)
{
	XM_MODEL *temp_model;

	XM_MODEL_NODE *mn1;

	float drt_length;        /* 视点到视面的距离 */
	float t;

	drt_length = (float)sqrt(view->drt->x * view->drt->x 
		                   + view->drt->y * view->drt->y 
					       + view->drt->z * view->drt->z);

	/* 模型的可见性判断 */
	mn1 = field->model_head;
	while (mn1->next != NULL)
	{
		mn1 = mn1->next;
		
		mn1->model->depth = (float)sqrt((view->pst->x - mn1->model->offset->x)
			                          * (view->pst->x - mn1->model->offset->x)
				                      + (view->pst->y - mn1->model->offset->y)
				                      * (view->pst->y - mn1->model->offset->y)
				                      + (view->pst->z - mn1->model->offset->z)
				                      * (view->pst->z - mn1->model->offset->z));
		
		t = ((mn1->model->offset->x - view->pst->x) * view->drt->x 
		   + (mn1->model->offset->y - view->pst->y) * view->drt->y 
		   + (mn1->model->offset->z - view->pst->z) * view->drt->z) / drt_length;



		/* 判断模型的可见性，并对根据可见模型的位置设置绘制方式 */
		if (t + mn1->model->radius < drt_length)  /* 模型不可见 */
		{
			mn1->model->visible = XM_FALSE;
		}
		else    /* 模型可见 */
		{
			mn1->model->visible = XM_TRUE;
			
			if (t - mn1->model->radius < drt_length)
			{
				if ((mn1->model->style & DMS_LINE) == DMS_LINE 
					|| (mn1->model->style & DMS_FRAME) == DMS_FRAME)
				{
					mn1->model->style = mn1->model->style & (~DMS_HIDE);
						                 
				}
				mn1->model->style = mn1->model->style | DMS_CUT3D;
			}
			else
			{
				mn1->model->style = mn1->model->style & (~DMS_CUT3D);
			}/**/
		}
	}


	/* 遍历所有模型，
	   根据模型的深度决定绘制的顺序 */
	while (1)
	{
		temp_model = NULL;     /* 先设当前要绘制的模型为空 */
		
		/* 遍历所有模型 */
		mn1 = field->model_head;
		while (mn1->next !=	NULL)
		{
			mn1 = mn1->next;

			/* 找出可见的
			   并且深度最大的模型 
			   即远的先画，近的后画，近的自然挡住远的
			   实现模型的遮挡 */
			if (mn1->model->visible == XM_TRUE)       
			{
				if (temp_model == NULL)
				{
					temp_model = mn1->model;
				}

				if (mn1->model->depth > temp_model->depth)
				{
					temp_model = mn1->model;
				}
			}
		}

		/* 若找到了则绘制，然后做标记，设其为不可见，
		   说明该模型已绘制过；
		   若找不到，说明已绘完，直接跳出循环 */
		if (temp_model != NULL)
		{
			draw_model(temp_model, view, window);
			temp_model->visible = XM_FALSE;
		}
		else
		{
			break;
		}
	}
}
