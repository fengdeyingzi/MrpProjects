#include "mrc_base.h"
#include "Draw.h"

/* ������ɫ��RGBֵ */
#define PIXEL888RED(pixel) (uint8)(((pixel) >> 16) & 0xff)
#define PIXEL888GREEN(pixel) (uint8)(((pixel) >> 8) & 0xff)
#define PIXEL888BLUE(pixel) (uint8)((pixel) & 0xff)

/* ����ɫ����������
 * ������x,y ����X,Y����
 *		 w,h ���ο��
 *       ulLight ��ɫ
 *       ulDark ��ɫ
 */
void DrawShadeRect_x(int16 x, int16 y, int16 w, int16 h, uint32 ulLight, uint32 ulDark)
{
    int16 i;
    mr_colourSt clrL,clrD;//��ɫ����ɫ
    mr_screenRectSt r;//���ƾ���
    int16 CR,CG,CB;//��ɫ����ɫ��ֵ

    //ȡ�ø���ɫֵ
    clrL.r = PIXEL888RED(ulLight);
    clrL.g = PIXEL888GREEN(ulLight);
    clrL.b = PIXEL888BLUE(ulLight);
    clrD.r = PIXEL888RED(ulDark);
    clrD.g = PIXEL888GREEN(ulDark);
    clrD.b = PIXEL888BLUE(ulDark);
    CR = clrL.r - clrD.r;
    CG = clrL.g - clrD.g;
    CB = clrL.b - clrD.b;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;

    //�������
    for(i = 0; i < r.h; i++)
    {
        mrc_drawLine(r.x, r.y + i, r.x + r.w - 1, r.y + i, clrL.r - (CR * i) / h, clrL.g - (CG * i) / h, clrL.b - (CB * i) / h);
    }
}

/* ����Բ�Ǿ��α߿�
 * ������x,y ����X,Y����
 *		 w,h ���ο��
 *		 ulFrame �߿���ɫ
 */
void DrawRoundRect(int16 x, int16 y, int16 w, int16 h, uint32 ulFrame)
{
    mr_screenRectSt r;//���ƾ���
    mr_colourSt clrF;//�߿�ɫ

    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    clrF.r = PIXEL888RED(ulFrame);
    clrF.g = PIXEL888GREEN(ulFrame);
    clrF.b = PIXEL888BLUE(ulFrame);

    //���Ʊ߿�
    mrc_drawLine(r.x + 1, r.y, r.x + r.w - 2, r.y, clrF.r, clrF.g, clrF.b);
    mrc_drawLine(r.x + 1, r.y + r.h - 1, r.x + r.w - 2, r.y + r.h - 1, clrF.r, clrF.g, clrF.b);
    mrc_drawLine(r.x, r.y + 1, r.x, r.y + r.h - 2, clrF.r, clrF.g, clrF.b);
    mrc_drawLine(r.x + r.w - 1, r.y + 1, r.x + r.w - 1, r.y + r.h - 2, clrF.r, clrF.g, clrF.b);
}

//�������м���ʾ����Unicode����
//�������������֣���ɫ
void DrawTextMid(mr_screenRectSt drawRect, char *drawText, uint32 ulColor)
{
    int32 fw, fh;
    mr_colourSt tColor;

    tColor.r = PIXEL888RED(ulColor);
    tColor.g = PIXEL888GREEN(ulColor);
    tColor.b = PIXEL888BLUE(ulColor);
    //mrc_unicodeTextWidthHeight((uint16*)drawText, MR_FONT_MEDIUM, &fw, &fh);
    mrc_textWidthHeight(drawText, 1,MR_FONT_MEDIUM, &fw, &fh);
    mrc_drawTextLeft(drawText, (int16)(drawRect.x + (drawRect.w - fw) / 2), (int16)(drawRect.y + (drawRect.h - fh) / 2), drawRect, tColor, 0, MR_FONT_MEDIUM);
}

//�������м���ʾ����GB2312����
//�������������֣���ɫ
void DrawTextMidA(mr_screenRectSt drawRect, char *drawText, uint32 ulColor)
{
    int32 fw, fh;
    mr_colourSt tColor;

    tColor.r = PIXEL888RED(ulColor);
    tColor.g = PIXEL888GREEN(ulColor);
    tColor.b = PIXEL888BLUE(ulColor);
    mrc_textWidthHeight(drawText, 0, MR_FONT_MEDIUM, &fw, &fh);
    mrc_drawTextEx(drawText, (int16)(drawRect.x + (drawRect.w - fw) / 2), (int16)(drawRect.y + (drawRect.h - fh) / 2), drawRect, tColor, 0, MR_FONT_MEDIUM);
}

//���ư�ť���������������֣���ɫ1����ɫ2��״̬���Ƿ�ˢ��
void DrawCmd(mr_screenRectSt cmdRect, char *Tile, uint32 ulLight, uint32 ulDark, int32 IsPress, int32 Reflush)
{
    DrawRoundRect(cmdRect.x, cmdRect.y, cmdRect.w, cmdRect.h, ulDark);
    DrawShadeRect_x(cmdRect.x + 1, cmdRect.y + 1, cmdRect.w - 2, cmdRect.h - 2, (0 == IsPress) ? ulLight : ulDark, (0 == IsPress) ? ulDark : ulLight);
    DrawTextMidA(cmdRect, Tile, 0x00ffffff);

    if (Reflush > 0)
    {
        mrc_refreshScreen(cmdRect.x, cmdRect.y, cmdRect.w, cmdRect.h);
    }
}