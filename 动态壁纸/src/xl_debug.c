#include <mrc_base.h>
#include "xl_debug.h"
#include <debug.h>
/*

EBUG_FILE
风的影子 用于调试
*/


#ifndef va_start
typedef char * va_list;
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#define va_start(ap,v)  ( ap = (va_list)(&(v)) + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )


#endif

void debug_printf(char *text,...){
	int32 f = 0;
	int32 len = 0;
	char temp[500];
	va_list args; 
	
	len = mrc_getLen(DEBUG_CHECK_FILE);
	//mrc_clearScreen(240,240,240);
	//mrc_drawText(text, 0,0, 20,20,20, 0, 1);
	//mrc_refreshScreen(0,0,240,320);
	//mrc_sleep(200);
	if(len>=0){
		va_start(args, text);  
		mrc_sprintf(temp,text,args);
		va_end(args); 
		f = mrc_open(DEBUG_FILE, 2);
		mrc_seek(f,0,MR_SEEK_END);
		mrc_write(f, temp, mrc_strlen(temp));
		mrc_write(f, "\r\n", 2);
		mrc_close(f);
	}
}

/*
void toCharEx(float num, char *text) {
    char *p;
    // 处理特殊值
    if (isnan(num)) {
        strcpy(text, "nan");
        return;
    }
    
    if (isinf(num)) {
        if (num < 0) {
            strcpy(text, "-inf");
        } else {
            strcpy(text, "inf");
        }
        return;
    }
    
    // 处理0
    if (num == 0.0f) {
        strcpy(text, "0.0");
        return;
    }
    
    p = text;
    
    // 处理负数
    if (num < 0) {
        *p++ = '-';
        num = -num;
    }
    
    // 检查是否需要用科学计数法
    int exponent = 0;
    float abs_num = num;
    
    // 处理非常大的数
    if (abs_num >= 1e7f) {
        while (abs_num >= 10.0f) {
            abs_num /= 10.0f;
            exponent++;
        }
        num = abs_num;
    }
    // 处理非常小的数
    else if (abs_num > 0 && abs_num < 1e-5f) {
        while (abs_num < 1.0f) {
            abs_num *= 10.0f;
            exponent--;
        }
        num = abs_num;
    }
    
    // 四舍五入到指定位数
    float rounded = (float)((int)(num * 1000000.0f + 0.5f)) / 1000000.0f;
    
    // 分离整数和小数部分
    int integer = (int)rounded;
    float fraction = rounded - (float)integer;
    
    // 转换整数部分
    char int_buf[20];
    int int_idx = 0;
    
    if (integer == 0) {
        int_buf[int_idx++] = '0';
    } else {
        char rev_buf[20];
        int rev_idx = 0;
        int temp = integer;
        
        while (temp > 0) {
            rev_buf[rev_idx++] = (temp % 10) + '0';
            temp /= 10;
        }
        
        // 反转
        for (int i = rev_idx - 1; i >= 0; i--) {
            int_buf[int_idx++] = rev_buf[i];
        }
    }
    int_buf[int_idx] = '\0';
    
    // 复制整数部分
    strcpy(p, int_buf);
    p += int_idx;
    
    // 添加小数点
    *p++ = '.';
    
    // 转换小数部分
    float frac_temp = fraction;
    for (int i = 0; i < 6; i++) {
        frac_temp *= 10.0f;
        int digit = (int)frac_temp;
        *p++ = digit + '0';
        frac_temp -= (float)digit;
    }
    *p = '\0';
    
    // 移除末尾的0
    p--;
    while (p > text && *p == '0') {
        *p = '\0';
        p--;
    }
    
    // 如果是科学计数法，添加指数
    if (exponent != 0) {
        // 移除末尾的"."
        if (*p == '.') {
            *p = 'e';
        } else {
            *(++p) = 'e';
        }
        p++;
        
        // 添加指数
        if (exponent > 0) {
            *p++ = '+';
        } else {
            *p++ = '-';
            exponent = -exponent;
        }
        
        // 转换指数
        char exp_buf[10];
        int exp_idx = 0;
        
        if (exponent == 0) {
            exp_buf[exp_idx++] = '0';
        } else {
            char rev_buf[10];
            int rev_idx = 0;
            
            while (exponent > 0) {
                rev_buf[rev_idx++] = (exponent % 10) + '0';
                exponent /= 10;
            }
            
            for (int i = rev_idx - 1; i >= 0; i--) {
                exp_buf[exp_idx++] = rev_buf[i];
            }
        }
        exp_buf[exp_idx] = '\0';
        
        strcpy(p, exp_buf);
    }
}
*/
