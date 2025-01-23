// ��Ϸ����ʾ��һ��/ʧ��/ʤ������
#include "win_score.h"
#include "mrc_http.h"

HTTP get_score;

void scoreWinEvent(int32 data, int32 eventId)
{
    static int32 send = -1;
    mrc_printf("scoreWinEvent(%d, %d)\n", data, eventId);
    switch (eventId)
    {
    case WIN_EVENT_SHOW:
        initScoreWin();
        break;

    case WIN_EVENT_UPDATE:
        if (get_score.connect_ok != MR_SUCCESS)
            mrc_http_connect(&get_score);
        if (send == -1)
            send = mrc_http_send(&get_score);
        showScoreWin();
        break;
    }
}

void scoreKeyEvent(int32 data, int32 code, int32 param0, int32 param1)
{
    mrc_sendEventTo(scoreWinPtr, WIN_EVENT_UPDATE);
}

void get_score_cb()
{
}


void getScoreList()
{
    mrc_http_set_recvcb(&get_score, get_score_cb);
    mrc_http_get_init("http://sai.yzjlb.net/mrp/dsm.php?score=up3&appid=30089&name=2b&value=10", &get_score);
}

void closeScoreWin()
{
    mrc_winCloseById(scoreWinPtr);
}

void showScoreWin()
{
    int32 len = 0;
    char *unistr;
    mr_screenRectSt rect = {0, 0, 240, 320};
    mr_colourSt color = {0, 0, 0};
    mrc_clearScreen(255, 255, 255);

    if (get_score.data_len > 0)
    {
        len = mrc_strlen(get_score.data);
        unistr = malloc(sizeof(char) * len * 2);
        UTF8ToUni(get_score.data, unistr, len * 2);
        mrc_drawTextEx(unistr, 0, 0, rect, color, DRAW_TEXT_EX_IS_UNICODE | DRAW_TEXT_EX_IS_AUTO_NEWLINE, MR_FONT_MEDIUM);
        mrc_free(unistr);
    }
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

void initScoreWin()
{
    set_text_color(0x0);
    getScoreList();
}
