#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

/* 最大表达式长度 */
#define MAX_EXPR_LEN 100

/* 求值结果结构 */
typedef struct {
    double value;
    int error;
    char error_msg[50];
} EvalResult;

/* 函数声明 */
EvalResult evaluateExpression(char* expression);
int isOperator(char c);
int precedence(char op);
double applyOp(double a, double b, char op);

#endif /* EXPRESSION_EVALUATOR_H */