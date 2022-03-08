/************************************************************
  FileName: model.h
  Description:       ����ģ�͵Ľ�������������  
***********************************************************/
#ifndef _MODEL
#define _MODEL



#define PI ((float)3.1415926) /* Բ���� */
#define SQR2 ((float)1.414) /* ����2��ֵ */ 
#define SQR3 ((float)1.732) /* ����3��ֵ */
#define SQR5 ((float)2.236) /* ����5��ֵ */
#define SQR6 ((float)2.449) /* ����6��ֵ */

/* ��̬����һ���������� */
XM_MODEL *model_tetrahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* ��̬����һ���������� */
XM_MODEL *model_hexahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* ��̬����һ���������� */
XM_MODEL *model_octahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* ��̬����һ����ʮ������ */
XM_MODEL *model_dodecahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* ��̬����һ������ʮ���� */
XM_MODEL *model_icosahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle);

/* ��̬����һ����׼������ */
XM_MODEL *model_curve(XM_POINT *offset, XM_POINT *center, float angle, float a, float b, float c, int latitude, int longitude, enum CURVE_TYPE curve_kind);

/* ��̬����һ������bezier���� */
XM_MODEL *model_threecubedbezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float reference_point[16][3]);

/* ��̬����һ������bezier���� */
XM_MODEL *model_twocubedbezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float reference_point[9][3]);

/* ��̬����һ������bezier���� */
XM_MODEL *model_twobezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, float reference_point[3][3]);

/* ��̬����һ������bezier���� */
XM_MODEL *model_threebezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, float reference_point[4][3]);

/* ��̬����һ������������ */
XM_MODEL *model_leptospira(XM_POINT *offset, XM_POINT *center, float angle, float height, float w, int latitude, int longitude,float R1, float R2);

/* ��̬����һ������ */
XM_MODEL *model_annular(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float r0, float r1);
#endif