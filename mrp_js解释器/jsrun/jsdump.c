#include "jsi.h"
#include "jsparse.h"
#include "jscompile.h"
#include "jsvalue.h"

#include "utf.h"
#include <mrc_base.h>
#include <assert.h>
#include "base.h"

static char astname[94][32];
static char opname[84][32];

static int minify = 0;

const char *jsP_aststring(enum js_AstType type)
{
	if (type < 92)
		return astname[type];
	return "<unknown>";
}

const char *jsC_opcodestring(enum js_OpCode opcode)
{
	if (opcode < 83)
		return opname[opcode];
	return "<unknown>";
}

static int prec(enum js_AstType type)
{
	switch (type) {
	case AST_IDENTIFIER:
	case EXP_IDENTIFIER:
	case EXP_NUMBER:
	case EXP_STRING:
	case EXP_REGEXP:
	case EXP_UNDEF:
	case EXP_NULL:
	case EXP_TRUE:
	case EXP_FALSE:
	case EXP_THIS:
	case EXP_ARRAY:
	case EXP_OBJECT:
		return 170;

	case EXP_FUN:
	case EXP_INDEX:
	case EXP_MEMBER:
	case EXP_CALL:
	case EXP_NEW:
		return 160;

	case EXP_POSTINC:
	case EXP_POSTDEC:
		return 150;

	case EXP_DELETE:
	case EXP_VOID:
	case EXP_TYPEOF:
	case EXP_PREINC:
	case EXP_PREDEC:
	case EXP_POS:
	case EXP_NEG:
	case EXP_BITNOT:
	case EXP_LOGNOT:
		return 140;

	case EXP_MOD:
	case EXP_DIV:
	case EXP_MUL:
		return 130;

	case EXP_SUB:
	case EXP_ADD:
		return 120;

	case EXP_USHR:
	case EXP_SHR:
	case EXP_SHL:
		return 110;

	case EXP_IN:
	case EXP_INSTANCEOF:
	case EXP_GE:
	case EXP_LE:
	case EXP_GT:
	case EXP_LT:
		return 100;

	case EXP_STRICTNE:
	case EXP_STRICTEQ:
	case EXP_NE:
	case EXP_EQ:
		return 90;

	case EXP_BITAND: return 80;
	case EXP_BITXOR: return 70;
	case EXP_BITOR: return 60;
	case EXP_LOGAND: return 50;
	case EXP_LOGOR: return 40;

	case EXP_COND:
		return 30;

	case EXP_ASS:
	case EXP_ASS_MUL:
	case EXP_ASS_DIV:
	case EXP_ASS_MOD:
	case EXP_ASS_ADD:
	case EXP_ASS_SUB:
	case EXP_ASS_SHL:
	case EXP_ASS_SHR:
	case EXP_ASS_USHR:
	case EXP_ASS_BITAND:
	case EXP_ASS_BITXOR:
	case EXP_ASS_BITOR:
		return 20;

#define COMMA 15

	case EXP_COMMA:
		return 10;

	default:
		return 0;
	}
}

static void pc(int c)
{
	capp_putchar(c);
}

static void ps(const char *s)
{
	// fputs(s, stdout); //xldebug
}

static void pn(int n)
{
	mrc_printf("%d", n);
}

static void in(int d)
{
	if (minify < 1)
		while (d-- > 0)
			capp_putchar('\t');
}

static void nl(void)
{
	if (minify < 2)
		capp_putchar('\n');
}

static void sp(void)
{
	if (minify < 1)
		capp_putchar(' ');
}

static void comma(void)
{
	capp_putchar(',');
	sp();
}

/* Pretty-printed Javascript syntax */

static void pstmlist(int d, js_Ast *list);
static void pexpi(int d, int i, js_Ast *exp);
static void pstm(int d, js_Ast *stm);
static void slist(int d, js_Ast *list);
static void sblock(int d, js_Ast *list);

static void pargs(int d, js_Ast *list)
{
	while (list) {
		assert(list->type == AST_LIST);
		pexpi(d, COMMA, list->a);
		list = list->b;
		if (list)
			comma();
	}
}

static void parray(int d, js_Ast *list)
{
	pc('[');
	while (list) {
		assert(list->type == AST_LIST);
		pexpi(d, COMMA, list->a);
		list = list->b;
		if (list)
			comma();
	}
	pc(']');
}

static void pobject(int d, js_Ast *list)
{
	pc('{');
	if (list) {
		nl();
		in(d+1);
	}
	while (list) {
		js_Ast *kv = list->a;
		assert(list->type == AST_LIST);
		switch (kv->type) {
		default: break;
		case EXP_PROP_VAL:
			pexpi(d+1, COMMA, kv->a);
			pc(':'); sp();
			pexpi(d+1, COMMA, kv->b);
			break;
		case EXP_PROP_GET:
			ps("get ");
			pexpi(d+1, COMMA, kv->a);
			ps("()"); sp(); pc('{'); nl();
			pstmlist(d+1, kv->c);
			in(d+1); pc('}');
			break;
		case EXP_PROP_SET:
			ps("set ");
			pexpi(d+1, COMMA, kv->a);
			pc('(');
			pargs(d+1, kv->b);
			pc(')'); sp(); pc('{'); nl();
			pstmlist(d+1, kv->c);
			in(d+1); pc('}');
			break;
		}
		list = list->b;
		if (list) {
			pc(',');
			nl();
			in(d+1);
		} else {
			nl();
			in(d);
		}
	}
	pc('}');
}

static void pstr(const char *s)
{
	char *HEX = "0123456789ABCDEF";
	Rune c;
	pc(minify ? '\'' : '"');
	while (*s) {
		s += chartorune(&c, s);
		switch (c) {
		case '\'': ps("\\'"); break;
		case '"': ps("\\\""); break;
		case '\\': ps("\\\\"); break;
		case '\b': ps("\\b"); break;
		case '\f': ps("\\f"); break;
		case '\n': ps("\\n"); break;
		case '\r': ps("\\r"); break;
		case '\t': ps("\\t"); break;
		default:
			if (c < ' ' || c > 127) {
				ps("\\u");
				pc(HEX[(c>>12)&15]);
				pc(HEX[(c>>8)&15]);
				pc(HEX[(c>>4)&15]);
				pc(HEX[c&15]);
			} else {
				pc(c); break;
			}
		}
	}
	pc(minify ? '\'' : '"');
}

static void pregexp(const char *prog, int flags)
{
	pc('/');
	ps(prog);
	pc('/');
	if (flags & JS_REGEXP_G) pc('g');
	if (flags & JS_REGEXP_I) pc('i');
	if (flags & JS_REGEXP_M) pc('m');
}

static void pbin(int d, int p, js_Ast *exp, const char *op)
{
	pexpi(d, p, exp->a);
	sp();
	ps(op);
	sp();
	pexpi(d, p, exp->b);
}

static void puna(int d, int p, js_Ast *exp, const char *pre, const char *suf)
{
	ps(pre);
	pexpi(d, p, exp->a);
	ps(suf);
}

static void pexpi(int d, int p, js_Ast *exp)
{
	int tp, paren;

	if (!exp) return;

	tp = prec(exp->type);
	paren = 0;
	if (tp < p) {
		pc('(');
		paren = 1;
	}
	p = tp;

	switch (exp->type) {
	case AST_IDENTIFIER: ps(exp->string); break;
	case EXP_IDENTIFIER: ps(exp->string); break;
	case EXP_NUMBER: mrc_printf("%.9g", exp->number); break;
	case EXP_STRING: pstr(exp->string); break;
	case EXP_REGEXP: pregexp(exp->string, exp->number); break;

	case EXP_UNDEF: break;
	case EXP_NULL: ps("null"); break;
	case EXP_TRUE: ps("true"); break;
	case EXP_FALSE: ps("false"); break;
	case EXP_THIS: ps("this"); break;

	case EXP_OBJECT: pobject(d, exp->a); break;
	case EXP_ARRAY: parray(d, exp->a); break;

	case EXP_DELETE: puna(d, p, exp, "delete ", ""); break;
	case EXP_VOID: puna(d, p, exp, "void ", ""); break;
	case EXP_TYPEOF: puna(d, p, exp, "typeof ", ""); break;
	case EXP_PREINC: puna(d, p, exp, "++", ""); break;
	case EXP_PREDEC: puna(d, p, exp, "--", ""); break;
	case EXP_POSTINC: puna(d, p, exp, "", "++"); break;
	case EXP_POSTDEC: puna(d, p, exp, "", "--"); break;
	case EXP_POS: puna(d, p, exp, "+", ""); break;
	case EXP_NEG: puna(d, p, exp, "-", ""); break;
	case EXP_BITNOT: puna(d, p, exp, "~", ""); break;
	case EXP_LOGNOT: puna(d, p, exp, "!", ""); break;

	case EXP_LOGOR: pbin(d, p, exp, "||"); break;
	case EXP_LOGAND: pbin(d, p, exp, "&&"); break;
	case EXP_BITOR: pbin(d, p, exp, "|"); break;
	case EXP_BITXOR: pbin(d, p, exp, "^"); break;
	case EXP_BITAND: pbin(d, p, exp, "&"); break;
	case EXP_EQ: pbin(d, p, exp, "=="); break;
	case EXP_NE: pbin(d, p, exp, "!="); break;
	case EXP_STRICTEQ: pbin(d, p, exp, "==="); break;
	case EXP_STRICTNE: pbin(d, p, exp, "!=="); break;
	case EXP_LT: pbin(d, p, exp, "<"); break;
	case EXP_GT: pbin(d, p, exp, ">"); break;
	case EXP_LE: pbin(d, p, exp, "<="); break;
	case EXP_GE: pbin(d, p, exp, ">="); break;
	case EXP_IN: pbin(d, p, exp, "in"); break;
	case EXP_SHL: pbin(d, p, exp, "<<"); break;
	case EXP_SHR: pbin(d, p, exp, ">>"); break;
	case EXP_USHR: pbin(d, p, exp, ">>>"); break;
	case EXP_ADD: pbin(d, p, exp, "+"); break;
	case EXP_SUB: pbin(d, p, exp, "-"); break;
	case EXP_MUL: pbin(d, p, exp, "*"); break;
	case EXP_DIV: pbin(d, p, exp, "/"); break;
	case EXP_MOD: pbin(d, p, exp, "%"); break;
	case EXP_ASS: pbin(d, p, exp, "="); break;
	case EXP_ASS_MUL: pbin(d, p, exp, "*="); break;
	case EXP_ASS_DIV: pbin(d, p, exp, "/="); break;
	case EXP_ASS_MOD: pbin(d, p, exp, "%="); break;
	case EXP_ASS_ADD: pbin(d, p, exp, "+="); break;
	case EXP_ASS_SUB: pbin(d, p, exp, "-="); break;
	case EXP_ASS_SHL: pbin(d, p, exp, "<<="); break;
	case EXP_ASS_SHR: pbin(d, p, exp, ">>="); break;
	case EXP_ASS_USHR: pbin(d, p, exp, ">>>="); break;
	case EXP_ASS_BITAND: pbin(d, p, exp, "&="); break;
	case EXP_ASS_BITXOR: pbin(d, p, exp, "^="); break;
	case EXP_ASS_BITOR: pbin(d, p, exp, "|="); break;

	case EXP_INSTANCEOF:
		pexpi(d, p, exp->a);
		ps(" instanceof ");
		pexpi(d, p, exp->b);
		break;

	case EXP_COMMA:
		pexpi(d, p, exp->a);
		pc(','); sp();
		pexpi(d, p, exp->b);
		break;

	case EXP_COND:
		pexpi(d, p, exp->a);
		sp(); pc('?'); sp();
		pexpi(d, p, exp->b);
		sp(); pc(':'); sp();
		pexpi(d, p, exp->c);
		break;

	case EXP_INDEX:
		pexpi(d, p, exp->a);
		pc('[');
		pexpi(d, 0, exp->b);
		pc(']');
		break;

	case EXP_MEMBER:
		pexpi(d, p, exp->a);
		pc('.');
		pexpi(d, 0, exp->b);
		break;

	case EXP_CALL:
		pexpi(d, p, exp->a);
		pc('(');
		pargs(d, exp->b);
		pc(')');
		break;

	case EXP_NEW:
		ps("new ");
		pexpi(d, p, exp->a);
		pc('(');
		pargs(d, exp->b);
		pc(')');
		break;

	case EXP_FUN:
		if (p == 0) pc('(');
		ps("function ");
		pexpi(d, 0, exp->a);
		pc('(');
		pargs(d, exp->b);
		pc(')'); sp(); pc('{'); nl();
		pstmlist(d, exp->c);
		in(d); pc('}');
		if (p == 0) pc(')');
		break;

	default:
		ps("<UNKNOWN>");
		break;
	}

	if (paren) pc(')');
}

static void pexp(int d, js_Ast *exp)
{
	pexpi(d, 0, exp);
}

static void pvar(int d, js_Ast *var)
{
	assert(var->type == EXP_VAR);
	pexp(d, var->a);
	if (var->b) {
		sp(); pc('='); sp();
		pexp(d, var->b);
	}
}

static void pvarlist(int d, js_Ast *list)
{
	while (list) {
		assert(list->type == AST_LIST);
		pvar(d, list->a);
		list = list->b;
		if (list)
			comma();
	}
}

static void pblock(int d, js_Ast *block)
{
	assert(block->type == STM_BLOCK);
	pc('{'); nl();
	pstmlist(d, block->a);
	in(d); pc('}');
}

static void pstmh(int d, js_Ast *stm)
{
	if (stm->type == STM_BLOCK) {
		sp();
		pblock(d, stm);
	} else {
		nl();
		pstm(d+1, stm);
	}
}

static void pcaselist(int d, js_Ast *list)
{
	while (list) {
		js_Ast *stm = list->a;
		if (stm->type == STM_CASE) {
			in(d); ps("case "); pexp(d, stm->a); pc(':'); nl();
			pstmlist(d, stm->b);
		}
		if (stm->type == STM_DEFAULT) {
			in(d); ps("default:"); nl();
			pstmlist(d, stm->a);
		}
		list = list->b;
	}
}

static void pstm(int d, js_Ast *stm)
{
	if (stm->type == STM_BLOCK) {
		pblock(d, stm);
		return;
	}

	in(d);

	switch (stm->type) {
	case AST_FUNDEC:
		ps("function ");
		pexp(d, stm->a);
		pc('(');
		pargs(d, stm->b);
		pc(')'); sp(); pc('{'); nl();
		pstmlist(d, stm->c);
		in(d); pc('}');
		break;

	case STM_EMPTY:
		pc(';');
		break;

	case STM_VAR:
		ps("var ");
		pvarlist(d, stm->a);
		pc(';');
		break;

	case STM_IF:
		ps("if"); sp(); pc('('); pexp(d, stm->a); pc(')');
		pstmh(d, stm->b);
		if (stm->c) {
			nl(); in(d); ps("else");
			pstmh(d, stm->c);
		}
		break;

	case STM_DO:
		ps("do");
		pstmh(d, stm->a);
		nl();
		in(d); ps("while"); sp(); pc('('); pexp(d, stm->b); pc(')'); pc(';');
		break;

	case STM_WHILE:
		ps("while"); sp(); pc('('); pexp(d, stm->a); pc(')');
		pstmh(d, stm->b);
		break;

	case STM_FOR:
		ps("for"); sp(); pc('(');
		pexp(d, stm->a); pc(';'); sp();
		pexp(d, stm->b); pc(';'); sp();
		pexp(d, stm->c); pc(')');
		pstmh(d, stm->d);
		break;
	case STM_FOR_VAR:
		ps("for"); sp(); ps("(var ");
		pvarlist(d, stm->a); pc(';'); sp();
		pexp(d, stm->b); pc(';'); sp();
		pexp(d, stm->c); pc(')');
		pstmh(d, stm->d);
		break;
	case STM_FOR_IN:
		ps("for"); sp(); pc('(');
		pexp(d, stm->a); ps(" in ");
		pexp(d, stm->b); pc(')');
		pstmh(d, stm->c);
		break;
	case STM_FOR_IN_VAR:
		ps("for"); sp(); ps("(var ");
		pvarlist(d, stm->a); ps(" in ");
		pexp(d, stm->b); pc(')');
		pstmh(d, stm->c);
		break;

	case STM_CONTINUE:
		ps("continue");
		if (stm->a) {
			pc(' '); pexp(d, stm->a);
		}
		pc(';');
		break;

	case STM_BREAK:
		ps("break");
		if (stm->a) {
			pc(' '); pexp(d, stm->a);
		}
		pc(';');
		break;

	case STM_RETURN:
		ps("return");
		if (stm->a) {
			pc(' '); pexp(d, stm->a);
		}
		pc(';');
		break;

	case STM_WITH:
		ps("with"); sp(); pc('('); pexp(d, stm->a); pc(')');
		pstmh(d, stm->b);
		break;

	case STM_SWITCH:
		ps("switch"); sp(); pc('(');
		pexp(d, stm->a);
		pc(')'); sp(); pc('{'); nl();
		pcaselist(d, stm->b);
		in(d); pc('}');
		break;

	case STM_THROW:
		ps("throw "); pexp(d, stm->a); pc(';');
		break;

	case STM_TRY:
		ps("try");
		if (minify && stm->a->type != STM_BLOCK)
			pc(' ');
		pstmh(d, stm->a);
		if (stm->b && stm->c) {
			nl(); in(d); ps("catch"); sp(); pc('('); pexp(d, stm->b); pc(')');
			pstmh(d, stm->c);
		}
		if (stm->d) {
			nl(); in(d); ps("finally");
			pstmh(d, stm->d);
		}
		break;

	case STM_LABEL:
		pexp(d, stm->a); pc(':'); sp(); pstm(d, stm->b);
		break;

	case STM_DEBUGGER:
		ps("debugger");
		pc(';');
		break;

	default:
		pexp(d, stm);
		pc(';');
	}
}

static void pstmlist(int d, js_Ast *list)
{
	while (list) {
		assert(list->type == AST_LIST);
		pstm(d+1, list->a);
		nl();
		list = list->b;
	}
}

void jsP_dumpsyntax(js_State *J, js_Ast *prog, int dominify)
{
	minify = dominify;
	if (prog->type == AST_LIST)
		pstmlist(-1, prog);
	else {
		pstm(0, prog);
		nl();
	}
	if (minify > 1)
		capp_putchar('\n');
}

/* S-expression list representation */

static void snode(int d, js_Ast *node)
{
	void (*afun)(int,js_Ast*) = snode;
	void (*bfun)(int,js_Ast*) = snode;
	void (*cfun)(int,js_Ast*) = snode;
	void (*dfun)(int,js_Ast*) = snode;

	if (!node) {
		return;
	}

	if (node->type == AST_LIST) {
		slist(d, node);
		return;
	}

	pc('(');
	ps(astname[node->type]);
	pc(':');
	pn(node->line);
	switch (node->type) {
	default: break;
	case AST_IDENTIFIER: pc(' '); ps(node->string); break;
	case EXP_IDENTIFIER: pc(' '); ps(node->string); break;
	case EXP_STRING: pc(' '); pstr(node->string); break;
	case EXP_REGEXP: pc(' '); pregexp(node->string, node->number); break;
	case EXP_NUMBER: mrc_printf(" %.9g", node->number); break;
	case STM_BLOCK: afun = sblock; break;
	case AST_FUNDEC: case EXP_FUN: cfun = sblock; break;
	case EXP_PROP_GET: cfun = sblock; break;
	case EXP_PROP_SET: cfun = sblock; break;
	case STM_SWITCH: bfun = sblock; break;
	case STM_CASE: bfun = sblock; break;
	case STM_DEFAULT: afun = sblock; break;
	}
	if (node->a) { pc(' '); afun(d, node->a); }
	if (node->b) { pc(' '); bfun(d, node->b); }
	if (node->c) { pc(' '); cfun(d, node->c); }
	if (node->d) { pc(' '); dfun(d, node->d); }
	pc(')');
}

static void slist(int d, js_Ast *list)
{
	pc('[');
	while (list) {
		assert(list->type == AST_LIST);
		snode(d, list->a);
		list = list->b;
		if (list)
			pc(' ');
	}
	pc(']');
}

static void sblock(int d, js_Ast *list)
{
	ps("[\n");
	in(d+1);
	while (list) {
		assert(list->type == AST_LIST);
		snode(d+1, list->a);
		list = list->b;
		if (list) {
			nl();
			in(d+1);
		}
	}
	nl(); in(d); pc(']');
}

void jsP_dumplist(js_State *J, js_Ast *prog)
{
	minify = 0;
	if (prog->type == AST_LIST)
		sblock(0, prog);
	else
		snode(0, prog);
	nl();
}

/* Compiled code */

void jsC_dumpfunction(js_State *J, js_Function *F)
{
	js_Instruction *p = F->code;
	js_Instruction *end = F->code + F->codelen;
	int i;

	minify = 0;

	mrc_printf("%s(%d)\n", F->name, F->numparams);
	if (F->lightweight) mrc_printf("\tlightweight\n");
	if (F->arguments) mrc_printf("\targuments\n");
	mrc_printf("\tsource %s:%d\n", F->filename, F->line);
	for (i = 0; i < F->funlen; ++i)
		mrc_printf("\tfunction %d %s\n", i, F->funtab[i]->name);
	for (i = 0; i < F->varlen; ++i)
		mrc_printf("\tlocal %d %s\n", i + 1, F->vartab[i]);

	mrc_printf("{\n");
	while (p < end) {
		int ln = *p++;
		int c = *p++;

		mrc_printf("%5d(%3d): ", (int)(p - F->code) - 2, ln);
		ps(opname[c]);

		switch (c) {
		case OP_INTEGER:
			mrc_printf(" %ld", (long)((*p++) - 32768));
			break;
		case OP_NUMBER:
			mrc_printf(" %.9g", F->numtab[*p++]);
			break;
		case OP_STRING:
			pc(' ');
			pstr(F->strtab[*p++]);
			break;
		case OP_NEWREGEXP:
			pc(' ');
			pregexp(F->strtab[p[0]], p[1]);
			p += 2;
			break;

		case OP_GETVAR:
		case OP_HASVAR:
		case OP_SETVAR:
		case OP_DELVAR:
		case OP_GETPROP_S:
		case OP_SETPROP_S:
		case OP_DELPROP_S:
		case OP_CATCH:
			pc(' ');
			ps(F->strtab[*p++]);
			break;

		case OP_GETLOCAL:
		case OP_SETLOCAL:
		case OP_DELLOCAL:
			mrc_printf(" %s", F->vartab[*p++ - 1]);
			break;

		case OP_CLOSURE:
		case OP_CALL:
		case OP_NEW:
		case OP_JUMP:
		case OP_JTRUE:
		case OP_JFALSE:
		case OP_JCASE:
		case OP_TRY:
			mrc_printf(" %ld", (long)*p++);
			break;
		}

		nl();
	}
	mrc_printf("}\n");

	for (i = 0; i < F->funlen; ++i) {
		if (F->funtab[i] != F) {
			mrc_printf("function %d ", i);
			jsC_dumpfunction(J, F->funtab[i]);
		}
	}
}

/* Runtime values */

void js_dumpvalue(js_State *J, js_Value v)
{
	minify = 0;
	switch (v.type) {
	case JS_TUNDEFINED: mrc_printf("undefined"); break;
	case JS_TNULL: mrc_printf("null"); break;
	case JS_TBOOLEAN: mrc_printf(v.u.boolean ? "true" : "false"); break;
	case JS_TNUMBER: mrc_printf("%.9g", v.u.number); break;
	case JS_TSHRSTR: mrc_printf("'%s'", v.u.shrstr); break;
	case JS_TLITSTR: mrc_printf("'%s'", v.u.litstr); break;
	case JS_TMEMSTR: mrc_printf("'%s'", v.u.memstr->p); break;
	case JS_TOBJECT:
		if (v.u.object == J->G) {
			mrc_printf("[Global]");
			break;
		}
		switch (v.u.object->type) {
		case JS_COBJECT: mrc_printf("[Object %p]", (void*)v.u.object); break;
		case JS_CARRAY: mrc_printf("[Array %p]", (void*)v.u.object); break;
		case JS_CFUNCTION:
			mrc_printf("[Function %p, %s, %s:%d]",
				(void*)v.u.object,
				v.u.object->u.f.function->name,
				v.u.object->u.f.function->filename,
				v.u.object->u.f.function->line);
			break;
		case JS_CSCRIPT: mrc_printf("[Script %s]", v.u.object->u.f.function->filename); break;
		case JS_CEVAL: mrc_printf("[Eval %s]", v.u.object->u.f.function->filename); break;
		case JS_CCFUNCTION: mrc_printf("[CFunction %s]", v.u.object->u.c.name); break;
		case JS_CBOOLEAN: mrc_printf("[Boolean %d]", v.u.object->u.boolean); break;
		case JS_CNUMBER: mrc_printf("[Number %g]", v.u.object->u.number); break;
		case JS_CSTRING: mrc_printf("[String'%s']", v.u.object->u.s.string); break;
		case JS_CERROR: mrc_printf("[Error]"); break;
		case JS_CARGUMENTS: mrc_printf("[Arguments %p]", (void*)v.u.object); break;
		case JS_CITERATOR: mrc_printf("[Iterator %p]", (void*)v.u.object); break;
		case JS_CUSERDATA:
			mrc_printf("[Userdata %s %p]", v.u.object->u.user.tag, v.u.object->u.user.data);
			break;
		default: mrc_printf("[Object %p]", (void*)v.u.object); break;
		}
		break;
	}
}

static void js_dumpproperty(js_State *J, js_Property *node)
{
	minify = 0;
	if (node->left->level)
		js_dumpproperty(J, node->left);
	mrc_printf("\t%s: ", node->name);
	js_dumpvalue(J, node->value);
	mrc_printf(",\n");
	if (node->right->level)
		js_dumpproperty(J, node->right);
}

void js_dumpobject(js_State *J, js_Object *obj)
{
	minify = 0;
	mrc_printf("{\n");
	if (obj->properties->level)
		js_dumpproperty(J, obj->properties);
	mrc_printf("}\n");
}

void jsP_initnames(void)
{
	/* 初始化astname数组 */
	strcpy(astname[0], "list");
	strcpy(astname[1], "fundec");
	strcpy(astname[2], "identifier");
	strcpy(astname[3], "exp_identifier");
	strcpy(astname[4], "exp_number");
	strcpy(astname[5], "exp_string");
	strcpy(astname[6], "exp_regexp");
	strcpy(astname[7], "exp_undef");
	strcpy(astname[8], "exp_null");
	strcpy(astname[9], "exp_true");
	strcpy(astname[10], "exp_false");
	strcpy(astname[11], "exp_this");
	strcpy(astname[12], "exp_array");
	strcpy(astname[13], "exp_object");
	strcpy(astname[14], "exp_prop_val");
	strcpy(astname[15], "exp_prop_get");
	strcpy(astname[16], "exp_prop_set");
	strcpy(astname[17], "exp_fun");
	strcpy(astname[18], "exp_index");
	strcpy(astname[19], "exp_member");
	strcpy(astname[20], "exp_call");
	strcpy(astname[21], "exp_new");
	strcpy(astname[22], "exp_postinc");
	strcpy(astname[23], "exp_postdec");
	strcpy(astname[24], "exp_delete");
	strcpy(astname[25], "exp_void");
	strcpy(astname[26], "exp_typeof");
	strcpy(astname[27], "exp_preinc");
	strcpy(astname[28], "exp_predec");
	strcpy(astname[29], "exp_pos");
	strcpy(astname[30], "exp_neg");
	strcpy(astname[31], "exp_bitnot");
	strcpy(astname[32], "exp_lognot");
	strcpy(astname[33], "exp_mod");
	strcpy(astname[34], "exp_div");
	strcpy(astname[35], "exp_mul");
	strcpy(astname[36], "exp_sub");
	strcpy(astname[37], "exp_add");
	strcpy(astname[38], "exp_ushr");
	strcpy(astname[39], "exp_shr");
	strcpy(astname[40], "exp_shl");
	strcpy(astname[41], "exp_in");
	strcpy(astname[42], "exp_instanceof");
	strcpy(astname[43], "exp_ge");
	strcpy(astname[44], "exp_le");
	strcpy(astname[45], "exp_gt");
	strcpy(astname[46], "exp_lt");
	strcpy(astname[47], "exp_strictne");
	strcpy(astname[48], "exp_stricteq");
	strcpy(astname[49], "exp_ne");
	strcpy(astname[50], "exp_eq");
	strcpy(astname[51], "exp_bitand");
	strcpy(astname[52], "exp_bitxor");
	strcpy(astname[53], "exp_bitor");
	strcpy(astname[54], "exp_logand");
	strcpy(astname[55], "exp_logor");
	strcpy(astname[56], "exp_cond");
	strcpy(astname[57], "exp_ass");
	strcpy(astname[58], "exp_ass_mul");
	strcpy(astname[59], "exp_ass_div");
	strcpy(astname[60], "exp_ass_mod");
	strcpy(astname[61], "exp_ass_add");
	strcpy(astname[62], "exp_ass_sub");
	strcpy(astname[63], "exp_ass_shl");
	strcpy(astname[64], "exp_ass_shr");
	strcpy(astname[65], "exp_ass_ushr");
	strcpy(astname[66], "exp_ass_bitand");
	strcpy(astname[67], "exp_ass_bitxor");
	strcpy(astname[68], "exp_ass_bitor");
	strcpy(astname[69], "exp_comma");
	strcpy(astname[70], "exp_var");
	strcpy(astname[71], "stm_block");
	strcpy(astname[72], "stm_empty");
	strcpy(astname[73], "stm_var");
	strcpy(astname[74], "stm_if");
	strcpy(astname[75], "stm_do");
	strcpy(astname[76], "stm_while");
	strcpy(astname[77], "stm_for");
	strcpy(astname[78], "stm_for_var");
	strcpy(astname[79], "stm_for_in");
	strcpy(astname[80], "stm_for_in_var");
	strcpy(astname[81], "stm_continue");
	strcpy(astname[82], "stm_break");
	strcpy(astname[83], "stm_return");
	strcpy(astname[84], "stm_with");
	strcpy(astname[85], "stm_switch");
	strcpy(astname[86], "stm_throw");
	strcpy(astname[87], "stm_try");
	strcpy(astname[88], "stm_debugger");
	strcpy(astname[89], "stm_label");
	strcpy(astname[90], "stm_case");
	strcpy(astname[91], "stm_default");
	strcpy(astname[92], "");
	strcpy(astname[93], "");

	/* 初始化opname数组 */
	strcpy(opname[0], "pop");
	strcpy(opname[1], "dup");
	strcpy(opname[2], "dup2");
	strcpy(opname[3], "rot2");
	strcpy(opname[4], "rot3");
	strcpy(opname[5], "rot4");
	strcpy(opname[6], "integer");
	strcpy(opname[7], "number");
	strcpy(opname[8], "string");
	strcpy(opname[9], "closure");
	strcpy(opname[10], "newarray");
	strcpy(opname[11], "newobject");
	strcpy(opname[12], "newregexp");
	strcpy(opname[13], "undef");
	strcpy(opname[14], "null");
	strcpy(opname[15], "true");
	strcpy(opname[16], "false");
	strcpy(opname[17], "this");
	strcpy(opname[18], "current");
	strcpy(opname[19], "getlocal");
	strcpy(opname[20], "setlocal");
	strcpy(opname[21], "dellocal");
	strcpy(opname[22], "hasvar");
	strcpy(opname[23], "getvar");
	strcpy(opname[24], "setvar");
	strcpy(opname[25], "delvar");
	strcpy(opname[26], "in");
	strcpy(opname[27], "initprop");
	strcpy(opname[28], "initgetter");
	strcpy(opname[29], "initsetter");
	strcpy(opname[30], "getprop");
	strcpy(opname[31], "getprop_s");
	strcpy(opname[32], "setprop");
	strcpy(opname[33], "setprop_s");
	strcpy(opname[34], "delprop");
	strcpy(opname[35], "delprop_s");
	strcpy(opname[36], "iterator");
	strcpy(opname[37], "nextiter");
	strcpy(opname[38], "eval");
	strcpy(opname[39], "call");
	strcpy(opname[40], "new");
	strcpy(opname[41], "typeof");
	strcpy(opname[42], "pos");
	strcpy(opname[43], "neg");
	strcpy(opname[44], "bitnot");
	strcpy(opname[45], "lognot");
	strcpy(opname[46], "inc");
	strcpy(opname[47], "dec");
	strcpy(opname[48], "postinc");
	strcpy(opname[49], "postdec");
	strcpy(opname[50], "mul");
	strcpy(opname[51], "div");
	strcpy(opname[52], "mod");
	strcpy(opname[53], "add");
	strcpy(opname[54], "sub");
	strcpy(opname[55], "shl");
	strcpy(opname[56], "shr");
	strcpy(opname[57], "ushr");
	strcpy(opname[58], "lt");
	strcpy(opname[59], "gt");
	strcpy(opname[60], "le");
	strcpy(opname[61], "ge");
	strcpy(opname[62], "eq");
	strcpy(opname[63], "ne");
	strcpy(opname[64], "stricteq");
	strcpy(opname[65], "strictne");
	strcpy(opname[66], "jcase");
	strcpy(opname[67], "bitand");
	strcpy(opname[68], "bitxor");
	strcpy(opname[69], "bitor");
	strcpy(opname[70], "instanceof");
	strcpy(opname[71], "throw");
	strcpy(opname[72], "try");
	strcpy(opname[73], "endtry");
	strcpy(opname[74], "catch");
	strcpy(opname[75], "endcatch");
	strcpy(opname[76], "with");
	strcpy(opname[77], "endwith");
	strcpy(opname[78], "debugger");
	strcpy(opname[79], "jump");
	strcpy(opname[80], "jtrue");
	strcpy(opname[81], "jfalse");
	strcpy(opname[82], "return");
	strcpy(opname[83], "");
}