#include "haning.h"
#include "mrc_exb.h"
#include "libini.h"
#include "packged.h"


void Lib_PackgedMrp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Integer = PackgedMrp(Param[0]->Val->Pointer,Param[1]->Val->Pointer,Param[2]->Val->Pointer);
}


const  char packged_Defs[] = "\
";


int32 Packged_init(void)
{

	Haninit(NULL);

	AddString(Lib_PackgedMrp,"int PackgedMrp(char*,char**,char*);");


	return hanclose();

}
