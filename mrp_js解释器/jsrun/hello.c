#include <stdio.h>
#include <stdlib.h>
#include "mujs.h"
#include <mrc_base.h>
#include <mrc_base_i.h>


static void hello(js_State *J)
{
	const char *name = js_tostring(J, 1);
	mrc_printf("Hello,影子  %s!\n", name);
	js_pushundefined(J);
}

void js_drect(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int dw = js_toint32(J,3);
    int dh = js_toint32(J,4);
    int dr = js_toint32(J,5);
    int dg = js_toint32(J,6);
    int db = js_toint32(J,7);
    mrc_printf("drect(%d,%d,%d,%d,%d,%d,%d);\n",dx,dy,dw,dh,dr,dg,db);
    js_pushundefined(J);
}

void js_dline(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int dw = js_toint32(J,3);
    int dh = js_toint32(J,4);
    int dr = js_toint32(J,5);
    int dg = js_toint32(J,6);
    int db = js_toint32(J,7);
    mrc_printf("dline(%d,%d,%d,%d,%d,%d,%d);\n",dx,dy,dw,dh,dr,dg,db);
    js_pushundefined(J);
}


void js_dpointex(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int dr = js_toint32(J,3);
    int dg = js_toint32(J,4);
    int db = js_toint32(J,5);
    mrc_printf("dpointex(%d,%d,%d,%d,%d);\n",dx,dy,dr,dg,db);
    js_pushundefined(J);
}

void js_dtext(js_State *J){
    const char *text = js_tostring(J,1);
    int dx = js_toint32(J,2);
    int dy = js_toint32(J,3);
    int dr = js_toint32(J,4);
    int dg = js_toint32(J,5);
    int db = js_toint32(J,6);
    int font = js_toint32(J,7);
    mrc_printf("dtext(%s,%d,%d,%d,%d,%d,%d);\n",text,dx,dy,dr,dg,db,font);
    js_pushundefined(J);
}



void js_drawCir(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int r = js_toint32(J,3);
    int color = js_toint32(J,4);
    mrc_printf("drawCir(%d,%d,%d,%d);\n",dx,dy,r,color);
    js_pushundefined(J);
}

void js_drawLine(js_State *J){
    int x1 = js_toint32(J,1);
    int y1 = js_toint32(J,2);
    int x2 = js_toint32(J,3);
    int y2 = js_toint32(J,4);
    int color = js_toint32(J,5);

    mrc_printf("drawLine(%d,%d,%d,%d,%d);\n",x1,y1,x2,y2,color);
    js_pushundefined(J);
}

void js_drawPoint(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int color = js_toint32(J,3);

    mrc_printf("drawPoint(%d,%d,%d);\n",dx,dy,color);
    js_pushundefined(J);
}

void js_drawTriangle(js_State *J){
    int x1 = js_toint32(J,1);
    int y1 = js_toint32(J,2);
    int x2 = js_toint32(J,3);
    int y2 = js_toint32(J,4);
    int x3 = js_toint32(J,5);
    int y3 = js_toint32(J,6);
    int color = js_toint32(J,7);
    mrc_printf("drawTriangle(%d,%d,%d,%d,%d,%d,%d);\n",x1,y1,x2,y2,x3,y3,color);
    js_pushundefined(J);
}




int event(int type,int p1,int p2){
    
    return 0;
}

int pause(){
    return 0;
}

int resume(){
    return 0;
}

int exitApp(){
    return 0;
}
int init()
{
	
    return 0;
    // system("pause");
}
int SCRW;
int SCRH;
void drawPrint(char *text){
    mrc_clearScreen(255,255,255);
	mrc_drawText(text,0,0, 20,20,20, 0, 1);
	mrc_refreshScreen(0,0,SCRW,SCRH);
    mrc_sleep(300);
    mrc_printf("drawPrint %s",text);
}


int32 mrc_init()
{
    js_State *J = NULL;
    mr_screeninfo scrinfo;
    jsV_init();
    jsS_init();
    
	
	mrc_getScreenInfo(&scrinfo);
	SCRW = scrinfo.width;
	SCRH = scrinfo.height;
	mrc_clearScreen(255,255,255);
	mrc_drawText("helloworld",0,0, 20,20,20, 0, 1);
	mrc_refreshScreen(0,0,SCRW,SCRH);
    mrc_sleep(100);
    
    J = js_newstate(NULL, NULL, JS_STRICT);
    drawPrint("new");
    
	js_newcfunction(J, hello, "hello", 1);
    js_setglobal(J, "hello");
drawPrint("hello");
	js_newcfunction(J, js_drect, "drect", 7);
    js_setglobal(J, "drect");
drawPrint("drect");
    // js_newcfunction(J, js_dline, "dline", 7);
    // js_setglobal(J, "dline");

    // js_newcfunction(J, js_dpointex, "dpointex", 5);
    // js_setglobal(J, "dpointex");

    // js_newcfunction(J, js_dtext, "dtext", 7);
    // js_setglobal(J, "dtext");
    // js_dofile
	js_dostring(J, "hello('world'); \n\nconsole.log('log\n');");
    drawPrint("ok");
    // js_dostring(J, "var obj = {x:20,y:20};drect(obj.x,obj.y,30,30,255,255,255);dline(0,0,20,20,240,240,240); dpointex(0,0,240,240,240); var text = \"hhhk\"; dtext(text.charAt(0),0,0,240,240,240,1);");
	js_freestate(J);
    
	return 0;
}

int32 mrc_event(int32 type, int32 p0, int32 p1)
{
    if(type == MR_KEY_UP){
        mrc_exit();
    }
	return 0;
}
int32 mrc_pause()
{
	return 0;
}
int32 mrc_resume()
{
	return 0;
}
int32 mrc_exitApp()
{
	return 0;
}

 int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1){
	return 0;
 }
 int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5){
	return 0;
}

