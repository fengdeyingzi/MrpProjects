#ifndef _TYPE_H
#define _TYPE_H 

/*龙魂噬心(6369)
2012-7-11

*/
//#define FONT 1//必要时请注释
#define MAKERGB(r,g,b)(r<<16)+(g<<8)+(b)
#define MAKER(a)(a>>16&0xff)

#define MAKEG(a)(a>>8&0xff)

#define MAKEB(a)(a&0xff)


/*将888格式的颜色a分解为r,g,b
a：888格式的颜色
r，g，b  保存r，g，b的变量的地址(指针)
*/
void make(int32 a,int* r,int* g,int* b)
{
*r=MAKER(a);
*g=MAKEG(a);
*b=MAKEB(a);
}

/*gb2312和unicode编码的相互转换
str 欲转换的字符串
is_uni 是否输出的为unicode编码
  1 返回unicode的指针
  0 gb2312
返回 指向返回字符串的指针
*/
void* gb_uni(char* str,int is_uni)
{
int err,a,b;
//char p[255];
a=wstrlen(str);
if(is_uni){
u2c(str,a,&str,&a);
return str;
}
else{
 return c2u(str,&err,&b);
}
}
#endif