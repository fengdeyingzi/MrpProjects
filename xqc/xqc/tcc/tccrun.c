/*
 *  TCC - Tiny C Compiler - Support for -run switch
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

/* only native compiler supports -run */
#ifdef TCC_IS_NATIVE

/* ------------------------------------------------------------- */

/* relocate code. Return -1 on error, required size if ptr is NULL,
   otherwise copy code into buffer passed by the caller */
int tcc_relocate_ex(TCCState *s1, void *ptr)
{
    Section *s;
    unsigned long offset, length;
    uplong mem;
    int i;

    if (0 == s1->runtime_added) {
        s1->runtime_added = 1;
        s1->nb_errors = 0;

        //tcc_add_runtime(s1);
        relocate_common_syms();
        //tcc_add_linker_symbols(s1);
        build_got_entries(s1);

        if (s1->nb_errors)
            return -1;
    }

    offset = 0;
    mem = (uplong)ptr;

    for(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        if (0 == (s->sh_flags & SHF_ALLOC))
            continue;

        // 按16字节对齐内存地址
        length = s->data_offset;
        s->sh_addr = mem ? (mem + offset + 0xf) & ~0xf : 0;
        offset = (offset + length + 0xf) & ~0xf;
    }
    offset += 0x10;

    /* relocate symbols */
    relocate_syms(s1, 1);
    if (s1->nb_errors)
        return -1;

    s1->runtime_plt_and_got_offset = 0;
    s1->runtime_plt_and_got = (char *)(mem + offset);

#if 1  // 有点浪费内存，有必要优化 eleqian 2012-7-14
    /* double the size of the buffer for got and plt entries
       XXX: calculate exact size for them? */
    offset *= 2;
#endif

    if (0 == mem)
        return offset;

    /* relocate each section */
    for(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        if (s->reloc)
            relocate_section(s1, s);
    }

    for(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        if (0 == (s->sh_flags & SHF_ALLOC))
            continue;
        length = s->data_offset;
        // printf("%-12s %08x %04x\n", s->name, s->sh_addr, length);
        ptr = (void*)(uplong)s->sh_addr;
        if (NULL == s->data || s->sh_type == SHT_NOBITS)
            memset(ptr, 0, length);
        else
            memcpy(ptr, s->data, length);
    }

    return 0;
}

/* Do all relocations (needed before using tcc_get_symbol())
   Returns -1 on error. */
LIBTCCAPI int tcc_relocate(TCCState *s1)
{
    int ret;

    ret = tcc_relocate_ex(s1, NULL);
    if (-1 != ret) {
        s1->runtime_mem = tcc_malloc(ret);
        ret = tcc_relocate_ex(s1, s1->runtime_mem);
    }

    return ret;
}

/* launch the compiled program with the given arguments */
LIBTCCAPI int tcc_run(TCCState *s1, int argc, char **argv)
{
    int (*prog_main)(int, char **);
    int ret;

    if (tcc_relocate(s1) < 0)
        return -1;

    prog_main = (int(*)(int, char **))tcc_get_symbol_err(s1, "main");

#ifdef CONFIG_TCC_BCHECK
    if (s1->do_bounds_check) {
        void (*bound_init)(void);
        void (*bound_exit)(void);
        /* set error function */
        rt_bound_error_msg = tcc_get_symbol_err(s1, "__bound_error_msg");
        /* XXX: use .init section so that it also work in binary ? */
        bound_init = tcc_get_symbol_err(s1, "__bound_init");
        bound_exit = tcc_get_symbol_err(s1, "__bound_exit");
        bound_init();
        ret = (*prog_main)(argc, argv);
        bound_exit();
    } else
#endif
    ret = (*prog_main)(argc, argv);
    return ret;
}

/* ------------------------------------------------------------- */
#endif /* TCC_IS_NATIVE */

/* ------------------------------------------------------------- */

#ifdef CONFIG_TCC_STATIC

#define RTLD_LAZY       0x001
#define RTLD_NOW        0x002
#define RTLD_GLOBAL     0x100
#define RTLD_DEFAULT    NULL

/* dummy function for profiling */
ST_FUNC void *dlopen(const char *filename, int flag)
{
    return NULL;
}

ST_FUNC void dlclose(void *p)
{
}
/*
const char *dlerror(void)
{
    return "error";
}
*/
typedef struct TCCSyms {
    char *str;
    void *ptr;
} TCCSyms;

#define TCCSYM(a) { #a, &a, },

/* add the symbol you want here if no dynamic linking is done */
static TCCSyms tcc_syms[] = {
#if !defined(CONFIG_TCCBOOT)
    /*TCCSYM(printf)
    TCCSYM(fprintf)
    TCCSYM(fopen)
    TCCSYM(fclose)*/
#endif
    { NULL, NULL },
};

ST_FUNC void *resolve_sym(TCCState *s1, const char *symbol)
{
    TCCSyms *p;
    p = tcc_syms;
    while (p->str != NULL) {
        if (!strcmp(p->str, symbol))
            return p->ptr;
        p++;
    }
    return NULL;
}

#endif /* CONFIG_TCC_STATIC */

/* ------------------------------------------------------------- */
