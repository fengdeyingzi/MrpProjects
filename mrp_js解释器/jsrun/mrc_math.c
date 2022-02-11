#include <math.h>
#include "mrc_math.h"
const int MR_BY_RADIDAN = 0;                           // 弧度
const int MR_BY_ANGLE = 1;                                // 角度
const double MR_RATIO = 0.01745329251994;                 // 角度转弧度的转换率

double mrc_floor(double _X) {
    return (double)((long)_X);
}

double mrc_ceil(double _X) {
    if (_X != (long)_X) {
        return (double)((long)_X) + 1;
    }
    return (double)((long)_X);
}

double mrc_fmod(double _X, double _Y) {
    return ((double)(((long)(_X * 1000)) % ((long)(_Y * 1000)))) / 1000;
}

// tanθ = sinθ / cosθ
// 结果精度为7位
//  double mrc_tan( double x , MODE mode){
//      return ( Sin(x) / Cos(x) );
//  }

// 计算阶乘
long Factorial(long x) {
    long ans = 1;
    int i = 0;
    if (0 == x || 1 == x) {
        return 1;
    }

    
    for ( i = 2; i <= x; i++) {
        ans *= i;
    }

    return ans;
}

// ln(x) = 2 arctanh((x-1)/(x+1))
// 调用了Arctanh(double) 方法
static double Ln(double x) {
    return 2 * mrc_arctanh((x - 1) / (x + 1));
}

// 计算求幂, 默认是平方
// x^y=e^(ln(x)*y)
double mrc_pow(double x, double y) {
    return mrc_exp((Ln(x) * y));
}

// 求根
static double gen(double x, double y) {
    return mrc_pow(x, 1 / y);
}

// 计算绝对值
double mrc_fabs(double x) {
    if (x < -1e-9)
        return -x;
    return x;
}

int Int(double x) {
    return (int)x;
}

// 利用泰勒级数算 sin(x) , 默认传入弧度
// sinx ≈x-(x^3)/3!+(x^5)/5!-(x^7)/7!-(x^9)/9!...
// 计算精度为7位
double mrc_sin(double x, MATH_MODE mode) {
    // 如果是角度则转换成弧度计算
    double sum,fs,fm,t,t1;
    int n;
    if (MR_BY_ANGLE == mode) {
        x *= MR_RATIO;
    }
    x = x - Int((x / (2 * MR_PI))) * 2 * MR_PI;
    if (0 == x)
        return 0;

     sum = 0; fs = x; fm = 1;  t1 = 1;  //求和、分子、分母、精度、t1表示符号位
     n = 1;                                  //项数

    t = fs / fm;                 //精度赋值
    while (mrc_fabs(t) >= 1e-8)  //循环条件
    {
        sum = sum + t;                        //求和
        n++;                                  //基数增加
        fs = fs * x * x;                      //新分子
        fm = fm * (2 * n - 2) * (2 * n - 1);  //新分母
        t1 = -t1;                             //符号位　一正一负
        t = t1 * (fs / fm);                   //精度
    }

    return sum;
}

// cos(x) = 1 - (x^2)/2! + (x^4)/4!......
// 结果精度为7位
double mrc_cos(double x, MATH_MODE mode) {
    double term, sum;
    int n = 2;

    sum = 1.0;
    term = 1.0;

    while (mrc_fabs(term) > 1e-7) {
        term = term * x * x / (n * (n - 1)) * (-1.0);
        sum = sum + term;
        n += 2;
    }

    return sum;
}

// tanθ = sinθ / cosθ
// 结果精度为7位
double mrc_tan(double x, MATH_MODE mode) {
    return (mrc_sin(x, MR_BY_RADIDAN) / mrc_cos(x, MR_BY_RADIDAN));
}

// 计算 x^2
double mrc_square(double x) {
    return (x * x);
}

// 计算 x^3
double mrc_cube(double x) {
    return (x * x * x);
}

// arctanh(x)= x + x^3/3 + x^5/5 + ...       (x≤1)
// 求sinh(y) 会用到
// 精度为7位
double mrc_arctanh(double x) {
    int n = 1;
    double sum = 0, term = x, numer = x, denom = 1;
    while (mrc_fabs(term) > 1e-8) {
        sum += term;            // 求和
        numer = numer * x * x;  // 分子
        denom = n + 2;          // 分母
        term = numer / denom;   // 精度
        n += 2;                 // 计数
    }

    return sum;
}

// 求e^x 用于Pow( double, double )调用
// e^x = 1+x+(x^2)/2!+(x^3)/3!+...
// 精度为7位
double mrc_exp(double x) {
    double ret = 1, term = x, numer = x, denom = 1;
    
    int n = 1;
    while (mrc_fabs(term) > 1e-8) {
        ret += term;              // 求和
        numer *= x;               // 分子
        denom = denom * (n + 1);  // 分母
        n++;                      // 累加
        term = numer / denom;     // 精度
    }

    return ret;
}

// sinh(x)=(exp(x) - exp(-x)) / 2.0
// 精度为7位
double Sinh(double x) {
    return ((mrc_exp(x) - mrc_exp(-x)) / 2.0);
}

// cosh(x)=(exp(x) + exp(-x)) / 2.0;
// 精度为7位
double Cosh(double x) {
    return ((mrc_exp(x) + mrc_exp(-x)) / 2.0);
}

// tanh(x) = sinh(x) / cosh(x);
// 精度为7位
double Tanh(double x) {
    return (Sinh(x) / Cosh(x));
}
