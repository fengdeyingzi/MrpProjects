# 动态壁纸显示


这是一个动态壁纸显示程序，程序运行逻辑如下：

1. 程序入口为main.c，首次运行进行资源解压，解压合成bmas文件到内存卡
2. 加载bmas图集文件，用定时器进行播放（每隔33毫秒切换一张图），按OK键暂停/继续播放，按返回键返回主菜单
3. 应用支持按键和触屏
4. 加载bmas图集时，bitmap不用每次申请内存，直接一次性申请300kb内存用于bitmap缓存，在程序退出时释放


资源解压逻辑：
通过循环mrc_readFileFromAssets读取 "24.bmas.%03d" 序列文件，依次写入合并成一个文件（写入内存卡文件夹mrc_getPackName()目录下，如果目录不存在，则使用mrc_mkDir创建
当读取到的数据为NULL时，跳出循环。

注意：功能机平台可使用的内存大小为550kb，不可一次申请大量内存。

相关资料：
读取内存卡文件读写函数，在fopen.h文件中
开发规范参见 功能机开发规范.md
bma格式图片可以通过

以下函数定义在mrc_base.h
```c
/*
获取当前正在运行的mrp的文件名称
mythroad中支持的最大文件长度为127字节

返回:
      当前正在运行的mrp的文件名称
*/
char* mrc_getPackName(void);

/*
释放由mrc_readFileFromMrp函数读取的文件内容空间。
输入:
data     文件内容指针
filelen          文件内容长度
*/
void mrc_freeFileData(void* data, int32 filelen);

/*
创建目录。
输入:
name	目录名
返回:
      MR_SUCCESS  成功
      MR_FAILED   失败
*/
int32 mrc_mkDir(const char* name);

/*
取得文件类型信息。
输入:
filename	文件名
返回:
      MR_IS_FILE     1//是文件
      MR_IS_DIR      2//是目录
      MR_IS_INVALID  8//文件不存在，或既不是文件也不是目录
*/
int32 mrc_fileState(const char* filename);
```

以下函数定义在mrc_android.h
```c
//从aasts目录读取文件
extern void *mrc_readFileFromAssets(const char *filename, int32 *len);

//释放assets目录读取的文件 （作废）
void mrc_freeFileFromAssets(void *data,int32 len);
```

以下定义在mrc_graphics.h中
```c
typedef struct {
 uint16* bitmap; //缓存数据，必须通过mr_malloc申请
 int width; //
 int height;
 int color_bit; //颜色位数 默认16
 int transcolor; //透明色 0表示不透明
 int mode; //绘制模式 默认BM_COPY
 int32 buflen; //缓存区长度，用于释放
} BITMAP_565;

// 申请bitmap内存，释放使用mrc_freeFileData
void* mr_malloc(int32 len);
```


