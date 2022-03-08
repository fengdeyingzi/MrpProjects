#include <tcc.h>
#include <math.h>


int32 l_math_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"sin", (void*)sin);
	tcc_add_symbol(s,"cos", (void*)cos);
	tcc_add_symbol(s,"tan", (void*)tan);
	tcc_add_symbol(s,"asin", (void*)asin);
	tcc_add_symbol(s,"acos", (void*)acos);
	tcc_add_symbol(s,"atan", (void*)atan);
	tcc_add_symbol(s,"atan2", (void*)atan2);
	tcc_add_symbol(s,"sinh", (void*)sinh);
	tcc_add_symbol(s,"cosh", (void*)cosh);
	tcc_add_symbol(s,"tanh", (void*)tanh);
	tcc_add_symbol(s,"exp", (void*)exp);
	tcc_add_symbol(s,"fabs", (void*)fabs);
	tcc_add_symbol(s,"fmod", (void*)fmod);
	tcc_add_symbol(s,"frexp", (void*)frexp);
	tcc_add_symbol(s,"ldexp", (void*)ldexp);
	tcc_add_symbol(s,"log", (void*)log);
	tcc_add_symbol(s,"log10", (void*)log10);
	tcc_add_symbol(s,"modf", (void*)modf);
	tcc_add_symbol(s,"pow", (void*)pow);
	tcc_add_symbol(s,"sqrt", (void*)sqrt);
	tcc_add_symbol(s,"ceil", (void*)ceil);
	tcc_add_symbol(s,"floor", (void*)floor);
	tcc_add_symbol(s,"hypot", (void*)hypot);
	//tcc_add_symbol(s,"abs", (void*)abs);


	return 0;
}