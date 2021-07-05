#ifndef _LIST_H_
#define _LIST_H_

typedef struct Rc_Item
{
	int Index;
  int Size;
	char *DataStr;
	struct Rc_Item *Next;
}RC_ITEM, *PRC_ITEM;
/*
typedef struct Rc_Item
{
	int Index;
  int Size;
	char *DataStr;
	struct Rc_Item *Next;
}*PRC_ITEM;
*/

//º¯Êý
int32 ReadRc(const char *rFile);
int32 SaveRc(const char *rFile);
int32 SaveToText(const char *rFile);

int16 CalcListCount(const char* rFile);
//PRC_ITEM MakeRcList(const char* rFile, int16 listCount);
uint8 JudgeCode(PRC_ITEM headItem);

uint16 GetItemData(int16 Index, char *sBuffer, uint16 bufSize);
uint16 SaveItemData(uint16 Index, char *NewData);
void ClearItems(void);

#endif