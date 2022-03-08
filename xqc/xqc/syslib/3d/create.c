/************************************************************
  FileName: create.c
  Author:����        Version: 1.0         Date:2005-11-2
  Description:       ��̬��������ģ��           
  Version:           1.0
  Function List:   
    1.  create_point
	2.  create_point_node
	3.  create_line
	4.  create_line_node
	5.  create_polygon
	6.  create_polygon_node
	7.  polygon_add_point
	8.  polygon_add_line
	9.  create_model
	10. create_model_node
	11. model_add_point
	12. model_add_line
	13. model_add_polygon
	14. create_field
	15. field_add_model
	16. create_view
	17. create_window

  History:         
      <author>  <date>   <version >   <desc>
      ����    2005-11-2    1.0     ��̬��������ģ��
	  ����	  2009-11-5    1.1     ȥ������ε�������
***********************************************************/
#include <mrc_base.h>



#include "struct.h"
#include "create.h"

#define  c_Malloc(len) malloc(len)

/*************************************************
  Function:        // create_point
  Description:     // ��̬����һ����
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // x����
                  // y����
				  // z����
  Output:         
  Return:         // XM_POINT����ָ��  
  Others:         
*************************************************/
XM_POINT  *create_point(float x, float y, float z)
{
	XM_POINT *new_point = (XM_POINT *)c_Malloc(sizeof(XM_POINT));  /* ��̬����һ���� */
	
	new_point->x = x;               /* X���� */
	new_point->y = y;				/* Y���� */
	new_point->z = z;				/* Z���� */

	return new_point;
}



/*************************************************
  Function:			// create_point_node
  Description:		// ��̬����һ����������
  Calls:           
  Called By:       
  Table Accessed:   
  Table Updated:   
  Input:			// point ��Ӧ���ָ��
  Output:         
  Return:			// XM_POINT_NODEָ��
  Others:         
*************************************************/
XM_POINT_NODE *create_point_node(XM_POINT *point)
{
	XM_POINT_NODE *new_point_node = (XM_POINT_NODE *)c_Malloc(sizeof(XM_POINT_NODE));  /* �½��������� */

    new_point_node->point = point;     /* ��Ӧ�ĵ� */
    new_point_node->next = NULL;       /* ��һ���Ϊ�� */
    
    return new_point_node; 
}




/*************************************************
  Function:			// create_line
  Description:		// ��̬����һ����ָ��
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// point1����һ���˵�ָ��
					// point2���ڶ����˵�ָ��
  Output:           
  Return:           // XM_LINEָ��
  Others:         
*************************************************/
XM_LINE *create_line(XM_POINT *point1, XM_POINT *point2)
{
	XM_LINE *new_line = (XM_LINE *)c_Malloc(sizeof(XM_LINE));      /* �½��� */

    new_line->p1 = point1;            /* ���õ�һ���� */
    new_line->p2 = point2;          /* ���õڶ����� */
    return new_line;
}



/*************************************************
  Function:			// create_line_node
  Description:		// ��̬����һ����������
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// line����Ӧ��ָ��
                 
  Output:         
  Return:			// XM_LINE_NODEָ��
  Others:         
*************************************************/
XM_LINE_NODE *create_line_node(XM_LINE *line)
{
	XM_LINE_NODE *new_line_node = (XM_LINE_NODE *)c_Malloc(sizeof(XM_LINE_NODE));  /* �½�������ָ�� */

    new_line_node->line = line;               /* ��Ӧ���� */
    new_line_node->next = NULL;            /* ��һ���Ե�Ϊ�� */

    return new_line_node;
}


/*************************************************
  Function:			// create_polygon
  Description:		// ��̬����һ�������ָ��
  Calls:			// create_point_node
					// create_line_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:						
  Output:         
  Return:			// XM_POLYGONָ��     
  Others:         
*************************************************/
XM_POLYGON *create_polygon()
{
	XM_POLYGON *new_polygon;

    new_polygon = (XM_POLYGON *)c_Malloc(sizeof(XM_POLYGON));     /* �½������ */

    new_polygon->point_head = create_point_node(NULL);      /* ���õ�����ͷ��� */
	new_polygon->color = 0xffFFFF;
    /*new_polygon->line_head = create_line_node(NULL);     ����������ͷ��� */

    return new_polygon;
}



/*************************************************
  Function:			// create_polygon_node
  Description:		// ��̬��������ν��ָ��
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// polygon ��Ӧ�����ָ��
					
  Output:         
  Return:			// XM_POLYGON_NODE ָ��   
  Others:         
*************************************************/
XM_POLYGON_NODE *create_polygon_node(XM_POLYGON *polygon)      
{
	XM_POLYGON_NODE *new_polygon_node = (XM_POLYGON_NODE *)c_Malloc(sizeof(XM_POLYGON_NODE));     /* �½����������ָ�� */

    new_polygon_node->polygon = polygon;   /* ��Ӧ�Ķ���� */
    new_polygon_node->next = NULL;        /* ��һ���Ϊ�� */

    return new_polygon_node;	
}




/*************************************************
  Function:			// polygon_add_point
  Description:		// �����������ӵ�
  Calls:            //create_point_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// polygon�����˶�߿������һ����
					// point:   �Ѵ˵���ӵ������� 
					
  Output:         
  Return:			 
  Others:         
*************************************************/
void polygon_add_point(XM_POLYGON *polygon, XM_POINT *point)
{
	XM_POINT_NODE *point_node_pt = polygon->point_head;     /* ȡͷ��� */

    /* ָ���Ƶ�β��� */
    while (point_node_pt->next != NULL)
    {
        point_node_pt = point_node_pt->next;
    }

    point_node_pt->next = create_point_node(point);       /* ��������β */
}



/*************************************************
  Function:			// polygon_add_line
  Description:		// ������������һ����
  Calls:			//create_line_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// polygon�������������һ����
					// line��   �Ѵ�����ӵ�һ��polygon����
  Output:         
  Return:         
  Others:         
*************************************************/
/*void polygon_add_line(XM_POLYGON *polygon, XM_LINE *line)
{
	XM_LINE_NODE *line_node_pt;

    line_node_pt = polygon->line_head;*/

    /* ָ���Ƶ�β��� */
  /*  while (line_node_pt->next != NULL)
    {
        line_node_pt = line_node_pt->next;
    }

    line_node_pt->next = create_line_node(line);
}
*/


/*************************************************
  Function:			// create_model
  Description:		// ��̬����һ��ģ��
  Calls:			//create_point_node
					//create_line_node
					//create_polygon_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:		
  Output:         
  Return:			//XM_MODELָ��  
  Others:         
*************************************************/
XM_MODEL *create_model(XM_POINT *offset)
{
	XM_MODEL *new_model = (XM_MODEL *)c_Malloc(sizeof(XM_MODEL));   /* �½�ģ�� */

    XM_POINT *new_offset;
    XM_POINT_NODE *point_node_head;             /* �½�������ͷ��� */
    XM_LINE_NODE *line_node_head;              /* �½�������ͷ��� */
    XM_POLYGON_NODE *polygon_node_head;         /* �½����������ͷ��� */
    
	point_node_head = create_point_node(NULL);
	line_node_head = create_line_node(NULL);
	polygon_node_head = create_polygon_node(NULL);
	
	new_offset = (XM_POINT *)c_Malloc(sizeof(XM_POINT));

    if (offset == NULL)       /* �������ָ��Ϊ�գ����½�һ������ */
    {
        new_offset->x = 0;
        new_offset->y = 0;
        new_offset->z = 0;
    }
    else           /* ��������offset��ֵ */
    {
        new_offset->x = offset->x;
		new_offset->y = offset->y;
		new_offset->z = offset->z;
    }

    new_model->offset = new_offset;
    new_model->point_head = point_node_head;
    new_model->line_head = line_node_head;
    new_model->polygon_head = polygon_node_head;
	new_model->bordercolor = 0x00FF00;

    return new_model;	
}


/*************************************************
  Function:			// create_model_node 
  Description:		// ��̬������Ӧģ��������
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			//��Ӧģ�͵�ָ��
  Output:         
  Return:			//XM_MODEL_NODEָ��
  Others:         
*************************************************/
XM_MODEL_NODE *create_model_node(XM_MODEL *model)
{
	XM_MODEL_NODE *new_model_node = (XM_MODEL_NODE *)c_Malloc(sizeof(XM_MODEL_NODE));  /* �½������������ */

    new_model_node->model = model;    /* ��Ӧ��ģ�� */
    new_model_node->next = NULL;   /* ��һ���Ϊ�� */

    return new_model_node;
}



/*************************************************
  Function:			// model_add_point
  Description:		// ��ģ������ӵ�
  Calls:			// create_point_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// model������ģ������ӵ�
					// point: �Ѵ˵���ӵ�ģ����
				
  Output:         
  Return:         
  Others:         
*************************************************/
void model_add_point(XM_MODEL *model, XM_POINT *point)
{
	XM_POINT_NODE *point_node_pt = model->point_head;       /* ȡ������ͷ��� */

    /* ָ���Ƶ�β��� */
    while (point_node_pt->next != NULL)
    {
        point_node_pt = point_node_pt->next;
    }

    point_node_pt->next = create_point_node(point);     /* �½������������β */	
}





/*************************************************
  Function:			// model_add_line
  Description:		// ��ģ���������
  Calls:			// create_line_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// model������ģ�������һ����
					// line�� �Ѵ�����ӵ�ģ����
  Output:         
  Return:         
  Others:         
*************************************************/
void model_add_line(XM_MODEL *model, XM_LINE *line)
{
	XM_LINE_NODE *line_node_pt = model->line_head;    /* ȡ������ͷ��� */

    /* ָ���Ƶ�β��� */
    while (line_node_pt->next != NULL)
    {
        line_node_pt = line_node_pt->next;
    }

    line_node_pt->next = create_line_node(line);  /* �½������������β */
}




/*************************************************
  Function:			// model_add_polygon
  Description:		// ��ģ������Ӷ����
  Calls:			//create_polygon_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// model���ڴ�ģ�������һ�������
					// polygon���Ѵ˶������ӵ�ģ����ȥ
  Output:         
  Return:         
  Others:         
*************************************************/
void model_add_polygon(XM_MODEL *model, XM_POLYGON *polygon)
{
	XM_POLYGON_NODE *polygon_node_pt = model->polygon_head;     /* ȡ����ͷ��� */

    /* ָ���Ƶ�β��� */
    while (polygon_node_pt->next !=  NULL)
    {
        polygon_node_pt = polygon_node_pt->next;
    }

    polygon_node_pt->next = create_polygon_node(polygon);   /* �½������������β */
}




/*************************************************
  Function:			// create_field
  Description:		// ����һ������
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			
  Output:			
  Return:			//XM_FIELDָ��
  Others:         
*************************************************/
XM_FIELD *create_field()
{
	XM_FIELD *new_field = (XM_FIELD *)c_Malloc(sizeof(XM_FIELD));     /* �½����� */

    new_field->model_head = create_model_node(NULL);  /* �½�ģ������ͷ��� */

    return new_field;
}





/*************************************************
  Function:			// field_add_model
  Description:		// �����������һ��ģ��
  Calls:			// create_model_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// field�����˳��������ģ��
					// model���Ѵ�ģ����ӵ�������
  Output:         
  Return:         
  Others:         
*************************************************/
void field_add_model(XM_FIELD *field, XM_MODEL *model)
{
	XM_MODEL_NODE *model_node_pt = field->model_head;    /* ȡģ������ͷ��� */

    /* ָ���Ƶ�����β */
    while (model_node_pt->next != NULL)
    {
        model_node_pt = model_node_pt->next;
    }

    model_node_pt->next = create_model_node(model);
}



/*************************************************
  Function:			// create_view
  Description:		// ��̬����һ������
  Calls:			//create_point
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// pst���ӵ�λ������
					// drt���ӵ㷽������
					// width: �����Ŀ��
					// height: �����ĸ߶�
  Output:         
  Return:			// XM_VIEWָ�� 
  Others:         
*************************************************/
XM_VIEW *create_view(XM_POINT *pst, XM_POINT *drt, float width, float height)
{
	XM_VIEW *new_view;        /* ���ӵ� */

    XM_POINT *new_pst;    /* λ������ */
    XM_POINT *new_drt;    /* �������� */
    XM_POINT *new_Xe;    /* ����ο�����ϵX�� */
    XM_POINT *new_Ye;     /* ����ο�����ϵY�� */

    new_view = (XM_VIEW *)c_Malloc(sizeof(XM_VIEW));     /* �½��ӵ� */
	
	new_pst = create_point(4.0, 4.0, 4.0);
	new_drt = create_point(-1.0, -1.0, -1.0);

	/* �½���������ϵ���� */
    new_Xe = create_point(0.0,0.0,0.0);
    new_Ye = create_point(0.0,0.0,0.0);

    /* ����λ������ */
    if (pst != NULL)
    {
        new_pst->x = pst->x;
		new_pst->y = pst->y;
		new_pst->z = pst->z;
    }

    /* ���÷������� */
    if (drt != NULL)
	{
		new_drt->x = drt->x;
		new_drt->y = drt->y;
		new_drt->z = drt->z;
	}

    

    /* ��ʼ���ӵ� */
    new_view->pst = new_pst;
    new_view->drt = new_drt;
    new_view->Xe = new_Xe;
    new_view->Ye = new_Ye;
    new_view->width = width;
    new_view->height = height;

    return new_view;
}




/*************************************************
  Function:			// create_window
  Description:		// ��̬����һ������
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// left�����ڵ���߽�
					// top�� ���ڵ��ұ߽�
					// width:���ڵĸ߶�
					// height:���ڵĸ߶�
  Output:         
  Return:			// XM_WINDOW ָ�� 
  Others:         
*************************************************/
XM_WINDOW *create_window(int left, int top, int width, int height)
{
	XM_WINDOW *new_window;

    /* �½�һ�����ڣ�Ϊ�����ռ� */
    new_window = (XM_WINDOW *)c_Malloc(sizeof(XM_WINDOW));

    /* ��ʼ������ */
    new_window->width = width;
    new_window->height = height;
    new_window->left = left;
    new_window->top = top;

    return new_window;
}
















