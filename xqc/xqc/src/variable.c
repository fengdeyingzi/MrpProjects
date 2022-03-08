#include "interpreter.h"

/*  ���� ��ʱ�������ߴ� ֱ�����Ǵ���һ������ */
#define MAX_TMP_COPY_BUF 1024*4

/* ȫ�ֶ���ı� */
struct Table GlobalTable;
struct TableEntry *GlobalHashTable[GLOBAL_TABLE_SIZE];

/* �ַ���ֵ�� */
struct Table StringLiteralTable;
struct TableEntry *StringLiteralHashTable[STRING_LITERAL_TABLE_SIZE];

/* ��ջ */
struct StackFrame *TopStackFrame;


/* ��ʼ������ϵͳ */
void VariableInit(void)
{
    TableInitTable(&GlobalTable, &GlobalHashTable[0], GLOBAL_TABLE_SIZE, TRUE);
    TableInitTable(&StringLiteralTable, &StringLiteralHashTable[0], STRING_LITERAL_TABLE_SIZE, TRUE);
    TopStackFrame = NULL;
}

/* �ͷ�һ������������ */
void VariableFree(struct Value *Val)
{
    if (Val->ValOnHeap)
    {
        /* �ͷź����� */
        if (Val->Typ == &FunctionType && Val->Val->FuncDef.Intrinsic == NULL && Val->Val->FuncDef.Body.Pos != NULL)
            HeapFreeMem((void *)Val->Val->FuncDef.Body.Pos);

        /* �ͷź��� */
        if (Val->Typ == &MacroType)
            HeapFreeMem((void *)Val->Val->MacroDef.Body.Pos);

        /* �ͷű��� */
        HeapFreeMem(Val);
    }
}

/* �ͷ�ȫ�ֱ���ַ������ֱ� */
void VariableTableCleanup(struct Table *HashTable)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            VariableFree(Entry->p.v.Val);
                
            /* �ͷŹ�ϣ����� */
            HeapFreeMem(Entry);
        }
    }
}

void VariableCleanup(void)
{
    VariableTableCleanup(&GlobalTable);
    VariableTableCleanup(&StringLiteralTable);
}

/* ����һЩ�ڴ棬�������ڶѻ�ջ������Ƿ��Ѿ����� */
void *VariableAlloc(struct ParseState *Parser, int Size, int OnHeap)
{
    void *NewValue;
    if (OnHeap)
        NewValue = HeapAllocMem(Size);
    else
        NewValue = HeapAllocStack(Size);
    
    if (NewValue == NULL)
        ProgramFail(Parser, "�ڴ治��");
    
#ifdef DEBUG_HEAP
    if (!OnHeap)
        printf("pushing %d at 0x%lx\n", Size, (unsigned long)NewValue);
#endif
        
    return NewValue;
}

/* �ڶѻ��ջ�ռ����һ��������Ҫ�����͵ı��� */
struct Value *VariableAllocValueAndData(struct ParseState *Parser, int DataSize, int IsLValue, struct Value *LValueFrom, int OnHeap)
{
    struct Value *NewValue = VariableAlloc(Parser, MEM_ALIGN(sizeof(struct Value)) + DataSize, OnHeap);
    NewValue->Val = (union AnyValue *)((char *)NewValue + MEM_ALIGN(sizeof(struct Value)));
    NewValue->ValOnHeap = OnHeap;
    NewValue->ValOnStack = !OnHeap;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    
    return NewValue;
}

/* ����һ������������������ */
struct Value *VariableAllocValueFromType(struct ParseState *Parser, struct ValueType *Typ, int IsLValue, struct Value *LValueFrom, int OnHeap)
{
    int Size = TypeSize(Typ, Typ->ArraySize, FALSE);
    struct Value *NewValue = VariableAllocValueAndData(Parser, Size, IsLValue, LValueFrom, OnHeap);
    assert(Size > 0 || Typ == &VoidType);
    NewValue->Typ = Typ;
    
    return NewValue;
}

/* �ѻ�ջ�Ϸ���һ������������������ֵ�������ظ����� */
struct Value *VariableAllocValueAndCopy(struct ParseState *Parser, struct Value *FromValue, int OnHeap)
{
    struct ValueType *DType = FromValue->Typ;
    struct Value *NewValue;
    char TmpBuf[MAX_TMP_COPY_BUF];
    int CopySize = TypeSizeValue(FromValue, TRUE);

    assert(CopySize <= MAX_TMP_COPY_BUF);
    memcpy((void *)&TmpBuf[0], (void *)FromValue->Val, CopySize);
    NewValue = VariableAllocValueAndData(Parser, CopySize, FromValue->IsLValue, FromValue->LValueFrom, OnHeap);
    NewValue->Typ = DType;
    memcpy((void *)NewValue->Val, (void *)&TmpBuf[0], CopySize);
    
    return NewValue;
}

/* ������AnyValue�����͵Ķѻ�ջ����һ������ */
struct Value *VariableAllocValueFromExistingData(struct ParseState *Parser, struct ValueType *Typ, union AnyValue *FromValue, int IsLValue, struct Value *LValueFrom)
{
    struct Value *NewValue = VariableAlloc(Parser, sizeof(struct Value), FALSE);
    NewValue->Typ = Typ;
    NewValue->Val = FromValue;
    NewValue->ValOnHeap = FALSE;
    NewValue->ValOnStack = FALSE;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    
    return NewValue;
}

/* �ڶѻ�ջ����һ�����������е�ֵ�������������*/
struct Value *VariableAllocValueShared(struct ParseState *Parser, struct Value *FromValue)
{
    return VariableAllocValueFromExistingData(Parser, FromValue->Typ, FromValue->Val, FromValue->IsLValue, FromValue->IsLValue ? FromValue : NULL);
}

/* ����һ�������� IDENT������еǼ� */
struct Value *VariableDefine(struct ParseState *Parser, char *Ident, struct Value *InitValue, struct ValueType *Typ, int MakeWritable)
{
    struct Value *AssignValue;
    
    if (InitValue != NULL)
        AssignValue = VariableAllocValueAndCopy(Parser, InitValue, TopStackFrame == NULL);
    else
        AssignValue = VariableAllocValueFromType(Parser, Typ, MakeWritable, NULL, TopStackFrame == NULL);
    
    AssignValue->IsLValue = MakeWritable;
	///////////////////////////////////////
    if (!TableSet((TopStackFrame == NULL) ? &GlobalTable : &TopStackFrame->LocalTable, Ident, AssignValue, Parser ? ((char *)Parser->FileName) : NULL, Parser ? Parser->Line : 0, Parser ? Parser->CharacterPos : 0))
        ProgramFail(Parser, "'%s'�Ѷ���  ����һ������", Ident);
    
    return AssignValue;
}

/* ����һ�������� IDENT������еǼǡ��������һ���ظ�������ͬ�����������׳�һ������ */
struct Value *VariableDefineButIgnoreIdentical(struct ParseState *Parser, char *Ident, struct ValueType *Typ, int IsStatic, int *FirstVisit)
{
    struct Value *ExistingValue;
    const char *DeclFileName;
    int DeclLine;
    int DeclColumn;
    
    if (IsStatic)
    {
        char MangledName[LINEBUFFER_MAX];
        char *MNPos = &MangledName[0];
        char *MNEnd = &MangledName[LINEBUFFER_MAX-1];
        const char *RegisteredMangledName;
        
        /* make the mangled static name (����ʹ�� sprintf() �����ͻ) */
        memset((void *)&MangledName, '\0', sizeof(MangledName));
        *MNPos++ = '/';
        strncpy(MNPos, (char *)Parser->FileName, MNEnd - MNPos);
        MNPos += strlen(MNPos);
        
        if (TopStackFrame != NULL)
        {
            /* �����ں����� */
            if (MNEnd - MNPos > 0) *MNPos++ = '/';
            strncpy(MNPos, (char *)TopStackFrame->FuncName, MNEnd - MNPos);
            MNPos += strlen(MNPos);
        }
            
        if (MNEnd - MNPos > 0) *MNPos++ = '/';
        strncpy(MNPos, Ident, MNEnd - MNPos);
        RegisteredMangledName = TableStrRegister(MangledName);
        
        /* �����̬�����Ƿ��Ѿ�����? */
        if (!TableGet(&GlobalTable, RegisteredMangledName, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn))
        {
            /* define the mangled-named static variable store in the global scope */
            ExistingValue = VariableAllocValueFromType(Parser, Typ, TRUE, NULL, TRUE);
            TableSet(&GlobalTable, (char *)RegisteredMangledName, ExistingValue, (char *)Parser->FileName, Parser->Line, Parser->CharacterPos);
            *FirstVisit = TRUE;
        }

        /* static variable exists in the global scope - now make a mirroring variable in our own scope with the short name */
        VariableDefinePlatformVar(Parser, Ident, ExistingValue->Typ, ExistingValue->Val, TRUE);
        return ExistingValue;
    }
    else
    {
        if (Parser->Line != 0 && TableGet((TopStackFrame == NULL) ? &GlobalTable : &TopStackFrame->LocalTable, Ident, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn)
                && DeclFileName == Parser->FileName && DeclLine == Parser->Line && DeclColumn == Parser->CharacterPos)
            return ExistingValue;
        else
            return VariableDefine(Parser, Ident, NULL, Typ, TRUE);
    }
}

/* ���һ�������Ƿ����ض������ƶ��塣 IDENT������еǼ� */
int VariableDefined(const char *Ident)
{
    struct Value *FoundValue;
    
    if (TopStackFrame == NULL || !TableGet(&TopStackFrame->LocalTable, Ident, &FoundValue, NULL, NULL, NULL))
    {
        if (!TableGet(&GlobalTable, Ident, &FoundValue, NULL, NULL, NULL))
            return FALSE;
    }

    return TRUE;
}

/* ���һ��������ֵ�����붨�塣 IDENT�������ע�� */
void VariableGet(struct ParseState *Parser, const char *Ident, struct Value **LVal)
{
    if (TopStackFrame == NULL || !TableGet(&TopStackFrame->LocalTable, Ident, LVal, NULL, NULL, NULL))
    {
        if (!TableGet(&GlobalTable, Ident, LVal, NULL, NULL, NULL))
            ProgramFail(Parser, "'%s'δ֪", Ident);
    }
}

/* ����һ��ȫ�ֱ�����һ��ƽ̨��ȫ�ֹ��� IDENT����ע�� */
void VariableDefinePlatformVar(struct ParseState *Parser, char *Ident, struct ValueType *Typ, union AnyValue *FromValue, int IsWritable)
{
    struct Value *SomeValue = VariableAllocValueAndData(NULL, 0, IsWritable, NULL, TRUE);
    SomeValue->Typ = Typ;
    SomeValue->Val = FromValue;
    if (!TableSet((TopStackFrame == NULL) ? &GlobalTable : &TopStackFrame->LocalTable, TableStrRegister(Ident), SomeValue, Parser ? Parser->FileName : NULL, Parser ? Parser->Line : 0, Parser ? Parser->CharacterPos : 0))
        ProgramFail(Parser, "'%s'�Ѷ���   ȫ�ֱ�����һ��ƽ̨", Ident);
}

/* �ͷ� ��/�� ������ջ������ֵ�� var�����Ƕ�ջ������ֵ! */
void VariableStackPop(struct ParseState *Parser, struct Value *Var)
{
    int Success;
    
#ifdef DEBUG_HEAP
    if (Var->ValOnStack)
        printf("popping %ld at 0x%lx\n", (unsigned long)(sizeof(struct Value) + TypeSizeValue(Var, FALSE)), (unsigned long)Var);
#endif
        
    if (Var->ValOnHeap)
    { 
        if (Var->Val != NULL)
            HeapFreeMem(Var->Val);
            
        Success = HeapPopStack(Var, sizeof(struct Value));                       /* �Ӷ����ͷ� */
    }
    else if (Var->ValOnStack)
        Success = HeapPopStack(Var, sizeof(struct Value) + TypeSizeValue(Var, FALSE));  /* ��ջ���ͷ� */
    else
        Success = HeapPopStack(Var, sizeof(struct Value));                       /* ֵ�ǲ������ǵ����� */
        
    if (!Success)
        ProgramFail(Parser, "��ջ����");
}

/* ����һ����������ʱ�����һ����ջ�ṹ */
void VariableStackFrameAdd(struct ParseState *Parser, const char *FuncName, int NumParams)
{
    struct StackFrame *NewFrame;
    
    HeapPushStackFrame();
    NewFrame = HeapAllocStack(sizeof(struct StackFrame) + sizeof(struct Value *) * NumParams);
    if (NewFrame == NULL)
        ProgramFail(Parser, "�ڴ治��");
        
    ParserCopy(&NewFrame->ReturnParser, Parser);
    NewFrame->FuncName = FuncName;
    NewFrame->Parameter = (NumParams > 0) ? ((void *)((char *)NewFrame + sizeof(struct StackFrame))) : NULL;
    TableInitTable(&NewFrame->LocalTable, &NewFrame->LocalHashTable[0], LOCAL_TABLE_SIZE, FALSE);
    NewFrame->PreviousStackFrame = TopStackFrame;
    TopStackFrame = NewFrame;
}

/* ɾ��һ����ջ�ṹ */
void VariableStackFramePop(struct ParseState *Parser)
{
    if (TopStackFrame == NULL)
        ProgramFail(Parser, "ջ��");
        
    ParserCopy(Parser, &TopStackFrame->ReturnParser);
    TopStackFrame = TopStackFrame->PreviousStackFrame;
    HeapPopStackFrame();
}

/* �õ�һ���ַ������֡��ٶ�IDENT���Ѿ�ע�ᡣ NULL���û���ҵ� */
struct Value *VariableStringLiteralGet(char *Ident)
{
    struct Value *LVal = NULL;

    if (TableGet(&StringLiteralTable, Ident, &LVal, NULL, NULL, NULL))
        return LVal;
    else
        return NULL;
}

/* ����һ���ַ����������ٶ���ע��IDENT */
void VariableStringLiteralDefine(char *Ident, struct Value *Val)
{
    TableSet(&StringLiteralTable, Ident, Val, NULL, 0, 0);
}

/* ���ָ�����Ч�Ժ�ʹ�÷����� */
void *VariableDereferencePointer(struct ParseState *Parser, struct Value *PointerValue, struct Value **DerefVal, int *DerefOffset, struct ValueType **DerefType, int *DerefIsLValue)
{
    if (DerefVal != NULL)
        *DerefVal = NULL;
        
    if (DerefType != NULL)
        *DerefType = PointerValue->Typ->FromType;
        
    if (DerefOffset != NULL)
        *DerefOffset = 0;
        
    if (DerefIsLValue != NULL)
        *DerefIsLValue = TRUE;

    return PointerValue->Val->Pointer;
}

