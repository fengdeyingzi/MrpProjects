#include "haning.h"

void StdIsalnum(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isalnum(Param[0]->Val->Integer);
}

void StdIsalpha(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isalpha(Param[0]->Val->Integer);
}

void StdIsdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isdigit(Param[0]->Val->Integer);
}
void StdIsprint(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    int ch = Param[0]->Val->Integer;
    ReturnValue->Val->Integer = (ch >= 0x20 && ch <= 0x7e);//ASCII�ɴ�ӡ�ַ�
}

void StdTolower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = Param[0]->Val->Integer |= 0x20;//ת��ΪСд��ĸ
}

void StdToupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = Param[0]->Val->Integer &= 0xdf;//ת��Ϊ��д��ĸ
}


int32 ctype_init()
{
	Haninit(NULL);

	AddString(StdIsalnum,"int isalnum(int);");
	AddString(StdIsalpha,"int isalpha(int);");
	AddString(StdIsdigit,"int isdigit(int);");
	AddString(StdIsprint,"int isprint(int);");
	AddString(StdTolower,"int tolower(int);");
	AddString(StdToupper,"int toupper(int);");

	return hanclose();

}