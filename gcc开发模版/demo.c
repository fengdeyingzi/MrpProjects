
#include "bridge.h"
#include "mrc_base.h"
#include "mpc.h"
int32 mrc_init(void);
int32 mrc_pause(void);
int32 mrc_resume(void);
int32 mrc_exitApp(void);
int32 mrc_event(int32 code, int32 p0, int32 p1);




///////////////////////////////////////////////////////////////////////////////////////////

#define MAKERGB(r, g, b) (uint16)(((uint32)(r >> 3) << 11) | ((uint32)(g >> 2) << 5) | ((uint32)(b >> 3)))
#define BMPW 16
#define BMPH 16

// void testBmp() {
//     char *bmp;
//     int32 bmpLen;
//     bmp = inFuncs->mrc_readFileFromMrp("test.bmp", &bmpLen, 0);
//     if (bmp) {
//         mr_table->_DrawBitmap((uint16 *)bmp, 0, 0, BMPW, BMPH, BM_TRANSPARENT, MAKERGB(0xff, 0x00, 0xff), 0, 0, BMPW);
//         inFuncs->mrc_freeFileData(bmp, bmpLen);
//     }
// }

int32 mrc_init(void) {
    mrc_printf("test................");
    mrc_printf("===============");
    mrc_clearScreen(0, 0, 0);
    mrc_drawText("sample.c event test", 0, 30, 255, 255, 255, 0, 1);
    // testBmp();
    mrc_refreshScreen(0, 0, SCRW, SCRH);
    return 0;
}

int32 mrc_pause(void) {
    return 0;
}

int32 mrc_resume(void) {
    return 0;
}

int32 mrc_exitApp(void) {
    return 0;
}

int32 mrc_event(int32 code, int32 p0, int32 p1) {
    if (code == MR_MOUSE_DOWN || code == MR_MOUSE_MOVE) {
        char text[40];

        mrc_clearScreen(0xff, 0xff, 0xff);
        mrc_drawRect(p0, 0, 1, SCRH, 0xff, 0, 0);
        mrc_drawRect(0, p1, SCRW, 1, 0xff, 0, 0);
        mrc_sprintf(text, "x=%d,y=%d", p0, p1);
        mrc_drawText(text, 0, 0, 255, 0, 0, 0, 1);
        mrc_refreshScreen(0, 0, SCRW, SCRH);

    } else if (code == MR_KEY_RELEASE) {
        if (p0 == MR_KEY_SELECT) {
            mrc_exit();
        }
    }
    return 0;
}
