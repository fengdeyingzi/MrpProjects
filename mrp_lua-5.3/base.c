#include "mrc_base.h"
#include "base.h"
#include "stdarg.h"



// #define GET_HELPER()      mr_get_helper()
// #define GET_C_FUNCTION_P()      mr_get_c_function_p()
// #define MR_REALLOC(b,os,s)  GET_HELPER()->mr_realloc(b,os,s)

CAPP_FILE *capp_fopen(const char *path, const char *mode) {
    int file_mode = 0;
    int32 f;
    CAPP_FILE *file;
    int32 file_len = mrc_getLen(path);
    int seek_mode = MR_SEEK_SET;
    mrc_printf("fopen %s %s",path,mode);
    if (strcmp(mode, "r") == 0) {  //以只读方式打开文件，该文件必须存在。
        file_mode = MR_FILE_RDONLY;
    } else if (strcmp(mode, "r+") == 0) {  //以读/写方式打开文件，该文件必须存在。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
    } else if (strcmp(mode, "rb+") == 0) {  //以读/写方式打开一个二进制文件，只允许读/写数据。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
    } else if (strcmp(mode, "rt+") == 0) {  //以读/写方式打开一个文本文件，允许读和写。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
    } else if (strcmp(mode, "w") == 0) {  //打开只写文件，若文件存在则长度清为0，即该文件内容消失，若不存在则创建该文件。
        file_mode = MR_FILE_WRONLY | MR_FILE_CREATE;
        mrc_remove(path);
    } else if (strcmp(mode, "w+") == 0) {  //打开可读/写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY | MR_FILE_CREATE;
        mrc_remove(path);
    } else if (strcmp(mode, "a") == 0) {  //以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留（EOF符保留)。
        file_mode = MR_FILE_WRONLY;
        seek_mode = MR_SEEK_END;
    } else if (strcmp(mode, "a+") == 0) {  //以附加方式打开可读/写的文件。若文件不存在，则会建立该文件，如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（原来的EOF符 不保留)。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY | MR_FILE_CREATE;
        seek_mode = MR_SEEK_END;
    } else if (strcmp(mode, "wb") == 0) {  //以只写方式打开或新建一个二进制文件，只允许写数据。
        file_mode = MR_FILE_WRONLY;
    } else if (strcmp(mode, "wb+") == 0) {  //以读/写方式打开或建立一个二进制文件，允许读和写。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY | MR_FILE_CREATE;
    } else if (strcmp(mode, "wt+") == 0) {  //以读/写方式打开或建立一个文本文件，允许读写。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY | MR_FILE_CREATE;
    } else if (strcmp(mode, "at+") == 0) {  //以读/写方式打开一个文本文件，允许读或在文本末追加数据。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
        seek_mode = MR_SEEK_END;
    } else if (strcmp(mode, "ab+") == 0) {  //以读/写方式打开一个二进制文件，允许读或在文件末追加数据。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
        seek_mode = MR_SEEK_END;
    }
     f = mrc_open(path, file_mode);
    if (f > 0) {
        mrc_seek(f, 0, seek_mode);
        file = (CAPP_FILE *)mrc_malloc(sizeof(CAPP_FILE));
        // file->_tmpfname = path;
        file->_file = f;
        file->len = file_len;
        return file;
    }

    return NULL;
}

unsigned int capp_fread(void *buf, size_t size, size_t count, CAPP_FILE *f) {
    int re = mrc_read(f->_file, buf, size * count);
    if (re == -1)
        return -1;
    else {
        int count = re % size;
        mrc_seek(f->_file, -count, MR_SEEK_CUR);
        return re / size;
    }
}

int capp_feof(CAPP_FILE *f){
    return 0;
}

CAPP_FILE *capp_freopen(const char *path, const char *mode, CAPP_FILE *file){
    int file_mode = 0;
    int32 f;
    int seek_mode = MR_SEEK_SET;
    int32 file_len = mrc_getLen(path);
    mrc_printf("freopen %s %s",path,mode);
    if (strcmp(mode, "r") == 0) {  //以只读方式打开文件，该文件必须存在。
        file_mode = MR_FILE_RDONLY;
    } else if (strcmp(mode, "r+") == 0) {  //以读/写方式打开文件，该文件必须存在。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
    } else if (strcmp(mode, "rb+") == 0) {  //以读/写方式打开一个二进制文件，只允许读/写数据。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
    } else if (strcmp(mode, "rt+") == 0) {  //以读/写方式打开一个文本文件，允许读和写。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
    } else if (strcmp(mode, "w") == 0) {  //打开只写文件，若文件存在则长度清为0，即该文件内容消失，若不存在则创建该文件。
        file_mode = MR_FILE_WRONLY | MR_FILE_CREATE;
        mrc_remove(path);
    } else if (strcmp(mode, "w+") == 0) {  //打开可读/写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY | MR_FILE_CREATE;
        mrc_remove(path);
    } else if (strcmp(mode, "a") == 0) {  //以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留（EOF符保留)。
        file_mode = MR_FILE_WRONLY;
        seek_mode = MR_SEEK_END;
    } else if (strcmp(mode, "a+") == 0) {  //以附加方式打开可读/写的文件。若文件不存在，则会建立该文件，如果文件存在，则写入的数据会被加到文件尾后，即文件原先的内容会被保留（原来的EOF符 不保留)。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY | MR_FILE_CREATE;
        seek_mode = MR_SEEK_END;
    } else if (strcmp(mode, "wb") == 0) {  //以只写方式打开或新建一个二进制文件，只允许写数据。
        file_mode = MR_FILE_WRONLY;
    } else if (strcmp(mode, "wb+") == 0) {  //以读/写方式打开或建立一个二进制文件，允许读和写。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY | MR_FILE_CREATE;
    } else if (strcmp(mode, "wt+") == 0) {  //以读/写方式打开或建立一个文本文件，允许读写。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY | MR_FILE_CREATE;
    } else if (strcmp(mode, "at+") == 0) {  //以读/写方式打开一个文本文件，允许读或在文本末追加数据。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
        seek_mode = MR_SEEK_END;
    } else if (strcmp(mode, "ab+") == 0) {  //以读/写方式打开一个二进制文件，允许读或在文件末追加数据。
        file_mode = MR_FILE_RDONLY | MR_FILE_WRONLY;
        seek_mode = MR_SEEK_END;
    }
     f = mrc_open(path, file_mode);
    if (f > 0) {
        mrc_seek(f, 0, seek_mode);
        // file->_tmpfname = path;
        file->_file = f;
        file->len = file_len;
        return file;
    }

    return NULL;
}

int capp_ferror(CAPP_FILE *file){
    mrc_printf("ferror");
    return 0;
}

int capp_getc(CAPP_FILE *f){
    
    unsigned char c = '\0';
    int32 re = mrc_read(f->_file,&c,1);
    mrc_printf("getc %d %d",c, re);
    if(re>0){
        return (int)c;
    }
    mrc_printf("getc EOF");
    return EOF;
}

int capp_fseek(CAPP_FILE *f, long offset, int mode) {
    return mrc_seek(f->_file, offset, mode);
}

int capp_fprintf(CAPP_FILE *f, char *format, ...){
    // #define __FPRINTF__(_fname_, ...) 
    // typedef char *  va_list;
    va_list params;
	// int len;
    mrc_printf("fprintf ........................");
	va_start(params, format);
	// len = vsnprintf(printfBuf, PRINTF_BUF_LEN, format, params);
	
	do{
		int32 h31415926len;
		char *msgBuffer;
		int32 h31415926 = f->_file;
		if(h31415926)	
		{
			msgBuffer = malloc(1024);
			if (!msgBuffer)
			{
				mrc_close(h31415926);
				break;
			}
			mrc_seek(h31415926,0,MR_SEEK_END);
			memset(msgBuffer, 0, 1024);
			h31415926len = vsnprintf(msgBuffer,255, format, params);
			mrc_write(h31415926, msgBuffer, h31415926len);
            mrc_printf("fprintf %s",msgBuffer);
			mrc_close(h31415926);
			free(msgBuffer);
		}
	}while(0);
    va_end(params);
    return 0;
}

int capp_ftell(CAPP_FILE *f){
    mrc_printf("ftell");
	if(f->_file > 0){
		return f->_file;
	}
	else
	return -1;
}

int capp_putchar(char ch){
    mrc_printf("putchar %c",ch);
	mrc_printf("%c",ch);
}

CAPP_FILE *capp_tmpfile(){
    mrc_printf("tmpfile");
    return capp_fopen("temp","wb+");
}

// void snprintf(char *text, int len, char *format, ...)
// {
// 	va_list params;
// 	int len;
// 	va_start(params, format);
// 	len = sprintf(printfBuf, format, params);
// 	va_end(params);
// }


int capp_fwrite(const void *buf, size_t size, size_t count, CAPP_FILE *f) {
    int32 re = mrc_write(f->_file, (void*)buf, size * count);
    if (re == -1)
        return -1;
    else
        return (int)re / size;
}


int capp_fflush(CAPP_FILE *f) {
    mrc_printf("fflush");
    // return mrc_close(f->_file);
}
int capp_fclose(CAPP_FILE *f) {
    return mrc_close(f->_file);
}


long mrc_strtol(const char *text,char **endptr,int base){
				int len = mrc_strlen((const char*)text);
				int math = 0;
				int ptr = 0;
                *endptr = (char *)text;
				if(len==0)return 0;
				//跳过0
				while(text[ptr]=='0'){
						ptr++;
					}

				while(ptr<len){
					switch(text[ptr])
						{
						case '0': 
						case '1': 
						case '2': 
						case '3': 
						case '4': 
						case '5': 
						case '6': 
						case '7': 
						case '8': 
						case '9': 
							math=math*10+(text[ptr]-48);

							break;
						case ' ':
						break;
						case '-':
							math=-math;
						break;
						default:
						    *endptr = ((char*)text+ptr);
							return math;
						}
						ptr++;
					}
				*endptr = (char*)text+ptr;	
	return math;
}

char *getenv(const char *encar){
    return NULL;
}

void *realloc(void *ptr, size_t newsize){
    void* buf;
    if(newsize == 0){
        // mrc_free(ptr);
        return ptr;
    }
    //ptr,(uint32)*(((uint32*)ptr)-1),(uint32)
    buf = mrc_malloc(newsize);
    // mrc_printf("malloc %d %d", (uint32)*(((uint32*)buf)-1), newsize);
    mrc_memset(buf,0,(int)newsize);
    if(ptr!=NULL){
        mrc_memcpy(buf,ptr,(uint32)*(((uint32*)ptr)-1));
        // mrc_free(ptr);
    }
    
    
    return buf;
}

char *capp_strpbrk (const char *s, const char *accept)
{
    while (*s != '\0')
    {
        const char *a = accept;
        while (*a != '\0')
            if (*a++ == *s)
                return (char *) s;
        ++s;
    }
    return NULL;
}

size_t capp_strspn (const char *s,const char *accept)
{
    const char *p;
    const char *a;
    size_t count = 0;
    for (p = s; *p != '\0'; ++p)
    {
        for (a = accept; *a != '\0'; ++a)
            if (*p == *a)
                break;
            if (*a == '\0')
                return count;
            else
                ++count;
    }
    return count;
}

/*

   这个函数是把浮点数字符串转换为浮点数的函数。

   函数将会跳过字符串中的空格字符和不是'+'、'-'、'.'、

   数字的字符。如果字符串是空的或者都是由空格组成，将不会

   做任何转换，仅仅是把字符串的结束地址赋给endptr。如果字

   符串合法，将会进行转换，并把字符串最后的NULL的地址给

   endptr。如果你想使用endptr参数，那么赋一个NULL值就

   可以了。

*/

double capp_strtod(const char* s, char** endptr)

{

   register const char*  p     = s;

    register long double  value = 0.L;

    int                   sign  = 0;

    long double           factor;

    unsigned int          expo;

  

    while ( isspace(*p) )//跳过前面的空格

      p++;

 

    if(*p == '-' || *p == '+')

      sign = *p++;//把符号赋给字符sign，指针后移。

  

   //处理数字字符

 

    while ( (unsigned int)(*p - '0') < 10u )//转换整数部分

      value = value*10 + (*p++ - '0');

   //如果是正常的表示方式（如：1234.5678）

   if ( *p == '.' )

   {

        factor = 1.;

        p++;

        while ( (unsigned int)(*p - '0') < 10u )

      {

         factor *= 0.1;

            value  += (*p++ - '0') * factor;

        }

    }

   //如果是IEEE754标准的格式（如：1.23456E+3）

    if ( (*p | 32) == 'e' )

   {

      expo   = 0;

        factor = 10.L;

        switch (*++p)

      {

        case '-':

           factor = 0.1;

        case '+':

           p++;

           break;

        case '0':

        case '1':

        case '2':

        case '3':

        case '4':

        case '5':

        case '6':

        case '7':

        case '8':

        case '9':

           break;

        default :

           value = 0.L;

           p     = s;

           goto done;

        }

        while ( (unsigned int)(*p - '0') < 10u )

            expo = 10 * expo + (*p++ - '0');

        while ( 1 )

      {

        if ( expo & 1 )

           value *= factor;

            if ( (expo >>= 1) == 0 )

                break;

            factor *= factor;

        }

    }

done:

    if ( endptr != 0 )

        *endptr = (char*)p;

 

    return (sign == '-' ? -value : value);

}