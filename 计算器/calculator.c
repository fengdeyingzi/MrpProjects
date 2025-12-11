#include "calculator.h"
#include "expression_evaluator.h"
#include <mrc_base.h>

/* 全局变量 */
static CalculatorState calc_state;
static HistoryManager history_manager;

/* 辅助函数：检查字符是否为数字 */
static int is_digit(char c) {
    return c >= '0' && c <= '9';
}

/* 辅助函数：检查字符是否为运算符 */
static int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')';
}

/* 辅助函数：检查字符是否为小数点 */
static int is_dot(char c) {
    return c == '.';
}

/* 初始化计算器 */
void initCalculator(void) {
    resetCalculator();
    history_manager.count = 0;
    history_manager.current_index = 0;
}

/* 重置计算器状态 */
void resetCalculator(void) {
    mrc_memset(calc_state.expression, 0, MAX_EXPRESSION_LEN);
    mrc_memset(calc_state.result, 0, 50);
    mrc_strcpy(calc_state.expression, "0");
    calc_state.expr_pos = 1;
    calc_state.error = 0;
    mrc_memset(calc_state.error_msg, 0, 100);
}

/* 添加字符到表达式 */
void calculatorAddChar(char c) {
    int len = mrc_strlen(calc_state.expression);
    
    /* 清除错误状态 */
    clearError();
    
    /* 处理初始状态 */
    if (mrc_strcmp(calc_state.expression, "0") == 0) {
        if (is_digit(c) || c == '(' || c == '.') {
            calc_state.expression[0] = c;
            calc_state.expression[1] = '\0';
        } else if (is_operator(c) && c != ')') {
            calc_state.expression[0] = c;
            calc_state.expression[1] = '\0';
        }
        return;
    }
    
    /* 检查长度限制 */
    if (len >= MAX_EXPRESSION_LEN - 1) {
        calc_state.error = 1;
        mrc_strcpy(calc_state.error_msg, "表达式太长");
        return;
    }
    
    /* 添加字符 */
    calc_state.expression[len] = c;
    calc_state.expression[len + 1] = '\0';
}

/* 退格删除 */
void calculatorBackspace(void) {
    int len = mrc_strlen(calc_state.expression);
    
    clearError();
    
    if (len > 1) {
        calc_state.expression[len - 1] = '\0';
    } else {
        mrc_strcpy(calc_state.expression, "0");
    }
}

/* 清除当前表达式 */
void calculatorClear(void) {
    clearError();
    mrc_strcpy(calc_state.expression, "0");
    mrc_memset(calc_state.result, 0, 50);
}

/* 清除全部 */
void calculatorClearAll(void) {
    resetCalculator();
    history_manager.count = 0;
}

/* 计算表达式 */
int calculatorEvaluate(void) {
    char* expr = calc_state.expression;
    int len = mrc_strlen(expr);
    char result_str[50];
    EvalResult eval_result;
    double value;
    int i;

    clearError();
    
    /* 检查空表达式 */
    if (len == 0 || mrc_strcmp(expr, "0") == 0) {
        calc_state.error = 1;
        mrc_strcpy(calc_state.error_msg, "请输入表达式");
        return -1;
    }
    
    /* 使用表达式求值器 */
    eval_result = evaluateExpression(expr);
    
    if (eval_result.error) {
        calc_state.error = 1;
        mrc_strcpy(calc_state.error_msg, eval_result.error_msg);
        return -1;
    }
    
    /* 调试输出 */
    mrc_printf("[CALC] Expression: %s\n", expr);
    mrc_printf("[CALC] Result value: %f\n", eval_result.value);
    
    /* 格式化结果 */
    value = eval_result.value;
    
    /* 检查是否为整数 */
    if (value == (int)value) {
        mrc_sprintf(result_str, "%d", (int)value);
    } else {
        /* 手动转换浮点数为字符串（兼容方案） */
        int int_part;
        int frac_part;
        int is_negative;
        
        /* 处理负数 */
        if (value < 0) {
            is_negative = 1;
            value = -value;
        } else {
            is_negative = 0;
        }
        
        /* 获取整数部分 */
        int_part = (int)value;
        
        /* 获取小数部分（6位精度） */
        frac_part = (int)((value - int_part) * 1000000 + 0.5);  /* 四舍五入 */
        
        /* 处理进位 */
        if (frac_part >= 1000000) {
            int_part++;
            frac_part = 0;
        }
        
        /* 构建字符串 */
        if (is_negative) {
            mrc_sprintf(result_str, "-%d.%06d", int_part, frac_part);
        } else {
            mrc_sprintf(result_str, "%d.%06d", int_part, frac_part);
        }
        
        /* 移除末尾的零 */
        i = mrc_strlen(result_str) - 1;
        while (i > 0 && result_str[i] == '0') {
            result_str[i] = '\0';
            i--;
        }
        if (i > 0 && result_str[i] == '.') {
            result_str[i] = '\0';
        }
    }
    
    mrc_strcpy(calc_state.result, result_str);
    
    /* 添加到历史记录 */
    addToHistory(expr, calc_state.result);
    
    /* 用结果更新当前表达式，这样输入框会显示计算结果 */
    mrc_strcpy(calc_state.expression, result_str);
    
    return 0;
}

/* 添加历史记录 */
void addToHistory(char* expression, char* result) {
    HistoryItem* item;
    int i;
    if (history_manager.count >= MAX_HISTORY_ITEMS) {
        /* 移除最旧的历史记录 */
        for (i = 0; i < history_manager.count - 1; i++) {
            history_manager.items[i] = history_manager.items[i + 1];
        }
        history_manager.count--;
    }
    
    /* 添加新记录 */
    item = &history_manager.items[history_manager.count];
    mrc_strcpy(item->expression, expression);
    mrc_strcpy(item->result, result);
    history_manager.count++;
}

/* 获取当前表达式 */
char* getCurrentExpression(void) {
    return calc_state.expression;
}

/* 获取当前结果 */
char* getCurrentResult(void) {
    if (calc_state.error) {
        return calc_state.error_msg;
    }
    return calc_state.result[0] != '\0' ? calc_state.result : "";
}

/* 获取错误信息 */
char* getErrorMessage(void) {
    return calc_state.error_msg;
}

/* 检查是否有错误 */
int hasError(void) {
    return calc_state.error;
}

/* 清除错误 */
void clearError(void) {
    calc_state.error = 0;
    mrc_memset(calc_state.error_msg, 0, 100);
}

/* 获取历史记录数量 */
int getHistoryCount(void) {
    return history_manager.count;
}

/* 获取历史记录项 */
HistoryItem* getHistoryItem(int index) {
    if (index >= 0 && index < history_manager.count) {
        return &history_manager.items[index];
    }
    return NULL;
}