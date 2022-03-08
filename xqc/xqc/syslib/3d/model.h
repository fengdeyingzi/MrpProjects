/************************************************************
  FileName: model.h
  Description:       物体模型的建立函数的声明  
***********************************************************/
#ifndef _MODEL
#define _MODEL



#define PI ((float)3.1415926) /* 圆周率 */
#define SQR2 ((float)1.414) /* 根号2的值 */ 
#define SQR3 ((float)1.732) /* 根号3的值 */
#define SQR5 ((float)2.236) /* 根号5的值 */
#define SQR6 ((float)2.449) /* 根号6的值 */

/* 动态建立一个正四面体 */
XM_MODEL *model_tetrahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* 动态建立一个正六面体 */
XM_MODEL *model_hexahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* 动态建立一个正八面体 */
XM_MODEL *model_octahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* 动态建立一个正十二面体 */
XM_MODEL *model_dodecahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* 动态建立一个正二十面体 */
XM_MODEL *model_icosahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* 动态建立一个标准曲球体 */
XM_MODEL *model_curve(XM_POINT *offset, XM_POINT *center, float angle, float a, float b, float c, int latitude, int longitude, enum CURVE_TYPE curve_kind);

/* 动态建立一个三次bezier曲面 */
XM_MODEL *model_threecubedbezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float reference_point[16][3]);

/* 动态建立一个二次bezier曲面 */
XM_MODEL *model_twocubedbezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float reference_point[9][3]);

/* 动态建立一个二次bezier曲线 */
XM_MODEL *model_twobezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, float reference_point[3][3]);

/* 动态建立一个三次bezier曲线 */
XM_MODEL *model_threebezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, float reference_point[4][3]);

/* 动态建立一个螺旋体曲面 */
XM_MODEL *model_leptospira(XM_POINT *offset, XM_POINT *center, float angle, float height, float w, int latitude, int longitude,float R1, float R2);

/* 动态建立一个环面 */
XM_MODEL *model_annular(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float r0, float r1);
#endif