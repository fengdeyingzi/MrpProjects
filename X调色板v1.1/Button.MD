这是一个自定义的按钮控件，支持设置点击事件。(源码见附件)

使用方法：

    //声明按钮结构体
    button *btn_ok;

    //创建按钮
    btn_ok = btn_create("测试1",20,20,720-40,120-40);
    
    //设置按钮id
    btn_setid(btn_ok,1);
    
    //设置按钮点击事件
    btn_setonclick(btn_ok, "click_ok");
    
    //绘制按钮
    btn_draw(btn_ok);
    //刷新屏幕
    ref(0,0,SCRW,SCRH);
    
** 按钮需要监听event事件，所以在event函数中添加btn_event **

    //监听event事件
    btn_event(btn_ok,type,p1,p2);
    
    