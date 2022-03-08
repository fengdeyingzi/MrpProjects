/************************************************************
  FileName: struct.h
  Description:           ����ģ�ͽṹ
*******************************************************************/






/* �������� */
enum XM_BOOL
{
	XM_FALSE,
	XM_TRUE

};

/* ��ɫ */
typedef long XM_COLOR;

/* ��׼ģ�� */
enum XM_MODEL_TYPE 
{
	TETRAHEDRON,         /* ������ */
	HEXAHEDRON,        /* ������ */
	OCTAHEDRON,         /* ������ */
	DODECAHEDRON,        /* ʮ������ */
	ICOSAHEDRON,         /* ��ʮ���� */
	CURVE ,             /* ��׼������ */
	THREECUBEDBEZIER, /* ����bezier���� */
	TWOCUBEDBEZIER ,/* ����bezier���� */
	THREEBEZIER,	/* ����bezier���� */
	TWOBEZIER,	/* ����bezier���� */
	LEPTOSPIRA ,/* ������ */
	ANNULAR   /* ���� */

};

/* ��׼�������е���������*/
enum CURVE_TYPE
{
	ELLIPSE_TAPER, /* ��Բ׶�� */
	ELLIPSOID,	/* ��Բ�� */
	BIFOLIATE, /* ˫Ҷ˫���� */
	UNIFOLIATE,	/* ��Ҷ˫���� */

};

/*****************************************************/

/* ��ά�� */
typedef struct xm_point2d
{
	int x, y;      /* ��Ķ�ά���� */
}XM_POINT2D;


/* �� */
typedef struct xm_point
{
	float x, y, z;      /* �����ά���� */
	float prj_x, prj_y;   /* ���ͶӰ���� */
	long visible;    /*  ��Ŀɼ��� */
}XM_POINT;


/* �������� */
typedef struct xm_point_node
{
	XM_POINT *point;      /* ��Ӧ�ĵ�ָ�� */

	struct xm_point_node *next;   /* ��һ�����ָ�� */
}XM_POINT_NODE;


/*  �� */
typedef struct xm_line
{
	XM_POINT *p1;      /* ��һ���� */
	XM_POINT *p2;      /* �ڶ����� */

	long visible;    /* �ߵĿɼ��� */
}XM_LINE;


/* �������� */
typedef struct xm_line_node
{
	XM_LINE *line;    /* ��Ӧ���� */

	struct xm_line_node *next;   /* ��һ�����ָ�� */
}XM_LINE_NODE;


/* ����� */
typedef struct xm_polygon
{
	XM_POINT_NODE *point_head;     /* ������ͷ��� */
	 /*XM_LINE_NODE *line_head;     ������ͷ��� */
	
	XM_COLOR color;             /* ��ɫ */
	long visible;            /* ��Ŀɼ��� */
}XM_POLYGON;


/* ����������� */
typedef struct xm_polygon_node
{
	XM_POLYGON *polygon;           /* ��Ӧ�Ķ���� */

	struct xm_polygon_node *next;       /* ��һ�����ָ�� */
}XM_POLYGON_NODE;


/* ģ�� */
typedef struct xm_model
{
	XM_POINT_NODE *point_head;        /* ������ͷ��� */
	XM_LINE_NODE *line_head;         /* ������ͷ��� */
	XM_POLYGON_NODE *polygon_head;    /* ���������ͷ��� */
	XM_COLOR bordercolor;	/* �߿���ɫ */

	XM_POINT *offset;         /* ģ�͵�ƫ���� */
	enum XM_MODEL_TYPE type;          /* ���� */

	float radius;            /* ��Χ��뾶 */
	float depth;             /* ģ�͵���� */
	int style;              /* ������ʽ */
	long visible;       /* ģ�͵Ŀɼ��� */

}XM_MODEL;


/* ģ�������� */
typedef struct xm_model_node
{
	XM_MODEL *model;                  /* ��Ӧ��ģ�� */

	struct xm_model_node *next;            /* ��һ����� */

}XM_MODEL_NODE;



/* ���� */
typedef struct xm_field
{
	XM_MODEL_NODE *model_head;           /* ģ������ָ�� */
}XM_FIELD;


/****************************************************************/

/* �ӵ� */
typedef struct xm_view
{
	XM_POINT *pst;               /* �ӵ�λ������ */
	XM_POINT *drt;               /* �ӵ㷽������ */
	XM_POINT *Xe;                /* ����ˮƽ���굥λ���� */
	XM_POINT *Ye;                /* ������ֱ���굥λ���� */

	float A, B, C, D;            /* ���淽�̲��� */

	float width;                /* ������� */
	float height;               /* �����߶� */
}XM_VIEW;



/* ���� */
typedef struct xm_window
{
	int left;               /* ������߽� */
	int top;                /* �����ϱ߽� */ 
	int width;              /* ���ڿ�� */
	int height;             /* ���ڸ߶� */
}XM_WINDOW;