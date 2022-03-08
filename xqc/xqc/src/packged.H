
#ifndef PACKGED_H
#define PACKGED_H
#include "libini.h"

// 设置信息输出函数
void pack_set_print_func(void (*print_funcs)(void *opaque, const char *msg));
void mpr_tou(INI *pini);
///dir,file,
int PackgedMrp(char *cfile,char **fj,char *mrpname);/////////////打包运行时被调用

#endif//PACKGED_H
