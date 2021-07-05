#ifndef __URL_DOCODE_H__
#define __URL_DOCODE_H__
int hex2dec(char c); //hex十六进制字符串转十进制数字
char dec2hex(short int c); //数字转十六进制
void urlencode(char *url); //url编码
void urldecode(char *url); //url解码

#endif
