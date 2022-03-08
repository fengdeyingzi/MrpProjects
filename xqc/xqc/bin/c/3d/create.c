/************************************************************
  FileName: create.c
  Author:夏龙        Version: 1.0         Date:2005-11-2
  Description:       动态建立基本模型           
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
      夏龙    2005-11-2    1.0     动态建立基本模型
	  夏龙	  2009-11-5    1.1     去掉多边形的线链表
***********************************************************/
#include <mrc_base.h>



#include "struct.h"
#include "create.h"

#define  c_Malloc(len) malloc(len)

/*************************************************
  Function:        // create_point
  Description:     // 动态建立一个点
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // x分量
                  // y分量
				  // z分量
  Output:         
  Return:         // XM_POINT类型指针  
  Others:         
*************************************************/
XM_POINT  *create_point(float x, float y, float z)
{
	XM_POINT *new_point = (XM_POINT *)c_Malloc(sizeof(XM_POINT));  /* 动态建立一个点 */
	
	new_point->x = x;               /* X分量 */
	new_point->y = y;				/* Y分量 */
	new_point->z = z;				/* Z分量 */

	return new_point;
}



/*************************************************
  Function:			// create_point_node
  Description:		// 动态建立一个点链表结点
  Calls:           
  Called By:       
  Table Accessed:   
  Table Updated:   
  Input:			// point 对应点的指针
  Output:         
  Return:			// XM_POINT_NODE指针
  Others:         
*************************************************/
XM_POINT_NODE *create_point_node(XM_POINT *point)
{
	XM_POINT_NODE *new_point_node = (XM_POINT_NODE *)c_Malloc(sizeof(XM_POINT_NODE));  /* 新建点链表结点 */

    new_point_node->point = point;     /* 对应的点 */
    new_point_node->next = NULL;       /* 下一结点为空 */
    
    return new_point_node; 
}




/*************************************************
  Function:			// create_line
  Description:		// 动态建立一个线指针
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// point1：第一个端点指针
					// point2：第二个端点指针
  Output:           
  Return:           // XM_LINE指针
  Others:         
*************************************************/
XM_LINE *create_line(XM_POINT *point1, XM_POINT *point2)
{
	XM_LINE *new_line = (XM_LINE *)c_Malloc(sizeof(XM_LINE));      /* 新建线 */

    new_line->p1 = point1;            /* 设置第一个点 */
    new_line->p2 = point2;          /* 设置第二个点 */
    return new_line;
}



/*************************************************
  Function:			// create_line_node
  Description:		// 动态建立一个线链表结点
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// line：对应线指针
                 
  Output:         
  Return:			// XM_LINE_NODE指针
  Others:         
*************************************************/
XM_LINE_NODE *create_line_node(XM_LINE *line)
{
	XM_LINE_NODE *new_line_node = (XM_LINE_NODE *)c_Malloc(sizeof(XM_LINE_NODE));  /* 新建线链表指针 */

    new_line_node->line = line;               /* 对应的线 */
    new_line_node->next = NULL;            /* 下一个对点为空 */

    return new_line_node;
}


/*************************************************
  Function:			// create_polygon
  Description:		// 动态建立一个多边形指针
  Calls:			// create_point_node
					// create_line_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:						
  Output:         
  Return:			// XM_POLYGON指针     
  Others:         
*************************************************/
XM_POLYGON *create_polygon()
{
	XM_POLYGON *new_polygon;

    new_polygon = (XM_POLYGON *)c_Malloc(sizeof(XM_POLYGON));     /* 新建多边形 */

    new_polygon->point_head = create_point_node(NULL);      /* 设置点链表头结点 */
	new_polygon->color = 0xffFFFF;
    /*new_polygon->line_head = create_line_node(NULL);     设置线链表头结点 */

    return new_polygon;
}



/*************************************************
  Function:			// create_polygon_node
  Description:		// 动态建立多边形结点指针
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// polygon 对应多边形指针
					
  Output:         
  Return:			// XM_POLYGON_NODE 指针   
  Others:         
*************************************************/
XM_POLYGON_NODE *create_polygon_node(XM_POLYGON *polygon)      
{
	XM_POLYGON_NODE *new_polygon_node = (XM_POLYGON_NODE *)c_Malloc(sizeof(XM_POLYGON_NODE));     /* 新建多边形链表指针 */

    new_polygon_node->polygon = polygon;   /* 对应的多边形 */
    new_polygon_node->next = NULL;        /* 下一结点为空 */

    return new_polygon_node;	
}




/*************************************************
  Function:			// polygon_add_point
  Description:		// 往多边形中添加点
  Calls:            //create_point_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// polygon：往此多边开中添加一个点
					// point:   把此点添加到到面中 
					
  Output:         
  Return:			 
  Others:         
*************************************************/
void polygon_add_point(XM_POLYGON *polygon, XM_POINT *point)
{
	XM_POINT_NODE *point_node_pt = polygon->point_head;     /* 取头结点 */

    /* 指针移到尾结点 */
    while (point_node_pt->next != NULL)
    {
        point_node_pt = point_node_pt->next;
    }

    point_node_pt->next = create_point_node(point);       /* 连到链表尾 */
}



/*************************************************
  Function:			// polygon_add_line
  Description:		// 往多边形中添加一条线
  Calls:			//create_line_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// polygon：往此面中添加一条线
					// line：   把此线添加到一个polygon面中
  Output:         
  Return:         
  Others:         
*************************************************/
/*void polygon_add_line(XM_POLYGON *polygon, XM_LINE *line)
{
	XM_LINE_NODE *line_node_pt;

    line_node_pt = polygon->line_head;*/

    /* 指针移到尾结点 */
  /*  while (line_node_pt->next != NULL)
    {
        line_node_pt = line_node_pt->next;
    }

    line_node_pt->next = create_line_node(line);
}
*/


/*************************************************
  Function:			// create_model
  Description:		// 动态建立一个模型
  Calls:			//create_point_node
					//create_line_node
					//create_polygon_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:		
  Output:         
  Return:			//XM_MODEL指针  
  Others:         
*************************************************/
XM_MODEL *create_model(XM_POINT *offset)
{
	XM_MODEL *new_model = (XM_MODEL *)c_Malloc(sizeof(XM_MODEL));   /* 新建模型 */

    XM_POINT *new_offset;
    XM_POINT_NODE *point_node_head;             /* 新建点链表头结点 */
    XM_LINE_NODE *line_node_head;              /* 新建线链表头结点 */
    XM_POLYGON_NODE *polygon_node_head;         /* 新建多边形链表头结点 */
    
	point_node_head = create_point_node(NULL);
	line_node_head = create_line_node(NULL);
	polygon_node_head = create_polygon_node(NULL);
	
	new_offset = (XM_POINT *)c_Malloc(sizeof(XM_POINT));

    if (offset == NULL)       /* 若传入的指针为空，则新建一个向量 */
    {
        new_offset->x = 0;
        new_offset->y = 0;
        new_offset->z = 0;
    }
    else           /* 否则设置offset的值 */
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
  Description:		// 动态建立对应模型链表结点
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			//对应模型的指针
  Output:         
  Return:			//XM_MODEL_NODE指针
  Others:         
*************************************************/
XM_MODEL_NODE *create_model_node(XM_MODEL *model)
{
	XM_MODEL_NODE *new_model_node = (XM_MODEL_NODE *)c_Malloc(sizeof(XM_MODEL_NODE));  /* 新建多边形链表结点 */

    new_model_node->model = model;    /* 对应的模型 */
    new_model_node->next = NULL;   /* 下一结点为空 */

    return new_model_node;
}



/*************************************************
  Function:			// model_add_point
  Description:		// 往模型中添加点
  Calls:			// create_point_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// model：往此模型中添加点
					// point: 把此点添加到模型中
				
  Output:         
  Return:         
  Others:         
*************************************************/
void model_add_point(XM_MODEL *model, XM_POINT *point)
{
	XM_POINT_NODE *point_node_pt = model->point_head;       /* 取点链表头结点 */

    /* 指针移到尾结点 */
    while (point_node_pt->next != NULL)
    {
        point_node_pt = point_node_pt->next;
    }

    point_node_pt->next = create_point_node(point);     /* 新建结点连到链表尾 */	
}





/*************************************************
  Function:			// model_add_line
  Description:		// 往模型中添加线
  Calls:			// create_line_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// model：往此模型中添加一条线
					// line： 把此线添加到模型中
  Output:         
  Return:         
  Others:         
*************************************************/
void model_add_line(XM_MODEL *model, XM_LINE *line)
{
	XM_LINE_NODE *line_node_pt = model->line_head;    /* 取线链表头结点 */

    /* 指针移到尾结点 */
    while (line_node_pt->next != NULL)
    {
        line_node_pt = line_node_pt->next;
    }

    line_node_pt->next = create_line_node(line);  /* 新建结点连到链表尾 */
}




/*************************************************
  Function:			// model_add_polygon
  Description:		// 往模型中添加多边形
  Calls:			//create_polygon_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// model：在此模型中添加一个多边形
					// polygon：把此多边形添加到模型中去
  Output:         
  Return:         
  Others:         
*************************************************/
void model_add_polygon(XM_MODEL *model, XM_POLYGON *polygon)
{
	XM_POLYGON_NODE *polygon_node_pt = model->polygon_head;     /* 取链表头结点 */

    /* 指针移到尾结点 */
    while (polygon_node_pt->next !=  NULL)
    {
        polygon_node_pt = polygon_node_pt->next;
    }

    polygon_node_pt->next = create_polygon_node(polygon);   /* 新建结点连到链表尾 */
}




/*************************************************
  Function:			// create_field
  Description:		// 建立一个场景
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			
  Output:			
  Return:			//XM_FIELD指针
  Others:         
*************************************************/
XM_FIELD *create_field()
{
	XM_FIELD *new_field = (XM_FIELD *)c_Malloc(sizeof(XM_FIELD));     /* 新建场景 */

    new_field->model_head = create_model_node(NULL);  /* 新建模型链表头结点 */

    return new_field;
}





/*************************************************
  Function:			// field_add_model
  Description:		// 往场景中添加一个模型
  Calls:			// create_model_node
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// field：往此场景中添加模型
					// model：把此模型添加到场景中
  Output:         
  Return:         
  Others:         
*************************************************/
void field_add_model(XM_FIELD *field, XM_MODEL *model)
{
	XM_MODEL_NODE *model_node_pt = field->model_head;    /* 取模型链表头结点 */

    /* 指针移到链表尾 */
    while (model_node_pt->next != NULL)
    {
        model_node_pt = model_node_pt->next;
    }

    model_node_pt->next = create_model_node(model);
}



/*************************************************
  Function:			// create_view
  Description:		// 动态建立一个视区
  Calls:			//create_point
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// pst：视点位置向量
					// drt：视点方向向量
					// width: 视区的宽度
					// height: 视区的高度
  Output:         
  Return:			// XM_VIEW指针 
  Others:         
*************************************************/
XM_VIEW *create_view(XM_POINT *pst, XM_POINT *drt, float width, float height)
{
	XM_VIEW *new_view;        /* 新视点 */

    XM_POINT *new_pst;    /* 位置向量 */
    XM_POINT *new_drt;    /* 方向向量 */
    XM_POINT *new_Xe;    /* 视面参考坐标系X轴 */
    XM_POINT *new_Ye;     /* 视面参考坐标系Y轴 */

    new_view = (XM_VIEW *)c_Malloc(sizeof(XM_VIEW));     /* 新建视点 */
	
	new_pst = create_point(4.0, 4.0, 4.0);
	new_drt = create_point(-1.0, -1.0, -1.0);

	/* 新建视面坐标系向量 */
    new_Xe = create_point(0.0,0.0,0.0);
    new_Ye = create_point(0.0,0.0,0.0);

    /* 设置位置向量 */
    if (pst != NULL)
    {
        new_pst->x = pst->x;
		new_pst->y = pst->y;
		new_pst->z = pst->z;
    }

    /* 设置方向向量 */
    if (drt != NULL)
	{
		new_drt->x = drt->x;
		new_drt->y = drt->y;
		new_drt->z = drt->z;
	}

    

    /* 初始化视点 */
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
  Description:		// 动态建立一个窗口
  Calls:          
  Called By:      
  Table Accessed:  
  Table Updated:   
  Input:			// left：窗口的左边界
					// top： 窗口的右边界
					// width:窗口的高度
					// height:窗口的高度
  Output:         
  Return:			// XM_WINDOW 指针 
  Others:         
*************************************************/
XM_WINDOW *create_window(int left, int top, int width, int height)
{
	XM_WINDOW *new_window;

    /* 新建一个窗口，为其分配空间 */
    new_window = (XM_WINDOW *)c_Malloc(sizeof(XM_WINDOW));

    /* 初始化窗口 */
    new_window->width = width;
    new_window->height = height;
    new_window->left = left;
    new_window->top = top;

    return new_window;
}
















