#include "jsi.h"
#include "jsparse.h"
#include "jscompile.h"
#include "jsvalue.h"
#include "jsrun.h"
#include "jsbuiltin.h"
#include <mrc_base.h>
#include <assert.h>
#include <errno.h>
#include <mrc_base.h>
#include "base.h"

static void *js_defaultalloc(void *actx, void *ptr, int size)
{
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
	if (size == 0) {
		free(ptr);
		return NULL;
	}
#endif
    if(ptr == NULL){
		return malloc((size_t)size);
	}
	return realloc(ptr, (size_t)size);

}

static void js_defaultreport(js_State *J, const char *message)
{
	// fputs(message, stderr); //xldebug
	// fputc('\n', stderr);
}

static void js_defaultpanic(js_State *J)
{
	js_report(J, "uncaught exception");
	/* return to javascript to abort */
}

int js_ploadstring(js_State *J, const char *filename, const char *source)
{
	if (js_try(J))
		return 1;
	js_loadstring(J, filename, source);
	js_endtry(J);
	return 0;
}

int js_ploadfile(js_State *J, const char *filename)
{
	if (js_try(J))
		return 1;
	js_loadfile(J, filename);
	js_endtry(J);
	return 0;
}

const char *js_trystring(js_State *J, int idx, const char *error)
{
	const char *s;
	if (js_try(J)) {
		js_pop(J, 1);
		return error;
	}
	s = js_tostring(J, idx);
	js_endtry(J);
	return s;
}

double js_trynumber(js_State *J, int idx, double error)
{
	double v;
	if (js_try(J)) {
		js_pop(J, 1);
		return error;
	}
	v = js_tonumber(J, idx);
	js_endtry(J);
	return v;
}

int js_tryinteger(js_State *J, int idx, int error)
{
	int v;
	if (js_try(J)) {
		js_pop(J, 1);
		return error;
	}
	v = js_tointeger(J, idx);
	js_endtry(J);
	return v;
}

int js_tryboolean(js_State *J, int idx, int error)
{
	int v;
	if (js_try(J)) {
		js_pop(J, 1);
		return error;
	}
	v = js_toboolean(J, idx);
	js_endtry(J);
	return v;
}

static void js_loadstringx(js_State *J, const char *filename, const char *source, int iseval)
{
	js_Ast *P;
	js_Function *F;
	mrc_printf("js_loadstringx %d",1);
	if (js_try(J)) {
		jsP_freeparse(J);
		js_throw(J);
	}
mrc_printf("js_loadstringx %d",2);
	P = jsP_parse(J, filename, source);
	mrc_printf("js_loadstringx %d",3);
	F = jsC_compilescript(J, P, iseval ? J->strict : J->default_strict);
	jsP_freeparse(J);
	mrc_printf("js_loadstringx %d",4);
	js_newscript(J, F, iseval ? (J->strict ? J->E : NULL) : J->GE, iseval ? JS_CEVAL : JS_CSCRIPT);
mrc_printf("js_loadstringx %d",5);
	js_endtry(J);
}

void js_loadeval(js_State *J, const char *filename, const char *source)
{
	js_loadstringx(J, filename, source, 1);
}

void js_loadstring(js_State *J, const char *filename, const char *source)
{
	js_loadstringx(J, filename, source, 0);
}

void js_loadfile(js_State *J, const char *filename)
{
	CAPP_FILE *f;
	char *s, *p;
	int n, t;

	f = capp_fopen(filename, "rb");
	if (!f) {
		// js_error(J, "cannot open file '%s': %s", filename, strerror(errno)); //xldebug
		js_error(J, "cannot open file '%s'", filename);
	}

	if (capp_fseek(f, 0, SEEK_END) < 0) {
		capp_fclose(f);
		// js_error(J, "cannot seek in file '%s': %s", filename, strerror(errno)); //xldebug
		js_error(J, "cannot seek in file '%s'", filename);
	}

	n = capp_ftell(f);
	if (n < 0) {
		capp_fclose(f);
		// js_error(J, "cannot tell in file '%s': %s", filename, strerror(errno)); //xldebug
		js_error(J, "cannot tell in file '%s'", filename);
	}

	if (capp_fseek(f, 0, SEEK_SET) < 0) {
		capp_fclose(f);
		// js_error(J, "cannot seek in file '%s': %s", filename, strerror(errno)); //xldebug
		js_error(J, "cannot seek in file '%s'", filename);
	}

	if (js_try(J)) {
		capp_fclose(f);
		js_throw(J);
	}
	s = js_malloc(J, n + 1); /* add space for string terminator */
	js_endtry(J);

	t = capp_fread(s, 1, (size_t)n, f);
	if (t != n) {
		js_free(J, s);
		capp_fclose(f);
		// js_error(J, "cannot read data from file '%s': %s", filename, strerror(errno)); //xldebug
		js_error(J, "cannot read data from file '%s'", filename);
	}

	s[n] = 0; /* zero-terminate string containing file data */

	if (js_try(J)) {
		js_free(J, s);
		capp_fclose(f);
		js_throw(J);
	}

	/* skip first line if it starts with "#!" */
	p = s;
	if (p[0] == '#' && p[1] == '!') {
		p += 2;
		while (*p && *p != '\n')
			++p;
	}

	js_loadstring(J, filename, p);

	js_free(J, s);
	capp_fclose(f);
	js_endtry(J);
}

int js_dostring(js_State *J, const char *source)
{
	mrc_printf("js_dostring %d",1);
	if (js_try(J)) {
		js_report(J, js_trystring(J, -1, "Error"));
		js_pop(J, 1);
		return 1;
	}
	mrc_printf("js_dostring %d",2);
	js_loadstring(J, "[string]", source);
	mrc_printf("js_dostring %d",3);
	js_pushundefined(J);
	mrc_printf("js_dostring %d",4);
	js_call(J, 0);
	mrc_printf("js_dostring %d",5);
	js_pop(J, 1);
	mrc_printf("js_dostring %d",6);
	js_endtry(J);
	mrc_printf("js_dostring %d",7);
	return 0;
}

int js_dofile(js_State *J, const char *filename)
{
	if (js_try(J)) {
		js_report(J, js_trystring(J, -1, "Error"));
		js_pop(J, 1);
		return 1;
	}
	js_loadfile(J, filename);
	js_pushundefined(J);
	js_call(J, 0);
	js_pop(J, 1);
	js_endtry(J);
	return 0;
}

js_Panic js_atpanic(js_State *J, js_Panic panic)
{
	js_Panic old = J->panic;
	J->panic = panic;
	return old;
}

void js_report(js_State *J, const char *message)
{
	if (J->report)
		J->report(J, message);
}

void js_setreport(js_State *J, js_Report report)
{
	J->report = report;
}

void js_setcontext(js_State *J, void *uctx)
{
	J->uctx = uctx;
}

void *js_getcontext(js_State *J)
{
	return J->uctx;
}

js_State *js_newstate(js_Alloc alloc, void *actx, int flags)
{
	js_State *J;
    mrc_printf("js_newstate 1");
	assert(sizeof(js_Value) == 16);
	assert(soffsetof(js_Value, type) == 15);
mrc_printf("js_newstate 2");
	if (!alloc)
		alloc = js_defaultalloc;

	J = alloc(actx, NULL, sizeof *J);
	if (!J)
		return NULL;
	memset(J, 0, sizeof(*J));
	mrc_printf("js_newstate 3");
	J->actx = actx;
	J->alloc = alloc;
mrc_printf("js_newstate 4");
	if (flags & JS_STRICT)
		J->strict = J->default_strict = 1;

	J->trace[0].name = "-top-";
	J->trace[0].file = "native";
	J->trace[0].line = 0;
mrc_printf("js_newstate 5");
	J->report = js_defaultreport;
	J->panic = js_defaultpanic;

	J->stack = alloc(actx, NULL, JS_STACKSIZE * sizeof *J->stack);
	if (!J->stack) {
		alloc(actx, NULL, 0);
		return NULL;
	}
mrc_printf("js_newstate 6");
	J->gcmark = 1;
	J->nextref = 0;
	J->gcthresh = 0; /* reaches stability within ~ 2-5 GC cycles */

	J->R = jsV_newobject(J, JS_COBJECT, NULL);
	J->G = jsV_newobject(J, JS_COBJECT, NULL);
	J->E = jsR_newenvironment(J, J->G, NULL);
	J->GE = J->E;
mrc_printf("js_newstate 7");
	jsB_init(J);
mrc_printf("js_newstate 8");
	return J;
}
