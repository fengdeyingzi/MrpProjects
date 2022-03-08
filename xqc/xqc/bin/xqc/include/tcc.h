#ifndef HCC_H
#define HCC_H

#ifndef LIBTCCAPI
# define LIBTCCAPI
#endif

	/* ����һ����TCC���뻷�� */
	LIBTCCAPI void *tcc_new(void);

	/*�ͷ� TCC ���뻷�� */
	LIBTCCAPI void tcc_delete(void *s);

	/*�����ɵĴ�������ӵ�����Ϣ */
	LIBTCCAPI void tcc_enable_debug(void *s);

	/*���ô���/������ʾ�ص� */
	LIBTCCAPI void tcc_set_error_func(void *s, void *error_opaque,
		void (*error_func)(void *opaque, const char *msg));

	/* ����/����һ������ */
	LIBTCCAPI int tcc_set_warning(void *s, const char *warning_name, int value);

	/* ����������ѡ�� */
	LIBTCCAPI const char * tcc_set_linker(void *s, char *option, int multi);

	/*****************************/
	/* Ԥ������ */

	/* ��Ӱ���·�� */
	LIBTCCAPI int tcc_add_include_path(void *s, const char *pathname);

	/* ����ϵͳ����·�� */
	LIBTCCAPI int tcc_add_sysinclude_path(void *s, const char *pathname);

	/* Ԥ������������ŵ����������԰ѿ�ѡֵ */
	LIBTCCAPI void tcc_define_symbol(void *s, const char *sym, const char *value);

	/* δ����Ԥ������� 'sym' */
	LIBTCCAPI void tcc_undefine_symbol(void *s, const char *sym);

	/*****************************/
	/* ���� */

	/* ���һ���ļ�(C�ļ�,dll,һ������,һ�������һ��ld�ű�)��������󷵻�-1 */
	LIBTCCAPI int tcc_add_file(void *s, const char *filename);

	/* ����һ���ַ�������һ��CԴ���롣������󷵻ط��㡣 */
	LIBTCCAPI int tcc_compile_string(void *s, const char *buf);

	/*****************************/
	/* �������� */

	/* ����������͡�֮ǰ���뱻��Ϊ������ */
#define TCC_OUTPUT_MEMORY   0 /* ������������ڴ���(û������ļ�)(Ĭ�� */
#define TCC_OUTPUT_EXE      1 /* ��ִ���ļ� */
#define TCC_OUTPUT_DLL      2 /* ��̬�� */
#define TCC_OUTPUT_OBJ      3 /* Ŀ���ļ� */
#define TCC_OUTPUT_PREPROCESS 4 /* Ԥ�����ļ�(�ڲ�ʹ�� */
	LIBTCCAPI int tcc_set_output_type(void *s, int output_type);

#define TCC_OUTPUT_FORMAT_ELF    0 /* Ĭ�ϵ������ʽ ELF */
#define TCC_OUTPUT_FORMAT_BINARY 1 /* ������ͼ�����*/
#define TCC_OUTPUT_FORMAT_COFF   2 /* ͨ�ö����ļ���ʽCOFF */

	/* �൱��-Lpathѡ�� */
	LIBTCCAPI int tcc_add_library_path(void *s, const char *pathname);

	/* the library ������һ����- l��ѡ��Ĳ��� */
	LIBTCCAPI int tcc_add_library(void *s, const char *libraryname);

	/*������ӵ������ĳ��� */
	LIBTCCAPI int tcc_add_symbol(void *s, const char *name, const void *val);

	/* �����ִ�п������ļ���������tcc_relocate(). */
	LIBTCCAPI int tcc_output_file(void *s, const char *filename);

	/* ���Ӻ�����main()������������ֵ��������tcc_relocate() */
	LIBTCCAPI int tcc_run(void *s, int argc, char **argv);

	/* �����а�Ǩ (��Ҫʹ�� tcc_get_symbol()) ���󷵻�-1 */
	LIBTCCAPI int tcc_relocate(void *s1);

	/* ��Ǩ�Ĵ��롣����-1����,��Ҫ��С���ptrΪNULL,���򽫴��븴�Ƶ�����ͨ�������� */
	int tcc_relocate_ex(void *s1, void *ptr);

	/* ���û���ҵ����ط���ֵ��NULL */
	LIBTCCAPI void *tcc_get_symbol(void *s, const char *name);

	/* ������ʱ��������TCCDIR */
	LIBTCCAPI void tcc_set_lib_path(void *s, const char *path);

	/* ��ȡ��ǰ���еĳ���ָ�� */
	LIBTCCAPI void *GetMainsf(void);


#endif
