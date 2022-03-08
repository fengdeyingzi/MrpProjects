/***************************************************************************************
  FileName: xmmath.c
  Author: 马海文       Version: 1.1          Date: 2005-11-7
  Description:			   3D模型基本运算函数
                           增加模型变换函数(model_move, model_revolve,
						   model_revolve_x, model_revolve_y, model_revolve_z)
  Function List:   
		1. set_XeYe
		2. set_ABCD
		3. point_prj
		4. point_move
		5. point_revolve
		6. point_revolve_x
		7. point_revolve_y
		8. point_revolve_z
		9. translation
		10. proportion_change
		11. model_move
		12. model_revolve
		13. model_revolve_x
		14. model_revolve_y
		15. model_revolve_z
  History:         
      <author>       <date>       <version >   <desc>
	   马海文        2005-11-4     1.0         3D模型基本运算函数
	   夏龙			 2005-11-5	   1.1         添加了平移和比例变幻函数(translation,proportion)
	   马海文        2005-11-7     1.2         增加模型变换函数
*******************************************************************************************/










#include <mrc_base.h>
#include <math.h>

#include "struct.h"
#include "xmmath.h"



/*************************************************
  Function:        // set_XeYe
  Description:     // 求视面的参考坐标向量
                      根据视点方向向量 view->drt 求得，
                      结果保存在 view->Xe,view->Ye 中
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // view 视点结构指针
  Output:         // view->Xe   视面的参考坐标x轴单位向量
                  // view->Ye   视面的参考坐标y轴单位向量
  Return:           
  Others:         
*************************************************/
void set_XeYe(XM_VIEW *view)
{
    float t1, t2;

    t1 = (float)hypot(view->drt->x, view->drt->y);              /* t1=sqrt(x*x+y*y) */
    t2 = (float)sqrt(view->drt->x * view->drt->x + view->drt->y * view->drt->y + view->drt->z * view->drt->z);     /* t2=sqrt(x*x+y*y+z*z) */

    /* Xe(y/sqrt(x*x+y*y),-x/sqrt(x*x+y*y),0) */
    if(t1 != 0.0)
    {
        view->Xe->x = view->drt->y / t1;
        view->Xe->y = - view->drt->x / t1;
    }
    else
    {
        view->Xe->x = 1.0;
        view->Xe->y = 0.0;
    }
    view->Xe->z = 0.0;

    /* Ye(-x*z/sqrt(x*x+y*y+z*z),-y*z/sqrt(x*x+y*y+z*z),(x*x+y*y)/sqrt(x*x+y*y+z*z)) */
    view->Ye->x = - view->drt->x * view->drt->z / t1 / t2;
    view->Ye->y = - view->drt->y * view->drt->z / t1 / t2;
    view->Ye->z = (view->drt->x * view->drt->x + view->drt->y * view->drt->y) / t1 / t2;
}



/*************************************************
  Function:        // set_ABCD
  Description:     // 求视面方程的参数
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // view 视点结构指针
  Output:         // view->A, view->B, view->C, view->D
                     视点方程的参数
  Return:         
  Others:         
*************************************************/
void set_ABCD(XM_VIEW *view)
{
    view->A = view->drt->x;
    view->B = view->drt->y;
    view->C = view->drt->z;
    view->D = - view->drt->x * (view->pst->x + view->drt->x)
              - view->drt->y * (view->pst->y + view->drt->y)
              - view->drt->z * (view->pst->z + view->drt->z);
}




/*************************************************
  Function:        // point_prj
  Description:     // 求点在视面上的投影坐标
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // view 视点结构指针
                  // point 指定的点指针
  Output:         // point->prj_x 点在视面上的横坐标
                  // point->prj_y 点在视面上的纵坐标
  Return:         
  Others:         
*************************************************/
void point_prj(XM_VIEW *view, XM_POINT *point)
{
    float t;
    float t_x, t_y, t_z;
    float t_x1, t_y1, t_z1;

    /* (Xv,Yv,Zv):视点方向 */
    /* (Xp,Yp,Zp):视点位置 */
    /* (Xn,Yn,Zn):指定的点 */
    /* t=(Xv*Xv+Yv*Yv+Zv*Zv)/[Xv*(Xn-Xp)+Yv*(Yn-Yp)+Zv*(Zn-Zp)] */

    
    t = (view->drt->x * view->drt->x + view->drt->y * view->drt->y + view->drt->z * view->drt->z)
     / (view->drt->x * (point->x - view->pst->x)
     + view->drt->y * (point->y - view->pst->y) 
     + view->drt->z * (point->z - view->pst->z));

    /* 交点 */
    t_x = (point->x - view->pst->x) * t + view->pst->x;
    t_y = (point->y - view->pst->y) * t + view->pst->y;
    t_z = (point->z - view->pst->z) * t + view->pst->z;

    t_x1 = t_x - view->pst->x - view->drt->x;
    t_y1 = t_y - view->pst->y - view->drt->y;
    t_z1 = t_z - view->pst->z - view->drt->z;

    /* 在XeYe上的投影 */
    point->prj_x = t_x1 * view->Xe->x + t_y1 * view->Xe->y + t_z1 * view->Xe->z;
    point->prj_y = t_x1 * view->Ye->x + t_y1 * view->Ye->y + t_z1 * view->Ye->z;
}






/*************************************************
  Function:        // point_revolve
  Description:     // 使点绕任一轴旋转某一角度
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point 指定的点指针
                  // center 中心点向量指针
				  // angle 旋转的角度
  Output:         // point 旋转后的点指针
  Return:         
  Others:         
*************************************************/
extern int  ttttt;
void point_revolve(XM_POINT *point, XM_POINT *center, float angle)
{
    XM_POINT t_center;
    XM_POINT t_point;

    float Xa, Ya, Za,te;
    float Xn, Yn, Zn;

    float A, B;
    float D, D1, D2, D3;

    float t1, t2;

	te=(center->x * center->x + center->y * center->y + center->z * center->z);
	if(te==0.0)
		return;

    t1 = (point->x * center->x + point->y * center->y + point->z * center->z)
        / te;//(center->x * center->x + center->y * center->y + center->z * center->z);
    t_center.x = center->x * t1;
    t_center.y = center->y * t1;
    t_center.z = center->z * t1;

    /* 平移到原点 */
    t_point.x = point->x - t_center.x;
    t_point.y = point->y - t_center.y;
    t_point.z = point->z - t_center.z;

    /* 求向量积所需的两个向量 */
    Xa = t_point.x;
    Ya = t_point.y;
    Za = t_point.z;


    t2 = (float)((Xa * Xa + Ya * Ya + Za * Za) * sin(angle)
        /sqrt(te));// sqrt(center->x * center->x + center->y * center->y + center->z * center->z));
    Xn = center->x * t2;
    Yn = center->y * t2;
    Zn = center->z * t2;

    A = Xn + Yn + Zn;
    B = (float)((Xa * Xa + Ya * Ya + Za * Za) * cos(angle));
	
    /* 求行列式 */
    D = (Ya - Xa) * Xn * Ya + (Xa - Za) * Zn * Xa + (Za - Ya) * Yn * Za
      - (Za - Ya) * Zn * Ya - (Xa - Za) * Xn * Za - (Ya - Xa) * Yn * Xa;
	D2=(Xa - Za) * Zn * B;
	D1=A * Yn * Za - A * Zn * Ya - (Ya - Xa) * Yn * B;
    D1 = D2 + D1;

    D2 = (Ya - Xa) * Xn * B + A * Zn * Xa - (Za - Ya) * Zn * B - A * Xn * Za;
    D3 = A * Xn * Ya + (Za - Ya) * Yn * B - (Xa - Za) * Xn * B - A * Yn * Xa;

	if (D != 0.0)
	{
		/* 解方程 */
		t_point.x = D1 / D;
		t_point.y = D2 / D;
		t_point.z = D3 / D;

		/* 平移到原处 */
		point->x = t_point.x + t_center.x;
		point->y = t_point.y + t_center.y;
		point->z = t_point.z + t_center.z;
	}
}




/*************************************************
  Function:        // point_revolve_x
  Description:     // 使点绕 x 轴旋转某一角度
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point 指定的点指针
				  // angle 旋转的角度
  Output:         // point 旋转后的点指针
  Return:         
  Others:         
*************************************************/
void point_revolve_x(XM_POINT *point, float angle)
{
    float y, z;

    y = (float)(point->y * cos(angle) - point->z * sin(angle));     
    z = (float)(point->z * cos(angle) + point->y * sin(angle));

    point->y = y;
    point->z = z;
}


/*************************************************
  Function:        // point_revolve_y
  Description:     // 使点绕 y 轴旋转某一角度
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point 指定的点指针
				  // angle 旋转的角度
  Output:         // point 旋转后的点指针
  Return:         
  Others:         
*************************************************/
void point_revolve_y(XM_POINT *point, float angle)
{
    float z, x;

    z = (float)(point->z * cos(angle) - point->x * sin(angle));    
    x = (float)(point->x * cos(angle) + point->z * sin(angle));

    point->z = z;
    point->x = x;
}




/*************************************************
  Function:        // point_revolve_z
  Description:     // 使点绕 z 轴旋转某一角度
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point 指定的点指针
				  // angle 旋转的角度
  Output:         // point 旋转后的点指针
  Return:         
  Others:         
*************************************************/
void point_revolve_z(XM_POINT *point, float angle)
{
    float x, y;

    x = (float)(point->x * cos(angle) - point->y * sin(angle));     
    y = (float)(point->y * cos(angle) + point->x * sin(angle));

    point->x = x;
    point->y = y;
}



/*************************************************
  Function:        // translation
  Description:     // 点平移
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point 指定的点指针
				  // offset 平移量
  Output:         // 平移后的点指针 
  Return:         
  Others:         
*************************************************/
void translation(XM_POINT *point, XM_POINT *offset)
{
	/* 三维各自加平移分量 */
	point->x += offset->x;
	point->y += offset->y;
	point->z += offset->z;


}


/*************************************************
  Function:        // proportion_change
  Description:     // 坐标的比例变幻
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point 指定的点指针
				  // proportion 比例
  Output:         //比例变幻后的点指针
  Return:          
  Others:         
*************************************************/
void proportion_change(XM_POINT *point, float proportion)
{
	point->x *= proportion;
	point->y *= proportion;
	point->z *= proportion;


}






/*************************************************
  Function:				point_move
  Description:			使点平移
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				point	指定的点的指针
						offset	偏移量
  Output:				point	平移后的点的指针
  Return:         
  Others:         
*************************************************/
void point_move(XM_POINT *point, XM_POINT *offset)
{
	point->x += offset->x;
	point->y += offset->y;
	point->z += offset->z;
}




/*************************************************
  Function:				model_move
  Description:			使模型平移
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	指定的模型的指针
						offset	偏移量
  Output:				model	平移后的模型的指针
  Return:         
  Others:         
*************************************************/
void model_move(XM_MODEL *model, XM_POINT *offset)
{
	XM_POINT_NODE *point_node_pt;

	point_move(model->offset, offset);/**/

	/* 遍历点 */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* 使点平移 */
		point_move(point_node_pt->point, offset);
	}
}




/*************************************************
  Function:				model_revolve
  Description:			使模型绕一个点向量旋转 angle 角
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	指定的模型的指针
						center	中心点向量的指针
						angle	旋转的角度

  Output:				model	旋转后的模型的指针
  Return:         
  Others:         
*************************************************/
void model_revolve(XM_MODEL *model, XM_POINT *center, float angle)
{
	XM_POINT_NODE *point_node_pt;

	point_revolve(model->offset, center, angle);

	/* 遍历点 */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* 使点旋转 */
		point_revolve(point_node_pt->point, center, angle);
	}
}





/*************************************************
  Function:				model_revolve_x
  Description:			使模型绕 X 轴旋转 angle 角
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	指定的模型的指针
						angle	旋转的角度

  Output:				model	旋转后的模型的指针
  Return:         
  Others:         
*************************************************/
void model_revolve_x(XM_MODEL *model, float angle)
{
	XM_POINT_NODE *point_node_pt;

	point_revolve_x(model->offset, angle);

	/* 遍历点 */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* 使点旋转 */
		point_revolve_x(point_node_pt->point, angle);
	}
}





/*************************************************
  Function:				model_revolve_y
  Description:			使模型绕 Y 轴旋转 angle 角
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	指定的模型的指针
						angle	旋转的角度

  Output:				model	旋转后的模型的指针
  Return:         
  Others:         
*************************************************/
void model_revolve_y(XM_MODEL *model, float angle)
{
	XM_POINT_NODE *point_node_pt;

	point_revolve_y(model->offset, angle);

	/* 遍历点 */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* 使点旋转 */
		point_revolve_y(point_node_pt->point, angle);
	}
}





/*************************************************
  Function:				model_revolve_z
  Description:			使模型绕 Z 轴旋转 angle 角
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	指定的模型的指针
						angle	旋转的角度

  Output:				model	旋转后的模型的指针
  Return:         
  Others:         
*************************************************/
void model_revolve_z(XM_MODEL *model, float angle)
{
	XM_POINT_NODE *point_node_pt;

	point_revolve_z(model->offset, angle);

	/* 遍历点 */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* 使点旋转 */
		point_revolve_z(point_node_pt->point, angle);
	}
}


/*************************************************
  Function:				symmetry
  Description:			点的相对一个平面的对称点
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				normal_vector	法向量
						normal_point	平面上的一点指针
						point	对称前的点指针

  Output:				对称后的点指针
  Return:         
  Others:         
*************************************************/
void symmetry(XM_POINT *normal_vector, XM_POINT *normal_point, XM_POINT *point)
{
	float x, y, z;
	float t;  /* 中间变量 */
	float A, B, C, D; /* 平面方程的四个系数 */

	x = point->x;
	y = point->y;
	z = point->z;

	A = normal_vector->x;
	B = normal_vector->y;
	C = normal_vector->z;
	D = -(A * normal_point->x + B * normal_point->y + C * normal_point->z);

	t = -((A * x + B * y + C * z + D) / (A * A + B * B + C * C));

	point->x = 2 * A * t + x;
	point->y = 2 * B * t + y;
	point->z = 2 * C * t + z;

}