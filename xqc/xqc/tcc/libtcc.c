/*
 *  TCC - Tiny C Compiler
 * 
 *  Copyright (c) 2001-2004 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "tcc.h"

/********************************************************/
/* global variables */

/* use GNU C extensions */
ST_DATA int gnu_ext = 1;

/* use TinyCC extensions */
ST_DATA int tcc_ext = 1;

/* XXX: get rid of this ASAP */
ST_DATA struct TCCState *tcc_state;

/********************************************************/

#ifdef ONE_SOURCE
#include "tccpp.c"
#include "tccgen.c"
#include "tccelf.c"
#include "tccrun.c"
#ifdef TCC_TARGET_I386
#include "i386-gen.c"
#endif
#ifdef TCC_TARGET_ARM
#include "arm-gen.c"
#endif
#ifdef TCC_TARGET_C67
#include "c67-gen.c"
#endif
#ifdef TCC_TARGET_X86_64
#include "x86_64-gen.c"
#endif
#ifdef CONFIG_TCC_ASM
#include "tccasm.c"
#if defined TCC_TARGET_I386 || defined TCC_TARGET_X86_64
#include "i386-asm.c"
#endif
#endif
#ifdef TCC_TARGET_COFF
#include "tcccoff.c"
#endif
#ifdef TCC_TARGET_PE
#include "tccpe.c"
#endif
#endif /* ONE_SOURCE */

/********************************************************/
#ifndef CONFIG_TCC_ASM
ST_FUNC void asm_instr(void)
{
    tcc_error("内置函数 asm()不支持");
}

ST_FUNC void asm_global_instr(void)
{
    tcc_error("内置函数 asm()不支持");
}
#endif

/********************************************************/

// 转换路径中的'\'为'/'
char *normalize_slashes(char *path)
{
    char *p;
    for (p = path; *p; ++p)
        if (*p == '\\')
            *p = '/';
    return path;
}

/********************************************************/
/* copy a string and truncate it. */
PUB_FUNC char *pstrcpy(char *buf, int buf_size, const char *s)
{
    char *q, *q_end;
    int c;

    if (buf_size > 0) {
        q = buf;
        q_end = buf + buf_size - 1;
        while (q < q_end) {
            c = *s++;
            if (c == '\0')
                break;
            *q++ = c;
        }
        *q = '\0';
    }
    return buf;
}

/* strcat and truncate. */
PUB_FUNC char *pstrcat(char *buf, int buf_size, const char *s)
{
    int len;
    len = strlen(buf);
    if (len < buf_size) 
        pstrcpy(buf + len, buf_size - len, s);
    return buf;
}

PUB_FUNC char *pstrncpy(char *out, const char *in, size_t num)
{
    memcpy(out, in, num);
    out[num] = '\0';
    return out;
}

/* extract the basename of a file */
PUB_FUNC char *tcc_basename(const char *name)
{
    //char *p = strchr(name, 0);
    char *p = (char*)name + strlen((char*)name) + 1;

    while (p > name && !IS_DIRSEP(p[-1]))
        --p;

    return p;
}

/* extract extension part of a file
 *
 * (if no extension, return pointer to end-of-string)
 */
PUB_FUNC char *tcc_fileextension (const char *name)
{
    char *b = tcc_basename(name);
    char *e = strrchr(b, '.');

    return e ? e : strchr(b, 0);
}

/********************************************************/
/* memory management */

//#undef free
//#undef malloc
//#undef realloc

#ifdef MEM_DEBUG
int mem_cur_size;
int mem_max_size;
unsigned malloc_usable_size(void*);
#endif

PUB_FUNC void tcc_free(void *ptr)
{
#ifdef MEM_DEBUG
    mem_cur_size -= malloc_usable_size(ptr);
#endif
	if(ptr)
    free(ptr);
}

PUB_FUNC void *tcc_malloc(unsigned long size)
{
    void *ptr;
    ptr = malloc(size);
    if (!ptr && size)
        tcc_error("memory full");
#ifdef MEM_DEBUG
    mem_cur_size += malloc_usable_size(ptr);
    if (mem_cur_size > mem_max_size)
        mem_max_size = mem_cur_size;
#endif
    return ptr;
}

PUB_FUNC void *tcc_mallocz(unsigned long size)
{
    void *ptr;
    ptr = tcc_malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

PUB_FUNC void *tcc_realloc(void *ptr, unsigned long size)
{
    void *ptr1;
#ifdef MEM_DEBUG
    mem_cur_size -= malloc_usable_size(ptr);
#endif
    ptr1 = realloc(ptr, size);
    if (!ptr1 && size)
        tcc_error("memory full");
#ifdef MEM_DEBUG
    /* NOTE: count not correct if alloc error, but not critical */
    mem_cur_size += malloc_usable_size(ptr1);
    if (mem_cur_size > mem_max_size)
        mem_max_size = mem_cur_size;
#endif
    return ptr1;
}

PUB_FUNC char *tcc_strdup(const char *str)
{
    char *ptr;
    ptr = tcc_malloc(strlen(str) + 1);
    strcpy(ptr, str);
    return ptr;
}

PUB_FUNC void tcc_memstats(void)
{
#ifdef MEM_DEBUG
    printf("memory in use: %d\n", mem_cur_size);
#endif
}

//#define free(p) use_tcc_free(p)
//#define malloc(s) use_tcc_malloc(s)
//#define realloc(p, s) use_tcc_realloc(p, s)

/********************************************************/
/* dynarrays */

PUB_FUNC void dynarray_add(void ***ptab, int *nb_ptr, void *data)
{
    int nb, nb_alloc;
    void **pp;
    
    nb = *nb_ptr;
    pp = *ptab;
    /* 我们每双2的幂,数组的大小 */
    if ((nb & (nb - 1)) == 0) {
        if (!nb)
            nb_alloc = 1;
        else
            nb_alloc = nb * 2;
        pp = tcc_realloc(pp, nb_alloc * sizeof(void *));
        *ptab = pp;
    }
    pp[nb++] = data;
    *nb_ptr = nb;
}

PUB_FUNC void dynarray_reset(void *pp, int *n)
{
    void **p;
    for (p = *(void***)pp; *n; ++p, --*n)
        if (*p)
            tcc_free(*p);
    tcc_free(*(void**)pp);
    *(void**)pp = NULL;
}

static void tcc_split_path(TCCState *s, void ***p_ary, int *p_nb_ary, const char *in)
{
    const char *p;
    do {
        int c;
        CString str;

        cstr_new(&str);
        for (p = in; c = *p, c != '\0' && c != PATHSEP; ++p) {
            if (c == '{' && p[1] && p[2] == '}') {
                c = p[1], p += 2;
                if (c == 'B')
                    cstr_cat(&str, s->tcc_lib_path);
            } else {
                cstr_ccat(&str, c);
            }
        }
        cstr_ccat(&str, '\0');
        dynarray_add(p_ary, p_nb_ary, str.data);
        in = p+1;
    } while (*p);
}

/********************************************************/

ST_FUNC Section *new_section(TCCState *s1, const char *name, int sh_type, int sh_flags)
{
    Section *sec;

    sec = tcc_mallocz(sizeof(Section) + strlen(name));
    strcpy(sec->name, name);
    sec->sh_type = sh_type;
    sec->sh_flags = sh_flags;
    switch(sh_type) {
    case SHT_HASH:
    case SHT_REL:
    case SHT_RELA:
    case SHT_DYNSYM:
    case SHT_SYMTAB:
    case SHT_DYNAMIC:
        sec->sh_addralign = 4;
        break;
    case SHT_STRTAB:
        sec->sh_addralign = 1;
        break;
    default:
        sec->sh_addralign = 32; /* default conservative alignment */
        break;
    }

    if (sh_flags & SHF_PRIVATE) {
        dynarray_add((void ***)&s1->priv_sections, &s1->nb_priv_sections, sec);
    } else {
        sec->sh_num = s1->nb_sections;
        dynarray_add((void ***)&s1->sections, &s1->nb_sections, sec);
    }

    return sec;
}

static void free_section(Section *s)
{
    tcc_free(s->data);
}

/* realloc section and set its content to zero */
ST_FUNC void section_realloc(Section *sec, unsigned long new_size)
{
    unsigned long size;
    unsigned char *data;
    
    size = sec->data_allocated;
    if (size == 0)
        size = 1;
    while (size < new_size)
        size = size * 2;
    data = tcc_realloc(sec->data, size);
    memset(data + sec->data_allocated, 0, size - sec->data_allocated);
    sec->data = data;
    sec->data_allocated = size;
}

/* reserve at least 'size' bytes in section 'sec' from
   sec->data_offset. */
ST_FUNC void *section_ptr_add(Section *sec, unsigned long size)
{
    unsigned long offset, offset1;

    offset = sec->data_offset;
    offset1 = offset + size;
    if (offset1 > sec->data_allocated)
        section_realloc(sec, offset1);
    sec->data_offset = offset1;
    return sec->data + offset;
}

/* reserve at least 'size' bytes from section start */
ST_FUNC void section_reserve(Section *sec, unsigned long size)
{
    if (size > sec->data_allocated)
        section_realloc(sec, size);
    if (size > sec->data_offset)
        sec->data_offset = size;
}

/* return a reference to a section, and create it if it does not
   exists */
ST_FUNC Section *find_section(TCCState *s1, const char *name)
{
    Section *sec;
    int i;
    for(i = 1; i < s1->nb_sections; i++) {
        sec = s1->sections[i];
        if (!strcmp(name, sec->name)) 
            return sec;
    }
    /* sections are created as PROGBITS */
    return new_section(s1, name, SHT_PROGBITS, SHF_ALLOC);
}

/* update sym->c so that it points to an external symbol in section
   'section' with value 'value' */
ST_FUNC void put_extern_sym2(Sym *sym, Section *section, 
                            uplong value, unsigned long size,
                            int can_add_underscore)
{
    int sym_type, sym_bind, sh_num, info, other;
    ElfW(Sym) *esym;
    const char *name;
    char buf1[256];

    if (section == NULL)
        sh_num = SHN_UNDEF;
    else if (section == SECTION_ABS) 
        sh_num = SHN_ABS;
    else
        sh_num = section->sh_num;

    if ((sym->type.t & VT_BTYPE) == VT_FUNC) {
        sym_type = STT_FUNC;
    } else if ((sym->type.t & VT_BTYPE) == VT_VOID) {
        sym_type = STT_NOTYPE;
    } else {
        sym_type = STT_OBJECT;
    }

    if (sym->type.t & VT_STATIC)
        sym_bind = STB_LOCAL;
    else {
        if (sym->type.t & VT_WEAK)
            sym_bind = STB_WEAK;
        else
            sym_bind = STB_GLOBAL;
    }

    if (!sym->cd.c) {
        name = get_tok_str(sym->v, NULL);
#ifdef CONFIG_TCC_BCHECK
        if (tcc_state->do_bounds_check) {
            char buf[32];

            /* XXX: avoid doing that for statics ? */
            /* if bound checking is activated, we change some function
               names by adding the "__bound" prefix */
            switch(sym->v) {
#ifdef TCC_TARGET_PE
            /* XXX: we rely only on malloc hooks */
            case TOK_malloc: 
            case TOK_free: 
            case TOK_realloc: 
            case TOK_memalign: 
            case TOK_calloc: 
#endif
            case TOK_memcpy: 
            case TOK_memmove:
            case TOK_memset:
            case TOK_strlen:
            case TOK_strcpy:
            case TOK_alloca:
                strcpy(buf, "__bound_");
                strcat(buf, name);
                name = buf;
                break;
            }
        }
#endif
        other = 0;

        if (tcc_state->leading_underscore && can_add_underscore) {
            buf1[0] = '_';
            pstrcpy(buf1 + 1, sizeof(buf1) - 1, name);
            name = buf1;
        }
        if (sym->asm_label) {
            name = sym->asm_label;
        }
        info = ELFW(ST_INFO)(sym_bind, sym_type);
        sym->cd.c = add_elf_sym(symtab_section, value, size, info, other, sh_num, name);
    } else {
        esym = &((ElfW(Sym) *)symtab_section->data)[sym->cd.c];
        esym->st_value = value;
        esym->st_size = size;
        esym->st_shndx = sh_num;
    }
}

ST_FUNC void put_extern_sym(Sym *sym, Section *section, 
                           uplong value, unsigned long size)
{
    put_extern_sym2(sym, section, value, size, 1);
}

/* add a new relocation entry to symbol 'sym' in section 's' */
ST_FUNC void greloc(Section *s, Sym *sym, unsigned long offset, int type)
{
    int c = 0;
    if (sym) {
        if (0 == sym->cd.c)
            put_extern_sym(sym, NULL, 0, 0);
        c = sym->cd.c;
    }
    /* now we can add ELF relocation info */
    put_elf_reloc(symtab_section, s, offset, type, c);
}

/********************************************************/
// 编译信息输出

// 设置信息输出函数
LIBTCCAPI void tcc_set_error_func(TCCState *s, void *error_opaque,
                        void (*error_func)(void *opaque, const char *msg))
{
    s->error_opaque = error_opaque;
    s->error_func = error_func;
}

static void strcat_vprintf(char *buf, const char *fmt, va_list ap)
{
    int len;

    len = strlen(buf);
    vsprintf(buf + len, fmt, ap);
}

static void strcat_printf(char *buf, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    strcat_vprintf(buf, fmt, ap);
    va_end(ap);
}

static void error1(TCCState *s1, int is_warning, const char *fmt, va_list ap)
{
    char buf[512];
    BufferedFile **f;

    buf[0] = '\0';

    if (file) {
        for (f = s1->include_stack; f < s1->include_stack_ptr; f++)
            strcat_printf(buf, "文件 %s:第%d行:\n", 
            (*f)->filename, (*f)->line_num);
        if (file->line_num > 0) {
            strcat_printf(buf, "%s:第%d行: ",
                file->filename, file->line_num);
        } else {
            strcat_printf(buf, "%s: ", file->filename);
        }
    } else {
        strcat_printf(buf, "xqc: ");
    }

    if (is_warning)
        strcat_printf(buf, "警告: ");
    else
        strcat_printf(buf, "错误: ");

    strcat_vprintf(buf, fmt, ap);

    if (!s1->error_func) {
        /* default case: stderr */
        fprintf(stderr, "%s\n", buf);
    } else {
        s1->error_func(s1->error_opaque, buf);
    }

    if (!is_warning || s1->warn_error)
        s1->nb_errors++;
}

#if 0
void error1(TCCState *s1, int is_warning, char *msg)
{
    char buf[512] = {0};
    int slen = 0;
    BufferedFile **f;

    if (file) {
        for(f = s1->include_stack; f < s1->include_stack_ptr; f++)
            slen += sprintf(buf + slen, "In file included from %s:%d:\n", 
                          (*f)->filename, (*f)->line_num);

        if (file->line_num > 0) {
            slen += sprintf(buf + slen, "%s:%d: ", file->filename, file->line_num);
        } else {
            slen += sprintf(buf + slen, "%s: ", file->filename);
        }
    } else {
        slen += sprintf(buf + slen, "tcc: ");
    }

    if (is_warning)
        slen += sprintf(buf + slen, "warning: %s", msg);
    else
        slen += sprintf(buf + slen, "error: %s", msg);

    if (!s1->error_func) {
        /* default case: stderr */
        fprintf(stderr, "%s\n", buf);
    } else {
        s1->error_func(s1->error_opaque, buf);
    }

    if (!is_warning || s1->warn_error)
        s1->nb_errors++;
}
#endif

// 输出编译错误信息
PUB_FUNC void tcc_error(const char *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    va_start(ap, fmt);
    error1(s1, 0, fmt, ap);
    va_end(ap);

    /* better than nothing: in some cases, we accept to handle errors */
    if (tcc_state->error_set_jmp_enabled) {
        longjmp(tcc_state->error_jmp_buf, 1);
    } else {
        /* XXX: eliminate this someday */
        exit(1);
    }
}

// 输出编译非致命错误信息
/* error without aborting current compilation */
PUB_FUNC void tcc_error_noabort(const char *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    va_start(ap, fmt);
    error1(s1, 0, fmt, ap);
    va_end(ap);
}

// 输出编译警告信息
PUB_FUNC void tcc_warning(const char *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    if (s1->warn_none)
    {
        va_start(ap, fmt);
        error1(s1, 1, fmt, ap);
        va_end(ap);
    }
}

/********************************************************/
/* I/O layer */

ST_FUNC void tcc_open_bf(TCCState *s1, const char *filename, int initlen)
{
    BufferedFile *bf;
    int buflen = initlen ? initlen : IO_BUF_SIZE;

    bf = tcc_mallocz(sizeof(BufferedFile) + buflen);
    bf->buf_ptr = bf->buffer;
    bf->buf_end = bf->buffer + initlen;
    bf->buf_end[0] = CH_EOB; /* put eob symbol */
    pstrcpy(bf->filename, sizeof(bf->filename), filename);
#ifdef _WIN32
    normalize_slashes(bf->filename);
#endif
    bf->line_num = 1;
    bf->ifndef_macro = 0;
    bf->ifdef_stack_ptr = s1->ifdef_stack_ptr;
    bf->fd = -1;
    bf->prev = file;
    file = bf;
}

ST_FUNC void tcc_close(void)
{
    BufferedFile *bf = file;

    if (bf->fd > 0) {
        close(bf->fd);
        total_lines += bf->line_num;
    }
    file = bf->prev;
    tcc_free(bf);
}

ST_FUNC int tcc_open(TCCState *s1, const char *filename)
{
    int fd;

    if (strcmp(filename, "-") == 0)
        fd = 0, filename = "stdin";
    else
        fd = open(filename, MR_FILE_RDWR);

    if ((s1->verbose == 2 && fd >= 0) || s1->verbose == 3)
        printf("%s %*s%s\n", fd < 0 ? "nf":"->",
               (int)(s1->include_stack_ptr - s1->include_stack), "", filename);

    if (fd == 0)
        return -1;

    tcc_open_bf(s1, filename, 0);
    file->fd = fd;

    return fd;
}

/* 编译C文件中打开“文件”。如果错误返回非零. */
static int tcc_compile(TCCState *s1)
{
    Sym *define_start;
    SValue *pvtop;
    //char buf[512];
    volatile int section_sym;

#ifdef INC_DEBUG
    printf("%s: **** new file\n", file->filename);
#endif
    preprocess_init(s1);

    cur_text_section = NULL;
    funcname = "";
    anon_sym = SYM_FIRST_ANOM; 

    /* file info: full path + filename */
    section_sym = 0; /* avoid warning */

#if 0  // 不需要调试 eleqian 2012-7-15
    if (s1->do_debug) {
        section_sym = put_elf_sym(symtab_section, 0, 0, 
                                  ELFW(ST_INFO)(STB_LOCAL, STT_SECTION), 0, 
                                  text_section->sh_num, NULL);
        getcwd(buf, sizeof(buf));
#ifdef _WIN32
        normalize_slashes(buf);
#endif
        pstrcat(buf, sizeof(buf), "/");
        put_stabs_r(buf, N_SO, 0, 0, 
                    text_section->data_offset, text_section, section_sym);
        put_stabs_r(file->filename, N_SO, 0, 0, 
                    text_section->data_offset, text_section, section_sym);
    }
#endif

    /* an elf symbol of type STT_FILE must be put so that STB_LOCAL
       symbols can be safely used */
    put_elf_sym(symtab_section, 0, 0, 
                ELFW(ST_INFO)(STB_LOCAL, STT_FILE), 0, 
                SHN_ABS, file->filename);

    /* define some often used types */
    int_type.t = VT_INT;

    char_pointer_type.t = VT_BYTE;
    mk_pointer(&char_pointer_type);

#if PTR_SIZE == 4
    size_type.t = VT_INT;
#else
    size_type.t = VT_LLONG;
#endif

    func_old_type.t = VT_FUNC;
    func_old_type.ref = sym_push(SYM_FIELD, &int_type, FUNC_CDECL, FUNC_OLD);

#if defined(TCC_ARM_EABI) && defined(TCC_ARM_VFP)
    float_type.t = VT_FLOAT;
    double_type.t = VT_DOUBLE;

    func_float_type.t = VT_FUNC;
    func_float_type.ref = sym_push(SYM_FIELD, &float_type, FUNC_CDECL, FUNC_OLD);
    func_double_type.t = VT_FUNC;
    func_double_type.ref = sym_push(SYM_FIELD, &double_type, FUNC_CDECL, FUNC_OLD);
#endif

#if 0
    /* define 'void *alloca(unsigned int)' builtin function */
    {
        Sym *s1;

        p = anon_sym++;
        sym = sym_push(p, mk_pointer(VT_VOID), FUNC_CDECL, FUNC_NEW);
        s1 = sym_push(SYM_FIELD, VT_UNSIGNED | VT_INT, 0, 0);
        s1->nj.next = NULL;
        sym->nj.next = s1;
        sym_push(TOK_alloca, VT_FUNC | (p << VT_STRUCT_SHIFT), VT_CONST, 0);
    }
#endif

    define_start = define_stack;
    nocode_wanted = 1;

    if (setjmp(s1->error_jmp_buf) == 0) {
        s1->nb_errors = 0;
        s1->error_set_jmp_enabled = 1;

        ch = file->buf_ptr[0];
        tok_flags = TOK_FLAG_BOL | TOK_FLAG_BOF;
        parse_flags = PARSE_FLAG_PREPROCESS | PARSE_FLAG_TOK_NUM;
        pvtop = vtop;
        next();
        decl(VT_CONST);
        if (tok != TOK_EOF)
            expect("declaration");
        if (pvtop != vtop)
			tcc_warning("内部编译错误:vstack泄漏? (%d)", vtop - pvtop);

        /* end of translation unit info */
        /*if (s1->do_debug) {
            put_stabs_r(NULL, N_SO, 0, 0, 
                        text_section->data_offset, text_section, section_sym);
        }*/
    }

    s1->error_set_jmp_enabled = 0;

    /* reset define stack, but leave -Dsymbols (may be incorrect if
       they are undefined) */
    free_defines(define_start); 

    gen_inline_functions();

    sym_pop(&global_stack, NULL);
    sym_pop(&local_stack, NULL);

    return s1->nb_errors != 0 ? -1 : 0;
}

LIBTCCAPI int tcc_compile_string(TCCState *s, const char *str)
{
    int len, ret;
    len = strlen(str);

	tcc_open_bf(s, "<string>", len);
    memcpy(file->buffer, str, len);
    ret = tcc_compile(s);
    tcc_close();
    return ret;
}

/*定义一个预处理器的象征。值还可以提供“=”操作符 */
LIBTCCAPI void tcc_define_symbol(TCCState *s1, const char *sym, const char *value)
{
    int len1, len2;
    /* default value */
    if (!value)
        value = "1";
    len1 = strlen(sym);
    len2 = strlen(value);

    /* init file structure */
    tcc_open_bf(s1, "<define>", len1 + len2 + 1);
    memcpy(file->buffer, sym, len1);
    file->buffer[len1] = ' ';
    memcpy(file->buffer + len1 + 1, value, len2);

    /* parse with define parser */
    ch = file->buf_ptr[0];
    next_nomacro();
    parse_define();

    tcc_close();
}

/* undefine a preprocessor symbol */
LIBTCCAPI void tcc_undefine_symbol(TCCState *s1, const char *sym)
{
    TokenSym *ts;
    Sym *s;
    ts = tok_alloc(sym, strlen(sym));
    s = define_find(ts->tok);
    /* undefine symbol by putting an invalid name */
    if (s)
        define_undef(s);
}

LIBTCCAPI int tcc_add_symbol(TCCState *s, const char *name, const void *val)
{
    add_elf_sym(symtab_section, (uplong)val, 0,
        ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
        SHN_ABS, name);
    return 0;
}

static void tcc_cleanup(void)
{
    int i, n;

    if (NULL == tcc_state)
        return;
    tcc_state = NULL;

    /* free -D defines */
    free_defines(NULL);

    /* free tokens */
    n = tok_ident - TOK_IDENT;
    for(i = 0; i < n; i++)
        tcc_free(table_ident[i]);
    tcc_free(table_ident);

    /* free sym_pools */
    dynarray_reset(&sym_pools, &nb_sym_pools);
    /* string buffer */
    cstr_free(&tokcstr);
    /* reset symbol stack */
    sym_free_first = NULL;
    /* cleanup from error/setjmp */
    macro_ptr = NULL;
}

LIBTCCAPI TCCState *tcc_new(void)
{
    TCCState *s;
    //char buffer[100];
    //int a,b,c;

    tcc_cleanup();

    s = tcc_mallocz(sizeof(TCCState));
    if (!s)
        return NULL;
    tcc_state = s;
    tcc_set_lib_path(s, CONFIG_TCCDIR);
    s->output_type = TCC_OUTPUT_MEMORY;
    preprocess_new();
    s->include_stack_ptr = s->include_stack;

	/* 我们添加虚拟为一些特殊的宏定义加速测试和工作defined() */
    define_push(TOK___LINE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___FILE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___DATE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___TIME__, MACRO_OBJ, NULL, NULL);

    /* 标准定义 */
    tcc_define_symbol(s, "__STDC__", NULL);
    tcc_define_symbol(s, "__STDC_VERSION__", "199901L");
#if defined(TCC_TARGET_ARM)
    tcc_define_symbol(s, "__ARM_ARCH_4__", NULL);
    tcc_define_symbol(s, "__arm_elf__", NULL);
    tcc_define_symbol(s, "__arm_elf", NULL);
    tcc_define_symbol(s, "arm_elf", NULL);
    tcc_define_symbol(s, "__arm__", NULL);
    tcc_define_symbol(s, "__arm", NULL);
    tcc_define_symbol(s, "arm", NULL);
    tcc_define_symbol(s, "__APCS_32__", NULL);
#endif

	/* tiny C 具体定义 */
    /*sscanf(TCC_VERSION, "%d.%d.%d", &a, &b, &c);
    sprintf(buffer, "%d", a*10000 + b*100 + c);
    tcc_define_symbol(s, "__TINYC__", buffer);*/

    /* tiny C & gcc defines */
    tcc_define_symbol(s, "__SIZE_TYPE__", "unsigned long");
    tcc_define_symbol(s, "__PTRDIFF_TYPE__", "long");

    tcc_define_symbol(s, "__WCHAR_TYPE__", "int");

    /* glibc defines */
    tcc_define_symbol(s, "__REDIRECT(name, proto, alias)", "name proto __asm__ (#alias)");
    tcc_define_symbol(s, "__REDIRECT_NTH(name, proto, alias)", "name proto __asm__ (#alias) __THROW");

    /* no section zero */
    dynarray_add((void ***)&s->sections, &s->nb_sections, NULL);

    /* 创建标准部分 */
    text_section = new_section(s, ".text", SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR);
    data_section = new_section(s, ".data", SHT_PROGBITS, SHF_ALLOC | SHF_WRITE);
    bss_section = new_section(s, ".bss", SHT_NOBITS, SHF_ALLOC | SHF_WRITE);

    /* 符号总是为链接生成阶段 */
    symtab_section = new_symtab(s, ".symtab", SHT_SYMTAB, 0,
                                ".strtab",
                                ".hashtab", SHF_PRIVATE); 
    strtab_section = symtab_section->link;
    s->symtab = symtab_section;

    /* 私人动态符号的符号表 */
    s->dynsymtab_section = new_symtab(s, ".dynsymtab", SHT_SYMTAB, SHF_PRIVATE,
                                      ".dynstrtab", 
                                      ".dynhashtab", SHF_PRIVATE);
    s->alacarte_link = 1;
    s->nocommon = 1;

#ifdef CHAR_IS_UNSIGNED
    s->char_is_unsigned = 1;
#endif

    if (s->section_align == 0)
        s->section_align = ELF_PAGE_SIZE;

    return s;
}

LIBTCCAPI void tcc_delete(TCCState *s1)
{
    int i;

    tcc_cleanup();

    /* free all sections */
    for(i = 1; i < s1->nb_sections; i++)
        free_section(s1->sections[i]);
    dynarray_reset(&s1->sections, &s1->nb_sections);

    for(i = 0; i < s1->nb_priv_sections; i++)
        free_section(s1->priv_sections[i]);
    dynarray_reset(&s1->priv_sections, &s1->nb_priv_sections);
        
    /* free any loaded DLLs */
    for ( i = 0; i < s1->nb_loaded_dlls; i++) {
        DLLReference *ref = s1->loaded_dlls[i];
        if ( ref->handle )
            dlclose(ref->handle);
    }
    
    /* free loaded dlls array */
    dynarray_reset(&s1->loaded_dlls, &s1->nb_loaded_dlls);

    /* free library paths */
    dynarray_reset(&s1->library_paths, &s1->nb_library_paths);
    dynarray_reset(&s1->crt_paths, &s1->nb_crt_paths);

    /* free include paths */
    dynarray_reset(&s1->cached_includes, &s1->nb_cached_includes);
    dynarray_reset(&s1->include_paths, &s1->nb_include_paths);
    dynarray_reset(&s1->sysinclude_paths, &s1->nb_sysinclude_paths);

    dynarray_reset(&s1->target_deps, &s1->nb_target_deps);

    tcc_free(s1->tcc_lib_path);
    tcc_free(s1->runtime_mem);
    tcc_free(s1);
}

ST_FUNC int tcc_add_file_internal(TCCState *s1, const char *filename, int flags)
{
    const char *ext;
    ElfW(Ehdr) ehdr;
    int fd, ret, size;

    /* 找到源文件类型扩展名 */
    ext = tcc_fileextension(filename);
    if (ext[0])
        ext++;

	/* 打开文件 */
    ret = tcc_open(s1, filename);
    if (ret < 0) {
        if (flags & AFF_PRINT_ERROR)
            tcc_error_noabort("文件 '%s' 未找到", filename);
        return ret;
    }

    /* 更新目标deps */
    dynarray_add((void ***)&s1->target_deps, &s1->nb_target_deps,
            tcc_strdup(filename));

    if (flags & AFF_PREPROCESS) {
        ret = tcc_preprocess(s1);
        goto the_end;
    }

    if (!ext[0] || !PATHCMP(ext, "c")) {
        /* C 文件认为 */
        ret = tcc_compile(s1);
        goto the_end;
    }

    fd = file->fd;
    /* 假设文件是可执行格式 */
    size = read(fd, &ehdr, sizeof(ehdr));
    lseek(fd, 0, SEEK_SET);
    if (size <= 0) {
        tcc_error_noabort("不能读头");
        goto the_end;
    }

    if (size == sizeof(ehdr) &&
        ehdr.e_ident[0] == ELFMAG0 &&
        ehdr.e_ident[1] == ELFMAG1 &&
        ehdr.e_ident[2] == ELFMAG2 &&
        ehdr.e_ident[3] == ELFMAG3) {

        /* 不显示行号是否错误 */
        file->line_num = 0;
        if (ehdr.e_type == ET_REL) {
            ret = tcc_load_object_file(s1, fd, 0);
            goto the_end;
        }
        if (ehdr.e_type == ET_DYN) {
            /*if (s1->output_type == TCC_OUTPUT_MEMORY) {
                void *h;
                h = dlopen(filename, RTLD_GLOBAL | RTLD_LAZY);
                if (h)
                    ret = 0;
            }
            goto the_end;*/
        }
        tcc_error_noabort("未被认出的ELF文件");
        goto the_end;
    }

    if (memcmp((char *)&ehdr, ARMAG, 8) == 0) {
        file->line_num = 0; /* do not display line number if error */
        ret = tcc_load_archive(s1, fd);
        goto the_end;
    }

    if (ret < 0)
        tcc_error_noabort("无法识别的文件类型");

the_end:
    tcc_close();
    return ret;
}

LIBTCCAPI int tcc_add_file(TCCState *s, const char *filename)
{
    if (s->output_type == TCC_OUTPUT_PREPROCESS)
        return tcc_add_file_internal(s, filename, AFF_PRINT_ERROR | AFF_PREPROCESS);
    else
        return tcc_add_file_internal(s, filename, AFF_PRINT_ERROR);
}

LIBTCCAPI int tcc_add_include_path(TCCState *s, const char *pathname)
{
    tcc_split_path(s, (void ***)&s->include_paths, &s->nb_include_paths, pathname);
    return 0;
}

LIBTCCAPI int tcc_add_sysinclude_path(TCCState *s, const char *pathname)
{
    tcc_split_path(s, (void ***)&s->sysinclude_paths, &s->nb_sysinclude_paths, pathname);
    return 0;
}

#define WD_ALL    0x0001 /* 使用- wall时警报被激活 */
#define FD_INVERT 0x0002 /* 转化价值之前存储 */

typedef struct FlagDef {
    uint16_t offset;
    uint16_t flags;
    //const char *name;
    const char name[48];  // mrp中不支持字符串指针形式初始化
} FlagDef;

static const FlagDef warning_defs[] = {
    { offsetof(TCCState, warn_unsupported), 0, "unsupported" },
    { offsetof(TCCState, warn_write_strings), 0, "write-strings" },
    { offsetof(TCCState, warn_error), 0, "error" },
    { offsetof(TCCState, warn_implicit_function_declaration), WD_ALL,
      "implicit-function-declaration" },
};

ST_FUNC int set_flag(TCCState *s, const FlagDef *flags, int nb_flags,
                    const char *name, int value)
{
    int i;
    const FlagDef *p;
    const char *r;

    r = name;
    if (r[0] == 'n' && r[1] == 'o' && r[2] == '-') {
        r += 3;
        value = !value;
    }
    for(i = 0, p = flags; i < nb_flags; i++, p++) {
        if (!strcmp(r, p->name))
            goto found;
    }
    return -1;
 found:
    if (p->flags & FD_INVERT)
        value = !value;
    *(int *)((uint8_t *)s + p->offset) = value;
    return 0;
}

/* enable debug */
LIBTCCAPI void tcc_enable_debug(TCCState *s)
{
    s->do_debug = 1;
}

/* set/reset a warning */
LIBTCCAPI int tcc_set_warning(TCCState *s, const char *warning_name, int value)
{
    int i;
    const FlagDef *p;

    if (!strcmp(warning_name, "all")) {
        for(i = 0, p = warning_defs; i < countof(warning_defs); i++, p++) {
            if (p->flags & WD_ALL)
                *(int *)((uint8_t *)s + p->offset) = 1;
        }
        return 0;
    } else {
        return set_flag(s, warning_defs, countof(warning_defs),
                        warning_name, value);
    }
}

static const FlagDef flag_defs[] = {
    { offsetof(TCCState, char_is_unsigned), 0, "unsigned-char" },
    { offsetof(TCCState, char_is_unsigned), FD_INVERT, "signed-char" },
    { offsetof(TCCState, nocommon), FD_INVERT, "common" },
    { offsetof(TCCState, leading_underscore), 0, "leading-underscore" },
};

/* set/reset a flag */
PUB_FUNC int tcc_set_flag(TCCState *s, const char *flag_name, int value)
{
    return set_flag(s, flag_defs, countof(flag_defs),
                    flag_name, value);
}

PUB_FUNC void tcc_print_stats(TCCState *s, int64_t total_time)
{
    double tt;
    tt = (double)total_time / 1000000.0;
    if (tt < 0.001)
        tt = 0.001;
    if (total_bytes < 1)
        total_bytes = 1;
    printf("%d idents, %d lines, %d bytes, %0.3f s, %d lines/s, %0.1f MB/s\n", 
        tok_ident - TOK_IDENT, total_lines, total_bytes,
        tt, (int)(total_lines / tt),
        total_bytes / tt / 1000000.0);
}

#ifdef TCC_SUPORT_LINKER

static int strstart(const char *str, const char *val, char **ptr)
{
    const char *p, *q;
    p = str;
    q = val;
    while (*q != '\0') {
        if (*p != *q)
            return 0;
        p++;
        q++;
    }
    if (ptr)
        *ptr = (char *) p;
    return 1;
}

/* Like strstart, but automatically takes into account that ld options can
 *
 * - start with double or single dash (e.g. '--soname' or '-soname')
 * - arguments can be given as separate or after '=' (e.g. '-Wl,-soname,x.so'
 *   or '-Wl,-soname=x.so')
 *
 * you provide `val` always in 'option[=]' form (no leading -)
 */
static int link_option(const char *str, const char *val, char **ptr)
{
    const char *p, *q;

    /* there should be 1 or 2 dashes */
    if (*str++ != '-')
        return 0;
    if (*str == '-')
        str++;

    /* then str & val should match (potentialy up to '=') */
    p = str;
    q = val;

    while (*q != '\0' && *q != '=') {
        if (*p != *q)
            return 0;
        p++;
        q++;
    }

    /* '=' near eos means ',' or '=' is ok */
    if (*q == '=') {
        if (*p != ',' && *p != '=')
            return 0;
        p++;
        q++;
    }

    if (ptr)
        *ptr = (char *) p;
    return 1;
}

/* set linker options */
PUB_FUNC const char * tcc_set_linker(TCCState *s, char *option, int multi)
{
    char *p = option;
    char *end;

    while (option && *option) {
        end = NULL;
        if (link_option(option, "Bsymbolic", &p)) {
            s->symbolic = TRUE;
        } else if (link_option(option, "nostdlib", &p)) {
            s->nostdlib = TRUE;
        } else if (link_option(option, "fini=", &p)) {
            s->fini_symbol = p;
            if (s->warn_unsupported)
                tcc_warning("忽略 -fini %s", p);
        } else if (link_option(option, "image-base=", &p)) {
              // eleqian添加强制类型转换 2012-7-15
            s->text_addr = (uplong)strtoull(p, &end, 16);
            s->has_text_addr = 1;
        } else if (link_option(option, "init=", &p)) {
            s->init_symbol = p;
            if (s->warn_unsupported)
                tcc_warning("忽略 -init %s", p);
        } else if (link_option(option, "oformat=", &p)) {
            if (strstart(p, "elf32-", NULL)) {
                s->output_format = TCC_OUTPUT_FORMAT_ELF;
            } else if (!strcmp(p, "binary")) {
                s->output_format = TCC_OUTPUT_FORMAT_BINARY;
            } else {
                return p;
            }
        } else if (link_option(option, "rpath=", &p)) {
            s->rpath = p;
        } else if (link_option(option, "section-alignment=", &p)) {
            s->section_align = strtoul(p, &end, 16);
        } else if (link_option(option, "soname=", &p)) {
            s->soname = p;
            multi = 0;
        } else if (link_option(option, "Ttext=", &p)) {
            // eleqian添加强制类型转换 2012-7-15
            s->text_addr = (uplong)strtoull(p, &end, 16);
            s->has_text_addr = 1;
        } else {
            char *comma_ptr = strchr(option, ',');
            if (comma_ptr)
                *comma_ptr = '\0';
            return option;
        }

        if (multi) {
            option = NULL;
            p = strchr( (end) ? end : p, ',');
            if (p) {
                *p = 0;    /* terminate last option */
                option = ++p;
            }
        } else
            option = NULL;
    }
    return NULL;
}

static int tcc_add_library_internal(TCCState *s, const char *fmt,
                                    const char *filename, int flags, char **paths, int nb_paths)
{
    char buf[260];
    int i;

    for(i = 0; i < nb_paths; i++) {
        //snprintf(buf, sizeof(buf), fmt, paths[i], filename);
        sprintf(buf, fmt, paths[i], filename);
        if (tcc_add_file_internal(s, buf, flags) == 0)
            return 0;
    }
    return -1;
}

LIBTCCAPI int tcc_add_library_path(TCCState *s, const char *pathname)
{
    tcc_split_path(s, (void ***)&s->library_paths, &s->nb_library_paths, pathname);
    return 0;
}

/* 查找和加载dll。如果没有找到返回非零*/
/* XXX: 添加“-rpath”选项的支持 ? */
ST_FUNC int tcc_add_dll(TCCState *s, const char *filename, int flags)
{
    return tcc_add_library_internal(s, "%s/%s", filename, flags,
        s->library_paths, s->nb_library_paths);
}

ST_FUNC int tcc_add_crt(TCCState *s, const char *filename)
{
    if (-1 == tcc_add_library_internal(s, "%s/%s",
        filename, 0, s->crt_paths, s->nb_crt_paths))
        tcc_error_noabort("文件 '%s' 未找到", filename);
    return 0;
}

/* the library name is the same as the argument of the '-l' option */
LIBTCCAPI int tcc_add_library(TCCState *s, const char *libraryname)
{
#if 1  // mrp中不需要支持动态库
    const char *libs[] = { "%s/lib%s.so", "%s/lib%s.a", NULL };
    const char **pp = s->static_link ? libs + 1 : libs;

    while (*pp) {
        if (0 == tcc_add_library_internal(s, *pp,
            libraryname, 0, s->library_paths, s->nb_library_paths))
            return 0;
        ++pp;
    }
#else
    if (0 == tcc_add_library_internal(s, "%s/lib%s.a",
        libraryname, 0, s->library_paths, s->nb_library_paths))
        return 0;
#endif

    return -1;
}

/* set CONFIG_TCCDIR at runtime */
LIBTCCAPI void tcc_set_lib_path(TCCState *s, const char *path)
{
    tcc_free(s->tcc_lib_path);
    s->tcc_lib_path = tcc_strdup(path);
}

PUB_FUNC char *tcc_default_target(TCCState *s, const char *default_file)
{
    char buf[260];
    char *ext;
    const char *name = "a";

    if (default_file && strcmp(default_file, "-"))
        name = tcc_basename(default_file);
    pstrcpy(buf, sizeof(buf), name);
    ext = tcc_fileextension(buf);
    if (( (s->output_type == TCC_OUTPUT_OBJ && !s->reloc_output) ||
          (s->output_type == TCC_OUTPUT_PREPROCESS) )
        && *ext)
        strcpy(ext, ".o");
    else
        pstrcpy(buf, sizeof(buf), "a.out");

    return tcc_strdup(buf);
}

#endif  /* TCC_SUPORT_LINKER */

#if 1
PUB_FUNC void tcc_gen_makedeps(TCCState *s, const char *target, const char *filename)
{
    int32 depout;
    char buf[1024], *ext;
    int i;

    if (!filename) {
        /* compute filename automatically
         * dir/file.o -> dir/file.d             */
        pstrcpy(buf, sizeof(buf), target);
        ext = tcc_fileextension(buf);
        pstrcpy(ext, sizeof(buf) - (ext-buf), ".d");
        filename = buf;
    }

    if (s->verbose)
        printf("<- %s\n", filename);

    /* XXX return err codes instead of error() ? */
    depout = mrc_open(filename, MR_FILE_RDWR);
    if (!depout)
        tcc_error("无法打开 '%s'", filename);

    fprintf(depout, "%s : \\\n", target);
    for (i=0; i<s->nb_target_deps; ++i)
        fprintf(depout, " %s \\\n", s->target_deps[i]);
    fprintf(depout, "\n");
    mrc_close(depout);
}
#endif
