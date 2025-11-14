#include <mrc_base.h>
#include "mrc_graphics.h"
#include "mpc.h"
#include "spritex.h"
#include "fopen.h"

#define SPX_HEADER 0x53505258 // SPRX
#define SPX_VERSION 34        // 3.3
#define SPX_BYTE_SEQUENCE_JAVA 1

// #define TRANS_NONE 0
// #define TRANS_ROT90 5
// #define TRANS_ROT180 3
// #define TRANS_ROT270 6
// #define TRANS_MIRROR 2
// #define TRANS_MIRROR_ROT90 7
// #define TRANS_MIRROR_ROT180 1
// #define TRANS_MIRROR_ROT270 4

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

SpriteX *spxManager = NULL;
int spxManagerSize = 0;


char* my_strdup(const char *s) {
    // 计算字符串的长度
    int length = 0;
    char *copy;
    int i;
    if (s == NULL) {
        return NULL; // 如果输入指针为空，返回NULL
    }
  
    
    while (s[length] != '\0') {
        length++;
    }

    // 为新字符串分配内存
    copy = (char *)malloc((length + 1) * sizeof(char)); // +1 为了存储 '\0'
    if (copy == NULL) {
        return NULL; // 如果内存分配失败，返回NULL
    }

    // 复制字符串内容
    for ( i = 0; i < length; i++) {
        copy[i] = s[i];
    }

    // 添加字符串结束符
    copy[length] = '\0';

    return copy; // 返回指向新字符串的指针
}

void removeAllSpx(void)
{
    int i;
    for ( i = 0; i < spxManagerSize; ++i)
    {
        free(spxManager + i);
    }
    free(spxManager);
    spxManager = NULL;
    spxManagerSize = 0;
}

void addSpxToManage(SpriteX *spx)
{
    int i;
    // 为 spxManager 申请新的内存
    SpriteX *newManager = malloc((spxManagerSize + 1) * sizeof(SpriteX *));
    
    // 检查内存是否分配成功
    if (newManager == NULL) {
        // 处理内存分配失败的情况
        return; // 或者进行其他错误处理
    }

    // 复制原有的内容到新数组中
    for ( i = 0; i < spxManagerSize; i++) {
        newManager[i] = spxManager[i];
    }

    // 将新的精灵加入到新的管理数组中
    newManager[spxManagerSize] = *spx;
    
    // 更新 spxManager
    free(spxManager); // 释放原有的内存
    spxManager = newManager; // 指向新的内存
    
    // 增加管理的精灵计数
    spxManagerSize++;
}

int getTransformTable(int y, int x)
{
    if (y < 0 || y > 7 || x < 0 || x > 7)
    {
        // 返回错误代码或者处理越界情况
        return -1; // -1表示无效输入
    }

    switch (y)
    {
    case 0:
        switch (x)
        {
        case 0:
            return TRANS_NONE;
        case 1:
            return TRANS_MIRROR_ROT180;
        case 2:
            return TRANS_MIRROR;
        case 3:
            return TRANS_ROT180;
        case 4:
            return TRANS_MIRROR_ROT270;
        case 5:
            return TRANS_ROT90;
        case 6:
            return TRANS_ROT270;
        case 7:
            return TRANS_MIRROR_ROT90;
        }
        break;
    case 1:
        switch (x)
        {
        case 0:
            return TRANS_MIRROR_ROT180;
        case 1:
            return TRANS_NONE;
        case 2:
            return TRANS_ROT180;
        case 3:
            return TRANS_MIRROR;
        case 4:
            return TRANS_ROT90;
        case 5:
            return TRANS_MIRROR_ROT270;
        case 6:
            return TRANS_MIRROR_ROT90;
        case 7:
            return TRANS_ROT270;
        }
        break;
    case 2:
        switch (x)
        {
        case 0:
            return TRANS_MIRROR;
        case 1:
            return TRANS_ROT180;
        case 2:
            return TRANS_NONE;
        case 3:
            return TRANS_MIRROR_ROT180;
        case 4:
            return TRANS_ROT270;
        case 5:
            return TRANS_MIRROR_ROT90;
        case 6:
            return TRANS_MIRROR_ROT270;
        case 7:
            return TRANS_ROT90;
        }
        break;
    case 3:
        switch (x)
        {
        case 0:
            return TRANS_ROT180;
        case 1:
            return TRANS_MIRROR;
        case 2:
            return TRANS_MIRROR_ROT180;
        case 3:
            return TRANS_NONE;
        case 4:
            return TRANS_MIRROR_ROT90;
        case 5:
            return TRANS_ROT270;
        case 6:
            return TRANS_ROT90;
        case 7:
            return TRANS_MIRROR_ROT270;
        }
        break;
    case 4:
        switch (x)
        {
        case 0:
            return TRANS_MIRROR_ROT270;
        case 1:
            return TRANS_ROT270;
        case 2:
            return TRANS_ROT90;
        case 3:
            return TRANS_MIRROR_ROT90;
        case 4:
            return TRANS_NONE;
        case 5:
            return TRANS_MIRROR;
        case 6:
            return TRANS_MIRROR_ROT180;
        case 7:
            return TRANS_ROT180;
        }
        break;
    case 5:
        switch (x)
        {
        case 0:
            return TRANS_ROT90;
        case 1:
            return TRANS_MIRROR_ROT90;
        case 2:
            return TRANS_MIRROR_ROT270;
        case 3:
            return TRANS_ROT270;
        case 4:
            return TRANS_MIRROR_ROT180;
        case 5:
            return TRANS_ROT180;
        case 6:
            return TRANS_NONE;
        case 7:
            return TRANS_MIRROR;
        }
        break;
    case 6:
        switch (x)
        {
        case 0:
            return TRANS_ROT270;
        case 1:
            return TRANS_MIRROR_ROT270;
        case 2:
            return TRANS_MIRROR_ROT90;
        case 3:
            return TRANS_ROT90;
        case 4:
            return TRANS_MIRROR;
        case 5:
            return TRANS_NONE;
        case 6:
            return TRANS_ROT180;
        case 7:
            return TRANS_MIRROR_ROT180;
        }
        break;
    case 7:
        switch (x)
        {
        case 0:
            return TRANS_MIRROR_ROT90;
        case 1:
            return TRANS_ROT90;
        case 2:
            return TRANS_ROT270;
        case 3:
            return TRANS_MIRROR_ROT270;
        case 4:
            return TRANS_ROT180;
        case 5:
            return TRANS_MIRROR_ROT180;
        case 6:
            return TRANS_MIRROR;
        case 7:
            return TRANS_NONE;
        }
        break;
    }

    return -1; // 确保有返回值，尽管这个情况不会发生
}

SpriteX *createSpriteX(const char *spxName, BITMAP_565 *image)
{
    SpriteX *spx = malloc(sizeof(SpriteX));
    memset(spx, 0, sizeof(SpriteX));
    loadSpx(spx, spxName, image);
    return spx;
}

SpriteX *createSpriteXWithTiles(const char *spxName, BITMAP_565 **tiles, int tileCount)
{
    SpriteX *spx = malloc(sizeof(SpriteX));
    memset(spx, 0, sizeof(SpriteX));
    loadSpx(spx, spxName, NULL);
    spx->tiles = tiles;
    spx->tileMode = TRUE;
    spx->tileCount = tileCount;

    // 使用 malloc 和 memset 替代 calloc
    spx->tileUseCounter = malloc(tileCount * sizeof(int));
    if (spx->tileUseCounter != NULL) {
        memset(spx->tileUseCounter, 0, tileCount * sizeof(int));
    }
    return spx;
}

SpriteX *createSpriteXWithImageName(const char *spxName, const char *imageName)
{
    SpriteX *spx = malloc(sizeof(SpriteX));
    memset(spx, 0, sizeof(SpriteX));
    spx->spxName = my_strdup(spxName);
    spx->imageName = my_strdup(imageName);
    spx->tileMode = FALSE;
    
    spx->image = readBitmap565FromAssets(imageName);
    mrc_printf("imageName = %s %p\n", imageName, spx->image);
    loadSpx(spx, spxName, spx->image);
    return spx;
}

SpriteX *createSpriteXCopy(SpriteX *spx)
{
    int i;
    SpriteX *newSpx = malloc(sizeof(SpriteX));
    memset(newSpx, 0, sizeof(SpriteX));
    newSpx->actionCount = spx->actionCount;
    newSpx->frameCount = spx->frameCount;
    newSpx->tileCount = spx->tileCount;
    newSpx->actionData = malloc(spx->actionCount * sizeof(short *));
    newSpx->frameData = malloc(spx->frameCount * sizeof(short *));
    newSpx->tileData = malloc(spx->tileCount * sizeof(short *));
    for (i = 0; i < spx->actionCount; i++)
    {
        newSpx->actionData[i] = malloc(spx->actionData[i][0] * sizeof(short));
        memcpy(newSpx->actionData[i], spx->actionData[i], spx->actionData[i][0] * sizeof(short));
    }
    for (i = 0; i < spx->frameCount; i++)
    {
        newSpx->frameData[i] = malloc(spx->frameData[i][0] * sizeof(short));
        memcpy(newSpx->frameData[i], spx->frameData[i], spx->frameData[i][0] * sizeof(short));
    }
    for (i = 0; i < spx->tileCount; i++)
    {
        newSpx->tileData[i] = malloc(4 * sizeof(short));
        memcpy(newSpx->tileData[i], spx->tileData[i], 4 * sizeof(short));
    }
    newSpx->actionIndex = spx->actionIndex;
    newSpx->sequenceIndex = spx->sequenceIndex;
    newSpx->image = spx->image;
    newSpx->x = spx->x;
    newSpx->y = spx->y;
    newSpx->visible = spx->visible;
    newSpx->lastTime = spx->lastTime;
    newSpx->firstUpdate = spx->firstUpdate;
    newSpx->disableUpdate = spx->disableUpdate;
    return newSpx;
}

int16 swap_int16(int16 val)
{
    return (val << 8) | (val >> 8);
}
int32 swap_int32(int32 val)
{
    return ((val >> 24) & 0x000000FF) | // 提取最高字节
           ((val >> 8) & 0x0000FF00) |  // 提取次高字节
           ((val << 8) & 0x00FF0000) |  // 提取次低字节
           ((val << 24) & 0xFF000000);  // 提取最低字节
}

int32 readInt(uint8 *buffer, int32 buffer_pos)
{
    int32 val = *(int32 *)(buffer + buffer_pos);
    return swap_int32(val);
}

int16 readShort(uint8 *buffer, int32 buffer_pos)
{
    int16 val = *(int16 *)(buffer + buffer_pos);
    return swap_int16(val);
}

uint8 readByte(uint8 *buffer, int32 buffer_pos)
{
    uint8 val = *(uint8 *)(buffer + buffer_pos);
    return val;
}

void writeInt(FILE *file, uint32 val)
{
    val = swap_int32(val);
    fwrite(&val, sizeof(uint32), 1, file);
}

void writeShort(FILE *file, uint16 val)
{
    val = swap_int16(val);
    fwrite(&val, sizeof(uint16), 1, file);
}

void writeByte(FILE *file, uint8 val)
{
    fwrite(&val, sizeof(uint8), 1, file);
}

void loadSpx(SpriteX *spx, const char *spxName, BITMAP_565 *image)
{
    int32 file_size;
    int32 buffer_pos;
    int header;
    uint8 *buffer;
    uint8 version;
    uint8 byteSequence;
    int i, j, k;
    int32 sequenceLength;
    int length;
    int frameTileCount;
    int collisionCount;
    int referencePointCount;
    uint8 delay;
    int offset;
    FILE *file = fopen(spxName, "rb");
    if (!file)
    {
        mrc_printf("Failed to open file: %s\n", spxName);
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    file_size = mrc_getLen(spxName);
    fseek(file, 0, SEEK_SET);
   mrc_printf("File size: %zu\n", file_size); // 输出文件大小

    // Read file into buffer
    buffer = malloc(file_size);
    fread(buffer, 1, file_size, file);
    fclose(file);

    // Initialize buffer position
    buffer_pos = 0;

    // Check SPX format
    header = readInt(buffer, buffer_pos);
    buffer_pos += sizeof(uint32);
   mrc_printf("SPX Header: %d\n", header); // 输出SPX头部
    if (header != SPX_HEADER)
    {
        mrc_printf("Invalid SpriteX format\n");
        free(buffer);
        return;
    }

    version = readByte(buffer, buffer_pos);
    buffer_pos += sizeof(uint8);
   mrc_printf("SPX Version: %d\n", version); // 输出SPX版本
    if (version != SPX_VERSION)
    {
        mrc_printf("Version no matching\n");
        free(buffer);
        return;
    }

    byteSequence = readByte(buffer, buffer_pos);
    buffer_pos += sizeof(uint8);
   mrc_printf("Byte Sequence: %d\n", byteSequence); // 输出字节序列
    if ((byteSequence & 1) != SPX_BYTE_SEQUENCE_JAVA)
    {
        mrc_printf("Byte sequence error\n");
        free(buffer);
        return;
    }

    if ((byteSequence & 2) != 0)
    {
        spx->tileMode = TRUE;
    }

    // Read tile count
    spx->tileCount = readInt(buffer, buffer_pos);
    buffer_pos += sizeof(uint32);
   mrc_printf("Tile Count: %d\n", spx->tileCount); // 输出瓦片数量

    if (!spx->tileMode)
    {
        spx->tileData = malloc(spx->tileCount * sizeof(short *));
        for (i = 0; i < spx->tileCount; i++)
        {
            spx->tileData[i] = malloc(4 * sizeof(short));
            for (j = 0; j < 4; j++)
            {
                spx->tileData[i][j] = readShort(buffer, buffer_pos+2*j);
            }
            buffer_pos += 8;
           mrc_printf("Tile %d: x=%d, y=%d, width=%d, height=%d\n", i, spx->tileData[i][0], spx->tileData[i][1], spx->tileData[i][2], spx->tileData[i][3]);
        }
    }

    // Read frame count
    spx->frameCount = readInt(buffer, buffer_pos);
    buffer_pos += sizeof(uint32);
   mrc_printf("Frame Count: %d\n", spx->frameCount); // 输出帧数量
    spx->frameData = malloc(spx->frameCount * sizeof(short *));
    memset(spx->frameData, 0, spx->frameCount * sizeof(short *));
    for (i = 0; i < spx->frameCount; i++)
    {
        frameTileCount = readInt(buffer, buffer_pos);
        buffer_pos += sizeof(uint32);
       mrc_printf("Tile Count: %d\n", frameTileCount);
        collisionCount = readInt(buffer, buffer_pos);
        buffer_pos += sizeof(uint32);
       mrc_printf("Collision Count: %d\n", collisionCount);
        referencePointCount = readInt(buffer, buffer_pos);
        buffer_pos += sizeof(uint32);
       mrc_printf("Reference Point Count: %d\n", referencePointCount);
        length = FRAME_HEADER_SIZE + frameTileCount * 4 + collisionCount * 4 + referencePointCount * 2;
        spx->frameData[i] = malloc(length * sizeof(short));
        spx->frameData[i][0] = length;
        spx->frameData[i][FRAME_TILE_COUNT_BIT] = frameTileCount;
        spx->frameData[i][FRAME_COLLISION_COUNT_BIT] = collisionCount;
        spx->frameData[i][FRAME_REFERENCE_POINT_COUNT_BIT] = referencePointCount;
       mrc_printf("Frame %d: length=%d, frameTileCount=%d, collisionCount=%d, referencePointCount=%d\n", i, length, frameTileCount, collisionCount, referencePointCount);
        // for (int j = FRAME_TOP_POS_BIT; j <= FRAME_RIGHT_POS_BIT; j++) {
        //     spx->frameData[i][j] = readShort(buffer, buffer_pos);
        //     buffer_pos += sizeof(uint16);
        // }
        // Read frame top pos
        spx->frameData[i][FRAME_TOP_POS_BIT] = readShort(buffer, buffer_pos);
        buffer_pos += 2;
        // 读取帧底部位置
        spx->frameData[i][FRAME_BOTTOM_POS_BIT] = readShort(buffer, buffer_pos);
        buffer_pos += 2;
        // 读取帧左侧位置
        spx->frameData[i][FRAME_LEFT_POS_BIT] = readShort(buffer, buffer_pos);
        buffer_pos += 2;
        // Read frame right pos
        spx->frameData[i][FRAME_RIGHT_POS_BIT] = readShort(buffer, buffer_pos);
        buffer_pos += 2;
        //xldebug
        mrc_printf("Frame %d: left=%d, right=%d\n", i, spx->frameData[i][FRAME_LEFT_POS_BIT], spx->frameData[i][FRAME_RIGHT_POS_BIT]);
        mrc_printf("Frame %d: top=%d, bottom=%d\n", i, spx->frameData[i][FRAME_TOP_POS_BIT], spx->frameData[i][FRAME_BOTTOM_POS_BIT]);
        
         offset = FRAME_HEADER_SIZE;
        for (j = 0; j < frameTileCount; j++)
        {
            for (k = 0; k < 4; k++)
            {
                spx->frameData[i][offset + k] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16);
            }
           mrc_printf("read frameTitleCount = %d %d %d %d\n", spx->frameData[i][0 + offset], spx->frameData[i][1 + offset], spx->frameData[i][2 + offset], spx->frameData[i][3 + offset]);
            offset += 4;
        }
        for (j = 0; j < collisionCount; j++)
        {
            for (k = 0; k < 4; k++)
            {
                spx->frameData[i][offset + k] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16);
            }
           mrc_printf("read frameTitleCount = %d %d %d %d\n", spx->frameData[i][0 + offset], spx->frameData[i][1 + offset], spx->frameData[i][2 + offset], spx->frameData[i][3 + offset]);
            offset += 4;
        }
        for (j = 0; j < referencePointCount; j++)
        {
            for (k = 0; k < 2; k++)
            {
                spx->frameData[i][offset + k] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16);
            }
           mrc_printf("read collisionCount = %d %d\n", spx->frameData[i][0 + offset], spx->frameData[i][1 + offset]);
            offset += 2;
        }
    }

    // Read action count
    spx->actionCount = readInt(buffer, buffer_pos);
    buffer_pos += sizeof(uint32);
   mrc_printf("Action Count: %d\n", spx->actionCount); // 输出动作数量
   mrc_printf("pos = %d\n", buffer_pos);
    spx->actionData = malloc(spx->actionCount * sizeof(short *));
    for (i = 0; i < spx->actionCount; i++)
    {
        sequenceLength = readInt(buffer, buffer_pos);

        buffer_pos += sizeof(uint32);
        delay = readByte(buffer, buffer_pos);
        buffer_pos += sizeof(uint8);
       mrc_printf("Action %d: sequenceLength=%d, delay=%d\n", i, sequenceLength, delay);
        length = 0;
        if (delay == 1)
        {
            length = ACTION_HEADER_SIZE + sequenceLength * 2;
        }
        else
        {
            length = ACTION_HEADER_SIZE + sequenceLength;
        }
       mrc_printf("read %d bytes", length);
        spx->actionData[i] = malloc(length * sizeof(short));
        spx->actionData[i][0] = length;
        spx->actionData[i][ACTION_SEQUENCE_LENGTH_BIT] = sequenceLength;
        spx->actionData[i][ACTION_SEQUENCE_DELAY_BIT] = delay;
        spx->actionData[i][ACTION_TRANSFORM_BIT] = readShort(buffer, buffer_pos);
        buffer_pos += 4;
       mrc_printf("readAction %d %d %d %d %d %d\n", i, spx->actionData[i][0], spx->actionData[i][1], spx->actionData[i][2], spx->actionData[i][3], spx->actionData[i][4]);
         offset = ACTION_HEADER_SIZE;
       mrc_printf("delay = %d\n", delay);
        if (delay == 1)
        {
            for (j = 0; j < sequenceLength; j++)
            {
                spx->actionData[i][offset] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16);
                spx->actionData[i][offset + 1] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16);
                offset += 2;
            }
        }
        else
        {
            for (j = 0; j < sequenceLength; j++)
            {
                spx->actionData[i][offset] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16);
                offset += 1;
            }
        }
       mrc_printf("pos = %d\n", buffer_pos);
    }

    // Set image
    setImage(spx, image);

    // Load complete
    spx->visible = TRUE;

    free(buffer);
}
void saveSpx(SpriteX *spx, const char *spxName)
{
    uint8 byteSequence;
    int i, j, k;
    int frameTileCount;
    int collisionCount;
    int referencePointCount;
    int sequenceLength;
    int offset;
    uint8 delay;
    FILE *file = fopen(spxName, "wb");
    if (!file)
    {
        mrc_printf("Failed to open file: %s\n", spxName);
        return;
    }

    // Write SPX header
    writeInt(file, SPX_HEADER);
    writeByte(file, SPX_VERSION);
    byteSequence = SPX_BYTE_SEQUENCE_JAVA;
    if (spx->tileMode)
    {
        byteSequence |= 2;
    }
    writeByte(file, byteSequence);

    // Write tile count
    writeInt(file, spx->tileCount);

    if (!spx->tileMode)
    {
        for (i = 0; i < spx->tileCount; i++)
        {
            for (j = 0; j < 4; j++)
            {
                writeShort(file, spx->tileData[i][j]);
            }
        }
    }

    // Write frame count
    writeInt(file, spx->frameCount);
    for (i = 0; i < spx->frameCount; i++)
    {
        frameTileCount = spx->frameData[i][FRAME_TILE_COUNT_BIT];
        collisionCount = spx->frameData[i][FRAME_COLLISION_COUNT_BIT];
        referencePointCount = spx->frameData[i][FRAME_REFERENCE_POINT_COUNT_BIT];
        writeInt(file, frameTileCount);
        writeInt(file, collisionCount);
        writeInt(file, referencePointCount);
        for (j = FRAME_TOP_POS_BIT; j <= FRAME_RIGHT_POS_BIT; j++)
        {
            writeShort(file, spx->frameData[i][j]);
        }

         offset = FRAME_HEADER_SIZE;
        for (j = 0; j < frameTileCount; j++)
        {
            for (k = 0; k < 4; k++)
            {
                writeShort(file, spx->frameData[i][offset + k]);
            }
            offset += 4;
        }
        for (j = 0; j < collisionCount; j++)
        {
            for (k = 0; k < 4; k++)
            {
                writeShort(file, spx->frameData[i][offset + k]);
            }
            offset += 4;
        }
        for (j = 0; j < referencePointCount; j++)
        {
            for (k = 0; k < 2; k++)
            {
                writeShort(file, spx->frameData[i][offset + k]);
            }
            offset += 2;
        }
    }

    // Write action count
    writeInt(file, spx->actionCount);
    for (i = 0; i < spx->actionCount; i++)
    {
        sequenceLength = spx->actionData[i][ACTION_SEQUENCE_LENGTH_BIT];
        delay = spx->actionData[i][ACTION_SEQUENCE_DELAY_BIT];
        writeInt(file, sequenceLength);
        writeByte(file, delay);
        writeShort(file, spx->actionData[i][ACTION_TRANSFORM_BIT]);

        offset = ACTION_HEADER_SIZE;
        if (delay == 1)
        {
            for (j = 0; j < sequenceLength; j++)
            {
                for (k = 0; k < 2; k++)
                {
                    writeShort(file, spx->actionData[i][offset + k]);
                }
                offset += 2;
            }
        }
        else
        {
            for (j = 0; j < sequenceLength; j++)
            {
                writeShort(file, spx->actionData[i][offset]);
                offset += 1;
            }
        }
    }

    fclose(file);
}

void initAction(SpriteX *spx, int nAction)
{
    int tile;
    int i, j;
    int frame;
    int frameTileCount;
    int frameDataOffset;
    int isDelay;
    int actionDataOffset;
    int sequenceLength;
    char fullName[256];
    int *tileUse;
    if (!spx->tileMode)
    {
        return;
    }

    tileUse = malloc(spx->tileCount * sizeof(int));
    if (tileUse != NULL) {
        memset(tileUse, 0, spx->tileCount * sizeof(int)); // 初始化内存为0
    }
    sequenceLength = spx->actionData[nAction][ACTION_SEQUENCE_LENGTH_BIT];
    isDelay = spx->actionData[nAction][ACTION_SEQUENCE_DELAY_BIT];
    actionDataOffset = ACTION_HEADER_SIZE;
    if (isDelay == 1)
    {
        for (i = 0; i < sequenceLength; ++i)
        {
            frame = spx->actionData[nAction][actionDataOffset];
            frameTileCount = spx->frameData[frame][FRAME_TILE_COUNT_BIT];
            frameDataOffset = FRAME_HEADER_SIZE;

            for ( j = 0; j < frameTileCount; ++j)
            {
                tile = spx->frameData[frame][frameDataOffset];
                frameDataOffset += 4;
                tileUse[tile] = 1;
            }
            actionDataOffset += 2;
        }
    }
    else
    {
        for (i = 0; i < sequenceLength; ++i)
        {
            frame = spx->actionData[nAction][actionDataOffset];
            frameTileCount = spx->frameData[frame][FRAME_TILE_COUNT_BIT];
            frameDataOffset = FRAME_HEADER_SIZE;

            for (j = 0; j < frameTileCount; ++j)
            {
                tile = spx->frameData[frame][frameDataOffset];
                frameDataOffset += 4;
                tileUse[tile] = 1;
            }
            ++actionDataOffset;
        }
    }

    for (i = 0; i < spx->tileCount; i++)
    {
        if (tileUse[i] == 1)
        {
            if (spx->tiles[i] == NULL)
            {
                mrc_sprintf(fullName, "%s%d%s", spx->spxFileName, i, spx->spxFileExt);
                spx->tiles[i] = readBitmap(fullName);
            }
            ++spx->tileUseCounter[i];
        }
    }

    free(tileUse);
}

void releaseAction(SpriteX *spx, int nAction)
{
    int tile;
    int i, j;
    int frame;
    int frameTileCount;
    int frameDataOffset;
    int isDelay;
    int actionDataOffset;
    int sequenceLength;
    int *tileUse;
    if (!spx->tileMode)
    {
        return;
    }

    tileUse = malloc(spx->tileCount * sizeof(int));
    if (tileUse != NULL) {
        mrc_memset(tileUse, 0, spx->tileCount * sizeof(int));
    } 

    sequenceLength = spx->actionData[nAction][ACTION_SEQUENCE_LENGTH_BIT];
    isDelay = spx->actionData[nAction][ACTION_SEQUENCE_DELAY_BIT];
    actionDataOffset = ACTION_HEADER_SIZE;
    if (isDelay == 1)
    {
        for (i = 0; i < sequenceLength; ++i)
        {
            frame = spx->actionData[nAction][actionDataOffset];
            frameTileCount = spx->frameData[frame][FRAME_TILE_COUNT_BIT];
            frameDataOffset = FRAME_HEADER_SIZE;

            for (j = 0; j < frameTileCount; ++j)
            {
                tile = spx->frameData[frame][frameDataOffset];
                frameDataOffset += 4;
                tileUse[tile] = 1;
            }
            actionDataOffset += 2;
        }
    }
    else
    {
        for (i = 0; i < sequenceLength; ++i)
        {
            frame = spx->actionData[nAction][actionDataOffset];
            frameTileCount = spx->frameData[frame][FRAME_TILE_COUNT_BIT];
            frameDataOffset = FRAME_HEADER_SIZE;

            for (j = 0; j < frameTileCount; ++j)
            {
                tile = spx->frameData[frame][frameDataOffset];
                frameDataOffset += 4;
                tileUse[tile] = 1;
            }
            ++actionDataOffset;
        }
    }

    for (i = 0; i < spx->tileCount; i++)
    {
        if (spx->tiles[i] != NULL)
        {
            --spx->tileUseCounter[i];
            if (spx->tileUseCounter[i] <= 0)
            {
                bitmapFree(spx->tiles[i]);
                spx->tiles[i] = NULL;
            }
        }
    }

    free(tileUse);
}

void initAllAction(SpriteX *spx)
{
    int i;
    char fullName[256];
    if (!spx->tileMode)
    {
        return;
    }

    for (i = 0; i < spx->tileCount; i++)
    {
        if (spx->tiles[i] == NULL)
        {
            mrc_sprintf(fullName, "%s%d%s", spx->spxFileName, i, spx->spxFileExt);
            spx->tiles[i] = readBitmap(fullName);
        }
        spx->tileUseCounter[i] = 1;
    }
}

void releaseAllAction(SpriteX *spx)
{
    int i;
    if (!spx->tileMode)
    {
        return;
    }

    for (i = 0; i < spx->tileCount; i++)
    {
        if (spx->tiles[i] != NULL)
        {
            bitmapFree(spx->tiles[i]);
            spx->tiles[i] = NULL;
        }
        spx->tileUseCounter[i] = 0;
    }
}

const char *getSpxName(SpriteX *spx)
{
    return spx->spxName;
}

const char *getImageName(SpriteX *spx)
{
    return spx->imageName;
}

void enableUpdate(SpriteX *spx, int enable)
{
    if (spx->disableUpdate && enable)
    {
        spx->lastTime = mrc_getUptime();
    }
    spx->disableUpdate = !enable;
}

int isEnableUpdate(SpriteX *spx)
{
    return !spx->disableUpdate;
}

void setImage(SpriteX *spx, BITMAP_565 *image)
{
    spx->image = image;
}

BITMAP_565 *getImage(SpriteX *spx)
{
    return spx->image;
}

BITMAP_565 *getImageAtIndex(SpriteX *spx, int index)
{
    return spx->tiles[index];
}

void setX(SpriteX *spx, int x)
{
    spx->x = x;
}

void setY(SpriteX *spx, int y)
{
    spx->y = y;
}

void setPosition(SpriteX *spx, int x, int y)
{
    spx->x = x;
    spx->y = y;
}

void move(SpriteX *spx, int dx, int dy)
{
    spx->x += dx;
    spx->y += dy;
}

int getX(SpriteX *spx)
{
    return spx->x;
}

int getY(SpriteX *spx)
{
    return spx->y;
}

int getWidth(SpriteX *spx)
{
    return spx->width;
}

int getHeight(SpriteX *spx)
{
    return spx->height;
}

void setVisible(SpriteX *spx, int visible)
{
    spx->visible = visible;
}

int isVisible(SpriteX *spx)
{
    return spx->visible;
}

void setAction(SpriteX *spx, int actionIndex)
{
    if (actionIndex == spx->actionIndex)
    {
        return;
    }
    if (actionIndex < 0 || actionIndex >= spx->actionCount)
    {
        mrc_printf("IndexOutOfBoundsException\n");
        return;
    }
    spx->actionIndex = actionIndex;
    spx->sequenceIndex = 0;
    spx->firstUpdate = FALSE;
}

int getAction(SpriteX *spx)
{
    return spx->actionIndex;
}

int getActionCount(SpriteX *spx)
{
    return spx->actionCount;
}

void setFrame(SpriteX *spx, int sequenceIndex)
{
    if (sequenceIndex < 0 || sequenceIndex >= getSequenceLength(spx))
    {
        mrc_printf("IndexOutOfBoundsException\n");
        return;
    }
    spx->sequenceIndex = sequenceIndex;
    spx->firstUpdate = FALSE;
}

int getFrame(SpriteX *spx)
{
    return spx->sequenceIndex;
}

void setTransform(SpriteX *spx, int transform)
{
    if (transform < 0 || transform > 7)
    {
        mrc_printf("IllegalArgumentException\n");
        return;
    }
    spx->actionData[spx->actionIndex][ACTION_TRANSFORM_BIT] = transform;
}

int getTransform(SpriteX *spx)
{
    return spx->actionData[spx->actionIndex][ACTION_TRANSFORM_BIT];
}

void deasilRotate90(SpriteX *spx)
{
    setTransform(spx, getTransformTable(getTransform(spx), DEASIL_90));
}

void anticlockwiseRotate90(SpriteX *spx)
{
    setTransform(spx, getTransformTable(getTransform(spx), ANTICLOCKWISE_90));
}

void horizontalMirror(SpriteX *spx)
{
    setTransform(spx, getTransformTable(getTransform(spx), HORIZONTAL));
}

void verticalMirror(SpriteX *spx)
{
    setTransform(spx, getTransformTable(getTransform(spx), VERTICAL));
}

void nextFrame(SpriteX *spx)
{
    spx->sequenceIndex = (spx->sequenceIndex + 1) % getSequenceLength(spx);
    spx->firstUpdate = FALSE;
}

void prevFrame(SpriteX *spx)
{
    if (spx->sequenceIndex == 0)
    {
        spx->sequenceIndex = getSequenceLength(spx) - 1;
    }
    else
    {
        spx->sequenceIndex--;
    }
    spx->firstUpdate = FALSE;
}

void update(SpriteX *spx, int32 time)
{
    int dms;
    if (!isEnableUpdate(spx))
    {
        return;
    }
    if (isDelay(spx))
    {
        if (!spx->firstUpdate)
        {
            spx->firstUpdate = TRUE;
            spx->lastTime = time;
        }
         dms = getDelayTime(spx);
        if ((time - spx->lastTime) >= dms)
        {
            nextFrame(spx);
            spx->lastTime = time;
        }
    }
    else
    {
        if (!spx->firstUpdate)
        {
            spx->firstUpdate = TRUE;
            spx->lastTime = time;
        }
         dms = spx->delay;
        if ((time - spx->lastTime) >= dms)
        {
            nextFrame(spx);
            spx->lastTime = time;
        }
    }
}

void setDelay(SpriteX *spx, int delay)
{
    spx->delay = delay;
}

int getDelay(SpriteX *spx)
{
    return spx->delay;
}

void updateSprite(SpriteX *spx)
{
    if (!isEnableUpdate(spx))
    {
        return;
    }
    update(spx, mrc_getUptime());
}

int getSequenceFrame(SpriteX *spx, int sequenceIndex)
{
    if (isDelay(spx))
    {
        return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + sequenceIndex * 2];
    }
    else
    {
        return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + sequenceIndex];
    }
}

int getSequenceFrameCurrent(SpriteX *spx)
{
    if (isDelay(spx))
    {
        return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + spx->sequenceIndex * 2];
    }
    else
    {
        return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + spx->sequenceIndex];
    }
}

int getSequenceLength(SpriteX *spx)
{
    return spx->actionData[spx->actionIndex][ACTION_SEQUENCE_LENGTH_BIT];
}

int isDelay(SpriteX *spx)
{
    return spx->actionData[spx->actionIndex][ACTION_SEQUENCE_DELAY_BIT] == 1;
}

int getDelayTime(SpriteX *spx)
{
    return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + spx->sequenceIndex * 2 + 1];
}

int getReferencePointX(SpriteX *spx, int index)
{
    int frameIndex = getSequenceFrameCurrent(spx);
    int frameTileCount = spx->frameData[frameIndex][FRAME_TILE_COUNT_BIT];
    int frameCollisionCount = spx->frameData[frameIndex][FRAME_COLLISION_COUNT_BIT];
    int offset = FRAME_HEADER_SIZE + frameTileCount * 4 + frameCollisionCount * 4 + index * 2;
    int refX = spx->frameData[frameIndex][offset];
    int refY = spx->frameData[frameIndex][offset + 1];
    return getTransformedReferenceX(spx, refX, refY, getTransform(spx)) + spx->x;
}

int getReferencePointY(SpriteX *spx, int index)
{
    int frameIndex = getSequenceFrameCurrent(spx);
    int frameTileCount = spx->frameData[frameIndex][FRAME_TILE_COUNT_BIT];
    int frameCollisionCount = spx->frameData[frameIndex][FRAME_COLLISION_COUNT_BIT];
    int offset = FRAME_HEADER_SIZE + frameTileCount * 4 + frameCollisionCount * 4 + index * 2;
    int refX = spx->frameData[frameIndex][offset];
    int refY = spx->frameData[frameIndex][offset + 1];
    return getTransformedReferenceY(spx, refX, refY, getTransform(spx)) + spx->y;
}

int getReferencePointCount(SpriteX *spx, int frameIndex)
{
    if (frameIndex < 0 || frameIndex >= spx->frameCount)
    {
        mrc_printf("ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    return spx->frameData[frameIndex][FRAME_REFERENCE_POINT_COUNT_BIT];
}

int getReferencePointCountCurrent(SpriteX *spx)
{
    int frameIndex = getSequenceFrameCurrent(spx);
    return getReferencePointCount(spx, frameIndex);
}

int getTransformedReferenceX(SpriteX *spx, int x, int y, int transform)
{
    int t_x = 0;
    switch (transform)
    {
    case TRANS_NONE:
        t_x = x;
        break;
    case TRANS_MIRROR:
        t_x = -x;
        break;
    }
    return t_x;
}

int getTransformedReferenceY(SpriteX *spx, int x, int y, int transform)
{
    int t_y = 0;
    switch (transform)
    {
    case TRANS_NONE:
        t_y = y;
        break;
    case TRANS_MIRROR:
        t_y = y;
        break;
    }
    return t_y;
}

int getCollidesX(SpriteX *spx, int frame, int index)
{
    int offset;
    if (index < 0 || index >= spx->frameData[frame][FRAME_COLLISION_COUNT_BIT])
    {
        mrc_printf("ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    offset = FRAME_HEADER_SIZE + spx->frameData[frame][FRAME_TILE_COUNT_BIT] * 4 + index * 4;
    return spx->frameData[frame][offset] + spx->x;
}

int getCollidesY(SpriteX *spx, int frame, int index)
{
    int offset;
    if (index < 0 || index >= spx->frameData[frame][FRAME_COLLISION_COUNT_BIT])
    {
        mrc_printf("ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    offset = FRAME_HEADER_SIZE + spx->frameData[frame][FRAME_TILE_COUNT_BIT] * 4 + index * 4 + 1;
    return spx->frameData[frame][offset] + spx->y;
}

int getCollidesWidth(SpriteX *spx, int frame, int index)
{
    int offset;
    if (index < 0 || index >= spx->frameData[frame][FRAME_COLLISION_COUNT_BIT])
    {
        mrc_printf("ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    offset = FRAME_HEADER_SIZE + spx->frameData[frame][FRAME_TILE_COUNT_BIT] * 4 + index * 4 + 2;
    return spx->frameData[frame][offset];
}

int getCollidesHeight(SpriteX *spx, int frame, int index)
{
    int offset;
    if (index < 0 || index >= spx->frameData[frame][FRAME_COLLISION_COUNT_BIT])
    {
        mrc_printf("ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    offset = FRAME_HEADER_SIZE + spx->frameData[frame][FRAME_TILE_COUNT_BIT] * 4 + index * 4 + 3;
    return spx->frameData[frame][offset];
}

int getCollidesXCurrent(SpriteX *spx, int index)
{
    int frameIndex = getSequenceFrameCurrent(spx);
    return getCollidesX(spx, frameIndex, index);
}

int getCollidesYCurrent(SpriteX *spx, int index)
{
    int frameIndex = getSequenceFrameCurrent(spx);
    return getCollidesY(spx, frameIndex, index);
}

int getCollidesWidthCurrent(SpriteX *spx, int index)
{
    int frameIndex = getSequenceFrameCurrent(spx);
    return getCollidesWidth(spx, frameIndex, index);
}

int getCollidesHeightCurrent(SpriteX *spx, int index)
{
    int frameIndex = getSequenceFrameCurrent(spx);
    return getCollidesHeight(spx, frameIndex, index);
}

int getCollidesCount(SpriteX *spx, int index)
{
    return spx->frameData[index][FRAME_COLLISION_COUNT_BIT];
}

int getCollidesCountCurrent(SpriteX *spx)
{
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_COLLISION_COUNT_BIT];
}

int collidesWith(SpriteX *spx, SpriteX *other, int spxCollides, int thisCollides, int type)
{
    int x;
    int y;
    int width;
    int height;
    if (!other->visible || !spx->visible)
    {
        return FALSE;
    }
    x = getCollidesXCurrent(other, spxCollides);
    y = getCollidesYCurrent(other, spxCollides);
    width = getCollidesWidthCurrent(other, spxCollides);
    height = getCollidesHeightCurrent(other, spxCollides);
    return collidesWithRect(spx, x, y, width, height, thisCollides, type);
}

int collidesWithImage(SpriteX *spx, BITMAP_565 *image, int x, int y, int collides, int type)
{
    return collidesWithRect(spx, x, y, image->width, image->height, collides, type);
}

int collidesWithRect(SpriteX *spx, int x, int y, int width, int height, int collides, int type)
{
    int x1;
    int y1;
    int w1;
    int h1;
    if (!spx->visible)
    {
        return FALSE;
    }
    x1 = getCollidesXCurrent(spx, collides);
    y1 = getCollidesYCurrent(spx, collides);
    w1 = getCollidesWidthCurrent(spx, collides);
    h1 = getCollidesHeightCurrent(spx, collides);
    if (type == COLLISION_INTERSECT)
    {
        return intersectRect(x1, y1, w1, h1, x, y, width, height);
    }
    else if (type == COLLISION_INCLUSION)
    {
        return inclusionRect(x1, y1, w1, h1, x, y, width, height);
    }
    else
    {
        mrc_printf("ArithmeticException\n");
        return FALSE;
    }
}

int collidesWithSprite(SpriteX *spx, SpriteX *other)
{
    if (!other->visible || !spx->visible)
    {
        return FALSE;
    }
    if (getCollidesCountCurrent(spx) == 0 || getCollidesCountCurrent(other) == 0)
    {
        return FALSE;
    }
    return collidesWith(spx, other, 0, 0, COLLISION_INTERSECT);
}

int collidesWithImageRect(SpriteX *spx, BITMAP_565 *image, int x, int y)
{
    if (!spx->visible)
    {
        return FALSE;
    }
    if (getCollidesCountCurrent(spx) == 0)
    {
        return FALSE;
    }
    return collidesWithImage(spx, image, x, y, 0, COLLISION_INTERSECT);
}

int collidesWithRectRect(SpriteX *spx, int x, int y, int width, int height)
{
    if (!spx->visible)
    {
        return FALSE;
    }
    if (getCollidesCountCurrent(spx) == 0)
    {
        return FALSE;
    }
    return collidesWithRect(spx, x, y, width, height, 0, COLLISION_INTERSECT);
}

int intersectRect(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
    if (y2 + height2 < y1 || y2 > y1 + height1 || x2 + width2 < x1 || x2 > x1 + width1)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

int inclusionRect(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
    if (y2 >= y1 && y2 + height2 <= y1 + height1 && x2 >= x1 && x2 + width2 <= x1 + width1)
    {
        return TRUE;
    }
    return FALSE;
}

void originOffset(SpriteX *spx, int x, int y)
{
    spx->originOffsetX = x;
    spx->originOffsetY = y;
}

int getFrameTopPos(SpriteX *spx)
{
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_TOP_POS_BIT] + spx->y;
}

int getFrameBottomPos(SpriteX *spx)
{
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_BOTTOM_POS_BIT] + spx->y;
}

int getFrameLeftPos(SpriteX *spx)
{
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_LEFT_POS_BIT] + spx->x;
}

int getFrameRightPos(SpriteX *spx)
{
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_RIGHT_POS_BIT] + spx->x;
}

int getFrameWidth(SpriteX *spx)
{
    return getFrameRightPos(spx) - getFrameLeftPos(spx);
}

int getFrameHeight(SpriteX *spx)
{
    return getFrameBottomPos(spx) - getFrameTopPos(spx);
}

void paint(SpriteX *spx, BITMAP_565 *g, int x, int y)
{
    int dw;
    int dh;
    int clipX;
    int clipY;
    int clipWidth;
    int clipHeight;
    int tileIndex; // tile index
    int dx;        // tile dest x
    int dy;        // tile dest y
    int transform; // tile transform
    int tx;        // tile x
    int ty;        // tile y
    int tw;        // tile width
    int th;        // tile height
    int offset;
    int i;
    int actionTransform;
    int frameIndex;
    int tileCount;
    int cx;
    int cy;
    char temp[10];
    int j;
    char *strtemp = malloc(10000);
    memset(strtemp, 0, 10000);

    if (g == NULL)
    {
        mrc_printf("NullPointerException\n");
        return;
    }
    mrc_printf("Starting paint function\n");

    if (spx->visible)
    {
        x -= spx->originOffsetX;
        y -= spx->originOffsetY;

        clipX = g->width;
        clipY = g->height;
        clipWidth = g->width;
        clipHeight = g->height;

        actionTransform = getTransform(spx);
        mrc_printf("Action Transform: %d\n", actionTransform);
        mrc_printf("x=%d,y=%d\n", x, y);
        if (actionTransform == TRANS_NONE)
        {
            frameIndex = getSequenceFrameCurrent(spx);
            tileCount = spx->frameData[frameIndex][FRAME_TILE_COUNT_BIT];
            offset = FRAME_HEADER_SIZE; // tile data offset
            mrc_printf("Frame Index: %d, Tile Count: %d\n", frameIndex, tileCount);
            
            for (i = 0; i < tileCount; i++)
            {
                if (spx->tileMode)
                {
                    tileIndex = spx->frameData[frameIndex][0 + offset];
                    dx = spx->frameData[frameIndex][1 + offset] + x;
                    dy = spx->frameData[frameIndex][2 + offset] + y;
                    transform = spx->frameData[frameIndex][3 + offset];
                    tx = 0;
                    ty = 0;
                    tw = spx->tiles[tileIndex]->width;
                    th = spx->tiles[tileIndex]->height;
                    mrc_printf("tileIndex = %d, tw = %d, th = %d\n", tileIndex, tw, th);
                    if ((transform & INVERTED_AXES) != 0)
                    {
                        dw = th;
                        dh = tw;
                    }
                    else
                    {
                        dw = tw;
                        dh = th;
                    }

                    mrc_printf("Tile Mode: %d, tileIndex: %d, dx: %d, dy: %d, dw: %d, dh: %d\n", 
                               spx->tileMode, tileIndex, dx, dy, dw, dh);
                    
                    if (intersectRect(dx, dy, dw, dh, clipX, clipY, clipWidth, clipHeight))
                    {
                        drawRegion(g, spx->tiles[tileIndex], tx, ty, tw, th, transform, dx, dy, GRAPHICS_TOP | GRAPHICS_LEFT);
                    }
                }
                else
                {
                    tileIndex = spx->frameData[frameIndex][0 + offset];
                    dx = spx->frameData[frameIndex][1 + offset] + x;
                    dy = spx->frameData[frameIndex][2 + offset] + y;
                    transform = spx->frameData[frameIndex][3 + offset];
                    tx = spx->tileData[tileIndex][0];
                    ty = spx->tileData[tileIndex][1];
                    //xldebug
                    
                    tw = spx->tileData[tileIndex][2];
                    th = spx->tileData[tileIndex][3];
                    
                    mrc_printf("tileData = %d %d %d %d\n", tx, ty, tw, th);
                    mrc_printf("tileIndex = %d\n", tileIndex);
                    for(j=0;j<4;j++){
                        mrc_sprintf(temp, "%d ", spx->tileData[tileIndex][j]);
                        strcat(strtemp, temp);
                    }
                    
                    mrc_printf("tileData = %s\n", strtemp);

                    if ((transform & INVERTED_AXES) != 0)
                    {
                        dw = th;
                        dh = tw;
                    }
                    else
                    {
                        dw = tw;
                        dh = th;
                    }

                    mrc_printf("Non-Tile Mode: tileIndex: %d, dx: %d, dy: %d, dw: %d, dh: %d\n", 
                               tileIndex, dx, dy, dw, dh);

                    // if (intersectRect(dx, dy, dw, dh, clipX, clipY, clipWidth, clipHeight))
                    // {
                        drawRegion(g, spx->image, tx, ty, tw, th, transform, dx, dy, GRAPHICS_TOP | GRAPHICS_LEFT);
                    // }
                }
                offset += 4;
            }
        }
        else
        {
            frameIndex = getSequenceFrameCurrent(spx);
            tileCount = spx->frameData[frameIndex][FRAME_TILE_COUNT_BIT];
            offset = FRAME_HEADER_SIZE; // tile data offset
            mrc_printf("Transform case. Frame Index: %d, Tile Count: %d\n", frameIndex, tileCount);

            for (i = 0; i < tileCount; i++)
            {
                if (spx->tileMode)
                {
                    tileIndex = spx->frameData[frameIndex][0 + offset];
                    dx = spx->frameData[frameIndex][1 + offset] + x;
                    dy = spx->frameData[frameIndex][2 + offset] + y;
                    transform = spx->frameData[frameIndex][3 + offset];
                    tx = 0;
                    ty = 0;
                    tw = spx->tiles[tileIndex]->width;
                    th = spx->tiles[tileIndex]->height;

                    if ((transform & INVERTED_AXES) != 0)
                    {
                        dw = th;
                        dh = tw;
                    }
                    else
                    {
                        dw = tw;
                        dh = th;
                    }

                    cx = x - dx;
                    cy = y - dy;
                    // finally transform
                    transform = getTransformTable(transform, actionTransform);
                    mrc_printf("Transform: %d, Adjusted dx: %d, dy: %d\n", transform, dx, dy);

                    // adjust coordinate
                    switch (actionTransform)
                    {
                    case TRANS_NONE:
                        break;
                    case TRANS_ROT90:
                        dx = x - (dh - cy);
                        dy = y - cx;
                        break;
                    case TRANS_ROT180:
                        dx = x - (dw - cx);
                        dy = y - (dh - cy);
                        break;
                    case TRANS_ROT270:
                        dx = x - cy;
                        dy = y - (dw - cx);
                        break;
                    case TRANS_MIRROR:
                        dx = x - (dw - cx);
                        break;
                    case TRANS_MIRROR_ROT90:
                        dx = x - (dh - cy);
                        dy = y - (dw - cx);
                        break;
                    case TRANS_MIRROR_ROT180:
                        dy = y - (dh - cy);
                        break;
                    case TRANS_MIRROR_ROT270:
                        dx = x - cy;
                        dy = y - cx;
                        break;
                    }

                    if ((transform & INVERTED_AXES) != 0)
                    {
                        dw = th;
                        dh = tw;
                    }
                    else
                    {
                        dw = tw;
                        dh = th;
                    }

                    if (intersectRect(dx, dy, dw, dh, clipX, clipY, clipWidth, clipHeight))
                    {
                        drawRegion(g, spx->tiles[tileIndex], tx, ty, tw, th, transform, dx, dy, GRAPHICS_TOP | GRAPHICS_LEFT);
                    }
                }
                else
                {
                    tileIndex = spx->frameData[frameIndex][0 + offset];
                    dx = spx->frameData[frameIndex][1 + offset] + x;
                    dy = spx->frameData[frameIndex][2 + offset] + y;
                    transform = spx->frameData[frameIndex][3 + offset];
                    tx = spx->tileData[tileIndex][0];
                    ty = spx->tileData[tileIndex][1];
                    tw = spx->tileData[tileIndex][2];
                    th = spx->tileData[tileIndex][3];

                    if ((transform & INVERTED_AXES) != 0)
                    {
                        dw = th;
                        dh = tw;
                    }
                    else
                    {
                        dw = tw;
                        dh = th;
                    }

                    cx = x - dx;
                    cy = y - dy;
                    transform = getTransformTable(transform, actionTransform);
                    mrc_printf("Non-Tile Transform: %d, Adjusted dx: %d, dy: %d\n", transform, dx, dy);

                    switch (actionTransform)
                    {
                    case TRANS_NONE:
                        break;
                    case TRANS_ROT90:
                        dx = x - (dh - cy);
                        dy = y - cx;
                        break;
                    case TRANS_ROT180:
                        dx = x - (dw - cx);
                        dy = y - (dh - cy);
                        break;
                    case TRANS_ROT270:
                        dx = x - cy;
                        dy = y - (dw - cx);
                        break;
                    case TRANS_MIRROR:
                        dx = x - (dw - cx);
                        break;
                    case TRANS_MIRROR_ROT90:
                        dx = x - (dh - cy);
                        dy = y - (dw - cx);
                        break;
                    case TRANS_MIRROR_ROT180:
                        dy = y - (dh - cy);
                        break;
                    case TRANS_MIRROR_ROT270:
                        dx = x - cy;
                        dy = y - cx;
                        break;
                    }

                    if ((transform & INVERTED_AXES) != 0)
                    {
                        dw = th;
                        dh = tw;
                    }
                    else
                    {
                        dw = tw;
                        dh = th;
                    }

                    if (intersectRect(dx, dy, dw, dh, clipX, clipY, clipWidth, clipHeight))
                    {
                        drawRegion(g, spx->image, tx, ty, tw, th, transform, dx, dy, GRAPHICS_TOP | GRAPHICS_LEFT);
                    }
                }
                offset += 4;
            }
        }
    }
    mrc_printf("Finished paint function\n");
}
void paintCurrent(SpriteX *spx, BITMAP_565 *g)
{
    paint(spx, g, spx->x, spx->y);
}

void drawRegion(BITMAP_565 *g, BITMAP_565 *src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor)
{
    mrc_printf("drawRegion: x_src=%d, y_src=%d, width=%d, height=%d, transform=%d, x_dest=%d, y_dest=%d, anchor=%d\n", x_src, y_src, width, height, transform, x_dest, y_dest, anchor);
    drawBitmapRegion(src, x_src, y_src, width, height, transform, x_dest, y_dest, anchor);
}










