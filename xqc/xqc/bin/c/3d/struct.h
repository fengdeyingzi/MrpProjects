/************************************************************
  FileName: struct.h
  Description:           基本模型结构
*******************************************************************/






/* 布尔类型 */
enum XM_BOOL
{
	XM_FALSE,
	XM_TRUE

};

/* 颜色 */
typedef long XM_COLOR;

/* 标准模型 */
enum XM_MODEL_TYPE 
{
	TETRAHEDRON,         /* 四面体 */
	HEXAHEDRON,        /* 六面体 */
	OCTAHEDRON,         /* 八面体 */
	DODECAHEDRON,        /* 十二面体 */
	ICOSAHEDRON,         /* 二十面体 */
	CURVE ,             /* 标准曲面球 */
	THREECUBEDBEZIER, /* 三次bezier曲面 */
	TWOCUBEDBEZIER ,/* 二次bezier曲面 */
	THREEBEZIER,	/* 三次bezier曲线 */
	TWOBEZIER,	/* 二次bezier曲线 */
	LEPTOSPIRA ,/* 螺旋体 */
	ANNULAR   /* 环面 */

};

/* 标准曲面球中的四种类型*/
enum CURVE_TYPE
{
	ELLIPSE_TAPER, /* 椭圆锥面 */
	ELLIPSOID,	/* 椭圆体 */
	BIFOLIATE, /* 双叶双曲面 */
	UNIFOLIATE,	/* 单叶双曲面 */

};

/*****************************************************/

/* 二维点 */
typedef struct xm_point2d
{
	int x, y;      /* 点的二维坐标 */
}XM_POINT2D;


/* 点 */
typedef struct xm_point
{
	float x, y, z;      /* 点的三维坐标 */
	float prj_x, prj_y;   /* 点的投影坐标 */
	long visible;    /*  点的可见性 */
}XM_POINT;


/* 点链表结点 */
typedef struct xm_point_node
{
	XM_POINT *point;      /* 对应的点指针 */

	struct xm_point_node *next;   /* 下一个结点指针 */
}XM_POINT_NODE;


/*  线 */
typedef struct xm_line
{
	XM_POINT *p1;      /* 第一个点 */
	XM_POINT *p2;      /* 第二个点 */

	long visible;    /* 线的可见性 */
}XM_LINE;


/* 线链表结点 */
typedef struct xm_line_node
{
	XM_LINE *line;    /* 对应的线 */

	struct xm_line_node *next;   /* 下一个结点指针 */
}XM_LINE_NODE;


/* 多边形 */
typedef struct xm_polygon
{
	XM_POINT_NODE *point_head;     /* 点链表头结点 */
	 /*XM_LINE_NODE *line_head;     线链表头结点 */
	
	XM_COLOR color;             /* 颜色 */
	long visible;            /* 面的可见性 */
}XM_POLYGON;


/* 多边形链表结点 */
typedef struct xm_polygon_node
{
	XM_POLYGON *polygon;           /* 对应的多边形 */

	struct xm_polygon_node *next;       /* 下一个结点指针 */
}XM_POLYGON_NODE;


/* 模型 */
typedef struct xm_model
{
	XM_POINT_NODE *point_head;        /* 点链表头结点 */
	XM_LINE_NODE *line_head;         /* 线链表头结点 */
	XM_POLYGON_NODE *polygon_head;    /* 多边形链表头结点 */
	XM_COLOR bordercolor;	/* 边框颜色 */

	XM_POINT *offset;         /* 模型的偏移量 */
	enum XM_MODEL_TYPE type;          /* 类型 */

	float radius;            /* 包围球半径 */
	float depth;             /* 模型的深度 */
	int style;              /* 绘制样式 */
	long visible;       /* 模型的可见性 */

}XM_MODEL;


/* 模型链表结点 */
typedef struct xm_model_node
{
	XM_MODEL *model;                  /* 对应的模型 */

	struct xm_model_node *next;            /* 下一个结点 */

}XM_MODEL_NODE;



/* 场景 */
typedef struct xm_field
{
	XM_MODEL_NODE *model_head;           /* 模型链表指针 */
}XM_FIELD;


/****************************************************************/

/* 视点 */
typedef struct xm_view
{
	XM_POINT *pst;               /* 视点位置向量 */
	XM_POINT *drt;               /* 视点方向向量 */
	XM_POINT *Xe;                /* 视面水平坐标单位向量 */
	XM_POINT *Ye;                /* 视面竖直坐标单位向量 */

	float A, B, C, D;            /* 视面方程参数 */

	float width;                /* 视区宽度 */
	float height;               /* 视区高度 */
}XM_VIEW;



/* 窗口 */
typedef struct xm_window
{
	int left;               /* 窗口左边界 */
	int top;                /* 窗口上边界 */ 
	int width;              /* 窗口宽度 */
	int height;             /* 窗口高度 */
}XM_WINDOW;