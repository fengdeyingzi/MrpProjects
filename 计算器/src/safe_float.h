/*
 * safe_float.h - 功能机平台安全浮点运算库
 *
 * 提供安全的float运算函数，避免极小数异常和溢出问题
 * 简单易用，直接替换原生float运算
 */

#ifndef SAFE_FLOAT_H
#define SAFE_FLOAT_H

#include <mrc_base.h>

/* 浮点数安全阈值 */
#define FS_EPSILON  1e-6f    /* 最小有效值阈值 */
#define FS_MAX      1e6f     /* 最大安全值 */
#define FS_MIN     -1e6f     /* 最小安全值 */

/* ========== 基础运算 ========== */

/* 安全加法: a + b */
float fs_add(float a, float b);

/* 安全减法: a - b */
float fs_sub(float a, float b);

/* 安全乘法: a * b */
float fs_mul(float a, float b);

/* 安全除法: a / b (自动处理除零) */
float fs_div(float a, float b);

/* ========== 常用函数 ========== */

/* 绝对值 */
float fs_abs(float x);

/* 平方: x * x */
float fs_square(float x);

/* 平方根 (牛顿迭代法) */
float fs_sqrt(float x);

/* 最小值 */
float fs_min(float a, float b);

/* 最大值 */
float fs_max(float a, float b);

/* 限制范围 */
float fs_clamp(float x, float min_val, float max_val);

/* ========== 安全检查 ========== */

/* 检查是否为安全数值 (非NaN、非Inf、在合理范围内) */
int32 fs_is_safe(float x);

/* 将数值限制在安全范围 */
float fs_sanitize(float x);

/* 检查是否接近零 */
int32 fs_is_zero(float x);

/* ========== 比较运算 (带容差) ========== */

/* 相等比较: |a - b| < epsilon */
int32 fs_eq(float a, float b);

/* 小于: a < b - epsilon */
int32 fs_lt(float a, float b);

/* 小于等于: a <= b + epsilon */
int32 fs_le(float a, float b);

/* 大于: a > b + epsilon */
int32 fs_gt(float a, float b);

/* 大于等于: a >= b - epsilon */
int32 fs_ge(float a, float b);

#endif /* SAFE_FLOAT_H */
