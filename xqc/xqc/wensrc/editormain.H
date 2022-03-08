#ifndef _EDITORMAIN_H
#define _EDITORMAIN_H


///////////////////////////////////////
enum Windows{
	EDITOR,//编辑器
	MAINMENU,//主菜单
	FILEVIEW,//文件浏览
	SETING//设置
};

int32 memdir(const char* dst,const char* dirName);


#define mallocchar(text) strcpy(malloc(strlen(text)+1),text)
#define Malloc(len) mrc_memset(mrc_malloc(len),0,len)
//内置颜色初始化
int32 ColInit(void);
//打开文件调用，文件夹和文件名合成到打开的文件名
void EditFile(char *path,char *name);
//保存文档
int SaveFile(void);
//将mrp内文件直接写入文件
int fileaz(char *mrpfile,char *file);
//包含的文件夹
int az(void);
//创建编辑框返回句柄
int32 MyCreate(char *title,char *text,int type,int len);
//运行mrp
int32 runmrps(char *name);
//临时保存
void Debugging(void);
//保存设置
int IniSetSave(void);

void editorinit(void);


#endif
