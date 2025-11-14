#include <mrc_base.h>
#include "mpc.h"
#include "mrc_graphics.h"
#include "logo.h"

#define LOGO_COUNT 20
static int logo_index;
static BITMAP_565 *logo;
static int32 logo_timer;
void logo_timerCd(int32 data)
{
    int alpha;

    alpha = 255 * logo_index / LOGO_COUNT;

    cls(250, 250, 250);
    drawBitmapAlpha(logo, (SCRW - logo->width) / 2, (SCRW - logo->height) / 2, alpha);
    ref(0, 0, SCRW, SCRH);
    if (logo_index == LOGO_COUNT)
    {
        timerstop(logo_timer);
        timerdel(logo_timer);
        bitmapFree(logo);
        sleep(2000);
        mrc_event(LOGO_EVENT, 0, 0);
    }
    logo_index++;
}

void logo_draw(void)
{
    logo_index = 0;
    logo_timer = timercreate();
    timerstart(logo_timer, 100, 1, logo_timerCd, 1);
    logo = readBitmap565FromAssets("logo.bma");
}