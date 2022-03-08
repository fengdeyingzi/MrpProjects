#include <tcc.h>

extern TCCState *mainsf;//运行主程序链接库指针

TCCState *GetMainsf()
{
	return mainsf;
}

int32 l_tcc_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"tcc_new", (void*)tcc_new);
	tcc_add_symbol(s,"tcc_delete", (void*)tcc_delete);
	tcc_add_symbol(s,"tcc_enable_debug", (void*)tcc_enable_debug);
	tcc_add_symbol(s,"tcc_set_error_func", (void*)tcc_set_error_func);
	tcc_add_symbol(s,"tcc_set_warning", (void*)tcc_set_warning);
	tcc_add_symbol(s,"tcc_set_linker", (void*)tcc_set_linker);
	tcc_add_symbol(s,"tcc_add_include_path", (void*)tcc_add_include_path);
	tcc_add_symbol(s,"tcc_add_sysinclude_path", (void*)tcc_add_sysinclude_path);
	tcc_add_symbol(s,"tcc_define_symbol", (void*)tcc_define_symbol);
	tcc_add_symbol(s,"tcc_undefine_symbol", (void*)tcc_undefine_symbol);
	tcc_add_symbol(s,"tcc_add_file", (void*)tcc_add_file);
	tcc_add_symbol(s,"tcc_compile_string", (void*)tcc_compile_string);
	tcc_add_symbol(s,"tcc_add_library_path", (void*)tcc_add_library_path);
	tcc_add_symbol(s,"tcc_add_library", (void*)tcc_add_library);
	tcc_add_symbol(s,"tcc_add_symbol", (void*)tcc_add_symbol);
	tcc_add_symbol(s,"tcc_output_file", (void*)tcc_output_file);
	tcc_add_symbol(s,"tcc_run", (void*)tcc_run);
	tcc_add_symbol(s,"tcc_relocate", (void*)tcc_relocate);
	tcc_add_symbol(s,"tcc_relocate_ex", (void*)tcc_relocate_ex);
	tcc_add_symbol(s,"tcc_get_symbol", (void*)tcc_get_symbol);
	tcc_add_symbol(s,"tcc_set_lib_path", (void*)tcc_set_lib_path);
	tcc_add_symbol(s,"GetMainsf", (void*)GetMainsf);

	return 0;
}