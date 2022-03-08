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

#ifndef _TCC_H
#define _TCC_H


#define _GNU_SOURCE
#include "config.h"

#ifdef CONFIG_TCCBOOT

#include "tccboot.h"
#define CONFIG_TCC_STATIC

#else

#ifndef TCC_TARGET_MYTHROAD

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <setjmp.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <sys/timeb.h>
#include <io.h> /* open, close etc. */
#include <direct.h> /* getcwd */
#define inline __inline
#define inp next_inp
#ifdef LIBTCC_AS_DLL
# define LIBTCCAPI __declspec(dllexport)
# define PUB_FUNC LIBTCCAPI
#endif
#endif

#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#include <sys/ucontext.h>
#include <sys/mman.h>
#include <dlfcn.h>
#endif

#else  /* TCC_TARGET_MYTHROAD */

#include <mrc_base.h>
#include <mrc_exb.h>

#include <setjmp.h>

#ifdef INCLUDE_STDARG
#include <stdarg.h>
#else   /* !INCLUDE_STDARG */
typedef char* va_list;
#ifndef __va_rounded_size
#define __va_rounded_size(TYPE) (((sizeof(TYPE)+sizeof(int)-1)/sizeof(int))*sizeof(int))
#endif

#ifndef va_start
#define va_start(AP, LASTARG)   (AP = ((char *)& (LASTARG) + __va_rounded_size(LASTARG)))
#endif

#ifndef va_arg
#define va_arg(AP, TYPE)        (AP += __va_rounded_size(TYPE), *((TYPE *)(AP - __va_rounded_size(TYPE))))
#endif

#ifndef va_end
#define va_end(AP)              (AP = (va_list)0 )
#endif
#endif   /* !INCLUDE_STDARG */

#ifdef SDK_MOD
#include <io.h>
#define open _open
#define close _close
#define read _read
#define write _write
#define unlink remove
#define lseek _lseek
#define tell _tell
#define O_RDONLY 0
#define O_BINARY 0
#define O_WRONLY 0x0001
#define O_CREAT  0x0100
#define O_TRUNC  0x0200

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#else  /* !SDK_MOD */
int32 xq_open(const char *name,uint32 mode);
int32 xq_close(int32 f);
int32 xq_write(int32 f,void *p,uint32 l);
int32 xq_read(int32 f,void *pp,uint32 l);
int32 xq_seek(int32 f,int32 l,int mode);
int32 xq_tell(int32 f);

#define open xq_open
#define close xq_close
#define read xq_read
#define write xq_write
#define unlink mrc_remove
#define lseek xq_seek
#define tell xq_tell
#define O_RDONLY MR_FILE_RDONLY
#define O_WRONLY MR_FILE_WRONLY
#define O_CREAT  MR_FILE_CREATE
#define O_BINARY 0
#define O_TRUNC  0

#define SEEK_SET MR_SEEK_SET
#define SEEK_CUR MR_SEEK_CUR
#define SEEK_END MR_SEEK_END

#endif  /* !SDK_MOD */

//#undef strchr
//#define strchr(s,c) (char*)mrc_strchr(s,c)
#define strrchr(s,c) mrc_strrchr(s,c) 

#define exit(code) mrc_exit()  // 需要进行其它处理
#define printf o_printf
#define fprintf(...)
#define fputs(str,f)
//#define snprintf(...)  // 已经完全替换为sprintf
#define getenv(aaa) "MRPGCMAP"

#define CONFIG_TCC_ELFINTERP ""

#define inline __inline
typedef unsigned int size_t;

/* ------------ patch.c ------------ */
extern int errno;



unsigned long long my_strtoull(const char *nptr, char **endptr, int base);
#define strtoull my_strtoull

long my_strtol(const char *nptr, char **endptr, int base);
#define strtol  my_strtol

double my_strtod(const char* s, char** endptr);
#define strtod  my_strtod
#define strtof  (float)my_strtod
#define strtold (long double)my_strtod

int my_stricmp(const char *dst, const char *src);
#define stricmp my_stricmp

int my_vsprintf(char *buf, const char *fmt, va_list args);
#define vsprintf my_vsprintf 

void *my_realloc(void *p, int size);
#define realloc my_realloc

#endif  /* TCC_TARGET_MYTHROAD */

#endif /* !CONFIG_TCCBOOT */

//#undef free
//#define free(p) use_tcc_free(p)
//#undef malloc
//#define malloc(s) use_tcc_malloc(s)
//#define realloc(p, s) use_tcc_realloc(p, s)
//#undef strdup
//#define strdup(s) use_tcc_strdup(s)

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef SA_SIGINFO
#define SA_SIGINFO 0x00000004u
#endif

#include "elf.h"
#include "stab.h"
#include "libtcc.h"

/* parser debug */
//#define PARSE_DEBUG
/* preprocessor debug */
//#define PP_DEBUG
/* include file debug */
//#define INC_DEBUG

//#define MEM_DEBUG

/* assembler debug */
//#define ASM_DEBUG

/* target selection */
//#define TCC_TARGET_I386   /* i386 code generator */
//#define TCC_TARGET_ARM    /* ARMv4 code generator */
//#define TCC_TARGET_C67    /* TMS320C67xx code generator */
//#define TCC_TARGET_X86_64 /* x86-64 code generator */

/* target address type */
# define uplong unsigned long

/* ------------ path configuration ------------ */

#ifndef CONFIG_SYSROOT
# define CONFIG_SYSROOT ""
#endif

#ifdef CONFIG_MULTIARCHDIR
# define CONFIG_LDDIR "lib/" CONFIG_MULTIARCHDIR
#endif

#ifndef CONFIG_LDDIR
# define CONFIG_LDDIR "lib"
#endif

/*路径找到crt1。o,crti。o和crtn.o*/
#ifndef CONFIG_TCC_CRTPREFIX
# define CONFIG_TCC_CRTPREFIX CONFIG_SYSROOT "/usr/" CONFIG_LDDIR
#endif

/* 通过配置如下:{ B }替换TCCDIR计划。- b选项 */

/* 系统包括路径 */
#ifndef CONFIG_TCC_SYSINCLUDEPATHS
# ifdef TCC_TARGET_PE
#  define CONFIG_TCC_SYSINCLUDEPATHS "{B}/include;{B}/include/winapi"
# elif defined CONFIG_MULTIARCHDIR
#  define CONFIG_TCC_SYSINCLUDEPATHS \
        CONFIG_SYSROOT "/usr/local/include" \
    ":" CONFIG_SYSROOT "/usr/local/include/" CONFIG_MULTIARCHDIR \
    ":" CONFIG_SYSROOT "/usr/include" \
    ":" CONFIG_SYSROOT "/usr/include/" CONFIG_MULTIARCHDIR \
    ":" "{B}/include"
# else
#  define CONFIG_TCC_SYSINCLUDEPATHS \
        CONFIG_SYSROOT "/usr/local/include" \
    ":" CONFIG_SYSROOT "/usr/include" \
    ":" "{B}/include"
# endif
#endif

/* library search paths */
#ifndef CONFIG_TCC_LIBPATHS
# ifdef TCC_TARGET_PE
#  define CONFIG_TCC_LIBPATHS "{B}/lib"
# else
#  define CONFIG_TCC_LIBPATHS \
        CONFIG_SYSROOT "/usr/" CONFIG_LDDIR \
    ":" CONFIG_SYSROOT "/" CONFIG_LDDIR \
    ":" CONFIG_SYSROOT "/usr/local/" CONFIG_LDDIR
# endif
#endif


//#if defined(_WIN32)
//#define snprintf _snprintf
//#define vsnprintf _vsnprintf
//#ifndef __GNUC__
//  #define strtold (long double)strtod
//  #define strtof (float)strtod
//  #define strtoll _strtoi64
//  #define strtoull _strtoui64
//#endif
//#else
///* XXX: need to define this to use them in non ISOC99 context */
//extern float strtof (const char *__nptr, char **__endptr);
//extern long double strtold (const char *__nptr, char **__endptr);
//#endif

#if defined(_WIN32) || 1  // mrp上和win32一样处理
#define IS_DIRSEP(c) (c == '/' || c == '\\')
#define IS_ABSPATH(p) (IS_DIRSEP(p[0]) || (p[0] && p[1] == ':' && IS_DIRSEP(p[2])))
#define PATHCMP stricmp
#else
#define IS_DIRSEP(c) (c == '/')
#define IS_ABSPATH(p) IS_DIRSEP(p[0])
#define PATHCMP strcmp
#endif

#define PATHSEP ':'

/* -------------------------------------------- */

#define FALSE 0
#define false 0
#define TRUE 1
#define true 1

//#ifndef　BOOL
//typedef int BOOL;
//#endif


#define INCLUDE_STACK_SIZE  32
#define IFDEF_STACK_SIZE    64
#define VSTACK_SIZE         256
#define STRING_MAX_SIZE     1024
#define PACK_STACK_SIZE     8

#define TOK_HASH_SIZE       8192 /* 必须是2的幂,*/
#define TOK_ALLOC_INCR      512  /* 必须是2的幂, */
#define TOK_MAX_SIZE        4 /* 令牌int马克斯大小单位当存储在字符串*/

/* token symbol management */
typedef struct TokenSym {
    struct TokenSym *hash_next;
    struct Sym *sym_define; /* 直接指针定义 */
    struct Sym *sym_label; /* 直接指向标签 */
    struct Sym *sym_struct; /* 直接指向结构的指针 */
    struct Sym *sym_identifier; /* 直接指针标识符 */
    int tok; /* 数字r */
    int len;
    char str[1];
} TokenSym;

#ifdef TCC_TARGET_PE
typedef unsigned short nwchar_t;
#else
typedef int nwchar_t;
#endif

typedef struct CString {
	int size; /* 文件大小的字节数 */
    void *data; /* either 'char *' 或 'nwchar_t *' */
    int size_allocated;
    void *data_allocated; /* 如果不为NULL,数据已经been malloced */
} CString;

/*类型定义 */
typedef struct CType {
    int t;
    struct Sym *ref;
} CType;

/* 常量值 */
typedef union CValue {
    long double ld;
    double d;
    float f;
    int i;
    unsigned int ui;
    unsigned int ul; /* address (should be unsigned long on 64 bit cpu) */
    long long ll;
    unsigned long long ull;
    struct CString *cstr;
    void *ptr;
    int tab[2];
} CValue;

/* 值栈 */
typedef struct SValue {
    CType type;      /* 类型 */
    unsigned short r;      /* 注册+旗帜 */
    unsigned short r2;     /* 注册+第二寄存器,用于 'long long'
                              类型。如果不习惯,设置为 VT_CONST */
	CValue c;              /* 常数,如果 VT_CONST */
    struct Sym *sym;       /* 符号，如果(VT_SYM | VT_CONST) */
} SValue;

/* 符号 管理 */
typedef struct Sym {
    int v;    /* 字符记号 */
    char *asm_label;    /* asm相关标签 */
    long r;    /* 相关登记 */
#if 0  // ADS1.2不支持匿名union
    union {
        long c;    /* associated number */
        int *d;   /* define token stream */
    };
    union {
        struct Sym *next; /* next related symbol */
        long jnext; /* next jump label */
    };
#else
    union {
        long c;    /*联结数 */
        int *d;   /* 定义标记流 */
    } cd;
    union {
        struct Sym *next; /* 下一个相关的符号 */
        long jnext; /* 下一个标签 */
    } nj;
#endif
    CType type;    /* 关联类型 */
    struct Sym *prev; /* prev符号在堆栈 */
    struct Sym *prev_tok; /* 之前这个符号 */
} Sym;

/* section 定义 */
/* XXX:直接使用精灵结构参数？ */
/* 特殊标志表明,部分不应该联系 */
#define SHF_PRIVATE 0x80000000

/* 特殊标志,也 */
#define SECTION_ABS ((void *)1)

typedef struct Section {
    unsigned long data_offset; /* current data offset */
    unsigned char *data;       /* section data */
    unsigned long data_allocated; /* used for realloc() handling */
    int sh_name;             /* elf section name (only used during output) */
    int sh_num;              /* elf section number */
    int sh_type;             /* elf section type */
    int sh_flags;            /* elf section flags */
    int sh_info;             /* elf section info */
    int sh_addralign;        /* elf section alignment */
    int sh_entsize;          /* elf entry size */
    unsigned long sh_size;   /* section size (only used during output) */
    uplong sh_addr;          /* address at which the section is relocated */
    unsigned long sh_offset; /* file offset */
    int nb_hashed_syms;      /* used to resize the hash table */
    struct Section *link;    /* link to another section */
    struct Section *reloc;   /* corresponding section for relocation, if any */
    struct Section *hash;     /* hash table for symbols */
    struct Section *next;
    char name[1];           /* section name */
} Section;

typedef struct DLLReference {
    int level;
    void *handle;
    char name[1];
} DLLReference;

/* GNUC attribute definition */
typedef struct AttributeDef {
    unsigned
      func_call     : 3, /* calling convention (0..5), see below */
      aligned       : 5, /* alignement (0..16) */
      packed        : 1,
      func_export   : 1,
      func_import   : 1,
      func_args     : 5,
      mode          : 4,
      weak          : 1,
      fill          : 11;
    struct Section *section;
    int alias_target;    /* token */
} AttributeDef;

/* gr: wrappers for casting sym->r for other purposes */
#define FUNC_CALL(r) (((AttributeDef*)&(r))->func_call)
#define FUNC_EXPORT(r) (((AttributeDef*)&(r))->func_export)
#define FUNC_IMPORT(r) (((AttributeDef*)&(r))->func_import)
#define FUNC_ARGS(r) (((AttributeDef*)&(r))->func_args)
#define FUNC_ALIGN(r) (((AttributeDef*)&(r))->aligned)
#define FUNC_PACKED(r) (((AttributeDef*)&(r))->packed)
#define ATTR_MODE(r)  (((AttributeDef*)&(r))->mode)
#define INT_ATTR(ad) (*(int*)(ad))

/* -------------------------------------------------- */

#define SYM_STRUCT     0x40000000 /* struct/union/enum symbol space */
#define SYM_FIELD      0x20000000 /* struct/union field symbol space */
#define SYM_FIRST_ANOM 0x10000000 /* first anonymous sym */

/* stored in 'Sym.c' field */
#define FUNC_NEW       1 /* ansi function prototype */
#define FUNC_OLD       2 /* old function prototype */
#define FUNC_ELLIPSIS  3 /* ansi function prototype with ... */

/* stored in 'Sym.r' field */
#define FUNC_CDECL     0 /* standard c call */
#define FUNC_STDCALL   1 /* pascal c call */
#define FUNC_FASTCALL1 2 /* first param in %eax */
#define FUNC_FASTCALL2 3 /* first parameters in %eax, %edx */
#define FUNC_FASTCALL3 4 /* first parameter in %eax, %edx, %ecx */
#define FUNC_FASTCALLW 5 /* first parameter in %ecx, %edx */

/* field 'Sym.t' for macros */
#define MACRO_OBJ      0 /* object like macro */
#define MACRO_FUNC     1 /* function like macro */

/* field 'Sym.r' for C labels */
#define LABEL_DEFINED  0 /* label is defined */
#define LABEL_FORWARD  1 /* label is forward defined */
#define LABEL_DECLARED 2 /* label is declared but never used */

/* type_decl() types */
#define TYPE_ABSTRACT  1 /* type without variable */
#define TYPE_DIRECT    2 /* type with variable */

#define IO_BUF_SIZE 8192

typedef struct BufferedFile {
    uint8_t *buf_ptr;
    uint8_t *buf_end;
    int fd;
    struct BufferedFile *prev;
    int line_num;    /* current line number - here to simplify code */
    int ifndef_macro;  /* #ifndef macro / #endif search */
    int ifndef_macro_saved; /* saved ifndef_macro */
    int *ifdef_stack_ptr; /* ifdef_stack value at the start of the file */
    char inc_type;          /* type of include */
    char inc_filename[260]; /* filename specified by the user */
    char filename[260];    /* current filename - here to simplify code */
    unsigned char buffer[IO_BUF_SIZE + 1]; /* extra size for CH_EOB char */
} BufferedFile;

#define CH_EOB   '\\'       /* end of buffer or '\0' char in file */
#define CH_EOF   (-1)   /* end of file */

/* parsing state (used to save parser state to reparse part of the
   source several times) */
typedef struct ParseState {
    const int *macro_ptr;
    int line_num;
    int tok;
    CValue tokc;
} ParseState;

/* used to record tokens */
typedef struct TokenString {
    int *str;
    int len;
    int allocated_len;
    int last_line_num;
} TokenString;

/* inline functions */
typedef struct InlineFunc {
    int *token_str;
    Sym *sym;
    char filename[1];
} InlineFunc;

/* include file cache, used to find files faster and also to eliminate
   inclusion if the include file is protected by #ifndef ... #endif */
typedef struct CachedInclude {
    int ifndef_macro;
    int hash_next; /* -1 if none */
    char type; /* '"' or '>' to give include type */
    char filename[1]; /* path specified in #include */
} CachedInclude;

#define CACHED_INCLUDES_HASH_SIZE 512

struct TCCState {
    unsigned output_type    : 8;
    unsigned reloc_output   : 1;
 
    BufferedFile **include_stack_ptr;
    int *ifdef_stack_ptr;

    /* 包括文件处理 */
    char **include_paths;
    int nb_include_paths;
    char **sysinclude_paths;
    int nb_sysinclude_paths;
    CachedInclude **cached_includes;
    int nb_cached_includes;

    char **library_paths;
    int nb_library_paths;
    char **crt_paths;
    int nb_crt_paths;

	/* 所有加载dll的数组(包括引用的加载dll） */
    DLLReference **loaded_dlls;
    int nb_loaded_dlls;

    /* 部分 */
    Section **sections;
	int nb_sections; /* 数量的部分,包括第一个虚拟部分 */

    Section **priv_sections;
    int nb_priv_sections; /* 数量的私有部分 */

    /* 得到了处理 */
    Section *got;
    Section *plt;
    unsigned long *got_offsets;
    int nb_got_offsets;
    /* 给dynsym symtab索引的相关的索引*/
    int *symtab_to_dynsym;

    /* 临时动态符号部分(dll加载) */
    Section *dynsymtab_section;
    /* exported dynamic symbol section */
    Section *dynsym;

    /* copy of the gobal symtab_section variable */
    Section *symtab;

    int nostdinc; /* if true, no standard headers are added */
    int nostdlib; /* if true, no standard libraries are added */
    int nocommon; /* if true, do not use common symbols for .bss data */

    /* if true, static linking is performed */
    int static_link;

    /* soname as specified on the command line (-soname) */
    const char *soname;
    /* rpath as specified on the command line (-Wl,-rpath=) */
    const char *rpath;

    /* if true, all symbols are exported */
    int rdynamic;

    /* if true, resolve symbols in the current module first (-Wl,Bsymbolic) */
    int symbolic;

    /* if true, only link in referenced objects from archive */
    int alacarte_link;

    /* address of text section */
    uplong text_addr;
    int has_text_addr;

    /* symbols to call at load-time / unload-time */
    const char *init_symbol;
    const char *fini_symbol;
    
    /* output format, see TCC_OUTPUT_FORMAT_xxx */
    int output_format;

    /* C language options */
    int char_is_unsigned;
    int leading_underscore;
    
    /* warning switches */
    int warn_write_strings;
    int warn_unsupported;
    int warn_error;
    int warn_none;
    int warn_implicit_function_declaration;

    /* display some information during compilation */
    int verbose;
    /* compile with debug symbol (and use them if error during execution) */
    int do_debug;
#ifdef CONFIG_TCC_BCHECK
    /* compile with built-in memory and bounds checker */
    int do_bounds_check;
#endif
    /*函数库的路径*/
    char *tcc_lib_path;

    /* error handling */
    void *error_opaque;
    void (*error_func)(void *opaque, const char *msg);
    int error_set_jmp_enabled;
    jmp_buf error_jmp_buf;
    int nb_errors;

    /* tiny assembler state */
    Sym *asm_labels;

    /* see include_stack_ptr */
    BufferedFile *include_stack[INCLUDE_STACK_SIZE];

    /* see ifdef_stack_ptr */
    int ifdef_stack[IFDEF_STACK_SIZE];

    /* see cached_includes */
    int cached_includes_hash[CACHED_INCLUDES_HASH_SIZE];

    /* pack stack */
    int pack_stack[PACK_STACK_SIZE];
    int *pack_stack_ptr;

    /* output file for preprocessing */
    int32 outfile;

    /* automatically collected dependencies for this compilation */
    char **target_deps;
    int nb_target_deps;

    /* for tcc_relocate */
    int runtime_added;
    void *runtime_mem;

    struct InlineFunc **inline_fns;
    int nb_inline_fns;

    /* section alignment */
    unsigned long section_align;

#if defined TCC_TARGET_ARM
    /* write PLT and GOT here */
    char *runtime_plt_and_got;
    unsigned int runtime_plt_and_got_offset;
#endif
};

/* The current value can be: */
#define VT_VALMASK   0x003f
#define VT_CONST     0x0030  /* constant in vc
                              (must be first non register value) */
#define VT_LLOCAL    0x0031  /* lvalue, offset on stack */
#define VT_LOCAL     0x0032  /* offset on stack */
#define VT_CMP       0x0033  /* the value is stored in processor flags (in vc) */
#define VT_JMP       0x0034  /* value is the consequence of jmp true (even) */
#define VT_JMPI      0x0035  /* value is the consequence of jmp false (odd) */
#define VT_REF       0x0040  /* value is pointer to structure rather than address */
#define VT_LVAL      0x0100  /* var is an lvalue */
#define VT_SYM       0x0200  /* a symbol value is added */
#define VT_MUSTCAST  0x0400  /* value must be casted to be correct (used for
                                char/short stored in integer registers) */
#define VT_MUSTBOUND 0x0800  /* bound checking must be done before
                                dereferencing value */
#define VT_BOUNDED   0x8000  /* value is bounded. The address of the
                                bounding function call point is in vc */
#define VT_LVAL_BYTE     0x1000  /* lvalue is a byte */
#define VT_LVAL_SHORT    0x2000  /* lvalue is a short */
#define VT_LVAL_UNSIGNED 0x4000  /* lvalue is unsigned */
#define VT_LVAL_TYPE     (VT_LVAL_BYTE | VT_LVAL_SHORT | VT_LVAL_UNSIGNED)

/* types */
#define VT_INT        0  /* integer type */
#define VT_BYTE       1  /* signed byte type */
#define VT_SHORT      2  /* short type */
#define VT_VOID       3  /* void type */
#define VT_PTR        4  /* pointer */
#define VT_ENUM       5  /* enum definition */
#define VT_FUNC       6  /* function type */
#define VT_STRUCT     7  /* struct/union definition */
#define VT_FLOAT      8  /* IEEE float */
#define VT_DOUBLE     9  /* IEEE double */
#define VT_LDOUBLE   10  /* IEEE long double */
#define VT_BOOL      11  /* ISOC99 boolean type */
#define VT_LLONG     12  /* 64 bit integer */
#define VT_LONG      13  /* long integer (NEVER USED as type, only
                            during parsing) */
#define VT_BTYPE      0x000f /* mask for basic type */
#define VT_UNSIGNED   0x0010  /* unsigned type */
#define VT_ARRAY      0x0020  /* array type (also has VT_PTR) */
#define VT_VLA        0x20000 /* VLA type (also has VT_PTR and VT_ARRAY) */
#define VT_BITFIELD   0x0040  /* bitfield modifier */
#define VT_CONSTANT   0x0800  /* const modifier */
#define VT_VOLATILE   0x1000  /* volatile modifier */
#define VT_SIGNED     0x2000  /* signed type */

/* storage */
#define VT_EXTERN  0x00000080  /* extern definition */
#define VT_STATIC  0x00000100  /* static variable */
#define VT_TYPEDEF 0x00000200  /* typedef definition */
#define VT_INLINE  0x00000400  /* inline definition */
#define VT_IMPORT  0x00004000  /* win32: extern data imported from dll */
#define VT_EXPORT  0x00008000  /* win32: data exported from dll */
#define VT_WEAK    0x00010000  /* win32: data exported from dll */

#define VT_STRUCT_SHIFT 18   /* shift for bitfield shift values */

/* type mask (except storage) */
#define VT_STORAGE (VT_EXTERN | VT_STATIC | VT_TYPEDEF | VT_INLINE | VT_IMPORT | VT_EXPORT | VT_WEAK)
#define VT_TYPE    (~(VT_STORAGE))

/* token values */

/* warning: the following compare tokens depend on i386 asm code */
#define TOK_ULT 0x92
#define TOK_UGE 0x93
#define TOK_EQ  0x94
#define TOK_NE  0x95
#define TOK_ULE 0x96
#define TOK_UGT 0x97
#define TOK_Nset 0x98
#define TOK_Nclear 0x99
#define TOK_LT  0x9c
#define TOK_GE  0x9d
#define TOK_LE  0x9e
#define TOK_GT  0x9f

#define TOK_LAND  0xa0
#define TOK_LOR   0xa1

#define TOK_DEC   0xa2
#define TOK_MID   0xa3 /* inc/dec, to void constant */
#define TOK_INC   0xa4
#define TOK_UDIV  0xb0 /* unsigned division */
#define TOK_UMOD  0xb1 /* unsigned modulo */
#define TOK_PDIV  0xb2 /* fast division with undefined rounding for pointers */
#define TOK_CINT   0xb3 /* number in tokc */
#define TOK_CCHAR 0xb4 /* char constant in tokc */
#define TOK_STR   0xb5 /* pointer to string in tokc */
#define TOK_TWOSHARPS 0xb6 /* ## preprocessing token */
#define TOK_LCHAR    0xb7
#define TOK_LSTR     0xb8
#define TOK_CFLOAT   0xb9 /* float constant */
#define TOK_LINENUM  0xba /* line number info */
#define TOK_CDOUBLE  0xc0 /* double constant */
#define TOK_CLDOUBLE 0xc1 /* long double constant */
#define TOK_UMULL    0xc2 /* unsigned 32x32 -> 64 mul */
#define TOK_ADDC1    0xc3 /* add with carry generation */
#define TOK_ADDC2    0xc4 /* add with carry use */
#define TOK_SUBC1    0xc5 /* add with carry generation */
#define TOK_SUBC2    0xc6 /* add with carry use */
#define TOK_CUINT    0xc8 /* unsigned int constant */
#define TOK_CLLONG   0xc9 /* long long constant */
#define TOK_CULLONG  0xca /* unsigned long long constant */
#define TOK_ARROW    0xcb
#define TOK_DOTS     0xcc /* three dots */
#define TOK_SHR      0xcd /* unsigned shift right */
#define TOK_PPNUM    0xce /* preprocessor number */
#define TOK_NOSUBST  0xcf /* means following token has already been pp'd */

#define TOK_SHL   0x01 /* shift left */
#define TOK_SAR   0x02 /* signed shift right */
  
/* assignement operators : normal operator or 0x80 */
#define TOK_A_MOD 0xa5
#define TOK_A_AND 0xa6
#define TOK_A_MUL 0xaa
#define TOK_A_ADD 0xab
#define TOK_A_SUB 0xad
#define TOK_A_DIV 0xaf
#define TOK_A_XOR 0xde
#define TOK_A_OR  0xfc
#define TOK_A_SHL 0x81
#define TOK_A_SAR 0x82

#ifndef offsetof
#define offsetof(type, field) ((size_t) &((type *)0)->field)
#endif

#ifndef countof
#define countof(tab) (sizeof(tab) / sizeof((tab)[0]))
#endif

#define TOK_EOF       (-1)  /* end of file */
#define TOK_LINEFEED  10    /* line feed */

/* all identificators and strings have token above that */
#define TOK_IDENT 256

#define DEF_ASM(x) DEF(TOK_ASM_ ## x, #x)
#define TOK_ASM_int TOK_INT
#define TOK_ASM_weak TOK_WEAK1

enum tcc_token {
    TOK_LAST = TOK_IDENT - 1,
#define DEF(id, str) id,
#include "tcctok.h"
#undef DEF
    TOK_MAX
};

#define TOK_UIDENT TOK_DEFINE

/* space exlcuding newline */
static inline int is_space(int ch)
{
    return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r';
}

static inline int isid(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static inline int isnum(int c)
{
    return c >= '0' && c <= '9';
}

static inline int isoct(int c)
{
    return c >= '0' && c <= '7';
}

static inline int toup(int c)
{
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

#ifndef PUB_FUNC
# define PUB_FUNC
#endif

#ifdef ONE_SOURCE
#define ST_INLN static inline
#define ST_FUNC static
#define ST_DATA static
#else
#define ST_INLN
#define ST_FUNC
#define ST_DATA extern
#endif

/* ------------ libtcc.c ------------ */

/* use GNU C extensions */
ST_DATA int gnu_ext;
/* use Tiny C extensions */
ST_DATA int tcc_ext;
/* XXX: get rid of this ASAP */
ST_DATA struct TCCState *tcc_state;

#define AFF_PRINT_ERROR     0x0001 /* 如果文件未找到打印错误 */
#define AFF_REFERENCED_DLL  0x0002 /* 从另一个dll加载引用dll */
#define AFF_PREPROCESS      0x0004 /* 预处理文件 */

/* 目前使用的公共tcc主要功能 */
PUB_FUNC char *pstrcpy(char *buf, int buf_size, const char *s);
PUB_FUNC char *pstrcat(char *buf, int buf_size, const char *s);
PUB_FUNC char *pstrncpy(char *out, const char *in, size_t num);
PUB_FUNC char *tcc_basename(const char *name);
PUB_FUNC char *tcc_fileextension (const char *name);
PUB_FUNC void tcc_free(void *ptr);
PUB_FUNC void *tcc_malloc(unsigned long size);
PUB_FUNC void *tcc_mallocz(unsigned long size);
PUB_FUNC void *tcc_realloc(void *ptr, unsigned long size);
PUB_FUNC char *tcc_strdup(const char *str);

PUB_FUNC void tcc_memstats(void);
PUB_FUNC void dynarray_add(void ***ptab, int *nb_ptr, void *data);
PUB_FUNC void dynarray_reset(void *pp, int *n);

PUB_FUNC void tcc_error_noabort(const char *fmt, ...);
PUB_FUNC void tcc_error(const char *fmt, ...);
PUB_FUNC void tcc_warning(const char *fmt, ...);

/* other utilities */
PUB_FUNC void cstr_ccat(CString *cstr, int ch);
PUB_FUNC void cstr_cat(CString *cstr, const char *str);
PUB_FUNC void cstr_wccat(CString *cstr, int ch);
PUB_FUNC void cstr_new(CString *cstr);
PUB_FUNC void cstr_free(CString *cstr);
#define cstr_reset(cstr) cstr_free(cstr)

ST_FUNC Section *new_section(TCCState *s1, const char *name, int sh_type, int sh_flags);
ST_FUNC void section_realloc(Section *sec, unsigned long new_size);
ST_FUNC void *section_ptr_add(Section *sec, unsigned long size);
ST_FUNC void section_reserve(Section *sec, unsigned long size);
ST_FUNC Section *find_section(TCCState *s1, const char *name);

ST_FUNC void put_extern_sym2(Sym *sym, Section *section, uplong value, unsigned long size, int can_add_underscore);
ST_FUNC void put_extern_sym(Sym *sym, Section *section, uplong value, unsigned long size);
ST_FUNC void greloc(Section *s, Sym *sym, unsigned long offset, int type);

ST_INLN void sym_free(Sym *sym);
ST_FUNC Sym *sym_push2(Sym **ps, int v, int t, long c);
ST_FUNC Sym *sym_find2(Sym *s, int v);
ST_FUNC Sym *sym_push(int v, CType *type, int r, int c);
ST_FUNC void sym_pop(Sym **ptop, Sym *b);
ST_INLN Sym *struct_find(int v);
ST_INLN Sym *sym_find(int v);
ST_FUNC Sym *global_identifier_push(int v, int t, int c);

ST_FUNC void tcc_open_bf(TCCState *s1, const char *filename, int initlen);
ST_FUNC int tcc_open(TCCState *s1, const char *filename);
ST_FUNC void tcc_close(void);

ST_FUNC int tcc_add_file_internal(TCCState *s1, const char *filename, int flags);
ST_FUNC int tcc_add_crt(TCCState *s, const char *filename);
#ifndef TCC_TARGET_PE
ST_FUNC int tcc_add_dll(TCCState *s, const char *filename, int flags);
#endif

PUB_FUNC int tcc_set_flag(TCCState *s, const char *flag_name, int value);
PUB_FUNC void tcc_print_stats(TCCState *s, int64_t total_time);
PUB_FUNC char *tcc_default_target(TCCState *s, const char *default_file);
PUB_FUNC void tcc_gen_makedeps(TCCState *s, const char *target, const char *filename);

/* ------------ tccpp.c ------------ */

ST_DATA struct BufferedFile *file;
ST_DATA int ch, tok;
ST_DATA CValue tokc;
ST_DATA const int *macro_ptr;
ST_DATA int parse_flags;
ST_DATA int tok_flags;
ST_DATA CString tokcstr; /* current parsed string, if any */

/* display benchmark infos */
ST_DATA int total_lines;
ST_DATA int total_bytes;
ST_DATA int tok_ident;
ST_DATA TokenSym **table_ident;

#define TOK_FLAG_BOL   0x0001 /* beginning of line before */
#define TOK_FLAG_BOF   0x0002 /* beginning of file before */
#define TOK_FLAG_ENDIF 0x0004 /* a endif was found matching starting #ifdef */
#define TOK_FLAG_EOF   0x0008 /* end of file */

#define PARSE_FLAG_PREPROCESS 0x0001 /* activate preprocessing */
#define PARSE_FLAG_TOK_NUM    0x0002 /* return numbers instead of TOK_PPNUM */
#define PARSE_FLAG_LINEFEED   0x0004 /* line feed is returned as a
                                        token. line feed is also
                                        returned at eof */
#define PARSE_FLAG_ASM_COMMENTS 0x0008 /* '#' can be used for line comment */
#define PARSE_FLAG_SPACES     0x0010 /* next() returns space tokens (for -E) */

ST_FUNC TokenSym *tok_alloc(const char *str, int len);
ST_FUNC char *get_tok_str(int v, CValue *cv);
ST_FUNC void save_parse_state(ParseState *s);
ST_FUNC void restore_parse_state(ParseState *s);
ST_INLN void tok_str_new(TokenString *s);
ST_FUNC void tok_str_free(int *str);
ST_FUNC void tok_str_add(TokenString *s, int t);
ST_FUNC void tok_str_add_tok(TokenString *s);
ST_INLN void define_push(int v, int macro_type, int *str, Sym *first_arg);
ST_FUNC void define_undef(Sym *s);
ST_INLN Sym *define_find(int v);
ST_FUNC void free_defines(Sym *b);
ST_FUNC Sym *label_find(int v);
ST_FUNC Sym *label_push(Sym **ptop, int v, int flags);
ST_FUNC void label_pop(Sym **ptop, Sym *slast);
ST_FUNC void parse_define(void);
ST_FUNC void preprocess(int is_bof);
ST_FUNC void next_nomacro(void);
ST_FUNC void next(void);
ST_INLN void unget_tok(int last_tok);
ST_FUNC void preprocess_init(TCCState *s1);
ST_FUNC void preprocess_new(void);
ST_FUNC int tcc_preprocess(TCCState *s1);
ST_FUNC void skip(int c);
ST_FUNC void expect(const char *msg);

/* ------------ tccgen.c ------------ */

ST_DATA Section *text_section, *data_section, *bss_section; /* predefined sections */
ST_DATA Section *cur_text_section; /* current section where function code is generated */
#ifdef CONFIG_TCC_BCHECK
/* bound check related sections */
ST_DATA Section *bounds_section; /* contains global data bound description */
ST_DATA Section *lbounds_section; /* contains local data bound description */
#endif
/* symbol sections */
ST_DATA Section *symtab_section, *strtab_section;
/* debug sections */
ST_DATA Section *stab_section, *stabstr_section;

#define SYM_POOL_NB (8192 / sizeof(Sym))
ST_DATA Sym *sym_free_first;
ST_DATA void **sym_pools;
ST_DATA int nb_sym_pools;

ST_DATA Sym *global_stack;
ST_DATA Sym *local_stack;
ST_DATA Sym *local_label_stack;
ST_DATA Sym *global_label_stack;
ST_DATA Sym *define_stack;
ST_DATA CType char_pointer_type, func_old_type, int_type, size_type;
ST_DATA SValue vstack[VSTACK_SIZE], *vtop;
ST_DATA int rsym, anon_sym, ind, loc;

ST_DATA int const_wanted; /* true if constant wanted */
ST_DATA int nocode_wanted; /* true if no code generation wanted for an expression */
ST_DATA int global_expr;  /* true if compound literals must be allocated globally (used during initializers parsing */
ST_DATA CType func_vt; /* current function return type (used by return instruction) */
ST_DATA int func_vc;
ST_DATA int last_line_num, last_ind, func_ind; /* debug last line number and pc */
ST_DATA char *funcname;

ST_INLN int is_float(int t);
ST_FUNC int ieee_finite(double d);
ST_FUNC void test_lvalue(void);
ST_FUNC void swap(int *p, int *q);
ST_FUNC void vpushi(int v);
ST_FUNC Sym *external_global_sym(int v, CType *type, int r);
ST_FUNC void vset(CType *type, int r, int v);
ST_FUNC void vswap(void);
ST_FUNC void vpush_global_sym(CType *type, int v);
ST_FUNC void vrote(SValue *e, int n);
ST_FUNC void vrott(int n);
ST_FUNC void vrotb(int n);
#ifdef TCC_TARGET_ARM
ST_FUNC int get_reg_ex(int rc, int rc2);
ST_FUNC void lexpand_nr(void);
#endif
ST_FUNC void vpushv(SValue *v);
ST_FUNC void save_reg(int r);
ST_FUNC int get_reg(int rc);
ST_FUNC void save_regs(int n);
ST_FUNC int gv(int rc);
ST_FUNC void gv2(int rc1, int rc2);
ST_FUNC void vpop(void);
ST_FUNC void gen_op(int op);
ST_FUNC int type_size(CType *type, int *a);
ST_FUNC void mk_pointer(CType *type);
ST_FUNC void vstore(void);
ST_FUNC void inc(int post, int c);
ST_FUNC void parse_asm_str(CString *astr);
ST_FUNC int lvalue_type(int t);
ST_FUNC void indir(void);
ST_FUNC void unary(void);
ST_FUNC void expr_prod(void);
ST_FUNC void expr_sum(void);
ST_FUNC void gexpr(void);
ST_FUNC int expr_const(void);
ST_FUNC void gen_inline_functions(void);
ST_FUNC void decl(int l);
#if defined CONFIG_TCC_BCHECK
ST_FUNC Sym *get_sym_ref(CType *type, Section *sec, unsigned long offset, unsigned long size);
#endif

/* ------------ tccelf.c ------------ */

#define ARMAG  "!<arch>\012"    /* For COFF and a.out archives */

typedef struct {
    unsigned int n_strx;         /* index into string table of name */
    unsigned char n_type;         /* type of symbol */
    unsigned char n_other;        /* misc info (usually empty) */
    unsigned short n_desc;        /* description field */
    unsigned int n_value;        /* value of symbol */
} Stab_Sym;

ST_FUNC Section *new_symtab(TCCState *s1, const char *symtab_name, int sh_type, int sh_flags, const char *strtab_name, const char *hash_name, int hash_sh_flags);

ST_FUNC int put_elf_str(Section *s, const char *sym);
ST_FUNC int put_elf_sym(Section *s, uplong value, unsigned long size, int info, int other, int shndx, const char *name);
ST_FUNC int add_elf_sym(Section *s, uplong value, unsigned long size, int info, int other, int sh_num, const char *name);
ST_FUNC int find_elf_sym(Section *s, const char *name);
ST_FUNC void put_elf_reloc(Section *symtab, Section *s, unsigned long offset, int type, int symbol);

ST_FUNC void put_stabs(const char *str, int type, int other, int desc, unsigned long value);
ST_FUNC void put_stabs_r(const char *str, int type, int other, int desc, unsigned long value, Section *sec, int sym_index);
ST_FUNC void put_stabn(int type, int other, int desc, int value);
ST_FUNC void put_stabd(int type, int other, int desc);

ST_FUNC void relocate_common_syms(void);
ST_FUNC void relocate_syms(TCCState *s1, int do_resolve);
ST_FUNC void relocate_section(TCCState *s1, Section *s);

ST_FUNC void tcc_add_linker_symbols(TCCState *s1);
ST_FUNC int tcc_load_object_file(TCCState *s1, int fd, unsigned long file_offset);
ST_FUNC int tcc_load_archive(TCCState *s1, int fd);
ST_FUNC void *tcc_get_symbol_err(TCCState *s, const char *name);
ST_FUNC void tcc_add_bcheck(TCCState *s1);

ST_FUNC void build_got_entries(TCCState *s1);
ST_FUNC void tcc_add_runtime(TCCState *s1);

ST_FUNC int tcc_load_dll(TCCState *s1, int fd, const char *filename, int level);
ST_FUNC int tcc_load_ldscript(TCCState *s1);
ST_FUNC uint8_t *parse_comment(uint8_t *p);
ST_FUNC void minp(void);
ST_INLN void inp(void);
ST_FUNC int handle_eob(void);

# define ELFCLASSW ELFCLASS32
# define ElfW(type) Elf##32##_##type
# define ELFW(type) ELF##32##_##type
# define ElfW_Rel ElfW(Rel)
# define SHT_RELX SHT_REL
# define REL_SECTION_FMT ".rel%s"

/* ------------ xxx-gen.c ------------ */

ST_FUNC void gsym_addr(int t, int a);
ST_FUNC void gsym(int t);
ST_FUNC void load(int r, SValue *sv);
ST_FUNC void store(int r, SValue *v);
ST_FUNC void gfunc_call(int nb_args);
ST_FUNC void gfunc_prolog(CType *func_type);
ST_FUNC void gfunc_epilog(void);
ST_FUNC int gjmp(int t);
ST_FUNC void gjmp_addr(int a);
ST_FUNC int gtst(int inv, int t);
ST_FUNC void gen_opi(int op);
ST_FUNC void gen_opf(int op);
ST_FUNC void gen_cvt_ftoi(int t);
ST_FUNC void gen_cvt_ftof(int t);
ST_FUNC void ggoto(void);
ST_FUNC void o(unsigned int c);

#ifndef TCC_TARGET_ARM
ST_FUNC void gen_cvt_itof(int t);
#endif

/* ------------ arm-gen.c ------------ */
#ifdef TCC_TARGET_ARM
ST_FUNC uint32_t encbranch(int pos, int addr, int fail);
ST_FUNC void gen_cvt_itof1(int t);
#endif

/* ------------ tccrun.c ----------------- */
#ifdef CONFIG_TCC_STATIC
#define RTLD_LAZY       0x001
#define RTLD_NOW        0x002
#define RTLD_GLOBAL     0x100
#define RTLD_DEFAULT    NULL
/* dummy function for profiling */
ST_FUNC void *dlopen(const char *filename, int flag);
ST_FUNC void dlclose(void *p);
//ST_FUNC const char *dlerror(void);
ST_FUNC void *resolve_sym(TCCState *s1, const char *symbol);
#elif !defined TCC_TARGET_PE || !defined _WIN32
ST_FUNC void *resolve_sym(TCCState *s1, const char *symbol);
#endif

/********************************************************/
/* include the target specific definitions */

//#define TARGET_DEFS_ONLY
#ifdef TCC_TARGET_ARM
#include "arm-gen.h"
#endif
//#undef TARGET_DEFS_ONLY

ST_DATA const int reg_classes[];

/********************************************************/
#undef ST_DATA
#ifdef ONE_SOURCE
#define ST_DATA static
#else
#define ST_DATA
#endif
/********************************************************/
#endif /* _TCC_H */
