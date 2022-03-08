#include "platform.h"
#include "picoc.h"
#include "interpreter.h"
#include "mrc_exb.h "
#include "font.h "
/* ֵ���ݸ� exit() */
int FuncRet;

struct ReleaseFile{
    char *FilePtr;
    struct ReleaseFile* next;
};
static struct ReleaseFile *FileList=NULL;

/* ��ʼ������ */
void PicocInitialise(int StackSize)
{
    PlatformInit();
    BasicIOInit();
    HeapInit(StackSize);
    TableInit();
    VariableInit();
    LexInit();
    TypeInit();
    IncludeInit();
    LibraryInit();
}

/* �ͷ��ڴ� */
void PicocCleanup(void)
{
    PlatformCleanup();
    IncludeCleanup();
    ParseCleanup();
    LexCleanup();
    VariableCleanup();
    TypeCleanup();
    TableStrFree();
    HeapCleanup();
    fileFree();
	freeFunc();
	closefont();
}

jmp_buf PicocExitBuf;
int32 debugfile;


void fileFree(void)
{
    while (FileList != NULL)
    {
        struct ReleaseFile *Next = FileList->next;

        if (FileList->FilePtr != NULL)
            free((void *)FileList->FilePtr);

        free(FileList);
        FileList = Next;
    }
}

void PlatformInit(void)
{
    mrc_remove(XQC_PRINT_FILE);
    debugfile = mrc_open(XQC_PRINT_FILE,12);
}

void PlatformCleanup(void)
{
    if(debugfile)
    {
        mrc_close(debugfile);
        debugfile=0;
    }
}

void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *Stream)
{
    if(debugfile)
        mrc_write(debugfile,&OutCh,1);
}
#ifdef includefile
/* �ļ������ڴ� */
char *PlatformReadFile(const char *FileName)
{
    int32 fd;
    uint32 fSize;
    char *fBuf = NULL;
    
    fSize = mrc_getLen(FileName);
    fd = mrc_open(FileName, MR_FILE_RDONLY);

    if (fd)
    {
        struct ReleaseFile *NewNode=mrc_malloc(sizeof(struct ReleaseFile));

        if(!NewNode) goto err0;
        NewNode->FilePtr=NULL;
        NewNode->next=FileList;
        FileList=NewNode;

        fBuf = (char*)mrc_malloc(fSize + 1);
        if(!fBuf) goto err0;
        mrc_read(fd, fBuf, fSize);
        fBuf[fSize]=0;
        mrc_close(fd);
        FileList->FilePtr=fBuf;
    }
    else goto err;

    return fBuf;

err0:
    mrc_close(fd);
err:
    ProgramFail(NULL, "�޷���%s",FileName);
    return NULL;
}
#else
/* �ڲ��ļ������ڴ� */
char *PlatformReadFilenei(const char *FileName)
{
	uint32 fSize;
	char *temp=NULL,*fBuf = NULL;

	temp=mrc_strrchr(FileName,'/');
	if(temp==NULL)
		temp=(char *)FileName;
	else
		temp++;
	fBuf=filenei(temp,&fSize);

	if (fBuf)
	{
		struct ReleaseFile *NewNode=mrc_malloc(sizeof(struct ReleaseFile));

		if(!NewNode) goto err;
		NewNode->FilePtr=NULL;
		NewNode->next=FileList;
		FileList=NewNode;
		FileList->FilePtr=fBuf;
	}
	else goto err;

	return fBuf;

err:
	if(fBuf)
		free(fBuf);
	return NULL;
}
#endif

/* ����ɨ���ļ����� */
char *PicocPlatformScanFile(const char *FileName)
{
	char *SourceStr;
#ifdef includefile
	SourceStr=PlatformReadFile(FileName);
#else
	SourceStr= PlatformReadFilenei(FileName);
#endif
	PicocParse(FileName, SourceStr, strlen(SourceStr), TRUE, FALSE, FALSE);
	return SourceStr;
}



/* �˳����� */
void PlatformExit(int RetVal)
{
    FuncRet = RetVal;
    longjmp(PicocExitBuf, 1);
}

void PrintSourceTextErrorLine(const char *FileName, const char *SourceText, int Line, int CharacterPos)
{
    int LineCount;
    const char *LinePos;
    const char *CPos;
    int CCount;
    
    if (SourceText)
    {
        /* �ҵ�Դ������ */
        for (LinePos = SourceText, LineCount = 1; *LinePos && LineCount < Line; LinePos++)
        {
            if (*LinePos == '\n')
                LineCount++;
        }
        
        /* ��ʾ�� */
        for (CPos = LinePos; *CPos != '\n' && *CPos; CPos++)
            PrintCh(*CPos, CStdOut);
        PrintStr("\r\n", CStdOut);
        
        /* ��ʾ����λ�� */
        for (CPos = LinePos, CCount = 0; *CPos != '\n' && *CPos && (CCount < CharacterPos || *CPos == ' '); CPos++, CCount++)
        {
            if (*CPos == '\t')
                PrintCh('\t', CStdOut);
            else
                PrintCh(' ', CStdOut);
        }
    }
    //����ģʽ
//     else
//     {
//         /* assume we're in interactive mode - try to make the arrow match up with the input text */
//         for (CCount = 0; CCount < CharacterPos + strlen(INTERACTIVE_PROMPT_STATEMENT); CCount++)
//             PrintCh(' ', CStdOut);
//     }
    PlatformPrintf("^ %s,��:%d,λ��:%d��Ϣ:", FileName, Line, CharacterPos);
    //mrc_clearScreen(0,0,0);
    //mrc_drawText("�������",0,0,240,0,0,0,1);
    //mrc_refreshScreen(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    //mrc_sleep(3000);
    
}

/* ��ʾԴ�����к��к�ʶ����� */
void PlatformErrorPrefix(struct ParseState *Parser)
{
    if (Parser != NULL)
        PrintSourceTextErrorLine(Parser->FileName, Parser->SourceText, Parser->Line, Parser->CharacterPos);
}

/* �˳���Ϣ */
void ProgramFail(struct ParseState *Parser, const char *Message, ...)
{
    va_list Args;

    PlatformErrorPrefix(Parser);
    va_start(Args, Message);
    PlatformVPrintf(Message, Args);
    va_end(Args);
    PlatformPrintf("\r\n");
    PlatformExit(1);
}

/* like ProgramFail() but gives descriptive error messages for assignment */
void AssignFail(struct ParseState *Parser, const char *Format, struct ValueType *Type1, struct ValueType *Type2, int Num1, int Num2, const char *FuncName, int ParamNo)
{
    PlatformErrorPrefix(Parser);
    PlatformPrintf("���� %s ", (FuncName == NULL) ? "��ֵ" : "set");   
        
    if (Type1 != NULL)
        PlatformPrintf(Format, Type1, Type2);
    else
        PlatformPrintf(Format, Num1, Num2);
    
    if (FuncName != NULL)
        PlatformPrintf("��%s()�ĵ�%d����", FuncName, ParamNo);
        
    ProgramFail(NULL, "");
}

/* ����Ϣ�˳� */
void LexFail(struct LexState *Lexer, const char *Message, ...)
{
    va_list Args;

    PrintSourceTextErrorLine(Lexer->FileName, Lexer->SourceText, Lexer->Line, Lexer->CharacterPos);
    va_start(Args, Message);
    PlatformVPrintf(Message, Args);
    va_end(Args);
    PlatformPrintf("\r\n");
    PlatformExit(1);
}

/* printf �������Ĵ��󱨸� */
void PlatformPrintf(const char *Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    PlatformVPrintf(Format, Args);
    va_end(Args);
}

void PlatformVPrintf(const char *Format, va_list Args)
{
    const char *FPos;
    
    for (FPos = Format; *FPos; FPos++)
    {
        if (*FPos == '%')
        {
            FPos++;
            switch (*FPos)
            {
            case 's': PrintStr(va_arg(Args, char *), CStdOut); break;
            case 'd': PrintSimpleInt(va_arg(Args, int), CStdOut); break;
            case 'c': PrintCh(va_arg(Args, int), CStdOut); break;
            case 't': PrintType(va_arg(Args, struct ValueType *), CStdOut); break;
#ifndef NO_FP
            case 'f': PrintFP(va_arg(Args, double), CStdOut); break;
#endif
            case '%': PrintCh('%', CStdOut); break;
            case '\0': FPos--; break;
            }
        }
        else
            PrintCh(*FPos, CStdOut);
    }
}
