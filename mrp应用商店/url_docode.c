#include <mrc_base.h>

 
#define BURSIZE 2048
 
int hex2dec(char c)
{
    if ('0' <= c && c <= '9')
    {
        return c - '0';
    }
    else if ('a' <= c && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else if ('A' <= c && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else
    {
        return -1;
    }
}
 
char dec2hex(short int c)
{
    if (0 <= c && c <= 9)
    {
        return c + '0';
    }
    else if (10 <= c && c <= 15)
    {
        return c + 'A' - 10;
    }
    else
    {
        return (char)-1;
    }
}
 
 
//编码一个url 
void urlencode(char *url)
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    int j;
    int i0,i1;
    char res[BURSIZE];
    for (i = 0; i < len; ++i)

    {
        char c = url[i];
        if (    ('0' <= c && c <= '9') ||
                ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') ||
                c == '/' || c == '.')
        {
            res[res_len++] = c;
        }
        else
        {
            j = (short int)c;
            if (j < 0)
                j += 256;
            i1 = j / 16;
            i0 = j - i1 * 16;
            res[res_len++] = '%';
            res[res_len++] = dec2hex(i1);
            res[res_len++] = dec2hex(i0);
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
}
 
// 解码url
void urldecode(char *url)
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char res[BURSIZE];
    for (i = 0; i < len; ++i)
    {
        char c = url[i];
        if (c != '%')
        {
            res[res_len++] = c;
        }
        else
        {
            char c1 = url[++i];
            char c0 = url[++i];
            int num = 0;
            num = hex2dec(c1) * 16 + hex2dec(c0);
            res[res_len++] = num;
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
}

// int main(int argc, char *argv[])
// {
// 　　char url[100] = "二份";
// 　　urlencode(url); //编码后
// 　　printf("%s\n", url);
 
// 　　//char buf[100] = "%E4%BA%8C%E4%BB%BD"; //当使用utf-8编码时　　"二份"解码出来为此
// 　　char buf[100] = "%B6%FE%B7%DD"; //当使用gb2312编码时 "二份"解码出来为此
// 　　urldecode(buf); //解码后
// 　　printf("%s\n", buf);
 
// 　　return 0;
// }