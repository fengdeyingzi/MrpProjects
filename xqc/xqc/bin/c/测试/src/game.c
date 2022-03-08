#include <3d/field.c>

#define MAX_X 10
#define MAX_Y 10
#define MAX_Z 20

#define SHAPE_COUNT 7

/* ���� */
typedef struct
{
	float offset_x;
	float offset_y;
	float offset_z;       /* �������ĵ�ƫ���� */
	float depth;           /* �������� */
	XM_COLOR color;         /* �������ɫ */
	long visible;          /* �ɼ��� */
}BLOCK;

/* ���� */
typedef struct
{
	BLOCK block[MAX_X][MAX_Y][MAX_Z];           /* �����ڵķ��� */
}AREA;


/* ��״ */
typedef struct
{
	BLOCK block[3][3][3];           /* ��״�����ķ��� */
	int x;int y;int z;            /* ��״��λ�� */
}SHAPE;
/* ��״������ */
long shape_visible[SHAPE_COUNT][27] ;/*= {
										 {0, 0, 0, 0, 0, 0, 0, 0, 0, 
										  0, 0, 0, 0, 1, 0, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0, 0},

										 {0, 0, 0, 0, 0, 0, 0, 0, 0, 
										  0, 0, 0, 1, 1, 0, 0, 0, 0,
										  0, 0, 0, 0, 1, 1, 0, 0, 0},

										 {0, 0, 0, 0, 0, 0, 0, 0, 0,
										  0, 0, 0, 1, 1, 1, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0, 0},

										 {0, 0, 0, 0, 0, 1, 0, 0, 0,
										  0, 0, 0, 1, 1, 1, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0, 0},

										 {0, 0, 0, 0, 1, 0, 0, 0, 0,
										  0, 0, 0, 1, 1, 1, 0, 0, 0,
										  0, 0, 0, 0, 0, 0, 0, 0, 0},

										 {0, 0, 0, 0, 1, 0, 0, 0, 0,
										  0, 0, 0, 1, 1, 1, 0, 0, 0,
										  0, 0, 0, 0, 1, 0, 0, 0, 0},

										 {0, 0, 0, 1, 1, 1, 0, 0, 0,
										  0, 0, 0, 1, 0, 1, 0, 0, 0,
										  0, 0, 0, 1, 1, 1, 0, 0, 0},
									};*/
XM_COLOR shape_color[SHAPE_COUNT] = {0x398454, 0x0000ff, 0x345363, 0x4929f5, 0x219123, 0x456721, 0x823747};

/* ģ�� */
XM_MODEL *ground;
XM_MODEL *model;
XM_MODEL *temp_model;

XM_VIEW *view;
XM_WINDOW *window;

AREA area;
SHAPE shape;
SHAPE temp_shape;



/****************************************************************************/



/* �����ʼ�� */
void init_area()
{
	int i, j, k;

	for (i = 0; i < MAX_Z; i++)
	{
		for (j = 0; j < MAX_Y; j++)
		{
			for (k = 0; k < MAX_X; k++)
			{
				area.block[k][j][i].offset_x = (float)(- MAX_X * 1.0 / 2 + 0.5 + k * 1.0);
				area.block[k][j][i].offset_y = (float)(- MAX_Y * 1.0 / 2 + 0.5 + j * 1.0);
				area.block[k][j][i].offset_z = (float)(MAX_Z * 1.0 - 0.5 - i * 1.0);
				area.block[k][j][i].visible = XM_FALSE;
			}
		}
	}
}


/* ����״װ������ */
void put_shape()
{
	int i, j, k;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				if (shape.block[k][j][i].visible == XM_TRUE)
				{
					area.block[k + shape.x][j + shape.y][i + shape.z].color 
						= shape.block[k][j][i].color;
					area.block[k + shape.x][j + shape.y][i + shape.z].visible
						= XM_TRUE;
				}
			}
		}
	}
}


/* �������������״ */
void get_shape()
{
	int i, j, k;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				if (shape.block[k][j][i].visible == XM_TRUE)
				{
					area.block[k + shape.x][j + shape.y][i + shape.z].visible = XM_FALSE;
				}
			}
		}
	}

}


/* ������񷽿� */
void sweep_area()
{
	int i, j, k, m;

	for (i = MAX_Z - 1; i >= 0; i--)
	{
		for (j = 0; j < MAX_Y; j++)
		{
			for (k = 0; k < MAX_X; k++)
			{
				if (area.block[k][j][i].visible == XM_FALSE)
				{
					break;
				}
			}

			if (k < MAX_X)     
			{
				break;
			}
		}

		if (j < MAX_Y)     /* û���� */
		{
			continue;
		}
		else      /* ���� */
		{
			for (m = i; m > 0; m--)
			{
				for (j = 0; j < MAX_Y; j++)
				{
					for (k = 0; k < MAX_X; k++)
					{
						area.block[k][j][m].visible  = area.block[k][j][m - 1].visible;
						area.block[k][j][m].color = area.block[k][j][m - 1].color;
					}
				}
			}
			i++;
		}
	}
}


/* �½���״ */
void new_shape()
{
	int type, n, t;
	int i, j, k;

	srand(time(NULL));
	type = (int)(rand() % 100 / 100.0 * SHAPE_COUNT);
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				shape.block[k][j][i].color = shape_color[type];
				shape.block[k][j][i].visible = shape_visible[type][i * 9 + j * 3 + k];
			}
		}
	}

	/* ����n�� */
	n = (int)(rand() % 100 / 100.0 * 4);
	temp_shape = shape;
	for (t = 1; t <= n; t++)
	{
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < 3; k++)
				{
					temp_shape.block[j][2-k][i] = shape.block[k][j][i];
				}
			}
		}
		shape = temp_shape;
	}

	/* ����n�� */
	n = (int)(rand() % 100 / 100.0 * 4);
	temp_shape = shape;
	for (t = 1; t <= n; t++)
	{
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < 3; k++)
				{
					temp_shape.block[k][2-i][j] = shape.block[k][j][i];
				}
			}
		}
		shape = temp_shape;
	}

	/* ������״�ĳ�ʼλ�� */
	shape.x = 3;
	shape.y = 3;
	shape.z = 0;
}


/* �ж������Ƿ���������״ */
long is_empty(SHAPE *shape)
{
	int i, j, k;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < 3; k++)
			{
				if (shape->block[k][j][i].visible == XM_TRUE)
				{
					if (i + shape->z > MAX_Z - 1
						|| k + shape->x < 0 || k + shape->x > MAX_X - 1
						|| j + shape->y < 0 || j + shape->y > MAX_Y - 1)
					{
						return XM_FALSE;
					}
						
					if (area.block[k + shape->x][j + shape->y][i + shape->z].visible == XM_TRUE)
					{
						return XM_FALSE;
					}
				}
			}
		}
	}
	return XM_TRUE;
};



/* ��ʾ���� */
void show_area()
{
	int i, j, k;
	BLOCK *temp_block;
	XM_POINT_NODE *point_node_pt1;
	XM_POINT_NODE *point_node_pt2;
	XM_POLYGON_NODE *polygon_node_pt;
	XM_POINT temp_offset;

	/* �������ֵ */
	for (i = 0; i < MAX_Z; i++)
	{
		for (j = 0; j < MAX_Y; j++)
		{
			for (k = 0; k < MAX_X; k++)
			{
				if (area.block[k][j][i].visible == XM_TRUE)
				{
					area.block[k][j][i].depth 
						= (float)sqrt((view->pst->x - area.block[k][j][i].offset_x)
									* (view->pst->x - area.block[k][j][i].offset_x)
									+ (view->pst->y - area.block[k][j][i].offset_y)
									 * (view->pst->y - area.block[k][j][i].offset_y)
									 + (view->pst->z - area.block[k][j][i].offset_z)
									 * (view->pst->z - area.block[k][j][i].offset_z));
				}
			}
		}
	}

	/* ���� */
	while (1)
	{
		temp_block = NULL;
		for (i = 0; i < MAX_Z; i++)
		{
			for (j = 0; j < MAX_Y; j++)
			{
				for (k = 0; k < MAX_X; k++)
				{
					if (area.block[k][j][i].visible == XM_TRUE
						&& area.block[k][j][i].depth >= 0.0)
					{
						if (temp_block == NULL)
						{
							temp_block = &area.block[k][j][i];
						}

						if (temp_block->depth < area.block[k][j][i].depth)
						{
							temp_block = &area.block[k][j][i];
						}
					}
				}
			}
		}

		if (temp_block != NULL)
		{
			point_node_pt1 = model->point_head;
			point_node_pt2 = temp_model->point_head;
			while (point_node_pt1->next != NULL)
			{
				point_node_pt1 = point_node_pt1->next;
				point_node_pt2 = point_node_pt2->next;
				point_node_pt2->point->x = point_node_pt1->point->x;
				point_node_pt2->point->y = point_node_pt1->point->y;
				point_node_pt2->point->z = point_node_pt1->point->z;
			}

			temp_offset.x = temp_block->offset_x;
			temp_offset.y = temp_block->offset_y;
			temp_offset.z = temp_block->offset_z;

			model_move(temp_model, &temp_offset);

			temp_model->bordercolor = 0x000000;
			polygon_node_pt = temp_model->polygon_head;
			while (polygon_node_pt->next != NULL)
			{
				polygon_node_pt = polygon_node_pt->next;
				polygon_node_pt->polygon->color = temp_block->color;
			}
			draw_model(temp_model, view, window);
			temp_block->depth = - 1.0;
		}
		else
		{
			break;
		}
	}
}

/********************************************************************/

/* ��ʼ�� */
void on_start()
{
	XM_POINT center = {0.0, 0.0, 1.0};
memset(shape_visible,0,4*SHAPE_COUNT*27);
shape_visible[0][13]=1;
shape_visible[1][12]=shape_visible[1][13]=shape_visible[1][21]=shape_visible[1][22]=1;
shape_visible[2][12]=shape_visible[2][13]=shape_visible[2][14]=1;
shape_visible[3][5]= shape_visible[3][12]= shape_visible[3][13]= shape_visible[3][14]=1;
shape_visible[4][5]= shape_visible[4][11]= shape_visible[4][12]= shape_visible[4][13]=1;
shape_visible[5][5]= shape_visible[5][11]= shape_visible[5][12]= shape_visible[5][13] = shape_visible[5][21] =1;
shape_visible[6][4]= shape_visible[6][5]= shape_visible[6][6]= shape_visible[6][12] = shape_visible[6][14] = shape_visible[6][20]=shape_visible[6][21]=shape_visible[6][22]= 1;
	/* ������ʼ�� */ 
	ground = create_ground(MAX_X, MAX_Y);
	ground->bordercolor = 0x00ff00;
	ground->style = DMS_LINE | DMS_CUT3D;/**/
	model = model_hexahedron(NULL, &center, (float)0.866, 0.0);
	model->bordercolor = 0x00ff00;
	model->style = DMS_FILL | DMS_CUT3D;
	temp_model = model_hexahedron(NULL, &center, (float)0.866, 0.0);
	temp_model->bordercolor = 0xffff00;
	temp_model->style = DMS_FILL | DMS_HIDE;

	/* �ӵ��ʼ�� */
	view = create_view(NULL, NULL, 4.0, 3.0);
	
	view->pst->x = -20.0;
	view->pst->y = -40.0;
	view->pst->z = 30.0;

	view->drt->x = 2.0;
	view->drt->y = 4.0;
	view->drt->z = -3.0;

	set_XeYe(view);
	set_ABCD(view);

	/* ����������ʼ�� */
	window = create_window(0, 0, SCRW, SCRH);

	/* �������� */
	init_area();

	/* ��������״ */
	new_shape();

	put_shape();
}


/* �����¼� */
void on_paint()
{
	init_graph();
	set_clip_rectangle(0, 0, 1023, 767);
	draw_model(ground, view, window);
	show_area();
	flush_graph();
	close_graph();
}


/* ���� */
int on_timer()
{
	get_shape();
	temp_shape = shape;
	temp_shape.z++;
	if (is_empty(&temp_shape) == XM_TRUE)   /* ������ */
	{
		shape = temp_shape;
		put_shape();
	}
	else         /* �������� */
	{
		put_shape();
		sweep_area();
		new_shape();
		if (is_empty(&shape) == XM_FALSE)
		{
			return 1;
		}
		put_shape();
	}
	return 0;
}




/* �����¼� */
void on_keydown(int32 wParam)
{
	float t;
	int i, j, k;
	XM_POINT temp_point;

	switch (wParam)
	{
	/************/
	/* �ӵ���� */
	/************/

	/* ���� */
	case 'A':
		point_revolve_z(view->pst, (float)(-0.1));
		point_revolve_z(view->drt, (float)(-0.1));
		set_XeYe(view);
		set_ABCD(view);
		break;

	/* ���� */
	case 'D':
		point_revolve_z(view->pst, (float)0.1);
		point_revolve_z(view->drt, (float)0.1);
		set_XeYe(view);
		set_ABCD(view);
		break;

	/* ���� */
	case 'W':
		temp_point.x = view->pst->x;
		temp_point.y = view->pst->y;
		temp_point.z = view->pst->z;
		point_revolve(&temp_point, view->Xe, -0.1);
		t = temp_point.z / sqrt(temp_point.x * temp_point.x 
			                  + temp_point.y * temp_point.y 
							  + temp_point.z * temp_point.z);
		if (t < 0.9)
		{
			view->pst->x = temp_point.x;
			view->pst->y = temp_point.y;
			view->pst->z = temp_point.z;
			point_revolve(view->drt, view->Xe, -0.1);
			set_XeYe(view);
			set_ABCD(view);
		}
		break;

	/* ���� */
	case 'S':
		temp_point.x = view->pst->x;
		temp_point.y = view->pst->y;
		temp_point.z = view->pst->z;
		point_revolve(&temp_point, view->Xe, 0.1);
		t = temp_point.z / sqrt(temp_point.x * temp_point.x 
			                  + temp_point.y * temp_point.y 
							  + temp_point.z * temp_point.z);
		if (t > 0.0)
		{
			view->pst->x = temp_point.x;
			view->pst->y = temp_point.y;
			view->pst->z = temp_point.z;
			point_revolve(view->drt, view->Xe, 0.1);
			set_XeYe(view);
			set_ABCD(view);
		}
		break;

	/* �ӽ� */
	case 'I':
		t = (float)sqrt(view->pst->x * view->pst->x + view->pst->y * view->pst->y);
		if (t > sqrt(MAX_X * MAX_X + MAX_Y * MAX_Y) / 2 + 1.0)
		{
			view->pst->x *= (float)0.99;
			view->pst->y *= (float)0.99;
			view->pst->z *= (float)0.99;
			set_XeYe(view);
			set_ABCD(view);
		}
		break;

	/* Զ�� */
	case 'K':
		view->pst->x *= (float)1.01;
		view->pst->y *= (float)1.01;
		view->pst->z *= (float)1.01;
		set_XeYe(view);
		set_ABCD(view);
		break;

	/************/
	/* �ƶ���״ */
	/************/
	
	/* ���� */
	case VK_LEFT:
		get_shape();
		temp_shape = shape;
		temp_shape.x--;
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;

	/* ���� */
	case VK_RIGHT:
		get_shape();
		temp_shape = shape;
		temp_shape.x++;
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;

	/* ǰ�� */
	case VK_DOWN:
		get_shape();
		temp_shape = shape;
		temp_shape.y--;
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;

	/* ���� */
	case VK_UP:
		get_shape();
		temp_shape = shape;
		temp_shape.y++;
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;

	/* ���� */
	case VK_SPACE:
		get_shape();
		temp_shape = shape;
		temp_shape.z++;
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;

	/************/
	/* ��ת��״ */
	/************/
	
	/* ���� */
	case VK_NUMPAD4:
		get_shape();
		temp_shape = shape;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < 3; k++)
				{
					temp_shape.block[j][2 - k][i] = shape.block[k][j][i];
				}
			}
		}
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;

	/* ���� */
	case VK_NUMPAD6:
		get_shape();
		temp_shape = shape;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < 3; k++)
				{
					temp_shape.block[2 - j][k][i] = shape.block[k][j][i];
				}
			}
		}
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;

	/* ���� */
	case VK_NUMPAD8:
		get_shape();
		temp_shape = shape;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < 3; k++)
				{
					temp_shape.block[k][2 - i][j] = shape.block[k][j][i];
				}
			}
		}
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;

	/* ���� */
	case VK_NUMPAD5:
		get_shape();
		temp_shape = shape;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < 3; k++)
				{
					temp_shape.block[k][i][2 - j] = shape.block[k][j][i];
				}
			}
		}
		if (is_empty(&temp_shape) == XM_TRUE)
		{
			shape = temp_shape;
		}
		put_shape();
		break;
	}
}
