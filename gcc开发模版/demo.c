
#include "mrc_base.h"
#include "mpc.h"
#include "mrc_graphics.h"
#include "display_object.h"

int32 mrc_init(void);
int32 mrc_pause(void);
int32 mrc_resume(void);
int32 mrc_exitApp(void);
int32 mrc_event(int32 code, int32 p0, int32 p1);




///////////////////////////////////////////////////////////////////////////////////////////

#define MAKERGB(r, g, b) (uint16)(((uint32)(r >> 3) << 11) | ((uint32)(g >> 2) << 5) | ((uint32)(b >> 3)))
#define BMPW 16
#define BMPH 16
BITMAP_565 *logo;
int32 timer;
int32 rotate = 1;
Stage *stage;
Bitmap *bitmap;
Shape *shape;

void timer_run(int32 data){
    mrc_clearScreen(0, 0, 0);
    // gl_clearScreen(0, 0, 0);
    // mrc_drawText("sample.c event test", 0, 30, 255, 255, 255, 0, 1);
    drawBitmap565(logo, rotate, 20);
    // gl_drawRotatedHollowRect drawBitmapRotate gl_drawRotatedRect
    // gl_drawRotatedRect(50, 50, 32, 50, 16, 25, rotate, 0xffff0000);
    // gl_drawRotatedHollowRect(50, 50, 32, 50, 16, 25, rotate, 0xffffffff);
    // drawBitmapRotate(logo, SCRW/2, SCRH/2, logo->width/2, logo->height/2, rotate);
    mrc_printf("render....");
    stage->render(stage);
    bitmap->setX(bitmap, rotate);
    bitmap->base.skewX += 1;
    if(rotate > 200){
        rotate = -100;
    }
    if(bitmap->base.skewX > 10){
        bitmap->base.skewX = 0;
    }
    bitmap->base.scaleX = 0.5;
    bitmap->base.scaleY = 0.5;
    bitmap->base.alpha -= 1;
    if(bitmap->base.alpha <10){
        bitmap->base.alpha = 255;
    }
    shape->base.rotation += 1;

    // drawBitmap565(logo, (SCRW - logo->width)/2, (SCRH - logo->height)/2);
    // testBmp();
    mrc_refreshScreen(0, 0, SCRW, SCRH);

    
    
    // bitmap->setAnchorOffset(bitmap, bitmap->base.width/2, bitmap->base.height/2);
    // bitmap->setRotation(bitmap, bitmap->base.rotation + 2);

    rotate += 5;
}

int32 mrc_init(void) {
    mrc_printf("mrc_init...");
    mpc_init();
    mrc_printf("create....");
    stage = egret_Stage_create();
    mrc_printf("bitmap create");
    bitmap = egret_Bitmap_create();
    mrc_printf("logo...");

    logo = readBitmapFromAssets("logo_240.png");
    // logo->mode = BM_COPY;
    mrc_printf("setData.....");
    bitmap->setBitmapData(bitmap, logo);
    mrc_printf("setX.....");
    DisplayObject_setX(bitmap, 32);
    DisplayObject_setY(bitmap, 120);
    mrc_printf("add..........");
    Sprite_addChild(stage, bitmap);
    shape = egret_Shape_create();
    stage->addChild(stage, shape);
    shape->beginFill(shape, 0xffff0000, 255);
    shape->drawCircle(shape, 32, 32, 15);
    shape->drawRect(shape, 0, 0, 32, 32);
    shape->endFill(shape);
    timer = mrc_timerCreate();
    // timer_run(0);
    mrc_timerStart(timer, 3000, 1, timer_run, 1);

    return 0;
}

int32 mrc_pause(void) {
    return 0;
}

int32 mrc_resume(void) {
    return 0;
}

int32 mrc_exitApp(void) {
    bitmapFree(logo);
    mrc_timerStop(timer);
    mrc_timerDelete(timer);
    egret_destroy();
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
        if(p0 == MR_KEY_1){
            mrc_runMrp("mrpshop.mrp", "start.mr", NULL);
        }
        if (p0 == MR_KEY_SELECT) {
            mrc_exit();
        }
    }
    return 0;
}
