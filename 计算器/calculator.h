#ifndef CALCULATOR_H
#define CALCULATOR_H
#include "ui_system.h"
/* 最大表达式长度 */
#define MAX_EXPRESSION_LEN 100
#define MAX_HISTORY_ITEMS 20

/* 计算器状态结构 */
typedef struct {
    char expression[MAX_EXPRESSION_LEN];
    char result[50];
    int expr_pos;
    int error;
    char error_msg[100];
} CalculatorState;

/* 历史记录管理 */
typedef struct {
    HistoryItem items[MAX_HISTORY_ITEMS];
    int count;
    int current_index;
} HistoryManager;

/* 函数声明 */
void initCalculator(void);
void calculatorAddChar(char c);
void calculatorBackspace(void);
void calculatorClear(void);
void calculatorClearAll(void);
int calculatorEvaluate(void);
void addToHistory(char* expression, char* result);
char* getCurrentExpression(void);
char* getCurrentResult(void);
char* getErrorMessage(void);
int hasError(void);
void clearError(void);
int getHistoryCount(void);
HistoryItem* getHistoryItem(int index);
void resetCalculator(void);

#endif /* CALCULATOR_H */