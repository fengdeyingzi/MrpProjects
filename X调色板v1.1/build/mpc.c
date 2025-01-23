//加载系统头文件base.h
#include <base.h>
#include <graphics.h>
#include <android.h>
#include <android.h>

/*
头文件：graphics.h
从安装包assets目录读取bitmap
并绘制到屏幕上
*/

void helloworld() {
	//用指定颜色清除屏幕
	cls(0, 0, 0);

	int iy = 10;
	dtext("手机CAPP运行器", 10, iy += 26, 240, 240, 240, 0, 1);
	dtext("兼容冒泡开发实验系统", 10, iy += 26, 240, 240, 240, 0, 1);
	dtext("运行mrp内mpc.c文件", 10, iy += 26, 240, 240, 240, 0, 1);
	dtext("支持从mrp包中读取", 10, iy += 26, 240, 240, 240, 0, 1);
	dtext("支持graphics.h", 10, iy += 26, 240, 240, 240, 0, 1);
	dtext("支持net.h", 10, iy += 26, 240, 240, 240, 0, 1);
	dtext("支持exb.h", 10, iy += 26, 240, 240, 240, 0, 1);
	dtext("支持readFileFromAssets函数", 10, iy += 26, 240, 240, 240, 0, 1);
	dtext("按返回或点击屏幕退出", 10, iy += 26, 240, 240, 240, 0, 1);

	//刷新屏幕
	ref(0, 0, SCRW, SCRH);
}

//入口函数，程序启动时开始执行
int init() {
	//调用函数helloworld
	helloworld();
	return 0;
}

//event函数，接收消息事件
int event(int type, int p1, int p2) {
	if (KY_UP == type) {
		switch (p1) {
		case _BACK:
			exit();
			break;
		case _MENU:
			exit();
			break;
		case 1:
		int32 len = 0;
		    int32 bitmap = readBitmapFromAssets("1.png");
			drawBitmap(bitmap, 0,0);
			bitmapFree(bitmap);
			ref(0,0,SCRW,SCRH);
			break;
		case 2:
		    toast("测试",0);
		}
		
	} else if (MS_UP == type) {
        exit();
	}

	return 0;
}

//应用暂停，当程序进入后台或变为不可见时会调用此函数
int pause() {
	return 0;
}

//应用恢复，程序变为可见时调用此函数
int resume() {
	return 0;
}
