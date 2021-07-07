#include "lualib.h"
#include "lauxlib.h"
#include "base.h"
#include <mrc_base.h>
static int printhelloworld(lua_State *L){
    mrc_printf("[canvas] helloworld\n");
	return 0;
}


static int lua_dpoint(lua_State *L){
	mrc_printf("dpoint\n");
	mrc_drawPointEx((int)luaL_checknumber(L,1),(int)luaL_checknumber(L,2),(int)luaL_checknumber(L,3),(int)luaL_checknumber(L,4),(int)luaL_checknumber(L,5));
	lua_pushnumber(L, 0);
	// ref(0,0,SCRW,SCRH);
	return 1;
}

static int lua_drect(lua_State *L){
	mrc_printf("drect\n");
	mrc_drawRect((int)luaL_checknumber(L,1),(int)luaL_checknumber(L,2),(int)luaL_checknumber(L,3),(int)luaL_checknumber(L,4),(int)luaL_checknumber(L,5),(int)luaL_checknumber(L,6),(int)luaL_checknumber(L,7));
	lua_pushnumber(L, 0);
	// ref(0,0,SCRW,SCRH);
	return 1;
}

static int lua_dline(lua_State *L){
	mrc_printf("dline\n");
	mrc_drawLine((int)luaL_checknumber(L,1),(int)luaL_checknumber(L,2),(int)luaL_checknumber(L,3),(int)luaL_checknumber(L,4),(int)luaL_checknumber(L,5),(int)luaL_checknumber(L,6),(int)luaL_checknumber(L,7));
	lua_pushnumber(L, 0);
	// ref(0,0,SCRW,SCRH);
	return 1;
}

static int lua_dtext(lua_State *L){
	mrc_printf("dtext\n");
	mrc_drawText((char*)luaL_checkstring(L,1),(int)luaL_checknumber(L,2),(int)luaL_checknumber(L,3),
	(int)luaL_checknumber(L,4),(int)luaL_checknumber(L,5),(int)luaL_checknumber(L,6),0,(int)luaL_checknumber(L,7));
	return 1;
}

static int lua_ref(lua_State *L){
	mrc_printf("ref\n");
	mrc_refreshScreen((int)luaL_checknumber(L,1),(int)luaL_checknumber(L,2),(int)luaL_checknumber(L,3),(int)luaL_checknumber(L,4));
	return 1;
}

static int lua_cls(lua_State *L){
	mrc_printf("cls\n");
	mrc_clearScreen((int)luaL_checknumber(L,1),(int)luaL_checknumber(L,2),(int)luaL_checknumber(L,3));
	return 1;
}
typedef struct LUA_TIMERDATA
{
	int data;
	char *timerCB;
	lua_State *L;
} LUA_TIMERDATA;


void lua_timerCB(int32 data){
	// printf("定时器回调");
	LUA_TIMERDATA *lua_timerdata = (LUA_TIMERDATA*) data;
	int lua_data = lua_timerdata -> data;
	lua_State *L = lua_timerdata -> L;
	// printf("timerCB %s data %d\n",lua_timerdata->timerCB,lua_data);
	lua_getglobal(L, lua_timerdata->timerCB);
	lua_pushnumber(L, lua_data);
	if(lua_pcall(L,1,0,0) != 0)
	  mrc_printf("cannot run timerCB\n");
	free(lua_timerdata);
	// lua_pop(L,1);
}

static int lua_timerStart(lua_State *L){
	LUA_TIMERDATA *lua_timerdata = malloc(sizeof(LUA_TIMERDATA));
	int timer = mrc_timerCreate();
	int data = (int)luaL_checknumber(L,1);
	int time = (int)luaL_checknumber(L,2);
	char *timerCB = (char*)luaL_checkstring(L,3);
	lua_timerdata->L = L;
	lua_timerdata->timerCB = timerCB;
	lua_timerdata->data = data;
	mrc_printf("timerstart");
	mrc_timerStart(timer, time, (int32)lua_timerdata, lua_timerCB, 0);
	mrc_printf("定时器启动");
	return 1;
}





static const luaL_Reg my_funcs[] = {
	// {"printhello",printhelloworld},
	// {"add",myadd},
		{"dpoint",lua_dpoint},
		{"drect",lua_drect},
		{"dline",lua_dline},
		{"cls",lua_cls},
		{"ref",lua_ref},
		{"dtext",lua_dtext},
		{"timerStart",lua_timerStart},

	{NULL,NULL}
};

LUAMOD_API int luaopen_mypackage(lua_State *L){
	// luaL_newlib(L,my_funcs);
	lua_pushglobaltable(L);
	luaL_setfuncs(L,my_funcs,0);
	return 1;
}

