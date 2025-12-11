#include <mrc_base.h>
#include "uc3_font.h"
#include "mpc.h"
#include "mrc_graphics.h"
#include "xl_debug.h"
#include "ui_system.h"
#include "calculator.h"

int32 timer_cd;

/* 定时器回调函数 */
void timer_run(int32 id)
{
    /* 根据当前窗口刷新显示 */
    switch(currentWindow) {
        case WINDOW_CALCULATOR:
            drawCalculatorWindow();
            break;
        case WINDOW_HISTORY:
            drawHistoryWindow();
            break;
        case WINDOW_HELP:
            drawHelpWindow();
            break;
    }
    return;
}

/* 应用程序初始化 */
int32 mrc_init(void)
{
    LOG_MSG("mpc init");
    mpc_init();

    LOG_MSG("uc3 init");
    uc3_init();
    
    LOG_MSG("初始化计算器");
    initCalculator();
    
    LOG_MSG("初始化UI系统");
    initUI();
    
    LOG_MSG("绘制计算器窗口");
    drawCalculatorWindow();

    LOG_MSG("创建定时器");
    timer_cd = mrc_timerCreate();
    mrc_timerStart(timer_cd, 100, 0, timer_run, 0);  /* 10fps刷新率 */

    return 0;
}

/* 事件处理 */
int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    /* 根据当前窗口分发事件 */
    switch(currentWindow) {
        case WINDOW_CALCULATOR:
            handleCalculatorEvent(code, param0, param1);
            break;
        case WINDOW_HISTORY:
            handleHistoryEvent(code, param0, param1);
            break;
        case WINDOW_HELP:
            handleHelpEvent(code, param0, param1);
            break;
    }
    return 0;
}

/* 应用程序暂停 */
int32 mrc_pause() { 
    LOG_MSG("应用程序暂停");
    return 0; 
}

/* 应用程序恢复 */
int32 mrc_resume() { 
    LOG_MSG("应用程序恢复");
    /* 恢复时重绘当前窗口 */
    switch(currentWindow) {
        case WINDOW_CALCULATOR:
            drawCalculatorWindow();
            break;
        case WINDOW_HISTORY:
            drawHistoryWindow();
            break;
        case WINDOW_HELP:
            drawHelpWindow();
            break;
    }
    return 0; 
}

/* 应用程序退出 */
int32 mrc_exitApp()
{
    LOG_MSG("应用程序退出");
    
    /* 停止定时器 */
    mrc_timerStop(timer_cd);
    mrc_timerDelete(timer_cd);
    
    /* 释放字体 */
    uc3_free();
    
    LOG_MSG("应用程序退出完成");
    return 0;
}

/* 扩展应用程序事件接收 */
int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
    mrc_printf("mrc_extRecvAppEvent: app=%d, code=%d", app, code);
    return 0;
}

/* 扩展应用程序事件接收(扩展版) */
int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3,
                            int32 p4, int32 p5)
{
    mrc_printf("mrc_extRecvAppEventEx: code=%d", code);
    return 0;
}