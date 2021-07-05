//º¯Êý
void ShowLOGO(void);
void ReleaseIco(void);

void LoadAllStrings(void);
void ReleaseAllStrings(void);
char* GetString(uint16 Index);

char *FindPos(char *StrS, char *StrF, int32 SLen, int32 FLen);
int32 watoi(char *UniStr);
void GBToUni(const char *gbStr, char *uniBuffer, int32 bufSize);
void UniToGB(const char *uniStr, char *gbBuffer, int32 bufSize);

void WLog(char* Text, int32 Value);
//void GetMemInfo(char *DescText);

#define MIN(x, y) (((x) < (y))?(x):(y))