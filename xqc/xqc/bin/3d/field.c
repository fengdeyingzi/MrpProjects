
#include "3d.h"







/*************************************************
  Function:				create_ground
  Description:			建立一个指定长宽的地面
  Calls:				create_model
						create_point
						create_line
						model_add_point
						model_add_line
  Table Accessed:  
  Table Updated:   
  Input:				length	指定的长
						width	指定的宽
  Output:         
  Return:				返回一个地面模型指针
  Others:         
*************************************************/
XM_MODEL *create_ground(int length, int width)
{
	XM_MODEL *new_model;
	XM_POINT *p1;
	XM_POINT *p2;
	XM_LINE *l;
	int i;

	/* 建立一个空模型，并初始化 
	   以原点为中心 */
	new_model = create_model(NULL);
	new_model->offset->x = 0.0;
	new_model->offset->y = 0.0;
	new_model->offset->z = 0.0;

	/* 建立竖直线及对应的点 
	   以坐标系：
	      ↑x
	      ∣
	      ∣
	      ∣          y
	     ——————→
	   为准，Z轴为零 */
	for (i = 0; i <= length; i++)
	{
		p1 = create_point(-(float)length / 2 + i * 1.0, (float)width / 2, 0.0);
		p2 = create_point(-(float)length / 2 + i * 1.0, -(float)width / 2, 0.0);
		l = create_line(p1, p2);
		model_add_point(new_model, p1);
		model_add_point(new_model, p2);
		model_add_line(new_model, l);
	}

	/* 建立水平线及对应的点 
	   以坐标系：
	      ↑x
	      ∣
	      ∣
	      ∣          y
	     ——————→
	   为准，Z轴为零 */
	for (i = 0; i <= width; i++)
	{
		p1 = create_point(-(float)length / 2, -(float)width / 2 + i * 1.0, 0.0);
		p2 = create_point((float)length / 2, -(float)width / 2 + i * 1.0, 0.0);
		l = create_line(p1, p2);
		model_add_point(new_model, p1);
		model_add_point(new_model, p2);
		model_add_line(new_model, l);
	}

	p1 = create_point(-(float)length / 2, -(float)width / 2 + (-3) * 1.0, 0.0);
	p2 = create_point((float)length / 2, -(float)width / 2 + (-3) * 1.0, 0.0);
	l = create_line(p1, p2);
	model_add_point(new_model, p1);
	model_add_point(new_model, p2);
	model_add_line(new_model, l);

	/* 设置模型的绘制方式 
	   以线绘制，该模型是以线为主的，没有多边形图元
	   只有点链与线链 */
	new_model->style = DMS_LINE | DMS_CUT3D;

	return new_model;
};