
/************************************************************
  FileName: draw.c
  Author: ����       Version: 1.0         Date: 2005-11-3
  Description:         ģ���볡���Ļ���  
  Function List:   
    1. draw_model
	2. draw_field
  History:         
      <author>  <date>       <version >   <desc>
      ����     2005-11-3   1.0           ģ���볡���Ļ���  
***********************************************************/




#include <mrc_base.h>
#include <math.h>

#include "struct.h"
#include "xmmath.h"
#include "graphics.h"
#include "draw.h"


/*************************************************
  Function:				draw_model
  Description:			����ģ��
  Calls:				point_prj
  Table Accessed:  
  Table Updated:   
  Input:				model: ָ����ģ��
						view: ָ�����ӵ�
						window: ָ���Ĵ���
						style: ���Ƶ���ʽ
  Output:         
  Return:         
  Others:         
*************************************************/
void draw_model(XM_MODEL *model, XM_VIEW *view, XM_WINDOW *window)
{
	XM_POINT_NODE *point_node_pt;         /* ��������ָ�� */
	XM_LINE_NODE *line_node_pt;            /* ��������ָ�� */
	XM_POLYGON_NODE *polygon_node_pt;     /* �����������ָ�� */

	float plg_A, plg_B, plg_C, plg_D;    /* ���������ƽ�淽�̲��� */

	float pst_sign;             /* �ӵ���������λ�ñ�־ */
	float point_sign;          /* ��һ����������λ�ñ�־ */

	int offset_x, offset_y;              /* ��Ļ���������ƫ���� */
    float unit_x, unit_y;                /* ��Ļ�����������Ķ�Ӧ���� */

	XM_POINT *p1;                 
	XM_POINT *p2;
	XM_POINT *p3;

	float t;

	XM_POINT temp_point;          /* һ��ʱ�㣬����ֱ��������Ľ��� */

	XM_POINT2D ps[10];
	int point_count = 0;



	/****************************************************************/
	/*                        ����ε�����                          */
	/****************************************************************/

	if ((model->style & DMS_HIDE) == DMS_HIDE)     /* ���� */
	{
		polygon_node_pt = model->polygon_head;
		while (polygon_node_pt->next != NULL)
		{
			polygon_node_pt = polygon_node_pt->next;
	
			/* ������������ķ��̲��� */
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
	
			/* �ӵ���Զ�����������λ�ñ�� */
			pst_sign = plg_A * view->pst->x 
				     + plg_B * view->pst->y 
					 + plg_C * view->pst->z 
					 + plg_D;

			point_sign = plg_A * model->offset->x 
				       + plg_B * model->offset->y 
					   + plg_C * model->offset->z 
					   + plg_D;

			if (pst_sign <= 0.0)    /* ����οɼ� */
			{
				polygon_node_pt->polygon->visible = XM_TRUE;
			}
			else       /* ����β��ɼ� */
			{
				polygon_node_pt->polygon->visible = XM_FALSE;
			}
		}   

	}   /* ������������� */



	/****************************************************************/
	/*                           ͶӰ                               */
	/****************************************************************/
	switch (model->style & (DMS_HIDE | DMS_CUT3D))
	{
	case 0:  /* �����������ü� */
		/* ȫ����ͶӰ */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			point_prj(view, point_node_pt->point);
		}
		break;

	case 2:  /* ���������ü� */
		/* �����е�Ϊ���ɼ� */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			point_node_pt->point->visible = XM_FALSE;
		}

		/* �Կɼ���ͶӰ */
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

	case 1:  /* ���������ü� */
		/* �����е�Ϊ���ɼ� */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			point_node_pt->point->visible = XM_FALSE;
		}

		/* �ɼ����Ӧ�ĵ�ͶӰ */
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

	case 3:  /* �������ü� */
		/* �����е�Ϊ���ɼ� */
		point_node_pt = model->point_head;
		while (point_node_pt->next != NULL)
		{
			point_node_pt = point_node_pt->next;
			point_node_pt->point->visible = XM_FALSE;
		}

		/* �ɼ����Ӧ�ĵ�ͶӰ */
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

		/* �Կɼ���ͶӰ */
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


	}   /* ͶӰ���� */




	/**********************************************************/
	/*                ����ӳ�������ʼ��                      */
	/**********************************************************/

	offset_x = window->left + window->width / 2;
	offset_y = window->top + window->height / 2;
	unit_x = view->width / window->width;
	unit_y = view->height / window->height;


	

	set_color(model->bordercolor);
	/*********************************************************************/
	/*                              ����ģ��                             */
	/*********************************************************************/

	switch (model->style & (DMS_LINE | DMS_FRAME | DMS_FILL))
	{

	/******************/
	/* ����Ϊ��λ���� */
	/******************/
	case DMS_LINE:         
		if ((model->style & DMS_CUT3D) != DMS_CUT3D)    /* ���ü� */
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
		else      /* �ü� */
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
	/* �Զ����Ϊ��λ���ƣ������ */
	/******************************/
	case DMS_FRAME:        
		if ((model->style & DMS_CUT3D) != DMS_CUT3D)    /* ���ü� */
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
		else      /* �ü� */
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
			}  /* ��������ν��� */

		}   /* �߿���ƽ��� */
		break;

	/****************************/
	/* �Զ����Ϊ��λ���ƣ���� */
	/****************************/
	case DMS_FILL:         
		if ((model->style & DMS_CUT3D) != DMS_CUT3D)    /* ���ü� */
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
		else      /* �ü� */
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
				}  /* if (polygon_node_pt->polygon->visible == XM_TRUE) ���� */
			}  /* ��������ν��� */

		}   /* �����ƽ��� */
		break;

	}  /* ����ģ�ͽ��� */

}








/*************************************************
  Function:				draw_field
  Description:			���Ƴ���
  Calls:				draw_model
  Table Accessed:  
  Table Updated:   
  Input:				field: ָ���ĳ���
						view: ָ�����ӵ�
						window: ָ���Ĵ���
						style: ���Ƶ���ʽ
  Output:         
  Return:         
  Others:         
*************************************************/
void draw_field(XM_FIELD *field, XM_VIEW *view, XM_WINDOW *window)
{
	XM_MODEL *temp_model;

	XM_MODEL_NODE *mn1;

	float drt_length;        /* �ӵ㵽����ľ��� */
	float t;

	drt_length = (float)sqrt(view->drt->x * view->drt->x 
		                   + view->drt->y * view->drt->y 
					       + view->drt->z * view->drt->z);

	/* ģ�͵Ŀɼ����ж� */
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



		/* �ж�ģ�͵Ŀɼ��ԣ����Ը��ݿɼ�ģ�͵�λ�����û��Ʒ�ʽ */
		if (t + mn1->model->radius < drt_length)  /* ģ�Ͳ��ɼ� */
		{
			mn1->model->visible = XM_FALSE;
		}
		else    /* ģ�Ϳɼ� */
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


	/* ��������ģ�ͣ�
	   ����ģ�͵���Ⱦ������Ƶ�˳�� */
	while (1)
	{
		temp_model = NULL;     /* ���赱ǰҪ���Ƶ�ģ��Ϊ�� */
		
		/* ��������ģ�� */
		mn1 = field->model_head;
		while (mn1->next !=	NULL)
		{
			mn1 = mn1->next;

			/* �ҳ��ɼ���
			   �����������ģ�� 
			   ��Զ���Ȼ������ĺ󻭣�������Ȼ��סԶ��
			   ʵ��ģ�͵��ڵ� */
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

		/* ���ҵ�������ƣ�Ȼ������ǣ�����Ϊ���ɼ���
		   ˵����ģ���ѻ��ƹ���
		   ���Ҳ�����˵���ѻ��ֱ꣬������ѭ�� */
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
