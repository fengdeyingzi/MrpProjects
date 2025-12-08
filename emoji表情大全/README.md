# emoji表情大全

进入程序后，从mrp中读取emoji.txt文件，然后列表显示emoji（由于功能机不能用文字显示emoji，所以读取mrp中emoji相关图片，例如：☹_32x32.png，依次类推
一次性最多读取64张emoji，因为读取过多会占用大量内存！
使用上下左右键翻页（光标使用gl_drawHollowRect绘制），按OK键放大查看emoji，（使用drawBitmapEx放大绘制）
滑动屏幕也可以滚动显示emoji，点击相应emoji放大显示（并显示返回键返回列表）
自适应屏幕宽高（使用SCRW SCRH）


以下函数定义在mrc_base.h
```c
/*
从mrp中读取文件的所有内容到申请的内存中。
使用该函数得到的内存，需要使用mrc_freeFileData函数释放。

输入:
filename     mrp文件名
lookfor          指出函数的操作形式:
                    0:    读取mrp中文件的内容
                    1:    仅仅查找mrp中是否存在该文件，并不读取
                               文件内容

输出:
*filelen         当lookfor==0时返回文件内容长度
                   当lookfor==1时未知

返回:
   当lookfor==0时
      非NULL         指向读取到的内容的指针，该内存需要
                              调用者释放
      NULL         失败
   当lookfor==1时
      1         mrp中存在该文件
      NULL         mrp中不存在该文件
*/
void * mrc_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor);

/*
释放由mrc_readFileFromMrp函数读取的文件内容空间。
输入:
data     文件内容指针
filelen          文件内容长度
*/
void mrc_freeFileData(void* data, int32 filelen);
```

以下函数定义在mrc_graphics.h
```c
/*
将bitmap上(tx,ty,tw,th)区域缩放绘制到屏幕(x,y,w,h)区域上
参数：
x 绘制到屏幕上的x坐标
y 绘制到屏幕上的y坐标
w 绘制宽度 h 绘制高度 tx ty tw th 裁剪区域
*/
void drawBitmapEx(BITMAP_565 *bmp, int32 x, int32 y, int32 w, int32 h, int32 tx, int32 ty, int32 tw, int32 th)

// 画线框
void gl_drawHollowRect(int x, int y, int width, int height, uint32 color);
```