#ifndef _LOGO_H
#define _LOGO_H
//显示
//-1 恢复屏幕显示
//0  加载logo并开始
//此函数归事件函数调用
void logo_xian(int32 data);
//暂停
void logo_pause();
//恢复
void logo_resume();
//初始化
void logo_init();
//释放
void logo_free();
//事件通知
void logo_event();


#endif