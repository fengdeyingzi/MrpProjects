/************************************************************
  FileName: xmmath.h
  Description:			    ����ģ�ͱ任�����ӿ�
***********************************************************/


#ifndef _XMMATH
#define _XMMATH




void set_XeYe(XM_VIEW *view);       /* ������ο��������� */
void set_ABCD(XM_VIEW *view);       /* �����淽�̲��� */

void point_prj(XM_VIEW *view, XM_POINT *point);           /* ����ͶӰ */

void point_move(XM_POINT *point, XM_POINT *offset);      /* ʹ��ƽ�� */
void point_revolve(XM_POINT *point, XM_POINT *center, float angle);   /* ʹ������һ����������ת angle �� */
void point_revolve_x(XM_POINT *point, float angle);  /* ʹ���� X ����ת angle �� */
void point_revolve_y(XM_POINT *point, float angle);  /* ʹ���� Y ����ת angle �� */
void point_revolve_z(XM_POINT *point, float angle);  /* ʹ���� Z ����ת angle �� */
void translation(XM_POINT *point, XM_POINT *offset); /* ʹ��ƽ�� */
void proportion_change(XM_POINT *point, float proportion);		/* ���������� */


void model_move(XM_MODEL *model, XM_POINT *offset);         /* ʹģ��ƽ�� */
void model_revolve(XM_MODEL *model, XM_POINT *center, float angle);       /* ʹģ����һ����������תangle �� */
void model_revolve_x(XM_MODEL *model, float angle);     /* ʹģ���� X ����ת angle �� */
void model_revolve_y(XM_MODEL *model, float angle);     /* ʹģ���� Y ����ת angle �� */
void model_revolve_z(XM_MODEL *model, float angle);     /* ʹģ���� Z ����ת angle �� */
void symmetry(XM_POINT *normal_vector, XM_POINT *normal_point, XM_POINT *point); /*�������ƽ��ĶԳƵ� */

#endif