/*
 * safe_float.c - 功能机平台安全浮点运算库实现
 */

#include "safe_float.h"

/* ========== 内部辅助函数 ========== */

/* 检查NaN */
static int32 is_nan(float x) {
    /* NaN的特性: x != x */
    return (x != x);
}

/* 检查无穷大 */
static int32 is_inf(float x) {
    /* 简化检查: 超过最大安全范围 */
    return (x > FS_MAX * 10.0f || x < FS_MIN * 10.0f);
}

/* ========== 安全检查 ========== */

int32 fs_is_safe(float x) {
    if (is_nan(x)) return 0;
    if (is_inf(x)) return 0;
    if (x > FS_MAX || x < FS_MIN) return 0;
    return 1;
}

float fs_sanitize(float x) {
    if (is_nan(x)) return 0.0f;
    if (is_inf(x)) return (x > 0) ? FS_MAX : FS_MIN;
    if (x > FS_MAX) return FS_MAX;
    if (x < FS_MIN) return FS_MIN;

    /* 极小数归零 */
    if (x > -FS_EPSILON && x < FS_EPSILON) return 0.0f;

    return x;
}

int32 fs_is_zero(float x) {
    return (x > -FS_EPSILON && x < FS_EPSILON);
}

/* ========== 基础运算 ========== */

float fs_add(float a, float b) {
    float result;

    /* 输入检查 */
    a = fs_sanitize(a);
    b = fs_sanitize(b);

    /* 执行运算 */
    result = a + b;

    /* 结果检查 */
    return fs_sanitize(result);
}

float fs_sub(float a, float b) {
    float result;

    a = fs_sanitize(a);
    b = fs_sanitize(b);

    result = a - b;

    return fs_sanitize(result);
}

float fs_mul(float a, float b) {
    float result;

    a = fs_sanitize(a);
    b = fs_sanitize(b);

    /* 极小数直接归零，避免精度问题 */
    if (fs_is_zero(a) || fs_is_zero(b)) {
        return 0.0f;
    }

    result = a * b;

    /* 结果检查 - 防止溢出和下溢 */
    if (result > FS_MAX) return FS_MAX;
    if (result < FS_MIN) return FS_MIN;
    if (result > -FS_EPSILON && result < FS_EPSILON) return 0.0f;

    return result;
}

float fs_div(float a, float b) {
    float result;

    a = fs_sanitize(a);
    b = fs_sanitize(b);

    /* 除零保护 */
    if (fs_is_zero(b)) {
        return 0.0f;
    }

    /* 分子为零 */
    if (fs_is_zero(a)) {
        return 0.0f;
    }

    result = a / b;

    /* 结果检查 - 防止溢出 */
    if (result > FS_MAX) return FS_MAX;
    if (result < FS_MIN) return FS_MIN;
    if (result > -FS_EPSILON && result < FS_EPSILON) return 0.0f;

    return result;
}

/* ========== 常用函数 ========== */

float fs_abs(float x) {
    return (x < 0.0f) ? -x : x;
}

float fs_square(float x) {
    return fs_mul(x, x);
}

float fs_sqrt(float x) {
    float result, prev, temp;
    int32 i;

    x = fs_sanitize(x);

    /* 负数或零 */
    if (x <= 0.0f) {
        return 0.0f;
    }

    /* 极小值直接返回0 */
    if (x < FS_EPSILON) {
        return 0.0f;
    }

    /* 牛顿迭代法: x_n+1 = (x_n + x/x_n) / 2 */
    /* 使用原生运算避免递归调用 */
    result = x;

    /* 最多迭代8次 */
    for (i = 0; i < 8; i++) {
        prev = result;

        /* 直接使用原生运算，但做安全检查 */
        if (result < FS_EPSILON) {
            break;
        }

        temp = x / result;  /* x/result */
        temp = result + temp;  /* result + x/result */
        result = temp * 0.5f;  /* (result + x/result) / 2 */

        /* 收敛检查 */
        temp = result - prev;
        if (temp < 0) temp = -temp;  /* abs */
        if (temp < FS_EPSILON) {
            break;
        }
    }

    return fs_sanitize(result);
}

float fs_min(float a, float b) {
    a = fs_sanitize(a);
    b = fs_sanitize(b);
    return (a < b) ? a : b;
}

float fs_max(float a, float b) {
    a = fs_sanitize(a);
    b = fs_sanitize(b);
    return (a > b) ? a : b;
}

float fs_clamp(float x, float min_val, float max_val) {
    x = fs_sanitize(x);
    min_val = fs_sanitize(min_val);
    max_val = fs_sanitize(max_val);

    if (x < min_val) return min_val;
    if (x > max_val) return max_val;
    return x;
}

/* ========== 比较运算 ========== */

int32 fs_eq(float a, float b) {
    float diff;
    a = fs_sanitize(a);
    b = fs_sanitize(b);
    diff = fs_abs(fs_sub(a, b));
    return (diff < FS_EPSILON);
}

int32 fs_lt(float a, float b) {
    a = fs_sanitize(a);
    b = fs_sanitize(b);
    return (a < b - FS_EPSILON);
}

int32 fs_le(float a, float b) {
    a = fs_sanitize(a);
    b = fs_sanitize(b);
    return (a <= b + FS_EPSILON);
}

int32 fs_gt(float a, float b) {
    a = fs_sanitize(a);
    b = fs_sanitize(b);
    return (a > b + FS_EPSILON);
}

int32 fs_ge(float a, float b) {
    a = fs_sanitize(a);
    b = fs_sanitize(b);
    return (a >= b - FS_EPSILON);
}
