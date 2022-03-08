/***************************************************************************************
  FileName: xmmath.c
  Author: ����       Version: 1.1          Date: 2005-11-7
  Description:			   3Dģ�ͻ������㺯��
                           ����ģ�ͱ任����(model_move, model_revolve,
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
	   ����        2005-11-4     1.0         3Dģ�ͻ������㺯��
	   ����			 2005-11-5	   1.1         �����ƽ�ƺͱ�����ú���(translation,proportion)
	   ����        2005-11-7     1.2         ����ģ�ͱ任����
*******************************************************************************************/










#include <mrc_base.h>
#include <math.h>

#include "struct.h"
#include "xmmath.h"



/*************************************************
  Function:        // set_XeYe
  Description:     // ������Ĳο���������
                      �����ӵ㷽������ view->drt ��ã�
                      ��������� view->Xe,view->Ye ��
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // view �ӵ�ṹָ��
  Output:         // view->Xe   ����Ĳο�����x�ᵥλ����
                  // view->Ye   ����Ĳο�����y�ᵥλ����
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
  Description:     // �����淽�̵Ĳ���
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // view �ӵ�ṹָ��
  Output:         // view->A, view->B, view->C, view->D
                     �ӵ㷽�̵Ĳ���
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
  Description:     // ����������ϵ�ͶӰ����
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // view �ӵ�ṹָ��
                  // point ָ���ĵ�ָ��
  Output:         // point->prj_x ���������ϵĺ�����
                  // point->prj_y ���������ϵ�������
  Return:         
  Others:         
*************************************************/
void point_prj(XM_VIEW *view, XM_POINT *point)
{
    float t;
    float t_x, t_y, t_z;
    float t_x1, t_y1, t_z1;

    /* (Xv,Yv,Zv):�ӵ㷽�� */
    /* (Xp,Yp,Zp):�ӵ�λ�� */
    /* (Xn,Yn,Zn):ָ���ĵ� */
    /* t=(Xv*Xv+Yv*Yv+Zv*Zv)/[Xv*(Xn-Xp)+Yv*(Yn-Yp)+Zv*(Zn-Zp)] */

    
    t = (view->drt->x * view->drt->x + view->drt->y * view->drt->y + view->drt->z * view->drt->z)
     / (view->drt->x * (point->x - view->pst->x)
     + view->drt->y * (point->y - view->pst->y) 
     + view->drt->z * (point->z - view->pst->z));

    /* ���� */
    t_x = (point->x - view->pst->x) * t + view->pst->x;
    t_y = (point->y - view->pst->y) * t + view->pst->y;
    t_z = (point->z - view->pst->z) * t + view->pst->z;

    t_x1 = t_x - view->pst->x - view->drt->x;
    t_y1 = t_y - view->pst->y - view->drt->y;
    t_z1 = t_z - view->pst->z - view->drt->z;

    /* ��XeYe�ϵ�ͶӰ */
    point->prj_x = t_x1 * view->Xe->x + t_y1 * view->Xe->y + t_z1 * view->Xe->z;
    point->prj_y = t_x1 * view->Ye->x + t_y1 * view->Ye->y + t_z1 * view->Ye->z;
}






/*************************************************
  Function:        // point_revolve
  Description:     // ʹ������һ����תĳһ�Ƕ�
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point ָ���ĵ�ָ��
                  // center ���ĵ�����ָ��
				  // angle ��ת�ĽǶ�
  Output:         // point ��ת��ĵ�ָ��
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

    /* ƽ�Ƶ�ԭ�� */
    t_point.x = point->x - t_center.x;
    t_point.y = point->y - t_center.y;
    t_point.z = point->z - t_center.z;

    /* ��������������������� */
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
	
    /* ������ʽ */
    D = (Ya - Xa) * Xn * Ya + (Xa - Za) * Zn * Xa + (Za - Ya) * Yn * Za
      - (Za - Ya) * Zn * Ya - (Xa - Za) * Xn * Za - (Ya - Xa) * Yn * Xa;
	D2=(Xa - Za) * Zn * B;
	D1=A * Yn * Za - A * Zn * Ya - (Ya - Xa) * Yn * B;
    D1 = D2 + D1;

    D2 = (Ya - Xa) * Xn * B + A * Zn * Xa - (Za - Ya) * Zn * B - A * Xn * Za;
    D3 = A * Xn * Ya + (Za - Ya) * Yn * B - (Xa - Za) * Xn * B - A * Yn * Xa;

	if (D != 0.0)
	{
		/* �ⷽ�� */
		t_point.x = D1 / D;
		t_point.y = D2 / D;
		t_point.z = D3 / D;

		/* ƽ�Ƶ�ԭ�� */
		point->x = t_point.x + t_center.x;
		point->y = t_point.y + t_center.y;
		point->z = t_point.z + t_center.z;
	}
}




/*************************************************
  Function:        // point_revolve_x
  Description:     // ʹ���� x ����תĳһ�Ƕ�
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point ָ���ĵ�ָ��
				  // angle ��ת�ĽǶ�
  Output:         // point ��ת��ĵ�ָ��
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
  Description:     // ʹ���� y ����תĳһ�Ƕ�
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point ָ���ĵ�ָ��
				  // angle ��ת�ĽǶ�
  Output:         // point ��ת��ĵ�ָ��
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
  Description:     // ʹ���� z ����תĳһ�Ƕ�
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point ָ���ĵ�ָ��
				  // angle ��ת�ĽǶ�
  Output:         // point ��ת��ĵ�ָ��
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
  Description:     // ��ƽ��
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point ָ���ĵ�ָ��
				  // offset ƽ����
  Output:         // ƽ�ƺ�ĵ�ָ�� 
  Return:         
  Others:         
*************************************************/
void translation(XM_POINT *point, XM_POINT *offset)
{
	/* ��ά���Լ�ƽ�Ʒ��� */
	point->x += offset->x;
	point->y += offset->y;
	point->z += offset->z;


}


/*************************************************
  Function:        // proportion_change
  Description:     // ����ı������
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // point ָ���ĵ�ָ��
				  // proportion ����
  Output:         //������ú�ĵ�ָ��
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
  Description:			ʹ��ƽ��
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				point	ָ���ĵ��ָ��
						offset	ƫ����
  Output:				point	ƽ�ƺ�ĵ��ָ��
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
  Description:			ʹģ��ƽ��
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	ָ����ģ�͵�ָ��
						offset	ƫ����
  Output:				model	ƽ�ƺ��ģ�͵�ָ��
  Return:         
  Others:         
*************************************************/
void model_move(XM_MODEL *model, XM_POINT *offset)
{
	XM_POINT_NODE *point_node_pt;

	point_move(model->offset, offset);/**/

	/* ������ */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* ʹ��ƽ�� */
		point_move(point_node_pt->point, offset);
	}
}




/*************************************************
  Function:				model_revolve
  Description:			ʹģ����һ����������ת angle ��
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	ָ����ģ�͵�ָ��
						center	���ĵ�������ָ��
						angle	��ת�ĽǶ�

  Output:				model	��ת���ģ�͵�ָ��
  Return:         
  Others:         
*************************************************/
void model_revolve(XM_MODEL *model, XM_POINT *center, float angle)
{
	XM_POINT_NODE *point_node_pt;

	point_revolve(model->offset, center, angle);

	/* ������ */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* ʹ����ת */
		point_revolve(point_node_pt->point, center, angle);
	}
}





/*************************************************
  Function:				model_revolve_x
  Description:			ʹģ���� X ����ת angle ��
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	ָ����ģ�͵�ָ��
						angle	��ת�ĽǶ�

  Output:				model	��ת���ģ�͵�ָ��
  Return:         
  Others:         
*************************************************/
void model_revolve_x(XM_MODEL *model, float angle)
{
	XM_POINT_NODE *point_node_pt;

	point_revolve_x(model->offset, angle);

	/* ������ */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* ʹ����ת */
		point_revolve_x(point_node_pt->point, angle);
	}
}





/*************************************************
  Function:				model_revolve_y
  Description:			ʹģ���� Y ����ת angle ��
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	ָ����ģ�͵�ָ��
						angle	��ת�ĽǶ�

  Output:				model	��ת���ģ�͵�ָ��
  Return:         
  Others:         
*************************************************/
void model_revolve_y(XM_MODEL *model, float angle)
{
	XM_POINT_NODE *point_node_pt;

	point_revolve_y(model->offset, angle);

	/* ������ */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* ʹ����ת */
		point_revolve_y(point_node_pt->point, angle);
	}
}





/*************************************************
  Function:				model_revolve_z
  Description:			ʹģ���� Z ����ת angle ��
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				model	ָ����ģ�͵�ָ��
						angle	��ת�ĽǶ�

  Output:				model	��ת���ģ�͵�ָ��
  Return:         
  Others:         
*************************************************/
void model_revolve_z(XM_MODEL *model, float angle)
{
	XM_POINT_NODE *point_node_pt;

	point_revolve_z(model->offset, angle);

	/* ������ */
	point_node_pt = model->point_head;
	while (point_node_pt->next != NULL)
	{
		point_node_pt = point_node_pt->next;

		/* ʹ����ת */
		point_revolve_z(point_node_pt->point, angle);
	}
}


/*************************************************
  Function:				symmetry
  Description:			������һ��ƽ��ĶԳƵ�
  Calls:           
  Table Accessed:   
  Table Updated:   
  Input:				normal_vector	������
						normal_point	ƽ���ϵ�һ��ָ��
						point	�Գ�ǰ�ĵ�ָ��

  Output:				�Գƺ�ĵ�ָ��
  Return:         
  Others:         
*************************************************/
void symmetry(XM_POINT *normal_vector, XM_POINT *normal_point, XM_POINT *point)
{
	float x, y, z;
	float t;  /* �м���� */
	float A, B, C, D; /* ƽ�淽�̵��ĸ�ϵ�� */

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