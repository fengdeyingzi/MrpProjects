#include "bridge.h"
#include "mrc_base.h"
#include <stdarg.h>
#include <stdlib.h>

// 是否使用UTF-8编码
#define USE_UTF8 0

extern int32 mrc_init(void);
extern int32 mrc_event(int32 code, int32 p0, int32 p1);
extern int32 mrc_exitApp(void);
extern int32 mrc_pause(void);
extern int32 mrc_resume(void);


void *(*MR_MEMCPY)(void * s1, const void * s2, int n) = NULL;
void *(*MR_MEMMOVE)(void * s1, const void * s2, int n) = NULL;
char *(*MR_STRCPY)(char * s1, const char * s2) = NULL;
char *(*MR_STRNCPY)(char * s1, const char * s2, int n) = NULL;
char *(*MR_STRCAT)(char * s1, const char * s2)
    = NULL;
char *(*MR_STRNCAT)(char * s1, const char * s2, int n)
    = NULL;
int (*MR_MEMCMP)(const void * s1, const void * s2, int
    n) = NULL;
int (*MR_STRCMP)(const char * s1, const char * s2)
    = NULL;
int (*MR_STRNCMP)(const char * s1, const char * s2, int n)
    = NULL;
int (*MR_STRCOLL)(const char * s1, const char * s2)
    = NULL;
void *(*MR_MEMCHR)(const void * s, int c, int n) = NULL;
void *(*MR_MEMSET)(void * s, int c, int n) = NULL
    ;
int (*MR_STRLEN)(const char * s) = NULL;
char *(*MR_STRSTR)(const char * s1, const char * s2)
    = NULL;
int (*MR_SPRINTF)(char * s, const char * format, ...)
    = NULL;
int (*MR_ATOI)(const char * nptr) = NULL;
unsigned long int (*MR_STRTOUL)(const char * nptr, char ** endptr, int base)
    = NULL;


uint16 SCRW, SCRH;
mr_table_st *mr_table;
mr_c_function_st *mr_c_function;
inFuncs_st *inFuncs;
void *mrc_malloc(int size){
    return inFuncs->mrc_malloc(size);
}
void mrc_free(void *address){
    inFuncs->mrc_free(address);
}

int32 mrc_sleep(uint32 ms){
    return mr_table -> mr_sleep(ms);
}

 int32 mrc_platExCP(int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb){
    return mr_table -> mr_platEx(code, input, input_len, output, output_len, cb);
 }
 int32 mrc_platCP(int32 code, int32 param){
    return mr_table -> mr_plat(code, param);
 }

 // 在代码中添加简单的 memcpy 实现
void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

const char *mrc_strchr(const char *src,int c)
{
    while(*src!='\0')
    {
        if(*src==(char)c)
            return (char*)src;
        src++;
    }
    if(c=='\0')
        return (char*)src;
    return NULL;
}


 int32 mrc_setReloadFile(char *pack_filename, char *start_filename){
    // if(pack_filename == NULL && start_filename == NULL){
    //     mr_table->old_pack_filename = mr_table->pack_filename;
    //     mr_table->old_start_filename = mr_table->start_filename;
    //     return 0;
    // }
    // mr_table->old_pack_filename = pack_filename;
    // mr_table->old_start_filename = start_filename;
    mr_table->memset(mr_table->old_pack_filename, 0, 0x20);  // 清空32字节
    mr_table->memset(mr_table->old_start_filename, 0, 0x20);  // 清空32字节
    // // 将当前的 pack_filename 备份到 old_pack_filename (偏移 408)
    mr_table->strcpy(mr_table->old_pack_filename, pack_filename);
    // // 将当前的 start_filename 备份到 old_start_filename (偏移 412)
    mr_table->strcpy(mr_table->old_start_filename, start_filename);
    return 0;
 }

 int32 mrc_runMrp(char* pack_filename_param, char* start_filename_param, char* parameter)
  {
        
    mrc_printf("============= start filename = %s %s", mr_table->pack_filename, mr_table->start_filename);
    /*
      mr_internal_table* internal_table = mr_table->_mr_c_internal_table;
 // 1. 【修复】：先保存当前现场到 old_* 变量
    // 将当前的 pack_filename 备份到 old_pack_filename (偏移 408)
    mr_table->strcpy(mr_table->old_pack_filename, mr_table->pack_filename);
    // 将当前的 start_filename 备份到 old_start_filename (偏移 412)
    mr_table->strcpy(mr_table->old_start_filename, mr_table->start_filename);

      // 如果 pack_filename 与参数不同，更新它
      if (mr_table->pack_filename != pack_filename_param) {
          mr_table->memset(mr_table->pack_filename, 0, 0x20);  // 清空32字节
          mr_table->strncpy(mr_table->pack_filename, pack_filename_param, 0x1f);  // 复制31字 节
      }

      // 如果 start_filename 与参数不同，更新它
      if (mr_table->start_filename != start_filename_param) {
          mr_table->memset(mr_table->start_filename, 0, 0x20);  // 清空32字节
          mr_table->strncpy(mr_table->start_filename, start_filename_param, 0x1f);  // 复制31 字节
      }

      // 设置定时器相关状态
      *internal_table->mr_timer_p = (void*)0x89600;  // 设置定时器指针
      mr_table->g_mr_timerStart(100);                 // 启动100ms定时器
      *internal_table->mr_timer_state = 1;            // 设置定时器状态为运行
      *internal_table->mr_state = 3;                  // 设置MR状态为3
     */
    if(inFuncs->mrc_runMrp){
        inFuncs->mrc_runMrp(pack_filename_param, start_filename_param, parameter);
    }
      return 0;
  }

int32 mrc_getVersion(void){
    if(inFuncs->mrc_getVersion){
        return inFuncs->mrc_getVersion();
    }
    return 0;
  }

   int32 mrc_getDatetime(mr_datetime* datetime){
    return mr_table -> mr_getDatetime(datetime);
   }




/*
  int32 mrc_runMrp(char* newPackName, char* newStartName, char *param)
{
    mr_internal_table* internal_table = mr_table->_mr_c_internal_table;

    // 1. 【修复】：先保存当前现场到 old_* 变量
    // 将当前的 pack_filename 备份到 old_pack_filename (偏移 408)
    mr_table->strcpy(mr_table->old_pack_filename, mr_table->pack_filename);
    // 将当前的 start_filename 备份到 old_start_filename (偏移 412)
    mr_table->strcpy(mr_table->old_start_filename, mr_table->start_filename);

    // 2. 然后再更新当前文件名 (原有的逻辑)
    if (mr_table->pack_filename != newPackName) {
        mr_table->memset(mr_table->pack_filename, 0, 32);
        mr_table->strncpy(mr_table->pack_filename, newPackName, 31);
    }
    
    if (mr_table->start_filename != newStartName) {
        mr_table->memset(mr_table->start_filename, 0, 32);
        mr_table->strncpy(mr_table->start_filename, newStartName, 31);
    }

    // 3. 切换回调并运行
    *(void**)internal_table->mr_timer_p = (void*)0x89600; // 子应用回调
    mr_table->g_mr_timerStart(100);
    // ...
    return 0;
}
*/

 /*
功能:
	打开、关闭LCD定时休眠功能。
	
输入:
	char CanSleep=1 时，允许LCD定时休眠；
	char CanSleep=0时，不允许LCD定时休眠。

输出：
MR_SUCCESS ：成功
MR_FAILED  ：失败
MR_IGNORE  : 不支持该功能
*/
int32 mrc_LCDCanSleep(char CanSleep){
    int32 input_len = 0;
    mrc_platExCP(CanSleep ? 1223 : 1222, NULL, input_len, NULL, NULL, NULL);
}

void mrc_freeFileData(void* data, int32 filelen){
    inFuncs->mrc_freeFileData(data, filelen);
}

void *mrc_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor){
    return inFuncs->mrc_readFileFromMrp(filename, filelen, lookfor);
}

uint16 *w_getScreenBuffer() {
    return *(mr_table->mr_screenBuf);
}

/*
创建定时器

返回:
      非NULL     定时器句柄
      NULL          失败
*/
int32 mrc_timerCreate(void) {
    return inFuncs->mrc_timerCreate();
}

/*
删除定时器

输入:
t           定时器句柄
*/
void mrc_timerDelete(int32 t) {
    inFuncs->mrc_timerDelete(t);
}

/*
停止定时器

输入:
t           定时器句柄
*/
void mrc_timerStop(int32 t) {
    inFuncs->mrc_timerStop(t);
}

/*
启动定时器

输入:
t           定时器句柄
time      定时器时长，单位毫秒
data      定时器数据
f           定时器回调函数
loop      是否循环；0:不循环，1:循环

返回:
      MR_SUCCESS     成功
      MR_FAILED         失败
*/
int32 mrc_timerStart(int32 t, int32 time, int32 data, mrc_timerCB f, int32 loop) {
    return inFuncs->mrc_timerStart(t, time, data, f, loop);
}

/*
更改定时器时长。

输入:
t           定时器句柄
time      定时器时长，单位毫秒

返回:
      MR_SUCCESS     成功
      MR_FAILED         失败
*/
int32 mrc_timerSetTimeEx(int32 t, int32 time) {
    return inFuncs->mrc_timerSetTimeEx(t, time);
}

/********************************文件接口********************************/
/*
该函数用于调试使用，返回的是最后一次操作文件
失败的错误信息，返回的错误信息具体含义与平台
上使用的文件系统有关。
该函数可能被实现为总是返回MR_SUCCESS。
返回:
      最后一次操作文件失败的错误信息
*/
int32 mrc_ferrno(void) {
    return mr_table->mr_ferrno();
}

/*
以mode方式打开文件，如果文件不存在，根据mode值
判断是否创建之。
返回:
非 NULL	       文件句柄
NULL	            失败（注意，这里与其他接口不一样）
*/
int32 mrc_open(const char* filename, uint32 mode) {
    return mr_table->mr_open(filename, mode);
}

/*
关闭文件。
返回:
MR_SUCCESS	成功
MR_FAILED	失败
*/
int32 mrc_close(int32 f) {
    return mr_table->mr_close(f);
}

/*
取得文件类型信息。
返回:
      MR_IS_FILE     1//是文件
      MR_IS_DIR      2//是目录
      MR_IS_INVALID  8//文件不存在，或既不是文件也不是目录
*/
int32 mrc_fileState(const char* filename) {
    return mr_table->mr_info(filename);
}

/*
写文件
返回:
      >0                   确切写入的字节数
      MR_FAILED      失败
*/
int32 mrc_write(int32 f, void *p, uint32 l) {
    return mr_table->mr_write(f, p, l);
}

/*
读取文件的内容到指定的缓冲。
返回:
      >=0                确切读取的字节数
      MR_FAILED      失败
*/
int32 mrc_read(int32 f, void *p, uint32 l) {
    return mr_table->mr_read(f, p, l);
}

/*
读取文件的所有内容到申请的内存中。
返回:
      非NULL         指向读取到的内容的指针，该内存需要调用者释放
      NULL              失败
*/
void* mrc_readAll(const char* filename, uint32 *len) {
    int32 file = mrc_open(filename, MR_FILE_RDONLY);
    if (file <= 0) {
        return NULL;
    }
    
    int32 file_len = mrc_getLen(filename);
    if (file_len <= 0) {
        mrc_close(file);
        return NULL;
    }
    
    void *buffer = mrc_malloc(file_len);
    if (!buffer) {
        mrc_close(file);
        return NULL;
    }
    
    int32 read_len = mrc_read(file, buffer, file_len);
    mrc_close(file);
    
    if (read_len != file_len) {
        mrc_free(buffer);
        return NULL;
    }
    
    if (len) {
        *len = file_len;
    }
    
    return buffer;
}

/*
设置文件指针。
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_seek(int32 f, int32 pos, int method) {
    return mr_table->mr_seek(f, pos, method);
}

/*
取得文件长度。
返回:
      >=0   文件长度
      MR_FAILED   失败/文件不存在
*/
int32 mrc_getLen(const char* filename) {
    return mr_table->mr_getLen(filename);
}

/*
删除文件。
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_remove(const char* filename) {
    return mr_table->mr_remove(filename);
}

/*
文件重命名。
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_rename(const char* oldname, const char* newname) {
    return mr_table->mr_rename(oldname, newname);
}

/*
创建目录。
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_mkDir(const char* name) {
    return mr_table->mr_mkDir(name);
}

/*
删除目录以及目录下面的文件和子目录。
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_rmDir(const char* name) {
    return mr_table->mr_rmDir(name);
}

/*
准备搜索name指定的目录。
返回:
      >0                  查找句柄，供mr_c_findGetNext、mr_c_findStop函数使用
      MR_FAILED      失败
*/
int32 mrc_findStart(const char* name, char* buffer, uint32 len) {
    return mr_table->mr_findStart(name, buffer, len);
}

/*
取得一个目录搜索的结果。
返回:
      MR_SUCCESS  搜索成功
      MR_FAILED   搜索结束或搜索失败
*/
int32 mrc_findGetNext(int32 search_handle, char* buffer, uint32 len) {
    return mr_table->mr_findGetNext(search_handle, buffer, len);
}

/*
目录搜索结束。中止一个mr_c_findStart开启的目录搜索。
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_findStop(int32 search_handle) {
    return mr_table->mr_findStop(search_handle);
}

/********************************绘图接口********************************/
int32 mrc_drawChar(uint8* chr, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, uint16 font) {
    // 提取unicode字符（大端字节序）
    uint16 unicode_char = (chr[0] << 8) | chr[1];
    
    // 将RGB颜色转换为nativecolor格式 (R:G:B = 5:6:5)
    uint16 nativecolor = ((colorst.r & 0xF8) << 8) | ((colorst.g & 0xFC) << 3) | ((colorst.b & 0xF8) >> 3);
    
    // 调用平台绘图函数
    if (mr_table->mr_platDrawChar) {
        mr_table->mr_platDrawChar(unicode_char, x, y, nativecolor);
        return MR_SUCCESS;
    }
    
    return MR_FAILED;
}

static uint16 swap_bytes_shift(uint16 value) {
    return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
}

int32 mrc_drawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font) {
    // 参照 uc3_drawText 优化，添加 UTF-8 支持
    if (is_unicode) {
        // is_unicode 为 1 时，使用 Unicode（大端）
        return inFuncs->mrc_drawText(pcText, x, y, r, g, b, is_unicode, font);
    } else {
        #if USE_UTF8
        {
            // 使用 UTF-8 编码，需要转换为 Unicode
            uint8 *utf8 = (uint8 *)pcText;
            int32 total_len = mrc_strlen(pcText);
            // 分配缓冲区存储转换后的 Unicode 字符串
            uint16 *unicode_buf = (uint16 *)mrc_malloc((total_len + 1) * sizeof(uint16));
            if (unicode_buf == NULL) {
                return MR_FAILED;
            }

            uint16 *ptr = unicode_buf;
            while (*utf8) {
                uint16 unicode;
                // UTF-8 解码
                if ((*utf8 & 0x80) == 0) {
                    // 单字节 ASCII
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    // 双字节 UTF-8
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    // 三字节 UTF-8
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }
                *ptr++ = swap_bytes_shift(unicode); // 存储为大端序
            }
            *ptr = 0; // 字符串结束符
            
            // 调用底层函数，设置 is_unicode=1
            int32 result = inFuncs->mrc_drawText((char*)unicode_buf, x, y, r, g, b, 1, font);

            mrc_free(unicode_buf);
            return result;
        }
        #else
        {
            // 非 UTF-8 模式，使用 GBK 编码
            return inFuncs->mrc_drawText(pcText, x, y, r, g, b, is_unicode, font);
        }
        #endif
    }
}

int32 mrc_drawTextEx(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font) {
    // flag 参数包含 is_unicode 信息（通常 flag & 0x01 表示 is_unicode）
    int is_unicode = flag & 0x01;

    if (is_unicode) {
        // is_unicode 为 1 时，使用 Unicode（大端）
        return mr_table->_DrawTextEx(pcText, x, y, rect, colorst, flag, font);
    } else {
        #if USE_UTF8
        {
            // 使用 UTF-8 编码，需要转换为 Unicode
            uint8 *utf8 = (uint8 *)pcText;
            int32 total_len = mrc_strlen(pcText);
            // 分配缓冲区存储转换后的 Unicode 字符串
            uint16 *unicode_buf = (uint16 *)mrc_malloc((total_len + 1) * sizeof(uint16));
            if (unicode_buf == NULL) {
                return MR_FAILED;
            }

            uint16 *ptr = unicode_buf;
            while (*utf8) {
                uint16 unicode;
                // UTF-8 解码
                if ((*utf8 & 0x80) == 0) {
                    // 单字节 ASCII
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    // 双字节 UTF-8
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    // 三字节 UTF-8
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }
                *ptr++ = swap_bytes_shift(unicode); // 存储为大端序
            }
            *ptr = 0; // 字符串结束符

            // 调用底层函数，设置 is_unicode=1（修改 flag）
            int new_flag = flag | 0x01; // 设置 Unicode 标志位
            int32 result = mr_table->_DrawTextEx((char*)unicode_buf, x, y, rect, colorst, new_flag, font);

            mrc_free(unicode_buf);
            return result;
        }
        #else
        {
            // 非 UTF-8 模式，使用 GBK 编码
            return mr_table->_DrawTextEx(pcText, x, y, rect, colorst, flag, font);
        }
        #endif
    }
}

int32 mrc_drawTextExVM(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font) {
    // flag 参数包含 is_unicode 信息（通常 flag & 0x01 表示 is_unicode）
    int is_unicode = flag & 0x01;

    if (is_unicode) {
        // is_unicode 为 1 时，使用 Unicode（大端）
        return mr_table->_DrawTextEx(pcText, x, y, rect, colorst, flag, font);
    } else {
        #if USE_UTF8
        {
            // 使用 UTF-8 编码，需要转换为 Unicode
            uint8 *utf8 = (uint8 *)pcText;
            int32 total_len = mrc_strlen(pcText);
            // 分配缓冲区存储转换后的 Unicode 字符串
            uint16 *unicode_buf = (uint16 *)mrc_malloc((total_len + 1) * sizeof(uint16));
            if (unicode_buf == NULL) {
                return MR_FAILED;
            }

            uint16 *ptr = unicode_buf;
            while (*utf8) {
                uint16 unicode;
                // UTF-8 解码
                if ((*utf8 & 0x80) == 0) {
                    // 单字节 ASCII
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    // 双字节 UTF-8
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    // 三字节 UTF-8
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }
                *ptr++ = swap_bytes_shift(unicode); // 存储为大端序
            }
            *ptr = 0; // 字符串结束符

            // 调用底层函数，设置 is_unicode=1（修改 flag）
            int new_flag = flag | 0x01; // 设置 Unicode 标志位
            int32 result = mr_table->_DrawTextEx((char*)unicode_buf, x, y, rect, colorst, new_flag, font);

            mrc_free(unicode_buf);
            return result;
        }
        #else
        {
            // 非 UTF-8 模式，使用 GBK 编码
            return mr_table->_DrawTextEx(pcText, x, y, rect, colorst, flag, font);
        }
        #endif
    }
}

int32 mrc_drawTextLeft(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font) {
    // flag 参数包含 is_unicode 信息（通常 flag & 0x01 表示 is_unicode）
    int is_unicode = flag & 0x01;

    if (is_unicode) {
        // is_unicode 为 1 时，使用 Unicode（大端）
        return mr_table->_DrawTextEx(pcText, x, y, rect, colorst, flag, font);
    } else {
        #if USE_UTF8
        {
            // 使用 UTF-8 编码，需要转换为 Unicode
            uint8 *utf8 = (uint8 *)pcText;
            int32 total_len = mrc_strlen(pcText);
            // 分配缓冲区存储转换后的 Unicode 字符串
            uint16 *unicode_buf = (uint16 *)mrc_malloc((total_len + 1) * sizeof(uint16));
            if (unicode_buf == NULL) {
                return MR_FAILED;
            }

            uint16 *ptr = unicode_buf;
            while (*utf8) {
                uint16 unicode;
                // UTF-8 解码
                if ((*utf8 & 0x80) == 0) {
                    // 单字节 ASCII
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    // 双字节 UTF-8
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    // 三字节 UTF-8
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }
                *ptr++ = swap_bytes_shift(unicode); // 存储为大端序
            }
            *ptr = 0; // 字符串结束符

            // 调用底层函数，设置 is_unicode=1（修改 flag）
            int new_flag = flag | 0x01; // 设置 Unicode 标志位
            int32 result = mr_table->_DrawTextEx((char*)unicode_buf, x, y, rect, colorst, new_flag, font);

            mrc_free(unicode_buf);
            return result;
        }
        #else
        {
            // 非 UTF-8 模式，使用 GBK 编码
            return mr_table->_DrawTextEx(pcText, x, y, rect, colorst, flag, font);
        }
        #endif
    }
}

int32 mrc_drawTextRight(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font) {
    if (!pcText) {
        return 0;
    }

    // flag 参数包含 is_unicode 信息（通常 flag & 0x01 表示 is_unicode）
    int is_unicode = flag & 0x01;

    // 将RGB颜色转换为nativecolor格式
    uint16 nativecolor = ((colorst.r & 0xF8) << 8) | ((colorst.g & 0xFC) << 3) | ((colorst.b & 0xF8) >> 3);

    uint16* unicode_text = NULL;
    int32 size = 0;
    int32 err = 0;

    if (is_unicode) {
        // is_unicode 为 1 时，使用 Unicode（大端）
        unicode_text = (uint16*)pcText;
        // 计算 Unicode 字符串长度
        size = 0;
        while (unicode_text[size]) {
            size++;
        }
        size++; // 包含结束符
    } else {
        #if USE_UTF8
        {
            // 使用 UTF-8 编码，需要转换为 Unicode
            uint8 *utf8 = (uint8 *)pcText;
            int32 total_len = mrc_strlen(pcText);
            // 分配缓冲区存储转换后的 Unicode 字符串
            unicode_text = (uint16 *)mrc_malloc((total_len + 1) * sizeof(uint16));
            if (unicode_text == NULL) {
                return 0;
            }

            uint16 *ptr = unicode_text;
            while (*utf8) {
                uint16 unicode;
                // UTF-8 解码
                if ((*utf8 & 0x80) == 0) {
                    // 单字节 ASCII
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    // 双字节 UTF-8
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    // 三字节 UTF-8
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }
                *ptr++ = swap_bytes_shift(unicode); // 存储为大端序
                size++;
            }
            *ptr = 0; // 字符串结束符
        }
        #else
        {
            // 非 UTF-8 模式，使用 GBK 编码（通过 mrc_c2u 转换）
            unicode_text = mrc_c2u(pcText, &err, &size);
            if (!unicode_text) {
                return 0;
            }
        }
        #endif
    }

    // 计算文本总宽度
    int32 total_width = 0;
    int32 char_count = 0;
    for (int32 i = 0; i < size && unicode_text[i]; i++) {
        int char_width = 0, char_height = 0;
        mr_table->mr_getCharBitmap(unicode_text[i], font, &char_width, &char_height);
        
            total_width += char_width;
            char_count++;
        
    }

    // 从右向左绘制
    int32 current_x = x;
    int32 drawn_count = 0;

    for (int32 i = char_count - 1; i >= 0; i--) {
        uint16 ch = unicode_text[i];
        int char_width = 0, char_height = 0;
        mr_table->mr_getCharBitmap(ch, font, &char_width, &char_height);

        
            // 检查是否在矩形范围内
            if (current_x - char_width >= rect.x && current_x <= rect.x + rect.w &&
                y >= rect.y && y + char_height <= rect.y + rect.h) {
                // 绘制字符
                if (mr_table->mr_platDrawChar) {
                    mr_table->mr_platDrawChar(ch, current_x - char_width, y, nativecolor);
                    drawn_count++;
                }
            }
            current_x -= char_width;
        
    }

    // 释放转换后的内存（如果是动态分配的）
    #if USE_UTF8
    if (!is_unicode) {
        mrc_free(unicode_text);
    }
    #else
    if (!is_unicode) {
        mrc_free(unicode_text);
    }
    #endif

    return drawn_count;
}

uint16* mrc_c2u(char *cp, int32 *err, int32 *size) {
    return mr_table->c2u(cp, err, size);
}

uint16* mrc_c2uVM(char *cp, int32 *err, int32 *size) {
    // 使用VM中的码表进行变换
    return mr_table->c2u(cp, err, size);
}

int32 mrc_textWidthHeight(char* pcText, int is_unicode, uint16 font, int32* w, int32* h) {
    if (!pcText || !w || !h) {
        return MR_FAILED;
    }

    int32 total_width = 0;
    int32 max_height = 0;
    int char_width = 0, char_height = 0;

    if (is_unicode) {
        // Unicode字符串
        uint16* unicode_text = (uint16*)pcText;
        int32 i = 0;
        int char_width = 0, char_height = 0;
        while (unicode_text[i]) {
            uint16 ch = unicode_text[i];
            ch = swap_bytes_shift(ch);

            // 获取字符位图信息
            mr_table->mr_getCharBitmap(ch, font, &char_width, &char_height);
            
                total_width += char_width;
                if (char_height > max_height) {
                    max_height = char_height;
                }
            
            i++;
        }
    } else {
        #if USE_UTF8
        {
            // 使用 UTF-8 编码，需要转换为 Unicode
            uint8 *utf8 = (uint8 *)pcText;
            int char_width = 0, char_height = 0;
            while (*utf8) {
                uint16 unicode;
                // UTF-8 解码
                if ((*utf8 & 0x80) == 0) {
                    // 单字节 ASCII
                    unicode = *utf8++;
                } else if ((*utf8 & 0x20) == 0) {
                    // 双字节 UTF-8
                    unicode = ((utf8[0] & 0x1f) << 6) | (utf8[1] & 0x3f);
                    utf8 += 2;
                } else {
                    // 三字节 UTF-8
                    unicode = ((utf8[0] & 0x0f) << 12) | ((utf8[1] & 0x3f) << 6) | (utf8[2] & 0x3f);
                    utf8 += 3;
                }

                
                // 获取字符位图信息
                mr_table->mr_getCharBitmap(unicode, font, &char_width, &char_height);
                
                    total_width += char_width;
                    if (char_height > max_height) {
                        max_height = char_height;
                    }
                
            }
        }
        #else
        {
            // GB2312字符串，需要转换为Unicode
            int32 err = 0, size = 0;
            uint16* unicode_text = mrc_c2u(pcText, &err, &size);
            if (!unicode_text) {
                return MR_FAILED;
            }

            int32 i = 0;
            while (i < size && unicode_text[i]) {
                uint16 ch = unicode_text[i];
                ch = swap_bytes_shift(ch);
                

                // 获取字符位图信息
                mr_table->mr_getCharBitmap(ch, font, &char_width, &char_height);
                
                    total_width += char_width;
                    if (char_height > max_height) {
                        max_height = char_height;
                    }
                
                i++;
            }

            // 释放转换后的内存
            mrc_free(unicode_text);
        }
        #endif
    }

    *w = total_width;
    *h = max_height;
    mrc_printf("============ total width %d", total_width);
    return MR_SUCCESS;
}

int32 mrc_unicodeTextWidthHeight(uint16* pcText, uint16 font, int32* w, int32* h) {
    if (!pcText || !w || !h) {
        return MR_FAILED;
    }
    
    int32 total_width = 0;
    int32 max_height = 0;
    
    int32 i = 0;
    while (pcText[i]) {
        uint16 ch = pcText[i];
        int char_width = 0, char_height = 0;
        
        // 获取字符位图信息
        mr_table->mr_getCharBitmap(ch, font, &char_width, &char_height);
        
            total_width += char_width;
            if (char_height > max_height) {
                max_height = char_height;
            }
        
        i++;
    }
    
    *w = total_width;
    *h = max_height;
    return MR_SUCCESS;
}

int32 mrc_unicodeTextRow(uint16* pcText, uint16 font, int32 w) {
    if (!pcText || w <= 0) {
        return MR_FAILED;
    }
    
    int32 rows = 1;
    int32 current_width = 0;
    
    int32 i = 0;
    while (pcText[i]) {
        uint16 ch = pcText[i];
        int char_width = 0, char_height = 0;
        
        // 获取字符位图信息
        mr_table->mr_getCharBitmap(ch, font, &char_width, &char_height);
        
            if (current_width + char_width > w) {
                // 换行
                rows++;
                current_width = char_width;
            } else {
                current_width += char_width;
            }
        
        i++;
    }
    
    return rows;
}

int32 mrc_getScreenInfo(mr_screeninfo * screeninfo) {
    return mr_table->mr_getScreenInfo(screeninfo);
}

void mrc_drawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b) {
    mr_table->DrawRect(x, y, w, h, r, g, b);
}

void mrc_drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 r, uint8 g, uint8 b) {
    // 使用Bresenham直线算法绘制线段
    int16 dx = abs(x2 - x1);
    int16 dy = abs(y2 - y1);
    int16 sx = (x1 < x2) ? 1 : -1;
    int16 sy = (y1 < y2) ? 1 : -1;
    int16 err = dx - dy;
    
    // 将RGB转换为nativecolor格式
    uint16 nativecolor = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
    
    while (1) {
        // 绘制当前点
        mrc_drawPoint(x1, y1, nativecolor);
        
        // 如果到达终点则退出
        if (x1 == x2 && y1 == y2) {
            break;
        }
        
        int16 e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void mrc_drawPoint(int16 x, int16 y, uint16 nativecolor) {
    mr_table->_DrawPoint(x, y, nativecolor);
}

void mrc_drawPointEx(int16 x, int16 y, int32 r, int32 g, int32 b) {
    // 将RGB转换为nativecolor格式 (R:G:B = 5:6:5)
    uint16 nativecolor = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
    mr_table->_DrawPoint(x, y, nativecolor);
}

void mrc_clearScreen(int32 r, int32 g, int32 b) {
    inFuncs->mrc_clearScreen(r, g, b);
}

void mrc_refreshScreen(int16 x, int16 y, uint16 w, uint16 h) {
    inFuncs->mrc_refreshScreen(x, y, w, h);
}

void mrc_sand(uint32 seed) {
    // 设置随机数种子
    inFuncs->mrc_sand(seed);
}

uint32 mrc_rand(void) {
    // 生成随机数
    if (mr_table->rand) {
        return mr_table->rand();
    }
    return 0;  // 如果没有rand函数，返回0
}

int32 mrc_getUptime(void) {
    // 获取系统时间，单位毫秒
    if (mr_table->mr_getTime) {
        return mr_table->mr_getTime();
    }
    return 0;  // 如果没有时间函数，返回0
}



int32 mrc_wstrlen(const char *txt) {
    int32 i = 0;
    if (txt == NULL) return 0;
    while (txt[i] || txt[i + 1]) i += 2;

    return i;
}

// 退出函数
void mrc_exit(void) {
    if (inFuncs->mrc_exit) {
        inFuncs->mrc_exit();
    }
}


outFuncs_st *_start(inFuncs_st *in) {
    outFuncs_st *ret = in->mrc_malloc(sizeof(outFuncs_st));
    ret->mrc_init = mrc_init;
    ret->mrc_event = mrc_event;
    ret->mrc_exitApp = mrc_exitApp;
    ret->mrc_pause = mrc_pause;
    ret->mrc_resume = mrc_resume;

    // 把一些常用的函数放到全局变量
    mr_table = in->mr_table;
    mr_c_function = in->mr_c_function;
    // mrc_malloc = in->mrc_malloc;
    // mrc_free = in->mrc_free;
    // mrc_clearScreen = in->mrc_clearScreen;
    // mrc_drawText = in->mrc_drawText;
    // mrc_refreshScreen = in->mrc_refreshScreen;
    // mrc_readFileFromMrp = inFuncs->mrc_readFileFromMrp;

    SCRW = (uint16)(*in->mr_table->mr_screen_w);
    SCRH = (uint16)(*in->mr_table->mr_screen_h);

    
    MR_MEMCPY = *in->mr_table->memcpy;
    MR_MEMMOVE = *in->mr_table->memmove;
    MR_STRCPY = *in->mr_table->strcpy;
    MR_STRNCPY = *in->mr_table->strncpy;
    MR_STRCAT = *in->mr_table->strcat;
    MR_STRNCAT = *in->mr_table->strncat;
    MR_MEMCMP = *in->mr_table->memcmp;
    MR_STRCMP = *in->mr_table->strcmp;
    MR_STRNCMP = *in->mr_table->strncmp;
    MR_STRCOLL = *in->mr_table->strcoll;
    MR_MEMCHR = *in->mr_table->memchr;
    MR_MEMSET = *in->mr_table->memset;
    MR_STRLEN = *in->mr_table->strlen;
    MR_STRSTR = *in->mr_table->strstr;
    MR_SPRINTF = *in->mr_table->sprintf;
    MR_ATOI = *in->mr_table->atoi;
    MR_STRTOUL = *in->mr_table->strtoul;
    


    inFuncs = in;

    return ret;
}

// 实现strrchr函数
char *mrc_strrchr(const char *s, int c) {
    char *last = NULL;
    if (s == NULL) return NULL;
    
    while (*s) {
        if (*s == (char)c) {
            last = (char *)s;
        }
        s++;
    }
    
    // 处理查找'\0'的情况
    if (c == '\0') {
        return (char *)s;
    }
    
    return last;
}