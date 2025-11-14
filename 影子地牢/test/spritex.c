#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// #include "mrc_graphics.h"
#include "spritex.h"

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


SpriteX* spxManager = NULL;
int spxManagerSize = 0;

int32_t mrc_getUptime(){
    return 0;
}

BITMAP_565 *readBitmap(const char *filename){
    BITMAP_565 *bmp = malloc(sizeof(BITMAP_565));
    memset(bmp, 0, sizeof(BITMAP_565));
    return bmp;
}

void bitmapFree(BITMAP_565 *bmp){
    free(bmp);
}

void removeAllSpx(void) {
    for (int i = 0; i < spxManagerSize; ++i) {
        free(spxManager+i);
    }
    free(spxManager);
    spxManager = NULL;
    spxManagerSize = 0;
}


void addSpxToManage(SpriteX* spx) {
    spxManager = realloc(spxManager, (spxManagerSize + 1) * sizeof(SpriteX*));
    *(spxManager+spxManagerSize++) = *spx;
}

int getTransformTable(int y, int x) {
    if (y < 0 || y > 7 || x < 0 || x > 7) {
        // 返回错误代码或者处理越界情况
        return -1; // -1表示无效输入
    }

    switch (y) {
        case 0:
            switch (x) {
                case 0: return TRANS_NONE;
                case 1: return TRANS_MIRROR_ROT180;
                case 2: return TRANS_MIRROR;
                case 3: return TRANS_ROT180;
                case 4: return TRANS_MIRROR_ROT270;
                case 5: return TRANS_ROT90;
                case 6: return TRANS_ROT270;
                case 7: return TRANS_MIRROR_ROT90;
            }
            break;
        case 1:
            switch (x) {
                case 0: return TRANS_MIRROR_ROT180;
                case 1: return TRANS_NONE;
                case 2: return TRANS_ROT180;
                case 3: return TRANS_MIRROR;
                case 4: return TRANS_ROT90;
                case 5: return TRANS_MIRROR_ROT270;
                case 6: return TRANS_MIRROR_ROT90;
                case 7: return TRANS_ROT270;
            }
            break;
        case 2:
            switch (x) {
                case 0: return TRANS_MIRROR;
                case 1: return TRANS_ROT180;
                case 2: return TRANS_NONE;
                case 3: return TRANS_MIRROR_ROT180;
                case 4: return TRANS_ROT270;
                case 5: return TRANS_MIRROR_ROT90;
                case 6: return TRANS_MIRROR_ROT270;
                case 7: return TRANS_ROT90;
            }
            break;
        case 3:
            switch (x) {
                case 0: return TRANS_ROT180;
                case 1: return TRANS_MIRROR;
                case 2: return TRANS_MIRROR_ROT180;
                case 3: return TRANS_NONE;
                case 4: return TRANS_MIRROR_ROT90;
                case 5: return TRANS_ROT270;
                case 6: return TRANS_ROT90;
                case 7: return TRANS_MIRROR_ROT270;
            }
            break;
        case 4:
            switch (x) {
                case 0: return TRANS_MIRROR_ROT270;
                case 1: return TRANS_ROT270;
                case 2: return TRANS_ROT90;
                case 3: return TRANS_MIRROR_ROT90;
                case 4: return TRANS_NONE;
                case 5: return TRANS_MIRROR;
                case 6: return TRANS_MIRROR_ROT180;
                case 7: return TRANS_ROT180;
            }
            break;
        case 5:
            switch (x) {
                case 0: return TRANS_ROT90;
                case 1: return TRANS_MIRROR_ROT90;
                case 2: return TRANS_MIRROR_ROT270;
                case 3: return TRANS_ROT270;
                case 4: return TRANS_MIRROR_ROT180;
                case 5: return TRANS_ROT180;
                case 6: return TRANS_NONE;
                case 7: return TRANS_MIRROR;
            }
            break;
        case 6:
            switch (x) {
                case 0: return TRANS_ROT270;
                case 1: return TRANS_MIRROR_ROT270;
                case 2: return TRANS_MIRROR_ROT90;
                case 3: return TRANS_ROT90;
                case 4: return TRANS_MIRROR;
                case 5: return TRANS_NONE;
                case 6: return TRANS_ROT180;
                case 7: return TRANS_MIRROR_ROT180;
            }
            break;
        case 7:
            switch (x) {
                case 0: return TRANS_MIRROR_ROT90;
                case 1: return TRANS_ROT90;
                case 2: return TRANS_ROT270;
                case 3: return TRANS_MIRROR_ROT270;
                case 4: return TRANS_ROT180;
                case 5: return TRANS_MIRROR_ROT180;
                case 6: return TRANS_MIRROR;
                case 7: return TRANS_NONE;
            }
            break;
    }

    return -1; // 确保有返回值，尽管这个情况不会发生
}

SpriteX* createSpriteX(const char* spxName, BITMAP_565* image) {
    SpriteX* spx = malloc(sizeof(SpriteX));
    loadSpx(spx, spxName, image);
    return spx;
}

SpriteX* createSpriteXWithTiles(const char* spxName, BITMAP_565** tiles, int tileCount) {
    SpriteX* spx = malloc(sizeof(SpriteX));
    loadSpx(spx, spxName, NULL);
    spx->tiles = tiles;
    spx->tileMode = true;
    spx->tileCount = tileCount;
    spx->tileUseCounter = calloc(tileCount, sizeof(int));
    return spx;
}

SpriteX* createSpriteXWithImageName(const char* spxName, const char* imageName) {
    SpriteX* spx = malloc(sizeof(SpriteX));
    spx->spxName = strdup(spxName);
    spx->imageName = strdup(imageName);
    spx->tileMode = false;
    spx->image = readBitmap(imageName);
    loadSpx(spx, spxName, spx->image);
    return spx;
}

SpriteX* createSpriteXCopy(SpriteX* spx) {
    SpriteX* newSpx = malloc(sizeof(SpriteX));
    newSpx->actionCount = spx->actionCount;
    newSpx->frameCount = spx->frameCount;
    newSpx->tileCount = spx->tileCount;
    newSpx->actionData = malloc(spx->actionCount * sizeof(short*));
    newSpx->frameData = malloc(spx->frameCount * sizeof(short*));
    newSpx->tileData = malloc(spx->tileCount * sizeof(short*));
    for (int i = 0; i < spx->actionCount; i++) {
        newSpx->actionData[i] = malloc(spx->actionData[i][0] * sizeof(short));
        memcpy(newSpx->actionData[i], spx->actionData[i], spx->actionData[i][0] * sizeof(short));
    }
    for (int i = 0; i < spx->frameCount; i++) {
        newSpx->frameData[i] = malloc(spx->frameData[i][0] * sizeof(short));
        memcpy(newSpx->frameData[i], spx->frameData[i], spx->frameData[i][0] * sizeof(short));
    }
    for (int i = 0; i < spx->tileCount; i++) {
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

int16_t swap_int16(int16_t val) {
    return (val << 8) | (val >> 8);
}
int32_t swap_int32(int32_t val) {
    return ((val >> 24) & 0x000000FF) | // 提取最高字节
           ((val >> 8) & 0x0000FF00) |  // 提取次高字节
           ((val << 8) & 0x00FF0000) |  // 提取次低字节
           ((val << 24) & 0xFF000000);   // 提取最低字节
}

int32_t readInt(uint8_t* buffer, size_t buffer_pos) {
    int32_t val = *(int32_t*)(buffer + buffer_pos);
    return swap_int32(val);
}

int16_t readShort(uint8_t* buffer, size_t buffer_pos) {
    int16_t val = *(int16_t*)(buffer + buffer_pos);
    return swap_int16(val);
}

uint8_t readByte(uint8_t* buffer, size_t buffer_pos) {
    uint8_t val = *(uint8_t*)(buffer + buffer_pos);
    return val;
}

void writeInt(FILE* file, uint32_t val) {
    val = swap_int32(val);
    fwrite(&val, sizeof(uint32_t), 1, file);
}

void writeShort(FILE* file, uint16_t val) {
    val = swap_int16(val);
    fwrite(&val, sizeof(uint16_t), 1, file);
}

void writeByte(FILE* file, uint8_t val) {
    fwrite(&val, sizeof(uint8_t), 1, file);
}

void loadSpx(SpriteX* spx, const char* spxName, BITMAP_565* image) {
    FILE* file = fopen(spxName, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", spxName);
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("File size: %zu\n", file_size);  // 输出文件大小

    // Read file into buffer
    uint8_t* buffer = malloc(file_size);
    fread(buffer, 1, file_size, file);
    fclose(file);

    // Initialize buffer position
    size_t buffer_pos = 0;

    // Check SPX format
    int header = readInt(buffer, buffer_pos);
    buffer_pos += sizeof(uint32_t);
    printf("SPX Header: %d\n", header);  // 输出SPX头部
    if (header != SPX_HEADER) {
        fprintf(stderr, "Invalid SpriteX format\n");
        free(buffer);
        return;
    }

    uint8_t version = readByte(buffer, buffer_pos);
    buffer_pos += sizeof(uint8_t);
    printf("SPX Version: %d\n", version);  // 输出SPX版本
    if (version != SPX_VERSION) {
        fprintf(stderr, "Version no matching\n");
        free(buffer);
        return;
    }

    uint8_t byteSequence = readByte(buffer, buffer_pos);
    buffer_pos += sizeof(uint8_t);
    printf("Byte Sequence: %d\n", byteSequence);  // 输出字节序列
    if ((byteSequence & 1) != SPX_BYTE_SEQUENCE_JAVA) {
        fprintf(stderr, "Byte sequence error\n");
        free(buffer);
        return;
    }

    if ((byteSequence & 2) != 0) {
        spx->tileMode = true;
    }

    // Read tile count
    spx->tileCount = readInt(buffer, buffer_pos);
    buffer_pos += sizeof(uint32_t);
    printf("Tile Count: %d\n", spx->tileCount);  // 输出瓦片数量

    if (!spx->tileMode) {
        spx->tileData = malloc(spx->tileCount * sizeof(short*));
        for (int i = 0; i < spx->tileCount; i++) {
            spx->tileData[i] = malloc(4 * sizeof(short));
            for (int j = 0; j < 4; j++) {
                spx->tileData[i][j] = readShort(buffer, buffer_pos);
                
            }
            buffer_pos += 8;
            printf("Tile %d: x=%d, y=%d, width=%d, height=%d\n", i, spx->tileData[i][0], spx->tileData[i][1], spx->tileData[i][2], spx->tileData[i][3]);
        }
    }

    // Read frame count
    spx->frameCount = readInt(buffer, buffer_pos);
    buffer_pos += sizeof(uint32_t);
    printf("Frame Count: %d\n", spx->frameCount);  // 输出帧数量
    spx->frameData = malloc(spx->frameCount * sizeof(short*));
    memset(spx->frameData, 0, spx->frameCount * sizeof(short*));
    for (int i = 0; i < spx->frameCount; i++) {
        int frameTileCount = readInt(buffer, buffer_pos);
        buffer_pos += sizeof(uint32_t);
        printf("Tile Count: %d\n", frameTileCount);
        int collisionCount = readInt(buffer, buffer_pos);
        buffer_pos += sizeof(uint32_t);
        printf("Collision Count: %d\n", collisionCount);
        int referencePointCount = readInt(buffer, buffer_pos);
        buffer_pos += sizeof(uint32_t);
        printf("Reference Point Count: %d\n", referencePointCount);
        int length = FRAME_HEADER_SIZE + frameTileCount * 4 + collisionCount * 4 + referencePointCount * 2;
        spx->frameData[i] = malloc(length * sizeof(short));
        spx->frameData[i][0] = length;
        spx->frameData[i][FRAME_TILE_COUNT_BIT] = frameTileCount;
        spx->frameData[i][FRAME_COLLISION_COUNT_BIT] = collisionCount;
        spx->frameData[i][FRAME_REFERENCE_POINT_COUNT_BIT] = referencePointCount;
        printf("Frame %d: length=%d, frameTileCount=%d, collisionCount=%d, referencePointCount=%d\n", i, length, frameTileCount, collisionCount, referencePointCount);
        // for (int j = FRAME_TOP_POS_BIT; j <= FRAME_RIGHT_POS_BIT; j++) {
        //     spx->frameData[i][j] = readShort(buffer, buffer_pos);
        //     buffer_pos += sizeof(uint16_t);
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

        int offset = FRAME_HEADER_SIZE;
        for (int j = 0; j < frameTileCount; j++) {
            for (int k = 0; k < 4; k++) {
                spx->frameData[i][offset + k] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16_t);
            }
            offset += 4;
        }
        for (int j = 0; j < collisionCount; j++) {
            for (int k = 0; k < 4; k++) {
                spx->frameData[i][offset + k] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16_t);
            }
            printf("read frameTitleCount = %d %d %d %d\n", spx->frameData[i][0 + offset], spx->frameData[i][1 + offset], spx->frameData[i][2 + offset], spx->frameData[i][3 + offset]);
            offset += 4;
        }
        for (int j = 0; j < referencePointCount; j++) {
            for (int k = 0; k < 2; k++) {
                spx->frameData[i][offset + k] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16_t);
            }
            printf("read collisionCount = %d %d\n", spx->frameData[i][0 + offset], spx->frameData[i][1 + offset]);
            offset += 2;
        }
    }

    // Read action count
    spx->actionCount = readInt(buffer, buffer_pos);
    buffer_pos += sizeof(uint32_t);
    printf("Action Count: %d\n", spx->actionCount);  // 输出动作数量
    printf("pos = %d\n", buffer_pos);
    spx->actionData = malloc(spx->actionCount * sizeof(short*));
    for (int i = 0; i < spx->actionCount; i++) {
        int32_t sequenceLength = readInt(buffer, buffer_pos);
        
        buffer_pos += sizeof(uint32_t);
        uint8_t delay = readByte(buffer, buffer_pos);
        buffer_pos += sizeof(uint8_t);
        printf("Action %d: sequenceLength=%d, delay=%d\n", i, sequenceLength, delay);
        int length = 0;
        if (delay == 1) {
            length = ACTION_HEADER_SIZE + sequenceLength * 2;
        } else {
            length = ACTION_HEADER_SIZE + sequenceLength;
        }
        printf("read %d bytes", length);
        spx->actionData[i] = malloc(length * sizeof(short));
        spx->actionData[i][0] = length;
        spx->actionData[i][ACTION_SEQUENCE_LENGTH_BIT] = sequenceLength;
        spx->actionData[i][ACTION_SEQUENCE_DELAY_BIT] = delay;
        spx->actionData[i][ACTION_TRANSFORM_BIT] = readShort(buffer, buffer_pos);
        buffer_pos += 4;
        printf("readAction %d %d %d %d %d %d\n", i, spx->actionData[i][0], spx->actionData[i][1], spx->actionData[i][2], spx->actionData[i][3], spx->actionData[i][4]);
        int offset = ACTION_HEADER_SIZE;
        printf("delay = %d\n", delay);
        if (delay == 1) {
            for (int j = 0; j < sequenceLength; j++) {
                spx->actionData[i][offset] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16_t);
                spx->actionData[i][offset + 1] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16_t);
                offset += 2;
            }
        } else {
            for (int j = 0; j < sequenceLength; j++) {
                spx->actionData[i][offset] = readShort(buffer, buffer_pos);
                buffer_pos += sizeof(uint16_t);
                offset += 1;
            }
        }
        printf("pos = %d\n", buffer_pos);
    }

    // Set image
    setImage(spx, image);

    // Load complete
    spx->visible = true;

    free(buffer);
}
void saveSpx(SpriteX* spx, const char* spxName) {
    FILE* file = fopen(spxName, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", spxName);
        return;
    }

    // Write SPX header
    writeInt(file, SPX_HEADER);
    writeByte(file, SPX_VERSION);
    uint8_t byteSequence = SPX_BYTE_SEQUENCE_JAVA;
    if (spx->tileMode) {
        byteSequence |= 2;
    }
    writeByte(file, byteSequence);

    // Write tile count
    writeInt(file, spx->tileCount);

    if (!spx->tileMode) {
        for (int i = 0; i < spx->tileCount; i++) {
            for (int j = 0; j < 4; j++) {
                writeShort(file, spx->tileData[i][j]);
            }
        }
    }

    // Write frame count
    writeInt(file, spx->frameCount);
    for (int i = 0; i < spx->frameCount; i++) {
        int frameTileCount = spx->frameData[i][FRAME_TILE_COUNT_BIT];
        int collisionCount = spx->frameData[i][FRAME_COLLISION_COUNT_BIT];
        int referencePointCount = spx->frameData[i][FRAME_REFERENCE_POINT_COUNT_BIT];
        writeInt(file, frameTileCount);
        writeInt(file, collisionCount);
        writeInt(file, referencePointCount);
        for (int j = FRAME_TOP_POS_BIT; j <= FRAME_RIGHT_POS_BIT; j++) {
            writeShort(file, spx->frameData[i][j]);
        }

        int offset = FRAME_HEADER_SIZE;
        for (int j = 0; j < frameTileCount; j++) {
            for (int k = 0; k < 4; k++) {
                writeShort(file, spx->frameData[i][offset + k]);
            }
            offset += 4;
        }
        for (int j = 0; j < collisionCount; j++) {
            for (int k = 0; k < 4; k++) {
                writeShort(file, spx->frameData[i][offset + k]);
            }
            offset += 4;
        }
        for (int j = 0; j < referencePointCount; j++) {
            for (int k = 0; k < 2; k++) {
                writeShort(file, spx->frameData[i][offset + k]);
            }
            offset += 2;
        }
    }

    // Write action count
    writeInt(file, spx->actionCount);
    for (int i = 0; i < spx->actionCount; i++) {
        int sequenceLength = spx->actionData[i][ACTION_SEQUENCE_LENGTH_BIT];
        uint8_t delay = spx->actionData[i][ACTION_SEQUENCE_DELAY_BIT];
        writeInt(file, sequenceLength);
        writeByte(file, delay);
        writeShort(file, spx->actionData[i][ACTION_TRANSFORM_BIT]);

        int offset = ACTION_HEADER_SIZE;
        if (delay == 1) {
            for (int j = 0; j < sequenceLength; j++) {
                for (int k = 0; k < 2; k++) {
                    writeShort(file, spx->actionData[i][offset + k]);
                }
                offset += 2;
            }
        } else {
            for (int j = 0; j < sequenceLength; j++) {
                writeShort(file, spx->actionData[i][offset]);
                offset += 1;
            }
        }
    }

    fclose(file);
}

void initAction(SpriteX* spx, int nAction) {
    if (!spx->tileMode) {
        return;
    }

    int* tileUse = calloc(spx->tileCount, sizeof(int));

    int sequenceLength = spx->actionData[nAction][ACTION_SEQUENCE_LENGTH_BIT];
    int isDelay = spx->actionData[nAction][ACTION_SEQUENCE_DELAY_BIT];
    int actionDataOffset = ACTION_HEADER_SIZE;
    if (isDelay == 1) {
        for (int i = 0; i < sequenceLength; ++i) {
            int frame = spx->actionData[nAction][actionDataOffset];
            int frameTileCount = spx->frameData[frame][FRAME_TILE_COUNT_BIT];
            int frameDataOffset = FRAME_HEADER_SIZE;
            int tile;
            for (int j = 0; j < frameTileCount; ++j) {
                tile = spx->frameData[frame][frameDataOffset];
                frameDataOffset += 4;
                tileUse[tile] = 1;
            }
            actionDataOffset += 2;
        }
    } else {
        for (int i = 0; i < sequenceLength; ++i) {
            int frame = spx->actionData[nAction][actionDataOffset];
            int frameTileCount = spx->frameData[frame][FRAME_TILE_COUNT_BIT];
            int frameDataOffset = FRAME_HEADER_SIZE;
            int tile;
            for (int j = 0; j < frameTileCount; ++j) {
                tile = spx->frameData[frame][frameDataOffset];
                frameDataOffset += 4;
                tileUse[tile] = 1;
            }
            ++actionDataOffset;
        }
    }

    char fullName[256];
    for (int i = 0; i < spx->tileCount; i++) {
        if (tileUse[i] == 1) {
            if (spx->tiles[i] == NULL) {
                snprintf(fullName, sizeof(fullName), "%s%d%s", spx->spxFileName, i, spx->spxFileExt);
                spx->tiles[i] = readBitmap(fullName);
            }
            ++spx->tileUseCounter[i];
        }
    }

    free(tileUse);
}

void releaseAction(SpriteX* spx, int nAction) {
    if (!spx->tileMode) {
        return;
    }

    int* tileUse = calloc(spx->tileCount, sizeof(int));

    int sequenceLength = spx->actionData[nAction][ACTION_SEQUENCE_LENGTH_BIT];
    int isDelay = spx->actionData[nAction][ACTION_SEQUENCE_DELAY_BIT];
    int actionDataOffset = ACTION_HEADER_SIZE;
    if (isDelay == 1) {
        for (int i = 0; i < sequenceLength; ++i) {
            int frame = spx->actionData[nAction][actionDataOffset];
            int frameTileCount = spx->frameData[frame][FRAME_TILE_COUNT_BIT];
            int frameDataOffset = FRAME_HEADER_SIZE;
            int tile;
            for (int j = 0; j < frameTileCount; ++j) {
                tile = spx->frameData[frame][frameDataOffset];
                frameDataOffset += 4;
                tileUse[tile] = 1;
            }
            actionDataOffset += 2;
        }
    } else {
        for (int i = 0; i < sequenceLength; ++i) {
            int frame = spx->actionData[nAction][actionDataOffset];
            int frameTileCount = spx->frameData[frame][FRAME_TILE_COUNT_BIT];
            int frameDataOffset = FRAME_HEADER_SIZE;
            int tile;
            for (int j = 0; j < frameTileCount; ++j) {
                tile = spx->frameData[frame][frameDataOffset];
                frameDataOffset += 4;
                tileUse[tile] = 1;
            }
            ++actionDataOffset;
        }
    }

    for (int i = 0; i < spx->tileCount; i++) {
        if (spx->tiles[i] != NULL) {
            --spx->tileUseCounter[i];
            if (spx->tileUseCounter[i] <= 0) {
                bitmapFree(spx->tiles[i]);
                spx->tiles[i] = NULL;
            }
        }
    }

    free(tileUse);
}

void initAllAction(SpriteX* spx) {
    if (!spx->tileMode) {
        return;
    }

    char fullName[256];
    for (int i = 0; i < spx->tileCount; i++) {
        if (spx->tiles[i] == NULL) {
            snprintf(fullName, sizeof(fullName), "%s%d%s", spx->spxFileName, i, spx->spxFileExt);
            spx->tiles[i] = readBitmap(fullName);
        }
        spx->tileUseCounter[i] = 1;
    }
}

void releaseAllAction(SpriteX* spx) {
    if (!spx->tileMode) {
        return;
    }

    for (int i = 0; i < spx->tileCount; i++) {
        if (spx->tiles[i] != NULL) {
            bitmapFree(spx->tiles[i]);
            spx->tiles[i] = NULL;
        }
        spx->tileUseCounter[i] = 0;
    }
}

const char* getSpxName(SpriteX* spx) {
    return spx->spxName;
}

const char* getImageName(SpriteX* spx) {
    return spx->imageName;
}


void enableUpdate(SpriteX* spx, bool enable) {
    if (spx->disableUpdate && enable) {
        spx->lastTime = mrc_getUptime();
    }
    spx->disableUpdate = !enable;
}

bool isEnableUpdate(SpriteX* spx) {
    return !spx->disableUpdate;
}

void setImage(SpriteX* spx, BITMAP_565* image) {
    spx->image = image;
}

BITMAP_565* getImage(SpriteX* spx) {
    return spx->image;
}

BITMAP_565* getImageAtIndex(SpriteX* spx, int index) {
    return spx->tiles[index];
}

void setX(SpriteX* spx, int x) {
    spx->x = x;
}

void setY(SpriteX* spx, int y) {
    spx->y = y;
}

void setPosition(SpriteX* spx, int x, int y) {
    spx->x = x;
    spx->y = y;
}

void move(SpriteX* spx, int dx, int dy) {
    spx->x += dx;
    spx->y += dy;
}

int getX(SpriteX* spx) {
    return spx->x;
}

int getY(SpriteX* spx) {
    return spx->y;
}

int getWidth(SpriteX* spx) {
    return spx->width;
}

int getHeight(SpriteX* spx) {
    return spx->height;
}

void setVisible(SpriteX* spx, bool visible) {
    spx->visible = visible;
}

bool isVisible(SpriteX* spx) {
    return spx->visible;
}

void setAction(SpriteX* spx, int actionIndex) {
    if (actionIndex == spx->actionIndex) {
        return;
    }
    if (actionIndex < 0 || actionIndex >= spx->actionCount) {
        fprintf(stderr, "IndexOutOfBoundsException\n");
        return;
    }
    spx->actionIndex = actionIndex;
    spx->sequenceIndex = 0;
    spx->firstUpdate = false;
}

int getAction(SpriteX* spx) {
    return spx->actionIndex;
}

int getActionCount(SpriteX* spx) {
    return spx->actionCount;
}

void setFrame(SpriteX* spx, int sequenceIndex) {
    if (sequenceIndex < 0 || sequenceIndex >= getSequenceLength(spx)) {
        fprintf(stderr, "IndexOutOfBoundsException\n");
        return;
    }
    spx->sequenceIndex = sequenceIndex;
    spx->firstUpdate = false;
}

int getFrame(SpriteX* spx) {
    return spx->sequenceIndex;
}

void setTransform(SpriteX* spx, int transform) {
    if (transform < 0 || transform > 7) {
        fprintf(stderr, "IllegalArgumentException\n");
        return;
    }
    spx->actionData[spx->actionIndex][ACTION_TRANSFORM_BIT] = transform;
}

int getTransform(SpriteX* spx) {
    return spx->actionData[spx->actionIndex][ACTION_TRANSFORM_BIT];
}

void deasilRotate90(SpriteX* spx) {
    setTransform(spx, getTransformTable(getTransform(spx), DEASIL_90));
}

void anticlockwiseRotate90(SpriteX* spx) {
    setTransform(spx, getTransformTable(getTransform(spx), ANTICLOCKWISE_90));
}

void horizontalMirror(SpriteX* spx) {
    setTransform(spx, getTransformTable(getTransform(spx), HORIZONTAL));
}

void verticalMirror(SpriteX* spx) {
    setTransform(spx, getTransformTable(getTransform(spx), VERTICAL));
}

void nextFrame(SpriteX* spx) {
    spx->sequenceIndex = (spx->sequenceIndex + 1) % getSequenceLength(spx);
    spx->firstUpdate = false;
}

void prevFrame(SpriteX* spx) {
    if (spx->sequenceIndex == 0) {
        spx->sequenceIndex = getSequenceLength(spx) - 1;
    } else {
        spx->sequenceIndex--;
    }
    spx->firstUpdate = false;
}

void update(SpriteX* spx, int32_t time) {
    if (!isEnableUpdate(spx)) {
        return;
    }
    printf("update time = %d\n", time);
    printf("isDelay = %d\n", isDelay(spx));
    if (isDelay(spx)) {
        if (!spx->firstUpdate) {
            spx->firstUpdate = true;
            spx->lastTime = time;
        }
        int dms = getDelayTime(spx);
        if ((time - spx->lastTime) >= dms) {
            nextFrame(spx);
            spx->lastTime = time;
        }
    } else {
        if (!spx->firstUpdate) {
            spx->firstUpdate = true;
            spx->lastTime = time;
        }
        int dms = spx->delay;
        if ((time - spx->lastTime) >= dms) {
            nextFrame(spx);
            spx->lastTime = time;
        }
    }
}

void setDelay(SpriteX* spx, int delay) {
    spx->delay = delay;
}

int getDelay(SpriteX* spx) {
    return spx->delay;
}

void updateSprite(SpriteX* spx) {
    if (!isEnableUpdate(spx)) {
        return;
    }
    update(spx, mrc_getUptime());
}

int getSequenceFrame(SpriteX* spx, int sequenceIndex) {
    if (isDelay(spx)) {
        return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + sequenceIndex * 2];
    } else {
        return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + sequenceIndex];
    }
}

int getSequenceFrameCurrent(SpriteX* spx) {
    if (isDelay(spx)) {
        return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + spx->sequenceIndex * 2];
    } else {
        return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + spx->sequenceIndex];
    }
}

int getSequenceLength(SpriteX* spx) {
    return spx->actionData[spx->actionIndex][ACTION_SEQUENCE_LENGTH_BIT];
}

bool isDelay(SpriteX* spx) {
    return spx->actionData[spx->actionIndex][ACTION_SEQUENCE_DELAY_BIT] == 1;
}

int getDelayTime(SpriteX* spx) {
    return spx->actionData[spx->actionIndex][ACTION_HEADER_SIZE + spx->sequenceIndex * 2 + 1];
}

int getReferencePointX(SpriteX* spx, int index) {
    int frameIndex = getSequenceFrameCurrent(spx);
    int frameTileCount = spx->frameData[frameIndex][FRAME_TILE_COUNT_BIT];
    int frameCollisionCount = spx->frameData[frameIndex][FRAME_COLLISION_COUNT_BIT];
    int offset = FRAME_HEADER_SIZE + frameTileCount * 4 + frameCollisionCount * 4 + index * 2;
    int refX = spx->frameData[frameIndex][offset];
    int refY = spx->frameData[frameIndex][offset + 1];
    return getTransformedReferenceX(spx, refX, refY, getTransform(spx)) + spx->x;
}

int getReferencePointY(SpriteX* spx, int index) {
    int frameIndex = getSequenceFrameCurrent(spx);
    int frameTileCount = spx->frameData[frameIndex][FRAME_TILE_COUNT_BIT];
    int frameCollisionCount = spx->frameData[frameIndex][FRAME_COLLISION_COUNT_BIT];
    int offset = FRAME_HEADER_SIZE + frameTileCount * 4 + frameCollisionCount * 4 + index * 2;
    int refX = spx->frameData[frameIndex][offset];
    int refY = spx->frameData[frameIndex][offset + 1];
    return getTransformedReferenceY(spx, refX, refY, getTransform(spx)) + spx->y;
}

int getReferencePointCount(SpriteX* spx, int frameIndex) {
    if (frameIndex < 0 || frameIndex >= spx->frameCount) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    return spx->frameData[frameIndex][FRAME_REFERENCE_POINT_COUNT_BIT];
}

int getReferencePointCountCurrent(SpriteX* spx) {
    int frameIndex = getSequenceFrameCurrent(spx);
    return getReferencePointCount(spx, frameIndex);
}

int getTransformedReferenceX(SpriteX* spx, int x, int y, int transform) {
    int t_x = 0;
    switch (transform) {
        case TRANS_NONE:
            t_x = x;
            break;
        case TRANS_MIRROR:
            t_x = -x;
            break;
    }
    return t_x;
}

int getTransformedReferenceY(SpriteX* spx, int x, int y, int transform) {
    int t_y = 0;
    switch (transform) {
        case TRANS_NONE:
            t_y = y;
            break;
        case TRANS_MIRROR:
            t_y = y;
            break;
    }
    return t_y;
}

int getCollidesX(SpriteX* spx, int frame, int index) {
    if (index < 0 || index >= spx->frameData[frame][FRAME_COLLISION_COUNT_BIT]) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    int offset = FRAME_HEADER_SIZE + spx->frameData[frame][FRAME_TILE_COUNT_BIT] * 4 + index * 4;
    return spx->frameData[frame][offset] + spx->x;
}

int getCollidesY(SpriteX* spx, int frame, int index) {
    if (index < 0 || index >= spx->frameData[frame][FRAME_COLLISION_COUNT_BIT]) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    int offset = FRAME_HEADER_SIZE + spx->frameData[frame][FRAME_TILE_COUNT_BIT] * 4 + index * 4 + 1;
    return spx->frameData[frame][offset] + spx->y;
}

int getCollidesWidth(SpriteX* spx, int frame, int index) {
    if (index < 0 || index >= spx->frameData[frame][FRAME_COLLISION_COUNT_BIT]) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    int offset = FRAME_HEADER_SIZE + spx->frameData[frame][FRAME_TILE_COUNT_BIT] * 4 + index * 4 + 2;
    return spx->frameData[frame][offset];
}

int getCollidesHeight(SpriteX* spx, int frame, int index) {
    if (index < 0 || index >= spx->frameData[frame][FRAME_COLLISION_COUNT_BIT]) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException\n");
        return -1;
    }
    int offset = FRAME_HEADER_SIZE + spx->frameData[frame][FRAME_TILE_COUNT_BIT] * 4 + index * 4 + 3;
    return spx->frameData[frame][offset];
}

int getCollidesXCurrent(SpriteX* spx, int index) {
    int frameIndex = getSequenceFrameCurrent(spx);
    return getCollidesX(spx, frameIndex, index);
}

int getCollidesYCurrent(SpriteX* spx, int index) {
    int frameIndex = getSequenceFrameCurrent(spx);
    return getCollidesY(spx, frameIndex, index);
}

int getCollidesWidthCurrent(SpriteX* spx, int index) {
    int frameIndex = getSequenceFrameCurrent(spx);
    return getCollidesWidth(spx, frameIndex, index);
}

int getCollidesHeightCurrent(SpriteX* spx, int index) {
    int frameIndex = getSequenceFrameCurrent(spx);
    return getCollidesHeight(spx, frameIndex, index);
}

int getCollidesCount(SpriteX* spx, int index) {
    return spx->frameData[index][FRAME_COLLISION_COUNT_BIT];
}

int getCollidesCountCurrent(SpriteX* spx) {
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_COLLISION_COUNT_BIT];
}

bool collidesWith(SpriteX* spx, SpriteX* other, int spxCollides, int thisCollides, int type) {
    if (!other->visible || !spx->visible) {
        return false;
    }
    int x = getCollidesXCurrent(other, spxCollides);
    int y = getCollidesYCurrent(other, spxCollides);
    int width = getCollidesWidthCurrent(other, spxCollides);
    int height = getCollidesHeightCurrent(other, spxCollides);
    return collidesWithRect(spx, x, y, width, height, thisCollides, type);
}

bool collidesWithImage(SpriteX* spx, BITMAP_565* image, int x, int y, int collides, int type) {
    return collidesWithRect(spx, x, y, image->width, image->height, collides, type);
}

bool collidesWithRect(SpriteX* spx, int x, int y, int width, int height, int collides, int type) {
    if (!spx->visible) {
        return false;
    }
    int x1 = getCollidesXCurrent(spx, collides);
    int y1 = getCollidesYCurrent(spx, collides);
    int w1 = getCollidesWidthCurrent(spx, collides);
    int h1 = getCollidesHeightCurrent(spx, collides);
    if (type == COLLISION_INTERSECT) {
        return intersectRect(x1, y1, w1, h1, x, y, width, height);
    } else if (type == COLLISION_INCLUSION) {
        return inclusionRect(x1, y1, w1, h1, x, y, width, height);
    } else {
        fprintf(stderr, "ArithmeticException\n");
        return false;
    }
}

bool collidesWithSprite(SpriteX* spx, SpriteX* other) {
    if (!other->visible || !spx->visible) {
        return false;
    }
    if (getCollidesCountCurrent(spx) == 0 || getCollidesCountCurrent(other) == 0) {
        return false;
    }
    return collidesWith(spx, other, 0, 0, COLLISION_INTERSECT);
}

bool collidesWithImageRect(SpriteX* spx, BITMAP_565* image, int x, int y) {
    if (!spx->visible) {
        return false;
    }
    if (getCollidesCountCurrent(spx) == 0) {
        return false;
    }
    return collidesWithImage(spx, image, x, y, 0, COLLISION_INTERSECT);
}

bool collidesWithRectRect(SpriteX* spx, int x, int y, int width, int height) {
    if (!spx->visible) {
        return false;
    }
    if (getCollidesCountCurrent(spx) == 0) {
        return false;
    }
    return collidesWithRect(spx, x, y, width, height, 0, COLLISION_INTERSECT);
}

bool intersectRect(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {
    if (y2 + height2 < y1 || y2 > y1 + height1 || x2 + width2 < x1 || x2 > x1 + width1) {
        return false;
    } else {
        return true;
    }
}

bool inclusionRect(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {
    if (y2 >= y1 && y2 + height2 <= y1 + height1 && x2 >= x1 && x2 + width2 <= x1 + width1) {
        return true;
    }
    return false;
}

void originOffset(SpriteX* spx, int x, int y) {
    spx->originOffsetX = x;
    spx->originOffsetY = y;
}

int getFrameTopPos(SpriteX* spx) {
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_TOP_POS_BIT] + spx->y;
}

int getFrameBottomPos(SpriteX* spx) {
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_BOTTOM_POS_BIT] + spx->y;
}

int getFrameLeftPos(SpriteX* spx) {
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_LEFT_POS_BIT] + spx->x;
}

int getFrameRightPos(SpriteX* spx) {
    return spx->frameData[getSequenceFrameCurrent(spx)][FRAME_RIGHT_POS_BIT] + spx->x;
}

int getFrameWidth(SpriteX* spx) {
    return getFrameRightPos(spx) - getFrameLeftPos(spx);
}

int getFrameHeight(SpriteX* spx) {
    return getFrameBottomPos(spx) - getFrameTopPos(spx);
}

void paint(SpriteX* spx, BITMAP_565* g, int x, int y) {
    if (g == NULL) {
        fprintf(stderr, "NullPointerException\n");
        return;
    }
    if (spx->visible) {
        x -= spx->originOffsetX;
        y -= spx->originOffsetY;

        int clipX = g->width;
        int clipY = g->height;
        int clipWidth = g->width;
        int clipHeight = g->height;
        int dw;
        int dh;

        int actionTransform = getTransform(spx);
        if (actionTransform == TRANS_NONE) {
            int tileIndex; // tile index
            int dx; // tile dest x
            int dy; // tile dest y
            int transform; // tile transform
            int tx; // tile x
            int ty; // tile y
            int tw; // tile width
            int th; // tile height
            int frameIndex = getSequenceFrameCurrent(spx);
            int tileCount = spx->frameData[frameIndex][FRAME_TILE_COUNT_BIT];
            int offset = FRAME_HEADER_SIZE; // tile data offset
            for (int i = 0; i < tileCount; i++) {
                if (spx->tileMode) {
                    tileIndex = spx->frameData[frameIndex][0 + offset];
                    dx = spx->frameData[frameIndex][1 + offset] + x;
                    dy = spx->frameData[frameIndex][2 + offset] + y;
                    transform = spx->frameData[frameIndex][3 + offset];
                    tx = 0;
                    ty = 0;
                    tw = spx->tiles[tileIndex]->width;
                    th = spx->tiles[tileIndex]->height;

                    if ((transform & INVERTED_AXES) != 0) {
                        dw = th;
                        dh = tw;
                    } else {
                        dw = tw;
                        dh = th;
                    }

                    if (intersectRect(dx, dy, dw, dh, clipX, clipY, clipWidth, clipHeight)) {
                        drawRegion(g, spx->tiles[tileIndex], tx, ty, tw, th, transform, dx, dy, GRAPHICS_TOP | GRAPHICS_LEFT);
                    }
                } else {
                    tileIndex = spx->frameData[frameIndex][0 + offset];
                    dx = spx->frameData[frameIndex][1 + offset] + x;
                    dy = spx->frameData[frameIndex][2 + offset] + y;
                    transform = spx->frameData[frameIndex][3 + offset];
                    tx = spx->tileData[tileIndex][0];
                    ty = spx->tileData[tileIndex][1];
                    tw = spx->tileData[tileIndex][2];
                    th = spx->tileData[tileIndex][3];

                    if ((transform & INVERTED_AXES) != 0) {
                        dw = th;
                        dh = tw;
                    } else {
                        dw = tw;
                        dh = th;
                    }

                    if (intersectRect(dx, dy, dw, dh, clipX, clipY, clipWidth, clipHeight)) {
                        drawRegion(g, spx->image, tx, ty, tw, th, transform, dx, dy, GRAPHICS_TOP | GRAPHICS_LEFT);
                    }
                }
                offset += 4;
            }
        } else {
            // render transform action
            int tileIndex; // tile index
            int dx; // tile dest x
            int dy; // tile dest y
            int transform; // tile transform
            int tx; // tile x
            int ty; // tile y
            int tw; // tile width
            int th; // tile height
            int cx;
            int cy;
            int frameIndex = getSequenceFrameCurrent(spx);
            int tileCount = spx->frameData[frameIndex][FRAME_TILE_COUNT_BIT];
            int offset = FRAME_HEADER_SIZE; // tile data offset
            for (int i = 0; i < tileCount; i++) {
                if (spx->tileMode) {
                    tileIndex = spx->frameData[frameIndex][0 + offset];
                    dx = spx->frameData[frameIndex][1 + offset] + x;
                    dy = spx->frameData[frameIndex][2 + offset] + y;
                    transform = spx->frameData[frameIndex][3 + offset];
                    tx = 0;
                    ty = 0;
                    tw = spx->tiles[tileIndex]->width;
                    th = spx->tiles[tileIndex]->height;

                    if ((transform & INVERTED_AXES) != 0) {
                        dw = th;
                        dh = tw;
                    } else {
                        dw = tw;
                        dh = th;
                    }

                    cx = x - dx;
                    cy = y - dy;
                    // finally transform
                    transform = getTransformTable(transform, actionTransform);
                    // adjust coordinate
                    switch (actionTransform) {
                        case TRANS_NONE:
                            // null
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

                    if ((transform & INVERTED_AXES) != 0) {
                        dw = th;
                        dh = tw;
                    } else {
                        dw = tw;
                        dh = th;
                    }

                    if (intersectRect(dx, dy, dw, dh, clipX, clipY, clipWidth, clipHeight)) {
                        drawRegion(g, spx->tiles[tileIndex], tx, ty, tw, th, transform, dx, dy, GRAPHICS_TOP | GRAPHICS_LEFT);
                    }
                } else {
                    tileIndex = spx->frameData[frameIndex][0 + offset];
                    dx = spx->frameData[frameIndex][1 + offset] + x;
                    dy = spx->frameData[frameIndex][2 + offset] + y;
                    transform = spx->frameData[frameIndex][3 + offset];
                    tx = spx->tileData[tileIndex][0];
                    ty = spx->tileData[tileIndex][1];
                    tw = spx->tileData[tileIndex][2];
                    th = spx->tileData[tileIndex][3];

                    if ((transform & INVERTED_AXES) != 0) {
                        dw = th;
                        dh = tw;
                    } else {
                        dw = tw;
                        dh = th;
                    }

                    cx = x - dx;
                    cy = y - dy;
                    // finally transform
                    transform = getTransformTable(transform, actionTransform);
                    // adjust coordinate
                    switch (actionTransform) {
                        case TRANS_NONE:
                            // null
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

                    if ((transform & INVERTED_AXES) != 0) {
                        dw = th;
                        dh = tw;
                    } else {
                        dw = tw;
                        dh = th;
                    }

                    if (intersectRect(dx, dy, dw, dh, clipX, clipY, clipWidth, clipHeight)) {
                        drawRegion(g, spx->image, tx, ty, tw, th, transform, dx, dy, GRAPHICS_TOP | GRAPHICS_LEFT);
                    }
                }
                offset += 4;
            }
        }
    }
}

void paintCurrent(SpriteX* spx, BITMAP_565* g) {
    paint(spx, g, spx->x, spx->y);
}

void drawRegion(BITMAP_565* g, BITMAP_565* src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor) {
    // Implement the drawRegion logic here
    // drawBitmapRegion(src, x_src, y_src, width, height, transform, x_dest, y_dest, anchor);
}