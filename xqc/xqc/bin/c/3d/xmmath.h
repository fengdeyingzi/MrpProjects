/************************************************************
  FileName: xmmath.h
  Description:			    增加模型变换函数接口
***********************************************************/


#ifndef _XMMATH
#define _XMMATH




void set_XeYe(XM_VIEW *view);       /* 求视面参考坐标向量 */
void set_ABCD(XM_VIEW *view);       /* 求视面方程参数 */

void point_prj(XM_VIEW *view, XM_POINT *point);           /* 求点的投影 */

void point_move(XM_POINT *point, XM_POINT *offset);      /* 使点平移 */
void point_revolve(XM_POINT *point, XM_POINT *center, float angle);   /* 使点绕另一个点向量旋转 angle 角 */
void point_revolve_x(XM_POINT *point, float angle);  /* 使点绕 X 轴旋转 angle 角 */
void point_revolve_y(XM_POINT *point, float angle);  /* 使点绕 Y 轴旋转 angle 角 */
void point_revolve_z(XM_POINT *point, float angle);  /* 使点绕 Z 轴旋转 angle 角 */
void translation(XM_POINT *point, XM_POINT *offset); /* 使点平移 */
void proportion_change(XM_POINT *point, float proportion);		/* 坐标比例变幻 */


void model_move(XM_MODEL *model, XM_POINT *offset);         /* 使模型平移 */
void model_revolve(XM_MODEL *model, XM_POINT *center, float angle);       /* 使模型绕一个点向量旋转angle 角 */
void model_revolve_x(XM_MODEL *model, float angle);     /* 使模型绕 X 轴旋转 angle 角 */
void model_revolve_y(XM_MODEL *model, float angle);     /* 使模型绕 Y 轴旋转 angle 角 */
void model_revolve_z(XM_MODEL *model, float angle);     /* 使模型绕 Z 轴旋转 angle 角 */
void symmetry(XM_POINT *normal_vector, XM_POINT *normal_point, XM_POINT *point); /*点相对于平面的对称点 */

#endif