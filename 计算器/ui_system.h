#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include <mrc_base.h>
#include "uc3_font.h"

/* 窗口类型枚举 */
typedef enum {
    WINDOW_CALCULATOR = 0,    /* 计算器主窗口 */
    WINDOW_HISTORY = 1,       /* 历史记录窗口 */
    WINDOW_HELP = 2           /* 帮助窗口 */
} WindowType;

/* 按钮结构体 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
    char label[10];
    int type;  /* 0:数字, 1:操作符, 2:功能, 3:等号, 4:清除 */
} Button;

/* 历史记录项 */
typedef struct {
    char expression[100];
    char result[50];
} HistoryItem;

/* 全局变量声明 */
extern int currentWindow;
extern int screenWidth;
extern int screenHeight;

/* 函数声明 */
void initUI(void);
void switchWindow(int windowType);
void drawSoftKeyHints(char* leftText, char* rightText);
void drawCalculatorWindow(void);
void drawHistoryWindow(void);
void drawHelpWindow(void);
void handleCalculatorEvent(int32 code, int32 param0, int32 param1);
void handleHistoryEvent(int32 code, int32 param0, int32 param1);
void handleHelpEvent(int32 code, int32 param0, int32 param1);

#endif /* UI_SYSTEM_H */