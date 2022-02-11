#include <mrc_base.h>
#include "base.h"
CAPP_FILE *capp_fopen(const char *path, const char *mode) {
    int file_mode = 0;
    int32 f;
    CAPP_FILE *file;
    int seek_mode = MR_SEEK_SET;
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


int capp_fseek(CAPP_FILE *f, long offset, int mode) {
    return mrc_seek(f->_file, offset, mode);
}

int capp_ftell(CAPP_FILE *f){
	if(f->_file > 0){
		return f->_file;
	}
	else
	return -1;
}

int capp_putchar(char ch){
	mrc_printf("%c",ch);
}

// void snprintf(char *text, int len, char *format, ...)
// {
// 	va_list params;
// 	int len;
// 	va_start(params, format);
// 	len = sprintf(printfBuf, format, params);
// 	va_end(params);
// }


int capp_fwrite(void *buf, size_t size, size_t count, CAPP_FILE *f) {
    int32 re = mrc_write(f->_file, buf, size * count);
    if (re == -1)
        return -1;
    else
        return (int)re / size;
}


int capp_fflush(CAPP_FILE *f) {
    return mrc_close(f->_file);
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
