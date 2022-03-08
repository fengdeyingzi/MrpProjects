/************************************************************
  FileName: draw.h
  Description:           ģ�ͳ������ƺ����ӿ�
***************************************************************/

#ifndef _DRAW
#define _DRAW

#define DMS_HIDE 1         /* ������� */
#define DMS_CUT3D 2         /* ����ü���� */ 
#define DMS_CUT2D 4         /* �����ü���� */
#define DMS_LINE 8       /* �߿���� */
#define DMS_FRAME 16       /* ������ */
#define DMS_FILL 32       /* ���ͻ��� */

void draw_model(XM_MODEL *model, XM_VIEW *view, XM_WINDOW *window);    /* ����ģ�� */
void draw_field(XM_FIELD *field, XM_VIEW *view, XM_WINDOW *window);        /* ���Ƴ��� */

#endif