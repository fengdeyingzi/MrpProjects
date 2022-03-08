#ifndef _TYPE_H
#define _TYPE_H 

/*��������(6369)
2012-7-11

*/
//#define FONT 1//��Ҫʱ��ע��
#define MAKERGB(r,g,b)(r<<16)+(g<<8)+(b)
#define MAKER(a)(a>>16&0xff)

#define MAKEG(a)(a>>8&0xff)

#define MAKEB(a)(a&0xff)


/*��888��ʽ����ɫa�ֽ�Ϊr,g,b
a��888��ʽ����ɫ
r��g��b  ����r��g��b�ı����ĵ�ַ(ָ��)
*/
void make(int32 a,int* r,int* g,int* b)
{
*r=MAKER(a);
*g=MAKEG(a);
*b=MAKEB(a);
}

/*gb2312��unicode������໥ת��
str ��ת�����ַ���
is_uni �Ƿ������Ϊunicode����
  1 ����unicode��ָ��
  0 gb2312
���� ָ�򷵻��ַ�����ָ��
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