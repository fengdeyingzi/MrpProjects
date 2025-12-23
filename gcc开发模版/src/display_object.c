/**
 * 模仿Egret显示对象API的C语言实现
 * 实现显示对象树、变换、渲染等功能
 * 风的影子 编写
 */

#include "display_object.h"
#include "mrc_base.h"
#include "mrc_graphics.h"
#include "ex_math.h"

/* ========== 内部绘图辅助函数 ========== */

/**
 * RGB565颜色混合
 */
static uint16 blendColorRGB565(uint16 dstColor, uint16 srcColor, int alpha) {
    uint8 r1, g1, b1, r2, g2, b2, r, g, b;

    if (alpha >= 255) return srcColor;
    if (alpha <= 0) return dstColor;

    r1 = (dstColor >> 11) & 0x1F;
    g1 = (dstColor >> 5) & 0x3F;
    b1 = dstColor & 0x1F;

    r2 = (srcColor >> 11) & 0x1F;
    g2 = (srcColor >> 5) & 0x3F;
    b2 = srcColor & 0x1F;

    r = (r1 * (255 - alpha) + r2 * alpha) / 255;
    g = (g1 * (255 - alpha) + g2 * alpha) / 255;
    b = (b1 * (255 - alpha) + b2 * alpha) / 255;

    return (r << 11) | (g << 5) | b;
}

/* ========== 全局对象管理器 ========== */
#define MAX_OBJECTS 1000

typedef struct {
    DisplayObject* objects[MAX_OBJECTS]; // 存储所有创建的对象指针
    int count; // 当前对象数量
    Stage* stage;
} ObjectManager;

static ObjectManager g_objectManager = {0};

/* ========== 内部辅助函数 ========== */

/**
 * 注册对象到管理器
 */
static void registerObject(DisplayObject* obj) {
    int i;
    if (!obj || g_objectManager.count >= MAX_OBJECTS) {
        return;
    }

    for (i = 0; i < g_objectManager.count; i++) {
        if (g_objectManager.objects[i] == obj) {
            return;
        }
    }

    g_objectManager.objects[g_objectManager.count++] = obj;
}

/**
 * 从管理器中注销对象
 */
static void unregisterObject(DisplayObject* obj) {
    int i, j;
    if (!obj) return;

    for (i = 0; i < g_objectManager.count; i++) {
        if (g_objectManager.objects[i] == obj) {
            for (j = i; j < g_objectManager.count - 1; j++) {
                g_objectManager.objects[j] = g_objectManager.objects[j + 1];
            }
            g_objectManager.count--;
            return;
        }
    }
}

/**
 * 检查对象是否在舞台树中
 */
static int isInStageTree(DisplayObject* obj, Stage* stage) {
    DisplayObject* current;
    if (!obj || !stage) return 0;

    current = obj;
    while (current) {
        if (current == (DisplayObject*)stage) {
            return 1;
        }
        current = current->parent;
    }
    return 0;
}

/**
 * 标记对象的矩阵为脏
 */
static void markMatrixDirty(DisplayObject* obj) {
    DisplayObject* child;
    if (!obj) return;

    obj->_matrixDirty = 1;

    child = obj->_firstChild;
    while (child) {
        markMatrixDirty(child);
        child = child->_nextSibling;
    }
}

/**
 * 更新世界变换矩阵
 */
static void updateWorldMatrix(DisplayObject* obj) {
    Matrix localMatrix;
    int cosSkewY, sinSkewY, cosSkewX, sinSkewX;
    int anchorX, anchorY;
    int angleY, angleX;
    double transformedAnchorX, transformedAnchorY;

    if (!obj) return;

    if (!obj->_matrixDirty && !obj->parent) {
        return;
    }

    /* 计算旋转+倾斜的角度，确保在0-359范围内 */
    angleY = (obj->rotation + obj->skewY) % 360;
    if (angleY < 0) angleY += 360;

    angleX = (obj->rotation - obj->skewX) % 360;
    if (angleX < 0) angleX += 360;

    /* 计算三角函数值 */
    cosSkewY = INT_COS(angleY);
    sinSkewY = INT_SIN(angleY);
    cosSkewX = INT_COS(angleX);
    sinSkewX = INT_SIN(angleX);

    /* 构建包含旋转、缩放和倾斜的完整2D变换矩阵 */
    localMatrix.a = (obj->scaleX * cosSkewY) / TRIG_SCALE;
    localMatrix.b = (obj->scaleX * sinSkewY) / TRIG_SCALE;
    localMatrix.c = -(obj->scaleY * sinSkewX) / TRIG_SCALE;
    localMatrix.d = (obj->scaleY * cosSkewX) / TRIG_SCALE;

    /* 应用锚点变换：
     * 1. 先将锚点通过旋转和缩放矩阵变换
     * 2. 位移 = 对象位置 + 锚点原始位置 - 变换后的锚点位置
     * 这样可以确保旋转和缩放围绕锚点进行
     */
    anchorX = obj->anchorOffsetX;
    anchorY = obj->anchorOffsetY;

    transformedAnchorX = anchorX * localMatrix.a + anchorY * localMatrix.c;
    transformedAnchorY = anchorX * localMatrix.b + anchorY * localMatrix.d;

    localMatrix.tx = obj->x + anchorX - transformedAnchorX;
    localMatrix.ty = obj->y + anchorY - transformedAnchorY;

    if (obj->parent) {
        Matrix parentMatrix;
        updateWorldMatrix(obj->parent);
        parentMatrix = obj->parent->_worldMatrix;

        obj->_worldMatrix.a = localMatrix.a * parentMatrix.a + localMatrix.b * parentMatrix.c;
        obj->_worldMatrix.b = localMatrix.a * parentMatrix.b + localMatrix.b * parentMatrix.d;
        obj->_worldMatrix.c = localMatrix.c * parentMatrix.a + localMatrix.d * parentMatrix.c;
        obj->_worldMatrix.d = localMatrix.c * parentMatrix.b + localMatrix.d * parentMatrix.d;
        obj->_worldMatrix.tx = localMatrix.tx * parentMatrix.a + localMatrix.ty * parentMatrix.c + parentMatrix.tx;
        obj->_worldMatrix.ty = localMatrix.tx * parentMatrix.b + localMatrix.ty * parentMatrix.d + parentMatrix.ty;
    } else {
        obj->_worldMatrix = localMatrix;
    }

    obj->_matrixDirty = 0;
}

/* ========== 矩阵工具函数 ========== */

Matrix Matrix_create(int a, int b, int c, int d, int tx, int ty) {
    Matrix m;
    m.a = a;
    m.b = b;
    m.c = c;
    m.d = d;
    m.tx = tx;
    m.ty = ty;
    return m;
}

Matrix Matrix_identity(void) {
    return Matrix_create(1, 0, 0, 1, 0, 0);
}

void Matrix_preMultiply(Matrix* m1, const Matrix* m2) {
    Matrix result;
    if (!m1 || !m2) return;

    result.a = m1->a * m2->a + m1->b * m2->c;
    result.b = m1->a * m2->b + m1->b * m2->d;
    result.c = m1->c * m2->a + m1->d * m2->c;
    result.d = m1->c * m2->b + m1->d * m2->d;
    result.tx = m1->tx * m2->a + m1->ty * m2->c + m2->tx;
    result.ty = m1->tx * m2->b + m1->ty * m2->d + m2->ty;

    *m1 = result;
}

void Matrix_invert(Matrix* matrix) {
    double det;
    Matrix inv;

    if (!matrix) return;

    det = matrix->a * matrix->d - matrix->b * matrix->c;
    if (det == 0) return;

    inv.a = matrix->d / det;
    inv.b = -matrix->b / det;
    inv.c = -matrix->c / det;
    inv.d = matrix->a / det;
    inv.tx = (matrix->c * matrix->ty - matrix->d * matrix->tx) / det;
    inv.ty = (matrix->b * matrix->tx - matrix->a * matrix->ty) / det;

    *matrix = inv;
}

Point Matrix_transformPoint(const Matrix* matrix, int x, int y) {
    Point p;
    if (!matrix) {
        p.x = x;
        p.y = y;
        return p;
    }

    p.x = (int)(x * matrix->a + y * matrix->c + matrix->tx);
    p.y = (int)(x * matrix->b + y * matrix->d + matrix->ty);
    return p;
}

/* ========== 矩形工具函数 ========== */

Rectangle Rectangle_create(int x, int y, int width, int height) {
    Rectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    return rect;
}

void Rectangle_union(Rectangle* rect1, const Rectangle* rect2) {
    int right1, bottom1, right2, bottom2;
    int newX, newY, newRight, newBottom;

    if (!rect1 || !rect2) return;

    right1 = rect1->x + rect1->width;
    bottom1 = rect1->y + rect1->height;
    right2 = rect2->x + rect2->width;
    bottom2 = rect2->y + rect2->height;

    newX = (rect1->x < rect2->x) ? rect1->x : rect2->x;
    newY = (rect1->y < rect2->y) ? rect1->y : rect2->y;
    newRight = (right1 > right2) ? right1 : right2;
    newBottom = (bottom1 > bottom2) ? bottom1 : bottom2;

    rect1->x = newX;
    rect1->y = newY;
    rect1->width = newRight - newX;
    rect1->height = newBottom - newY;
}

bool Rectangle_contains(Rectangle* rect, int x, int y) {
    if (!rect) return 0;
    return (x >= rect->x && x < rect->x + rect->width &&
            y >= rect->y && y < rect->y + rect->height);
}

bool Rectangle_intersects(Rectangle* rect1, const Rectangle* rect2) {
    if (!rect1 || !rect2) return 0;

    return !(rect1->x >= rect2->x + rect2->width ||
             rect1->x + rect1->width <= rect2->x ||
             rect1->y >= rect2->y + rect2->height ||
             rect1->y + rect1->height <= rect2->y);
}

/* ========== 点工具函数 ========== */

Point Point_create(int x, int y) {
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

int Point_distance(const Point* p1, const Point* p2) {
    int dx, dy;
    if (!p1 || !p2) return 0;

    dx = p2->x - p1->x;
    dy = p2->y - p1->y;
    return int_sqrt(dx * dx + dy * dy);
}

/* ========== DisplayObject基础实现 ========== */

DisplayObject* egret_DisplayObject_create(void) {
    DisplayObject* obj;

    obj = (DisplayObject*)mrc_malloc(sizeof(DisplayObject));
    if (!obj) return NULL;

    mrc_memset(obj, 0, sizeof(DisplayObject));

    obj->type = DISPLAY_OBJECT_TYPE_BASE;
    obj->x = 0;
    obj->y = 0;
    obj->scaleX = 1.0;
    obj->scaleY = 1.0;
    obj->rotation = 0;
    obj->alpha = 255;
    obj->visible = 1;
    obj->anchorOffsetX = 0;
    obj->anchorOffsetY = 0;
    obj->matrix = Matrix_identity();
    obj->_worldMatrix = Matrix_identity();
    obj->_matrixDirty = 1;
    obj->touchEnabled = 0;
    obj->cacheAsBitmap = 0;

    /* 初始化方法函数指针 */
    obj->setX = DisplayObject_setX;
    obj->setY = DisplayObject_setY;
    obj->setWidth = DisplayObject_setWidth;
    obj->setHeight = DisplayObject_setHeight;
    obj->setScaleX = DisplayObject_setScaleX;
    obj->setScaleY = DisplayObject_setScaleY;
    obj->setScale = DisplayObject_setScale;
    obj->setRotation = DisplayObject_setRotation;
    obj->setAlpha = DisplayObject_setAlpha;
    obj->setVisible = DisplayObject_setVisible;
    obj->setAnchorOffset = DisplayObject_setAnchorOffset;

    registerObject(obj);

    return obj;
}

void DisplayObject_setTransform(DisplayObject* obj, int x, int y,
                               int scaleX, int scaleY,
                               int rotation, int skewX, int skewY) {
    if (!obj) return;

    obj->x = x;
    obj->y = y;
    obj->scaleX = scaleX;
    obj->scaleY = scaleY;
    obj->rotation = rotation;
    obj->skewX = skewX;
    obj->skewY = skewY;

    markMatrixDirty(obj);
}

Matrix DisplayObject_getMatrix(DisplayObject* obj) {
    if (!obj) return Matrix_identity();

    updateWorldMatrix(obj);
    return obj->_worldMatrix;
}

void DisplayObject_setMatrix(DisplayObject* obj, const Matrix* matrix) {
    if (!obj || !matrix) return;

    obj->matrix = *matrix;
    markMatrixDirty(obj);
}

void DisplayObject_localToGlobal(DisplayObject* obj, const Point* localPoint, Point* globalPoint) {
    Matrix worldMatrix;

    if (!obj || !localPoint || !globalPoint) return;

    worldMatrix = DisplayObject_getMatrix(obj);
    *globalPoint = Matrix_transformPoint(&worldMatrix, localPoint->x, localPoint->y);
}

void DisplayObject_globalToLocal(DisplayObject* obj, const Point* globalPoint, Point* localPoint) {
    Matrix worldMatrix;

    if (!obj || !globalPoint || !localPoint) return;

    worldMatrix = DisplayObject_getMatrix(obj);
    Matrix_invert(&worldMatrix);
    *localPoint = Matrix_transformPoint(&worldMatrix, globalPoint->x, globalPoint->y);
}

Rectangle DisplayObject_getBounds(DisplayObject* obj) {
    if (!obj) return Rectangle_create(0, 0, 0, 0);
    return obj->_bounds;
}

Rectangle DisplayObject_getTransformedBounds(DisplayObject* obj) {
    Rectangle bounds;
    Point corners[4];
    Point transformed[4];
    int i;
    int minX, minY, maxX, maxY;
    Matrix worldMatrix;

    if (!obj) return Rectangle_create(0, 0, 0, 0);

    bounds = obj->_bounds;

    corners[0] = Point_create(bounds.x, bounds.y);
    corners[1] = Point_create(bounds.x + bounds.width, bounds.y);
    corners[2] = Point_create(bounds.x + bounds.width, bounds.y + bounds.height);
    corners[3] = Point_create(bounds.x, bounds.y + bounds.height);

    worldMatrix = DisplayObject_getMatrix(obj);

    for (i = 0; i < 4; i++) {
        transformed[i] = Matrix_transformPoint(&worldMatrix, corners[i].x, corners[i].y);
    }

    minX = maxX = transformed[0].x;
    minY = maxY = transformed[0].y;

    for (i = 1; i < 4; i++) {
        if (transformed[i].x < minX) minX = transformed[i].x;
        if (transformed[i].x > maxX) maxX = transformed[i].x;
        if (transformed[i].y < minY) minY = transformed[i].y;
        if (transformed[i].y > maxY) maxY = transformed[i].y;
    }

    return Rectangle_create(minX, minY, maxX - minX, maxY - minY);
}

bool DisplayObject_hitTestPoint(DisplayObject* obj, int x, int y, bool isPixelPerfect) {
    Rectangle bounds;
    Point localPoint;
    Point globalPoint;

    if (!obj || !obj->visible) return 0;

    globalPoint = Point_create(x, y);
    DisplayObject_globalToLocal(obj, &globalPoint, &localPoint);

    bounds = DisplayObject_getBounds(obj);
    return Rectangle_contains(&bounds, localPoint.x, localPoint.y);
}

/* ========== 属性设置函数 ========== */

void DisplayObject_setScale(DisplayObject* obj, double scale) {
    Bitmap* bitmap;
    int originalWidth, originalHeight;

    if (!obj) return;

    obj->scaleX = scale;
    obj->scaleY = scale;

    bitmap = (Bitmap*)obj;
    if (bitmap->_bitmapData) {
        originalWidth = bitmap->_bitmapData->width;
        originalHeight = bitmap->_bitmapData->height;
        obj->width = (int)(originalWidth * scale);
        obj->height = (int)(originalHeight * scale);
        obj->_bounds.width = obj->width;
        obj->_bounds.height = obj->height;
    }

    markMatrixDirty(obj);
}

void DisplayObject_setScaleX(DisplayObject* obj, double scaleX) {
    Bitmap* bitmap;
    int originalWidth;

    if (!obj) return;

    obj->scaleX = scaleX;

    bitmap = (Bitmap*)obj;
    if (bitmap->_bitmapData) {
        originalWidth = bitmap->_bitmapData->width;
        obj->width = (int)(originalWidth * scaleX);
        obj->_bounds.width = obj->width;
    }

    markMatrixDirty(obj);
}

void DisplayObject_setScaleY(DisplayObject* obj, double scaleY) {
    Bitmap* bitmap;
    int originalHeight;

    if (!obj) return;

    obj->scaleY = scaleY;

    bitmap = (Bitmap*)obj;
    if (bitmap->_bitmapData) {
        originalHeight = bitmap->_bitmapData->height;
        obj->height = (int)(originalHeight * scaleY);
        obj->_bounds.height = obj->height;
    }

    markMatrixDirty(obj);
}

void DisplayObject_setAlpha(DisplayObject* obj, int alpha) {
    if (!obj) return;
    obj->alpha = (alpha < 0) ? 0 : (alpha > 255 ? 255 : alpha);
}

void DisplayObject_setVisible(DisplayObject* obj, bool visible) {
    if (!obj) return;
    obj->visible = visible;
}

void DisplayObject_setName(DisplayObject* obj, const char* name) {
    if (!obj) return;
    obj->name = name;
}

void DisplayObject_setWidth(DisplayObject* obj, int width) {
    if (!obj) return;
    obj->width = width;
    obj->_bounds.width = width;
}

void DisplayObject_setHeight(DisplayObject* obj, int height) {
    if (!obj) return;
    obj->height = height;
    obj->_bounds.height = height;
}

void DisplayObject_setPosition(DisplayObject* obj, int x, int y) {
    if (!obj) return;
    obj->x = x;
    obj->y = y;
    markMatrixDirty(obj);
}

void DisplayObject_setRotation(DisplayObject* obj, int rotation) {
    if (!obj) return;
    obj->rotation = rotation % 360;
    if (obj->rotation < 0) obj->rotation += 360;
    markMatrixDirty(obj);
}

void DisplayObject_setX(DisplayObject* obj, int x) {
    if (!obj) return;
    obj->x = x;
    markMatrixDirty(obj);
}

void DisplayObject_setY(DisplayObject* obj, int y) {
    if (!obj) return;
    obj->y = y;
    markMatrixDirty(obj);
}

void DisplayObject_setSkew(DisplayObject* obj, int skewX, int skewY) {
    if (!obj) return;
    obj->skewX = skewX;
    obj->skewY = skewY;
    markMatrixDirty(obj);
}

void DisplayObject_setAnchorOffset(DisplayObject* obj, int x, int y) {
    if (!obj) return;
    obj->anchorOffsetX = x;
    obj->anchorOffsetY = y;
    markMatrixDirty(obj);
}

/* ========== 属性获取函数 ========== */

int DisplayObject_getX(DisplayObject* obj) {
    return obj ? obj->x : 0;
}

int DisplayObject_getY(DisplayObject* obj) {
    return obj ? obj->y : 0;
}

int DisplayObject_getWidth(DisplayObject* obj) {
    return obj ? obj->width : 0;
}

int DisplayObject_getHeight(DisplayObject* obj) {
    return obj ? obj->height : 0;
}

double DisplayObject_getScaleX(DisplayObject* obj) {
    return obj ? obj->scaleX : 1.0;
}

double DisplayObject_getScaleY(DisplayObject* obj) {
    return obj ? obj->scaleY : 1.0;
}

int DisplayObject_getRotation(DisplayObject* obj) {
    return obj ? obj->rotation : 0;
}

int DisplayObject_getAlpha(DisplayObject* obj) {
    return obj ? obj->alpha : 255;
}

bool DisplayObject_getVisible(DisplayObject* obj) {
    return obj ? obj->visible : 0;
}

const char* DisplayObject_getName(DisplayObject* obj) {
    return obj ? obj->name : NULL;
}

/* ========== 遮罩和滤镜 ========== */

void DisplayObject_setMask(DisplayObject* obj, DisplayObject* mask) {
    if (!obj) return;
    obj->mask = mask;
}

void DisplayObject_setFilters(DisplayObject* obj, void* filters) {
    if (!obj) return;
    obj->_filters = filters;
}

void DisplayObject_setCacheAsBitmap(DisplayObject* obj, bool value) {
    if (!obj) return;
    obj->cacheAsBitmap = value;
}

/* ========== Shape图形对象实现 ========== */

/* 图形绘制指令类型 */
typedef enum {
    GRAPHICS_CMD_BEGIN_FILL,
    GRAPHICS_CMD_END_FILL,
    GRAPHICS_CMD_LINE_STYLE,
    GRAPHICS_CMD_DRAW_RECT,
    GRAPHICS_CMD_DRAW_CIRCLE,
    GRAPHICS_CMD_DRAW_ROUND_RECT,
    GRAPHICS_CMD_DRAW_ELLIPSE,
    GRAPHICS_CMD_DRAW_LINE,
    GRAPHICS_CMD_DRAW_TRIANGLE,
    GRAPHICS_CMD_DRAW_POLYGON
} GraphicsCommandType;

/* 图形绘制指令 */
typedef struct GraphicsCommand {
    GraphicsCommandType type;
    union {
        struct { uint32 color; int alpha; } beginFill;
        struct { int thickness; uint32 color; int alpha; } lineStyle;
        struct { int x; int y; int width; int height; } rect;
        struct { int x; int y; int radius; } circle;
        struct { int x; int y; int width; int height; int ellipseWidth; int ellipseHeight; } roundRect;
        struct { int x; int y; int width; int height; } ellipse;
        struct { int x1; int y1; int x2; int y2; } line;
        struct { int x1; int y1; int x2; int y2; int x3; int y3; } triangle;
        struct { Point* points; int pointCount; } polygon;
    } params;
    struct GraphicsCommand* next;
} GraphicsCommand;

Shape* egret_Shape_create(void) {
    Shape* shape;

    shape = (Shape*)mrc_malloc(sizeof(Shape));
    if (!shape) return NULL;

    mrc_memset(shape, 0, sizeof(Shape));

    shape->base = *egret_DisplayObject_create();
    shape->base.type = DISPLAY_OBJECT_TYPE_SHAPE;
    shape->_style.fillColor = 0xFFFFFFFF;
    shape->_style.strokeColor = 0xFF000000;
    shape->_style.strokeWidth = 1;
    shape->_cachedBitmap = NULL;
    shape->_cacheNeedsUpdate = 1;

    /* 初始化函数指针 */
    shape->beginFill = Shape_graphics_beginFill;
    shape->endFill = Shape_graphics_endFill;
    shape->lineStyle = Shape_graphics_lineStyle;
    shape->drawRect = Shape_graphics_drawRect;
    shape->drawCircle = Shape_graphics_drawCircle;
    shape->drawRoundRect = Shape_graphics_drawRoundRect;
    shape->clear = Shape_graphics_clear;

    registerObject((DisplayObject*)shape);

    return shape;
}

void Shape_graphics_clear(Shape* shape) {
    GraphicsCommand* cmd;
    GraphicsCommand* next;

    if (!shape) return;

    /* 释放图形指令链表 */
    if (shape->_graphicsData) {
        cmd = (GraphicsCommand*)shape->_graphicsData;
        while (cmd) {
            next = cmd->next;

            /* 释放多边形指令中动态分配的点数组 */
            if (cmd->type == GRAPHICS_CMD_DRAW_POLYGON && cmd->params.polygon.points) {
                mrc_free(cmd->params.polygon.points);
            }

            mrc_free(cmd);
            cmd = next;
        }
        shape->_graphicsData = NULL;
    }

    /* 释放缓存的bitmap */
    if (shape->_cachedBitmap) {
        if (shape->_cachedBitmap->bitmap) {
            mrc_free(shape->_cachedBitmap->bitmap);
        }
        mrc_free(shape->_cachedBitmap);
        shape->_cachedBitmap = NULL;
    }

    /* 标记缓存需要更新 */
    shape->_cacheNeedsUpdate = 1;
}

void Shape_graphics_beginFill(Shape* shape, uint32 color, int alpha) {
    GraphicsCommand* cmd;

    if (!shape) return;

    shape->_style.fillColor = (color & 0xFFFFFF) | ((alpha & 0xFF) << 24);
    shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 beginFill 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    cmd->type = GRAPHICS_CMD_BEGIN_FILL;
    cmd->params.beginFill.color = color;
    cmd->params.beginFill.alpha = alpha;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

void Shape_graphics_endFill(Shape* shape) {
    if (!shape) return;
}

void Shape_graphics_lineStyle(Shape* shape, int thickness, uint32 color, int alpha) {
    GraphicsCommand* cmd;

    if (!shape) return;

    /* 更新当前样式 */
    shape->_style.strokeWidth = thickness;
    shape->_style.strokeColor = (color & 0xFFFFFF) | ((alpha & 0xFF) << 24);
    shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 lineStyle 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    cmd->type = GRAPHICS_CMD_LINE_STYLE;
    cmd->params.lineStyle.thickness = thickness;
    cmd->params.lineStyle.color = color;
    cmd->params.lineStyle.alpha = alpha;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

void Shape_graphics_drawRect(Shape* shape, int x, int y, int width, int height) {
    GraphicsCommand* cmd;

    if (!shape) return;

    shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 drawRect 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    cmd->type = GRAPHICS_CMD_DRAW_RECT;
    cmd->params.rect.x = x;
    cmd->params.rect.y = y;
    cmd->params.rect.width = width;
    cmd->params.rect.height = height;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

void Shape_graphics_drawCircle(Shape* shape, int x, int y, int radius) {
    GraphicsCommand* cmd;

    if (!shape) return;
shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 drawCircle 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    cmd->type = GRAPHICS_CMD_DRAW_CIRCLE;
    cmd->params.circle.x = x;
    cmd->params.circle.y = y;
    cmd->params.circle.radius = radius;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

void Shape_graphics_drawLine(Shape* shape, int x1, int y1, int x2, int y2, int thickness, uint32 color) {
    GraphicsCommand* cmd;

    if (!shape) return;
shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 drawLine 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    cmd->type = GRAPHICS_CMD_DRAW_LINE;
    cmd->params.line.x1 = x1;
    cmd->params.line.y1 = y1;
    cmd->params.line.x2 = x2;
    cmd->params.line.y2 = y2;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

void Shape_graphics_drawTriangle(Shape* shape, int x1, int y1,
                                int x2, int y2,
                                int x3, int y3,
                                uint32 fillColor, uint32 strokeColor, int strokeWidth) {
    GraphicsCommand* cmd;

    if (!shape) return;
shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 drawTriangle 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    cmd->type = GRAPHICS_CMD_DRAW_TRIANGLE;
    cmd->params.triangle.x1 = x1;
    cmd->params.triangle.y1 = y1;
    cmd->params.triangle.x2 = x2;
    cmd->params.triangle.y2 = y2;
    cmd->params.triangle.x3 = x3;
    cmd->params.triangle.y3 = y3;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

void Shape_graphics_drawPolygon(Shape* shape, const Point* points, int pointCount,
                                uint32 fillColor, uint32 strokeColor, int strokeWidth) {
    GraphicsCommand* cmd;
    Point* pointsCopy;
    int i;

    if (!shape || !points || pointCount < 3) return;
shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 drawPolygon 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    /* 复制点数组（需要持久化） */
    pointsCopy = (Point*)mrc_malloc(pointCount * sizeof(Point));
    if (!pointsCopy) {
        mrc_free(cmd);
        return;
    }

    for (i = 0; i < pointCount; i++) {
        pointsCopy[i] = points[i];
    }

    cmd->type = GRAPHICS_CMD_DRAW_POLYGON;
    cmd->params.polygon.points = pointsCopy;
    cmd->params.polygon.pointCount = pointCount;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

void Shape_graphics_drawRoundRect(Shape* shape, int x, int y,
                                 int width, int height, int ellipseWidth, int ellipseHeight) {
    GraphicsCommand* cmd;

    if (!shape) return;
shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 drawRoundRect 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    cmd->type = GRAPHICS_CMD_DRAW_ROUND_RECT;
    cmd->params.roundRect.x = x;
    cmd->params.roundRect.y = y;
    cmd->params.roundRect.width = width;
    cmd->params.roundRect.height = height;
    cmd->params.roundRect.ellipseWidth = ellipseWidth;
    cmd->params.roundRect.ellipseHeight = ellipseHeight;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

void Shape_graphics_drawEllipse(Shape* shape, int x, int y, int width, int height) {
    GraphicsCommand* cmd;

    if (!shape) return;
shape->_cacheNeedsUpdate = 1;  /* 标记缓存需要更新 */

    /* 创建 drawEllipse 指令 */
    cmd = (GraphicsCommand*)mrc_malloc(sizeof(GraphicsCommand));
    if (!cmd) return;

    cmd->type = GRAPHICS_CMD_DRAW_ELLIPSE;
    cmd->params.ellipse.x = x;
    cmd->params.ellipse.y = y;
    cmd->params.ellipse.width = width;
    cmd->params.ellipse.height = height;
    cmd->next = NULL;

    /* 添加到指令链表末尾 */
    if (!shape->_graphicsData) {
        shape->_graphicsData = cmd;
    } else {
        GraphicsCommand* last = (GraphicsCommand*)shape->_graphicsData;
        while (last->next) {
            last = last->next;
        }
        last->next = cmd;
    }
}

/* ========== Bitmap位图对象实现 ========== */

Bitmap* egret_Bitmap_create(void) {
    Bitmap* bitmap;

    bitmap = (Bitmap*)mrc_malloc(sizeof(Bitmap));
    if (!bitmap) return NULL;

    mrc_memset(bitmap, 0, sizeof(Bitmap));

    bitmap->base = *egret_DisplayObject_create();
    bitmap->base.type = DISPLAY_OBJECT_TYPE_BITMAP;
    bitmap->_smoothing = 0;
    bitmap->_pixelSnapping = 1;

    /* 初始化base的函数指针 */
    bitmap->base.setX = DisplayObject_setX;
    bitmap->base.setY = DisplayObject_setY;
    bitmap->base.setWidth = DisplayObject_setWidth;
    bitmap->base.setHeight = DisplayObject_setHeight;
    bitmap->base.setScaleX = DisplayObject_setScaleX;
    bitmap->base.setScaleY = DisplayObject_setScaleY;
    bitmap->base.setScale = DisplayObject_setScale;
    bitmap->base.setRotation = DisplayObject_setRotation;
    bitmap->base.setAlpha = DisplayObject_setAlpha;
    bitmap->base.setVisible = DisplayObject_setVisible;
    bitmap->base.setAnchorOffset = DisplayObject_setAnchorOffset;

    /* 初始化Bitmap特有的函数指针 */
    bitmap->setBitmapData = Bitmap_setBitmapData;
    bitmap->setSourceRect = Bitmap_setSourceRect;
    bitmap->setSmoothing = Bitmap_setSmoothing;
    bitmap->setPixelSnapping = Bitmap_setPixelSnapping;

    /* 初始化Bitmap的DisplayObject方法包装函数指针 */
    bitmap->setX = Bitmap_setX;
    bitmap->setY = Bitmap_setY;
    bitmap->setWidth = Bitmap_setWidth;
    bitmap->setHeight = Bitmap_setHeight;
    bitmap->setScaleX = Bitmap_setScaleX;
    bitmap->setScaleY = Bitmap_setScaleY;
    bitmap->setScale = Bitmap_setScale;
    bitmap->setRotation = Bitmap_setRotation;
    bitmap->setAlpha = Bitmap_setAlpha;
    bitmap->setVisible = Bitmap_setVisible;
    bitmap->setAnchorOffset = Bitmap_setAnchorOffset;

    registerObject((DisplayObject*)bitmap);

    return bitmap;
}

void Bitmap_setBitmapData(Bitmap* bitmap, BITMAP_565* bitmapData) {
    if (!bitmap) return;

    bitmap->_bitmapData = bitmapData;

    if (bitmapData) {
        bitmap->base.width = bitmapData->width;
        bitmap->base.height = bitmapData->height;
        bitmap->base._bounds = Rectangle_create(0, 0, bitmapData->width, bitmapData->height);
    }
}

BITMAP_565* Bitmap_getBitmapData(Bitmap* bitmap) {
    return bitmap ? bitmap->_bitmapData : NULL;
}

void Bitmap_setSourceRect(Bitmap* bitmap, const Rectangle* rect) {
    if (!bitmap || !rect) return;
    bitmap->_sourceRect = *rect;
}

Rectangle Bitmap_getSourceRect(Bitmap* bitmap) {
    if (!bitmap) return Rectangle_create(0, 0, 0, 0);
    return bitmap->_sourceRect;
}

void Bitmap_setSmoothing(Bitmap* bitmap, bool smoothing) {
    if (!bitmap) return;
    bitmap->_smoothing = smoothing;
}

bool Bitmap_getSmoothing(Bitmap* bitmap) {
    return bitmap ? bitmap->_smoothing : 0;
}

void Bitmap_setPixelSnapping(Bitmap* bitmap, int snapping) {
    if (!bitmap) return;
    bitmap->_pixelSnapping = snapping;
}

int Bitmap_getPixelSnapping(Bitmap* bitmap) {
    return bitmap ? bitmap->_pixelSnapping : 0;
}

/* Bitmap的DisplayObject方法包装函数 */
void Bitmap_setX(Bitmap* bitmap, int x) {
    if (!bitmap) return;
    DisplayObject_setX((DisplayObject*)bitmap, x);
}

void Bitmap_setY(Bitmap* bitmap, int y) {
    if (!bitmap) return;
    DisplayObject_setY((DisplayObject*)bitmap, y);
}

void Bitmap_setWidth(Bitmap* bitmap, int width) {
    if (!bitmap) return;
    DisplayObject_setWidth((DisplayObject*)bitmap, width);
}

void Bitmap_setHeight(Bitmap* bitmap, int height) {
    if (!bitmap) return;
    DisplayObject_setHeight((DisplayObject*)bitmap, height);
}

void Bitmap_setScaleX(Bitmap* bitmap, double scaleX) {
    if (!bitmap) return;
    DisplayObject_setScaleX((DisplayObject*)bitmap, scaleX);
}

void Bitmap_setScaleY(Bitmap* bitmap, double scaleY) {
    if (!bitmap) return;
    DisplayObject_setScaleY((DisplayObject*)bitmap, scaleY);
}

void Bitmap_setScale(Bitmap* bitmap, double scale) {
    if (!bitmap) return;
    DisplayObject_setScale((DisplayObject*)bitmap, scale);
}

void Bitmap_setRotation(Bitmap* bitmap, int rotation) {
    if (!bitmap) return;
    DisplayObject_setRotation((DisplayObject*)bitmap, rotation);
}

void Bitmap_setAlpha(Bitmap* bitmap, int alpha) {
    if (!bitmap) return;
    DisplayObject_setAlpha((DisplayObject*)bitmap, alpha);
}

void Bitmap_setVisible(Bitmap* bitmap, bool visible) {
    if (!bitmap) return;
    DisplayObject_setVisible((DisplayObject*)bitmap, visible);
}

void Bitmap_setAnchorOffset(Bitmap* bitmap, int x, int y) {
    if (!bitmap) return;
    DisplayObject_setAnchorOffset((DisplayObject*)bitmap, x, y);
}

/* ========== Sprite容器实现 ========== */

Sprite* egret_Sprite_create(void) {
    Sprite* sprite;

    sprite = (Sprite*)mrc_malloc(sizeof(Sprite));
    if (!sprite) return NULL;

    mrc_memset(sprite, 0, sizeof(Sprite));

    sprite->base = *egret_DisplayObject_create();
    sprite->base.type = DISPLAY_OBJECT_TYPE_SPRITE;

    /* 初始化函数指针 */
    sprite->addChild = Sprite_addChild;
    sprite->addChildAt = Sprite_addChildAt;
    sprite->removeChild = Sprite_removeChild;
    sprite->removeChildAt = Sprite_removeChildAt;
    sprite->getChildAt = Sprite_getChildAt;
    sprite->getChildIndex = Sprite_getChildIndex;
    sprite->numChildren = Sprite_numChildren;
    sprite->swapChildren = Sprite_swapChildren;
    sprite->swapChildrenAt = Sprite_swapChildrenAt;

    registerObject((DisplayObject*)sprite);

    return sprite;
}

void Sprite_addChild(Sprite* sprite, DisplayObject* child) {
    if (!sprite || !child) return;

    if (child->parent) {
        Sprite_removeChild((Sprite*)child->parent, child);
    }

    child->parent = (DisplayObject*)sprite;

    if (!sprite->base._firstChild) {
        sprite->base._firstChild = child;
        sprite->base._lastChild = child;
        child->_prevSibling = NULL;
        child->_nextSibling = NULL;
    } else {
        child->_prevSibling = sprite->base._lastChild;
        child->_nextSibling = NULL;
        sprite->base._lastChild->_nextSibling = child;
        sprite->base._lastChild = child;
    }

    markMatrixDirty(child);
}

void Sprite_addChildAt(Sprite* sprite, DisplayObject* child, int32 index) {
    DisplayObject* current;
    int32 i;

    if (!sprite || !child) return;

    if (index <= 0) {
        if (sprite->base._firstChild) {
            child->_nextSibling = sprite->base._firstChild;
            sprite->base._firstChild->_prevSibling = child;
            sprite->base._firstChild = child;
        } else {
            Sprite_addChild(sprite, child);
        }
        return;
    }

    current = sprite->base._firstChild;
    for (i = 0; i < index && current; i++) {
        if (!current->_nextSibling) {
            Sprite_addChild(sprite, child);
            return;
        }
        current = current->_nextSibling;
    }

    if (current) {
        child->_prevSibling = current->_prevSibling;
        child->_nextSibling = current;
        if (current->_prevSibling) {
            current->_prevSibling->_nextSibling = child;
        } else {
            sprite->base._firstChild = child;
        }
        current->_prevSibling = child;
    }

    child->parent = (DisplayObject*)sprite;
    markMatrixDirty(child);
}

DisplayObject* Sprite_removeChild(Sprite* sprite, DisplayObject* child) {
    if (!sprite || !child || child->parent != (DisplayObject*)sprite) {
        return NULL;
    }

    if (child->_prevSibling) {
        child->_prevSibling->_nextSibling = child->_nextSibling;
    } else {
        sprite->base._firstChild = child->_nextSibling;
    }

    if (child->_nextSibling) {
        child->_nextSibling->_prevSibling = child->_prevSibling;
    } else {
        sprite->base._lastChild = child->_prevSibling;
    }

    child->parent = NULL;
    child->_prevSibling = NULL;
    child->_nextSibling = NULL;

    return child;
}

DisplayObject* Sprite_removeChildAt(Sprite* sprite, int32 index) {
    DisplayObject* child;
    int32 i;

    if (!sprite) return NULL;

    child = sprite->base._firstChild;
    for (i = 0; i < index && child; i++) {
        child = child->_nextSibling;
    }

    if (child) {
        return Sprite_removeChild(sprite, child);
    }

    return NULL;
}

void Sprite_removeChildren(Sprite* sprite) {
    DisplayObject* child;
    DisplayObject* next;

    if (!sprite) return;

    child = sprite->base._firstChild;
    while (child) {
        next = child->_nextSibling;
        Sprite_removeChild(sprite, child);
        child = next;
    }
}

DisplayObject* Sprite_getChildAt(Sprite* sprite, int32 index) {
    DisplayObject* child;
    int32 i;

    if (!sprite) return NULL;

    child = sprite->base._firstChild;
    for (i = 0; i < index && child; i++) {
        child = child->_nextSibling;
    }

    return child;
}

DisplayObject* Sprite_getChildByName(Sprite* sprite, const char* name) {
    DisplayObject* child;

    if (!sprite || !name) return NULL;

    child = sprite->base._firstChild;
    while (child) {
        if (child->name && mrc_strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->_nextSibling;
    }

    return NULL;
}

int32 Sprite_getChildIndex(Sprite* sprite, DisplayObject* child) {
    DisplayObject* current;
    int32 index;

    if (!sprite || !child) return -1;

    current = sprite->base._firstChild;
    index = 0;
    while (current) {
        if (current == child) return index;
        current = current->_nextSibling;
        index++;
    }

    return -1;
}

void Sprite_setChildIndex(Sprite* sprite, DisplayObject* child, int32 index) {
    if (!sprite || !child) return;

    Sprite_removeChild(sprite, child);
    Sprite_addChildAt(sprite, child, index);
}

int32 Sprite_numChildren(Sprite* sprite) {
    DisplayObject* child;
    int32 count;

    if (!sprite) return 0;

    count = 0;
    child = sprite->base._firstChild;
    while (child) {
        count++;
        child = child->_nextSibling;
    }

    return count;
}

void Sprite_swapChildren(Sprite* sprite, DisplayObject* child1, DisplayObject* child2) {
    int32 index1, index2;

    if (!sprite || !child1 || !child2) return;

    index1 = Sprite_getChildIndex(sprite, child1);
    index2 = Sprite_getChildIndex(sprite, child2);

    if (index1 < 0 || index2 < 0) return;

    Sprite_swapChildrenAt(sprite, index1, index2);
}

void Sprite_swapChildrenAt(Sprite* sprite, int32 index1, int32 index2) {
    DisplayObject* child1;
    DisplayObject* child2;

    if (!sprite || index1 == index2) return;

    child1 = Sprite_getChildAt(sprite, index1);
    child2 = Sprite_getChildAt(sprite, index2);

    if (!child1 || !child2) return;

    Sprite_removeChild(sprite, child1);
    Sprite_removeChild(sprite, child2);

    if (index1 < index2) {
        Sprite_addChildAt(sprite, child2, index1);
        Sprite_addChildAt(sprite, child1, index2);
    } else {
        Sprite_addChildAt(sprite, child1, index2);
        Sprite_addChildAt(sprite, child2, index1);
    }
}

bool Sprite_contains(Sprite* sprite, DisplayObject* child) {
    DisplayObject* current;

    if (!sprite || !child) return 0;

    current = sprite->base._firstChild;
    while (current) {
        if (current == child) return 1;
        current = current->_nextSibling;
    }

    return 0;
}

/* ========== Stage舞台实现 ========== */

Stage* egret_Stage_create(void) {
    Stage* stage;
    mr_screeninfo screenInfo;

    stage = (Stage*)mrc_malloc(sizeof(Stage));
    if (!stage) return NULL;

    mrc_memset(stage, 0, sizeof(Stage));

    stage->base = *egret_DisplayObject_create();
    stage->base.type = DISPLAY_OBJECT_TYPE_STAGE;

    mrc_getScreenInfo(&screenInfo);
    stage->stageWidth = screenInfo.width;
    stage->stageHeight = screenInfo.height;
    stage->base.width = screenInfo.width;
    stage->base.height = screenInfo.height;

    stage->renderContext = w_getScreenBuffer();
    stage->frameRate = 30;
    stage->scaleMode = "noScale";
    stage->align = "center";

    /* 初始化容器函数指针 */
    stage->addChild = Stage_addChild;
    stage->addChildAt = Stage_addChildAt;
    stage->removeChild = Stage_removeChild;
    stage->removeChildAt = Stage_removeChildAt;
    stage->getChildAt = Stage_getChildAt;
    stage->getChildIndex = Stage_getChildIndex;
    stage->numChildren = Stage_numChildren;
    stage->swapChildren = Stage_swapChildren;
    stage->swapChildrenAt = Stage_swapChildrenAt;

    /* 初始化渲染函数指针 */
    stage->render = Stage_render;

    g_objectManager.stage = stage;

    return stage;
}

Stage* Stage_create(uint16* buffer, int width, int height) {
    Stage* stage;

    stage = (Stage*)mrc_malloc(sizeof(Stage));
    if (!stage) return NULL;

    mrc_memset(stage, 0, sizeof(Stage));

    stage->base = *egret_DisplayObject_create();
    stage->base.type = DISPLAY_OBJECT_TYPE_STAGE;
    stage->stageWidth = width;
    stage->stageHeight = height;
    stage->base.width = width;
    stage->base.height = height;
    stage->renderContext = buffer;
    stage->frameRate = 30;

    /* 初始化容器函数指针 */
    stage->addChild = Stage_addChild;
    stage->addChildAt = Stage_addChildAt;
    stage->removeChild = Stage_removeChild;
    stage->removeChildAt = Stage_removeChildAt;
    stage->getChildAt = Stage_getChildAt;
    stage->getChildIndex = Stage_getChildIndex;
    stage->numChildren = Stage_numChildren;
    stage->swapChildren = Stage_swapChildren;
    stage->swapChildrenAt = Stage_swapChildrenAt;

    /* 初始化渲染函数指针 */
    stage->render = Stage_render;

    g_objectManager.stage = stage;

    return stage;
}

void Stage_setSize(Stage* stage, int width, int height) {
    if (!stage) return;

    stage->stageWidth = width;
    stage->stageHeight = height;
    stage->base.width = width;
    stage->base.height = height;
}

/* ========== Stage容器方法 ========== */

void Stage_addChild(Stage* stage, DisplayObject* child) {
    if (!stage || !child) return;

    if (child->parent) {
        Stage_removeChild((Stage*)child->parent, child);
    }

    child->parent = (DisplayObject*)stage;

    if (!stage->base._firstChild) {
        stage->base._firstChild = child;
        stage->base._lastChild = child;
        child->_prevSibling = NULL;
        child->_nextSibling = NULL;
    } else {
        child->_prevSibling = stage->base._lastChild;
        child->_nextSibling = NULL;
        stage->base._lastChild->_nextSibling = child;
        stage->base._lastChild = child;
    }

    markMatrixDirty(child);
}

void Stage_addChildAt(Stage* stage, DisplayObject* child, int32 index) {
    DisplayObject* current;
    int32 i;

    if (!stage || !child) return;

    if (index <= 0) {
        if (stage->base._firstChild) {
            child->_nextSibling = stage->base._firstChild;
            stage->base._firstChild->_prevSibling = child;
            stage->base._firstChild = child;
            child->parent = (DisplayObject*)stage;
        } else {
            Stage_addChild(stage, child);
        }
        return;
    }

    current = stage->base._firstChild;
    for (i = 0; i < index && current; i++) {
        if (!current->_nextSibling) {
            Stage_addChild(stage, child);
            return;
        }
        current = current->_nextSibling;
    }

    if (current) {
        child->_prevSibling = current->_prevSibling;
        child->_nextSibling = current;
        if (current->_prevSibling) {
            current->_prevSibling->_nextSibling = child;
        } else {
            stage->base._firstChild = child;
        }
        current->_prevSibling = child;
    }

    child->parent = (DisplayObject*)stage;
    markMatrixDirty(child);
}

DisplayObject* Stage_removeChild(Stage* stage, DisplayObject* child) {
    if (!stage || !child || child->parent != (DisplayObject*)stage) {
        return NULL;
    }

    if (child->_prevSibling) {
        child->_prevSibling->_nextSibling = child->_nextSibling;
    } else {
        stage->base._firstChild = child->_nextSibling;
    }

    if (child->_nextSibling) {
        child->_nextSibling->_prevSibling = child->_prevSibling;
    } else {
        stage->base._lastChild = child->_prevSibling;
    }

    child->parent = NULL;
    child->_prevSibling = NULL;
    child->_nextSibling = NULL;

    return child;
}

DisplayObject* Stage_removeChildAt(Stage* stage, int32 index) {
    DisplayObject* child;
    int32 i;

    if (!stage) return NULL;

    child = stage->base._firstChild;
    for (i = 0; i < index && child; i++) {
        child = child->_nextSibling;
    }

    if (child) {
        return Stage_removeChild(stage, child);
    }

    return NULL;
}

void Stage_removeChildren(Stage* stage) {
    DisplayObject* child;
    DisplayObject* next;

    if (!stage) return;

    child = stage->base._firstChild;
    while (child) {
        next = child->_nextSibling;
        Stage_removeChild(stage, child);
        child = next;
    }
}

DisplayObject* Stage_getChildAt(Stage* stage, int32 index) {
    DisplayObject* child;
    int32 i;

    if (!stage) return NULL;

    child = stage->base._firstChild;
    for (i = 0; i < index && child; i++) {
        child = child->_nextSibling;
    }

    return child;
}

DisplayObject* Stage_getChildByName(Stage* stage, const char* name) {
    DisplayObject* child;

    if (!stage || !name) return NULL;

    child = stage->base._firstChild;
    while (child) {
        if (child->name && mrc_strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->_nextSibling;
    }

    return NULL;
}

int32 Stage_getChildIndex(Stage* stage, DisplayObject* child) {
    DisplayObject* current;
    int32 index;

    if (!stage || !child) return -1;

    current = stage->base._firstChild;
    index = 0;
    while (current) {
        if (current == child) return index;
        current = current->_nextSibling;
        index++;
    }

    return -1;
}

void Stage_setChildIndex(Stage* stage, DisplayObject* child, int32 index) {
    if (!stage || !child) return;

    Stage_removeChild(stage, child);
    Stage_addChildAt(stage, child, index);
}

int32 Stage_numChildren(Stage* stage) {
    DisplayObject* child;
    int32 count;

    if (!stage) return 0;

    count = 0;
    child = stage->base._firstChild;
    while (child) {
        count++;
        child = child->_nextSibling;
    }

    return count;
}

void Stage_swapChildren(Stage* stage, DisplayObject* child1, DisplayObject* child2) {
    int32 index1, index2;

    if (!stage || !child1 || !child2) return;

    index1 = Stage_getChildIndex(stage, child1);
    index2 = Stage_getChildIndex(stage, child2);

    if (index1 < 0 || index2 < 0) return;

    Stage_swapChildrenAt(stage, index1, index2);
}

void Stage_swapChildrenAt(Stage* stage, int32 index1, int32 index2) {
    DisplayObject* child1;
    DisplayObject* child2;

    if (!stage || index1 == index2) return;

    child1 = Stage_getChildAt(stage, index1);
    child2 = Stage_getChildAt(stage, index2);

    if (!child1 || !child2) return;

    Stage_removeChild(stage, child1);
    Stage_removeChild(stage, child2);

    if (index1 < index2) {
        Stage_addChildAt(stage, child2, index1);
        Stage_addChildAt(stage, child1, index2);
    } else {
        Stage_addChildAt(stage, child1, index2);
        Stage_addChildAt(stage, child2, index1);
    }
}

bool Stage_contains(Stage* stage, DisplayObject* child) {
    DisplayObject* current;

    if (!stage || !child) return 0;

    current = stage->base._firstChild;
    while (current) {
        if (current == child) return 1;
        current = current->_nextSibling;
    }

    return 0;
}

/* ========== Stage渲染相关 ========== */

/**
 * 创建临时bitmap用于带变换的渲染
 */
static BITMAP_565* createTempBitmap(int width, int height) {
    BITMAP_565* bmp;

    bmp = (BITMAP_565*)mrc_malloc(sizeof(BITMAP_565));
    if (!bmp) return NULL;

    mrc_memset(bmp, 0, sizeof(BITMAP_565));
    bmp->bitmap = (uint16*)mrc_malloc(width * height * 2);
    if (!bmp->bitmap) {
        mrc_free(bmp);
        return NULL;
    }

    mrc_memset(bmp->bitmap, 0, width * height * 2);
    bmp->width = width;
    bmp->height = height;
    bmp->color_bit = 16;
    bmp->mode = BM_TRANSPARENT;
    bmp->transcolor = 0;
    bmp->buflen = width * height * 2;

    return bmp;
}

/**
 * 绘制三角形到stage缓冲区（扫描线填充算法）
 */
static void drawTriangleToStage(Stage* stage, int x1, int y1, int x2, int y2, int x3, int y3,
                                 uint16 color, int alpha) {
    uint16* stageBuf;
    int minX, maxX, minY, maxY;
    int y, x;
    int ax, ay, bx, by, cx, cy;
    int tmp;

    if (!stage || !stage->renderContext) return;

    stageBuf = (uint16*)stage->renderContext;

    /* 按y坐标排序三个顶点：(ax,ay) <= (bx,by) <= (cx,cy) */
    ax = x1; ay = y1;
    bx = x2; by = y2;
    cx = x3; cy = y3;

    if (ay > by) { tmp = ax; ax = bx; bx = tmp; tmp = ay; ay = by; by = tmp; }
    if (by > cy) { tmp = bx; bx = cx; cx = tmp; tmp = by; by = cy; cy = tmp; }
    if (ay > by) { tmp = ax; ax = bx; bx = tmp; tmp = ay; ay = by; by = tmp; }

    /* 计算边界 */
    minY = ay;
    maxY = cy;
    if (minY < 0) minY = 0;
    if (maxY >= stage->stageHeight) maxY = stage->stageHeight - 1;

    /* 扫描线填充 */
    for (y = minY; y <= maxY; y++) {
        int startX = stage->stageWidth;
        int endX = -1;

        /* 计算与三条边的交点 */
        /* 边 a-b */
        if (y >= ay && y <= by && by != ay) {
            int edgeX = ax + (bx - ax) * (y - ay) / (by - ay);
            if (edgeX < startX) startX = edgeX;
            if (edgeX > endX) endX = edgeX;
        }

        /* 边 b-c */
        if (y >= by && y <= cy && cy != by) {
            int edgeX = bx + (cx - bx) * (y - by) / (cy - by);
            if (edgeX < startX) startX = edgeX;
            if (edgeX > endX) endX = edgeX;
        }

        /* 边 a-c */
        if (y >= ay && y <= cy && cy != ay) {
            int edgeX = ax + (cx - ax) * (y - ay) / (cy - ay);
            if (edgeX < startX) startX = edgeX;
            if (edgeX > endX) endX = edgeX;
        }

        /* 限制x范围 */
        if (startX < 0) startX = 0;
        if (endX >= stage->stageWidth) endX = stage->stageWidth - 1;

        /* 绘制扫描线 */
        for (x = startX; x <= endX; x++) {
            int index = y * stage->stageWidth + x;
            if (alpha >= 255) {
                stageBuf[index] = color;
            } else if (alpha > 0) {
                stageBuf[index] = blendColorRGB565(stageBuf[index], color, alpha);
            }
        }
    }
}

/**
 * 绘制多边形到stage缓冲区（扫描线填充算法）
 */
static void drawPolygonToStage(Stage* stage, const Point* points, int pointCount,
                                uint16 color, int alpha) {
    uint16* stageBuf;
    int minY, maxY;
    int y, i, j;
    int* intersections;
    int intersectionCount;

    if (!stage || !stage->renderContext || !points || pointCount < 3) return;

    stageBuf = (uint16*)stage->renderContext;

    /* 找到y的范围 */
    minY = maxY = points[0].y;
    for (i = 1; i < pointCount; i++) {
        if (points[i].y < minY) minY = points[i].y;
        if (points[i].y > maxY) maxY = points[i].y;
    }

    if (minY < 0) minY = 0;
    if (maxY >= stage->stageHeight) maxY = stage->stageHeight - 1;

    /* 分配临时数组存储交点 */
    intersections = (int*)mrc_malloc(pointCount * sizeof(int));
    if (!intersections) return;

    /* 扫描线填充 */
    for (y = minY; y <= maxY; y++) {
        intersectionCount = 0;

        /* 计算扫描线与所有边的交点 */
        for (i = 0; i < pointCount; i++) {
            j = (i + 1) % pointCount;

            int y1 = points[i].y;
            int y2 = points[j].y;

            /* 跳过水平边 */
            if (y1 == y2) continue;

            /* 确保 y1 < y2 */
            if (y1 > y2) {
                int tmp = y1; y1 = y2; y2 = tmp;
                int tmpIdx = i; i = j; j = tmpIdx;
            }

            /* 检查扫描线是否与边相交 */
            if (y >= y1 && y < y2) {
                int x1 = points[i].x;
                int x2 = points[j].x;
                int intersectX = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                intersections[intersectionCount++] = intersectX;
            }
        }

        /* 对交点排序（简单冒泡排序） */
        for (i = 0; i < intersectionCount - 1; i++) {
            for (j = i + 1; j < intersectionCount; j++) {
                if (intersections[i] > intersections[j]) {
                    int tmp = intersections[i];
                    intersections[i] = intersections[j];
                    intersections[j] = tmp;
                }
            }
        }

        /* 填充交点对之间的像素 */
        for (i = 0; i < intersectionCount; i += 2) {
            if (i + 1 < intersectionCount) {
                int startX = intersections[i];
                int endX = intersections[i + 1];
                int x;

                if (startX < 0) startX = 0;
                if (endX >= stage->stageWidth) endX = stage->stageWidth - 1;

                for (x = startX; x <= endX; x++) {
                    int index = y * stage->stageWidth + x;
                    if (alpha >= 255) {
                        stageBuf[index] = color;
                    } else if (alpha > 0) {
                        stageBuf[index] = blendColorRGB565(stageBuf[index], color, alpha);
                    }
                }
            }
        }
    }

    mrc_free(intersections);
}

/**
 * 直接在Stage缓存中绘制像素，支持alpha、遮罩
 */
static void drawPixelToStage(Stage* stage, int x, int y, uint16 color, int alpha, DisplayObject* mask) {
    uint16* stageBuf;
    int stageIndex;

    if (!stage || x < 0 || y < 0 || x >= stage->stageWidth || y >= stage->stageHeight) {
        return;
    }

    if (mask && !DisplayObject_hitTestPoint(mask, x, y, 0)) {
        return;
    }

    stageBuf = (uint16*)stage->renderContext;
    if (!stageBuf) return;

    stageIndex = y * stage->stageWidth + x;

    if (alpha >= 255) {
        stageBuf[stageIndex] = color;
    } else if (alpha > 0) {
        stageBuf[stageIndex] = blendColorRGB565(stageBuf[stageIndex], color, alpha);
    }
}

/**
 * 渲染Bitmap到Stage，支持完整的变换矩阵、alpha、遮罩
 * 使用逆向映射避免空洞
 */
static void renderBitmapToStage(Stage* stage, BITMAP_565* srcBmp, Matrix* worldMatrix, int alpha, DisplayObject* mask) {
    int sx, sy, dx, dy;
    int minX, minY, maxX, maxY;
    Matrix invMatrix;
    int srcX, srcY;
    uint16 color;
    int srcIndex;
    int cornerX[4], cornerY[4];
    int i;

    if (!stage || !srcBmp || !srcBmp->bitmap || !worldMatrix) {
        return;
    }

    cornerX[0] = 0;
    cornerY[0] = 0;
    cornerX[1] = srcBmp->width;
    cornerY[1] = 0;
    cornerX[2] = srcBmp->width;
    cornerY[2] = srcBmp->height;
    cornerX[3] = 0;
    cornerY[3] = srcBmp->height;

    minX = stage->stageWidth;
    minY = stage->stageHeight;
    maxX = 0;
    maxY = 0;

    for (i = 0; i < 4; i++) {
        Point p = Matrix_transformPoint(worldMatrix, cornerX[i], cornerY[i]);
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX > stage->stageWidth) maxX = stage->stageWidth;
    if (maxY > stage->stageHeight) maxY = stage->stageHeight;

    invMatrix = *worldMatrix;
    Matrix_invert(&invMatrix);

    for (dy = minY; dy < maxY; dy++) {
        for (dx = minX; dx < maxX; dx++) {
            Point srcPoint;

            srcPoint = Matrix_transformPoint(&invMatrix, dx, dy);
            srcX = srcPoint.x;
            srcY = srcPoint.y;

            if (srcX >= 0 && srcX < srcBmp->width && srcY >= 0 && srcY < srcBmp->height) {
                srcIndex = srcY * srcBmp->width + srcX;
                color = srcBmp->bitmap[srcIndex];

                if (srcBmp->mode == BM_TRANSPARENT && color == srcBmp->transcolor) {
                    continue;
                }

                drawPixelToStage(stage, dx, dy, color, alpha, mask);
            }
        }
    }
}

/**
 * 渲染单个显示对象到Stage缓存
 */
/**
 * @brief 检查显示对象是否具有单位变换(无旋转、缩放、倾斜等)
 * @param obj 要检查的显示对象
 * @return 如果是单位变换返回1，否则返回0
 */
static int hasIdentityTransform(DisplayObject* obj) {
    if (!obj) return 0;

    return (obj->rotation == 0 &&
            obj->scaleX == 1.0 &&
            obj->scaleY == 1.0 &&
            obj->skewX == 0 &&
            obj->skewY == 0 &&
            obj->anchorOffsetX == 0 &&
            obj->anchorOffsetY == 0);
}

/**
 * @brief 检查显示对象及其父链是否都具有单位变换
 * @param obj 要检查的显示对象
 * @return 如果整个父链都是单位变换返回1，否则返回0
 */
static int hasIdentityTransformChain(DisplayObject* obj) {
    DisplayObject* current = obj;

    while (current) {
        if (!hasIdentityTransform(current)) {
            return 0;
        }
        current = current->parent;
    }

    return 1;
}

/**
 * @brief 优化的直接绘制函数（无变换）
 * @param stage 舞台对象
 * @param bmp 要绘制的位图
 * @param worldX 世界坐标X
 * @param worldY 世界坐标Y
 */
 void _renderBitmapDirect(Stage* stage, BITMAP_565* bmp, int worldX, int worldY) {
    /* 使用int16类型进行计算，与gl_bitmapShowExTrans保持一致 */
    int16 i, j;
    int16 startX, startY, endX, endY;
    int16 sourceX, sourceY;
    int32 sindex, pindex;
    uint16* screenBuf;
    uint16 pixelColor;

    if (!stage || !bmp || !bmp->bitmap || !stage->renderContext) {
        return;
    }

    screenBuf = stage->renderContext;

    /* 计算绘制区域实际边界 */
    startX = (worldX < 0) ? 0 : (int16)worldX;
    startY = (worldY < 0) ? 0 : (int16)worldY;
    endX = (worldX + bmp->width > stage->stageWidth) ? (int16)stage->stageWidth : (int16)(worldX + bmp->width);
    endY = (worldY + bmp->height > stage->stageHeight) ? (int16)stage->stageHeight : (int16)(worldY + bmp->height);

    /* 如果完全超出屏幕，直接返回 */
    if (startX >= endX || startY >= endY) {
        return;
    }

    /* 计算源图片在绘制区域中的对应位置 */
    sourceX = startX - (int16)worldX;
    sourceY = startY - (int16)worldY;

    /* 根据色深和模式选择不同的绘制方式 */
    if (bmp->color_bit == 16) {
        if (bmp->mode == BM_TRANSPARENT) {
            /* 透明模式：逐像素判断透明色 */
            uint16 transColor = bmp->transcolor;
            for (i = startY; i < endY; i++) {
                pindex = (sourceY * bmp->width + sourceX);
                sindex = (i * stage->stageWidth + startX);

                for (j = startX; j < endX; j++) {
                    pixelColor = bmp->bitmap[pindex];
                    if (pixelColor != transColor) {
                        screenBuf[sindex] = pixelColor;
                    }
                    sindex++;
                    pindex++;
                }
                sourceY++;
            }
        } else {
            /* 不透明模式：使用memcpy批量复制 */
            int32 copyBytes = (endX - startX) * 2;
            for (i = startY; i < endY; i++) {
                pindex = (sourceY * bmp->width + sourceX);
                sindex = (i * stage->stageWidth + startX);
                mrc_memcpy(&screenBuf[sindex], &bmp->bitmap[pindex], copyBytes);
                sourceY++;
            }
        }
    } else if (bmp->color_bit == 32) {
        /* 32位RGBA8888格式 */
        uint32* bitmap32 = (uint32*)bmp->bitmap;
        uint32 color32;
        uint8 alpha, r, g, b;

        for (i = startY; i < endY; i++) {
            pindex = (sourceY * bmp->width + sourceX);
            sindex = (i * stage->stageWidth + startX);

            for (j = startX; j < endX; j++) {
                color32 = bitmap32[pindex];
                alpha = (color32 >> 24) & 0xFF;

                if (alpha > 0) {
                    r = (color32 >> 16) & 0xFF;
                    g = (color32 >> 8) & 0xFF;
                    b = color32 & 0xFF;

                    if (alpha == 255) {
                        screenBuf[sindex] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
                    } else {
                        screenBuf[sindex] = blendColorRGB565(screenBuf[sindex],
                            ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3), alpha);
                    }
                }
                sindex++;
                pindex++;
            }
            sourceY++;
        }
    }
}

static void renderDisplayObject(DisplayObject* obj, Stage* stage) {
    Bitmap* bitmap;
    Shape* shape;
    int effectiveAlpha;
    DisplayObject* parent;
    Matrix renderMatrix;
    BITMAP_565* tempBmp;
    int needScale;
    int srcW, srcH;
    int useOptimizedPath;

    // mrc_printf("[renderDisplayObject] Start obj=%p", obj);

    if (!obj || !stage || !obj->visible || obj->alpha == 0) {
        // mrc_printf("[renderDisplayObject] Early return");
        return;
    }

    /* 计算有效透明度 */
    effectiveAlpha = obj->alpha;
    parent = obj->parent;
    while (parent) {
        if (parent->alpha < 255) {
            effectiveAlpha = (effectiveAlpha * parent->alpha) / 255;
        }
        parent = parent->parent;
    }

    /* 检查是否可以使用优化路径:
     * 1. 整个父链都是单位变换
     * 2. 无遮罩
     * 3. 透明度为255(完全不透明)
     */
    useOptimizedPath = (hasIdentityTransformChain(obj) &&
                        !obj->mask &&
                        effectiveAlpha == 255);

    // mrc_printf("[renderDisplayObject] Check bitmap, useOptimizedPath=%d", useOptimizedPath);

    /* 只有Bitmap对象才渲染bitmap数据 */
    if (obj->type == DISPLAY_OBJECT_TYPE_BITMAP) {
        bitmap = (Bitmap*)obj;
        if (bitmap->_bitmapData && bitmap->_bitmapData->bitmap) {
        // mrc_printf("[renderDisplayObject] Has bitmap data");
        srcW = bitmap->_bitmapData->width;
        srcH = bitmap->_bitmapData->height;
        needScale = (obj->width != srcW || obj->height != srcH);

        // mrc_printf("[renderDisplayObject] srcW=%d, srcH=%d, objW=%d, objH=%d, needScale=%d",
        //            srcW, srcH, obj->width, obj->height, needScale);

        if (useOptimizedPath) {
            /* 优化路径：直接绘制，无需矩阵变换 */
            int worldX = 0;
            int worldY = 0;
            DisplayObject* current = obj;

            /* 计算世界坐标(累加父对象的x,y) */
            while (current) {
                worldX += current->x;
                worldY += current->y;
                current = current->parent;
            }

            // mrc_printf("[renderDisplayObject] Optimized path: worldX=%d, worldY=%d", worldX, worldY);

            if (needScale) {
                int x, y;
                int x_ratio, y_ratio;
                int src_x, src_y;

                mrc_printf("[renderDisplayObject] Creating temp bitmap for scaling");
                tempBmp = createTempBitmap(obj->width, obj->height);
                if (!tempBmp) {
                    mrc_printf("[renderDisplayObject] ERROR: createTempBitmap failed");
                    return;
                }

                tempBmp->mode = bitmap->_bitmapData->mode;
                tempBmp->transcolor = bitmap->_bitmapData->transcolor;

                x_ratio = (srcW << 16) / obj->width + 1;
                y_ratio = (srcH << 16) / obj->height + 1;

                mrc_printf("[renderDisplayObject] Scaling bitmap");
                for (y = 0; y < obj->height; y++) {
                    for (x = 0; x < obj->width; x++) {
                        src_x = (x * x_ratio) >> 16;
                        src_y = (y * y_ratio) >> 16;

                        if (src_x >= 0 && src_y >= 0 && src_x < srcW && src_y < srcH) {
                            tempBmp->bitmap[y * obj->width + x] = bitmap->_bitmapData->bitmap[src_y * srcW + src_x];
                        }
                    }
                }

                mrc_printf("[renderDisplayObject] Direct draw scaled bitmap");
                _renderBitmapDirect(stage, tempBmp, worldX, worldY);

                mrc_printf("[renderDisplayObject] Freeing temp bitmap");
                mrc_free(tempBmp->bitmap);
                mrc_free(tempBmp);
            } else {
                /* 无缩放，直接绘制原始位图 */
                // mrc_printf("[renderDisplayObject] Direct draw bitmap");
                _renderBitmapDirect(stage, bitmap->_bitmapData, worldX, worldY);
            }
        } else {
            /* 标准路径：使用矩阵变换 */
            mrc_printf("[renderDisplayObject] updateWorldMatrix");
            updateWorldMatrix(obj);

            if (needScale) {
                int x, y;
                int x_ratio, y_ratio;
                int src_x, src_y;

                mrc_printf("[renderDisplayObject] Creating temp bitmap");
                tempBmp = createTempBitmap(obj->width, obj->height);
                if (!tempBmp) {
                    mrc_printf("[renderDisplayObject] ERROR: createTempBitmap failed");
                    return;
                }

                tempBmp->mode = bitmap->_bitmapData->mode;
                tempBmp->transcolor = bitmap->_bitmapData->transcolor;

                x_ratio = (srcW << 16) / obj->width + 1;
                y_ratio = (srcH << 16) / obj->height + 1;

                mrc_printf("[renderDisplayObject] Scaling bitmap");
                for (y = 0; y < obj->height; y++) {
                    for (x = 0; x < obj->width; x++) {
                        src_x = (x * x_ratio) >> 16;
                        src_y = (y * y_ratio) >> 16;

                        if (src_x >= 0 && src_y >= 0 && src_x < srcW && src_y < srcH) {
                            tempBmp->bitmap[y * obj->width + x] = bitmap->_bitmapData->bitmap[src_y * srcW + src_x];
                        }
                    }
                }

                mrc_printf("[renderDisplayObject] Rendering scaled bitmap");
                renderMatrix = obj->_worldMatrix;
                renderBitmapToStage(stage, tempBmp, &renderMatrix, effectiveAlpha, obj->mask);

                mrc_printf("[renderDisplayObject] Freeing temp bitmap");
                mrc_free(tempBmp->bitmap);
                mrc_free(tempBmp);
            } else {
                mrc_printf("[renderDisplayObject] Rendering without scaling");
                renderMatrix = obj->_worldMatrix;
                renderBitmapToStage(stage, bitmap->_bitmapData, &renderMatrix, effectiveAlpha, obj->mask);
            }
        }
        }
    }

    /* 只有Shape对象才渲染图形数据 */
    if (obj->type == DISPLAY_OBJECT_TYPE_SHAPE) {
        GraphicsCommand* cmd;
        uint32 currentFillColor;
        int worldX, worldY;
        DisplayObject* current;
        int useOptimizedPath;
        uint8 r, g, b, alpha;
        uint16 rgb565Color;
        uint16* stageBuf;

        shape = (Shape*)obj;
        if (shape->_graphicsData) {
            /* 检查是否可以使用优化路径（无变换） */
            useOptimizedPath = hasIdentityTransformChain(obj);

            if (useOptimizedPath) {
                /* 优化路径：直接绘制到stage->renderContext */
                /* 计算Shape的世界坐标 */
                worldX = 0;
                worldY = 0;
                current = obj;
                while (current) {
                    worldX += current->x;
                    worldY += current->y;
                    current = current->parent;
                }

                /* 计算有效透明度 */
                effectiveAlpha = obj->alpha;
                current = obj->parent;
                while (current) {
                    effectiveAlpha = (effectiveAlpha * current->alpha) / 255;
                    current = current->parent;
                }

                currentFillColor = 0xFFFFFFFF;
                stageBuf = (uint16*)stage->renderContext;

                /* 遍历并执行图形指令 */
                cmd = (GraphicsCommand*)shape->_graphicsData;
                while (cmd) {
                    switch (cmd->type) {
                        case GRAPHICS_CMD_BEGIN_FILL:
                            /* 提取颜色和alpha */
                            r = (cmd->params.beginFill.color >> 16) & 0xFF;
                            g = (cmd->params.beginFill.color >> 8) & 0xFF;
                            b = cmd->params.beginFill.color & 0xFF;
                            alpha = (cmd->params.beginFill.alpha * effectiveAlpha) / 255;

                            currentFillColor = (alpha << 24) | (r << 16) | (g << 8) | b;
                            break;

                        case GRAPHICS_CMD_DRAW_RECT: {
                            /* 直接绘制矩形到stage->renderContext */
                            int rectX = worldX + cmd->params.rect.x;
                            int rectY = worldY + cmd->params.rect.y;
                            int rectW = cmd->params.rect.width;
                            int rectH = cmd->params.rect.height;
                            int x, y;

                            /* 转换为RGB565 */
                            r = (currentFillColor >> 16) & 0xFF;
                            g = (currentFillColor >> 8) & 0xFF;
                            b = currentFillColor & 0xFF;
                            alpha = (currentFillColor >> 24) & 0xFF;
                            rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                            /* 绘制矩形 */
                            for (y = rectY; y < rectY + rectH; y++) {
                                if (y < 0 || y >= stage->stageHeight) continue;
                                for (x = rectX; x < rectX + rectW; x++) {
                                    if (x < 0 || x >= stage->stageWidth) continue;

                                    int index = y * stage->stageWidth + x;
                                    if (alpha >= 255) {
                                        stageBuf[index] = rgb565Color;
                                    } else if (alpha > 0) {
                                        stageBuf[index] = blendColorRGB565(stageBuf[index], rgb565Color, alpha);
                                    }
                                }
                            }
                            break;
                        }

                        case GRAPHICS_CMD_DRAW_CIRCLE: {
                            /* 直接绘制圆形到stage->renderContext */
                            int cx = worldX + cmd->params.circle.x;
                            int cy = worldY + cmd->params.circle.y;
                            int radius = cmd->params.circle.radius;
                            int x, y;
                            int dx, dy, distance;

                            /* 转换为RGB565 */
                            r = (currentFillColor >> 16) & 0xFF;
                            g = (currentFillColor >> 8) & 0xFF;
                            b = currentFillColor & 0xFF;
                            alpha = (currentFillColor >> 24) & 0xFF;
                            rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                            /* 绘制圆形（遍历包围盒） */
                            for (y = cy - radius; y <= cy + radius; y++) {
                                if (y < 0 || y >= stage->stageHeight) continue;
                                for (x = cx - radius; x <= cx + radius; x++) {
                                    if (x < 0 || x >= stage->stageWidth) continue;

                                    dx = x - cx;
                                    dy = y - cy;
                                    distance = int_sqrt(dx * dx + dy * dy);

                                    if (distance <= radius) {
                                        int index = y * stage->stageWidth + x;
                                        if (alpha >= 255) {
                                            stageBuf[index] = rgb565Color;
                                        } else if (alpha > 0) {
                                            stageBuf[index] = blendColorRGB565(stageBuf[index], rgb565Color, alpha);
                                        }
                                    }
                                }
                            }
                            break;
                        }

                        case GRAPHICS_CMD_LINE_STYLE:
                            /* 更新当前线条样式 */
                            shape->_style.strokeWidth = cmd->params.lineStyle.thickness;
                            shape->_style.strokeColor = (cmd->params.lineStyle.color & 0xFFFFFF) |
                                                       ((cmd->params.lineStyle.alpha & 0xFF) << 24);
                            break;

                        case GRAPHICS_CMD_DRAW_LINE: {
                            /* 绘制线段到stage->renderContext (使用Bresenham算法) */
                            int x0 = worldX + cmd->params.line.x1;
                            int y0 = worldY + cmd->params.line.y1;
                            int x1 = worldX + cmd->params.line.x2;
                            int y1 = worldY + cmd->params.line.y2;
                            int dx, dy, sx, sy, err, e2;
                            int x, y;

                            /* 提取线条颜色 */
                            uint32 lineColor = shape->_style.strokeColor;
                            r = (lineColor >> 16) & 0xFF;
                            g = (lineColor >> 8) & 0xFF;
                            b = lineColor & 0xFF;
                            alpha = ((lineColor >> 24) & 0xFF);
                            alpha = (alpha * effectiveAlpha) / 255;
                            rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                            /* Bresenham直线算法 */
                            dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
                            dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
                            sx = (x0 < x1) ? 1 : -1;
                            sy = (y0 < y1) ? 1 : -1;
                            err = dx - dy;

                            x = x0;
                            y = y0;
                            while (1) {
                                if (x >= 0 && x < stage->stageWidth && y >= 0 && y < stage->stageHeight) {
                                    int index = y * stage->stageWidth + x;
                                    if (alpha >= 255) {
                                        stageBuf[index] = rgb565Color;
                                    } else if (alpha > 0) {
                                        stageBuf[index] = blendColorRGB565(stageBuf[index], rgb565Color, alpha);
                                    }
                                }

                                if (x == x1 && y == y1) break;

                                e2 = 2 * err;
                                if (e2 > -dy) {
                                    err -= dy;
                                    x += sx;
                                }
                                if (e2 < dx) {
                                    err += dx;
                                    y += sy;
                                }
                            }
                            break;
                        }

                        case GRAPHICS_CMD_DRAW_ELLIPSE: {
                            /* 绘制椭圆到stage->renderContext */
                            int cx = worldX + cmd->params.ellipse.x + cmd->params.ellipse.width / 2;
                            int cy = worldY + cmd->params.ellipse.y + cmd->params.ellipse.height / 2;
                            int rx = cmd->params.ellipse.width / 2;
                            int ry = cmd->params.ellipse.height / 2;
                            int x, y;

                            /* 转换为RGB565 */
                            r = (currentFillColor >> 16) & 0xFF;
                            g = (currentFillColor >> 8) & 0xFF;
                            b = currentFillColor & 0xFF;
                            alpha = (currentFillColor >> 24) & 0xFF;
                            rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                            /* 遍历椭圆包围盒 */
                            for (y = cy - ry; y <= cy + ry; y++) {
                                if (y < 0 || y >= stage->stageHeight) continue;
                                for (x = cx - rx; x <= cx + rx; x++) {
                                    if (x < 0 || x >= stage->stageWidth) continue;

                                    /* 椭圆方程: (x-cx)²/rx² + (y-cy)²/ry² <= 1 */
                                    int dx = x - cx;
                                    int dy = y - cy;
                                    int value = (dx * dx * ry * ry + dy * dy * rx * rx);
                                    int threshold = rx * rx * ry * ry;

                                    if (value <= threshold) {
                                        int index = y * stage->stageWidth + x;
                                        if (alpha >= 255) {
                                            stageBuf[index] = rgb565Color;
                                        } else if (alpha > 0) {
                                            stageBuf[index] = blendColorRGB565(stageBuf[index], rgb565Color, alpha);
                                        }
                                    }
                                }
                            }
                            break;
                        }

                        case GRAPHICS_CMD_DRAW_ROUND_RECT: {
                            /* 绘制圆角矩形到stage->renderContext */
                            int rectX = worldX + cmd->params.roundRect.x;
                            int rectY = worldY + cmd->params.roundRect.y;
                            int rectW = cmd->params.roundRect.width;
                            int rectH = cmd->params.roundRect.height;
                            int radiusX = cmd->params.roundRect.ellipseWidth / 2;
                            int radiusY = cmd->params.roundRect.ellipseHeight / 2;
                            int x, y;

                            /* 转换为RGB565 */
                            r = (currentFillColor >> 16) & 0xFF;
                            g = (currentFillColor >> 8) & 0xFF;
                            b = currentFillColor & 0xFF;
                            alpha = (currentFillColor >> 24) & 0xFF;
                            rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                            /* 限制圆角半径不超过矩形尺寸的一半 */
                            if (radiusX > rectW / 2) radiusX = rectW / 2;
                            if (radiusY > rectH / 2) radiusY = rectH / 2;

                            /* 绘制圆角矩形 */
                            for (y = rectY; y < rectY + rectH; y++) {
                                if (y < 0 || y >= stage->stageHeight) continue;
                                for (x = rectX; x < rectX + rectW; x++) {
                                    if (x < 0 || x >= stage->stageWidth) continue;

                                    int inRect = 0;

                                    /* 判断点是否在圆角矩形内 */
                                    /* 四个角的圆形区域 */
                                    if (x < rectX + radiusX && y < rectY + radiusY) {
                                        /* 左上角 */
                                        int dx = (rectX + radiusX) - x;
                                        int dy = (rectY + radiusY) - y;
                                        int dist2 = dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX;
                                        inRect = (dist2 <= radiusX * radiusX * radiusY * radiusY);
                                    } else if (x >= rectX + rectW - radiusX && y < rectY + radiusY) {
                                        /* 右上角 */
                                        int dx = x - (rectX + rectW - radiusX - 1);
                                        int dy = (rectY + radiusY) - y;
                                        int dist2 = dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX;
                                        inRect = (dist2 <= radiusX * radiusX * radiusY * radiusY);
                                    } else if (x < rectX + radiusX && y >= rectY + rectH - radiusY) {
                                        /* 左下角 */
                                        int dx = (rectX + radiusX) - x;
                                        int dy = y - (rectY + rectH - radiusY - 1);
                                        int dist2 = dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX;
                                        inRect = (dist2 <= radiusX * radiusX * radiusY * radiusY);
                                    } else if (x >= rectX + rectW - radiusX && y >= rectY + rectH - radiusY) {
                                        /* 右下角 */
                                        int dx = x - (rectX + rectW - radiusX - 1);
                                        int dy = y - (rectY + rectH - radiusY - 1);
                                        int dist2 = dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX;
                                        inRect = (dist2 <= radiusX * radiusX * radiusY * radiusY);
                                    } else {
                                        /* 中间矩形区域 */
                                        inRect = 1;
                                    }

                                    if (inRect) {
                                        int index = y * stage->stageWidth + x;
                                        if (alpha >= 255) {
                                            stageBuf[index] = rgb565Color;
                                        } else if (alpha > 0) {
                                            stageBuf[index] = blendColorRGB565(stageBuf[index], rgb565Color, alpha);
                                        }
                                    }
                                }
                            }
                            break;
                        }

                        case GRAPHICS_CMD_DRAW_TRIANGLE: {
                            /* 绘制三角形 */
                            int tx1 = worldX + cmd->params.triangle.x1;
                            int ty1 = worldY + cmd->params.triangle.y1;
                            int tx2 = worldX + cmd->params.triangle.x2;
                            int ty2 = worldY + cmd->params.triangle.y2;
                            int tx3 = worldX + cmd->params.triangle.x3;
                            int ty3 = worldY + cmd->params.triangle.y3;

                            /* 转换为RGB565 */
                            r = (currentFillColor >> 16) & 0xFF;
                            g = (currentFillColor >> 8) & 0xFF;
                            b = currentFillColor & 0xFF;
                            alpha = (currentFillColor >> 24) & 0xFF;
                            rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                            drawTriangleToStage(stage, tx1, ty1, tx2, ty2, tx3, ty3, rgb565Color, alpha);
                            break;
                        }

                        case GRAPHICS_CMD_DRAW_POLYGON: {
                            /* 绘制多边形 */
                            Point* worldPoints;
                            int i;

                            /* 转换为RGB565 */
                            r = (currentFillColor >> 16) & 0xFF;
                            g = (currentFillColor >> 8) & 0xFF;
                            b = currentFillColor & 0xFF;
                            alpha = (currentFillColor >> 24) & 0xFF;
                            rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                            /* 复制点数组并转换到世界坐标 */
                            worldPoints = (Point*)mrc_malloc(cmd->params.polygon.pointCount * sizeof(Point));
                            if (worldPoints) {
                                for (i = 0; i < cmd->params.polygon.pointCount; i++) {
                                    worldPoints[i].x = worldX + cmd->params.polygon.points[i].x;
                                    worldPoints[i].y = worldY + cmd->params.polygon.points[i].y;
                                }

                                drawPolygonToStage(stage, worldPoints, cmd->params.polygon.pointCount,
                                                  rgb565Color, alpha);

                                mrc_free(worldPoints);
                            }
                            break;
                        }

                        default:
                            break;
                    }
                    cmd = cmd->next;
                }
            } else {
                /* 有变换属性：使用缓存bitmap进行渲染 */
                GraphicsCommand* cmd;
                Rectangle bounds;
                int bmpWidth, bmpHeight;
                BITMAP_565* cacheBitmap;
                Matrix renderMatrix;

                /* 计算Shape的包围盒 */
                int minX = 99999, minY = 99999, maxX = -99999, maxY = -99999;
                cmd = (GraphicsCommand*)shape->_graphicsData;
                while (cmd) {
                    switch (cmd->type) {
                        case GRAPHICS_CMD_DRAW_RECT:
                            if (cmd->params.rect.x < minX) minX = cmd->params.rect.x;
                            if (cmd->params.rect.y < minY) minY = cmd->params.rect.y;
                            if (cmd->params.rect.x + cmd->params.rect.width > maxX)
                                maxX = cmd->params.rect.x + cmd->params.rect.width;
                            if (cmd->params.rect.y + cmd->params.rect.height > maxY)
                                maxY = cmd->params.rect.y + cmd->params.rect.height;
                            break;
                        case GRAPHICS_CMD_DRAW_CIRCLE: {
                            int r = cmd->params.circle.radius;
                            if (cmd->params.circle.x - r < minX) minX = cmd->params.circle.x - r;
                            if (cmd->params.circle.y - r < minY) minY = cmd->params.circle.y - r;
                            if (cmd->params.circle.x + r > maxX) maxX = cmd->params.circle.x + r;
                            if (cmd->params.circle.y + r > maxY) maxY = cmd->params.circle.y + r;
                            break;
                        }
                        default:
                            break;
                    }
                    cmd = cmd->next;
                }

                if (minX >= maxX || minY >= maxY) {
                    /* 没有有效的图形 */
                    return;
                }

                bmpWidth = maxX - minX;
                bmpHeight = maxY - minY;

                /* 检查是否需要重新生成缓存 */
                if (shape->_cacheNeedsUpdate || !shape->_cachedBitmap) {
                    /* 释放旧的缓存 */
                    if (shape->_cachedBitmap) {
                        if (shape->_cachedBitmap->bitmap) {
                            mrc_free(shape->_cachedBitmap->bitmap);
                        }
                        mrc_free(shape->_cachedBitmap);
                    }

                    /* 创建新的缓存bitmap */
                    cacheBitmap = createTempBitmap(bmpWidth, bmpHeight);
                    if (!cacheBitmap) return;

                    /* 创建临时stage用于绘制 */
                    Stage tempStage;
                    mrc_memset(&tempStage, 0, sizeof(Stage));
                    tempStage.base.type = DISPLAY_OBJECT_TYPE_STAGE;
                    tempStage.stageWidth = bmpWidth;
                    tempStage.stageHeight = bmpHeight;
                    tempStage.renderContext = cacheBitmap->bitmap;

                    /* 绘制Shape到临时bitmap（调整坐标偏移） */
                    uint32 currentFillColor = 0xFFFFFFFF;
                    uint16 rgb565Color;
                    uint16* stageBuf = cacheBitmap->bitmap;
                    uint8 r, g, b, alpha;

                    cmd = (GraphicsCommand*)shape->_graphicsData;
                    while (cmd) {
                        switch (cmd->type) {
                            case GRAPHICS_CMD_BEGIN_FILL:
                                r = (cmd->params.beginFill.color >> 16) & 0xFF;
                                g = (cmd->params.beginFill.color >> 8) & 0xFF;
                                b = cmd->params.beginFill.color & 0xFF;
                                alpha = cmd->params.beginFill.alpha;
                                currentFillColor = (alpha << 24) | (r << 16) | (g << 8) | b;
                                break;

                            case GRAPHICS_CMD_DRAW_RECT: {
                                int rectX = cmd->params.rect.x - minX;
                                int rectY = cmd->params.rect.y - minY;
                                int rectW = cmd->params.rect.width;
                                int rectH = cmd->params.rect.height;
                                int x, y;

                                r = (currentFillColor >> 16) & 0xFF;
                                g = (currentFillColor >> 8) & 0xFF;
                                b = currentFillColor & 0xFF;
                                alpha = (currentFillColor >> 24) & 0xFF;
                                rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                                for (y = rectY; y < rectY + rectH && y < bmpHeight; y++) {
                                    if (y < 0) continue;
                                    for (x = rectX; x < rectX + rectW && x < bmpWidth; x++) {
                                        if (x < 0) continue;
                                        int index = y * bmpWidth + x;
                                        stageBuf[index] = rgb565Color;
                                    }
                                }
                                break;
                            }

                            case GRAPHICS_CMD_DRAW_CIRCLE: {
                                int cx = cmd->params.circle.x - minX;
                                int cy = cmd->params.circle.y - minY;
                                int radius = cmd->params.circle.radius;
                                int x, y, dx, dy, distance;

                                r = (currentFillColor >> 16) & 0xFF;
                                g = (currentFillColor >> 8) & 0xFF;
                                b = currentFillColor & 0xFF;
                                alpha = (currentFillColor >> 24) & 0xFF;
                                rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                                for (y = cy - radius; y <= cy + radius && y < bmpHeight; y++) {
                                    if (y < 0) continue;
                                    for (x = cx - radius; x <= cx + radius && x < bmpWidth; x++) {
                                        if (x < 0) continue;
                                        dx = x - cx;
                                        dy = y - cy;
                                        distance = int_sqrt(dx * dx + dy * dy);
                                        if (distance <= radius) {
                                            int index = y * bmpWidth + x;
                                            stageBuf[index] = rgb565Color;
                                        }
                                    }
                                }
                                break;
                            }

                            case GRAPHICS_CMD_LINE_STYLE:
                                /* 更新当前线条样式（暂不实现） */
                                break;

                            case GRAPHICS_CMD_DRAW_LINE: {
                                /* 绘制线段（使用Bresenham算法） */
                                int x0 = cmd->params.line.x1 - minX;
                                int y0 = cmd->params.line.y1 - minY;
                                int x1 = cmd->params.line.x2 - minX;
                                int y1 = cmd->params.line.y2 - minY;
                                int dx, dy, sx, sy, err, e2;
                                int x, y;

                                r = (currentFillColor >> 16) & 0xFF;
                                g = (currentFillColor >> 8) & 0xFF;
                                b = currentFillColor & 0xFF;
                                alpha = (currentFillColor >> 24) & 0xFF;
                                rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                                dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
                                dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
                                sx = (x0 < x1) ? 1 : -1;
                                sy = (y0 < y1) ? 1 : -1;
                                err = dx - dy;

                                x = x0;
                                y = y0;
                                while (1) {
                                    if (x >= 0 && x < bmpWidth && y >= 0 && y < bmpHeight) {
                                        int index = y * bmpWidth + x;
                                        stageBuf[index] = rgb565Color;
                                    }

                                    if (x == x1 && y == y1) break;

                                    e2 = 2 * err;
                                    if (e2 > -dy) {
                                        err -= dy;
                                        x += sx;
                                    }
                                    if (e2 < dx) {
                                        err += dx;
                                        y += sy;
                                    }
                                }
                                break;
                            }

                            case GRAPHICS_CMD_DRAW_ELLIPSE: {
                                /* 绘制椭圆 */
                                int cx = cmd->params.ellipse.x + cmd->params.ellipse.width / 2 - minX;
                                int cy = cmd->params.ellipse.y + cmd->params.ellipse.height / 2 - minY;
                                int rx = cmd->params.ellipse.width / 2;
                                int ry = cmd->params.ellipse.height / 2;
                                int x, y;

                                r = (currentFillColor >> 16) & 0xFF;
                                g = (currentFillColor >> 8) & 0xFF;
                                b = currentFillColor & 0xFF;
                                alpha = (currentFillColor >> 24) & 0xFF;
                                rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                                for (y = cy - ry; y <= cy + ry && y < bmpHeight; y++) {
                                    if (y < 0) continue;
                                    for (x = cx - rx; x <= cx + rx && x < bmpWidth; x++) {
                                        if (x < 0) continue;
                                        int dx = x - cx;
                                        int dy = y - cy;
                                        int value = (dx * dx * ry * ry + dy * dy * rx * rx);
                                        int threshold = rx * rx * ry * ry;
                                        if (value <= threshold) {
                                            int index = y * bmpWidth + x;
                                            stageBuf[index] = rgb565Color;
                                        }
                                    }
                                }
                                break;
                            }

                            case GRAPHICS_CMD_DRAW_ROUND_RECT: {
                                /* 绘制圆角矩形 */
                                int rectX = cmd->params.roundRect.x - minX;
                                int rectY = cmd->params.roundRect.y - minY;
                                int rectW = cmd->params.roundRect.width;
                                int rectH = cmd->params.roundRect.height;
                                int radiusX = cmd->params.roundRect.ellipseWidth / 2;
                                int radiusY = cmd->params.roundRect.ellipseHeight / 2;
                                int x, y;

                                r = (currentFillColor >> 16) & 0xFF;
                                g = (currentFillColor >> 8) & 0xFF;
                                b = currentFillColor & 0xFF;
                                alpha = (currentFillColor >> 24) & 0xFF;
                                rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                                if (radiusX > rectW / 2) radiusX = rectW / 2;
                                if (radiusY > rectH / 2) radiusY = rectH / 2;

                                for (y = rectY; y < rectY + rectH && y < bmpHeight; y++) {
                                    if (y < 0) continue;
                                    for (x = rectX; x < rectX + rectW && x < bmpWidth; x++) {
                                        if (x < 0) continue;
                                        int inRect = 0;

                                        if (x < rectX + radiusX && y < rectY + radiusY) {
                                            int dx = (rectX + radiusX) - x;
                                            int dy = (rectY + radiusY) - y;
                                            int dist2 = dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX;
                                            inRect = (dist2 <= radiusX * radiusX * radiusY * radiusY);
                                        } else if (x >= rectX + rectW - radiusX && y < rectY + radiusY) {
                                            int dx = x - (rectX + rectW - radiusX - 1);
                                            int dy = (rectY + radiusY) - y;
                                            int dist2 = dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX;
                                            inRect = (dist2 <= radiusX * radiusX * radiusY * radiusY);
                                        } else if (x < rectX + radiusX && y >= rectY + rectH - radiusY) {
                                            int dx = (rectX + radiusX) - x;
                                            int dy = y - (rectY + rectH - radiusY - 1);
                                            int dist2 = dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX;
                                            inRect = (dist2 <= radiusX * radiusX * radiusY * radiusY);
                                        } else if (x >= rectX + rectW - radiusX && y >= rectY + rectH - radiusY) {
                                            int dx = x - (rectX + rectW - radiusX - 1);
                                            int dy = y - (rectY + rectH - radiusY - 1);
                                            int dist2 = dx * dx * radiusY * radiusY + dy * dy * radiusX * radiusX;
                                            inRect = (dist2 <= radiusX * radiusX * radiusY * radiusY);
                                        } else {
                                            inRect = 1;
                                        }

                                        if (inRect) {
                                            int index = y * bmpWidth + x;
                                            stageBuf[index] = rgb565Color;
                                        }
                                    }
                                }
                                break;
                            }

                            case GRAPHICS_CMD_DRAW_TRIANGLE: {
                                /* 绘制三角形 */
                                int tx1 = cmd->params.triangle.x1 - minX;
                                int ty1 = cmd->params.triangle.y1 - minY;
                                int tx2 = cmd->params.triangle.x2 - minX;
                                int ty2 = cmd->params.triangle.y2 - minY;
                                int tx3 = cmd->params.triangle.x3 - minX;
                                int ty3 = cmd->params.triangle.y3 - minY;

                                r = (currentFillColor >> 16) & 0xFF;
                                g = (currentFillColor >> 8) & 0xFF;
                                b = currentFillColor & 0xFF;
                                alpha = (currentFillColor >> 24) & 0xFF;
                                rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                                /* 创建临时stage用于三角形绘制 */
                                Stage tempStageForTri;
                                tempStageForTri.stageWidth = bmpWidth;
                                tempStageForTri.stageHeight = bmpHeight;
                                tempStageForTri.renderContext = stageBuf;

                                drawTriangleToStage(&tempStageForTri, tx1, ty1, tx2, ty2, tx3, ty3,
                                                   rgb565Color, 255);
                                break;
                            }

                            case GRAPHICS_CMD_DRAW_POLYGON: {
                                /* 绘制多边形 */
                                Point* adjustedPoints;
                                int i;

                                r = (currentFillColor >> 16) & 0xFF;
                                g = (currentFillColor >> 8) & 0xFF;
                                b = currentFillColor & 0xFF;
                                alpha = (currentFillColor >> 24) & 0xFF;
                                rgb565Color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

                                /* 调整多边形点的坐标 */
                                adjustedPoints = (Point*)mrc_malloc(cmd->params.polygon.pointCount * sizeof(Point));
                                if (adjustedPoints) {
                                    for (i = 0; i < cmd->params.polygon.pointCount; i++) {
                                        adjustedPoints[i].x = cmd->params.polygon.points[i].x - minX;
                                        adjustedPoints[i].y = cmd->params.polygon.points[i].y - minY;
                                    }

                                    /* 创建临时stage用于多边形绘制 */
                                    Stage tempStageForPoly;
                                    tempStageForPoly.stageWidth = bmpWidth;
                                    tempStageForPoly.stageHeight = bmpHeight;
                                    tempStageForPoly.renderContext = stageBuf;

                                    drawPolygonToStage(&tempStageForPoly, adjustedPoints,
                                                      cmd->params.polygon.pointCount, rgb565Color, 255);

                                    mrc_free(adjustedPoints);
                                }
                                break;
                            }

                            default:
                                break;
                        }
                        cmd = cmd->next;
                    }

                    shape->_cachedBitmap = cacheBitmap;
                    shape->_cacheNeedsUpdate = 0;
                }

                /* 使用缓存的bitmap进行渲染 */
                if (shape->_cachedBitmap) {
                    updateWorldMatrix(obj);
                    renderMatrix = obj->_worldMatrix;

                    /* 调整矩阵以考虑图形在本地坐标系中的偏移 */
                    renderMatrix.tx += minX * renderMatrix.a + minY * renderMatrix.c;
                    renderMatrix.ty += minX * renderMatrix.b + minY * renderMatrix.d;

                    renderBitmapToStage(stage, shape->_cachedBitmap, &renderMatrix, effectiveAlpha, obj->mask);
                }
            }
        }
    }

    // mrc_printf("[renderDisplayObject] End");
}

/**
 * 递归渲染显示对象树
 */
static void renderTree(DisplayObject* obj, Stage* stage) {
    DisplayObject* child;

    // mrc_printf("[renderTree] Start obj=%p, stage=%p", obj, stage);

    if (!obj || !stage) {
        // mrc_printf("[renderTree] ERROR: obj or stage is NULL");
        return;
    }

    // mrc_printf("[renderTree] obj visible=%d, name=%s", obj->visible, obj->name ? obj->name : "NULL");

    if (!obj->visible) {
        // mrc_printf("[renderTree] Object not visible, skipping");
        return;
    }

    // mrc_printf("[renderTree] Calling renderDisplayObject");
    renderDisplayObject(obj, stage);
    // mrc_printf("[renderTree] renderDisplayObject completed");

    // mrc_printf("[renderTree] Processing children, firstChild=%p", obj->_firstChild);
    child = obj->_firstChild;
    while (child) {
        // mrc_printf("[renderTree] Rendering child=%p, name=%s", child, child->name ? child->name : "NULL");
        renderTree(child, stage);
        // mrc_printf("[renderTree] Child rendered, moving to next");
        child = child->_nextSibling;
    }

    // mrc_printf("[renderTree] End obj=%p", obj);
}

void Stage_render(Stage* stage) {
    // mrc_printf("[Stage_render] Start stage=%p", stage);

    if (!stage) {
        mrc_printf("[Stage_render] ERROR: stage is NULL");
        return;
    }

    // mrc_printf("[Stage_render] stage type=%d, size=%dx%d", stage->base.type, stage->stageWidth, stage->stageHeight);
    // mrc_printf("[Stage_render] renderContext=%p, firstChild=%p", stage->renderContext, stage->base._firstChild);

    renderTree((DisplayObject*)stage, stage);

    // mrc_printf("[Stage_render] End");
}

/* ========== 显示对象树遍历 ========== */

void DisplayObject_traverse(DisplayObject* obj, DisplayObjectCallback callback, void* userData) {
    DisplayObject* child;

    if (!obj || !callback) return;

    callback(obj, userData);

    child = obj->_firstChild;
    while (child) {
        DisplayObject_traverse(child, callback, userData);
        child = child->_nextSibling;
    }
}

void DisplayObject_traverseVisible(DisplayObject* obj, DisplayObjectCallback callback, void* userData) {
    DisplayObject* child;

    if (!obj || !callback || !obj->visible) return;

    callback(obj, userData);

    child = obj->_firstChild;
    while (child) {
        DisplayObject_traverseVisible(child, callback, userData);
        child = child->_nextSibling;
    }
}

void DisplayObject_traverseChildren(DisplayObject* obj, DisplayObjectCallback callback, void* userData, bool reverse) {
    DisplayObject* child;

    if (!obj || !callback) return;

    if (reverse) {
        child = obj->_lastChild;
        while (child) {
            callback(child, userData);
            child = child->_prevSibling;
        }
    } else {
        child = obj->_firstChild;
        while (child) {
            callback(child, userData);
            child = child->_nextSibling;
        }
    }
}

/* ========== 对象管理和清理 ========== */

/**
 * 递归释放对象及其子对象
 */
static void freeDisplayObject(DisplayObject* obj) {
    DisplayObject* child;
    DisplayObject* next;

    if (!obj) return;

    child = obj->_firstChild;
    while (child) {
        next = child->_nextSibling;
        freeDisplayObject(child);
        child = next;
    }

    unregisterObject(obj);
    mrc_free(obj);
}

void egret_Clear_object(Stage* stage) {
    int i;
    DisplayObject* obj;

    for (i = g_objectManager.count - 1; i >= 0; i--) {
        obj = g_objectManager.objects[i];
        if (obj && !isInStageTree(obj, stage)) {
            freeDisplayObject(obj);
        }
    }
}

int egret_Get_clearObjectCount(Stage* stage) {
    int i;
    int count;
    DisplayObject* obj;

    count = 0;
    for (i = 0; i < g_objectManager.count; i++) {
        obj = g_objectManager.objects[i];
        if (obj && !isInStageTree(obj, stage)) {
            count++;
        }
    }

    return count;
}

void egret_destroy(void) {
    int i;

    for (i = g_objectManager.count - 1; i >= 0; i--) {
        if (g_objectManager.objects[i]) {
            freeDisplayObject(g_objectManager.objects[i]);
        }
    }

    g_objectManager.count = 0;
    g_objectManager.stage = NULL;
}

/* ========== 事件系统占位实现 ========== */

void DisplayObject_addEventListener(DisplayObject* obj, const char* type,
                                   void (*listener)(DisplayObject*, const char*, void*),
                                   void* thisObject, bool useCapture, int32 priority) {
    if (!obj) return;
}

void DisplayObject_removeEventListener(DisplayObject* obj, const char* type,
                                      void (*listener)(DisplayObject*, const char*, void*),
                                      void* thisObject, bool useCapture) {
    if (!obj) return;
}

void DisplayObject_dispatchEvent(DisplayObject* obj, const char* type, void* data) {
    if (!obj) return;
}
