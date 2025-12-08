/*
** $Id: lua.c,v 1.230.1.1 2017/04/19 17:29:57 roberto Exp $
** Lua stand-alone interpreter
** See Copyright Notice in lua.h
*/

#define lua_c

#include <mrc_base.h>
#include <signal.h>

#include "lprefix.h"

// #include <stdlib.h>
#include <mrc_base.h>

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
extern int SCRW;
extern int SCRH;

#if !defined(LUA_PROMPT)
#define LUA_PROMPT "> "
#define LUA_PROMPT2 ">> "
#endif

#if !defined(LUA_PROGNAME)
#define LUA_PROGNAME "lua"
#endif

#if !defined(LUA_MAXINPUT)
#define LUA_MAXINPUT 512
#endif

#if !defined(LUA_INIT_VAR)
#define LUA_INIT_VAR "LUA_INIT"
#endif

#define LUA_INITVARVERSION LUA_INIT_VAR LUA_VERSUFFIX

/*
** lua_stdin_is_tty detects whether the standard input is a 'tty' (that
** is, whether we're running lua interactively).
*/
#if !defined(lua_stdin_is_tty) /* { */

#if defined(LUA_USE_POSIX) /* { */

#include <unistd.h>
#define lua_stdin_is_tty() isatty(0)

#elif defined(LUA_USE_WINDOWS) /* }{ */

#include <io.h>
#include <windows.h>

#define lua_stdin_is_tty() _isatty(_fileno(stdin))

#else                        /* }{ */

/* ISO C definition */
#define lua_stdin_is_tty() 1 /* assume stdin is a tty */

#endif /* } */

#endif /* } */

/*
** lua_readline defines how to show a prompt and then read a line from
** the standard input.
** lua_saveline defines how to "save" a read line in a "history".
** lua_freeline defines how to free a line read by lua_readline.
*/
#if !defined(lua_readline) /* { */

#if defined(LUA_USE_READLINE) /* { */

#include <readline/history.h>
#include <readline/readline.h>
#define lua_readline(L, b, p) ((void)L, ((b) = readline(p)) != NULL)
#define lua_saveline(L, line) ((void)L, add_history(line))
#define lua_freeline(L, b) ((void)L, free(b))

#else /* }{ */

// xldebug
// #define lua_readline(L,b,p) \
//         ((void)L, fputs(p, stdout), fflush(stdout),  /* show prompt */ \
//         fgets(b, LUA_MAXINPUT, stdin) != NULL)  /* get line */

#define lua_readline(L, b, p) NULL

#define lua_saveline(L, line) \
  {                           \
    (void)L;                  \
    (void)line;               \
  }
#define lua_freeline(L, b) \
  {                        \
    (void)L;               \
    (void)b;               \
  }

#endif /* } */

#endif /* } */

static lua_State *globalL = NULL;

static char *progname;  // xldebug

lua_State *lua_state;

int32 lua_app_main(int argc, char *argv[]) {
  int a = 11;
  int b = 12;
  int iError;
  int ret;
  mrc_printf("lua_app_main %d",1);
  lua_state = luaL_newstate();
  mrc_printf("lua_app_main %d",11);
  luaL_openlibs(lua_state);
   mrc_printf("lua_state",11);
  //   luaL_dofile(lua_state, "test.lua");
  // luaL_loadfile(lua_state, "run.lua");
  luaL_dostring(lua_state, "print(\"test................\");");

  mrc_printf("lua_app_main %d",2);
  //  iError = lua_pcall(lua_state, 0, 1, 0);
  //  mrc_printf("lua_app_main %d",3);
  // if (iError)
  // {
  //   mrc_printf("error %d \n", iError);
  //   lua_close(lua_state);
  //   return 1;
  // }
  mrc_printf("lua_app_main %d",4);
  lua_getglobal(lua_state, "init");
   ret = lua_pcall(lua_state, 0, 1, 0);
  if (ret != 0)
  {
    mrc_printf("error %d\n",ret);
    // error(lua_state, "error running funFction %s",lua_tostring(lua_state,-1));
    return 1;
  }
  mrc_printf("init:%ld\n", lua_tointeger(lua_state, -1));
  lua_pop(lua_state, 1);
  mrc_printf("lua_app_main %d",5);
  /* 清除Lua */
  lua_close(lua_state);
  return 0;
}

/*
int32 lua_app_main(int argc, char **argv) {
  int status, result;
  progname = "lua";
  lua_state = luaL_newstate();  // create state
  if (lua_state == NULL) {
    // l_message(argv[0], "cannot create state: not enough memory");
    return -1;
  }
  // lua_pushcfunction(lua_state, &pmain);  // to call 'pmain' in protected mode
  lua_pushinteger(lua_state, argc);        // 1st argument
  lua_pushlightuserdata(lua_state, argv);  // 2nd argument
  lua_pushnumber(lua_state, SCRW);
  lua_setglobal(lua_state, "SCRW");
  lua_pushnumber(lua_state, SCRH);
  lua_setglobal(lua_state, "SCRH");

  status = lua_pcall(lua_state, 2, 1, 0);  // do the call
  result = lua_toboolean(lua_state, -1);   // get result
  // report(lua_state, status);
  // lua_close(lua_state);
  return (result && status == LUA_OK) ? MR_SUCCESS : MR_FAILED;
}*/

int32 lua_app_event(int32 type, int32 p1, int32 p2) {
  double renumber;
  lua_getglobal(lua_state, "event");
  lua_pushnumber(lua_state, type);
  lua_pushnumber(lua_state, p1);
  lua_pushnumber(lua_state, p2);

  if (lua_pcall(lua_state, 3, 1, 0) != 0) mrc_printf("cannot call event\n");
  if (!lua_isnumber(lua_state, -1)) {
    // printf(lua_state, "function 'event' nust return a number"); //xldebug
    mrc_printf("function 'event' nust return a number");
  }
  renumber = lua_tonumber(lua_state, -1);
  lua_pop(lua_state, 1);
  return (int32)renumber;
}

int32 lua_app_pause() {
  double renumber;
  lua_getglobal(lua_state, "pause");

  if (lua_pcall(lua_state, 0, 1, 0) != 0) mrc_printf("cannot call pause\n");
  if (!lua_isnumber(lua_state, -1)) {
    mrc_printf("function 'pause' nust return a number");
  }
  renumber = lua_tonumber(lua_state, -1);
  lua_pop(lua_state, 1);
  return (int32)renumber;
}

int32 lua_app_resume() {
  double renumber;
  lua_getglobal(lua_state, "resume");

  if (lua_pcall(lua_state, 0, 1, 0) != 0) mrc_printf("cannot call resume\n");
  if (!lua_isnumber(lua_state, -1)) {
    mrc_printf("function 'resume' nust return a number");
  }
  renumber = lua_tonumber(lua_state, -1);
  lua_pop(lua_state, 1);
  return (int32)renumber;
}

int32 lua_app_exit() {
  mrc_printf("lua_app_exit");
  lua_close(lua_state);
  return 0;
}
