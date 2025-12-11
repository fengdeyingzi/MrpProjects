#include "ui_system.h"
#include "calculator.h"
#include "mrc_graphics.h"
#include <mrc_base.h>
#include "mpc.h"


/* 全局变量 */
int currentWindow = WINDOW_CALCULATOR;
int screenWidth = 240;  /* 默认值，会在初始化时更新 */
int screenHeight = 320; /* 默认值，会在初始化时更新 */

/* 按钮定义 */
Button calc_buttons[20];
int button_count = 0;

/* UI初始化 */
/* UI初始化 */
void initUI(void) {
    int i;
    
    /* 获取屏幕尺寸 */
    screenWidth = SCRW;
    screenHeight = SCRH;
    
    /* 初始化计算器按钮 */
    button_count = 0;
    i = 0;
    
    /* 第一行: C, ←, (, ) */
    calc_buttons[i].x = 10;
    calc_buttons[i].y = 60;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "C");
    calc_buttons[i].type = 4;
    i++;
    
    calc_buttons[i].x = 70;
    calc_buttons[i].y = 60;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "<");  /* 退格符号 */
    calc_buttons[i].type = 4;
    i++;
    
    calc_buttons[i].x = 130;
    calc_buttons[i].y = 60;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "(");
    calc_buttons[i].type = 1;
    i++;
    
    calc_buttons[i].x = 190;
    calc_buttons[i].y = 60;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, ")");
    calc_buttons[i].type = 1;
    i++;
    
    /* 第二行: 7, 8, 9, / */
    calc_buttons[i].x = 10;
    calc_buttons[i].y = 110;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "7");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 70;
    calc_buttons[i].y = 110;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "8");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 130;
    calc_buttons[i].y = 110;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "9");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 190;
    calc_buttons[i].y = 110;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "/");
    calc_buttons[i].type = 1;
    i++;
    
    /* 第三行: 4, 5, 6, * */
    calc_buttons[i].x = 10;
    calc_buttons[i].y = 160;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "4");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 70;
    calc_buttons[i].y = 160;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "5");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 130;
    calc_buttons[i].y = 160;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "6");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 190;
    calc_buttons[i].y = 160;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "*");
    calc_buttons[i].type = 1;
    i++;
    
    /* 第四行: 1, 2, 3, - */
    calc_buttons[i].x = 10;
    calc_buttons[i].y = 210;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "1");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 70;
    calc_buttons[i].y = 210;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "2");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 130;
    calc_buttons[i].y = 210;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "3");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 190;
    calc_buttons[i].y = 210;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "-");
    calc_buttons[i].type = 1;
    i++;
    
    /* 第五行: 0, ., =, + */
    calc_buttons[i].x = 10;
    calc_buttons[i].y = 260;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "0");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 70;
    calc_buttons[i].y = 260;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, ".");
    calc_buttons[i].type = 0;
    i++;
    
    calc_buttons[i].x = 130;
    calc_buttons[i].y = 260;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "=");
    calc_buttons[i].type = 3;
    i++;
    
    calc_buttons[i].x = 190;
    calc_buttons[i].y = 260;
    calc_buttons[i].width = 50;
    calc_buttons[i].height = 40;
    mrc_strcpy(calc_buttons[i].label, "+");
    calc_buttons[i].type = 1;
    i++;
    
    button_count = i;
}

/* 切换窗口 */
void switchWindow(int windowType) {
    currentWindow = windowType;
    
    switch(windowType) {
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
}

/* 绘制软键提示 */
void drawSoftKeyHints(char* leftText, char* rightText) {
    int leftX, rightX, y;
    char buffer[50];

    y = screenHeight - 20;

    /* 左软键提示 */
    if (leftText != NULL) {
        leftX = 10;
        uc3_drawText(leftText, leftX, y, 0, 0, 0, 0);
    }

    /* 右软键提示 */
    if (rightText != NULL) {
        mrc_sprintf(buffer, "%s", rightText);
        rightX = screenWidth - uc3_getWidth(buffer, 0) - 10;
        uc3_drawText(buffer, rightX, y, 0, 0, 0, 0);
    }
}

/* 绘制按钮 */
void drawButton(Button btn, int pressed) {
    int color_bg;
    int color_text;
    int textWidth;
    int textX;
    int textY;
    int r;
    int g;
    int b;
    
    /* 根据按钮类型设置颜色 */
    switch(btn.type) {
        case 0: /* 数字 */
            if (pressed) {
                color_bg = 0xFFCFD8DC;
            } else {
                color_bg = 0xFFECEFF1;
            }
            color_text = 0xFF263238;
            break;
        case 1: /* 操作符 */
            if (pressed) {
                color_bg = 0xFF303F9F;
            } else {
                color_bg = 0xFF3949AB;
            }
            color_text = 0xFFFFFFFF;
            break;
        case 3: /* 等号 */
            if (pressed) {
                color_bg = 0xFF7B1FA2;
            } else {
                color_bg = 0xFF8E24AA;
            }
            color_text = 0xFFFFFFFF;
            break;
        case 4: /* 清除 */
            if (pressed) {
                color_bg = 0xFFD32F2F;
            } else {
                color_bg = 0xFFE53935;
            }
            color_text = 0xFFFFFFFF;
            break;
        default:
            if (pressed) {
                color_bg = 0xFFBDBDBD;
            } else {
                color_bg = 0xFFEEEEEE;
            }
            color_text = 0xFF212121;
    }
    
    /* 绘制按钮背景 */
    gl_drawRect(btn.x, btn.y, btn.width, btn.height, color_bg);
    
    /* 绘制按钮边框 */
    gl_drawHollowRect(btn.x, btn.y, btn.width, btn.height, 0xFF757575);
    
    /* 绘制按钮文字 */
    textWidth = uc3_getWidth(btn.label, 0);
    textX = btn.x + (btn.width - textWidth) / 2;
    textY = btn.y + (btn.height - 16) / 2;
    
    /* 提取RGB颜色 */
    r = (color_text >> 16) & 0xFF;
    g = (color_text >> 8) & 0xFF;
    b = color_text & 0xFF;
    
    uc3_drawText(btn.label, textX, textY, r, g, b, 0);
}

/* 绘制计算器窗口 */
void drawCalculatorWindow(void) {
    int i;
    char buffer[100];
    char* expr;
    int expr_len;
    
    /* 清屏 */
    gl_clearScreen(240, 240, 240);
    
    /* 绘制标题 */
    uc3_drawText("计算器 - 风的影子制作", 10, 10, 0, 0, 0, 0);
    
    /* 绘制表达式显示区域 */
    gl_drawRect(10, 30, screenWidth - 20, 25, 0xFFF5F5F5);
    gl_drawHollowRect(10, 30, screenWidth - 20, 25, 0xFFBDBDBD);
    
    /* 显示当前表达式 */
    expr = getCurrentExpression();
    if (expr != NULL && expr[0] != '\0') {
        /* 如果表达式太长，显示最后部分 */
         expr_len = mrc_strlen(expr);
        if (expr_len > 30) {
            mrc_sprintf(buffer, "...%s", expr + expr_len - 27);
        } else {
            mrc_sprintf(buffer, "%s", expr);
        }
        uc3_drawText(buffer, 15, 35, 0, 0, 0, 0);
    } else {
        uc3_drawText("0", 15, 35, 0, 0, 0, 0);
    }
    
    /* 绘制结果区域 */
    gl_drawRect(10, 60, screenWidth - 20, 20, 0xFFE8F5E9);
    if (hasError()) {
        uc3_drawText("错误", 15, 65, 0xD3, 0x2F, 0x2F, 0);
    } else {
        char* result = getCurrentResult();
        if (result != NULL) {
            uc3_drawText(result, 15, 65, 0x1B, 0x5E, 0x20, 0);
        }
    }
    
    /* 绘制所有按钮 */
    for (i = 0; i < button_count; i++) {
        drawButton(calc_buttons[i], 0);
    }
    
    /* 绘制软键提示 */
    drawSoftKeyHints("历史", "退出");
    
    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, screenWidth, screenHeight);
}

/* 绘制历史记录窗口 */
void drawHistoryWindow(void) {
    int i;
    char buffer[150];
    int historyCount;
    int y;
    int textWidth;
    
    /* 清屏 */
    gl_clearScreen(240, 240, 240);
    
    /* 绘制标题 */
    uc3_drawText("计算历史", 10, 10, 0, 0, 0, 0);
    
    /* 绘制历史记录列表 */
     historyCount = getHistoryCount();
    if (historyCount == 0) {
        uc3_drawText("暂无历史记录", 20, 50, 0x75, 0x75, 0x75, 0);
    } else {
         y = 40;
        for (i = 0; i < historyCount && i < 8; i++) {
            HistoryItem* item = getHistoryItem(i);
            if (item != NULL) {
                /* 绘制表达式 */
                mrc_sprintf(buffer, "%d. %s", i + 1, item->expression);
                uc3_drawText(buffer, 10, y, 0x42, 0x42, 0x42, 0);
                
                /* 绘制结果 */
                mrc_sprintf(buffer, "= %s", item->result);
                 textWidth = uc3_getWidth(buffer, 0);
                uc3_drawText(buffer, screenWidth - textWidth - 10, y, 0x39, 0x49, 0xAB, 0);
                
                y += 25;
            }
        }
    }
    
    /* 绘制软键提示 */
    drawSoftKeyHints("返回", "清除历史");
    
    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, screenWidth, screenHeight);
}

/* 绘制帮助窗口 */
void drawHelpWindow(void) {
    /* 清屏 */
    gl_clearScreen(240, 240, 240);
    
    /* 绘制标题 */
    uc3_drawText("计算器帮助", 10, 10, 0, 0, 0, 0);
    
    /* 绘制帮助内容 */
    uc3_drawText("使用说明:", 10, 40, 0x39, 0x49, 0xAB, 0);
    uc3_drawText("1. 点击数字按钮输入数字", 20, 60, 0, 0, 0, 0);
    uc3_drawText("2. 点击运算符进行计算", 20, 80, 0, 0, 0, 0);
    uc3_drawText("3. C: 清除当前表达式", 20, 100, 0, 0, 0, 0);
    uc3_drawText("4. ←: 退格删除", 20, 120, 0, 0, 0, 0);
    uc3_drawText("5. =: 计算结果", 20, 140, 0, 0, 0, 0);
    uc3_drawText("支持: + - * / ( ) .", 20, 160, 0, 0, 0, 0);
    
    /* 绘制软键提示 */
    drawSoftKeyHints("确认", "返回");
    
    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, screenWidth, screenHeight);
}

/* 处理计算器窗口事件 */
void handleCalculatorEvent(int32 code, int32 param0, int32 param1) {
    int i;
    int x, y;
    
    if (code == MR_MOUSE_UP) {
        x = param0;
        y = param1;
        
        /* 检查软键区域点击（屏幕底部区域） */
        if (y >= screenHeight - 18) {  /* 底部40像素为软键区域 */
            /* 检查左软键区域（左侧1/3屏幕） */
            if (x < screenWidth / 3) {
                /* 左软键：切换到历史记录 */
                mrc_printf("[UI] Left softkey touched\n");
                switchWindow(WINDOW_HISTORY);
                return;
            }
            /* 检查右软键区域（右侧1/3屏幕） */
            else if (x > screenWidth * 2 / 3) {
                /* 右软键：退出程序 */
                mrc_printf("[UI] Right softkey touched\n");
                mrc_exit();
                return;
            }
        }
        
        /* 检查计算器按钮点击 */
        for (i = 0; i < button_count; i++) {
            Button btn = calc_buttons[i];
            if (x >= btn.x && x <= btn.x + btn.width &&
                y >= btn.y && y <= btn.y + btn.height) {
                
                /* 绘制按钮按下效果 */
                drawButton(btn, 1);
                mrc_refreshScreen(btn.x, btn.y, btn.width, btn.height);
                mrc_sleep(100);
                
                /* 调试输出 */
                mrc_printf("[UI] Button clicked: %s\n", btn.label);
                
                /* 处理按钮点击 */
                if (mrc_strcmp(btn.label, "C") == 0) {
                    calculatorClear();
                } else if (mrc_strcmp(btn.label, "<") == 0) {
                    calculatorBackspace();
                } else if (mrc_strcmp(btn.label, "=") == 0) {
                    calculatorEvaluate();
                } else {
                    calculatorAddChar(btn.label[0]);
                }
                
                /* 重绘窗口 */
                drawCalculatorWindow();
                break;
            }
        }
    } else if (code == MR_KEY_RELEASE) {
        if (param0 == MR_KEY_SOFTRIGHT) {
            /* 右软键：退出程序 */
            mrc_exit();
        } else if (param0 == MR_KEY_SOFTLEFT) {
            /* 左软键：切换到历史记录 */
            switchWindow(WINDOW_HISTORY);
        }
    }
}

/* 处理历史记录窗口事件 */
void handleHistoryEvent(int32 code, int32 param0, int32 param1) {
    int x;
    int y;
    
    if (code == MR_KEY_RELEASE) {
        if (param0 == MR_KEY_SOFTLEFT) {
            /* 左软键：返回计算器 */
            switchWindow(WINDOW_CALCULATOR);
        } else if (param0 == MR_KEY_SOFTRIGHT) {
            /* 右软键：清除历史记录 */
            calculatorClearAll();
            switchWindow(WINDOW_CALCULATOR);
        }
    } else if (code == MR_MOUSE_UP) {
        x = param0;
        y = param1;
        
        /* 检查软键区域点击（屏幕底部区域） */
        if (y >= screenHeight - 18) {  /* 底部40像素为软键区域 */
            /* 检查左软键区域（左侧1/3屏幕） */
            if (x < screenWidth / 3) {
                /* 左软键：返回计算器 */
                mrc_printf("[UI] History: Left softkey touched (back)\n");
                switchWindow(WINDOW_CALCULATOR);
                return;
            }
            /* 检查右软键区域（右侧1/3屏幕） */
            else if (x > screenWidth * 2 / 3) {
                /* 右软键：清除历史记录 */
                mrc_printf("[UI] History: Right softkey touched (clear history)\n");
                calculatorClearAll();
                switchWindow(WINDOW_CALCULATOR);
                return;
            }
        }
        
        /* 点击屏幕其他区域也返回计算器 */
        switchWindow(WINDOW_CALCULATOR);
    }
}


/* 处理帮助窗口事件 */
void handleHelpEvent(int32 code, int32 param0, int32 param1) {
    if (code == MR_KEY_RELEASE) {
        if (param0 == MR_KEY_SOFTLEFT || param0 == MR_KEY_SOFTRIGHT) {
            /* 左软键或右软键：返回计算器 */
            switchWindow(WINDOW_CALCULATOR);
        }
    } else if (code == MR_MOUSE_UP) {
        /* 点击屏幕返回计算器 */
        switchWindow(WINDOW_CALCULATOR);
    }
}