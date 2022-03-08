/************************************************************
  FileName: model.c
  Author:夏龙        Version: 1.0         Date:2005-11-2
  Description:       物体模型的建立           
  Version:           1.0
  Function List: 
	1.model_tetrahedron
	2.model_hexahedron
	3.model_octahedron
	4.model_dodecahedron
	5.model_icosahedron
	6.model_curve
	7.model_threecubedbezier
	8.model_twocubedbezier
	9.model_twobezier
	10.model_threebezier
	11.model_leptospira
	12.model_annular
    

  History:         
      <author>  <date>   <version >   <desc>
      夏龙    2005-11-5    1.0     物体模型的建立
***********************************************************/





#include <mrc_base.h>
#include <math.h>

#include "struct.h"
#include "create.h"
#include "xmmath.h"
#include "model.h"







/*************************************************
  Function:        // model_tetrahedron
  Description:     // 建立一个正四面体
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // proportion: 物体缩放比例
				  // angle: 旋转角度
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_tetrahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle)
{
	int i,j;

    enum XM_MODEL_TYPE type = TETRAHEDRON;	/* 正四面体 */

    XM_MODEL *model=create_model(offset);	/* 动态建立一个模型 */

    XM_POINT *ps[4];	/* 正四面体有四个点 */
	XM_LINE *line[6];	/* 正四面体有六条线 */
    XM_POLYGON *plgs[4];	/* 正四面体有四个面 */
	
	/* 四个点坐标 */
    float  t_coordinate[4][3] = {{0,0,1},{2*SQR2/3,0,-1.0/3},{-SQR2/3,SQR6/3,-1.0/3},{-SQR2/3,-SQR6/3,-1.0/3}};


	int line_point_mark[6][2] = {{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}};	/* 6条线分别由那些点构成 */
    int polygon_point_mark[4][3] = {{2,0,1},{3,0,2},{3,1,0},{3,2,1}};	/* 四个面当中的点标记 */
	/*int polygon_line_mark[4][3]={{0,1,3},{1,2,5},{0,4,2},{3,5,4}};	 四个面当中的线标记 */
	
	model->type = type;	/* 模型类型 */
	model->radius = proportion;	/* 模型的包围球 */

	/* 动态建立点 */
    for (i = 0; i < 4; i++)
    {
        ps[i] = create_point(t_coordinate[i][0], t_coordinate[i][1], t_coordinate[i][2]);

    }

	/* 对点进行比例变幻，平移，旋转 */
	for (i = 0; i < 4; i++)
	{
		proportion_change(ps[i], proportion);	/* 比例变幻 */
		point_revolve(ps[i], center, angle);
		translation(ps[i], offset);	/* 平移 */
		

	}
	
	/* 动态建立线 */
	for (i = 0; i < 6 ; i++)
	{
		line[i] = create_line(ps[line_point_mark[i][0]], ps[line_point_mark[i][1]]);
	}

	/* 动态建立面 */
    for(j = 0; j < 4; j++)
    {
        plgs[j] = create_polygon();

    }

	/* 往面添加点 */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 3 ; j++)
		{
			polygon_add_point(plgs[i], ps[polygon_point_mark[i][j]]);
		}
	}

	/* 往面添加线 
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 3 ; j++)
		{
			polygon_add_line(plgs[i], line[polygon_line_mark[i][j]]);
		}
	}*/
	
    /* 往模型中添加点 */
	for (i = 0; i < 4; i++)
	{
		model_add_point(model, ps[i]);
	}

	/* 往模型中添加线 */
	for (i = 0; i < 6; i++)
	{
		model_add_line(model, line[i]);
	}

	/* 往模型中添加面 */
	for (i = 0; i < 4; i++)
	{
		model_add_polygon(model, plgs[i]);
	}

    return model;

}



/*************************************************
  Function:        // model_hexahedron
  Description:     // 建立一个正六面体
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // proportion: 物体缩放比例
				  // angle: 旋转角度
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_hexahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle)
{
	int i,j;

    enum XM_MODEL_TYPE type = HEXAHEDRON;	/* 正六面体 */

    XM_MODEL *model=create_model(offset);	/* 动态建立一个模型 */

    XM_POINT *ps[8];	/* 正六面体有8个点 */
	XM_LINE *line[12];	/* 正六面体有12条线 */
    XM_POLYGON *plgs[6];	/* 正六面体有6个面 */
	
	/* 8个点坐标 */
	float t_coordinate[8][3]={{-1/SQR3,-1/SQR3,-1/SQR3},{1/SQR3,-1/SQR3,-1/SQR3},
                            {1/SQR3,1/SQR3,-1/SQR3},{-1/SQR3,1/SQR3,-1/SQR3},
                            {-1/SQR3,-1/SQR3,1/SQR3},{1/SQR3,-1/SQR3,1/SQR3},
                            {1/SQR3,1/SQR3,1/SQR3},{-1/SQR3,1/SQR3,1/SQR3}};

	int line_point_mark[12][2] = {{0,3},{3,2},{1,2},{1,0},{1,5},{5,4},
									{4,0},{4,7},{7,3},{5,6},{2,6},{6,7}};	/* 12条线分别由那些点构成 */

    int polygon_point_mark[6][4]={{1,2,3,0},{4,5,1,0},{3,7,4,0},
									{2,1,5,6},{7,3,2,6},{5,4,7,6}};	/* 6个面当中的点标记 */

/*	int polygon_line_mark[6][4]={{0,1,2,3},{3,4,5,6},{6,7,8,0},
									{9,4,2,10,},{10,1,8,11},{11,7,5,9}};	 6个面当中的线标记 */
	
	model->type = type;	/* 模型类型 */
	model->radius = proportion;	/* 模型的包围球 */
	
	/* 动态建立点 */
    for (i = 0; i < 8; i++)
    {
        ps[i] = create_point(t_coordinate[i][0], t_coordinate[i][1], t_coordinate[i][2]);

    }

	/* 对点进行比例变幻，平移，旋转 */
	for (i = 0; i < 8; i++)
	{
		proportion_change(ps[i], proportion);	/* 比例变幻 */
		translation(ps[i], model->offset);	/* 平移 */
		point_revolve(ps[i], center, angle);

	}
	
	/* 动态建立线 */
	for (i = 0; i < 12 ; i++)
	{
		line[i] = create_line(ps[line_point_mark[i][0]], ps[line_point_mark[i][1]]);
	}

	/* 动态建立面 */
    for(j = 0; j < 6; j++)
    {
        plgs[j] = create_polygon();

    }

	/* 往面添加点 */
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 4 ; j++)
		{
			polygon_add_point(plgs[i], ps[polygon_point_mark[i][j]]);
		}
	}

	/* 往面添加线 
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 4 ; j++)
		{
			polygon_add_line(plgs[i], line[polygon_line_mark[i][j]]);
		}
	}*/
	
    /* 往模型中添加点 */
	for (i = 0; i < 8; i++)
	{
		model_add_point(model, ps[i]);
	}

	/* 往模型中添加线 */
	for (i = 0; i < 12; i++)
	{
		model_add_line(model, line[i]);
	}

	/* 往模型中添加面 */
	for (i = 0; i < 6; i++)
	{
		model_add_polygon(model, plgs[i]);
	}

    return model;

}



/*************************************************
  Function:        // model_octahedron
  Description:     // 建立一个正八面体
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // proportion: 物体缩放比例
				  // angle: 旋转角度
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_octahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle)
{
	int i,j;

    enum XM_MODEL_TYPE type = OCTAHEDRON;	/* 正八面体 */

    XM_MODEL *model=create_model(offset);	/* 动态建立一个模型 */

    XM_POINT *ps[6];	/* 正八面体有6个点 */
	XM_LINE *line[12];	/* 正八面体有12条线 */
    XM_POLYGON *plgs[8];	/* 正八面体有8个面 */
	
	/* 6个点坐标 */
	float t_coordinate[6][3]={{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};

	int line_point_mark[12][2] = {{0,4},{0,2},{4,2},{1,2},{1,4},{1,3},{4,3},{3,0},{5,2},{5,0},{5,1},{5,3}};	/* 12条线分别由那些点构成 */
    int polygon_point_mark[8][3]={{2,0,4},{1,2,4},{3,1,4},{0,3,4},{0,2,5},{2,1,5},{1,3,5},{3,0,5}};	/* 8个面当中的点标记 */
	/*int polygon_line_mark[8][3]={{0,1,2},{2,3,4},{4,5,6},{6,7,0},{8,1,9},{10,3,8},{11,5,10},{9,7,11}};	 8个面当中的线标记 */
	
	model->type = type;	/* 模型类型 */
	model->radius = proportion;	/* 模型的包围球 */
	
	/* 动态建立点 */
    for (i = 0; i < 6; i++)
    {
        ps[i] = create_point(t_coordinate[i][0], t_coordinate[i][1], t_coordinate[i][2]);

    }

	/* 对点进行比例变幻，平移，旋转 */
	for (i = 0; i < 6; i++)
	{
		proportion_change(ps[i], proportion);	/* 比例变幻 */
		translation(ps[i], offset);	/* 平移 */
		point_revolve(ps[i], center, angle);

	}
	
	/* 动态建立线 */
	for (i = 0; i < 12 ; i++)
	{
		line[i] = create_line(ps[line_point_mark[i][0]], ps[line_point_mark[i][1]]);
	}

	/* 动态建立面 */
    for(j = 0; j < 8; j++)
    {
        plgs[j] = create_polygon();

    }

	/* 往面添加点 */
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 3 ; j++)
		{
			polygon_add_point(plgs[i], ps[polygon_point_mark[i][j]]);
		}
	}

	/* 往面添加线 
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 3 ; j++)
		{
			polygon_add_line(plgs[i], line[polygon_line_mark[i][j]]);
		}
	}
	*/
    /* 往模型中添加点 */
	for (i = 0; i < 6; i++)
	{
		model_add_point(model, ps[i]);
	}

	/* 往模型中添加线 */
	for (i = 0; i < 12; i++)
	{
		model_add_line(model, line[i]);
	}

	/* 往模型中添加面 */
	for (i = 0; i < 8; i++)
	{
		model_add_polygon(model, plgs[i]);
	}

    return model;

}




/*************************************************
  Function:        // model_dodecahedron
  Description:     // 建立一个正十二面体
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // proportion: 物体缩放比例
				  // angle: 旋转角度
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_dodecahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle)
{
	int i,j;

    enum XM_MODEL_TYPE type = DODECAHEDRON;	/* 正十二面体 */

    XM_MODEL *model=create_model(offset);	/* 动态建立一个模型 */

    XM_POINT *ps[20];	/* 正十二面体有20个点 */
	XM_LINE *line[30];	/* 正十二面体有30条线 */
    XM_POLYGON *plgs[12];	/* 正十二面体有12个面 */
	
	/* 20个点坐标 */
	float t_coordinate[20][3];

	int line_point_mark[30][2] = {  {0,8},{8,9},{9,4},{4,16},{16,0},
									{16,17},{17,2},{2,12},{12,0},{2,10},
									{10,3},{3,13},{13,12},{9,5},{5,15},
									{15,14},{14,4},{3,19},{19,18},{18,1},
									{1,13},{7,11},{11,6},{6,14},{15,7},
									{1,8},{18,5},{19,7},{19,3},{10,11}};	/* 30条线分别由那些点构成 */

    int polygon_point_mark[12][5] = { {16,4,9,8,0},{12,2,17,16,0},{13,3,10,2,12},
									{4,14,15,5,9},{13,1,18,19,3},{15,14,6,11,7},
									{8,1,13,12,0},{9,5,18,1,8},{17,6,14,4,16},
									{17,2,10,11,6},{19,18,5,15,7},{11,10,3,19,7}    };	/* 12个面当中的点标记 */

	/*int polygon_line_mark[8][3]={{0,1,2},{2,3,4},{4,5,6},{6,7,0},{8,1,9},{10,3,8},{11,5,10},{9,7,11}};	 12个面当中的线标记 */
	
	float a = 1 / SQR3,b = sqrt((3 - SQR5) / 6),c = sqrt((3 + SQR5) / 6);	/* 中间变量参数 */
	
	/* 20个顶点的坐标 */
	t_coordinate[0][0] = a;
    t_coordinate[0][1] = a;
    t_coordinate[0][2] = a;

    t_coordinate[1][0] = a;
    t_coordinate[1][1] = a;
    t_coordinate[1][2] = -a;

    t_coordinate[2][0] = a;
    t_coordinate[2][1] = -a;
    t_coordinate[2][2] = a;


    t_coordinate[3][0] = a;
    t_coordinate[3][1] = -a;
    t_coordinate[3][2] = -a;

    t_coordinate[4][0] = -a;
    t_coordinate[4][1] = a;
    t_coordinate[4][2] = a;


    t_coordinate[5][0] = -a;
    t_coordinate[5][1] = a;
    t_coordinate[5][2] = -a;


    t_coordinate[6][0] = -a;
    t_coordinate[6][1] = -a;
    t_coordinate[6][2] = a;


    t_coordinate[7][0] = -a;
    t_coordinate[7][1] = -a;
    t_coordinate[7][2] = -a;


    t_coordinate[8][0] = b;
    t_coordinate[8][1] = c;
    t_coordinate[8][2] = 0;


    t_coordinate[9][0] = -b;
    t_coordinate[9][1] = c;
    t_coordinate[9][2] = 0;


    t_coordinate[10][0] = b;
    t_coordinate[10][1] = -c;
    t_coordinate[10][2] = 0;


    t_coordinate[11][0] = -b;
    t_coordinate[11][1] = -c;
    t_coordinate[11][2] = 0;


    t_coordinate[12][0] = c;
    t_coordinate[12][1] = 0;
    t_coordinate[12][2] = b;


    t_coordinate[13][0] = c;
    t_coordinate[13][1] = 0;
    t_coordinate[13][2] = -b;


    t_coordinate[14][0] = -c;
    t_coordinate[14][1] = 0;
    t_coordinate[14][2] = b;


    t_coordinate[15][0] = -c;
    t_coordinate[15][1] = 0;
    t_coordinate[15][2] = -b;


    t_coordinate[16][0] = 0;
    t_coordinate[16][1] = b;
    t_coordinate[16][2] = c;


    t_coordinate[17][0] = 0;
    t_coordinate[17][1] = -b;
    t_coordinate[17][2] = c;


    t_coordinate[18][0] = 0;
    t_coordinate[18][1] = b;
    t_coordinate[18][2] = -c;


    t_coordinate[19][0] = 0;
    t_coordinate[19][1] = -b;
    t_coordinate[19][2] = -c;


	model->type = type;	/* 模型类型 */
	model->radius = proportion;	/* 模型的包围球 */
	
	/* 动态建立点 */
    for (i = 0; i < 20; i++)
    {
        ps[i] = create_point(t_coordinate[i][0], t_coordinate[i][1], t_coordinate[i][2]);

    }

	/* 对点进行比例变幻，平移，旋转 */
	for (i = 0; i < 20; i++)
	{
		proportion_change(ps[i], proportion);	/* 比例变幻 */
		translation(ps[i], offset);	/* 平移 */
		point_revolve(ps[i], center, angle);

	}
	
	/* 动态建立线 */
	for (i = 0; i < 30 ; i++)
	{
		line[i] = create_line(ps[line_point_mark[i][0]], ps[line_point_mark[i][1]]);
	}

	/* 动态建立面 */
    for(j = 0; j < 12; j++)
    {
        plgs[j] = create_polygon();

    }

	/* 往面添加点 */
	for (i = 0; i < 12; i++)
	{
		for (j = 0; j < 5 ; j++)
		{
			polygon_add_point(plgs[i], ps[polygon_point_mark[i][j]]);
		}
	}

	/* 往面添加线 
	for (i = 0; i < 12; i++)
	{
		for (j = 0; j < 5 ; j++)
		{
			polygon_add_line(plgs[i], line[polygon_line_mark[i][j]]);
		}
	}*/
	
    /* 往模型中添加点 */
	for (i = 0; i < 20; i++)
	{
		model_add_point(model, ps[i]);
	}

	/* 往模型中添加线 */
	for (i = 0; i < 30; i++)
	{
		model_add_line(model, line[i]);
	}

	/* 往模型中添加面 */
	for (i = 0; i < 12; i++)
	{
		model_add_polygon(model, plgs[i]);
	}

    return model;

}


/*************************************************
  Function:        // model_octahedron
  Description:     // 建立一个正二十面体
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // proportion: 物体缩放比例
				  // angle: 旋转角度
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_icosahedron(XM_POINT *offset, XM_POINT *center, float proportion, float angle)
{
	int i,j;

    enum XM_MODEL_TYPE type = ICOSAHEDRON;	/* 正二十面体 */

    XM_MODEL *model=create_model(offset);	/* 动态建立一个模型 */

    XM_POINT *ps[12];	/* 正二十面体有12个点 */
	XM_LINE *line[30];	/* 正二十面体有30条线 */
    XM_POLYGON *plgs[20];	/* 正二十面体有20个面 */
	
	/* 12个点坐标 */

	float t_coordinate[12][3];
	float t=(1+SQR5)/2;		/* 中间参数变量 */

	int line_point_mark[30][2] = {	{0,8},{8,4},{4,0},{0,5},{5,10},
									{10,0},{2,4},{4,9},{9,2},{2,11},
									{11,5},{5,2},{1,6},{6,8},{1,8},
									{1,10},{10,7},{7,1},{3,9},{9,6},
									{3,6},{3,7},{7,11},{3,11},{10,8},
									{9,11},{2,0},{1,3},{6,4},{5,7}};	/* 12条线分别由那些点构成 */
    int polygon_point_mark[20][3]={ {4,8,0},{10,5,0},{9,4,2},{5,11,2},{8,6,1},
                            {7,10,1},{6,9,3},{11,7,3},{8,10,0},{10,8,1},
                            {11,9,2},{9,11,3},{0,2,4},{2,0,5},{3,1,6},
                            {1,3,7},{4,6,8},{6,4,9},{7,5,10},{5,7,11}};	/* 20个面当中的点标记 */

	model->type = type;	/* 模型类型 */
	model->radius = proportion;	/* 模型的包围球 */

	t_coordinate[0][0]=t*sqrt(1+t*t);
    t_coordinate[0][1]=sqrt(1+t*t);
    t_coordinate[0][2]=0;

    t_coordinate[1][0]=-t*sqrt(1+t*t);
    t_coordinate[1][1]=sqrt(1+t*t);
    t_coordinate[1][2]=0;

    t_coordinate[2][0]=t*sqrt(1+t*t);
    t_coordinate[2][1]=-sqrt(1+t*t);
    t_coordinate[2][2]=0;


    t_coordinate[3][0]=-t*sqrt(1+t*t);
    t_coordinate[3][1]=-sqrt(1+t*t);
    t_coordinate[3][2]=0;

    t_coordinate[4][0]=sqrt(1+t*t);
    t_coordinate[4][1]=0;
    t_coordinate[4][2]=t*sqrt(1+t*t);


    t_coordinate[5][0]=sqrt(1+t*t);
    t_coordinate[5][1]=0;
    t_coordinate[5][2]=-t*sqrt(1+t*t);


    t_coordinate[6][0]=-sqrt(1+t*t);
    t_coordinate[6][1]=0;
    t_coordinate[6][2]=t*sqrt(1+t*t);


    t_coordinate[7][0]=-sqrt(1+t*t);
    t_coordinate[7][1]=0;
    t_coordinate[7][2]=-t*sqrt(1+t*t);


    t_coordinate[8][0]=0;
    t_coordinate[8][1]=t*sqrt(1+t*t);
    t_coordinate[8][2]=sqrt(1+t*t);


    t_coordinate[9][0]=0;
    t_coordinate[9][1]=-t*sqrt(1+t*t);
    t_coordinate[9][2]=sqrt(1+t*t);


    t_coordinate[10][0]=0;
    t_coordinate[10][1]=t*sqrt(1+t*t);
    t_coordinate[10][2]=-sqrt(1+t*t);


    t_coordinate[11][0]=0;
    t_coordinate[11][1]=-t*sqrt(1+t*t);
    t_coordinate[11][2]=-sqrt(1+t*t);
	
	/* 动态建立点 */
    for (i = 0; i < 12; i++)
    {
        ps[i] = create_point(t_coordinate[i][0], t_coordinate[i][1], t_coordinate[i][2]);

    }

	/* 对点进行比例变幻，平移，旋转 */
	for (i = 0; i < 12; i++)
	{
		proportion_change(ps[i], proportion);	/* 比例变幻 */
		translation(ps[i], offset);	/* 平移 */
		point_revolve(ps[i], center, angle);

	}
	
	/* 动态建立线 */
	for (i = 0; i < 30 ; i++)
	{
		line[i] = create_line(ps[line_point_mark[i][0]], ps[line_point_mark[i][1]]);
	}

	/* 动态建立面 */
    for(j = 0; j < 20; j++)
    {
        plgs[j] = create_polygon();

    }

	/* 往面添加点 */
	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 3 ; j++)
		{
			polygon_add_point(plgs[i], ps[polygon_point_mark[i][j]]);
		}
	}

	/* 往面添加线 
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 3 ; j++)
		{
			polygon_add_line(plgs[i], line[polygon_line_mark[i][j]]);
		}
	}
	*/
    /* 往模型中添加点 */
	for (i = 0; i < 12; i++)
	{
		model_add_point(model, ps[i]);
	}

	/* 往模型中添加线 */
	for (i = 0; i < 30; i++)
	{
		model_add_line(model, line[i]);
	}

	/* 往模型中添加面 */
	for (i = 0; i < 20; i++)
	{
		model_add_polygon(model, plgs[i]);
	}

    return model;

}



/*************************************************
  Function:        // model_curve
  Description:     // 建立一个标准曲球体
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // latitude: 纬度
				  // Longitude: 经度
				  // a: x轴参数
				  // b: y轴参数
				  // c: z轴参数
				  // angle: 旋转角度
				  // curve_kind: 标准曲球体中的类型
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_curve(XM_POINT *offset, XM_POINT *center, float angle, float a, float b, float c, int latitude, int longitude, enum CURVE_TYPE curve_kind)
{
	int i, j;
	float  x, y, z;	/* 球体上点的坐标 */
	 
	float h;	/* 球体的高度和对应高度的小圆半经 */
	int m, n, k; /* 用来计数 */
	XM_POINT *old_point; /* 用来记录上一个点的坐标 */
	XM_POINT *point_pt; /* 点指针 */
	XM_LINE *line_pt;	/* 线指针 */
	XM_POLYGON *polygon_pt; /* 面指针 */
	XM_POINT_NODE *point_node_pt;	/* 点结点指针 */
	XM_POINT_NODE *front_point; /*前一个纬度对应的点*/
	XM_POINT *temp_point;  /* 暂时点指针 */
	
	XM_POLYGON_NODE *polygon_node_pt;	/* 面结点指针 */

	XM_MODEL *model;   /* 动态建立一个模型 */
	enum XM_MODEL_TYPE type = CURVE;	/* 球体 */ 
	
	model = create_model(offset);
	model->type = type;	/* 模型类型 */
	model->radius = a;	/* 模型的包围球 */

	for (i = 0; i <= latitude; i++)	/* 纬度 */
	{
		/* 球体的高度和对应高度的小圆半经 */
		h = c - (i * (2 * c / latitude)) ;
	
		for (j = 0; j <= longitude; j++)	/* 经度 */
		{	/* 三维坐标 */
			switch (curve_kind)
			{
				case ELLIPSE_TAPER :
				{
					x = (a * h / c) * cos(j * 2 * PI / longitude);
					y = (a * h / c) * sin(j * 2 * PI / longitude);
					z = h;
					break;
				}

				case ELLIPSOID :
				{
					if (i == 0 || i == latitude)
					{
						x = (a * sqrt(1.0001 - (h * h / (c * c)))) * cos(j * 2 * PI / longitude);
						y = (a * sqrt(1.0001 - (h * h / (c * c)))) * sin(j * 2 * PI / longitude);
						z = h;
					}
					else 
					{
						x = (a * sqrt(1 - (h * h / (c * c)))) * cos(j * 2 * PI / longitude);
						y = (a * sqrt(1 - (h * h / (c * c)))) * sin(j * 2 * PI / longitude);
						z = h;
					}
					break;
				}
				case BIFOLIATE :
				{	
					if (h != 0)
					{
						x = (a * sqrt((h * h / (c * c)) - 1)) * cos(j * 2 * PI / longitude);
						y = (a * sqrt((h * h / (c * c)) - 1)) * sin(j * 2 * PI / longitude);
						z = h;
					}
					break;
				}
				case UNIFOLIATE :
				{
					x = (a * sqrt((h * h / (c * c)) + 1)) * cos(j * 2 * PI / longitude);
					y = (a * sqrt((h * h / (c * c)) + 1)) * sin(j * 2 * PI / longitude);
					z = h;
					break;
				}
				default : 
				{
					
					break;
				}
			}

			point_pt = create_point(x, y, z);
			point_revolve(point_pt, center,angle);
			translation(point_pt, offset);
			
			
			/* 创建纬度线 */
			if (j != 0)
			{
				line_pt = create_line(point_pt, old_point);
				model_add_line(model, line_pt);
			}
			
			old_point = point_pt;

			model_add_point(model, point_pt);
		}
	}

	/* 创建面,并且把点添加到面当中去 */
	n = 0;	/* 点的个数 */
	point_node_pt = model->point_head;	/* 让指针指向点链表的头部 */
	for (i = 0; i <= latitude; i++)
	{
		for (j = 0; j <= longitude; j++)
		{
			point_node_pt = point_node_pt->next;	/* 指向下一个结点 */
			n += 1;	/* 点的个数 */

			/*经度线的建立*/
			k = 0;
			if (i != 0)
			{
				front_point = model->point_head;
				while (front_point->next != NULL)
				{
					front_point = front_point ->next;
					k += 1;
					if (k == n - longitude - 1)
					{
						break;
					}
				}
				
				line_pt = create_line(point_node_pt->point, front_point->point);
				model_add_line(model, line_pt);
			}

			/*面的建立*/
			if (i != latitude && j != longitude)	/* 当这个时候不建立面 */
			{
				polygon_pt = create_polygon();	/* 动态建立一个面 */
				model_add_polygon(model, polygon_pt);	/* 把面添加到面中去 */
				
			}
			
			/* 把此点添加到对应的面中去 */
			polygon_node_pt = model ->polygon_head ;/* 面结点指针指到面链的头部 */
			m = 0;	/* 读取面的个数 */
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;/* 指向下一个面结点的指针 */
				m += 1; /* 读取到了那个面 */
				
				/* 左上角顶点 */
				if (i == 0 && j == 0)
				{
					if (m == n) 
					{
						polygon_add_point(polygon_node_pt->polygon ,point_node_pt->point);
					}
				}
				else if (i == 0 && j == longitude)/* 右上角顶点 */
				{
					if (m == (n - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				
				}
				else if (i == latitude && j == 0)/* 左下角顶点 */
				{
					if (m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j == longitude)/* 右下角顶点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude ))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (j == 0 && i != 0 && i != latitude)/* 左边中间点 */
				{
					if (m == (n - (n / (longitude+1))) || m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == 0 && j != 0 && j != longitude)/* 上边中间的点 */
				{
					if (m == n || m == n -1)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i != 0 && i != latitude && j == longitude)/* 右边中间的点 */
				{
					if (m == (n - (n / (longitude + 1))) || m == (n - (n / (longitude + 1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j != 0 && j != longitude)/*  下边中间的点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude) || m == (n - (n / (longitude + 1)) - longitude - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else 
				{
					if (m == (n - (n / (longitude+1))) 
						|| m == (n - (n / (longitude + 1)) - 1)
						|| m == (n - (n / (longitude+1)) - longitude)
						|| m == (n - (n / (longitude + 1)) - 1) - longitude)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}

			}
		}
	}
	
	polygon_node_pt = model->polygon_head ;
	while (polygon_node_pt->next != NULL)
	{
		polygon_node_pt = polygon_node_pt ->next;
		point_node_pt = polygon_node_pt->polygon->point_head->next->next->next;
		temp_point =  point_node_pt->point;
		point_node_pt->point = point_node_pt->next->point;
		point_node_pt->next->point = temp_point;
		
	}

/*	model_revolve(model, center, angle);
	model_move(model,offset);*/

	return model;
}




/*************************************************
  Function:        // model_threecubedbezier
  Description:     // 建立三次bezier曲面
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // latitude: 纬度
				  // Longitude: 经度
				  // reference_point 控制的坐标
				  
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_threecubedbezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float reference_point[16][3])
{
	int i, j;
	float  x, y, z;	/* 三次bezier体上点的坐标 */

	/*三次bezier曲面矩阵中间参数*/
    float a, b, c, d;
    float a1[3], b1[3], c1[3], d1[3];
    float a2[3], b2[3], c2[3], d2[3];
	 
	int m, n, k; /* 用来计数 */
	XM_POINT *old_point; /* 用来记录上一个点的坐标 */
	XM_POINT *point_pt; /* 点指针 */
	XM_LINE *line_pt;	/* 线指针 */
	XM_POLYGON *polygon_pt; /* 面指针 */
	XM_POINT_NODE *point_node_pt;	/* 点结点指针 */
	XM_POINT_NODE *front_point; /*前一个纬度对应的点*/
	XM_POINT *temp_point;  /* 暂时点指针 */
	
	XM_POLYGON_NODE *polygon_node_pt;	/* 面结点指针 */

	XM_MODEL *model;   /* 动态建立一个模型 */
	enum XM_MODEL_TYPE type = THREECUBEDBEZIER;	/* 三次bezier曲面体 */ 
	
	model = create_model(offset);
	model->type = type;	/* 模型类型 */
	model->radius = 0;	/* 模型的包围球 */

	for (i = 0; i <= latitude; i++)	/* 纬度 */
	{
		for (j = 0; j <= longitude; j++)	/* 经度 */
		{	
			/* 三维坐标 */
			a = -pow(i * 1.0 / (latitude),3) + 3 * pow(i * 1.0 / (latitude),2)-3 * i * 1.0 / (latitude) + 1;
            b = 3 * pow(i * 1.0 / (latitude),3)-6 * pow(i * 1.0 / (latitude),2) + 3 * pow(i * 1.0 / (latitude),1) ;
            c = -3 * pow(i * 1.0 / (latitude),3)  + 3 * pow(i * 1.0 / (latitude),2);
            d = pow(i * 1.0 / (latitude),3) ;

            a1[0] = a * reference_point[0][0] + b * reference_point[4][0] + c * reference_point[8][0] + d * reference_point[12][0] ;
            b1[0] = a * reference_point[1][0] + b * reference_point[5][0] + c * reference_point[9][0] + d * reference_point[13][0] ;
            c1[0] = a * reference_point[2][0] + b * reference_point[6][0] + c * reference_point[10][0] + d * reference_point[14][0];
            d1[0] = a * reference_point[3][0] + b * reference_point[7][0] + c * reference_point[11][0] + d * reference_point[15][0] ;

            a1[1] = a * reference_point[0][1] + b * reference_point[4][1] + c * reference_point[8][1] + d * reference_point[12][1] ;
            b1[1] = a * reference_point[1][1] + b * reference_point[5][1] + c * reference_point[9][1] + d * reference_point[13][1] ;
            c1[1] = a * reference_point[2][1] + b * reference_point[6][1] + c * reference_point[10][1] + d * reference_point[14][1];
            d1[1] = a * reference_point[3][1] + b * reference_point[7][1] + c * reference_point[11][1] + d * reference_point[15][1] ;


            a1[2] = a * reference_point[0][2] + b * reference_point[4][2] + c * reference_point[8][2] + d * reference_point[12][2] ;
            b1[2] = a * reference_point[1][2] + b * reference_point[5][2] + c * reference_point[9][2] + d * reference_point[13][2] ;
            c1[2] = a * reference_point[2][2] + b * reference_point[6][2] + c * reference_point[10][2] + d * reference_point[14][2];
            d1[2] = a * reference_point[3][2] + b * reference_point[7][2] + c * reference_point[11][2] + d * reference_point[15][2] ;

            a2[0] = -a1[0] + 3 * b1[0]-3 * c1[0] + d1[0];
            b2[0] = 3 * a1[0]-6 * b1[0] + 3 * c1[0];
            c2[0] = -3 * a1[0] + 3 * b1[0];
            d2[0] = a1[0];

            a2[1] = -a1[1] + 3 * b1[1]-3 * c1[1] + d1[1];
            b2[1] = 3 * a1[1]-6 * b1[1] + 3 * c1[1];
            c2[1] = -3 * a1[1] + 3 * b1[1];
            d2[1] = a1[1];

            a2[2] = -a1[2] + 3 * b1[2]-3 * c1[2] + d1[2];
            b2[2] = 3 * a1[2]-6 * b1[2] + 3 * c1[2];
            c2[2] = -3 * a1[2] + 3 * b1[2];
            d2[2] = a1[2];

            x = a2[0] *  pow(j * 1.0 / longitude,3) + b2[0] * pow(j * 1.0 / longitude,2) + c2[0] * pow(j * 1.0 / longitude,1) + d2[0];
            y = a2[1] *  pow(j * 1.0 / longitude,3) + b2[1] * pow(j * 1.0 / longitude,2) + c2[1] * pow(j * 1.0 / longitude,1) + d2[1];
            z = a2[2] *  pow(j * 1.0 / longitude,3) + b2[2] * pow(j * 1.0 / longitude,2) + c2[2] * pow(j * 1.0 / longitude,1) + d2[2];

			point_pt = create_point(x, y, z);
			translation(point_pt, offset);
			point_revolve(point_pt, center, angle);
			
			 /* 创建纬度线 */
			if (j != 0)
			{
				line_pt = create_line(point_pt, old_point);
				model_add_line(model, line_pt);
			}
			
			old_point = point_pt;

			model_add_point(model, point_pt);
		}
	}

	/* 创建面,并且把点添加到面当中去 */
	n = 0;	/* 点的个数 */
	point_node_pt = model->point_head;	/* 让指针指向点链表的头部 */
	for (i = 0; i <= latitude; i++)
	{
		for (j = 0; j <= longitude; j++)
		{
			point_node_pt = point_node_pt->next;	/* 指向下一个结点 */
			n += 1;	/* 点的个数 */

			/*经度线的建立*/
			k = 0;
			if (i != 0)
			{
				front_point = model->point_head;
				while (front_point->next != NULL)
				{
					front_point = front_point ->next;
					k += 1;
					if (k == n - longitude - 1)
					{
						break;
					}
				}
				line_pt = create_line(point_node_pt->point, front_point->point);
				model_add_line(model, line_pt);
			}

			/*面的建立*/
			if (i != latitude && j != longitude)	/* 当这个时候不建立面 */
			{
				polygon_pt = create_polygon();	/* 动态建立一个面 */
				model_add_polygon(model, polygon_pt);	/* 把面添加到面中去 */
				
			}
			
			/* 把此点添加到对应的面中去 */
			polygon_node_pt = model ->polygon_head ;/* 面结点指针指到面链的头部 */
			m = 0;	/* 读取面的个数 */
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;/* 指向下一个面结点的指针 */
				m += 1; /* 读取到了那个面 */
				
				/* 左上角顶点 */
				if (i == 0 && j == 0)
				{
					if (m == n) 
					{
						polygon_add_point(polygon_node_pt->polygon ,point_node_pt->point);
					}
				}
				else if (i == 0 && j == longitude)/* 右上角顶点 */
				{
					if (m == (n - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				
				}
				else if (i == latitude && j == 0)/* 左下角顶点 */
				{
					if (m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j == longitude)/* 右下角顶点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (j == 0 && i != 0 && i != latitude)/* 左边中间点 */
				{
					if (m == (n - (n / (longitude+1))) || m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == 0 && j != 0 && j != longitude)/* 上边中间的点 */
				{
					if (m == n || m == n -1)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i != 0 && i != latitude && j == longitude)/* 右边中间的点 */
				{
					if (m == (n - (n / (longitude + 1))) || m == (n - (n / (longitude + 1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j != 0 && j != longitude)/*  下边中间的点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude) || m == (n - (n / (longitude + 1)) - longitude - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else 
				{
					if (m == (n - (n / (longitude+1))) 
						|| m == (n - (n / (longitude + 1)) - 1)
						|| m == (n - (n / (longitude+1)) - longitude)
						|| m == (n - (n / (longitude + 1)) - 1) - longitude)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}

			}
		}
	}

	polygon_node_pt = model->polygon_head ;
	while (polygon_node_pt->next != NULL)
	{
		polygon_node_pt = polygon_node_pt ->next;
		point_node_pt = polygon_node_pt->polygon->point_head->next->next->next;
		temp_point =  point_node_pt->point;
		point_node_pt->point = point_node_pt->next->point;
		point_node_pt->next->point = temp_point;
		
	}

	return model;
}



/*************************************************
  Function:        // model_twocubedbezier
  Description:     // 建立二次bezier曲面
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // angle: 旋转的角度
				  // latitude: 纬度
				  // Longitude: 经度
				  // reference_point 控制的坐标
				  
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_twocubedbezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float reference_point[9][3])
{
	int i, j;
	float  x, y, z;	/* 二次bezier体上点的坐标 */

	/*二次bezier曲面矩阵中间参数*/
    float A,B,C;
    float A1[3],B1[3],C1[3];
    float A2[3],B2[3],C2[3];
	 
	int m, n, k; /* 用来计数 */
	XM_POINT *old_point; /* 用来记录上一个点的坐标 */
	XM_POINT *point_pt; /* 点指针 */
	XM_LINE *line_pt;	/* 线指针 */
	XM_POLYGON *polygon_pt; /* 面指针 */
	XM_POINT_NODE *point_node_pt;	/* 点结点指针 */
	XM_POINT_NODE *front_point; /*前一个纬度对应的点*/
	XM_POINT *temp_point;  /* 暂时点指针 */

	
	XM_POLYGON_NODE *polygon_node_pt;	/* 面结点指针 */

	XM_MODEL *model;   /* 动态建立一个模型 */
	enum XM_MODEL_TYPE type = TWOCUBEDBEZIER;	/* 二次bezier曲面体 */ 
	
	model = create_model(offset);
	model->type = type;	/* 模型类型 */
	model->radius = 0;	/* 模型的包围球 */

	for (i = 0;i<= latitude;i++)/* 纬度 */
    {
        for(j = 0;j<= longitude;j++)/* 经度 */
        {
			/* 二次bezier曲面的运算 */
			A = pow(i * 1.0 / (latitude),2) - 2 * (i * 1.0 / (latitude)) + 1;
            B = -2 * pow(i * 1.0 / (latitude),2) + 2 * (i * 1.0 / (latitude));
            C = pow(i * 1.0 / (latitude),2);

            A1[0] = A * reference_point[0][0] + B * reference_point[3][0] + C * reference_point[6][0];
            B1[0] = A * reference_point[1][0] + B * reference_point[4][0] + C * reference_point[7][0];
            C1[0] = A * reference_point[2][0] + B * reference_point[5][0] + C * reference_point[8][0];

            A1[1] = A * reference_point[0][1] + B * reference_point[3][1] + C * reference_point[6][1];
            B1[1] = A * reference_point[1][1] + B * reference_point[4][1] + C * reference_point[7][1];
            C1[1] = A * reference_point[2][1] + B * reference_point[5][1] + C * reference_point[8][1];

            A1[2] = A * reference_point[0][2] + B * reference_point[3][2] + C * reference_point[6][2];
            B1[2] = A * reference_point[1][2] + B * reference_point[4][2] + C * reference_point[7][2];
            C1[2] = A * reference_point[2][2] + B * reference_point[5][2] + C * reference_point[8][2];

            A2[0] = A1[0]-2 * B1[0] + C1[0];
            B2[0] = -2 * A1[0] + 2 * B1[0];
            C2[0] = A1[0];

            A2[1] = A1[1]-2 * B1[1] + C1[1];
            B2[1] = -2 * A1[1] + 2 * B1[1];
            C2[1] = A1[1];

            A2[2] = A1[2]-2 * B1[2] + C1[2];
            B2[2] = -2 * A1[2] + 2 * B1[2];
            C2[2] = A1[2];

           
            x = A2[0] * pow(j * 1.0 / longitude,2) + B2[0]  *  (j * 1.0 / longitude) + C2[0];
            y = A2[1] * pow(j * 1.0 / longitude,2) + B2[1]  *  (j * 1.0 / longitude) + C2[1];
            z = A2[2] * pow(j * 1.0 / longitude,2) + B2[2]  *  (j * 1.0 / longitude) + C2[2];

			point_pt = create_point(x, y, z);
			translation(point_pt, offset);
			point_revolve(point_pt, center, angle);
			
			/* 创建纬度线 */
			if (j != 0)
			{
				line_pt = create_line(point_pt, old_point);
				model_add_line(model, line_pt);
			}
			
			old_point = point_pt;

			model_add_point(model, point_pt);
		}
	}

	/* 创建面,并且把点添加到面当中去 */
	n = 0;	/* 点的个数 */
	point_node_pt = model->point_head;	/* 让指针指向点链表的头部 */
	for (i = 0; i <= latitude; i++)
	{
		for (j = 0; j <= longitude; j++)
		{
			point_node_pt = point_node_pt->next;	/* 指向下一个结点 */
			n += 1;	/* 点的个数 */

			/*经度线的建立*/
			k = 0;
			if (i != 0)
			{
				front_point = model->point_head;
				while (front_point->next != NULL)
				{
					front_point = front_point ->next;
					k += 1;
					if (k == n - longitude - 1)
					{
						break;
					}
				}
				line_pt = create_line(point_node_pt->point, front_point->point);
				model_add_line(model, line_pt);
			}

			/*面的建立*/
			if (i != latitude && j != longitude)	/* 当这个时候不建立面 */
			{
				polygon_pt = create_polygon();	/* 动态建立一个面 */
				model_add_polygon(model, polygon_pt);	/* 把面添加到面中去 */
				
			}
			
			/* 把此点添加到对应的面中去 */
			polygon_node_pt = model ->polygon_head ;/* 面结点指针指到面链的头部 */
			m = 0;	/* 读取面的个数 */
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;/* 指向下一个面结点的指针 */
				m += 1; /* 读取到了那个面 */
				
				/* 左上角顶点 */
				if (i == 0 && j == 0)
				{
					if (m == n) 
					{
						polygon_add_point(polygon_node_pt->polygon ,point_node_pt->point);
					}
				}
				else if (i == 0 && j == longitude)/* 右上角顶点 */
				{
					if (m == (n - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				
				}
				else if (i == latitude && j == 0)/* 左下角顶点 */
				{
					if (m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j == longitude)/* 右下角顶点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude ))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (j == 0 && i != 0 && i != latitude)/* 左边中间点 */
				{
					if (m == (n - (n / (longitude+1))) || m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == 0 && j != 0 && j != longitude)/* 上边中间的点 */
				{
					if (m == n || m == n -1)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i != 0 && i != latitude && j == longitude)/* 右边中间的点 */
				{
					if (m == (n - (n / (longitude + 1))) || m == (n - (n / (longitude + 1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j != 0 && j != longitude)/*  下边中间的点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude) || m == (n - (n / (longitude + 1)) - longitude - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else 
				{
					if (m == (n - (n / (longitude+1))) 
						|| m == (n - (n / (longitude + 1)) - 1)
						|| m == (n - (n / (longitude+1)) - longitude)
						|| m == (n - (n / (longitude + 1)) - 1) - longitude)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}

			}
		}
	}

	polygon_node_pt = model->polygon_head ;
	while (polygon_node_pt->next != NULL)
	{
		polygon_node_pt = polygon_node_pt ->next;
		point_node_pt = polygon_node_pt->polygon->point_head->next->next->next;
		temp_point =  point_node_pt->point;
		point_node_pt->point = point_node_pt->next->point;
		point_node_pt->next->point = temp_point;
		
	}

	return model;
}




/*************************************************
  Function:        // model_twobezier
  Description:     // 建立二次bezier曲线
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // angle: 旋转的角度
				  // latitude: 纬度
				  // reference_point: 控制点坐标				  
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_twobezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, float reference_point[3][3])
{
	float  x, y, z;	/* 二次bezier体上点的坐标 */
	int i;
	int t; /* bezier曲线的中间细分变量 */
	XM_POINT *point_pt; /* 当前点指针 */
	XM_POINT *old_point; /* 用来记录前一个点的坐标 */
	XM_LINE *line_pt; /* 线指针 */

	XM_MODEL *model;   /* 动态建立一个模型 */
	enum XM_MODEL_TYPE type = TWOBEZIER;	/* 二次bezier曲线*/ 
	
	model = create_model(offset);
	model->type = type;	/* 模型类型 */
	model->radius = 0;	/* 模型的包围球 */

	for (i = 0; i <= latitude; i++)
	{
		t = i / latitude;	/* 细分变量 */

		x = (t * t - 2 * t + 1) * reference_point[0][0] + (-2 * t * t + 2 * t) * reference_point[1][0] + t * t * reference_point[2][0];
		y = (t * t - 2 * t + 1) * reference_point[0][1] + (-2 * t * t + 2 * t) * reference_point[1][1] + t * t * reference_point[2][1];
		z = (t * t - 2 * t + 1) * reference_point[0][2] + (-2 * t * t + 2 * t) * reference_point[1][2] + t * t * reference_point[2][2];
		
		point_pt = create_point(x, y, z);
		translation(point_pt, offset);
		point_revolve(point_pt, center, angle);
			
		/* 创建纬度线 */
		if (i != 0)
		{
			line_pt = create_line(point_pt, old_point);
			model_add_line(model, line_pt);
		}
			
		old_point = point_pt;

		model_add_point(model, point_pt);
		
	}

	return model;

}



/*************************************************
  Function:        // model_threebezier
  Description:     // 建立三次bezier曲线
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // angle: 旋转的角度
				  // latitude: 纬度
				  // reference_point: 控制点坐标				  
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_threebezier(XM_POINT *offset, XM_POINT *center, float angle, int latitude, float reference_point[4][3])
{
	float  x, y, z;	/* 三次bezier体上点的坐标 */
	int i;
	int t; /* bezier曲线的中间细分变量 */
	XM_POINT *point_pt; /* 当前点指针 */
	XM_POINT *old_point; /* 用来记录前一个点的坐标 */
	XM_LINE *line_pt; /* 线指针 */

	XM_MODEL *model;   /* 动态建立一个模型 */
	enum XM_MODEL_TYPE type = THREEBEZIER;	/* 三次bezier曲线*/ 
	
	model = create_model(offset);
	model->type = type;	/* 模型类型 */
	model->radius = 0;	/* 模型的包围球 */

	for (i = 0; i <= latitude; i++)
	{
		t = i / latitude;	/* 细分变量 */

		x = (-t * t * t + 3 * t * t - 3 * t + 1) * reference_point[0][0] 
			+ (3 * t * t * t - 6 * t * t + 3 * t) * reference_point[1][0]
			+ (-3 * t * t * t + 3 * t * t) * reference_point[2][0]
			+ (t * t * t) * reference_point[3][0];
		y = (-t * t * t + 3 * t * t - 3 * t + 1) * reference_point[0][1] 
			+ (3 * t * t * t - 6 * t * t + 3 * t) * reference_point[1][1]
			+ (-3 * t * t * t + 3 * t * t) * reference_point[2][1]
			+ (t * t * t) * reference_point[3][1];
		z = (-t * t * t + 3 * t * t - 3 * t + 1) * reference_point[0][2] 
			+ (3 * t * t * t - 6 * t * t + 3 * t) * reference_point[1][2]
			+ (-3 * t * t * t + 3 * t * t) * reference_point[2][2]
			+ (t * t * t) * reference_point[3][2];
		
		point_pt = create_point(x, y, z);
		translation(point_pt, offset);
		point_revolve(point_pt, center, angle);
			
		/* 创建纬度线 */
		if (i != 0)
		{
			line_pt = create_line(point_pt, old_point);
			model_add_line(model, line_pt);
		}
			
		old_point = point_pt;

		model_add_point(model, point_pt);
		
	}

	return model;

}




/*************************************************
  Function:        // model_leptospira
  Description:     // 建立螺旋体
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // angle: 旋转的角度
				  // height:螺旋体的高度
				  // w:螺旋体转过的角度
				  // latitude: 纬度
				  // longitude: 经度
				  // R1: 内半经
				  // R2: 外半经
				  				  
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_leptospira(XM_POINT *offset, XM_POINT *center, float angle, float height, float w, int latitude, int longitude,float R1, float R2)
{
	int i, j;
    float  x, y, z;	/* 螺旋体上点的坐标 */
	float h; /* 螺旋体上升的高度 */

	int m, n, k; /* 用来计数 */
	XM_POINT *old_point; /* 用来记录上一个点的坐标 */
	XM_POINT *point_pt; /* 点指针 */
	XM_LINE *line_pt;	/* 线指针 */
	XM_POLYGON *polygon_pt; /* 面指针 */
	XM_POINT_NODE *point_node_pt;	/* 点结点指针 */
	XM_POINT_NODE *front_point; /*前一个纬度对应的点*/
	XM_POINT *temp_point;  /* 暂时点指针 */
	
	XM_POLYGON_NODE *polygon_node_pt;	/* 面结点指针 */

	XM_MODEL *model;   /* 动态建立一个模型 */
	enum XM_MODEL_TYPE type = LEPTOSPIRA;	/* 螺旋体 */ 
	
	model = create_model(offset);
	model->type = type;	/* 模型类型 */
	model->radius = 0;	/* 模型的包围球 */

	for (i = 0; i <= latitude; i++)
	{
		h = i * height / latitude;

		for (j = 0; j <= longitude; j++)
		{
			x = (R1 + (j * (R2 - R1) / longitude)) * cos(i * w / latitude);
			y = (R1 + (j * (R2 - R1) / longitude)) * sin(i * w / latitude);
			z = h;

			point_pt = create_point(x, y, z);
			translation(point_pt, offset);
			point_revolve(point_pt, center, angle);
			
			/* 创建纬度线 */
			if (j != 0)
			{
				line_pt = create_line(point_pt, old_point);
				model_add_line(model, line_pt);
			}
			
			old_point = point_pt; 

			model_add_point(model, point_pt);

			
		}
	}


	/* 创建面,并且把点添加到面当中去 */
	n = 0;	/* 点的个数 */
	point_node_pt = model->point_head;	/* 让指针指向点链表的头部 */
	for (i = 0; i <= latitude; i++)
	{
		for (j = 0; j <= longitude; j++)
		{
			point_node_pt = point_node_pt->next;	/* 指向下一个结点 */
			n += 1;	/* 点的个数 */

			/*经度线的建立*/
			k = 0;
			if (i != 0)
			{
				front_point = model->point_head;
				while (front_point->next != NULL)
				{
					front_point = front_point ->next;
					k += 1;
					if (k == n - longitude - 1)
					{
						break;
					}
				}
				line_pt = create_line(point_node_pt->point, front_point->point);
				model_add_line(model, line_pt);
			}

			/*面的建立*/
			if (i != latitude && j != longitude)	/* 当这个时候不建立面 */
			{
				polygon_pt = create_polygon();	/* 动态建立一个面 */
				model_add_polygon(model, polygon_pt);	/* 把面添加到面中去 */
				
			}
			
			/* 把此点添加到对应的面中去 */
			polygon_node_pt = model ->polygon_head ;/* 面结点指针指到面链的头部 */
			m = 0;	/* 读取面的个数 */
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;/* 指向下一个面结点的指针 */
				m += 1; /* 读取到了那个面 */
				
				/* 左上角顶点 */
				if (i == 0 && j == 0)
				{
					if (m == n) 
					{
						polygon_add_point(polygon_node_pt->polygon ,point_node_pt->point);
					}
				}
				else if (i == 0 && j == longitude)/* 右上角顶点 */
				{
					if (m == (n - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				
				}
				else if (i == latitude && j == 0)/* 左下角顶点 */
				{
					if (m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j == longitude)/* 右下角顶点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude ))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (j == 0 && i != 0 && i != latitude)/* 左边中间点 */
				{
					if (m == (n - (n / (longitude+1))) || m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == 0 && j != 0 && j != longitude)/* 上边中间的点 */
				{
					if (m == n || m == n -1)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i != 0 && i != latitude && j == longitude)/* 右边中间的点 */
				{
					if (m == (n - (n / (longitude + 1))) || m == (n - (n / (longitude + 1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j != 0 && j != longitude)/*  下边中间的点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude) || m == (n - (n / (longitude + 1)) - longitude - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else 
				{
					if (m == (n - (n / (longitude+1))) 
						|| m == (n - (n / (longitude + 1)) - 1)
						|| m == (n - (n / (longitude+1)) - longitude)
						|| m == (n - (n / (longitude + 1)) - 1) - longitude)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}

			}
		}
	}

	polygon_node_pt = model->polygon_head ;
	while (polygon_node_pt->next != NULL)
	{
		polygon_node_pt = polygon_node_pt ->next;
		point_node_pt = polygon_node_pt->polygon->point_head->next->next->next;
		temp_point =  point_node_pt->point;
		point_node_pt->point = point_node_pt->next->point;
		point_node_pt->next->point = temp_point;
		
	}

	return model;
}



/*************************************************
  Function:        // model_annular
  Description:     // 环面
  Calls:           
  Called By:          
  Table Accessed:   
  Table Updated:   
  Input:          // offset: 物体的偏移量
                  // center: 物体初始化的旋转轴
				  // angle: 旋转的角度
				  // latitude: 纬度
				  // longitude: 经度
				  // r0 : 主半径
				  // r1 : 次半径
				 				  
  Output:         
  Return:         // XM_MODEL类型指针  
  Others:         
*************************************************/
XM_MODEL *model_annular(XM_POINT *offset, XM_POINT *center, float angle, int latitude, int longitude, float r0, float r1)
{
	int i,j;
	float x, y, z; /* 点坐标 */

	int m, n, k; /* 用来计数 */
	XM_POINT *old_point; /* 用来记录上一个点的坐标 */
	XM_POINT *point_pt; /* 点指针 */
	XM_LINE *line_pt;	/* 线指针 */
	XM_POLYGON *polygon_pt; /* 面指针 */
	XM_POINT_NODE *point_node_pt;	/* 点结点指针 */
	XM_POINT_NODE *front_point; /*前一个纬度对应的点*/
	XM_POINT *temp_point;  /* 暂时点指针 */
	
	XM_POLYGON_NODE *polygon_node_pt;	/* 面结点指针 */

	XM_MODEL *model;   /* 动态建立一个模型 */
	enum XM_MODEL_TYPE type = ANNULAR;	/* 螺旋体 */ 
	
	model = create_model(offset);
	model->type = type;	/* 模型类型 */
	model->radius = 0;	/* 模型的包围球 */

	for (i = 0; i <= latitude; i++)
	{
		for (j = 0; j <= longitude; j++)
		{
			x = (r0 + r1 * cos(i * 2 * PI / latitude)) * cos(j * 2 * PI / longitude);
			y = (r0 + r1 * cos(i * 2 * PI / latitude)) * sin(j * 2 * PI / longitude);
			z = r1 * sin(i * 2 * PI / latitude);
			
			point_pt = create_point(x, y, z);
			translation(point_pt, offset);
			point_revolve(point_pt, center, angle);
			
			/* 创建纬度线 */
			if (j != 0)
			{
				line_pt = create_line(point_pt, old_point);
				model_add_line(model, line_pt);
			}
			
			old_point = point_pt;

			model_add_point(model, point_pt);
		}
	}

	/* 创建面,并且把点添加到面当中去 */
	n = 0;	/* 点的个数 */
	point_node_pt = model->point_head;	/* 让指针指向点链表的头部 */
	for (i = 0; i <= latitude; i++)
	{
		for (j = 0; j <= longitude; j++)
		{
			point_node_pt = point_node_pt->next;	/* 指向下一个结点 */
			n += 1;	/* 点的个数 */

			/*经度线的建立*/
			k = 0;
			if (i != 0)
			{
				front_point = model->point_head;
				while (front_point->next != NULL)
				{
					front_point = front_point ->next;
					k += 1;
					if (k == n - longitude - 1)
					{
						break;
					}
				}
				line_pt = create_line(point_node_pt->point, front_point->point);
				model_add_line(model, line_pt);
			}

			/*面的建立*/
			if (i != latitude && j != longitude)	/* 当这个时候不建立面 */
			{
				polygon_pt = create_polygon();	/* 动态建立一个面 */
				model_add_polygon(model, polygon_pt);	/* 把面添加到面中去 */
				
			}
			
			/* 把此点添加到对应的面中去 */
			polygon_node_pt = model ->polygon_head ;/* 面结点指针指到面链的头部 */
			m = 0;	/* 读取面的个数 */
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;/* 指向下一个面结点的指针 */
				m += 1; /* 读取到了那个面 */
				
				/* 左上角顶点 */
				if (i == 0 && j == 0)
				{
					if (m == n) 
					{
						polygon_add_point(polygon_node_pt->polygon ,point_node_pt->point);
					}
				}
				else if (i == 0 && j == longitude)/* 右上角顶点 */
				{
					if (m == (n - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				
				}
				else if (i == latitude && j == 0)/* 左下角顶点 */
				{
					if (m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j == longitude)/* 右下角顶点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude ))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (j == 0 && i != 0 && i != latitude)/* 左边中间点 */
				{
					if (m == (n - (n / (longitude+1))) || m == (n - (n / (longitude+1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == 0 && j != 0 && j != longitude)/* 上边中间的点 */
				{
					if (m == n || m == n -1)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i != 0 && i != latitude && j == longitude)/* 右边中间的点 */
				{
					if (m == (n - (n / (longitude + 1))) || m == (n - (n / (longitude + 1)) - longitude))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else if (i == latitude && j != 0 && j != longitude)/*  下边中间的点 */
				{
					if (m == (n - (n / (longitude + 1)) - longitude) || m == (n - (n / (longitude + 1)) - longitude - 1))
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}
				else 
				{
					if (m == (n - (n / (longitude+1))) 
						|| m == (n - (n / (longitude + 1)) - 1)
						|| m == (n - (n / (longitude+1)) - longitude)
						|| m == (n - (n / (longitude + 1)) - 1) - longitude)
					{
						polygon_add_point(polygon_node_pt->polygon, point_node_pt->point);
					}
				}

			}
		}
	}

	polygon_node_pt = model->polygon_head ;
	while (polygon_node_pt->next != NULL)
	{
		polygon_node_pt = polygon_node_pt ->next;
		point_node_pt = polygon_node_pt->polygon->point_head->next->next->next;
		temp_point =  point_node_pt->point;
		point_node_pt->point = point_node_pt->next->point;
		point_node_pt->next->point = temp_point;
		
	}

	return model;

	
}












