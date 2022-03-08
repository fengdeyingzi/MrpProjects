/************************************************************
  FileName: create.h
  Description:           ����ģ�Ͷ�̬�����ĺ�������
***********************************************************/


#ifndef _CREATE
#define _CREATE





XM_POINT *create_point(float x, float y, float z);          /* ��̬����һ���� */
XM_POINT_NODE *create_point_node(XM_POINT *point);             /* ��̬������������ */


XM_LINE *create_line(XM_POINT *point1, XM_POINT *point2);    /* ��̬����һ���� */
XM_LINE_NODE *create_line_node(XM_LINE *line);               /* ��̬������������ */


XM_POLYGON *create_polygon();                          /* ��̬����һ���ն���� */
XM_POLYGON_NODE *create_polygon_node(XM_POLYGON *polygon);       /* ��̬��������������� */
void polygon_add_point(XM_POLYGON *polygon, XM_POINT *point);  /* ������������һ���� */
/*void polygon_add_line(XM_POLYGON *polygon, XM_LINE *line); */     /* ������������һ���� */


XM_MODEL *create_model(XM_POINT *offset);                               /* ��̬����һ����ģ�� */
XM_MODEL_NODE *create_model_node(XM_MODEL *model);         /* ��̬����ģ�������� */
void model_add_point(XM_MODEL *model, XM_POINT *point);      /* ��ģ�������һ���� */
void model_add_line(XM_MODEL *model, XM_LINE *line);         /* ��ģ�������һ���� */
void model_add_polygon(XM_MODEL *model, XM_POLYGON *polygon);  /* ��ģ�������һ������� */


XM_FIELD *create_field();                            /* ��̬����һ���ճ��� */
void field_add_model(XM_FIELD *field, XM_MODEL *model);        /* �����������һ��ģ�� */


XM_VIEW *create_view(XM_POINT *pst, XM_POINT *drt, float width, float height); /* ��̬����һ���ӵ� */

XM_WINDOW *create_window(int left, int top, int width, int height); /* ��̬����һ������ */


#endif