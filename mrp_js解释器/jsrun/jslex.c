#include "jsi.h"
#include "jslex.h"
#include "utf.h"
#include <mrc_base.h>

JS_NORETURN static void jsY_error(js_State *J, const char *fmt, ...) JS_PRINTFLIKE(2,3);

static void jsY_error(js_State *J, const char *fmt, ...)
{
	va_list ap;
	char buf[512];
	char msgbuf[256];

	va_start(ap, fmt);
	vsnprintf(msgbuf, 256, fmt, ap);
	va_end(ap);

	snprintf(buf, 256, "%s:%d: ", J->filename, J->lexline);
	strcat(buf, msgbuf);

	js_newsyntaxerror(J, buf);
	js_throw(J);
}

static const char *tokenstring[320]; /* 运行时初始化，足够大以容纳所有token */
#define TOKENSTRING_LEN 320

void jsY_inittokenstring(void) {
	int i;
	tokenstring[0] = "(end-of-file)";
	tokenstring[1] = "'\\x01'"; tokenstring[2] = "'\\x02'"; tokenstring[3] = "'\\x03'"; tokenstring[4] = "'\\x04'";
	tokenstring[5] = "'\\x05'"; tokenstring[6] = "'\\x06'"; tokenstring[7] = "'\\x07'"; tokenstring[8] = "'\\x08'";
	tokenstring[9] = "'\\x09'"; tokenstring[10] = "'\\x0A'"; tokenstring[11] = "'\\x0B'"; tokenstring[12] = "'\\x0C'";
	tokenstring[13] = "'\\x0D'"; tokenstring[14] = "'\\x0E'"; tokenstring[15] = "'\\x0F'"; tokenstring[16] = "'\\x10'";
	tokenstring[17] = "'\\x11'"; tokenstring[18] = "'\\x12'"; tokenstring[19] = "'\\x13'"; tokenstring[20] = "'\\x14'";
	tokenstring[21] = "'\\x15'"; tokenstring[22] = "'\\x16'"; tokenstring[23] = "'\\x17'"; tokenstring[24] = "'\\x18'";
	tokenstring[25] = "'\\x19'"; tokenstring[26] = "'\\x1A'"; tokenstring[27] = "'\\x1B'"; tokenstring[28] = "'\\x1C'";
	tokenstring[29] = "'\\x1D'"; tokenstring[30] = "'\\x1E'"; tokenstring[31] = "'\\x1F'"; tokenstring[32] = "' '";
	tokenstring[33] = "'!'"; tokenstring[34] = "'\"'"; tokenstring[35] = "'#'"; tokenstring[36] = "'$'";
	tokenstring[37] = "'%'"; tokenstring[38] = "'&'"; tokenstring[39] = "'\\''"; tokenstring[40] = "'('";
	tokenstring[41] = "')'"; tokenstring[42] = "'*'"; tokenstring[43] = "'+'"; tokenstring[44] = "','";
	tokenstring[45] = "'-'"; tokenstring[46] = "'.'"; tokenstring[47] = "'/'"; tokenstring[48] = "'0'";
	tokenstring[49] = "'1'"; tokenstring[50] = "'2'"; tokenstring[51] = "'3'"; tokenstring[52] = "'4'";
	tokenstring[53] = "'5'"; tokenstring[54] = "'6'"; tokenstring[55] = "'7'"; tokenstring[56] = "'8'";
	tokenstring[57] = "'9'"; tokenstring[58] = "':'"; tokenstring[59] = "';'"; tokenstring[60] = "'<'";
	tokenstring[61] = "'='"; tokenstring[62] = "'>'"; tokenstring[63] = "'?'"; tokenstring[64] = "'@'";
	tokenstring[65] = "'A'"; tokenstring[66] = "'B'"; tokenstring[67] = "'C'"; tokenstring[68] = "'D'";
	tokenstring[69] = "'E'"; tokenstring[70] = "'F'"; tokenstring[71] = "'G'"; tokenstring[72] = "'H'";
	tokenstring[73] = "'I'"; tokenstring[74] = "'J'"; tokenstring[75] = "'K'"; tokenstring[76] = "'L'";
	tokenstring[77] = "'M'"; tokenstring[78] = "'N'"; tokenstring[79] = "'O'"; tokenstring[80] = "'P'";
	tokenstring[81] = "'Q'"; tokenstring[82] = "'R'"; tokenstring[83] = "'S'"; tokenstring[84] = "'T'";
	tokenstring[85] = "'U'"; tokenstring[86] = "'V'"; tokenstring[87] = "'W'"; tokenstring[88] = "'X'";
	tokenstring[89] = "'Y'"; tokenstring[90] = "'Z'"; tokenstring[91] = "'['"; tokenstring[92] = "'\\'";
	tokenstring[93] = "']'"; tokenstring[94] = "'^'"; tokenstring[95] = "'_'"; tokenstring[96] = "'`'";
	tokenstring[97] = "'a'"; tokenstring[98] = "'b'"; tokenstring[99] = "'c'"; tokenstring[100] = "'d'";
	tokenstring[101] = "'e'"; tokenstring[102] = "'f'"; tokenstring[103] = "'g'"; tokenstring[104] = "'h'";
	tokenstring[105] = "'i'"; tokenstring[106] = "'j'"; tokenstring[107] = "'k'"; tokenstring[108] = "'l'";
	tokenstring[109] = "'m'"; tokenstring[110] = "'n'"; tokenstring[111] = "'o'"; tokenstring[112] = "'p'";
	tokenstring[113] = "'q'"; tokenstring[114] = "'r'"; tokenstring[115] = "'s'"; tokenstring[116] = "'t'";
	tokenstring[117] = "'u'"; tokenstring[118] = "'v'"; tokenstring[119] = "'w'"; tokenstring[120] = "'x'";
	tokenstring[121] = "'y'"; tokenstring[122] = "'z'"; tokenstring[123] = "'{'"; tokenstring[124] = "'|'";
	tokenstring[125] = "'}'"; tokenstring[126] = "'~'"; tokenstring[127] = "'\\x7F'";
	for (i = 128; i < 256; i++) tokenstring[i] = 0; /* 128-255 are NULL */
	/* TK_IDENTIFIER=256 开始 */
	tokenstring[256] = "(identifier)"; tokenstring[257] = "(number)";
	tokenstring[258] = "(string)"; tokenstring[259] = "(regexp)";
	/* operators: TK_LE=260 开始 */
	tokenstring[260] = "'<='"; tokenstring[261] = "'>='"; tokenstring[262] = "'=='";
	tokenstring[263] = "'!='"; tokenstring[264] = "'==='"; tokenstring[265] = "'!=='";
	tokenstring[266] = "'<<'"; tokenstring[267] = "'>>'"; tokenstring[268] = "'>>>'";
	tokenstring[269] = "'&&'"; tokenstring[270] = "'||'"; tokenstring[271] = "'+='";
	tokenstring[272] = "'-='"; tokenstring[273] = "'*='"; tokenstring[274] = "'/='";
	tokenstring[275] = "'%='"; tokenstring[276] = "'<<='"; tokenstring[277] = "'>>='";
	tokenstring[278] = "'>>>='"; tokenstring[279] = "'&='"; tokenstring[280] = "'|='";
	tokenstring[281] = "'^='"; tokenstring[282] = "'++'"; tokenstring[283] = "'--'";
	/* keywords: TK_BREAK=284 开始 */
	tokenstring[284] = "'break'"; tokenstring[285] = "'case'"; tokenstring[286] = "'catch'";
	tokenstring[287] = "'continue'"; tokenstring[288] = "'debugger'"; tokenstring[289] = "'default'";
	tokenstring[290] = "'delete'"; tokenstring[291] = "'do'"; tokenstring[292] = "'else'";
	tokenstring[293] = "'false'"; tokenstring[294] = "'finally'"; tokenstring[295] = "'for'";
	tokenstring[296] = "'function'"; tokenstring[297] = "'if'"; tokenstring[298] = "'in'";
	tokenstring[299] = "'instanceof'"; tokenstring[300] = "'new'"; tokenstring[301] = "'null'";
	tokenstring[302] = "'return'"; tokenstring[303] = "'switch'"; tokenstring[304] = "'this'";
	tokenstring[305] = "'throw'"; tokenstring[306] = "'true'"; tokenstring[307] = "'try'";
	tokenstring[308] = "'typeof'"; tokenstring[309] = "'var'"; tokenstring[310] = "'void'";
	tokenstring[311] = "'while'"; tokenstring[312] = "'with'";
}

const char *jsY_tokenstring(int token)
{
	if (token >= 0 && token < (int)TOKENSTRING_LEN)
		if (tokenstring[token])
			return tokenstring[token];
	return "<unknown>";
}

char keywords_storage[29][12]; /* 存储关键字字符串 */
const char *keywords[29]; /* 指向keywords_storage的指针数组 */
#define KEYWORDS_LEN 29

int jsY_findword(const char *s, const char **list, int num)
{
	int m, c;
	int l = 0;
	int r = num - 1;
	while (l <= r) {
		m = (l + r) >> 1;
		c = strcmp(s, list[m]);
		mrc_printf("strcmp %s, %s", s, list[m]);
		if (c < 0)
			r = m - 1;
		else if (c > 0)
			l = m + 1;
		else
			return m;
	}
	return -1;
}

static int jsY_findkeyword(js_State *J, const char *s)
{
	int i = jsY_findword(s, keywords, KEYWORDS_LEN);
	if (i >= 0) {
		J->text = keywords[i];
		return TK_BREAK + i; /* first keyword + i */
	}
	mrc_printf("jsY_findkeyword: not found, identifier '%s'\n", s);
	J->text = js_intern(J, s);
	return TK_IDENTIFIER;
}

int jsY_iswhite(int c)
{
	return c == 0x9 || c == 0xB || c == 0xC || c == 0x20 || c == 0xA0 || c == 0xFEFF;
}

int jsY_isnewline(int c)
{
	return c == 0xA || c == 0xD || c == 0x2028 || c == 0x2029;
}

#ifndef isalpha
#define isalpha(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#endif
#ifndef isdigit
#define isdigit(c) (c >= '0' && c <= '9')
#endif
#ifndef ishex
#define ishex(c) ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
#endif

static int jsY_isidentifierstart(int c)
{
	return isalpha(c) || c == '$' || c == '_' || isalpharune(c);
}

static int jsY_isidentifierpart(int c)
{
	return isdigit(c) || isalpha(c) || c == '$' || c == '_' || isalpharune(c);
}

static int jsY_isdec(int c)
{
	return isdigit(c);
}

int jsY_ishex(int c)
{
	return isdigit(c) || ishex(c);
}

int jsY_tohex(int c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 0xA;
	if (c >= 'A' && c <= 'F') return c - 'A' + 0xA;
	return 0;
}

static void jsY_next(js_State *J)
{
	Rune c;
	if (*J->source == 0) {
		J->lexchar = EOF;
		return;
	}
	J->source += chartorune(&c, J->source);
	/* consume CR LF as one unit */
	if (c == '\r' && *J->source == '\n')
		++J->source;
	if (jsY_isnewline(c)) {
		J->line++;
		c = '\n';
	}
	J->lexchar = c;
}

#define jsY_accept(J, x) (J->lexchar == x ? (jsY_next(J), 1) : 0)

#define jsY_expect(J, x) if (!jsY_accept(J, x)) jsY_error(J, "expected '%c'", x)

static void jsY_unescape(js_State *J)
{
	if (jsY_accept(J, '\\')) {
		if (jsY_accept(J, 'u')) {
			int x = 0;
			if (!jsY_ishex(J->lexchar)) { goto error; } x |= jsY_tohex(J->lexchar) << 12; jsY_next(J);
			if (!jsY_ishex(J->lexchar)) { goto error; } x |= jsY_tohex(J->lexchar) << 8; jsY_next(J);
			if (!jsY_ishex(J->lexchar)) { goto error; } x |= jsY_tohex(J->lexchar) << 4; jsY_next(J);
			if (!jsY_ishex(J->lexchar)) { goto error; } x |= jsY_tohex(J->lexchar);
			J->lexchar = x;
			return;
		}
error:
		jsY_error(J, "unexpected escape sequence");
	}
}

static void textinit(js_State *J)
{
	if (!J->lexbuf.text) {
		J->lexbuf.cap = 4096;
		J->lexbuf.text = js_malloc(J, J->lexbuf.cap);
	}
	J->lexbuf.len = 0;
}

static void textpush(js_State *J, Rune c)
{
	int n;
	if (c == EOF)
		n = 1;
	else
		n = runelen(c);
	if (J->lexbuf.len + n > J->lexbuf.cap) {
		J->lexbuf.cap = J->lexbuf.cap * 2;
		J->lexbuf.text = js_realloc(J, J->lexbuf.text, J->lexbuf.cap);
	}
	if (c == EOF)
		J->lexbuf.text[J->lexbuf.len++] = 0;
	else
		J->lexbuf.len += runetochar(J->lexbuf.text + J->lexbuf.len, &c);
}

static char *textend(js_State *J)
{
	textpush(J, EOF);
	return J->lexbuf.text;
}

static void lexlinecomment(js_State *J)
{
	while (J->lexchar != EOF && J->lexchar != '\n')
		jsY_next(J);
}

static int lexcomment(js_State *J)
{
	/* already consumed initial '/' '*' sequence */
	while (J->lexchar != EOF) {
		if (jsY_accept(J, '*')) {
			while (J->lexchar == '*')
				jsY_next(J);
			if (jsY_accept(J, '/'))
				return 0;
		}
		else
			jsY_next(J);
	}
	return -1;
}

static double lexhex(js_State *J)
{
	double n = 0;
	if (!jsY_ishex(J->lexchar))
		jsY_error(J, "malformed hexadecimal number");
	while (jsY_ishex(J->lexchar)) {
		n = n * 16 + jsY_tohex(J->lexchar);
		jsY_next(J);
	}
	return n;
}

#if 0

static double lexinteger(js_State *J)
{
	double n = 0;
	if (!jsY_isdec(J->lexchar))
		jsY_error(J, "malformed number");
	while (jsY_isdec(J->lexchar)) {
		n = n * 10 + (J->lexchar - '0');
		jsY_next(J);
	}
	return n;
}

static double lexfraction(js_State *J)
{
	double n = 0;
	double d = 1;
	while (jsY_isdec(J->lexchar)) {
		n = n * 10 + (J->lexchar - '0');
		d = d * 10;
		jsY_next(J);
	}
	return n / d;
}

static double lexexponent(js_State *J)
{
	double sign;
	if (jsY_accept(J, 'e') || jsY_accept(J, 'E')) {
		if (jsY_accept(J, '-')) sign = -1;
		else if (jsY_accept(J, '+')) sign = 1;
		else sign = 1;
		return sign * lexinteger(J);
	}
	return 0;
}

static int lexnumber(js_State *J)
{
	double n;
	double e;

	if (jsY_accept(J, '0')) {
		if (jsY_accept(J, 'x') || jsY_accept(J, 'X')) {
			J->number = lexhex(J);
			return TK_NUMBER;
		}
		if (jsY_isdec(J->lexchar))
			jsY_error(J, "number with leading zero");
		n = 0;
		if (jsY_accept(J, '.'))
			n += lexfraction(J);
	} else if (jsY_accept(J, '.')) {
		if (!jsY_isdec(J->lexchar))
			return '.';
		n = lexfraction(J);
	} else {
		n = lexinteger(J);
		if (jsY_accept(J, '.'))
			n += lexfraction(J);
	}

	e = lexexponent(J);
	if (e < 0)
		n /= pow(10, -e);
	else if (e > 0)
		n *= pow(10, e);

	if (jsY_isidentifierstart(J->lexchar))
		jsY_error(J, "number with letter suffix");

	J->number = n;
	return TK_NUMBER;
}

#else

static int lexnumber(js_State *J)
{
	const char *s = J->source - 1;

	if (jsY_accept(J, '0')) {
		if (jsY_accept(J, 'x') || jsY_accept(J, 'X')) {
			J->number = lexhex(J);
			return TK_NUMBER;
		}
		if (jsY_isdec(J->lexchar))
			jsY_error(J, "number with leading zero");
		if (jsY_accept(J, '.')) {
			while (jsY_isdec(J->lexchar))
				jsY_next(J);
		}
	} else if (jsY_accept(J, '.')) {
		if (!jsY_isdec(J->lexchar))
			return '.';
		while (jsY_isdec(J->lexchar))
			jsY_next(J);
	} else {
		while (jsY_isdec(J->lexchar))
			jsY_next(J);
		if (jsY_accept(J, '.')) {
			while (jsY_isdec(J->lexchar))
				jsY_next(J);
		}
	}

	if (jsY_accept(J, 'e') || jsY_accept(J, 'E')) {
		if (J->lexchar == '-' || J->lexchar == '+')
			jsY_next(J);
		if (jsY_isdec(J->lexchar))
			while (jsY_isdec(J->lexchar))
				jsY_next(J);
		else
			jsY_error(J, "missing exponent");
	}

	if (jsY_isidentifierstart(J->lexchar))
		jsY_error(J, "number with letter suffix");

	J->number = js_strtod(s, NULL);
	return TK_NUMBER;
}

#endif

static int lexescape(js_State *J)
{
	int x = 0;

	/* already consumed '\' */

	if (jsY_accept(J, '\n'))
		return 0;

	switch (J->lexchar) {
	case EOF: jsY_error(J, "unterminated escape sequence");
	case 'u':
		jsY_next(J);
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar) << 12; jsY_next(J); }
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar) << 8; jsY_next(J); }
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar) << 4; jsY_next(J); }
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar); jsY_next(J); }
		textpush(J, x);
		break;
	case 'x':
		jsY_next(J);
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar) << 4; jsY_next(J); }
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar); jsY_next(J); }
		textpush(J, x);
		break;
	case '0': textpush(J, 0); jsY_next(J); break;
	case '\\': textpush(J, '\\'); jsY_next(J); break;
	case '\'': textpush(J, '\''); jsY_next(J); break;
	case '"': textpush(J, '"'); jsY_next(J); break;
	case 'b': textpush(J, '\b'); jsY_next(J); break;
	case 'f': textpush(J, '\f'); jsY_next(J); break;
	case 'n': textpush(J, '\n'); jsY_next(J); break;
	case 'r': textpush(J, '\r'); jsY_next(J); break;
	case 't': textpush(J, '\t'); jsY_next(J); break;
	case 'v': textpush(J, '\v'); jsY_next(J); break;
	default: textpush(J, J->lexchar); jsY_next(J); break;
	}
	return 0;
}

static int lexstring(js_State *J)
{
	const char *s;

	int q = J->lexchar;
	jsY_next(J);

	textinit(J);

	while (J->lexchar != q) {
		if (J->lexchar == EOF || J->lexchar == '\n')
			jsY_error(J, "string not terminated");
		if (jsY_accept(J, '\\')) {
			if (lexescape(J))
				jsY_error(J, "malformed escape sequence");
		} else {
			textpush(J, J->lexchar);
			jsY_next(J);
		}
	}
	jsY_expect(J, q);

	s = textend(J);
	mrc_printf("lexstring: '%s'\n", s);
	J->text = js_intern(J, s);
	return TK_STRING;
}

/* the ugliest language wart ever... */
static int isregexpcontext(int last)
{
	switch (last) {
	case ']':
	case ')':
	case '}':
	case TK_IDENTIFIER:
	case TK_NUMBER:
	case TK_STRING:
	case TK_FALSE:
	case TK_NULL:
	case TK_THIS:
	case TK_TRUE:
		return 0;
	default:
		return 1;
	}
}

static int lexregexp(js_State *J)
{
	const char *s;
	int g, m, i;
	int inclass = 0;

	/* already consumed initial '/' */

	textinit(J);

	/* regexp body */
	while (J->lexchar != '/' || inclass) {
		if (J->lexchar == EOF || J->lexchar == '\n') {
			jsY_error(J, "regular expression not terminated");
		} else if (jsY_accept(J, '\\')) {
			if (jsY_accept(J, '/')) {
				textpush(J, '/');
			} else {
				textpush(J, '\\');
				if (J->lexchar == EOF || J->lexchar == '\n')
					jsY_error(J, "regular expression not terminated");
				textpush(J, J->lexchar);
				jsY_next(J);
			}
		} else {
			if (J->lexchar == '[' && !inclass)
				inclass = 1;
			if (J->lexchar == ']' && inclass)
				inclass = 0;
			textpush(J, J->lexchar);
			jsY_next(J);
		}
	}
	jsY_expect(J, '/');

	s = textend(J);

	/* regexp flags */
	g = i = m = 0;

	while (jsY_isidentifierpart(J->lexchar)) {
		if (jsY_accept(J, 'g')) ++g;
		else if (jsY_accept(J, 'i')) ++i;
		else if (jsY_accept(J, 'm')) ++m;
		else jsY_error(J, "illegal flag in regular expression: %c", J->lexchar);
	}

	if (g > 1 || i > 1 || m > 1)
		jsY_error(J, "duplicated flag in regular expression");
	mrc_printf("lexregexp: /%s/ %s%s%s\n", s, g ? "g" : "", i ? "i" : "", m ? "m" : "");
	J->text = js_intern(J, s);
	J->number = 0;
	if (g) J->number += JS_REGEXP_G;
	if (i) J->number += JS_REGEXP_I;
	if (m) J->number += JS_REGEXP_M;
	return TK_REGEXP;
}

/* simple "return [no Line Terminator here] ..." contexts */
static int isnlthcontext(int last)
{
	switch (last) {
	case TK_BREAK:
	case TK_CONTINUE:
	case TK_RETURN:
	case TK_THROW:
		return 1;
	default:
		return 0;
	}
}

static int jsY_lexx(js_State *J)
{
	J->newline = 0;

	while (1) {
		J->lexline = J->line; /* save location of beginning of token */

		while (jsY_iswhite(J->lexchar))
			jsY_next(J);

		if (jsY_accept(J, '\n')) {
			J->newline = 1;
			if (isnlthcontext(J->lasttoken))
				return ';';
			continue;
		}

		if (jsY_accept(J, '/')) {
			if (jsY_accept(J, '/')) {
				lexlinecomment(J);
				continue;
			} else if (jsY_accept(J, '*')) {
				if (lexcomment(J))
					jsY_error(J, "multi-line comment not terminated");
				continue;
			} else if (isregexpcontext(J->lasttoken)) {
				return lexregexp(J);
			} else if (jsY_accept(J, '=')) {
				return TK_DIV_ASS;
			} else {
				return '/';
			}
		}

		if (J->lexchar >= '0' && J->lexchar <= '9') {
			return lexnumber(J);
		}

		switch (J->lexchar) {
		case '(': jsY_next(J); return '(';
		case ')': jsY_next(J); return ')';
		case ',': jsY_next(J); return ',';
		case ':': jsY_next(J); return ':';
		case ';': jsY_next(J); return ';';
		case '?': jsY_next(J); return '?';
		case '[': jsY_next(J); return '[';
		case ']': jsY_next(J); return ']';
		case '{': jsY_next(J); return '{';
		case '}': jsY_next(J); return '}';
		case '~': jsY_next(J); return '~';

		case '\'':
		case '"':
			return lexstring(J);

		case '.':
			return lexnumber(J);

		case '<':
			jsY_next(J);
			if (jsY_accept(J, '<')) {
				if (jsY_accept(J, '='))
					return TK_SHL_ASS;
				return TK_SHL;
			}
			if (jsY_accept(J, '='))
				return TK_LE;
			return '<';

		case '>':
			jsY_next(J);
			if (jsY_accept(J, '>')) {
				if (jsY_accept(J, '>')) {
					if (jsY_accept(J, '='))
						return TK_USHR_ASS;
					return TK_USHR;
				}
				if (jsY_accept(J, '='))
					return TK_SHR_ASS;
				return TK_SHR;
			}
			if (jsY_accept(J, '='))
				return TK_GE;
			return '>';

		case '=':
			jsY_next(J);
			if (jsY_accept(J, '=')) {
				if (jsY_accept(J, '='))
					return TK_STRICTEQ;
				return TK_EQ;
			}
			return '=';

		case '!':
			jsY_next(J);
			if (jsY_accept(J, '=')) {
				if (jsY_accept(J, '='))
					return TK_STRICTNE;
				return TK_NE;
			}
			return '!';

		case '+':
			jsY_next(J);
			if (jsY_accept(J, '+'))
				return TK_INC;
			if (jsY_accept(J, '='))
				return TK_ADD_ASS;
			return '+';

		case '-':
			jsY_next(J);
			if (jsY_accept(J, '-'))
				return TK_DEC;
			if (jsY_accept(J, '='))
				return TK_SUB_ASS;
			return '-';

		case '*':
			jsY_next(J);
			if (jsY_accept(J, '='))
				return TK_MUL_ASS;
			return '*';

		case '%':
			jsY_next(J);
			if (jsY_accept(J, '='))
				return TK_MOD_ASS;
			return '%';

		case '&':
			jsY_next(J);
			if (jsY_accept(J, '&'))
				return TK_AND;
			if (jsY_accept(J, '='))
				return TK_AND_ASS;
			return '&';

		case '|':
			jsY_next(J);
			if (jsY_accept(J, '|'))
				return TK_OR;
			if (jsY_accept(J, '='))
				return TK_OR_ASS;
			return '|';

		case '^':
			jsY_next(J);
			if (jsY_accept(J, '='))
				return TK_XOR_ASS;
			return '^';

		case EOF:
			return 0; /* EOF */
		}

		/* Handle \uXXXX escapes in identifiers */
		jsY_unescape(J);
		if (jsY_isidentifierstart(J->lexchar)) {
			textinit(J);
			textpush(J, J->lexchar);

			jsY_next(J);
			jsY_unescape(J);
			while (jsY_isidentifierpart(J->lexchar)) {
				textpush(J, J->lexchar);
				jsY_next(J);
				jsY_unescape(J);
			}

			textend(J);

			return jsY_findkeyword(J, J->lexbuf.text);
		}

		if (J->lexchar >= 0x20 && J->lexchar <= 0x7E)
			jsY_error(J, "unexpected character: '%c'", J->lexchar);
		jsY_error(J, "unexpected character: \\u%04X", J->lexchar);
	}
}

void jsY_initlex(js_State *J, const char *filename, const char *source)
{
	J->filename = filename;
	J->source = source;
	J->line = 1;
	J->lasttoken = 0;
	jsY_next(J); /* load first lookahead character */
}

int jsY_lex(js_State *J)
{
	return J->lasttoken = jsY_lexx(J);
}

static int lexjsonnumber(js_State *J)
{
	const char *s = J->source - 1;

	if (J->lexchar == '-')
		jsY_next(J);

	if (J->lexchar == '0')
		jsY_next(J);
	else if (J->lexchar >= '1' && J->lexchar <= '9')
		while (isdigit(J->lexchar))
			jsY_next(J);
	else
		jsY_error(J, "unexpected non-digit");

	if (jsY_accept(J, '.')) {
		if (isdigit(J->lexchar))
			while (isdigit(J->lexchar))
				jsY_next(J);
		else
			jsY_error(J, "missing digits after decimal point");
	}

	if (jsY_accept(J, 'e') || jsY_accept(J, 'E')) {
		if (J->lexchar == '-' || J->lexchar == '+')
			jsY_next(J);
		if (isdigit(J->lexchar))
			while (isdigit(J->lexchar))
				jsY_next(J);
		else
			jsY_error(J, "missing digits after exponent indicator");
	}

	J->number = js_strtod(s, NULL);
	return TK_NUMBER;
}

static int lexjsonescape(js_State *J)
{
	int x = 0;

	/* already consumed '\' */

	switch (J->lexchar) {
	default: jsY_error(J, "invalid escape sequence");
	case 'u':
		jsY_next(J);
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar) << 12; jsY_next(J); }
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar) << 8; jsY_next(J); }
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar) << 4; jsY_next(J); }
		if (!jsY_ishex(J->lexchar)) return 1; else { x |= jsY_tohex(J->lexchar); jsY_next(J); }
		textpush(J, x);
		break;
	case '"': textpush(J, '"'); jsY_next(J); break;
	case '\\': textpush(J, '\\'); jsY_next(J); break;
	case '/': textpush(J, '/'); jsY_next(J); break;
	case 'b': textpush(J, '\b'); jsY_next(J); break;
	case 'f': textpush(J, '\f'); jsY_next(J); break;
	case 'n': textpush(J, '\n'); jsY_next(J); break;
	case 'r': textpush(J, '\r'); jsY_next(J); break;
	case 't': textpush(J, '\t'); jsY_next(J); break;
	}
	return 0;
}

static int lexjsonstring(js_State *J)
{
	const char *s;

	textinit(J);

	while (J->lexchar != '"') {
		if (J->lexchar == EOF)
			jsY_error(J, "unterminated string");
		else if (J->lexchar < 32)
			jsY_error(J, "invalid control character in string");
		else if (jsY_accept(J, '\\'))
			lexjsonescape(J);
		else {
			textpush(J, J->lexchar);
			jsY_next(J);
		}
	}
	jsY_expect(J, '"');

	s = textend(J);
	mrc_printf("lexjsonstring: '%s'\n", s);
	J->text = js_intern(J, s);
	return TK_STRING;
}

int jsY_lexjson(js_State *J)
{
	while (1) {
		J->lexline = J->line; /* save location of beginning of token */

		while (jsY_iswhite(J->lexchar) || J->lexchar == '\n')
			jsY_next(J);

		if ((J->lexchar >= '0' && J->lexchar <= '9') || J->lexchar == '-')
			return lexjsonnumber(J);

		switch (J->lexchar) {
		case ',': jsY_next(J); return ',';
		case ':': jsY_next(J); return ':';
		case '[': jsY_next(J); return '[';
		case ']': jsY_next(J); return ']';
		case '{': jsY_next(J); return '{';
		case '}': jsY_next(J); return '}';

		case '"':
			jsY_next(J);
			return lexjsonstring(J);

		case 'f':
			jsY_next(J); jsY_expect(J, 'a'); jsY_expect(J, 'l'); jsY_expect(J, 's'); jsY_expect(J, 'e');
			return TK_FALSE;

		case 'n':
			jsY_next(J); jsY_expect(J, 'u'); jsY_expect(J, 'l'); jsY_expect(J, 'l');
			return TK_NULL;

		case 't':
			jsY_next(J); jsY_expect(J, 'r'); jsY_expect(J, 'u'); jsY_expect(J, 'e');
			return TK_TRUE;

		case EOF:
			return 0; /* EOF */
		}

		if (J->lexchar >= 0x20 && J->lexchar <= 0x7E)
			jsY_error(J, "unexpected character: '%c'", J->lexchar);
		jsY_error(J, "unexpected character: \\u%04X", J->lexchar);
	}
}
