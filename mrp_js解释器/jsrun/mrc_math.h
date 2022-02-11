#ifndef __MRC_MATH_H__
#define __MRC_MATH_H__
#include <math.h>
typedef int MATH_MODE;
extern const int MR_BY_RADIDAN;                           // 弧度
extern const int MR_BY_ANGLE;                                // 角度
extern const double MR_RATIO;                 // 角度转弧度的转换率
#define MR_PI 3.1415926535
  extern double mrc_floor(double _X);
  extern double mrc_ceil(double _X);
  extern double mrc_fmod(double _X,double _Y);
  double mrc_tan( double x , MATH_MODE mode);
  double mrc_pow(double x, double y);
  double mrc_fabs( double x );
  double mrc_sin( double x, MATH_MODE mode );
  double mrc_cos( double x , MATH_MODE mode);
  double mrc_tan( double x , MATH_MODE mode);
  double mrc_exp( double x );
  double mrc_arctanh(double x);
  #define isfinite(x) (0) //当(fpclassify(x)!=FP_NAN&&fpclassify(x)!=FP_INFINITE)时，此宏得到一个非零值。
  #define isinf(x) (x) //当x是正无穷是返回1，当x是负无穷时返回-1。
  #define signbit(arg) ((int)arg)   //判断是否为负数？？
#endif

