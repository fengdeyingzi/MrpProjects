#ifndef HCC_H
#define HCC_H

#ifndef LIBTCCAPI
# define LIBTCCAPI
#endif

	/* 创建一个新TCC编译环境 */
	LIBTCCAPI void *tcc_new(void);

	/*释放 TCC 编译环境 */
	LIBTCCAPI void tcc_delete(void *s);

	/*在生成的代码中添加调试信息 */
	LIBTCCAPI void tcc_enable_debug(void *s);

	/*设置错误/警告显示回调 */
	LIBTCCAPI void tcc_set_error_func(void *s, void *error_opaque,
		void (*error_func)(void *opaque, const char *msg));

	/* 设置/重置一个警告 */
	LIBTCCAPI int tcc_set_warning(void *s, const char *warning_name, int value);

	/* 设置链接器选项 */
	LIBTCCAPI const char * tcc_set_linker(void *s, char *option, int multi);

	/*****************************/
	/* 预处理器 */

	/* 添加包含路径 */
	LIBTCCAPI int tcc_add_include_path(void *s, const char *pathname);

	/* 加入系统包括路径 */
	LIBTCCAPI int tcc_add_sysinclude_path(void *s, const char *pathname);

	/* 预处理器定义符号的象征。可以把可选值 */
	LIBTCCAPI void tcc_define_symbol(void *s, const char *sym, const char *value);

	/* 未定义预处理符号 'sym' */
	LIBTCCAPI void tcc_undefine_symbol(void *s, const char *sym);

	/*****************************/
	/* 编译 */

	/* 添加一个文件(C文件,dll,一个对象,一个库或者一个ld脚本)。如果错误返回-1 */
	LIBTCCAPI int tcc_add_file(void *s, const char *filename);

	/* 编译一个字符串包含一个C源代码。如果错误返回非零。 */
	LIBTCCAPI int tcc_compile_string(void *s, const char *buf);

	/*****************************/
	/* 链接命令 */

	/* 设置输出类型。之前必须被称为编译吗 */
#define TCC_OUTPUT_MEMORY   0 /* 输出将运行在内存中(没有输出文件)(默认 */
#define TCC_OUTPUT_EXE      1 /* 可执行文件 */
#define TCC_OUTPUT_DLL      2 /* 动态库 */
#define TCC_OUTPUT_OBJ      3 /* 目标文件 */
#define TCC_OUTPUT_PREPROCESS 4 /* 预处理文件(内部使用 */
	LIBTCCAPI int tcc_set_output_type(void *s, int output_type);

#define TCC_OUTPUT_FORMAT_ELF    0 /* 默认的输出格式 ELF */
#define TCC_OUTPUT_FORMAT_BINARY 1 /* 二进制图像输出*/
#define TCC_OUTPUT_FORMAT_COFF   2 /* 通用对象文件格式COFF */

	/* 相当于-Lpath选项 */
	LIBTCCAPI int tcc_add_library_path(void *s, const char *pathname);

	/* the library 的名字一样“- l”选项的参数 */
	LIBTCCAPI int tcc_add_library(void *s, const char *libraryname);

	/*符号添加到编译后的程序 */
	LIBTCCAPI int tcc_add_symbol(void *s, const char *name, const void *val);

	/* 输出可执行库或对象文件。不调用tcc_relocate(). */
	LIBTCCAPI int tcc_output_file(void *s, const char *filename);

	/* 链接和运行main()函数并返回其值。不调用tcc_relocate() */
	LIBTCCAPI int tcc_run(void *s, int argc, char **argv);

	/* 做所有搬迁 (需要使用 tcc_get_symbol()) 错误返回-1 */
	LIBTCCAPI int tcc_relocate(void *s1);

	/* 搬迁的代码。返回-1错误,需要大小如果ptr为NULL,否则将代码复制到缓冲通过调用者 */
	int tcc_relocate_ex(void *s1, void *ptr);

	/* 如果没有找到返回符号值或NULL */
	LIBTCCAPI void *tcc_get_symbol(void *s, const char *name);

	/* 在运行时设置配置TCCDIR */
	LIBTCCAPI void tcc_set_lib_path(void *s, const char *path);

	/* 获取当前运行的程序指针 */
	LIBTCCAPI void *GetMainsf(void);


#endif
