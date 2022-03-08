#include "haning.h"
#include "mrc_bmp.h "
#include "x_graphics.h "

void Lib_RectRe(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	RectRe(Param[0]->Val->Pointer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer,Param[4]->Val->Integer);
}
void Lib_Rectlinepd(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	Rectlinepd(Param[0]->Val->Pointer);
}

void Lib_Intersect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Integer = Intersect(Param[0]->Val->Pointer,Param[1]->Val->Pointer);
}
void Lib_pointpd(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Integer = pointpd(Param[0]->Val->Integer,Param[1]->Val->Integer);
}
void Lib_sectRect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	sectRect(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}

void Lib_gr_cls(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_cls(*(mr_colourSt*)Param[0]->Val->Pointer);
}

void Lib_gr_point(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_point(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->UnsignedShortInteger);
}
void Lib_gr_pointex(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_pointex(Param[0]->Val->Integer,Param[1]->Val->Integer,*(mr_colourSt*)Param[2]->Val->Pointer);
}

void Lib_gr_line(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_line(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer,*(mr_colourSt*)Param[4]->Val->Pointer);
}

void Lib_gr_rect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_rect(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer,*(mr_colourSt*)Param[4]->Val->Pointer);
}

void Lib_gr_fill_area(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_fill_area(Param[0]->Val->Pointer,Param[1]->Val->Integer,*(mr_colourSt*)Param[2]->Val->Pointer);
}


void Lib_gr_bigger(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_bigger(Param[0]->Val->Pointer,Param[1]->Val->Integer,*(mr_colourSt*)Param[2]->Val->Pointer);
}

void Lib_gr_Circle(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_Circle(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,*(mr_colourSt*)Param[3]->Val->Pointer);
}

void Lib_gr_Ellipse(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_Ellipse(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer,*(mr_colourSt*)Param[4]->Val->Pointer);
}

void Lib_getpixel(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->UnsignedInteger = getpixel(Param[0]->Val->Integer,Param[1]->Val->Integer);
}

void Lib_gr_Flood_Fill_4(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_Flood_Fill_4(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->UnsignedShortInteger,Param[3]->Val->UnsignedShortInteger);
}

void Lib_gr_set_clip_rect(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_set_clip_rect(Param[0]->Val->Integer,Param[1]->Val->Integer,Param[2]->Val->Integer,Param[3]->Val->Integer);
}

void Lib_gr_graph(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_graph();
}

void Lib_gr_init(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_init();
}

void Lib_gr_close(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	gr_close();
}

////////////////////////////
const  char mrc_graphics_Defs[] = "\
typedef struct{int left;int top;int width;int height;}RECT;\n\
typedef struct{int x;int y;}EACH_POINT;\
";

int32 graphics_init()
{
	Haninit(&mrc_graphics_Defs[0]);

	AddString(Lib_RectRe,"void RectRe(RECT*,int,int,int,int);");
	AddString(Lib_Rectlinepd,"void Rectlinepd(RECT*);");
	AddString(Lib_Intersect,"int Intersect(RECT*,RECT*);");
	AddString(Lib_pointpd,"int pointpd(int,int);");
	AddString(Lib_sectRect,"void sectRect(RECT*,RECT*,RECT*);");
	AddString(Lib_gr_cls,"void gr_cls(mr_colourSt*);");
	AddString(Lib_gr_point,"void gr_point(int,int,uint16);");
	AddString(Lib_gr_pointex,"void gr_pointex(int,int,mr_colourSt*);");
	AddString(Lib_gr_line,"void gr_line(int,int,int,int,mr_colourSt*);");
	AddString(Lib_gr_rect,"void gr_rect(int,int,int,int,mr_colourSt*);");
	AddString(Lib_gr_fill_area,"void gr_fill_area(EACH_POINT*,int,mr_colourSt*);");
	AddString(Lib_gr_bigger,"void gr_bigger(EACH_POINT*,int,mr_colourSt*) ;");
	AddString(Lib_gr_Circle,"void gr_Circle(int,int,int,mr_colourSt*);");
	AddString(Lib_gr_Ellipse,"void gr_Ellipse(int,int,int,int,mr_colourSt*);");
	AddString(Lib_getpixel,"uint16 getpixel(int,int);");
	AddString(Lib_gr_Flood_Fill_4,"int32 gr_flood_fill_4(int,int,uint16,uint16);");
	AddString(Lib_gr_set_clip_rect,"void gr_set_clip_rect(int,int,int,int);");
	AddString(Lib_gr_graph,"void gr_graph();");
	AddString(Lib_gr_init,"void gr_init();");
	AddString(Lib_gr_close,"void gr_close();");





	return hanclose();

}

