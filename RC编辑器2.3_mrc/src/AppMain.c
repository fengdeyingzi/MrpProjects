//#include <mrc_base.h>
//#include <mrc_exb.h>
//#include <mrc_win.h>

#include <mpc.h>
//#include "xl.h"
//#include "ui_list.h"
#include <ui.h>


#include "AppMain.h"
#include "Funcs.c"
#include "Draw.c"
#include "List.c"
#include "FileRW.c"
//#include "FileView.c"
#include "res_str.h"

#define FILERC "temp\res_lang0.rc"
#define APPNAME "RC�༭��V2.3"

//#define CONFIG_SPR

extern int32 mrc_pause(void);
void WLogu(char* Text, char *text2);
long wstrlen(char*);

static const int32 StrBufSize = 1024 * 8;//�ַ�����������С
static char *StrBuffer;//�ַ���������

char MrpRoot[256];//Mrp��Ŀ¼
char FilePath[256];//�ļ�·��
int16 RcCount, RcIndex;//�ַ�����������ǰ���
mr_screeninfo scnInfo;//��Ļ�ߴ���Ϣ

static int32 KeyTimerHandle;//������ʱ��
static int32 UIHwnd;//����UI���
static uint8 EditType;//�༭����(����/���/·��)
static uint8 OpenType;//������(Mrp/Rc)
static uint8 IsMultiPress;//�Ƿ񳤰���

static uint32 lBackColor, bBackColor, fBackColor;//��ɫ
static mr_screenRectSt rectHeader, rectTool, rectTCmd1, rectTCmd2;//�������͵ײ�����������
static mr_screenRectSt rectCmd1, rectCmd2, rectCmd3, rectCmd4;//�ĸ���ť����
static mr_screenRectSt rectMainText, rectIndex;//�ı������ſ�����

/*------------------------------------------------------------------------------------------------*/
//��ʼ�������ؼ�λ����Ϣ
void InitRects(void)
{
    int32 fw, fh, fw2, fh2;
    uint16 scnWidth, scnHeight;
    uint16 cmdWidth1, cmdWidth2, cmdHeight;
    uint16 mtboxHeight, itboxWidth, tooltWidth;

    mrc_getScreenInfo(&scnInfo);//��ȡ��Ļ�ߴ���Ϣ
// /*/*/
 mrc_textWidthHeight("\x65\x87\x67\x2c", 1,MR_FONT_MEDIUM, &fw, &fh);//"�ı�"
///*/*/*    
mrc_textWidthHeight("\x0\x31\x0\x32\x0\x33\x0\x34\x0\x2f\x0\x35\x0\x36\x0\x37\x0\x38", 1,MR_FONT_MEDIUM, &fw2, &fh2);//"1234/5678"

    //�趨���
    scnWidth = (uint16)scnInfo.width;
    scnHeight = (uint16)scnInfo.height;
    tooltWidth = (uint16)(fw + 8);
    itboxWidth = (uint16)(fw2 + 8);
    cmdWidth1 = (scnWidth - itboxWidth) / 4;
    cmdWidth2 = (scnWidth - itboxWidth) / 4;
    cmdHeight = (uint16)(fh + 4);
    mtboxHeight = (scnHeight - cmdHeight * 3);

    //�趨������Ϣ
    rectCmd1.x = 0;
    rectCmd1.y = cmdHeight;
    rectCmd1.w = cmdWidth1;
    rectCmd1.h = cmdHeight;
    rectCmd2.x = cmdWidth1;
    rectCmd2.y = cmdHeight;
    rectCmd2.w = cmdWidth2;
    rectCmd2.h = cmdHeight;
    rectCmd3.x = scnWidth - cmdWidth1 - cmdWidth2;
    rectCmd3.y = cmdHeight;
    rectCmd3.w = cmdWidth2;
    rectCmd3.h = cmdHeight;
    rectCmd4.x = scnWidth - cmdWidth1;
    rectCmd4.y = cmdHeight;
    rectCmd4.w = cmdWidth1;
    rectCmd4.h = cmdHeight;

    rectHeader.x = 0;
    rectHeader.y = 0;
    rectHeader.w = scnWidth;
    rectHeader.h = cmdHeight;
    rectTool.x = 0;
    rectTool.y = scnHeight - cmdHeight;
    rectTool.w = scnWidth;
    rectTool.h = cmdHeight;
    rectTCmd1.x = 0;
    rectTCmd1.y = rectTool.y;
    rectTCmd1.w = tooltWidth;
    rectTCmd1.h = rectTool.h;
    rectTCmd2.x = scnWidth - tooltWidth;
    rectTCmd2.y = rectTool.y;
    rectTCmd2.w = tooltWidth;
    rectTCmd2.h = rectTool.h;

    rectMainText.x = 0;
    rectMainText.y = cmdHeight * 2;
    rectMainText.w = scnWidth;
    rectMainText.h = scnHeight - cmdHeight * 3;
    rectIndex.x = cmdWidth1 + cmdWidth2;
    rectIndex.y = cmdHeight;
    rectIndex.w = scnWidth - cmdWidth1 * 2 - cmdWidth2 * 2;
    rectIndex.h = cmdHeight;
}

//���������ڣ��������Ƿ�ˢ��
void DrawMainForm(int32 Reflush)
{
    int32 fw, fh, fw2, fh2;

    mrc_clearScreen(255, 255, 255);

    //���Ʊ�����
    DrawCmd(rectHeader, NULL, lBackColor, bBackColor, 0, 0);

    //���Ƶײ�������
    DrawCmd(rectTool, NULL, lBackColor, bBackColor, 0, 0);

    //���ư�ť
    DrawCmd(rectCmd1, "<<", lBackColor, bBackColor, 0, 0);
    DrawCmd(rectCmd2, "<", lBackColor, bBackColor, 0, 0);
    DrawCmd(rectCmd3, ">", lBackColor, bBackColor, 0, 0);
    DrawCmd(rectCmd4, ">>", lBackColor, bBackColor, 0, 0);

    //���ı���
    DrawRoundRect(rectMainText.x, rectMainText.y, rectMainText.w, rectMainText.h, bBackColor);

    //��ſ�
    DrawRoundRect(rectIndex.x, rectIndex.y, rectIndex.w, rectIndex.h, bBackColor);

    //����������
    mrc_textWidthHeight(APPNAME, 0, MR_FONT_MEDIUM, &fw, &fh);
    mrc_textWidthHeight("RC", 0, MR_FONT_MEDIUM, &fw2, &fh2);
    mrc_drawText("RC", (int16)((rectHeader.w - fw) / 2), (int16)((rectHeader.h - fh2) / 2), 0xff, 0xff, 0xff, 0, MR_FONT_MEDIUM);
    mrc_drawText("�༭��", (int16)((rectHeader.w - fw) / 2 + fw2), (int16)((rectHeader.h - fh) / 2), 0xff, 0xff, 0xff, 0, MR_FONT_MEDIUM);
    mrc_textWidthHeight("V2.3", 0, MR_FONT_MEDIUM, &fw2, &fh2);
    mrc_drawText("V2.3", (int16)((rectHeader.w - fw) / 2 + fw - fw2), (int16)((rectHeader.h - fh2) / 2), 0xff, 0xff, 0xff, 0, MR_FONT_MEDIUM);

    //�ײ�����������
    DrawTextMid(rectTCmd1, GetString(STR_T_MENU), 0x00ffffff);
    DrawTextMid(rectTCmd2, GetString(STR_T_EXIT), 0x00ffffff);

    if (Reflush > 0)
    {
        mrc_refreshScreen(0, 0, (uint16)scnInfo.width, (uint16)scnInfo.height);
    }
}

//�����ı�������ı����������ı����Ƿ�ˢ��
void ShowText(char *tStr, int32 Reflush)
{
    mr_screenRectSt tRect;
    mr_colourSt tColor;
    int32 tCount, fw, fh;
    char CountText[12];

    tRect.x = rectMainText.x + 2;
    tRect.y = rectMainText.y + 2;
    tRect.w = rectMainText.w - 4;
    tRect.h = rectMainText.h - 4;
    tColor.b = 0;
    tColor.g = 0;
    tColor.r = 0;

    mrc_drawRect(rectMainText.x + 1, rectMainText.y + 1, rectMainText.w - 2, rectMainText.h - 2, 0xff, 0xff, 0xff);
    mrc_drawTextLeft(tStr, tRect.x, tRect.y, tRect, tColor, DRAW_TEXT_EX_IS_AUTO_NEWLINE, MR_FONT_MEDIUM);

    //��ʾ�ַ���
    tCount = mrc_wstrlen(tStr) / 2;
    if (tCount < 256)
    {
        tColor.r = 0;
        tColor.g = 255;
        tColor.b = 0; //��ɫ
    }
    else if (tCount < 512)
    {
        tColor.r = 255;
        tColor.g = 0;
        tColor.b = 255; //��ɫ
    }
    else
    {
        tColor.r = 255;
        tColor.g = 0;
        tColor.b = 0; //��ɫ
    }

    mrc_memset(CountText, 0, sizeof(CountText));
    mrc_sprintf(CountText, "[%d]", tCount);
    mrc_textWidthHeight(CountText, 0, MR_FONT_MEDIUM, &fw, &fh);
    mrc_drawText(CountText, (int16)(tRect.x + tRect.w - fw), (int16)(tRect.y + tRect.h - fh), tColor.r, tColor.g, tColor.b, 0, MR_FONT_MEDIUM);

    if (Reflush > 0)
    {
        mrc_refreshScreen(rectMainText.x, rectMainText.y, rectMainText.w, rectMainText.h);
    }
}

//������ţ���������ţ��������Ƿ�ˢ��
void ShowIndex(int16 Index, int16 Count, int32 Reflush)
{
    char tStr[20];

    mrc_drawRect(rectIndex.x + 1, rectIndex.y + 1, rectIndex.w - 2, rectIndex.h - 2, 0xff, 0xff, 0xff);
    mrc_memset(tStr, 0, 20);
    mrc_sprintf(tStr, "%d/%d", Index, Count);
    DrawTextMidA(rectIndex, tStr, 0);

    if (Reflush > 0)
    {
        mrc_refreshScreen(rectIndex.x, rectIndex.y, rectIndex.w, rectIndex.h);
    }
}

//��������
void DrawAll()
{
    DrawMainForm(0);
    ShowIndex(RcIndex, RcCount, 0);
    ShowText(StrBuffer, 0);
    mrc_refreshScreen(0, 0, (uint16)scnInfo.width, (uint16)scnInfo.height);
}

/*----------------------------------------------------------------------------------------------------*/
//�����ı������������������ַ���
void TextToBuffer(char *pText)
{
    int32 tLen = mrc_wstrlen(pText);

    memset(StrBuffer, 0, StrBufSize);
    memcpy(StrBuffer, pText, MIN(StrBufSize - 2, tLen));
}

//�����������ݵ�������
void ItemDataToBuffer(void)
{
    if (RcIndex > 0 && RcIndex <= RcCount)
    {
        GetItemData(RcIndex, StrBuffer, (uint16)StrBufSize);
    }
}

//���ƻ��������ݵ�����
void BufferToItemData(void)
{
    if (RcIndex > 0 && RcIndex <= RcCount)
    {
        SaveItemData(RcIndex, StrBuffer);
    }
}

//�ı���ţ��������ı�ֵ���Ƿ�ˢ��
void ChangeIndex(int16 ChangeNum, int32 Reflush)
{
    if (RcCount <= 0) return;

    RcIndex += ChangeNum;
    if (RcIndex < 1)
    {
        RcIndex = 1;
    }
    else if (RcIndex > RcCount)
    {
        RcIndex = RcCount;
    }

    ShowIndex(RcIndex, RcCount, 0);
    ItemDataToBuffer();
    ShowText(StrBuffer, 0);

    if (Reflush > 0)
    {
        mrc_refreshScreen(rectIndex.x, rectIndex.y, rectIndex.w, rectIndex.h);
        mrc_refreshScreen(rectMainText.x, rectMainText.y, rectMainText.w, rectMainText.h);
    }
    //WLog("��ʾ�ɹ�����ţ�", RcIndex);
}

/*------------------------------------------------------------------------------------------*/
//���ļ�
void OpenFile(const char* Path)
{
    char *gbPath;

    TextToBuffer(GetString(STR_OPENING));
    RcIndex=0;
    RcCount=0;
    DrawAll();


    gbPath = mrc_malloc(256);
    memset(gbPath,0,256); 
    UniToGB(Path, gbPath, 256);//ת��ΪGB2312
    WLog(gbPath,mrc_getLen(gbPath));
    WLog("���ļ�",0); 
    if(mrc_getLen(gbPath) == 0)
    {
    	WLog("�ļ�����Ϊ0",0); 
        OpenType = 2;
        TextToBuffer(GetString(STR_PATHERR));
    }
    /*
    if (MR_IS_FILE != mrc_fileState(gbPath))//�ļ�������
    {
        OpenType = 2;
        TextToBuffer(GetString(STR_PATHERR));
    }
    */
    else if (MR_SUCCESS == ReadMrp(gbPath))//��ΪMRP
    {
        OpenType = 0;
    }
    else if (MR_SUCCESS == ReadRc(gbPath))//��ΪRC
    {
        OpenType = 1;
    }
    else//�������(�޷���ȡ����)
    {
        OpenType = 3;
        TextToBuffer(GetString(STR_CANNOTREAD));
    }

    mrc_free(gbPath);

    if (0 == RcCount && (0 == OpenType || 1 == OpenType))//��Ŀ��Ϊ0��ʧ�ܣ�
    {
        OpenType = 4;
        TextToBuffer(GetString(STR_OPENFAILED));
    }
}

//�����޸ĵ��ļ�
void SaveFile(void)
{
    char *gbPath;
    int32 rValue=0;

    //"��ʾ�����ڱ��档����"
    ShowText(GetString(STR_SAVING), 1);

    gbPath = mrc_malloc(256);
    UniToGB(FilePath, gbPath, 256);//ת��ΪGB2312

    if (0 == OpenType)
    {
        rValue = SaveMrp(gbPath);
    }
    else if (1 == OpenType)
    {
        rValue = SaveRc(gbPath);
    }
    else
    {
        //"��ʾ����δ���ļ���"
        ShowText(GetString(STR_NOFILE), 1);
        mrc_free(gbPath);
        return;
    }

    mrc_free(gbPath);

    if (rValue > 0)
    {
        //"��ʾ������ɹ���"
        ShowText(GetString(STR_SAVEOK), 1);
    }
    else
    {
        //"��ʾ������ʧ�ܣ�"
        ShowText(GetString(STR_SAVEFAILED), 1);
    }
}

//ȡ�ø�Ŀ¼
void GetRoot(void)
{
    int32 rValue;

    mrc_memset(MrpRoot, 0, sizeof(MrpRoot));
    rValue = 0;//mrc_GetCurrentDirectory(sizeof(MrpRoot), MrpRoot);
    if (rValue > 3)
    {
        mrc_memmove(&MrpRoot[4], &MrpRoot[3], rValue - 3);
        MrpRoot[3] = '/';
    }
}

/*----------------------------------------------------------------------------------------------------------*/
//��ʾ���ر༭���ڣ��༭·������������ԭʼ·��
int32 ShowEditPath(const char *lastPath)
{
    int32 Hwnd;

    Hwnd = mrc_editCreate(GetString(STR_E_PATH), lastPath, MR_EDIT_ANY, 128); //"����·��"

    EditType = 2;
    return Hwnd;
}

//��ʾ���ر༭���ڣ��༭�ı�������������ǰ�ı�
int32 ShowEditText(const char *lastText)
{
    int32 Hwnd;

    //"�༭����"
    Hwnd = mrc_editCreate(GetString(STR_E_TEXT), lastText, MR_EDIT_ANY, 0);

    EditType = 0;
    return Hwnd;
}

//�ӱ��ر༭����ȡ�ý�����ݣ��༭�ı����������������������
void GetTextFromEdit(int32 Hwnd, char sBuffer[], int32 bufSize)
{
    const char *tStr = mrc_editGetText(Hwnd);
    int32 tSize;

    if(NULL != tStr)
    {
        tSize = mrc_wstrlen((char*)tStr);
        if (tSize > bufSize)
            tSize = bufSize;
        mrc_memset(sBuffer, 0, bufSize);
        mrc_memcpy(sBuffer, tStr, tSize);
    }
}

//��ʾ���ر༭���ڣ��༭��ţ�����������ţ����������أ����
int32 ShowEditIndex(int16 LastIndex, int16 MaxIndex)
{
    int32 Hwnd, rsize;
    char IndexStr[10], TitleStr[20];
    char *pIndex, *pTitle;

    mrc_memset(IndexStr, 0, 10);
    mrc_memset(TitleStr, 0, 20);
    mrc_sprintf(IndexStr, "%d", LastIndex);
    mrc_sprintf(TitleStr, "ת��(1-%d)", MaxIndex);
    pIndex = (char*)mrc_c2u(TitleStr, 0, &rsize);
    pTitle = (char*)mrc_c2u(IndexStr, 0, &rsize);

    //"ת��(1-X)"
    Hwnd = mrc_editCreate((const char*)pIndex, (const char*)pTitle, MR_EDIT_NUMERIC, mrc_strlen(TitleStr)-8);

    mrc_free(pIndex);
    mrc_free(pTitle);
    EditType = 1;

    return Hwnd;
}

//�ӱ��ر༭����ȡ�ý�����ݣ��༭��ţ�����������������أ�������ֵ
int16 GetIndexFromEdit(int32 Hwnd)
{
    const char *tStr = mrc_editGetText(Hwnd);
    int16 outValue = 0;

    if(NULL != tStr)
    {
        outValue = (int16)watoi((char*)tStr);
    }
    return outValue;
}

//ɾ�����ر༭���ڣ����������
void DestoryEdit(int32 Hwnd)
{
    if (0 != Hwnd)
    {
        mrc_editRelease(Hwnd);
        Hwnd = 0;
    }
}

//��ʾ���ز˵�
void ShowMenu()
{
    int32 mHwnd;

    mHwnd = menucreate(GetString(STR_T_MENU), 7);//"�˵�"
    menuset(mHwnd, GetString(STR_M_OPEN), 0);//"���ļ�(���)"
    menuset(mHwnd, GetString(STR_M_OPEN_DEF), 1);//"���ļ�(����)"
    menuset(mHwnd, GetString(STR_M_SAVE), 2);//"�����޸�"
    menuset(mHwnd, GetString(STR_M_TOTEXT), 3);//"�����ı�"
    menuset(mHwnd, GetString(STR_M_HELP), 4);//"ʹ�ð���"
    menuset(mHwnd, GetString(STR_M_ABOUT), 5);//"��������"
    menuset(mHwnd, GetString(STR_M_EXIT), 6);//"�˳�����"
    menushow(mHwnd);

    UIHwnd = mHwnd;
}

int32 fileview;
//��ʾ�����ļ��������
void ShowOpen()
{
    fileinit();
    fileview=filecreate(".\\",".mrp",FILE_HZ);
    filesettitle(fileview,"ѡ��mrp�ļ�");
    WLog("�ļ����������",MR_LOCALUI_EVENT);
    fileshow(fileview);
    /*
    FV_Init();
    FV_SetType(0, 1);
    FV_BeginView(MrpRoot, MR_DSPL_NONE);
    */

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//ģ����������
void KeyTimerDo(int32 data)
{
    ChangeIndex((int16)data, 1);
}

//��ʼ������ʱ
void KeyTimerStart(int32 data)
{
    if (0 == KeyTimerHandle) //���°���
    {
        KeyTimerHandle = mrc_timerCreate();
        mrc_timerStart(KeyTimerHandle, 500, data, KeyTimerStart, 0);
        IsMultiPress = 0;
    }
    else //��ס����
    {
        mrc_timerStop(KeyTimerHandle);
        mrc_timerStart(KeyTimerHandle, 100, data, KeyTimerDo, 1);
        IsMultiPress = 1;
    }
}

//����������ʱ
void KeyTimerStop(int32 data)
{
    if (KeyTimerHandle != 0)
    {
        mrc_timerStop(KeyTimerHandle);
        mrc_timerDelete(KeyTimerHandle);
        KeyTimerHandle = 0;
    }

    if (0 == IsMultiPress) //δ���г���������ʱ��һ�μ�
    {
        if (0 != data)
        {
            KeyTimerDo(data);
        }
    }
}

/*----------------------------------------------------------------------------------------------------*/
//�������¼�
void DoKeyPress(int32 data, int32 p1)
{
    switch(p1)
    {
    case MR_KEY_4://-1
    case MR_KEY_UP:
        DrawCmd(rectCmd2, "<", lBackColor, bBackColor, 0, 1);
        KeyTimerStop(-1);
        break;
    case MR_KEY_6://+1
    case MR_KEY_DOWN:
        DrawCmd(rectCmd3, ">", lBackColor, bBackColor, 0, 1);
        KeyTimerStop(1);
        break;
    case MR_KEY_1://-10
    case MR_KEY_LEFT:
        DrawCmd(rectCmd1, "<<", lBackColor, bBackColor, 0, 1);
        KeyTimerStop(-10);
        break;
    case MR_KEY_3://+10
    case MR_KEY_RIGHT:
        DrawCmd(rectCmd4, ">>", lBackColor, bBackColor, 0, 1);
        KeyTimerStop(10);
        break;
    case MR_KEY_5://�༭�ı�
    case MR_KEY_SELECT:
        if (RcCount > 0)
            UIHwnd = ShowEditText(StrBuffer);
        break;
    case MR_KEY_2://�༭���
        if (RcCount > 0)
            UIHwnd = ShowEditIndex(RcIndex, RcCount);
        break;
    case MR_KEY_SOFTLEFT://�˵�
        ShowMenu();
        break;
    case MR_KEY_0://����
        TextToBuffer(GetString(STR_ABOUT));
        DrawAll();
        break;
    case MR_KEY_SOFTRIGHT://�˳�
        //mrc_winClose();
        mrc_exit();
        break;
    }
}

//��������
void DoKeyDown(int32 data, int32 p1)
{
    KeyTimerStop(0); //��֤����ֹͣ��ʱ

    switch(p1)
    {
    case MR_KEY_4://-1
    case MR_KEY_UP:
        DrawCmd(rectCmd2, "<", lBackColor, bBackColor, 1, 1);
        KeyTimerStart(-1);
        break;
    case MR_KEY_6://+1
    case MR_KEY_DOWN:
        DrawCmd(rectCmd3, ">", lBackColor, bBackColor, 1, 1);
        KeyTimerStart(1);
        break;
    case MR_KEY_1://-10
    case MR_KEY_LEFT:
        DrawCmd(rectCmd1, "<<", lBackColor, bBackColor, 1, 1);
        KeyTimerStart(-10);
        break;
    case MR_KEY_3://+10
    case MR_KEY_RIGHT:
        DrawCmd(rectCmd4, ">>", lBackColor, bBackColor, 1, 1);
        KeyTimerStart(10);
        break;
    }
}

//��Ļ����
void DoMouseDown(int32 data, int32 p1, int32 p2)
{
    if(p2 >= rectCmd1.y && p2 < (rectCmd1.y + rectCmd1.h))
    {
        if(p1 < rectCmd1.w)
        {
            DoKeyDown(0, MR_KEY_1);//-10
        }
        else if(p1 >= rectCmd2.x && p1 < (rectCmd2.x + rectCmd2.w))
        {
            DoKeyDown(0, MR_KEY_4);//-1
        }
        else if(p1 >= rectCmd3.x && p1 < (rectCmd3.x + rectCmd3.w))
        {
            DoKeyDown(0, MR_KEY_6);//+1
        }
        else if(p1 >= rectCmd4.x && p1 < (rectCmd4.x + rectCmd4.w))
        {
            DoKeyDown(0, MR_KEY_3);//+10
        }
    }
}

//������Ļ����¼�
void DoClick(int32 data, int32 p1, int32 p2)
{
    if(p2 < rectHeader.h)
    {
        DoKeyPress(0, MR_KEY_0);//�����������ڣ�
    }
    else if(p2 >= rectCmd1.y && p2 < (rectCmd1.y + rectCmd1.h))
    {
        if(p1<rectCmd1.w)
        {
            DoKeyPress(0, MR_KEY_1);//-10
        }
        else if(p1 >= rectCmd2.x && p1 < (rectCmd2.x + rectCmd2.w))
        {
            DoKeyPress(0, MR_KEY_4);//-1
        }
        else if(p1 >= rectIndex.x && p1 < (rectIndex.x + rectIndex.w))
        {
            DoKeyPress(0, MR_KEY_2);//���
        }
        else if(p1 >= rectCmd3.x && p1 < (rectCmd3.x + rectCmd3.w))
        {
            DoKeyPress(0, MR_KEY_6);//+1
        }
        else if(p1 >= rectCmd4.x && p1 < (rectCmd4.x + rectCmd4.w))
        {
            DoKeyPress(0, MR_KEY_3);//+10
        }
    }
    else if(p2 >= rectMainText.y && p2 < (rectMainText.y + rectMainText.h))
    {
        DoKeyPress(0, MR_KEY_5);//����
    }
    else if(p2 >= rectTool.y && p2 < (rectTool.y + rectTool.h))
    {
        if(p1 >= rectTCmd1.x && p1 < (rectTCmd1.x + rectTCmd1.w))
        {
            DoKeyPress(0, MR_KEY_SOFTLEFT);//�����
        }
        else if(p1 >= rectTCmd2.x && p1 < (rectTCmd2.x + rectTCmd2.w))
        {
            DoKeyPress(0, MR_KEY_SOFTRIGHT);//�����
        }
    }
}

/*----------------------------------------------------------------------------------------------------------*/
//����Ի����¼�
void DoDialogEvent(int32 data, int32 p1)
{
    int16 NewIndex;

    switch(p1)
    {
    case MR_DIALOG_KEY_OK://ȷ������������
        switch(EditType)
        {
        case 0://�ı�����
            GetTextFromEdit(UIHwnd, StrBuffer, StrBufSize);
            BufferToItemData();
            break;
        case 1://���
            NewIndex = GetIndexFromEdit(UIHwnd);
            ChangeIndex(NewIndex - RcIndex, 0);
            break;
        case 2://·��
            GetTextFromEdit(UIHwnd, FilePath, sizeof(FilePath));//ȡ������
            OpenFile(FilePath);//���ļ�
            ChangeIndex(0, 0);
            break;
        default:
            break;
        }
        DestoryEdit(UIHwnd);
        DrawAll();//ȫ���ػ�
        break;
    case MR_DIALOG_KEY_CANCEL://ȡ��
        DestoryEdit(UIHwnd);
        if (2 == EditType)
        {
            ShowMenu();//���ز˵�
        }
        else
        {
            DrawAll();//ȫ���ػ�
        }
        break;
    }
}


//2.3���ļ���������¼�(��gb2312�ı�)
void openfilemrp(char *pXPath)
{
    char *FilePath = NULL;
    FilePath=malloc(300); 
    GBToUni(pXPath, FilePath, strlen(pXPath)*2);//ת��ΪUnicode
    WLog("unicodeת���ɹ�",0); 
    OpenFile(FilePath);//���ļ�
    ChangeIndex(0, 0);


    DrawAll();//����������

}


//�����ش��ڣ��ļ�������¼�
void DoFileView(int32 data, int32 p1)
{
    if(p1==FILE_SL)
    {
        //WLog(filegetfilename(fileview),0);
        openfilemrp(filegetname(fileview));




    }
    if(p1==FILE_SR)
    {
        WLog("���ز˵�",UI_MENU);
        ShowMenu();//���ز˵�
        k_ui=UI_MENU;

    }
    /*
    char *pXPath = NULL, *pRPath = NULL;

    switch(p1)
    {
    case MR_LOCALUI_KEY_OK://ȷ������������
    	pRPath = (char*)mrc_malloc(256);
    	if (NULL != pRPath)
    	{
    		FV_GetPath(pRPath, 256);
    		pXPath = mrc_strstr(pRPath, MrpRoot);
    		if (NULL != pXPath)//��MRP��Ŀ¼
    		{
    			pXPath = pXPath + mrc_strlen(MrpRoot) + 1;//�������·��
                GBToUni(pXPath, FilePath, sizeof(FilePath));//ת��ΪUnicode
    			OpenFile(FilePath);//���ļ�
                ChangeIndex(0, 0);
    		}
    		else//����MRP��Ŀ¼
    		{
    			TextToBuffer(GetString(STR_PATHERR));
    		}
            if (pRPath)
            {
    		    mrc_free(pRPath);
            }
    	}
        FV_EndView();
    	DrawAll();//����������
        break;
    case MR_LOCALUI_KEY_CANCEL://ȡ��
    	FV_EndView();
    	ShowMenu();//���ز˵�
    	break;
    }
    */
}

//�����ز˵��¼�
void DoMenuEvent(int32 data, int32 p1)
{
    if (0 != UIHwnd)
    {
        menudel(UIHwnd);
        UIHwnd = 0;
    }

    switch (p1)
    {
    case 0://���ļ����
        ShowOpen();
        break;
    case 1:
        if (0 == mrc_wstrlen(FilePath))
        {
            //Ĭ��·����"temp\res_lang0.rc"
            UIHwnd = ShowEditPath(GetString(STR_DEFPATH));
        }
        else
        {
            UIHwnd = ShowEditPath(FilePath);
        }
        break;
    case 2:
        DrawAll();
        SaveFile();
        break;
    case 3:
        DrawAll();
        ShowText(GetString(STR_SAVETEXTING), 1);
        //DrawAll();
        if (0 == SaveToText("�����ı�.txt"))
        {
            ShowText(GetString(STR_SAVETEXTOK), 1);
        }
        else
        {
            ShowText("", 1);
        }
        break;
    case 4://����
        TextToBuffer(GetString(STR_HELP));
        DrawAll();
        break;
    case 5://����
        DoKeyPress(0, MR_KEY_0);
        break;
    case 6://�˳�
        DoKeyPress(0, MR_KEY_SOFTRIGHT);
        break;
    default://����
        DrawAll();
        break;
    }
}

/*-----------------------------------------------------------------------------------------*/
//�����¼�
/*
void MainWinWinEvent(int32 data, int32 eventId)
{
    switch (eventId)
    {
    case WIN_EVENT_SHOW:
        DrawAll();
        ShowLOGO();
        break;
    case WIN_EVENT_REFRESH:
        if (0 == UIHwnd)
        {
            DrawAll();
        }
        break;
    case WIN_EVENT_UPDATE:
        DrawAll();
        break;
    case WIN_EVENT_PAUSE:
        break;
    case WIN_EVENT_EXIT:
        break;
    }
}
*/
//��Ļ&�����¼�
void MainKeyWinEvent(int32 data, int32 type, int32 p1, int32 p2)
{
    if(MR_KEY_PRESS == type)//���������¼�
    {
        DoKeyDown(data, p1);
    }
    else if(MR_KEY_RELEASE == type)//�����¼�
    {
        DoKeyPress(data, p1);
    }
    else if(MR_MOUSE_DOWN == type)//���������¼�
    {
        DoMouseDown(data, p1, p2);
    }
    else if(MR_MOUSE_UP == type)//�����¼�
    {
        DoClick(data, p1, p2);
    }
    else if(MR_DIALOG_EVENT == type)//�Ի����¼����༭��
    {
        DoDialogEvent(data, p1);
    }
    else if(MR_LOCALUI_EVENT == type)//���ش����¼����ļ������
    {
        //DoFileView(data, p1);
        DoFileView(data, p1);
        return;
    }
    else if(MR_MENU_SELECT == type)//ѡ��˵�
    {
        DoMenuEvent(data, p1);
    }
    else if(MR_MENU_RETURN == type)//ȡ��ѡ��˵�
    {
        DoMenuEvent(data, -1);
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//Ӧ�ó�ʼ������
int32 mrc_init(void)
{
    mpc_init();
    //remove("log.txt");
    printf("rc...");
    RcIndex = 0;
    OpenType = 2;

    lBackColor = 0x00d3e0f7;
    bBackColor = 0x002888d0;
    fBackColor = 0x002888d0;

    StrBuffer = (char*)mrc_malloc(StrBufSize);
    mrc_memset(StrBuffer, 0, StrBufSize);
    mrc_memset(FilePath, 0, sizeof(FilePath));
    LoadAllStrings();//��������ַ���
    InitRects();//��ʼ�������ؼ�λ�úʹ�С
    //GetRoot();//ȡ��MRP��Ŀ¼
    OpenFile(GetString(STR_DEFPATH));//���ļ�
    memcpy(FilePath,GetString(STR_DEFPATH),255);
    
    WLogu("��·��",FilePath);
    ChangeIndex(0, 0);
    DrawAll();
    ///mrc_winInit();
    //mrc_winNew(0, MainWinWinEvent, MainKeyWinEvent);
    return MR_SUCCESS;
}

//�ú�����Ӧ�������ڼ䣬ÿ��mythroadƽ̨�յ��¼�ʱ�����á�
int _event(int code, int param0, int param1)
{
    //mrc_winEvent(code, param0, param1);
    MainKeyWinEvent(0, code, param0, param1);
    //DoMenuEvent(code, param0)
    return MR_SUCCESS;
}

/*
int32 mrc_appEvent(int32 code, int32 param0, int32 param1)
{
    event(code, param0, param1);
    return MR_SUCCESS;
}
*/

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    event(code, param0, param1);
    return MR_SUCCESS;
}

//�ú�����Ӧ����ͣʱ��mythroadƽ̨���á�
int32 mrc_pause()
{
    return MR_SUCCESS;
}

//�ú�����Ӧ�ûָ�����ʱ��mythroadƽ̨���á�
int32 mrc_resume()
{
    //mrc_winResume();
    return MR_SUCCESS;
}


//�ú�����Ӧ���˳�ʱ��mythroadƽ̨���á�
int32 mrc_exitApp()
{
    TextToBuffer(GetString(STR_EXITING));//��ʾ�������˳�������
    DrawAll();
    mrc_free(StrBuffer);
    ClearItems();
    ReleaseIco();
    ReleaseAllStrings();

    return MR_SUCCESS;
}

