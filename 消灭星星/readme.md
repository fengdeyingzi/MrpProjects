# **mrp_core项目简介**
mrp游戏开发模版，实现Window, Sprite, Object, Map等功能，加快mrp游戏开发


## **Window系统**
mrp的SGL库过于复杂，采用更加简便的win系统，斯凯SDK：mrc_win.c
暂时未进行封装，做了几个window模版可供参考

## **Sprite精灵**
计划实现两种方案：mrp自带Sprite精灵和动作编辑器SpriteX精灵

### **斯凯SDK的Sprite**
斯凯sdk内包含了简单的Sprite绘制功能，会自动切图，但是多帧精灵bmp图片需要是竖向的图，对Sprite进行了简单的封装

``` 
//精灵
typedef struct sprite
{
    int32 bmp_id; //对应bmpID
    int32 width;
    int32 height;
    int32 image_number;
    int32 start; //可以一个图片多个动作, 默认是0

    int32 posx; //中心点相对于图片位置，用来升级绘制功能
    int32 posy;
    int32 bmmode;

    Bbox box; //相对于posx和posy的矩形
} Sprite; 
```

封装后的Sprite支持多帧绘制，多个动作，绑定碰撞盒等优点

### **动作编辑器的SpriteX**
整体功能制作中，因为需要联动Object等模块，进展缓慢  

## **Object物体**
绑定Sprite，运行逻辑规则等，目前已实现简单功能

## **map地图**
地图功能包含了绘制和碰撞检测，自动寻路等功能
目前实现mape文件的读取显示，计划实现tmx地图的读取绘制

## **其他模块**
本项目中用到或实现的其他功能

### **TPNG模块**
png图片解析


