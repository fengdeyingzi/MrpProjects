//必要的头文件
#include "haning.h"
//需要入库的头文件
#include "mrc_camera.h "

//////////////////////////////////////////////////////////////////////////

// 函数定义开始
void Lib_Camera_init(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger = mrc_Camera_init();
}

void Lib_Camera_creat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger = mrc_Camera_creat(Param[0]->Val->ShortInteger,Param[1]->Val->ShortInteger,Param[2]->Val->ShortInteger,Param[3]->Val->ShortInteger,Param[4]->Val->ShortInteger,Param[5]->Val->ShortInteger,Param[6]->Val->UnsignedLongInteger);
}

void Lib_Camera_capture(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger = mrc_Camera_capture(Param[0]->Val->Character,(CAMERA_IMG_DATA_T*)Param[4]->Val->Pointer);
}

void Lib_Camera_destroy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	 mrc_Camera_destroy();
}

void Lib_Camera_pause(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger = mrc_Camera_pause();
}

void Lib_Camera_resume(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger = mrc_Camera_resume();
}

void Lib_Camera_getZoom(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger = mrc_Camera_getZoom();
}

void Lib_Camera_zoomOut(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger = mrc_Camera_zoomOut();
}

void Lib_Camera_zoomIn(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->LongInteger = mrc_Camera_zoomIn();
}

void Lib_Camera_status(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
	ReturnValue->Val->Character = mrc_Camera_status();
}
///函数定义结束

////////////////////////////需要入库的源码，参照mpc书写规范。包括此头文件需要的结构体，枚举，宏定义等
const  char mrc_acmera_Defs[] = "\
typedef enum\
{\
STATUS_UNINIT,\
STATUS_INIT,\
STATUS_INIT_FAILED,\
STATUS_START,\
STATUS_START_FAILED,\
STATUS_CAPTURE,\
STATUS_CAPTURE_FAILED,\
STATUS_PAUSE,\
STATUS_RESUME_FAILED,\
STATUS_CLOSE\
}CAMERA_STATUS_T;\n\
typedef enum\
{\
CAPTURE_MODE_MULTIPLE = 1,\
CAPTURE_MODE_SINGLE = 2\
}CAMERA_CAPTURE_MODE_T;\n\
typedef enum\
{\
IMG_QUALITY_LOW,\
IMG_QUALITY_NORMAL,\
IMG_QUALITY_HIGH,\
IMG_QUALITY_T};\n\
typedef struct\
{\
void* img_data;\
int32 img_data_len;\
}IMG_DATA_T;";


//封装函数总个数+1
#define MAX_ACMERA_COUNT 10+1

//申请MAX_ACMERA_COUNT个包含函数信息的结构
struct LibraryFunction mrc_Acmera_Functions[MAX_ACMERA_COUNT];

//初始化
int32 mrc_init()
{
	//作为返回值，插件函数库句柄
	int32 RE;
	//注册此库插件
	//函数原型int32 Haninit(int big,struct LibraryFunction *han,const char *Defs)
	//big  封装函数总个数
	//han  函数信息的结构
	//Defs 包含源码
	RE=Haninit(MAX_ACMERA_COUNT,&mrc_Acmera_Functions[0],&mrc_acmera_Defs[0]);


	//函数原型void AddString(CFuncType func, const char *proto_type)
	//func        之前定义的库函数
	//proto_type  对应的mpc函数原型
	AddString(Lib_Camera_init,"int32 camera_init();");
	AddString(Lib_Camera_creat,"int32 camera_creat(uint16,uint16,uint16,uint16,uint16,uint16,uint32);");
	AddString(Lib_Camera_capture,"int32 camera_capture(uint8,IMG_DATA_T*);");
	AddString(Lib_Camera_destroy,"void camera_destroy();");
	AddString(Lib_Camera_pause,"int32 camera_pause();");
	AddString(Lib_Camera_resume,"int32 camera_resume();");
	AddString(Lib_Camera_getZoom,"int32 camera_getzoom();");
	AddString(Lib_Camera_zoomOut,"int32 camera_zoomout();");
	AddString(Lib_Camera_zoomIn,"int32 camera_zoomin();");
	AddString(Lib_Camera_status,"uint8 camera_status();");

	//NULL终止
	AddString(NULL,NULL);

	//返回句柄
	return RE;

}


//不需要事件，后面都不需要，但必须要写，否则编译出错
int32 mrc_appEvent(int32 code, int32 param0, int32 param1)

{
	return MR_IGNORE;
}



int32 mrc_pause()

{
	return MR_IGNORE;
}



int32 mrc_resume()

{
	return (int32)NULL;
}



int32 mrc_exitApp()

{
	return MR_SUCCESS;
}
