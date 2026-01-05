#include <mrc_base.h>

char* ex_coding(char* text,int len,char* coding,char* tocoding);
int IsUTF8(void *pBuffer, long size);
int UTF8ToUni( char *utf8str, char *unistr, int size);
int UniToUTF8(char *unistr, char *utf8str, int size);
char *unToGb(char *text);
char *gbToUn(char *text);