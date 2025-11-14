#ifndef SPRITEX_H
#define SPRITEX_H
#include <stdio.h>
 #include <stdbool.h>
 #include <stdint.h>
// #include <mrc_base.h>
// #include "mrc_graphics.h"

typedef  unsigned short     uint16;      //有符号16bit整型
typedef  unsigned int  uint32;      //无符号32bit整型
typedef  int                int32;      //有符号32bit整型
typedef  unsigned char      uint8;        //无符号8bit整型
typedef  signed char          int8;        //有符号8bit整型
typedef  signed short         int16;       //有符号16bit整型

#define GRAPHICS_LEFT 3
#define GRAPHICS_TOP 48
#define GRAPHICS_RIGHT 5
#define GRAPHICS_CENTER 17
#define GRAPHICS_BOTTOM 80

typedef struct {
 uint16* bitmap;
 int width; //
 int height;
 int color_bit; //颜色位数 默认16
 int transcolor; //透明色 默认0xffff
 int mode; //绘制模式 默认BM_COPY
 int32 buflen; //缓存区宽高
 int32 *memcache; //memcpy绘图优化缓存 存放dst src count(共12字节)
 int32 memlen; //memcpy绘图优化缓存
} BITMAP_565;


#define SPX_HEADER 0x53505258 // SPRX
#define SPX_VERSION 34 // 3.3
#define SPX_BYTE_SEQUENCE_JAVA 1

#define TRANS_NONE 0
#define TRANS_ROT90 5
#define TRANS_ROT180 3
#define TRANS_ROT270 6
#define TRANS_MIRROR 2
#define TRANS_MIRROR_ROT90 7
#define TRANS_MIRROR_ROT180 1
#define TRANS_MIRROR_ROT270 4

#define ANTICLOCKWISE_90 0
#define DEASIL_90 1
#define HORIZONTAL 2
#define VERTICAL 3

#define DIRECTION_NONE 0
#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

#define FRAME_HEADER_SIZE 8
#define FRAME_TILE_COUNT_BIT 1
#define FRAME_COLLISION_COUNT_BIT 2
#define FRAME_REFERENCE_POINT_COUNT_BIT 3
#define FRAME_TOP_POS_BIT 4
#define FRAME_BOTTOM_POS_BIT 5
#define FRAME_LEFT_POS_BIT 6
#define FRAME_RIGHT_POS_BIT 7

#define ACTION_HEADER_SIZE 4
#define ACTION_SEQUENCE_LENGTH_BIT 1
#define ACTION_SEQUENCE_DELAY_BIT 2
#define ACTION_TRANSFORM_BIT 3

#define COLLISION_INTERSECT 1
#define COLLISION_INCLUSION 2

#define INVERTED_AXES 0x4

typedef struct {
    char *spxName;
    char *imageName;
    short** actionData;
    int actionCount;
    short** frameData;
    int frameCount;
    short** tileData;
    int tileCount;
    int actionIndex;
    int sequenceIndex;
    BITMAP_565* image;
    BITMAP_565** tiles;
    int* tileUseCounter;
    bool tileMode;
    char* spxFileName;
    char* spxFileExt;
    int delay;
    int width;
    int height;
    int x;
    int y;
    bool visible;
    int32_t lastTime;
    bool firstUpdate;
    bool disableUpdate;
    int originOffsetX;
    int originOffsetY;
} SpriteX;

extern SpriteX* spxManager;
extern int spxManagerSize;

void removeAllSpx(void);
void addSpxToManage(SpriteX* spx);
SpriteX* createSpriteX(const char* spxName, BITMAP_565* image);
SpriteX* createSpriteXWithTiles(const char* spxName, BITMAP_565** tiles, int tileCount);
SpriteX* createSpriteXWithImageName(const char* spxName, const char* imageName);
SpriteX* createSpriteXCopy(SpriteX* spx);
void loadSpx(SpriteX* spx, const char* spxName, BITMAP_565* image);
void initAction(SpriteX* spx, int nAction);
void releaseAction(SpriteX* spx, int nAction);
void initAllAction(SpriteX* spx);
void releaseAllAction(SpriteX* spx);
const char* getSpxName(SpriteX* spx);
const char* getImageName(SpriteX* spx);
void enableUpdate(SpriteX* spx, bool enable);
bool isEnableUpdate(SpriteX* spx);
void setImage(SpriteX* spx, BITMAP_565* image);
BITMAP_565* getImage(SpriteX* spx);
BITMAP_565* getImageAtIndex(SpriteX* spx, int index);
void setX(SpriteX* spx, int x);
void setY(SpriteX* spx, int y);
void setPosition(SpriteX* spx, int x, int y);
void move(SpriteX* spx, int dx, int dy);
int getX(SpriteX* spx);
int getY(SpriteX* spx);
int getWidth(SpriteX* spx);
int getHeight(SpriteX* spx);
void setVisible(SpriteX* spx, bool visible);
bool isVisible(SpriteX* spx);
void setAction(SpriteX* spx, int actionIndex);
int getAction(SpriteX* spx);
int getActionCount(SpriteX* spx);
void setFrame(SpriteX* spx, int sequenceIndex);
int getFrame(SpriteX* spx);
void setTransform(SpriteX* spx, int transform);
int getTransform(SpriteX* spx);
void deasilRotate90(SpriteX* spx);
void anticlockwiseRotate90(SpriteX* spx);
void horizontalMirror(SpriteX* spx);
void verticalMirror(SpriteX* spx);
void nextFrame(SpriteX* spx);
void prevFrame(SpriteX* spx);
extern void update(SpriteX* spx, int32_t time);
void setDelay(SpriteX* spx, int delay);
int getDelay(SpriteX* spx);
void updateSprite(SpriteX* spx);
int getSequenceFrame(SpriteX* spx, int sequenceIndex);
int getSequenceFrameCurrent(SpriteX* spx);
int getSequenceLength(SpriteX* spx);
bool isDelay(SpriteX* spx);
int getDelayTime(SpriteX* spx);
int getReferencePointX(SpriteX* spx, int index);
int getReferencePointY(SpriteX* spx, int index);
int getReferencePointCount(SpriteX* spx, int frameIndex);
int getReferencePointCountCurrent(SpriteX* spx);
int getTransformedReferenceX(SpriteX* spx, int x, int y, int transform);
int getTransformedReferenceY(SpriteX* spx, int x, int y, int transform);
int getCollidesX(SpriteX* spx, int frame, int index);
int getCollidesY(SpriteX* spx, int frame, int index);
int getCollidesWidth(SpriteX* spx, int frame, int index);
int getCollidesHeight(SpriteX* spx, int frame, int index);
int getCollidesXCurrent(SpriteX* spx, int index);
int getCollidesYCurrent(SpriteX* spx, int index);
int getCollidesWidthCurrent(SpriteX* spx, int index);
int getCollidesHeightCurrent(SpriteX* spx, int index);
int getCollidesCount(SpriteX* spx, int index);
int getCollidesCountCurrent(SpriteX* spx);
bool collidesWith(SpriteX* spx, SpriteX* other, int spxCollides, int thisCollides, int type);
bool collidesWithImage(SpriteX* spx, BITMAP_565* image, int x, int y, int collides, int type);
bool collidesWithRect(SpriteX* spx, int x, int y, int width, int height, int collides, int type);
bool collidesWithSprite(SpriteX* spx, SpriteX* other);
bool collidesWithImageRect(SpriteX* spx, BITMAP_565* image, int x, int y);
bool collidesWithRectRect(SpriteX* spx, int x, int y, int width, int height);
bool intersectRect(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);
bool inclusionRect(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);
void originOffset(SpriteX* spx, int x, int y);
int getFrameTopPos(SpriteX* spx);
int getFrameBottomPos(SpriteX* spx);
int getFrameLeftPos(SpriteX* spx);
int getFrameRightPos(SpriteX* spx);
int getFrameWidth(SpriteX* spx);
int getFrameHeight(SpriteX* spx);
void paint(SpriteX* spx, BITMAP_565* g, int x, int y);
void paintCurrent(SpriteX* spx, BITMAP_565* g);
void drawRegion(BITMAP_565* g, BITMAP_565* src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor);

#endif // SPRITEX_H