#include "expression_evaluator.h"
#include <mrc_base.h>

/* 简单栈实现 */
typedef struct {
    double data[MAX_EXPR_LEN];
    int top;
} DoubleStack;

typedef struct {
    char data[MAX_EXPR_LEN];
    int top;
} CharStack;

static void initDoubleStack(DoubleStack* s) { s->top = -1; }
static void initCharStack(CharStack* s) { s->top = -1; }
static int isDoubleStackEmpty(DoubleStack* s) { return s->top == -1; }
static int isCharStackEmpty(CharStack* s) { return s->top == -1; }
static void doublePush(DoubleStack* s, double val) { s->data[++s->top] = val; }
static void charPush(CharStack* s, char val) { s->data[++s->top] = val; }
static double doublePop(DoubleStack* s) { return s->data[s->top--]; }
static char charPop(CharStack* s) { return s->data[s->top--]; }
static double doublePeek(DoubleStack* s) { return s->data[s->top]; }
static char charPeek(CharStack* s) { return s->data[s->top]; }

/* 检查是否为运算符 */
int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/* 获取运算符优先级 */
int precedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

/* 应用运算符 */
double applyOp(double a, double b, char op) {
    double result;
    
    result = 0.0;
    switch(op) {
        case '+': 
            result = a + b;
            break;
        case '-': 
            result = a - b;
            break;
        case '*': 
            result = a * b;
            break;
        case '/': 
            if (b == 0.0) {
                return 0.0; /* 会在调用处检查 */
            }
            result = a / b;
            break;
    }
    return result;
}

/* 表达式求值主函数 */
EvalResult evaluateExpression(char* expression) {
    EvalResult result;
    int i;
    DoubleStack values;
    CharStack ops;
    char token;
    double val;
    int expect_operand;
    int has_decimal;
    int decimal_places;
    int len;
    int j;
    double a;
    double b;
    char op;
    
    /* 初始化结果 */
    result.value = 0.0;
    result.error = 0;
    mrc_memset(result.error_msg, 0, 50);
    
    /* 初始化局部变量 */
    expect_operand = 1; /* 1表示期望操作数，0表示期望运算符 */
    
    /* 初始化栈 */
    initDoubleStack(&values);
    initCharStack(&ops);
    
    i = 0;
    while (expression[i] != '\0') {
        token = expression[i];
        
        /* 跳过空格 */
        if (token == ' ') {
            i++;
            continue;
        }
        
        /* 处理数字 */
        if ((token >= '0' && token <= '9') || token == '.') {
            if (!expect_operand) {
                result.error = 1;
                mrc_strcpy(result.error_msg, "语法错误");
                return result;
            }
            
            val = 0.0;
            has_decimal = 0;
            decimal_places = 0;
            j = i;
            
            /* 解析数字 */
            while ((expression[j] >= '0' && expression[j] <= '9') || expression[j] == '.') {
                if (expression[j] == '.') {
                    if (has_decimal) {
                        result.error = 1;
                        mrc_strcpy(result.error_msg, "多个小数点");
                        return result;
                    }
                    has_decimal = 1;
                } else {
                    if (has_decimal) {
                        decimal_places++;
                        val = val * 10.0 + (expression[j] - '0');
                    } else {
                        val = val * 10.0 + (expression[j] - '0');
                    }
                }
                j++;
            }
            
            /* 处理小数位 */
            while (decimal_places > 0) {
                val /= 10.0;
                decimal_places--;
            }
            
            doublePush(&values, val);
            expect_operand = 0;
            i = j - 1;
        }
        
        /* 处理左括号 */
        else if (token == '(') {
            if (!expect_operand) {
                result.error = 1;
                mrc_strcpy(result.error_msg, "语法错误");
                return result;
            }
            charPush(&ops, token);
            expect_operand = 1;
        }
        
        /* 处理右括号 */
        else if (token == ')') {
            if (expect_operand) {
                result.error = 1;
                mrc_strcpy(result.error_msg, "语法错误");
                return result;
            }
            
            while (!isCharStackEmpty(&ops) && charPeek(&ops) != '(') {
                if (isDoubleStackEmpty(&values)) {
                    result.error = 1;
                    mrc_strcpy(result.error_msg, "表达式错误");
                    return result;
                }
                
                b = doublePop(&values);
                
                if (isDoubleStackEmpty(&values)) {
                    result.error = 1;
                    mrc_strcpy(result.error_msg, "表达式错误");
                    return result;
                }
                
                a = doublePop(&values);
                op = charPop(&ops);
                
                if (op == '/' && b == 0.0) {
                    result.error = 1;
                    mrc_strcpy(result.error_msg, "除数不能为零");
                    return result;
                }
                
                doublePush(&values, applyOp(a, b, op));
            }
            
            if (isCharStackEmpty(&ops)) {
                result.error = 1;
                mrc_strcpy(result.error_msg, "括号不匹配");
                return result;
            }
            
            charPop(&ops); /* 弹出左括号 */
        }
        
        /* 处理运算符 */
        else if (isOperator(token)) {
            if (expect_operand && token == '-') {
                /* 处理负数 */
                i++;
                if (expression[i] >= '0' && expression[i] <= '9') {
                    val = 0.0;
                    has_decimal = 0;
                    decimal_places = 0;
                    j = i;
                    
                    while ((expression[j] >= '0' && expression[j] <= '9') || expression[j] == '.') {
                        if (expression[j] == '.') {
                            if (has_decimal) {
                                result.error = 1;
                                mrc_strcpy(result.error_msg, "多个小数点");
                                return result;
                            }
                            has_decimal = 1;
                        } else {
                            if (has_decimal) {
                                decimal_places++;
                                val = val * 10.0 + (expression[j] - '0');
                            } else {
                                val = val * 10.0 + (expression[j] - '0');
                            }
                        }
                        j++;
                    }
                    
                    while (decimal_places > 0) {
                        val /= 10.0;
                        decimal_places--;
                    }
                    
                    doublePush(&values, -val);
                    expect_operand = 0;
                    i = j - 1;
                } else {
                    result.error = 1;
                    mrc_strcpy(result.error_msg, "无效的负数");
                    return result;
                }
            } else {
                if (expect_operand) {
                    result.error = 1;
                    mrc_strcpy(result.error_msg, "语法错误");
                    return result;
                }
                
                while (!isCharStackEmpty(&ops) && precedence(charPeek(&ops)) >= precedence(token)) {
                    if (isDoubleStackEmpty(&values)) {
                        result.error = 1;
                        mrc_strcpy(result.error_msg, "表达式错误");
                        return result;
                    }
                    
                    b = doublePop(&values);
                    
                    if (isDoubleStackEmpty(&values)) {
                        result.error = 1;
                        mrc_strcpy(result.error_msg, "表达式错误");
                        return result;
                    }
                    
                    a = doublePop(&values);
                    op = charPop(&ops);
                    
                    if (op == '/' && b == 0.0) {
                        result.error = 1;
                        mrc_strcpy(result.error_msg, "除数不能为零");
                        return result;
                    }
                    
                    doublePush(&values, applyOp(a, b, op));
                }
                
                charPush(&ops, token);
                expect_operand = 1;
            }
        } else {
            result.error = 1;
            mrc_strcpy(result.error_msg, "无效字符");
            return result;
        }
        
        i++;
    }
    
    /* 处理剩余的运算符 */
    while (!isCharStackEmpty(&ops)) {
        if (isDoubleStackEmpty(&values)) {
            result.error = 1;
            mrc_strcpy(result.error_msg, "表达式错误");
            return result;
        }
        
        b = doublePop(&values);
        
        if (isDoubleStackEmpty(&values)) {
            result.error = 1;
            mrc_strcpy(result.error_msg, "表达式错误");
            return result;
        }
        
        a = doublePop(&values);
        op = charPop(&ops);
        
        if (op == '/' && b == 0.0) {
            result.error = 1;
            mrc_strcpy(result.error_msg, "除数不能为零");
            return result;
        }
        
        doublePush(&values, applyOp(a, b, op));
    }
    
    /* 获取结果 */
    if (isDoubleStackEmpty(&values)) {
        result.error = 1;
        mrc_strcpy(result.error_msg, "表达式为空");
        return result;
    }
    
    result.value = doublePop(&values);
    
    if (!isDoubleStackEmpty(&values)) {
        result.error = 1;
        mrc_strcpy(result.error_msg, "表达式错误");
        return result;
    }
    
    return result;
}