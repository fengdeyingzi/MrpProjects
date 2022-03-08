/************************************************************
  FileName: create.h
  Description:           基本模型动态建立的函数声明
***********************************************************/


#ifndef _CREATE
#define _CREATE





XM_POINT *create_point(float x, float y, float z);          /* 动态建立一个点 */
XM_POINT_NODE *create_point_node(XM_POINT *point);             /* 动态建立点链表结点 */


XM_LINE *create_line(XM_POINT *point1, XM_POINT *point2);    /* 动态建立一条线 */
XM_LINE_NODE *create_line_node(XM_LINE *line);               /* 动态建立线链表结点 */


XM_POLYGON *create_polygon();                          /* 动态建立一个空多边形 */
XM_POLYGON_NODE *create_polygon_node(XM_POLYGON *polygon);       /* 动态建立多边形链表结点 */
void polygon_add_point(XM_POLYGON *polygon, XM_POINT *point);  /* 往多边形中添加一个点 */
/*void polygon_add_line(XM_POLYGON *polygon, XM_LINE *line); */     /* 往多边形中添加一条线 */


XM_MODEL *create_model(XM_POINT *offset);                               /* 动态建立一个空模型 */
XM_MODEL_NODE *create_model_node(XM_MODEL *model);         /* 动态建立模型链表结点 */
void model_add_point(XM_MODEL *model, XM_POINT *point);      /* 往模型中添加一个点 */
void model_add_line(XM_MODEL *model, XM_LINE *line);         /* 往模型中添加一条线 */
void model_add_polygon(XM_MODEL *model, XM_POLYGON *polygon);  /* 往模型中添加一个多边形 */


XM_FIELD *create_field();                            /* 动态建立一个空场景 */
void field_add_model(XM_FIELD *field, XM_MODEL *model);        /* 往场景中添加一个模型 */


XM_VIEW *create_view(XM_POINT *pst, XM_POINT *drt, float width, float height); /* 动态建立一个视点 */

XM_WINDOW *create_window(int left, int top, int width, int height); /* 动态建立一个窗口 */


#endif