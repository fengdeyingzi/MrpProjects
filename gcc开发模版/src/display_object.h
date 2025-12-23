/**
 * 模仿Egret显示对象API的C语言头文件
 * 设计原则：保持Egret的API结构和命名风格
 * 统一管理对象引用，统一释放
 * 所有的绘制都在舞台对象的缓存中
 * 增加Bitmap显示对象支持
 * 风的影子 编写
 */

#ifndef DISPLAY_OBJECT_H
#define DISPLAY_OBJECT_H

#include "mrc_base.h"
#include "mrc_graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 基础类型定义 ========== */
typedef int int32;         ///< 32位有符号整数
typedef unsigned int uint32; ///< 32位无符号整数
typedef int bool;

/* 点结构 */
typedef struct {
    int x;   ///< x坐标
    int y;   ///< y坐标
} Point;

/* 矩形结构 */
typedef struct {
    int x;       ///< 矩形左上角x坐标
    int y;       ///< 矩形左上角y坐标
    int width;   ///< 矩形宽度
    int height;  ///< 矩形高度
} Rectangle;

/* 矩阵结构 (Egret风格) */
typedef struct {
    double a;   ///< 缩放和旋转分量
    double b;   ///< 倾斜分量
    double c;   ///< 倾斜分量
    double d;   ///< 缩放和旋转分量
    double tx;  ///< x轴平移
    double ty;  ///< y轴平移
} Matrix;

/* 颜色结构 */
typedef struct {
    uint32 color;   ///< 颜色值(RGB)
    int alpha;   ///< 透明度(0-255)
} ColorTransform;


/* ========== 显示对象基类 ========== */
typedef struct DisplayObject DisplayObject;

/* 显示对象类型枚举 */
typedef enum {
    DISPLAY_OBJECT_TYPE_BASE = 0,    ///< 基础DisplayObject
    DISPLAY_OBJECT_TYPE_SHAPE = 1,   ///< Shape图形对象
    DISPLAY_OBJECT_TYPE_BITMAP = 2,  ///< Bitmap位图对象
    DISPLAY_OBJECT_TYPE_SPRITE = 3,  ///< Sprite容器对象
    DISPLAY_OBJECT_TYPE_STAGE = 4    ///< Stage舞台对象
} DisplayObjectType;

/* 事件回调函数指针 */
typedef void (*EventCallback)(DisplayObject* target, const char* type, void* data);

/* 渲染回调函数指针 */
typedef void (*RenderCallback)(DisplayObject* target, void* renderContext);

/* 显示对象结构 */
struct DisplayObject {
    /* 类型标识 */
    DisplayObjectType type;  ///< 对象类型标识

    /* 核心属性 (和Egret保持一致) */
    int x;           ///< x坐标
    int y;           ///< y坐标
    double scaleX;      ///< x轴缩放比例
    double scaleY;      ///< y轴缩放比例
    int rotation;    ///< 旋转角度(弧度)
    int skewX;       ///< x轴倾斜角度
    int skewY;       ///< y轴倾斜角度
    int alpha;       ///< 透明度(0.0-1.0)
    int width;       ///< 显示宽度
    int height;      ///< 显示高度
    bool visible;       ///< 是否可见
    const char* name;   ///< 对象名称

    /* 锚点属性 (用于旋转和缩放的中心点) */
    int anchorOffsetX;   ///< 锚点X偏移(相对于左上角)
    int anchorOffsetY;   ///< 锚点Y偏移(相对于左上角)
    
    /* 变换相关 */
    Matrix matrix;              ///< 本地变换矩阵
    Matrix _worldMatrix;        ///< 世界变换矩阵(缓存)
    bool _matrixDirty;          ///< 矩阵脏标记(需要重新计算)
    
    /* 显示列表相关 */
    DisplayObject* parent;          ///< 父对象指针
    DisplayObject* _firstChild;     ///< 第一个子对象
    DisplayObject* _lastChild;      ///< 最后一个子对象
    DisplayObject* _nextSibling;    ///< 下一个兄弟对象
    DisplayObject* _prevSibling;    ///< 上一个兄弟对象
    
    /* 显示属性 */
    Rectangle _bounds;              ///< 边界矩形
    Rectangle _scrollRect;          ///< 滚动矩形(用于裁剪)
    ColorTransform _colorTransform; ///< 颜色变换
    
    /* 遮罩 */
    DisplayObject* mask;            ///< 遮罩对象
    
    /* 滤镜占位 (简化版本) */
    void* _filters;                 ///< 滤镜效果数组
    
    /* 事件系统 */
    EventCallback _touchEnabledCallback; ///< 触摸事件回调
    bool touchEnabled;                   ///< 是否启用触摸
    
    /* 缓存策略 */
    bool cacheAsBitmap;              ///< 是否缓存为位图

    /* 用户数据 */
    void* userData;                  ///< 用户自定义数据指针

    /* 方法函数指针 */
    void (*setX)(DisplayObject* obj, int x);                    ///< 设置X坐标
    void (*setY)(DisplayObject* obj, int y);                    ///< 设置Y坐标
    void (*setWidth)(DisplayObject* obj, int width);            ///< 设置宽度
    void (*setHeight)(DisplayObject* obj, int height);          ///< 设置高度
    void (*setScaleX)(DisplayObject* obj, double scaleX);       ///< 设置X缩放
    void (*setScaleY)(DisplayObject* obj, double scaleY);       ///< 设置Y缩放
    void (*setScale)(DisplayObject* obj, double scale);         ///< 设置缩放
    void (*setRotation)(DisplayObject* obj, int rotation);      ///< 设置旋转角度
    void (*setAlpha)(DisplayObject* obj, int alpha);            ///< 设置透明度
    void (*setVisible)(DisplayObject* obj, bool visible);       ///< 设置可见性
    void (*setAnchorOffset)(DisplayObject* obj, int x, int y);  ///< 设置锚点偏移
};

/* ========== 图形对象 ========== */
typedef struct Shape Shape;

/* 图形样式结构 */
typedef struct {
    uint32 fillColor;       ///< 填充颜色
    uint32 strokeColor;     ///< 描边颜色
    int strokeWidth;     ///< 描边宽度
    int lineDash[10];    ///< 虚线模式数组
    uint32 lineDashCount;   ///< 虚线模式长度
} GraphicsStyle;

struct Shape {
    DisplayObject base;  ///< 继承自DisplayObject的基类成员

    /* 图形相关属性 */
    GraphicsStyle _style;        ///< 图形样式
    void* _graphicsData;         ///< 存储绘图指令的数据结构
    BITMAP_565* _cachedBitmap;   ///< 缓存的位图数据（用于带变换的渲染）
    int _cacheNeedsUpdate;       ///< 缓存是否需要更新

    /* 图形API函数指针 */
    void (*graphics)(Shape* shape);  ///< 获取图形上下文
    void (*beginFill)(Shape* shape, uint32 color, int alpha);  ///< 开始填充
    void (*endFill)(Shape* shape);   ///< 结束填充
    void (*lineStyle)(Shape* shape, int thickness, uint32 color, int alpha);  ///< 设置线条样式
    void (*moveTo)(Shape* shape, int x, int y);  ///< 移动画笔到指定位置
    void (*lineTo)(Shape* shape, int x, int y);  ///< 从当前位置画线到指定位置
    void (*drawRect)(Shape* shape, int x, int y, int width, int height);  ///< 绘制矩形
    void (*drawCircle)(Shape* shape, int x, int y, int radius);  ///< 绘制圆形
    void (*drawRoundRect)(Shape* shape, int x, int y,
                         int width, int height, int ellipseWidth, int ellipseHeight);  ///< 绘制圆角矩形
    void (*clear)(Shape* shape);  ///< 清除图形
};

/* ========== Bitmap位图对象 ========== */
typedef struct Bitmap Bitmap;

struct Bitmap {
    DisplayObject base;  ///< 继承自DisplayObject的基类成员
    
    /* Bitmap特有属性 */
    BITMAP_565* _bitmapData;     ///< 位图数据指针
    Rectangle _sourceRect;       ///< 源矩形区域(用于裁剪)
    int _pixelSnapping;       ///< 像素对齐(0=关闭,1=开启)
    bool _smoothing;             ///< 平滑处理开关
    
    /* 绘制模式 */
    int _blendMode;              ///< 混合模式
    
    /* 位图方法函数指针 */
    void (*setBitmapData)(Bitmap* bitmap, BITMAP_565* bitmapData);  ///< 设置位图数据
    void (*setSourceRect)(Bitmap* bitmap, const Rectangle* rect);   ///< 设置源矩形
    void (*setSmoothing)(Bitmap* bitmap, bool smoothing);           ///< 设置平滑处理
    void (*setPixelSnapping)(Bitmap* bitmap, int snapping);         ///< 设置像素对齐

    /* DisplayObject方法函数指针(从base继承,为方便使用也在Bitmap中添加) */
    void (*setX)(Bitmap* bitmap, int x);                    ///< 设置X坐标
    void (*setY)(Bitmap* bitmap, int y);                    ///< 设置Y坐标
    void (*setWidth)(Bitmap* bitmap, int width);            ///< 设置宽度
    void (*setHeight)(Bitmap* bitmap, int height);          ///< 设置高度
    void (*setScaleX)(Bitmap* bitmap, double scaleX);       ///< 设置X缩放
    void (*setScaleY)(Bitmap* bitmap, double scaleY);       ///< 设置Y缩放
    void (*setScale)(Bitmap* bitmap, double scale);         ///< 设置缩放
    void (*setRotation)(Bitmap* bitmap, int rotation);      ///< 设置旋转角度
    void (*setAlpha)(Bitmap* bitmap, int alpha);            ///< 设置透明度
    void (*setVisible)(Bitmap* bitmap, bool visible);       ///< 设置可见性
    void (*setAnchorOffset)(Bitmap* bitmap, int x, int y);  ///< 设置锚点偏移
};

/* ========== 容器对象 ========== */
typedef struct Sprite Sprite;

struct Sprite {
    DisplayObject base;  ///< 继承自DisplayObject的基类成员
    
    /* Sprite特有属性 */
    int _graphics;  ///< 简化的graphics引用(兼容Egret的Sprite.graphics)
    
    /* 容器方法函数指针 */
    void (*addChild)(Sprite* sprite, DisplayObject* child);  ///< 添加子对象
    void (*addChildAt)(Sprite* sprite, DisplayObject* child, int32 index);  ///< 在指定位置添加子对象
    DisplayObject* (*removeChild)(Sprite* sprite, DisplayObject* child);  ///< 移除指定子对象
    DisplayObject* (*removeChildAt)(Sprite* sprite, int32 index);  ///< 移除指定位置的子对象
    DisplayObject* (*getChildAt)(Sprite* sprite, int32 index);  ///< 获取指定位置的子对象
    int32 (*getChildIndex)(Sprite* sprite, DisplayObject* child);  ///< 获取子对象索引
    int32 (*numChildren)(Sprite* sprite);  ///< 获取子对象数量
    void (*swapChildren)(Sprite* sprite, DisplayObject* child1, DisplayObject* child2);  ///< 交换两个子对象位置
    void (*swapChildrenAt)(Sprite* sprite, int32 index1, int32 index2);  ///< 交换指定位置的子对象
};

/* ========== 舞台对象 ========== */
typedef struct Stage Stage;

struct Stage {
    DisplayObject base;  ///< 继承自DisplayObject的基类成员

    /* Stage特有属性 */
    int stageWidth;       ///< 舞台宽度
    int stageHeight;      ///< 舞台高度
    const char* scaleMode;   ///< 缩放模式(noScale/exactFit/showAll/noBorder)
    const char* align;       ///< 对齐方式(top/left/center等)
    int frameRate;        ///< 帧率

    /* 渲染上下文 */
    void* renderContext;     ///< 渲染上下文(通常为画布或显存指针)

    /* 容器方法函数指针（Stage作为根容器） */
    void (*addChild)(Stage* stage, DisplayObject* child);  ///< 添加子对象
    void (*addChildAt)(Stage* stage, DisplayObject* child, int32 index);  ///< 在指定位置添加子对象
    DisplayObject* (*removeChild)(Stage* stage, DisplayObject* child);  ///< 移除指定子对象
    DisplayObject* (*removeChildAt)(Stage* stage, int32 index);  ///< 移除指定位置的子对象
    DisplayObject* (*getChildAt)(Stage* stage, int32 index);  ///< 获取指定位置的子对象
    int32 (*getChildIndex)(Stage* stage, DisplayObject* child);  ///< 获取子对象索引
    int32 (*numChildren)(Stage* stage);  ///< 获取子对象数量
    void (*swapChildren)(Stage* stage, DisplayObject* child1, DisplayObject* child2);  ///< 交换两个子对象位置
    void (*swapChildrenAt)(Stage* stage, int32 index1, int32 index2);  ///< 交换指定位置的子对象

    /* 渲染函数指针 */
    void (*invalidate)(Stage* stage);  ///< 标记舞台需要重绘
    void (*render)(Stage* stage);      ///< 渲染舞台上的所有对象
};

/* ========== 创建函数 (Egret风格) ========== */

/**
 * @brief 创建基础显示对象
 * @return 新创建的DisplayObject指针
 */
DisplayObject* egret_DisplayObject_create(void);

/**
 * @brief 创建图形对象
 * @return 新创建的Shape指针
 */
Shape* egret_Shape_create(void);

/**
 * @brief 创建位图对象
 * @return 新创建的Bitmap指针
 */
Bitmap* egret_Bitmap_create(void);

/**
 * @brief 创建容器对象
 * @return 新创建的Sprite指针
 */
Sprite* egret_Sprite_create(void);

/**
 * @brief 创建舞台对象，舞台对象默认使用屏幕缓存
 * @return 新创建的Stage指针
 */
Stage* egret_Stage_create(void);

/**
 * @brief 统一释放函数 - 释放所有对象，但排除stage中的对象及子对象
 * @note 当每创建一个对象时，会将它的引用添加到对象管理器，以此来统一释放
 */
void egret_Clear_object(Stage* stage);

/**
 * 获取所有待清理的对象数量，排除stage中的对象
 */
int egret_Get_clearObjectCount(Stage* stage);

/**
 * 释放所有对象（包括舞台），退出程序时调用
 */
void egret_destroy();

/* ========== 显示对象基础API ========== */

/**
 * @brief 设置显示对象的变换属性
 * @param obj 目标显示对象
 * @param x x坐标
 * @param y y坐标
 * @param scaleX x轴缩放
 * @param scaleY y轴缩放
 * @param rotation 旋转角度(弧度)
 * @param skewX x轴倾斜
 * @param skewY y轴倾斜
 */
void DisplayObject_setTransform(DisplayObject* obj, int x, int y, 
                               int scaleX, int scaleY, 
                               int rotation, int skewX, int skewY);

/**
 * @brief 获取显示对象的变换矩阵
 * @param obj 目标显示对象
 * @return 变换矩阵
 */
Matrix DisplayObject_getMatrix(DisplayObject* obj);

/**
 * @brief 设置显示对象的变换矩阵
 * @param obj 目标显示对象
 * @param matrix 变换矩阵
 */
void DisplayObject_setMatrix(DisplayObject* obj, const Matrix* matrix);

/**
 * @brief 将本地坐标转换为全局坐标
 * @param obj 目标显示对象
 * @param localPoint 本地坐标点
 * @param globalPoint 输出的全局坐标点
 */
void DisplayObject_localToGlobal(DisplayObject* obj, const Point* localPoint, Point* globalPoint);

/**
 * @brief 将全局坐标转换为本地坐标
 * @param obj 目标显示对象
 * @param globalPoint 全局坐标点
 * @param localPoint 输出的本地坐标点
 */
void DisplayObject_globalToLocal(DisplayObject* obj, const Point* globalPoint, Point* localPoint);

/**
 * @brief 获取显示对象的边界矩形(不考虑变换)
 * @param obj 目标显示对象
 * @return 边界矩形
 */
Rectangle DisplayObject_getBounds(DisplayObject* obj);

/**
 * @brief 获取显示对象变换后的边界矩形
 * @param obj 目标显示对象
 * @return 变换后的边界矩形
 */
Rectangle DisplayObject_getTransformedBounds(DisplayObject* obj);

/**
 * @brief 检测点是否在显示对象内(碰撞检测)
 * @param obj 目标显示对象
 * @param x 检测点x坐标
 * @param y 检测点y坐标
 * @param isPixelPerfect 是否像素级精确检测
 * @return true:点在对象内, false:点在对象外
 */
bool DisplayObject_hitTestPoint(DisplayObject* obj, int x, int y, bool isPixelPerfect);

/* ========== 属性设置函数 ========== */

/**
 * @brief 设置显示对象的统一缩放比例
 * @param obj 目标显示对象
 * @param scale 缩放比例
 */
void DisplayObject_setScale(DisplayObject* obj, double scale);

/**
 * @brief 设置显示对象的x轴缩放比例
 * @param obj 目标显示对象
 * @param scaleX x轴缩放比例
 */
void DisplayObject_setScaleX(DisplayObject* obj, double scaleX);

/**
 * @brief 设置显示对象的y轴缩放比例
 * @param obj 目标显示对象
 * @param scaleY y轴缩放比例
 */
void DisplayObject_setScaleY(DisplayObject* obj, double scaleY);

/**
 * @brief 设置显示对象的透明度
 * @param obj 目标显示对象
 * @param alpha 透明度(0.0-1.0)
 */
void DisplayObject_setAlpha(DisplayObject* obj, int alpha);

/**
 * @brief 设置显示对象的可见性
 * @param obj 目标显示对象
 * @param visible 是否可见
 */
void DisplayObject_setVisible(DisplayObject* obj, bool visible);

/**
 * @brief 设置显示对象的名称
 * @param obj 目标显示对象
 * @param name 对象名称
 */
void DisplayObject_setName(DisplayObject* obj, const char* name);

/**
 * @brief 设置显示对象的宽度
 * @param obj 目标显示对象
 * @param width 宽度值
 */
void DisplayObject_setWidth(DisplayObject* obj, int width);

/**
 * @brief 设置显示对象的高度
 * @param obj 目标显示对象
 * @param height 高度值
 */
void DisplayObject_setHeight(DisplayObject* obj, int height);

/**
 * @brief 设置显示对象的位置
 * @param obj 目标显示对象
 * @param x x坐标
 * @param y y坐标
 */
void DisplayObject_setPosition(DisplayObject* obj, int x, int y);

/**
 * @brief 设置显示对象的旋转角度
 * @param obj 目标显示对象
 * @param rotation 旋转角度(弧度)
 */
void DisplayObject_setRotation(DisplayObject* obj, int rotation);

/**
 * @brief 设置显示对象的x坐标
 * @param obj 目标显示对象
 * @param x x坐标值
 */
void DisplayObject_setX(DisplayObject* obj, int x);

/**
 * @brief 设置显示对象的y坐标
 * @param obj 目标显示对象
 * @param y y坐标值
 */
void DisplayObject_setY(DisplayObject* obj, int y);

/**
 * @brief 设置显示对象的倾斜角度
 * @param obj 目标显示对象
 * @param skewX x轴倾斜角度
 * @param skewY y轴倾斜角度
 */
void DisplayObject_setSkew(DisplayObject* obj, int skewX, int skewY);

/**
 * @brief 设置显示对象的锚点偏移
 * @param obj 目标显示对象
 * @param x 锚点X偏移(相对于左上角)
 * @param y 锚点Y偏移(相对于左上角)
 * @note 锚点是旋转和缩放的中心点。例如设置为(width/2, height/2)可以让对象绕中心旋转
 */
void DisplayObject_setAnchorOffset(DisplayObject* obj, int x, int y);

/* ========== 属性获取函数 ========== */

/**
 * @brief 获取显示对象的x坐标
 * @param obj 目标显示对象
 * @return x坐标值
 */
int DisplayObject_getX(DisplayObject* obj);

/**
 * @brief 获取显示对象的y坐标
 * @param obj 目标显示对象
 * @return y坐标值
 */
int DisplayObject_getY(DisplayObject* obj);

/**
 * @brief 获取显示对象的宽度
 * @param obj 目标显示对象
 * @return 宽度值
 */
int DisplayObject_getWidth(DisplayObject* obj);

/**
 * @brief 获取显示对象的高度
 * @param obj 目标显示对象
 * @return 高度值
 */
int DisplayObject_getHeight(DisplayObject* obj);

/**
 * @brief 获取显示对象的x轴缩放比例
 * @param obj 目标显示对象
 * @return x轴缩放比例
 */
double DisplayObject_getScaleX(DisplayObject* obj);

/**
 * @brief 获取显示对象的y轴缩放比例
 * @param obj 目标显示对象
 * @return y轴缩放比例
 */
double DisplayObject_getScaleY(DisplayObject* obj);

/**
 * @brief 获取显示对象的旋转角度
 * @param obj 目标显示对象
 * @return 旋转角度(弧度)
 */
int DisplayObject_getRotation(DisplayObject* obj);

/**
 * @brief 获取显示对象的透明度
 * @param obj 目标显示对象
 * @return 透明度(0-255)
 */
int DisplayObject_getAlpha(DisplayObject* obj);

/**
 * @brief 获取显示对象的可见性
 * @param obj 目标显示对象
 * @return 是否可见
 */
bool DisplayObject_getVisible(DisplayObject* obj);

/**
 * @brief 获取显示对象的名称
 * @param obj 目标显示对象
 * @return 对象名称
 */
const char* DisplayObject_getName(DisplayObject* obj);

/* ========== 遮罩和滤镜 ========== */

/**
 * @brief 设置显示对象的遮罩
 * @param obj 目标显示对象
 * @param mask 遮罩对象
 */
void DisplayObject_setMask(DisplayObject* obj, DisplayObject* mask);

/**
 * @brief 设置显示对象的滤镜
 * @param obj 目标显示对象
 * @param filters 滤镜数组
 */
void DisplayObject_setFilters(DisplayObject* obj, void* filters);

/**
 * @brief 设置是否将显示对象缓存为位图
 * @param obj 目标显示对象
 * @param value true:缓存为位图, false:不缓存
 */
void DisplayObject_setCacheAsBitmap(DisplayObject* obj, bool value);

/* ========== Shape图形API ========== */

/**
 * @brief 创建图形对象
 * @return 新创建的Shape指针
 */
Shape* Shape_create(void);

/**
 * @brief 清除图形内容
 * @param shape 目标图形对象
 */
void Shape_graphics_clear(Shape* shape);

/**
 * @brief 开始填充区域
 * @param shape 目标图形对象
 * @param color 填充颜色
 * @param alpha 透明度
 */
void Shape_graphics_beginFill(Shape* shape, uint32 color, int alpha);

/**
 * @brief 结束填充区域
 * @param shape 目标图形对象
 */
void Shape_graphics_endFill(Shape* shape);

/**
 * @brief 设置线条样式
 * @param shape 目标图形对象
 * @param thickness 线条粗细
 * @param color 线条颜色
 * @param alpha 线条透明度
 */
void Shape_graphics_lineStyle(Shape* shape, int thickness, uint32 color, int alpha);

/**
 * @brief 移动画笔到指定位置
 * @param shape 目标图形对象
 * @param x x坐标
 * @param y y坐标
 */
void Shape_graphics_moveTo(Shape* shape, int x, int y);

/**
 * @brief 从当前位置画线到指定位置
 * @param shape 目标图形对象
 * @param x 目标x坐标
 * @param y 目标y坐标
 */
void Shape_graphics_lineTo(Shape* shape, int x, int y);

/**
 * @brief 绘制矩形
 * @param shape 目标图形对象
 * @param x 矩形左上角x坐标
 * @param y 矩形左上角y坐标
 * @param width 矩形宽度
 * @param height 矩形高度
 */
void Shape_graphics_drawRect(Shape* shape, int x, int y, int width, int height);

/**
 * @brief 绘制圆形
 * @param shape 目标图形对象
 * @param x 圆心x坐标
 * @param y 圆心y坐标
 * @param radius 圆半径
 */
void Shape_graphics_drawCircle(Shape* shape, int x, int y, int radius);

/**
 * @brief 绘制线
 * @param shape 目标图形对象
 * @param x1 起点x坐标
 * @param y1 起点y坐标
 * @param x2 终点x坐标
 * @param y2 终点y坐标
 * @param thickness 线条粗细
 * @param color 线条颜色
 */
void Shape_graphics_drawLine(Shape* shape, int x1, int y1, int x2, int y2, int thickness, uint32 color);

/**
 * @brief 绘制三角形
 * @param shape 目标图形对象
 * @param x1 第一个顶点x坐标
 * @param y1 第一个顶点y坐标
 * @param x2 第二个顶点x坐标
 * @param y2 第二个顶点y坐标
 * @param x3 第三个顶点x坐标
 * @param y3 第三个顶点y坐标
 * @param fillColor 填充颜色
 * @param strokeColor 描边颜色
 * @param strokeWidth 描边宽度
 */
void Shape_graphics_drawTriangle(Shape* shape, int x1, int y1, 
                                int x2, int y2, 
                                int x3, int y3, 
                                uint32 fillColor, uint32 strokeColor, int strokeWidth);

/**
 * @brief 绘制多边形
 * @param shape 目标图形对象
 * @param points 顶点数组
 * @param pointCount 顶点数量
 * @param fillColor 填充颜色
 * @param strokeColor 描边颜色
 * @param strokeWidth 描边宽度
 */
void Shape_graphics_drawPolygon(Shape* shape, const Point* points, int pointCount, 
                                uint32 fillColor, uint32 strokeColor, int strokeWidth);


/**
 * @brief 绘制圆角矩形
 * @param shape 目标图形对象
 * @param x 矩形左上角x坐标
 * @param y 矩形左上角y坐标
 * @param width 矩形宽度
 * @param height 矩形高度
 * @param ellipseWidth 圆角椭圆宽度
 * @param ellipseHeight 圆角椭圆高度
 */
void Shape_graphics_drawRoundRect(Shape* shape, int x, int y, 
                                 int width, int height, int ellipseWidth, int ellipseHeight);

/**
 * @brief 绘制椭圆
 * @param shape 目标图形对象
 * @param x 椭圆外接矩形左上角x坐标
 * @param y 椭圆外接矩形左上角y坐标
 * @param width 椭圆宽度
 * @param height 椭圆高度
 */
void Shape_graphics_drawEllipse(Shape* shape, int x, int y, int width, int height);

/* ========== Bitmap位图API ========== */

/**
 * @brief 创建位图对象
 * @return 新创建的Bitmap指针
 */
Bitmap* Bitmap_create(void);

/**
 * @brief 设置位图对象的位图数据
 * @param bitmap 目标位图对象
 * @param bitmapData BITMAP_565格式的位图数据
 */
void Bitmap_setBitmapData(Bitmap* bitmap, BITMAP_565* bitmapData);

/**
 * @brief 获取位图对象的位图数据
 * @param bitmap 目标位图对象
 * @return BITMAP_565格式的位图数据指针
 */
BITMAP_565* Bitmap_getBitmapData(Bitmap* bitmap);

/**
 * @brief 设置位图对象的源矩形区域(用于裁剪)
 * @param bitmap 目标位图对象
 * @param rect 源矩形区域
 */
void Bitmap_setSourceRect(Bitmap* bitmap, const Rectangle* rect);

/**
 * @brief 获取位图对象的源矩形区域
 * @param bitmap 目标位图对象
 * @return 源矩形区域
 */
Rectangle Bitmap_getSourceRect(Bitmap* bitmap);

/**
 * @brief 设置位图对象的平滑处理开关
 * @param bitmap 目标位图对象
 * @param smoothing true:启用平滑, false:禁用平滑
 */
void Bitmap_setSmoothing(Bitmap* bitmap, bool smoothing);

/**
 * @brief 获取位图对象的平滑处理状态
 * @param bitmap 目标位图对象
 * @return true:平滑启用, false:平滑禁用
 */
bool Bitmap_getSmoothing(Bitmap* bitmap);

/**
 * @brief 设置位图对象的像素对齐
 * @param bitmap 目标位图对象
 * @param snapping 像素对齐值(0:关闭,1:开启)
 */
void Bitmap_setPixelSnapping(Bitmap* bitmap, int snapping);

/**
 * @brief 获取位图对象的像素对齐值
 * @param bitmap 目标位图对象
 * @return 像素对齐值
 */
int Bitmap_getPixelSnapping(Bitmap* bitmap);

/**
 * @brief Bitmap的DisplayObject方法包装函数
 * @note 这些函数提供了类型安全的便捷接口，内部调用DisplayObject对应方法
 */
void Bitmap_setX(Bitmap* bitmap, int x);
void Bitmap_setY(Bitmap* bitmap, int y);
void Bitmap_setWidth(Bitmap* bitmap, int width);
void Bitmap_setHeight(Bitmap* bitmap, int height);
void Bitmap_setScaleX(Bitmap* bitmap, double scaleX);
void Bitmap_setScaleY(Bitmap* bitmap, double scaleY);
void Bitmap_setScale(Bitmap* bitmap, double scale);
void Bitmap_setRotation(Bitmap* bitmap, int rotation);
void Bitmap_setAlpha(Bitmap* bitmap, int alpha);
void Bitmap_setVisible(Bitmap* bitmap, bool visible);
void Bitmap_setAnchorOffset(Bitmap* bitmap, int x, int y);

/* ========== Sprite容器API ========== */

/**
 * @brief 创建Sprite容器对象
 * @return 新创建的Sprite指针
 */
Sprite* Sprite_create(void);

/**
 * @brief 向Sprite添加子对象
 * @param sprite 目标Sprite对象
 * @param child 要添加的子对象
 */
void Sprite_addChild(Sprite* sprite, DisplayObject* child);

/**
 * @brief 在指定位置添加子对象
 * @param sprite 目标Sprite对象
 * @param child 要添加的子对象
 * @param index 要添加的位置索引
 */
void Sprite_addChildAt(Sprite* sprite, DisplayObject* child, int32 index);

/**
 * @brief 从Sprite移除子对象
 * @param sprite 目标Sprite对象
 * @param child 要移除的子对象
 * @return 被移除的子对象指针
 */
DisplayObject* Sprite_removeChild(Sprite* sprite, DisplayObject* child);

/**
 * @brief 从指定位置移除子对象
 * @param sprite 目标Sprite对象
 * @param index 要移除的位置索引
 * @return 被移除的子对象指针
 */
DisplayObject* Sprite_removeChildAt(Sprite* sprite, int32 index);

/**
 * @brief 移除所有子对象
 * @param sprite 目标Sprite对象
 */
void Sprite_removeChildren(Sprite* sprite);

/**
 * @brief 获取指定位置的子对象
 * @param sprite 目标Sprite对象
 * @param index 位置索引
 * @return 指定位置的子对象指针
 */
DisplayObject* Sprite_getChildAt(Sprite* sprite, int32 index);

/**
 * @brief 通过名称获取子对象
 * @param sprite 目标Sprite对象
 * @param name 子对象名称
 * @return 匹配名称的子对象指针
 */
DisplayObject* Sprite_getChildByName(Sprite* sprite, const char* name);

/**
 * @brief 获取子对象的索引位置
 * @param sprite 目标Sprite对象
 * @param child 子对象
 * @return 子对象在父容器中的索引
 */
int32 Sprite_getChildIndex(Sprite* sprite, DisplayObject* child);

/**
 * @brief 设置子对象的位置索引
 * @param sprite 目标Sprite对象
 * @param child 子对象
 * @param index 新的索引位置
 */
void Sprite_setChildIndex(Sprite* sprite, DisplayObject* child, int32 index);

/**
 * @brief 获取子对象数量
 * @param sprite 目标Sprite对象
 * @return 子对象数量
 */
int32 Sprite_numChildren(Sprite* sprite);

/**
 * @brief 交换两个子对象的位置
 * @param sprite 目标Sprite对象
 * @param child1 第一个子对象
 * @param child2 第二个子对象
 */
void Sprite_swapChildren(Sprite* sprite, DisplayObject* child1, DisplayObject* child2);

/**
 * @brief 交换指定位置的两个子对象
 * @param sprite 目标Sprite对象
 * @param index1 第一个位置索引
 * @param index2 第二个位置索引
 */
void Sprite_swapChildrenAt(Sprite* sprite, int32 index1, int32 index2);

/**
 * @brief 检查是否包含指定子对象
 * @param sprite 目标Sprite对象
 * @param child 要检查的子对象
 * @return true:包含, false:不包含
 */
bool Sprite_contains(Sprite* sprite, DisplayObject* child);

/**
 * @brief Sprite开始填充(兼容Egret API)
 * @param sprite 目标Sprite对象
 * @param color 填充颜色
 * @param alpha 透明度
 */
void Sprite_graphics_beginFill(Sprite* sprite, uint32 color, int alpha);

/**
 * @brief Sprite绘制矩形(兼容Egret API)
 * @param sprite 目标Sprite对象
 * @param x 矩形左上角x坐标
 * @param y 矩形左上角y坐标
 * @param width 矩形宽度
 * @param height 矩形高度
 */
void Sprite_graphics_drawRect(Sprite* sprite, int x, int y, int width, int height);

/**
 * @brief Sprite绘制圆形(兼容Egret API)
 * @param sprite 目标Sprite对象
 * @param x 圆心x坐标
 * @param y 圆心y坐标
 * @param radius 圆半径
 */
void Sprite_graphics_drawCircle(Sprite* sprite, int x, int y, int radius);

/**
 * @brief Sprite结束填充(兼容Egret API)
 * @param sprite 目标Sprite对象
 */
void Sprite_graphics_endFill(Sprite* sprite);

/* ========== Stage舞台API ========== */

/**
 * @brief 创建舞台对象
 * @param buffer 舞台像素缓冲区
 * @param width 舞台宽度
 * @param height 舞台高度
 * @return 新创建的Stage指针
 */
Stage* Stage_create(uint16* buffer, int width, int height);

/**
 * @brief 设置舞台尺寸
 * @param stage 目标Stage对象
 * @param width 舞台宽度
 * @param height 舞台高度
 */
void Stage_setSize(Stage* stage, int width, int height);

/**
 * @brief 向Stage添加子对象
 * @param stage 目标Stage对象
 * @param child 要添加的子对象
 */
void Stage_addChild(Stage* stage, DisplayObject* child);

/**
 * @brief 在指定位置添加子对象
 * @param stage 目标Stage对象
 * @param child 要添加的子对象
 * @param index 要添加的位置索引
 */
void Stage_addChildAt(Stage* stage, DisplayObject* child, int32 index);

/**
 * @brief 从Stage移除子对象
 * @param stage 目标Stage对象
 * @param child 要移除的子对象
 * @return 被移除的子对象指针
 */
DisplayObject* Stage_removeChild(Stage* stage, DisplayObject* child);

/**
 * @brief 从指定位置移除子对象
 * @param stage 目标Stage对象
 * @param index 要移除的位置索引
 * @return 被移除的子对象指针
 */
DisplayObject* Stage_removeChildAt(Stage* stage, int32 index);

/**
 * @brief 移除所有子对象
 * @param stage 目标Stage对象
 */
void Stage_removeChildren(Stage* stage);

/**
 * @brief 获取指定位置的子对象
 * @param stage 目标Stage对象
 * @param index 位置索引
 * @return 指定位置的子对象指针
 */
DisplayObject* Stage_getChildAt(Stage* stage, int32 index);

/**
 * @brief 通过名称获取子对象
 * @param stage 目标Stage对象
 * @param name 子对象名称
 * @return 匹配名称的子对象指针
 */
DisplayObject* Stage_getChildByName(Stage* stage, const char* name);

/**
 * @brief 获取子对象的索引位置
 * @param stage 目标Stage对象
 * @param child 子对象
 * @return 子对象在舞台中的索引
 */
int32 Stage_getChildIndex(Stage* stage, DisplayObject* child);

/**
 * @brief 设置子对象的位置索引
 * @param stage 目标Stage对象
 * @param child 子对象
 * @param index 新的索引位置
 */
void Stage_setChildIndex(Stage* stage, DisplayObject* child, int32 index);

/**
 * @brief 获取子对象数量
 * @param stage 目标Stage对象
 * @return 子对象数量
 */
int32 Stage_numChildren(Stage* stage);

/**
 * @brief 交换两个子对象的位置
 * @param stage 目标Stage对象
 * @param child1 第一个子对象
 * @param child2 第二个子对象
 */
void Stage_swapChildren(Stage* stage, DisplayObject* child1, DisplayObject* child2);

/**
 * @brief 交换指定位置的两个子对象
 * @param stage 目标Stage对象
 * @param index1 第一个位置索引
 * @param index2 第二个位置索引
 */
void Stage_swapChildrenAt(Stage* stage, int32 index1, int32 index2);

/**
 * @brief 检查是否包含指定子对象
 * @param stage 目标Stage对象
 * @param child 要检查的子对象
 * @return true:包含, false:不包含
 */
bool Stage_contains(Stage* stage, DisplayObject* child);

/**
 * @brief 渲染舞台上的所有对象
 * @param stage 目标Stage对象
 */
void Stage_render(Stage* stage);

/* ========== 显示对象树遍历 ========== */

/* 遍历回调 */
typedef void (*DisplayObjectCallback)(DisplayObject* obj, void* userData);

/**
 * @brief 遍历显示对象树(深度优先)
 * @param obj 起始显示对象
 * @param callback 遍历回调函数
 * @param userData 用户数据
 */
void DisplayObject_traverse(DisplayObject* obj, DisplayObjectCallback callback, void* userData);

/**
 * @brief 遍历可见的显示对象树
 * @param obj 起始显示对象
 * @param callback 遍历回调函数
 * @param userData 用户数据
 */
void DisplayObject_traverseVisible(DisplayObject* obj, DisplayObjectCallback callback, void* userData);

/**
 * @brief 遍历子对象
 * @param obj 父显示对象
 * @param callback 遍历回调函数
 * @param userData 用户数据
 * @param reverse 是否反向遍历
 */
void DisplayObject_traverseChildren(DisplayObject* obj, DisplayObjectCallback callback, void* userData, bool reverse);

/* ========== 工具函数 ========== */

/**
 * @brief 创建变换矩阵
 * @param a 缩放和旋转分量
 * @param b 倾斜分量
 * @param c 倾斜分量
 * @param d 缩放和旋转分量
 * @param tx x轴平移
 * @param ty y轴平移
 * @return 创建的矩阵
 */
Matrix Matrix_create(int a, int b, int c, int d, int tx, int ty);

/**
 * @brief 创建单位矩阵
 * @return 单位矩阵
 */
Matrix Matrix_identity(void);

/**
 * @brief 矩阵前置乘法(m1 = m1 × m2)
 * @param m1 第一个矩阵(结果矩阵)
 * @param m2 第二个矩阵
 */
void Matrix_preMultiply(Matrix* m1, const Matrix* m2);

/**
 * @brief 矩阵求逆
 * @param matrix 要求逆的矩阵
 */
void Matrix_invert(Matrix* matrix);

/**
 * @brief 矩阵变换点
 * @param matrix 变换矩阵
 * @param x 点x坐标
 * @param y 点y坐标
 * @return 变换后的点
 */
Point Matrix_transformPoint(const Matrix* matrix, int x, int y);

/**
 * @brief 创建矩形
 * @param x 左上角x坐标
 * @param y 左上角y坐标
 * @param width 宽度
 * @param height 高度
 * @return 创建的矩形
 */
Rectangle Rectangle_create(int x, int y, int width, int height);

/**
 * @brief 合并两个矩形(并集)
 * @param rect1 第一个矩形(结果矩形)
 * @param rect2 第二个矩形
 */
void Rectangle_union(Rectangle* rect1, const Rectangle* rect2);

/**
 * @brief 检查点是否在矩形内
 * @param rect 矩形
 * @param x 点x坐标
 * @param y 点y坐标
 * @return true:点在矩形内, false:点在矩形外
 */
bool Rectangle_contains(Rectangle* rect, int x, int y);

/**
 * @brief 检查两个矩形是否相交
 * @param rect1 第一个矩形
 * @param rect2 第二个矩形
 * @return true:相交, false:不相交
 */
bool Rectangle_intersects(Rectangle* rect1, const Rectangle* rect2);

/**
 * @brief 创建点
 * @param x x坐标
 * @param y y坐标
 * @return 创建的点
 */
Point Point_create(int x, int y);

/**
 * @brief 计算两点间距离
 * @param p1 第一个点
 * @param p2 第二个点
 * @return 两点间距离
 */
int Point_distance(const Point* p1, const Point* p2);

/* ========== 事件系统占位 ========== */
/**
 * @brief 添加事件监听器
 * @param obj 目标显示对象
 * @param type 事件类型
 * @param listener 事件监听函数
 * @param thisObject this对象
 * @param useCapture 是否使用捕获阶段
 * @param priority 事件优先级
 */
void DisplayObject_addEventListener(DisplayObject* obj, const char* type, 
                                   void (*listener)(DisplayObject*, const char*, void*), 
                                   void* thisObject, bool useCapture, int32 priority);

/**
 * @brief 移除事件监听器
 * @param obj 目标显示对象
 * @param type 事件类型
 * @param listener 事件监听函数
 * @param thisObject this对象
 * @param useCapture 是否使用捕获阶段
 */
void DisplayObject_removeEventListener(DisplayObject* obj, const char* type, 
                                      void (*listener)(DisplayObject*, const char*, void*), 
                                      void* thisObject, bool useCapture);

/**
 * @brief 派发事件
 * @param obj 目标显示对象
 * @param type 事件类型
 * @param data 事件数据
 */
void DisplayObject_dispatchEvent(DisplayObject* obj, const char* type, void* data);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_OBJECT_H */