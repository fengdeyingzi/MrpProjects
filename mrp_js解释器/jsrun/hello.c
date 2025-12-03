
#include <stdlib.h>
#include "mujs.h"
#include <mrc_base.h>
#include <mrc_base_i.h>
#include "mrc_graphics.h"
#include "uc3_font.h"
#include "fopen.h"
#include "mrc_android.h"
#include "mpc.h"
#include "xl_coding.h"


/* 声明在jslex.c中定义的keywords相关变量 */
extern char keywords_storage[29][12];
extern const char *keywords[29];

/* 声明初始化函数 */
extern void jsY_inittokenstring(void);
extern void jsP_initnames(void);
extern void jsC_initfuturewords(void);
extern void jscapp_init(void);

/* 定时器回调支持 */
#define MAX_TIMERS 16
typedef struct {
    int32 timer_id;
    js_State *J;
    char callback_name[64];
    int in_use;
} TimerCallback;

static TimerCallback timer_callbacks[MAX_TIMERS];
static js_State *global_J = NULL;

static void hello(js_State *J)
{
	const char *name = js_tostring(J, 1);
	mrc_printf("Hello,影子  %s!\n", name);
    mrc_clearScreen(255, 255, 255);
    mrc_drawText("test", 0, 0, 20, 20, 20, 0, 1);
    mrc_refreshScreen(0, 0, SCRW, SCRH);
	js_pushundefined(J);
}

void js_drect(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int dw = js_toint32(J,3);
    int dh = js_toint32(J,4);
    int dr = js_toint32(J,5);
    int dg = js_toint32(J,6);
    int db = js_toint32(J,7);
    mrc_printf("drect(%d,%d,%d,%d,%d,%d,%d);\n",dx,dy,dw,dh,dr,dg,db);
    js_pushundefined(J);
}

void js_dline(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int dw = js_toint32(J,3);
    int dh = js_toint32(J,4);
    int dr = js_toint32(J,5);
    int dg = js_toint32(J,6);
    int db = js_toint32(J,7);
    mrc_printf("dline(%d,%d,%d,%d,%d,%d,%d);\n",dx,dy,dw,dh,dr,dg,db);
    js_pushundefined(J);
}


void js_dpointex(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int dr = js_toint32(J,3);
    int dg = js_toint32(J,4);
    int db = js_toint32(J,5);
    mrc_printf("dpointex(%d,%d,%d,%d,%d);\n",dx,dy,dr,dg,db);
    js_pushundefined(J);
}

void js_dtext(js_State *J){
    const char *text = js_tostring(J,1);
    int dx = js_toint32(J,2);
    int dy = js_toint32(J,3);
    int dr = js_toint32(J,4);
    int dg = js_toint32(J,5);
    int db = js_toint32(J,6);
    int font = js_toint32(J,7);
    mrc_printf("dtext(%s,%d,%d,%d,%d,%d,%d);\n",text,dx,dy,dr,dg,db,font);
    js_pushundefined(J);
}



void js_drawCir(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    int r = js_toint32(J,3);
    uint32 color = js_toint32(J,4);
    gl_drawCir(dx, dy, r, color);
    js_pushundefined(J);
}

void js_drawLine(js_State *J){
    int x1 = js_toint32(J,1);
    int y1 = js_toint32(J,2);
    int x2 = js_toint32(J,3);
    int y2 = js_toint32(J,4);
    uint32 color = js_toint32(J,5);
    gl_drawLine(x1, y1, x2, y2, color);
    js_pushundefined(J);
}

void js_drawPoint(js_State *J){
    int dx = js_toint32(J,1);
    int dy = js_toint32(J,2);
    uint32 color = js_toint32(J,3);
    gl_drawLine(dx, dy, dx, dy, color);  // 画点就是画一个像素的线
    js_pushundefined(J);
}

void js_drawTriangle(js_State *J){
    int x1 = js_toint32(J,1);
    int y1 = js_toint32(J,2);
    int x2 = js_toint32(J,3);
    int y2 = js_toint32(J,4);
    int x3 = js_toint32(J,5);
    int y3 = js_toint32(J,6);
    int color = js_toint32(J,7);
    gl_drawTriangle(x1, y1, x2, y2, x3, y3, color);
    js_pushundefined(J);
}

// ===== 新增图形绘制函数 =====

void js_clearScreen(js_State *J){
    int r = js_toint32(J,1);
    int g = js_toint32(J,2);
    int b = js_toint32(J,3);
    mrc_printf("js_clearScreen");
    // gl_clearScreen(r, g, b);
    js_pushundefined(J);
}

void js_drawRect(js_State *J){
    int x = js_toint32(J,1);
    int y = js_toint32(J,2);
    int w = js_toint32(J,3);
    int h = js_toint32(J,4);
    uint32 color = js_toint32(J,5);
    gl_drawRect(x, y, w, h, color);
    js_pushundefined(J);
}

void js_drawHollowRect(js_State *J){
    int x = js_toint32(J,1);
    int y = js_toint32(J,2);
    int width = js_toint32(J,3);
    int height = js_toint32(J,4);
    uint32 color = js_toint32(J,5);
    gl_drawHollowRect(x, y, width, height, color);
    js_pushundefined(J);
}

void js_drawHollowCir(js_State *J){
    int x = js_toint32(J,1);
    int y = js_toint32(J,2);
    int r = js_toint32(J,3);
    uint32 color = js_toint32(J,4);
    gl_drawHollowCir(x, y, r, color);
    js_pushundefined(J);
}

void js_drawHollowTriangle(js_State *J){
    int x1 = js_toint32(J,1);
    int y1 = js_toint32(J,2);
    int x2 = js_toint32(J,3);
    int y2 = js_toint32(J,4);
    int x3 = js_toint32(J,5);
    int y3 = js_toint32(J,6);
    uint32 color = js_toint32(J,7);
    gl_drawHollowTriangle(x1, y1, x2, y2, x3, y3, color);
    js_pushundefined(J);
}

void js_drawPolygon(js_State *J){
    int count;
    int *points;
    uint32 color;
    int i;
    if (!js_isarray(J, 1)) {
        js_error(J, "drawPolygon: first parameter must be an array");
        return;
    }
    count = js_toint32(J, 2);
    color = js_toint32(J, 3);

    points = (int*)malloc(count * 2 * sizeof(int));
    if (!points) {
        js_error(J, "drawPolygon: memory allocation failed");
        return;
    }

    for (i = 0; i < count * 2; i++) {
        js_getindex(J, 1, i);
        points[i] = js_toint32(J, -1);
        js_pop(J, 1);
    }

    gl_drawPolygon(points, count, color);
    free(points);
    js_pushundefined(J);
}

void js_drawHollowPolygon(js_State *J){
    int count;
    int *points;
    int i;
    uint32 color;
    if (!js_isarray(J, 1)) {
        js_error(J, "drawHollowPolygon: first parameter must be an array");
        return;
    }
    count = js_toint32(J, 2);
    color = js_toint32(J, 3);

    points = (int*)malloc(count * 2 * sizeof(int));
    if (!points) {
        js_error(J, "drawHollowPolygon: memory allocation failed");
        return;
    }

    for (i = 0; i < count * 2; i++) {
        js_getindex(J, 1, i);
        points[i] = js_toint32(J, -1);
        js_pop(J, 1);
    }

    gl_drawHollowPolygon(points, count, color);
    free(points);
    js_pushundefined(J);
}

void js_drawRotatedRect(js_State *J){
    int centerX = js_toint32(J, 1);
    int centerY = js_toint32(J, 2);
    int width = js_toint32(J, 3);
    int height = js_toint32(J, 4);
    int bx = js_toint32(J, 5);
    int by = js_toint32(J, 6);
    double angle = js_tonumber(J, 7);
    uint32 color = js_toint32(J, 8);
    gl_drawRotatedRect(centerX, centerY, width, height, bx, by, angle, color);
    js_pushundefined(J);
}

void js_drawRotatedHollowRect(js_State *J){
    int centerX = js_toint32(J, 1);
    int centerY = js_toint32(J, 2);
    int width = js_toint32(J, 3);
    int height = js_toint32(J, 4);
    int bx = js_toint32(J, 5);
    int by = js_toint32(J, 6);
    double angle = js_tonumber(J, 7);
    uint32 color = js_toint32(J, 8);
    gl_drawRotatedHollowRect(centerX, centerY, width, height, bx, by, angle, color);
    js_pushundefined(J);
}

void js_drawShadeRect(js_State *J){
    int x = js_toint32(J, 1);
    int y = js_toint32(J, 2);
    int w = js_toint32(J, 3);
    int h = js_toint32(J, 4);
    uint32 colorA = js_toint32(J, 5);
    uint32 colorB = js_toint32(J, 6);
    int mode = js_toint32(J, 7);
    drawShadeRect(x, y, w, h, colorA, colorB, mode);
    js_pushundefined(J);
}

void js_refreshScreen(js_State *J){
    int x = js_toint32(J, 1);
    int y = js_toint32(J, 2);
    int w = js_toint32(J, 3);
    int h = js_toint32(J, 4);
    mrc_refreshScreen(x, y, w, h);
    js_pushundefined(J);
}

// ===== 位图操作函数 =====

void js_readBitmapFromAssets(js_State *J){
    const char *filename = js_tostring(J, 1);
    BITMAP_565 *bmp = readBitmap565FromAssets((char*)filename);
    if (bmp) {
        js_pushnumber(J, (double)(intptr_t)bmp);
    } else {
        js_pushnull(J);
    }
}

void js_readBitmap(js_State *J){
    const char *path = js_tostring(J, 1);
    BITMAP_565 *bmp = readBitmap((char*)path);
    if (bmp) {
        js_pushnumber(J, (double)(intptr_t)bmp);
    } else {
        js_pushnull(J);
    }
}

void js_createBitmap(js_State *J){
    int width = js_toint32(J, 1);
    int height = js_toint32(J, 2);
    BITMAP_565 *bmp = createBitmap565(width, height);
    if (bmp) {
        js_pushnumber(J, (double)(intptr_t)bmp);
    } else {
        js_pushnull(J);
    }
}

void js_drawBitmap(js_State *J){
    BITMAP_565 *bmp = (BITMAP_565*)(intptr_t)js_tonumber(J, 1);
    int x = js_toint32(J, 2);
    int y = js_toint32(J, 3);
    if (bmp) {
        drawBitmap(bmp, x, y);
    }
    js_pushundefined(J);
}

void js_drawBitmapFlip(js_State *J){
    BITMAP_565 *bmp = (BITMAP_565*)(intptr_t)js_tonumber(J, 1);
    int x = js_toint32(J, 2);
    int y = js_toint32(J, 3);
    int w = js_toint32(J, 4);
    int h = js_toint32(J, 5);
    int sx = js_toint32(J, 6);
    int sy = js_toint32(J, 7);
    if (bmp) {
        drawBitmapFlip(bmp, x, y, w, h, sx, sy);
    }
    js_pushundefined(J);
}

void js_drawBitmapRegion(js_State *J){
    BITMAP_565 *src = (BITMAP_565*)(intptr_t)js_tonumber(J, 1);
    int x_src = js_toint32(J, 2);
    int y_src = js_toint32(J, 3);
    int width = js_toint32(J, 4);
    int height = js_toint32(J, 5);
    int transform = js_toint32(J, 6);
    int x_dest = js_toint32(J, 7);
    int y_dest = js_toint32(J, 8);
    int anchor = js_toint32(J, 9);
    if (src) {
        drawBitmapRegion(src, x_src, y_src, width, height, transform, x_dest, y_dest, anchor);
    }
    js_pushundefined(J);
}

void js_drawBitmapEx(js_State *J){
    BITMAP_565 *bmp = (BITMAP_565*)(intptr_t)js_tonumber(J, 1);
    int x = js_toint32(J, 2);
    int y = js_toint32(J, 3);
    int w = js_toint32(J, 4);
    int h = js_toint32(J, 5);
    int tx = js_toint32(J, 6);
    int ty = js_toint32(J, 7);
    int tw = js_toint32(J, 8);
    int th = js_toint32(J, 9);
    if (bmp) {
        drawBitmapEx(bmp, x, y, w, h, tx, ty, tw, th);
    }
    js_pushundefined(J);
}

void js_createBitmapFromBitmap(js_State *J){
    BITMAP_565 *newBmp;
    BITMAP_565 *bmp = (BITMAP_565*)(intptr_t)js_tonumber(J, 1);
    int32 width = js_toint32(J, 2);
    int32 height = js_toint32(J, 3);
    if (bmp) {
        newBmp = createBitmapFromBitmap(bmp, width, height);
        if (newBmp) {
            js_pushnumber(J, (double)(intptr_t)newBmp);
        } else {
            js_pushnull(J);
        }
    } else {
        js_pushnull(J);
    }
}

void js_saveBitmap(js_State *J){
    BITMAP_565 *bmp = (BITMAP_565*)(intptr_t)js_tonumber(J, 1);
    const char *filename = js_tostring(J, 2);
    if (bmp && filename) {
        saveBitmap(bmp, filename);
    }
    js_pushundefined(J);
}

void js_bitmapFree(js_State *J){
    BITMAP_565 *bmp = (BITMAP_565*)(intptr_t)js_tonumber(J, 1);
    if (bmp) {
        bitmapFree(bmp);
    }
    js_pushundefined(J);
}

// ===== 字体绘制函数 =====

void js_drawText(js_State *J){
    const char *text = js_tostring(J, 1);
    char *temp;
    int32 len;
    int x = js_toint32(J, 2);
    int y = js_toint32(J, 3);
    int r = js_toint32(J, 4);
    int g = js_toint32(J, 5);
    int b = js_toint32(J, 6);
    len = mrc_strlen(text);
    temp = mrc_malloc(len*2+2);
    UTF8ToUni((char*)text, temp, len);
    mrc_drawText(temp, x, y, r, g, b, 1, 1);
    mrc_free(temp);
    js_pushundefined(J);
}

void js_getWidth(js_State *J){
    int32 w, h;
    char *temp;
    int32 len;
    const char *text = js_tostring(J, 1);
    len = mrc_strlen(text);
    temp = mrc_malloc(len*2+2);
    UTF8ToUni((char*)text, temp, len);

    mrc_textWidthHeight(temp, 1, 1, &w, &h);
    mrc_free(temp);
    js_pushnumber(J, w);
}

void js_getFontHeight(js_State *J){
    int32 w, h;
    char *temp;
    const char *text = js_tostring(J, 1);
    int32 len = mrc_strlen(text);
    temp = mrc_malloc(len*2+2);
    UTF8ToUni((char*)text, temp, len);
    mrc_textWidthHeight(temp, 1, 1, &w, &h);
    mrc_free(temp);
    js_pushnumber(J, h);
}

// ===== 文件操作函数 =====

void js_fopen(js_State *J){
    const char *path = js_tostring(J, 1);
    const char *mode = js_tostring(J, 2);
    FILE *file = fopen(path, mode);
    if (file) {
        js_pushnumber(J, (double)(intptr_t)file);
    } else {
        js_pushnull(J);
    }
}

void js_fread(js_State *J){
    // 暂时不实现完整的 fread，因为需要处理 buffer
    js_error(J, "fread not fully implemented yet");
}

void js_fwrite(js_State *J){
    // 暂时不实现完整的 fwrite，因为需要处理 buffer
    js_error(J, "fwrite not fully implemented yet");
}

void js_fseek(js_State *J){
    FILE *file = (FILE*)(intptr_t)js_tonumber(J, 1);
    long offset = (long)js_tonumber(J, 2);
    int mode = js_toint32(J, 3);
    if (file) {
        int result = fseek(file, offset, mode);
        js_pushnumber(J, result);
    } else {
        js_pushnumber(J, -1);
    }
}

void js_fflush(js_State *J){
    FILE *file = (FILE*)(intptr_t)js_tonumber(J, 1);
    if (file) {
        int result = fflush(file);
        js_pushnumber(J, result);
    } else {
        js_pushnumber(J, -1);
    }
}

void js_fclose(js_State *J){
    FILE *file = (FILE*)(intptr_t)js_tonumber(J, 1);
    if (file) {
        int result = fclose(file);
        js_pushnumber(J, result);
    } else {
        js_pushnumber(J, -1);
    }
}

// ===== 资源读取函数 =====

void js_readFileFromAssets(js_State *J){
    const char *filename = js_tostring(J, 1);
    int32 len = 0;
    void *data = mrc_readFileFromAssets(filename, &len);
    if (data) {
        // 将数据转换为字符串返回
        js_pushlstring(J, (const char*)data, len);
        mrc_freeFileFromAssets(data, len);
    } else {
        js_pushnull(J);
    }
}

void js_freeFileFromAssets(js_State *J){
    // 由于在 readFileFromAssets 中已经释放，这个函数暂时不需要实现
    js_pushundefined(J);
}

// ===== 定时器函数 =====

/* 通用的 C 定时器回调函数 */
void timer_c_callback(int32 data) {
    int i;
    TimerCallback *tc = NULL;

    /* 查找定时器回调 */
    for (i = 0; i < MAX_TIMERS; i++) {
        if (timer_callbacks[i].in_use && timer_callbacks[i].timer_id == data) {
            tc = &timer_callbacks[i];
            break;
        }
    }

    if (tc && tc->J && tc->callback_name[0] != '\0') {
        /* 调用 JavaScript 回调函数 */
        js_getglobal(tc->J, tc->callback_name);
        if (js_iscallable(tc->J, -1)) {
            js_pushnumber(tc->J, data);  /* 传递定时器ID作为参数 */
            if (js_pcall(tc->J, 1)) {
                /* 调用失败 */
                const char *err = js_tostring(tc->J, -1);
                mrc_printf("Timer callback error: %s\n", err);
                js_pop(tc->J, 1);
            }
        }
        js_pop(tc->J, 1);
    }
}

void js_timerCreate(js_State *J){
    int32 timer = mrc_timerCreate();
    js_pushnumber(J, timer);
}

void js_timerStart(js_State *J){
    int32 timer_id = js_toint32(J, 1);
    int32 time = js_toint32(J, 2);
    const char *callback_name = js_tostring(J, 3);
    int32 loop = js_toint32(J, 4);
    int i;
    int32 result;
    int slot = -1;

    /* 查找空闲槽位或已存在的定时器 */
    for (i = 0; i < MAX_TIMERS; i++) {
        if (timer_callbacks[i].in_use && timer_callbacks[i].timer_id == timer_id) {
            slot = i;
            break;
        }
        if (!timer_callbacks[i].in_use && slot == -1) {
            slot = i;
        }
    }

    if (slot == -1) {
        js_error(J, "Too many timers (max %d)", MAX_TIMERS);
        return;
    }

    /* 保存回调信息 */
    timer_callbacks[slot].timer_id = timer_id;
    timer_callbacks[slot].J = J;
    strncpy(timer_callbacks[slot].callback_name, callback_name, 63);
    timer_callbacks[slot].callback_name[63] = '\0';
    timer_callbacks[slot].in_use = 1;

    /* 启动定时器，使用 timer_id 作为 data 参数 */
    result = mrc_timerStart(timer_id, time, timer_id, timer_c_callback, loop);
    js_pushnumber(J, result);
}

void js_timerStop(js_State *J){
    int32 timer = js_toint32(J, 1);
    int i;

    mrc_timerStop(timer);

    /* 清除回调信息 */
    for (i = 0; i < MAX_TIMERS; i++) {
        if (timer_callbacks[i].in_use && timer_callbacks[i].timer_id == timer) {
            timer_callbacks[i].in_use = 0;
            timer_callbacks[i].callback_name[0] = '\0';
            break;
        }
    }

    js_pushundefined(J);
}

void js_timerDelete(js_State *J){
    int32 timer = js_toint32(J, 1);
    int i;

    mrc_timerDelete(timer);

    /* 清除回调信息 */
    for (i = 0; i < MAX_TIMERS; i++) {
        if (timer_callbacks[i].in_use && timer_callbacks[i].timer_id == timer) {
            timer_callbacks[i].in_use = 0;
            timer_callbacks[i].callback_name[0] = '\0';
            break;
        }
    }

    js_pushundefined(J);
}





void drawPrint(char *text){
    mrc_clearScreen(255,255,255);
	mrc_drawText(text,0,0, 20,20,20, 0, 1);
	mrc_refreshScreen(0,0,SCRW,SCRH);
    mrc_sleep(300);
    mrc_printf("drawPrint %s",text);
}


int32 mrc_init()
{
    js_State *J = NULL;
    int i;

    /* ===== 最重要：先初始化字符串内部化系统 ===== */
    jsV_init();
    jsS_init();

    /* 初始化各种字符串查找表 */
    jsY_inittokenstring();
    jsP_initnames();
    jsC_initfuturewords();
    jscapp_init();

    /* 初始化keywords数组 */
    strcpy(keywords_storage[0], "break");
    strcpy(keywords_storage[1], "case");
    strcpy(keywords_storage[2], "catch");
    strcpy(keywords_storage[3], "continue");
    strcpy(keywords_storage[4], "debugger");
    strcpy(keywords_storage[5], "default");
    strcpy(keywords_storage[6], "delete");
    strcpy(keywords_storage[7], "do");
    strcpy(keywords_storage[8], "else");
    strcpy(keywords_storage[9], "false");
    strcpy(keywords_storage[10], "finally");
    strcpy(keywords_storage[11], "for");
    strcpy(keywords_storage[12], "function");
    strcpy(keywords_storage[13], "if");
    strcpy(keywords_storage[14], "in");
    strcpy(keywords_storage[15], "instanceof");
    strcpy(keywords_storage[16], "new");
    strcpy(keywords_storage[17], "null");
    strcpy(keywords_storage[18], "return");
    strcpy(keywords_storage[19], "switch");
    strcpy(keywords_storage[20], "this");
    strcpy(keywords_storage[21], "throw");
    strcpy(keywords_storage[22], "true");
    strcpy(keywords_storage[23], "try");
    strcpy(keywords_storage[24], "typeof");
    strcpy(keywords_storage[25], "var");
    strcpy(keywords_storage[26], "void");
    strcpy(keywords_storage[27], "while");
    strcpy(keywords_storage[28], "with");

    /* 设置指针数组指向存储数组 */
    keywords[0] = keywords_storage[0];
    keywords[1] = keywords_storage[1];
    keywords[2] = keywords_storage[2];
    keywords[3] = keywords_storage[3];
    keywords[4] = keywords_storage[4];
    keywords[5] = keywords_storage[5];
    keywords[6] = keywords_storage[6];
    keywords[7] = keywords_storage[7];
    keywords[8] = keywords_storage[8];
    keywords[9] = keywords_storage[9];
    keywords[10] = keywords_storage[10];
    keywords[11] = keywords_storage[11];
    keywords[12] = keywords_storage[12];
    keywords[13] = keywords_storage[13];
    keywords[14] = keywords_storage[14];
    keywords[15] = keywords_storage[15];
    keywords[16] = keywords_storage[16];
    keywords[17] = keywords_storage[17];
    keywords[18] = keywords_storage[18];
    keywords[19] = keywords_storage[19];
    keywords[20] = keywords_storage[20];
    keywords[21] = keywords_storage[21];
    keywords[22] = keywords_storage[22];
    keywords[23] = keywords_storage[23];
    keywords[24] = keywords_storage[24];
    keywords[25] = keywords_storage[25];
    keywords[26] = keywords_storage[26];
    keywords[27] = keywords_storage[27];
    keywords[28] = keywords_storage[28];

    mpc_init();

	mrc_clearScreen(255,255,255);
	mrc_drawText("helloworld",0,0, 20,20,20, 0, 1);
	mrc_refreshScreen(0,0,SCRW,SCRH);
    mrc_sleep(100);

    /* 初始化定时器回调表 */
    for (i = 0; i < MAX_TIMERS; i++) {
        timer_callbacks[i].timer_id = 0;
        timer_callbacks[i].J = NULL;
        timer_callbacks[i].callback_name[0] = '\0';
        timer_callbacks[i].in_use = 0;
    }

    J = js_newstate(NULL, NULL, JS_STRICT);
    global_J = J;  /* 保存全局 JavaScript 状态指针 */

    // ===== 注册全局变量 =====
    js_pushnumber(J, SCRW);
    js_setglobal(J, "SCRW");
    js_pushnumber(J, SCRH);
    js_setglobal(J, "SCRH");

    // ===== 图形绘制函数 =====
    

    js_newcfunction(J, js_drawRect, "drawRect", 5);
    js_setglobal(J, "drawRect");

    js_newcfunction(J, js_drawHollowRect, "drawHollowRect", 5);
    js_setglobal(J, "drawHollowRect");

    js_newcfunction(J, js_drawLine, "drawLine", 5);
    js_setglobal(J, "drawLine");

    js_newcfunction(J, js_drawCir, "drawCir", 4);
    js_setglobal(J, "drawCir");

    js_newcfunction(J, js_drawHollowCir, "drawHollowCir", 4);
    js_setglobal(J, "drawHollowCir");

    js_newcfunction(J, js_drawTriangle, "drawTriangle", 7);
    js_setglobal(J, "drawTriangle");

    js_newcfunction(J, js_drawHollowTriangle, "drawHollowTriangle", 7);
    js_setglobal(J, "drawHollowTriangle");

    js_newcfunction(J, js_drawPolygon, "drawPolygon", 3);
    js_setglobal(J, "drawPolygon");

    js_newcfunction(J, js_drawHollowPolygon, "drawHollowPolygon", 3);
    js_setglobal(J, "drawHollowPolygon");

    js_newcfunction(J, js_drawRotatedRect, "drawRotatedRect", 8);
    js_setglobal(J, "drawRotatedRect");

    js_newcfunction(J, js_drawRotatedHollowRect, "drawRotatedHollowRect", 8);
    js_setglobal(J, "drawRotatedHollowRect");

    js_newcfunction(J, js_drawShadeRect, "drawShadeRect", 7);
    js_setglobal(J, "drawShadeRect");

    js_newcfunction(J, js_refreshScreen, "ref", 4);
    js_setglobal(J, "ref");
/*
    // ===== 位图操作函数 =====
    js_newcfunction(J, js_readBitmapFromAssets, "readBitmapFromAssets", 1);
    js_setglobal(J, "readBitmapFromAssets");

    js_newcfunction(J, js_readBitmap, "readBitmap", 1);
    js_setglobal(J, "readBitmap");

    js_newcfunction(J, js_createBitmap, "createBitmap", 2);
    js_setglobal(J, "createBitmap");

    js_newcfunction(J, js_drawBitmap, "drawBitmap", 3);
    js_setglobal(J, "drawBitmap");

    js_newcfunction(J, js_drawBitmapFlip, "drawBitmapFlip", 7);
    js_setglobal(J, "drawBitmapFlip");

    js_newcfunction(J, js_drawBitmapRegion, "drawBitmapRegion", 9);
    js_setglobal(J, "drawBitmapRegion");

    js_newcfunction(J, js_drawBitmapEx, "drawBitmapEx", 9);
    js_setglobal(J, "drawBitmapEx");

    js_newcfunction(J, js_createBitmapFromBitmap, "createBitmapFromBitmap", 3);
    js_setglobal(J, "createBitmapFromBitmap");

    js_newcfunction(J, js_saveBitmap, "saveBitmap", 2);
    js_setglobal(J, "saveBitmap");

    js_newcfunction(J, js_bitmapFree, "bitmapFree", 1);
    js_setglobal(J, "bitmapFree");

    // ===== 字体绘制函数 =====
    js_newcfunction(J, js_drawText, "drawText", 6);
    js_setglobal(J, "drawText");

    js_newcfunction(J, js_getWidth, "getWidth", 1);
    js_setglobal(J, "getWidth");

    js_newcfunction(J, js_getFontHeight, "getFontHeight", 0);
    js_setglobal(J, "getFontHeight");

    // ===== 文件操作函数 =====
    js_newcfunction(J, js_fopen, "fopen", 2);
    js_setglobal(J, "fopen");

    js_newcfunction(J, js_fseek, "fseek", 3);
    js_setglobal(J, "fseek");

    js_newcfunction(J, js_fflush, "fflush", 1);
    js_setglobal(J, "fflush");

    js_newcfunction(J, js_fclose, "fclose", 1);
    js_setglobal(J, "fclose");

    // ===== 资源读取函数 =====
    js_newcfunction(J, js_readFileFromAssets, "readFileFromAssets", 1);
    js_setglobal(J, "readFileFromAssets");

    js_newcfunction(J, js_freeFileFromAssets, "freeFileFromAssets", 2);
    js_setglobal(J, "freeFileFromAssets");

    // ===== 定时器函数 =====
    js_newcfunction(J, js_timerCreate, "timerCreate", 0);
    js_setglobal(J, "timerCreate");

    js_newcfunction(J, js_timerStart, "timerStart", 4);
    js_setglobal(J, "timerStart");

    js_newcfunction(J, js_timerStop, "timerStop", 1);
    js_setglobal(J, "timerStop");

    js_newcfunction(J, js_timerDelete, "timerDelete", 1);
    js_setglobal(J, "timerDelete");
*/
    // ===== 测试代码 =====
	js_newcfunction(J, hello, "hello", 1);
    js_setglobal(J, "hello");
    js_newcfunction(J, js_clearScreen, "clearScreen", 3);
    js_setglobal(J, "clearScreen");

	/* 测试简单的代码 */
	js_dostring(J, "clearScreen(1, 1, 1);");

	js_freestate(J);

	return 0;
}

int32 mrc_event(int32 type, int32 p0, int32 p1)
{
    if(type == MR_KEY_UP){
        mrc_exit();
    }
	return 0;
}
int32 mrc_pause()
{
	return 0;
}
int32 mrc_resume()
{
	return 0;
}
int32 mrc_exitApp()
{
	return 0;
}

 int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1){
	return 0;
 }
 int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5){
	return 0;
}

