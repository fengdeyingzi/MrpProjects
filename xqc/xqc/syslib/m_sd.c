#include "haning.h"
#include "xm.h"
int32 sd_init(void);

/************************************************************
  FileName: create.h
  Description:           基本模型动态建立的函数声明
***********************************************************/

void Lib_create_point(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = create_point(Param[0]->Val->FP,Param[1]->Val->FP,Param[2]->Val->FP);
}

void Lib_create_point_node(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = create_point_node(Param[0]->Val->Pointer);
}

void Lib_create_line(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = create_line(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_create_line_node(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = create_line_node(Param[0]->Val->Pointer);
}

void Lib_create_polygon(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = create_polygon();
}

void Lib_create_polygon_node(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = create_polygon_node(Param[0]->Val->Pointer);
}

void Lib_polygon_add_point(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	polygon_add_point(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_create_model(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = create_model(Param[0]->Val->Pointer);
}
void Lib_create_model_node(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer = create_model_node(Param[0]->Val->Pointer);
}

void Lib_model_add_point(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	model_add_point(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_model_add_line(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	model_add_line(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_model_add_polygon(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	model_add_polygon(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_create_field(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	create_field();
}

void Lib_field_add_model(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	field_add_model(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}
void Lib_create_view(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =create_view(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->FP);
}

void Lib_create_window(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =create_window(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer);
}
/************************************************************
  FileName: xmmath.h
  Description:			    增加模型变换函数接口
***********************************************************/
void Lib_set_XeYe(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	set_XeYe(Param[0]->Val->Pointer);
}
void Lib_set_ABCD(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	set_ABCD(Param[0]->Val->Pointer);
}


void Lib_point_prj(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	point_prj(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_point_move(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	point_move(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_point_revolve(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	point_revolve(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP);
}

void Lib_point_revolve_x(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	point_revolve_x(Param[0]->Val->Pointer,Param[1]->Val->FP);
}

void Lib_point_revolve_y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	point_revolve_y(Param[0]->Val->Pointer,Param[1]->Val->FP);
}

void Lib_point_revolve_z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	point_revolve_z(Param[0]->Val->Pointer,Param[1]->Val->FP);
}


void Lib_translation(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	translation(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}

void Lib_proportion_change(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	proportion_change(Param[0]->Val->Pointer,Param[1]->Val->FP);
}

void Lib_model_move(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	model_move(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}


void Lib_model_revolve(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	model_revolve(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP);
}

void Lib_model_revolve_x(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	model_revolve_x(Param[0]->Val->Pointer,Param[1]->Val->FP);
}

void Lib_model_revolve_y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	model_revolve_y(Param[0]->Val->Pointer,Param[1]->Val->FP);
}

void Lib_model_revolve_z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	model_revolve_z(Param[0]->Val->Pointer,Param[1]->Val->FP);
}

void Lib_symmetry(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	symmetry(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}
/************************************************************
  FileName: model.h
  Description:       物体模型的建立函数的声明  
***********************************************************/

void Lib_model_tetrahedron(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =model_tetrahedron(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->FP);
}

void Lib_model_hexahedron(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =model_hexahedron(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->FP);
}

void Lib_model_octahedron(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =model_octahedron(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->FP);
}

void Lib_model_dodecahedron(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =model_dodecahedron(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->FP);
}

void Lib_model_icosahedron(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =model_icosahedron(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->FP);
}

void Lib_model_curve(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =model_curve(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->FP,Param[4]->Val->FP,Param[5]->Val->FP,Param[6]->Val->Integer,Param[7]->Val->Integer,Param[8]->Val->LongInteger);
}
void Lib_model_threecubedbezier(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	float reference_point[16][3],*temp=Param[5]->Val->Pointer;
	int i,j;
	for(i=0;i<16;i++)
		for(j=0;j<3;j++)
			reference_point[i][j]=temp[i*3+j];
	ReturnValue->Val->Pointer =model_threecubedbezier(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->Integer,Param[4]->Val->Integer,reference_point);
}

void Lib_model_twocubedbezier(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	float reference_point[9][3],*temp=Param[5]->Val->Pointer;
	int i,j;
	for(i=0;i<9;i++)
		for(j=0;j<3;j++)
			reference_point[i][j]=temp[i*3+j];
	ReturnValue->Val->Pointer =model_twocubedbezier(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->Integer,Param[4]->Val->Integer,reference_point);
}
void Lib_model_twobezier(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	float reference_point[3][3],*temp=Param[5]->Val->Pointer;
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			reference_point[i][j]=temp[i*3+j];
	ReturnValue->Val->Pointer =model_twobezier(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->Integer,reference_point);
}

void Lib_model_threebezier(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	float reference_point[4][3],*temp=Param[5]->Val->Pointer;
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<3;j++)
			reference_point[i][j]=temp[i*3+j];
	ReturnValue->Val->Pointer =model_threebezier(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->Integer,reference_point);
}

void Lib_model_leptospira(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =model_leptospira(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->FP,Param[4]->Val->FP,Param[5]->Val->Integer,Param[6]->Val->Integer,Param[7]->Val->FP,Param[8]->Val->FP);
}

void Lib_model_annular(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Pointer =model_annular(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->FP,Param[3]->Val->Integer,Param[4]->Val->Integer,Param[5]->Val->FP,Param[6]->Val->FP);
}
/************************************************************
  FileName: draw.h
  Description:           模型场景绘制函数接口
***************************************************************/

void Lib_draw_model(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	draw_model(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}


void Lib_draw_field(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	draw_field(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}


/************************************************************
  FileName: graphics.h
        设备无关的图形函数接口  
***********************************************************/

void Lib_init_graph(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	init_graph();
}

void Lib_close_graph(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	close_graph();
}

void Lib_set_color(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	set_color((XM_COLOR)Param[0]->Val->LongInteger);
}

void Lib_set_fillcolor(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	set_fillcolor((XM_COLOR)Param[0]->Val->LongInteger);
}

void Lib_put_pixel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	put_pixel(Param[0]->Val->Integer,Param[1]->Val->Integer,(XM_COLOR)Param[2]->Val->LongInteger);
}

void Lib_put_line(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	put_line(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer);
}

void Lib_put_polyline(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	put_polyline(Param[0]->Val->Pointer,Param[1]->Val->Integer);
}

void Lib_put_polygon(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	put_polygon(Param[0]->Val->Pointer,Param[1]->Val->Integer);
}

void Lib_put_rectangle(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	put_rectangle(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer);
}

void Lib_set_clip_rectangle(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	set_clip_rectangle(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer);
}

void Lib_flush_graph(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	flush_graph();
}



const  char mrc_3d_Defs[]="\
enum{XM_FALSE,XM_TRUE};\n\
typedef long XM_COLOR;\n\
enum{TETRAHEDRON,HEXAHEDRON,OCTAHEDRON,DODECAHEDRON,ICOSAHEDRON,\
CURVE,THREECUBEDBEZIER,TWOCUBEDBEZIER,THREEBEZIER,TWOBEZIER,LEPTOSPIRA,ANNULAR};\n\
enum{ELLIPSE_TAPER,ELLIPSOID,BIFOLIATE,UNIFOLIATE};\n\
typedef struct xm_point2d{int x;int y;}XM_POINT2D;\n\
typedef struct xm_point{float x;float y;float z;float prj_x;float prj_y;int visible;}XM_POINT;\n\
typedef struct xm_point_node{XM_POINT *point;struct xm_point_node *next;}XM_POINT_NODE;\n\
typedef struct xm_line{XM_POINT *p1;XM_POINT *p2;int visible;}XM_LINE;\n\
typedef struct xm_line_node{XM_LINE *line;struct xm_line_node *next;}XM_LINE_NODE;\n\
typedef struct xm_polygon{XM_POINT_NODE *point_head;XM_COLOR color;int visible;}XM_POLYGON;\n\
typedef struct xm_polygon_node{XM_POLYGON *polygon;struct xm_polygon_node *next;}XM_POLYGON_NODE;\n\
typedef struct xm_model{XM_POINT_NODE *point_head;XM_LINE_NODE *line_head;XM_POLYGON_NODE *polygon_head;XM_COLOR bordercolor;\n\
XM_POINT *offset;enum XM_MODEL_TYPE type;float radius;float depth;int style;int visible;}XM_MODEL;\n\
typedef struct xm_model_node{XM_MODEL *model;struct xm_model_node *next;}XM_MODEL_NODE;\n\
typedef struct xm_field{XM_MODEL_NODE *model_head;}XM_FIELD;\n\
typedef struct xm_view{XM_POINT *pst;XM_POINT *drt;XM_POINT *Xe;XM_POINT *Ye;float A;float B;float C;float D;\
float width;float height;}XM_VIEW;\n\
typedef struct xm_window{int left;int top;int width;int height;}XM_WINDOW;\
#define DMS_HIDE 1\n\
#define DMS_CUT3D 2\n\
#define DMS_CUT2D 4\n\
#define DMS_LINE 8\n\
#define DMS_FRAME 16\n\
#define DMS_FILL 32\n\
#define PI ((float)3.1415926)\n\
#define SQR2 ((float)1.414)\n\
#define SQR3 ((float)1.732)\n\
#define SQR5 ((float)2.236)\n\
#define SQR6 ((float)2.449)\n\
";


int32 sd_init(void)
{
	Haninit(mrc_3d_Defs);

	AddString(Lib_create_point,"XM_POINT *create_point(float,float,float);");
	AddString(Lib_create_point_node,"XM_POINT_NODE *create_point_node(XM_POINT*);");

	AddString(Lib_create_line,"XM_LINE *create_line(XM_POINT *,XM_POINT *);");
	AddString(Lib_create_line_node,"XM_LINE_NODE *create_line_node(XM_LINE *);");

	AddString(Lib_create_polygon,"XM_POLYGON *create_polygon();");
	AddString(Lib_create_polygon_node,"XM_POLYGON_NODE *create_polygon_node(XM_POLYGON *);");
	AddString(Lib_polygon_add_point,"void polygon_add_point(XM_POLYGON *,XM_POINT *);");

	AddString(Lib_create_model,"XM_MODEL *create_model(XM_POINT *);");
	AddString(Lib_create_model_node,"XM_MODEL_NODE *create_model_node(XM_MODEL *);");
	AddString(Lib_model_add_point,"void model_add_point(XM_MODEL *,XM_POINT *);");
	AddString(Lib_model_add_line,"void model_add_line(XM_MODEL *, XM_LINE *);");
	AddString(Lib_model_add_polygon,"void model_add_polygon(XM_MODEL *,XM_POLYGON *);");

	AddString(Lib_create_field,"XM_FIELD *create_field();");
	AddString(Lib_field_add_model,"void field_add_model(XM_FIELD *,XM_MODEL *);");
	AddString(Lib_create_view,"XM_VIEW *create_view(XM_POINT *,XM_POINT *,float,float);");
	AddString(Lib_create_window,"XM_WINDOW *create_window(int,int,int,int);");



	AddString(Lib_set_XeYe,"void set_XeYe(XM_VIEW *);");
	AddString(Lib_set_ABCD,"void set_ABCD(XM_VIEW *);");
	AddString(Lib_point_prj,"void point_prj(XM_VIEW *, XM_POINT *);");

	AddString(Lib_point_move,"void point_move(XM_POINT *, XM_POINT *);");
	AddString(Lib_point_revolve,"void point_revolve(XM_POINT *, XM_POINT *,float);");
	AddString(Lib_point_revolve_x,"void point_revolve_x(XM_POINT*,float);");
	AddString(Lib_point_revolve_y,"void point_revolve_y(XM_POINT*,float);");
	AddString(Lib_point_revolve_z,"void point_revolve_z(XM_POINT*,float);");
	AddString(Lib_translation,"void translation(XM_POINT *,XM_POINT *);");
	AddString(Lib_proportion_change,"void proportion_change(XM_POINT *,float);");

	AddString(Lib_model_move,"void model_move(XM_MODEL*,XM_POINT*);");
	AddString(Lib_model_revolve,"void model_revolve(XM_POINT *, XM_POINT *,float);");
	AddString(Lib_model_revolve_x,"void model_revolve_x(XM_POINT*,float);");
	AddString(Lib_model_revolve_y,"void model_revolve_y(XM_POINT*,float);");
	AddString(Lib_model_revolve_z,"void model_revolve_z(XM_POINT*,float);");
	AddString(Lib_symmetry,"void symmetry(XM_POINT *, XM_POINT *, XM_POINT *);");





	AddString(Lib_model_tetrahedron,"XM_MODEL *model_tetrahedron(XM_POINT*,XM_POINT*,float,float);");
	AddString(Lib_model_hexahedron,"XM_MODEL *model_hexahedron(XM_POINT*,XM_POINT*,float,float);");
	AddString(Lib_model_octahedron,"XM_MODEL *model_octahedron(XM_POINT*,XM_POINT*,float,float);");
	AddString(Lib_model_dodecahedron,"XM_MODEL *model_dodecahedron(XM_POINT*,XM_POINT*,float,float);");
	AddString(Lib_model_icosahedron,"XM_MODEL *model_icosahedron(XM_POINT*,XM_POINT*,float,float);");
	AddString(Lib_model_curve,"XM_MODEL *model_curve(XM_POINT*,XM_POINT*,float,float,float,float,int,int,long);");
	AddString(Lib_model_threecubedbezier,"XM_MODEL *model_threecubedbezier(XM_POINT*,XM_POINT*,float,int,int,float*);");
	AddString(Lib_model_twocubedbezier,"XM_MODEL *model_twocubedbezier(XM_POINT*,XM_POINT*,float,int,int,float*);");
	AddString(Lib_model_twobezier,"XM_MODEL *model_twobezier(XM_POINT*,XM_POINT*,float,int,float*);");
	AddString(Lib_model_threebezier,"XM_MODEL *model_threebezier(XM_POINT*,XM_POINT*,float,int,float*);");
	AddString(Lib_model_leptospira,"XM_MODEL *model_leptospira(XM_POINT*,XM_POINT*,float,float,float,int,int,float,float);");
	AddString(Lib_model_annular,"XM_MODEL *model_annular(XM_POINT*,XM_POINT*,float,int,int,float,float);");




	AddString(Lib_draw_model,"void draw_model(XM_MODEL*,XM_VIEW*,XM_WINDOW*);");
	AddString(Lib_draw_field,"void draw_field(XM_FIELD*,XM_VIEW*,XM_WINDOW*);");







	AddString(Lib_init_graph,"void init_graph();");
	AddString(Lib_close_graph,"void close_graph();");
	AddString(Lib_set_color,"void set_color(long);");
	AddString(Lib_set_fillcolor,"void set_fillcolor(long);");
	AddString(Lib_put_pixel,"void put_pixel(int,int, long);");
	AddString(Lib_put_line,"void put_line(int,int,int,int);");
	AddString(Lib_put_polyline,"void put_polyline(XM_POINT2D*,int);");
	AddString(Lib_put_polygon,"void put_polygon(XM_POINT2D*,int);");
	AddString(Lib_put_rectangle,"void put_rectangle(int,int,int,int);");
	AddString(Lib_set_clip_rectangle,"void set_clip_rectangle(int,int,int,int);");
	AddString(Lib_flush_graph,"void flush_graph();");







	return hanclose();

}
