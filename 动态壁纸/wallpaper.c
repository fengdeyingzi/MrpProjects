#include <mrc_base.h>
#include "wallpaper.h"
#include "fopen.h"
#include "mrc_android.h"
#include "xl_debug.h"
#include "uc3_font.h"

extern int loadCurrentFrame(WALLPAPER_PLAYER *player);
extern int copyFrameToImage(WALLPAPER_PLAYER *player);
void drawCenterUI(char *text){
  int textWidth;
  mrc_clearScreen(255, 255, 255);
  textWidth = uc3_getWidth(text, 0);
  uc3_drawText(text, (SCRW - textWidth) / 2, (SCRH - 16) / 2, 0, 0, 0, 0);
  mrc_refreshScreen(0, 0, SCRW, SCRH);
  mrc_sleep(100);
}

/* 资源解压：读取plays.txt并解压所有壁纸 */
int wallpaper_extractResources(void) {
    WALLPAPER_LIST list;
    int i;
    int ret;
    uint8 packName[32];
    char *endName;
    int extractedCount = 0;

    LOG_MSG("开始资源解压...");

    /* 获取包名 */
    mrc_GetMrpInfo((char*)mrc_getPackName(), MRP_FILENAME, packName, 32);
    endName = mrc_strrchr((const char *)packName, '.');
    if (endName != NULL) {
        *endName = 0;
    }

    /* 检查目录是否存在，不存在则创建 */
    if (mrc_fileState((char*)packName) != MR_IS_DIR) {
        LOG_MSG("创建目录");
        ret = mrc_mkDir((char*)packName);
        if (ret != MR_SUCCESS) {
            LOG_MSG("创建目录失败");
            return -1;
        }
    }

    /* 读取plays.txt */
    ret = wallpaper_readPlaylist(&list);
    if (ret != 0) {
        LOG_MSG("读取plays.txt失败");
        return -2;
    }

    if (list.count == 0) {
        LOG_MSG("plays.txt中没有壁纸");
        wallpaper_freeList(&list);
        return -3;
    }

    /* 解压所有壁纸 */
    for (i = 0; i < list.count; i++) {
        char outputPath[128];

        /* 构造输出文件路径 */
        mrc_sprintf(outputPath, "%s/%s", packName, list.wallpapers[i].dfaName);

        /* 检查是否已解压 */
        if (mrc_fileState(outputPath) == MR_IS_FILE) {
            LOG_VAR("壁纸已存在，跳过: %s", list.wallpapers[i].dfaName);
            list.wallpapers[i].isExtracted = 1;
            continue;
        }

        /* 解压壁纸 */
        ret = wallpaper_extractWallpaper(list.wallpapers[i].dfaName);
        if (ret != 0) {
            LOG_VAR("解压壁纸失败: %s", list.wallpapers[i].dfaName);
            /* 继续解压其他壁纸 */
        } else {
            list.wallpapers[i].isExtracted = 1;
            LOG_VAR("壁纸解压成功: %s", list.wallpapers[i].dfaName);
        }
    }

    /* 统计解压结果 */
    
    for (i = 0; i < list.count; i++) {
        if (list.wallpapers[i].isExtracted) {
            extractedCount++;
        }
    }

    mrc_printf("资源解压完成，成功解压 %d/%d 个壁纸", extractedCount, list.count);

    /* 释放列表 */
    wallpaper_freeList(&list);

    return (extractedCount > 0) ? 0 : -4;
}

/* 初始化壁纸播放器 */
int wallpaper_init(WALLPAPER_PLAYER *player) {
    /* 清零 */
    mrc_memset(player, 0, sizeof(WALLPAPER_PLAYER));

    player->currentFrame = 0;
    player->isPaused = 0;
    player->file = NULL;
    player->frameInfos = NULL;
    player->frameBuffer = NULL;
    player->currentBitmap = NULL;
    /* list 成员已经通过 mrc_memset 清零 */

    return 0;
}

/* 加载dfa文件 */
int wallpaper_loadDfa(WALLPAPER_PLAYER *player, const char *filename) {
    DFA_HEADER header;
    DFA_FRAME_HEADER frameHeader;
    uint32 i;
    int32 readSize;
    uint32 currentOffset;
    uint32 totalPixels;

    LOG_VAR("[DEBUG] 加载dfa文件: %s", filename);

    /* 打开文件 */
    player->file = fopen(filename, "rb");
    if (!player->file) {
        LOG_MSG("无法打开dfa文件");
        return -1;
    }

    /* 读取文件头 - 逐个字段读取避免对齐问题 */
    readSize = fread(header.magic, 1, 4, player->file);
    if (readSize != 4) {
        LOG_MSG("读取魔数失败");
        fclose(player->file);
        player->file = NULL;
        return -2;
    }

    readSize = fread(&header.version, 1, sizeof(uint16), player->file);
    if (readSize != sizeof(uint16)) {
        LOG_MSG("读取版本失败");
        fclose(player->file);
        player->file = NULL;
        return -2;
    }

    readSize = fread(&header.width, 1, sizeof(uint16), player->file);
    if (readSize != sizeof(uint16)) {
        LOG_MSG("读取宽度失败");
        fclose(player->file);
        player->file = NULL;
        return -2;
    }

    readSize = fread(&header.height, 1, sizeof(uint16), player->file);
    if (readSize != sizeof(uint16)) {
        LOG_MSG("读取高度失败");
        fclose(player->file);
        player->file = NULL;
        return -2;
    }

    readSize = fread(&header.totalFrames, 1, sizeof(uint16), player->file);
    if (readSize != sizeof(uint16)) {
        LOG_MSG("读取总帧数失败");
        fclose(player->file);
        player->file = NULL;
        return -2;
    }

    readSize = fread(&header.keyframeInterval, 1, sizeof(uint16), player->file);
    if (readSize != sizeof(uint16)) {
        LOG_MSG("读取关键帧间隔失败");
        fclose(player->file);
        player->file = NULL;
        return -2;
    }

    readSize = fread(&header.compressionMode, 1, sizeof(uint8), player->file);
    if (readSize != sizeof(uint8)) {
        LOG_MSG("读取压缩模式失败");
        fclose(player->file);
        player->file = NULL;
        return -2;
    }

    readSize = fread(&header.reserved, 1, sizeof(uint8), player->file);
    if (readSize != sizeof(uint8)) {
        LOG_MSG("读取保留字段失败");
        fclose(player->file);
        player->file = NULL;
        return -2;
    }

    /* 验证魔数 */
    if (mrc_memcmp(header.magic, "DFA\0", 4) != 0) {
        LOG_MSG("文件格式错误，不是DFA文件");
        fclose(player->file);
        player->file = NULL;
        return -3;
    }

    /* 检查版本 */
    if (header.version != 1) {
        LOG_MSG("不支持的DFA版本");
        fclose(player->file);
        player->file = NULL;
        return -4;
    }

    /* 保存文件头信息 */
    player->header = header;

    LOG_VAR("DFA信息: %dx%d, 总帧数: %d", header.width, header.height, header.totalFrames);

    /* 计算总像素数 */
    totalPixels = header.width * header.height;
    if (totalPixels == 0) {
        LOG_MSG("无效的图像尺寸");
        fclose(player->file);
        player->file = NULL;
        return -5;
    }

    /* 分配帧缓冲区 */
    player->frameBuffer = (uint16*)mrc_malloc(totalPixels * sizeof(uint16));
    if (!player->frameBuffer) {
        LOG_MSG("分配帧缓冲区失败");
        fclose(player->file);
        player->file = NULL;
        return -6;
    }

    /* 初始化缓冲区 */
    mrc_memset(player->frameBuffer, 0, totalPixels * sizeof(uint16));

    /* 分配帧信息表 */
    player->frameInfos = (DFA_FRAME_INFO*)mrc_malloc(sizeof(DFA_FRAME_INFO) * header.totalFrames);
    if (!player->frameInfos) {
        LOG_MSG("分配帧信息表失败");
        mrc_free(player->frameBuffer);
        fclose(player->file);
        player->file = NULL;
        return -7;
    }

    /* 扫描所有帧，记录偏移和类型 */
    currentOffset = sizeof(DFA_HEADER);
    for (i = 0; i < header.totalFrames; i++) {
        /* 定位到帧头位置 */
        fseek(player->file, currentOffset, SEEK_SET);

        /* 读取帧头 - 逐个字段读取避免对齐问题 */
        readSize = fread(&frameHeader.frameType, 1, sizeof(uint8), player->file);
        if (readSize != sizeof(uint8)) {
            LOG_VAR("读取帧类型失败: 帧 %d", i);
            mrc_free(player->frameInfos);
            mrc_free(player->frameBuffer);
            fclose(player->file);
            player->file = NULL;
            return -8;
        }

        readSize = fread(&frameHeader.reserved, 1, sizeof(uint8), player->file);
        if (readSize != sizeof(uint8)) {
            LOG_VAR("读取保留字段失败: 帧 %d", i);
            mrc_free(player->frameInfos);
            mrc_free(player->frameBuffer);
            fclose(player->file);
            player->file = NULL;
            return -8;
        }

        readSize = fread(&frameHeader.dataSize, 1, sizeof(uint32), player->file);
        if (readSize != sizeof(uint32)) {
            LOG_VAR("读取数据大小失败: 帧 %d", i);
            mrc_free(player->frameInfos);
            mrc_free(player->frameBuffer);
            fclose(player->file);
            player->file = NULL;
            return -8;
        }

        /* 保存帧信息 */
        player->frameInfos[i].offset = currentOffset;
        player->frameInfos[i].dataSize = frameHeader.dataSize;
        player->frameInfos[i].frameType = frameHeader.frameType;

        /* 移动到下一帧 */
        currentOffset += 6 + frameHeader.dataSize;
    }

    LOG_MSG("dfa加载成功");
    return 0;
}

/* 加载第一帧（第0帧） */
int wallpaper_loadFirstFrame(WALLPAPER_PLAYER *player) {
    if (!player || !player->file || !player->frameInfos || player->header.totalFrames == 0) {
        return -1;
    }

    player->currentFrame = 0;
    return loadCurrentFrame(player);
}

/* 加载并解码当前帧 */
int loadCurrentFrame(WALLPAPER_PLAYER *player) {
    DFA_FRAME_INFO *frameInfo;
    DFA_FRAME_HEADER frameHeader;
    uint8 *frameData;
    int32 readSize;
    int32 useScreenBufer;
    int ret;
    useScreenBufer = FALSE;
    if (!player->file || !player->frameInfos || player->currentFrame >= player->header.totalFrames) {
        return -1;
    }

    /* 获取当前帧信息 */
    frameInfo = &player->frameInfos[player->currentFrame];

    /* 定位到帧头位置 */
    fseek(player->file, frameInfo->offset, SEEK_SET);

    /* 读取帧头 - 逐个字段读取避免对齐问题 */
    readSize = fread(&frameHeader.frameType, 1, sizeof(uint8), player->file);
    if (readSize != sizeof(uint8)) {
        LOG_VAR("读取帧类型失败: 帧 %d", player->currentFrame);
        return -2;
    }

    readSize = fread(&frameHeader.reserved, 1, sizeof(uint8), player->file);
    if (readSize != sizeof(uint8)) {
        LOG_VAR("读取保留字段失败: 帧 %d", player->currentFrame);
        return -2;
    }

    readSize = fread(&frameHeader.dataSize, 1, sizeof(uint32), player->file);
    if (readSize != sizeof(uint32)) {
        LOG_VAR("读取数据大小失败: 帧 %d", player->currentFrame);
        return -2;
    }

    /* 验证帧类型 */
    if (frameHeader.frameType != frameInfo->frameType) {
        mrc_printf("帧类型不匹配: 预期 %d, 实际 %d", frameInfo->frameType, frameHeader.frameType);
        return -3;
    }
    switch (frameHeader.frameType)
    {
    case 0:
        useScreenBufer = FALSE;
        break;
    default:
        useScreenBufer = TRUE;
        break;
    }
    /* 分配帧数据缓冲区 */
    if(useScreenBufer){
         frameData = (uint8*)w_getScreenBuffer();
         LOG_MSG("使用屏幕缓存区");
    }
    else{
        frameData = (uint8*)player->frameBuffer;
        LOG_MSG("使用帧缓存区");
    }
    

    /* 读取帧数据 */
    readSize = fread(frameData, 1, frameHeader.dataSize, player->file);
    if (readSize != frameHeader.dataSize) {
        mrc_printf("读取帧数据失败: 预期 %d, 实际 %d", frameHeader.dataSize, readSize);
        // mrc_free(frameData);
        return -5;
    }

    /* 根据帧类型解码 */
    switch (frameHeader.frameType) {
        case 0: /* 关键帧 因为直接read进了frameBuffer，所以无需处理*/
            ret = 0; //dfa_decodeKeyFrame(player, frameData, frameHeader.dataSize);
            break;
        case 1: /* 差分帧 */
            ret = dfa_decodeDiffFrame(player, frameData, frameHeader.dataSize);
            break;
        case 2: /* 跳过帧 */
            ret = dfa_decodeSkipFrame(player);
            break;
        case 16: /* 矩形区域差分帧 */
            ret = dfa_decodeRectFrame(player, frameData, frameHeader.dataSize);
            break;
        default:
            LOG_VAR("不支持的帧类型: %d", frameHeader.frameType);
            ret = -6;
            break;
    }

    /* 释放帧数据缓冲区 */
    // mrc_free(frameData);

    if (ret != 0) {
        mrc_printf("帧解码失败: 类型 %d, 错误 %d", frameHeader.frameType, ret);
        return -7;
    }
    copyFrameToImage(player);
    return 0;
}

/* 绘制当前帧 */
void wallpaper_drawFrame(WALLPAPER_PLAYER *player) {
    int32 x;
    int32 y;
    char infoText[64];
    int textWidth;

    if (!player->currentBitmap) {
        LOG_MSG("没有可绘制的bitmap");
        return;
    }

    LOG_VAR("绘制帧", player->currentFrame);
    /* 清屏 */
    // mrc_clearScreen(0, 0, 0);

    /* 居中绘制 */
    x = (SCRW - player->currentBitmap->width) / 2;
    y = (SCRH - player->currentBitmap->height) / 2;

    /* 绘制bitmap */
    drawBitmap(player->currentBitmap, x, y);

    /* 显示壁纸信息 */
    if (player->list.count > 0) {
        mrc_sprintf(infoText, "%s (%d/%d)",
                   wallpaper_getCurrentName(player),
                   player->list.currentIndex + 1,
                   player->list.count);
        textWidth = uc3_getWidth(infoText, 0);
        uc3_drawText(infoText, (SCRW - textWidth) / 2, 10, 255, 255, 255, 0);
    }

    /* 显示控制提示 */
    uc3_drawText("上/下", (SCRW - 40)/2, SCRH - 25, 255, 255, 255, 0);
    uc3_drawText("暂停", 10, SCRH - 25, 255, 255, 255, 0);
    uc3_drawText("返回", SCRW - 50, SCRH - 25, 255, 255, 255, 0);

    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

int copyFrameToImage(WALLPAPER_PLAYER *player) {
    uint32 totalPixels;
    uint32 imageSize;
    /* 计算图像尺寸 */
    totalPixels = player->header.width * player->header.height;
    imageSize = totalPixels * 2; /* RGB565 = 2字节/像素 */

    /* 如果还没有bitmap结构，创建一个 */
    if (!player->currentBitmap) {
        player->currentBitmap = (BITMAP_565*)mrc_malloc(sizeof(BITMAP_565));
        if (!player->currentBitmap) {
            LOG_MSG("申请bitmap结构失败");
            return -2;
        }
        mrc_memset(player->currentBitmap, 0, sizeof(BITMAP_565));
        player->currentBitmap->bitmap = player->frameBuffer;
        player->currentBitmap->buflen = 0;
    }


    /* 更新bitmap属性 */
    player->currentBitmap->width = player->header.width;
    player->currentBitmap->height = player->header.height;
    player->currentBitmap->color_bit = 16;
    player->currentBitmap->transcolor = 0;
    player->currentBitmap->mode = BM_COPY;


    /* 注意：frameBuffer现在保持当前帧数据，为下一帧（差分帧）做准备 */
    return 0;
}

/* 下一帧 */
int wallpaper_nextFrame(WALLPAPER_PLAYER *player) {
    int ret;
    

    if (player->isPaused) {
        return 0;
    }

    player->currentFrame++;
    if (player->currentFrame >= player->header.totalFrames) {
        player->currentFrame = 0; /* 循环播放 */
    }

    /* 加载并解码当前帧 */
    ret = loadCurrentFrame(player);
    if (ret != 0) {
        return ret;
    }


    return 0;
}



/* 暂停/继续 */
void wallpaper_togglePause(WALLPAPER_PLAYER *player) {
    player->isPaused = !player->isPaused;
    mrc_printf("[DEBUG] 壁纸播放: %s", player->isPaused ? "暂停" : "继续");
}

/* 释放资源 */
void wallpaper_free(WALLPAPER_PLAYER *player) {
    /* 释放当前bitmap */
    if (player->currentBitmap) {
        bitmapFree(player->currentBitmap);
        player->currentBitmap = NULL;
    }

    /* 释放帧信息表 */
    if (player->frameInfos) {
        mrc_free(player->frameInfos);
        player->frameInfos = NULL;
    }

    /* 释放帧缓冲区 */
    if (player->frameBuffer) {
        mrc_free(player->frameBuffer);
        player->frameBuffer = NULL;
    }


    /* 关闭文件 */
    if (player->file) {
        fclose(player->file);
        player->file = NULL;
    }

    /* 注意：壁纸列表由外部管理，这里不释放 */

    LOG_MSG("壁纸资源已释放");
}

/* 读取plays.txt文件 */
int wallpaper_readPlaylist(WALLPAPER_LIST *list) {
    void *data;
    int32 len;
    char *text;

   
    int lineCount;
    int i;
    char *ptr;
    char *lineStart;
    char *comma;

    /* 清零列表 */
    mrc_memset(list, 0, sizeof(WALLPAPER_LIST));

    /* 从assets读取plays.txt */
    data = mrc_readFileFromAssets("plays.txt", &len);
    if (data == NULL || len <= 0) {
        LOG_MSG("无法读取plays.txt");
        return -1;
    }

    /* 分配文本缓冲区 */
    text = (char*)mrc_malloc(len + 1);
    if (!text) {
        LOG_MSG("分配文本缓冲区失败");
        mrc_freeFileFromAssets(data, len);
        return -2;
    }

    /* 复制数据并添加结束符 */
    mrc_memcpy(text, data, len);
    text[len] = '\0';
    mrc_freeFileFromAssets(data, len);

    /* 计算行数并解析文本 */
    lineCount = 0;
    ptr = text;
    lineStart = text;

    /* 第一遍：计算行数 */
    while (*ptr) {
        if (*ptr == '\n') {
            lineCount++;
        }
        ptr++;
    }
    /* 最后一行（如果没有换行符结尾） */
    if (ptr > text && *(ptr - 1) != '\n') {
        lineCount++;
    }

    if (lineCount == 0) {
        LOG_MSG("plays.txt为空");
        mrc_free(text);
        return -3;
    }

    /* 分配壁纸数组 */
    list->wallpapers = (WALLPAPER_INFO*)mrc_malloc(sizeof(WALLPAPER_INFO) * lineCount);
    if (!list->wallpapers) {
        LOG_MSG("分配壁纸数组失败");
        mrc_free(text);
        return -4;
    }

    /* 第二遍：解析每一行 */
    i = 0;
    ptr = text;
    lineStart = text;

    while (*ptr && i < lineCount) {
        /* 查找行结束位置 */
        if (*ptr == '\n' || *ptr == '\r' || *ptr == '\0') {
            int lineLen = ptr - lineStart;

            /* 跳过空行 */
            if (lineLen > 0) {
                /* 复制行内容到临时缓冲区 */
                char lineBuf[64];
                int copyLen = lineLen;
                if (copyLen > 63) copyLen = 63;
                mrc_memcpy(lineBuf, lineStart, copyLen);
                lineBuf[copyLen] = '\0';

                /* 去除末尾的\r字符（Windows换行符） */
                if (copyLen > 0 && lineBuf[copyLen - 1] == '\r') {
                    lineBuf[copyLen - 1] = '\0';
                    copyLen--;
                }

                /* 跳过只有空白字符的行 */
                if (copyLen == 0) {
                    lineStart = ptr + 1;
                    ptr++;
                    continue;
                }

                /* 查找逗号分隔符 */
                comma = (char *)mrc_strchr((const char*)lineBuf, ',');
                if (comma) {
                    /* 格式：壁纸名称,dfa文件名 */
                    *comma = '\0';
                    mrc_strncpy(list->wallpapers[i].name, lineBuf, 31);
                    list->wallpapers[i].name[31] = '\0';

                    mrc_strncpy(list->wallpapers[i].dfaName, comma + 1, 31);
                    list->wallpapers[i].dfaName[31] = '\0';
                } else {
                    /* 只有dfa文件名，使用文件名作为壁纸名称 */
                    mrc_strncpy(list->wallpapers[i].dfaName, lineBuf, 31);
                    list->wallpapers[i].dfaName[31] = '\0';

                    /* 使用文件名作为名称（包含.dfa后缀） */
                    mrc_strncpy(list->wallpapers[i].name, lineBuf, 31);
                    list->wallpapers[i].name[31] = '\0';
                }

                list->wallpapers[i].isExtracted = 0;
                i++;
            }

            lineStart = ptr + 1;
        }
        ptr++;
    }

    list->count = i;
    list->currentIndex = 0;

    mrc_free(text);

    LOG_VAR("成功读取 %d 个壁纸", list->count);
    for (i = 0; i < list->count && i < 5; i++) {
        mrc_printf("壁纸[%d]: %s -> %s", i, list->wallpapers[i].name, list->wallpapers[i].dfaName);
    }

    return 0;
}

/* 释放壁纸列表 */
void wallpaper_freeList(WALLPAPER_LIST *list) {
    if (list->wallpapers) {
        mrc_free(list->wallpapers);
        list->wallpapers = NULL;
    }
    list->count = 0;
    list->currentIndex = 0;
}

/* 解压指定壁纸 */
int wallpaper_extractWallpaper(const char *dfaName) {
    uint8 packName[32];
    char outputPath[128];
    char assetName[64];
    char *endName;
    FILE *outFile;
    void *data;
    int32 len;
    int index;
    int32 totalWritten;
    int32 written;
    int ret;

    /* 获取包名 */
    mrc_GetMrpInfo((char*)mrc_getPackName(), MRP_FILENAME, packName, 32);
    endName = mrc_strrchr((const char *)packName, '.');
    if (endName != NULL) {
        *endName = 0;
    }

    LOG_VAR("开始解压壁纸: %s", dfaName);

    /* 构造输出文件路径：内存卡/包名/{dfaName} */
    mrc_sprintf(outputPath, "%s/%s", packName, dfaName);

    /* 检查是否已解压 */
    if (mrc_fileState(outputPath) == MR_IS_FILE) {
        LOG_MSG("壁纸已存在，跳过解压");
        return 0;
    }

    /* 打开输出文件 */
    outFile = fopen(outputPath, "w+");
    if (!outFile) {
        LOG_MSG("无法创建输出文件");
        return -1;
    }

    /* 循环读取并合并分片 */
    index = 1;
    totalWritten = 0;
    while (1) {
        /* 构造资源文件名：{dfaName}.001, {dfaName}.002, ... */
        mrc_sprintf(assetName, "%s.%03d", dfaName, index);
        mrc_printf("解压：%s =========", assetName);
        /* 从assets读取 */
        data = mrc_readFileFromAssets(assetName, &len);
        if (data == NULL || len <= 0) {
            LOG_VAR("读取失败：%s", assetName);
            /* 读取失败，说明已经读完所有分片 */
            break;
        }

        mrc_printf("[DEBUG] 读取分片 %d, 大小: %d", index, len);

        /* 写入到输出文件 */
        written = fwrite(data, 1, len, outFile);
        if (written != len) {
            LOG_MSG("写入失败");
            mrc_freeFileFromAssets(data, len);
            fclose(outFile);
            return -2;
        }

        totalWritten += written;

        /* 释放读取的数据 */
        mrc_freeFileFromAssets(data, len);

        index++;
    }

    fclose(outFile);

    if (index == 1) {
        LOG_MSG("未找到任何分片文件");
        return -3;
    }

    mrc_printf("[DEBUG] 壁纸解压完成，共 %d 个分片，总大小: %d", index - 1, totalWritten);
    return 0;
}

/* 切换壁纸 */
int wallpaper_switchWallpaper(WALLPAPER_PLAYER *player, int direction) {
    WALLPAPER_LIST *list;
    int newIndex;
    char dfaPath[128];
    uint8 packName[32];
    char *endName;
    int ret;

    if (!player || player->list.count == 0) {
        return -1;
    }

    list = &player->list;

    /* 计算新索引 */
    newIndex = list->currentIndex + direction;
    if (newIndex < 0) {
        newIndex = list->count - 1;
    } else if (newIndex >= list->count) {
        newIndex = 0;
    }

    if (newIndex == list->currentIndex) {
        return 0; /* 没有变化 */
    }

    /* 释放当前壁纸资源 */
    wallpaper_free(player);

    /* 更新当前索引 */
    list->currentIndex = newIndex;

    /* 获取包名 */
    mrc_GetMrpInfo((char*)mrc_getPackName(), MRP_FILENAME, packName, 32);
    endName = mrc_strrchr((const char *)packName, '.');
    if (endName != NULL) {
        *endName = 0;
    }

    /* 构造dfa文件路径 */
    mrc_sprintf(dfaPath, "%s/%s", packName, list->wallpapers[newIndex].dfaName);

    /* 检查文件是否存在，如果不存在则解压 */
    if (mrc_fileState(dfaPath) != MR_IS_FILE) {
        ret = wallpaper_extractWallpaper(list->wallpapers[newIndex].dfaName);
        if (ret != 0) {
            LOG_MSG("解压壁纸失败");
            return -2;
        }
        list->wallpapers[newIndex].isExtracted = 1;
    }

    /* 加载新的dfa文件 */
    ret = wallpaper_loadDfa(player, dfaPath);
    if (ret != 0) {
        LOG_MSG("加载dfa失败");
        return -3;
    }

    /* 加载并绘制第一帧 */
    player->currentFrame = -1;
    wallpaper_nextFrame(player);
    wallpaper_drawFrame(player);

    LOG_VAR("切换到壁纸: %s (%s)", list->wallpapers[newIndex].name, list->wallpapers[newIndex].dfaName);
    return 0;
}

/* 获取当前壁纸名称 */
const char* wallpaper_getCurrentName(WALLPAPER_PLAYER *player) {
    if (!player || player->list.count == 0) {
        return "未知";
    }
    return player->list.wallpapers[player->list.currentIndex].name;
}

/* DFA解码函数 */

/* 解码关键帧 */
int dfa_decodeKeyFrame(WALLPAPER_PLAYER *player, uint8 *data, uint32 dataSize) {
    uint32 totalPixels;
    uint32 expectedSize;
    mrc_printf("解析关键帧 ============");
    totalPixels = player->header.width * player->header.height;
    expectedSize = totalPixels * 2; /* RGB565 = 2字节/像素 */

    if (dataSize != expectedSize) {
        LOG_VAR("关键帧数据大小不匹配: 预期 %d, 实际 %d", expectedSize, dataSize);
        return -1;
    }

    /* 直接复制像素数据到帧缓冲区 */
    mrc_memcpy(player->frameBuffer, data, dataSize);

    return 0;
}

/* 解码差分帧 */
int dfa_decodeDiffFrame(WALLPAPER_PLAYER *player, uint8 *data, uint32 dataSize) {
    uint8 compressionMode;
    uint32 offset;
    uint32 totalPixels;

    if (dataSize < 1) {
        LOG_MSG("差分帧数据太小");
        return -1;
    }

    totalPixels = player->header.width * player->header.height;

    /* frameBuffer已经包含前一帧数据，直接在其基础上更新 */

    /* 读取压缩模式 */
    compressionMode = data[0];
    offset = 1;

    switch (compressionMode) {
        case 1: /* 直接差分 */
            while (offset < dataSize) {
                DFA_DIFF_BLOCK_HEADER blockHeader;
                uint32 i;

                if (offset + sizeof(DFA_DIFF_BLOCK_HEADER) > dataSize) {
                    LOG_MSG("块头数据不完整");
                    return -2;
                }

                /* 读取块头 */
                mrc_memcpy(&blockHeader, &data[offset], sizeof(DFA_DIFF_BLOCK_HEADER));
                offset += sizeof(DFA_DIFF_BLOCK_HEADER);

                /* 检查边界 */
                if (blockHeader.startIdx + blockHeader.length > totalPixels) {
                    LOG_VAR("块超出图像边界: start=%d, length=%d, total=%d",
                           blockHeader.startIdx, blockHeader.length, totalPixels);
                    return -3;
                }

                if (offset + blockHeader.length * 2 > dataSize) {
                    LOG_MSG("块像素数据不完整");
                    return -4;
                }

                /* 应用差分 */
                for (i = 0; i < blockHeader.length; i++) {
                    uint16 pixelValue;
                    mrc_memcpy(&pixelValue, &data[offset + i * 2], 2);
                    player->frameBuffer[blockHeader.startIdx + i] = pixelValue;
                }

                offset += blockHeader.length * 2;
            }
            break;

        case 2: /* RLE压缩 */
            while (offset < dataSize) {
                DFA_RLE_BLOCK rleBlock;
                uint32 i;

                if (offset + sizeof(DFA_RLE_BLOCK) > dataSize) {
                    LOG_MSG("RLE块数据不完整");
                    return -5;
                }

                /* 读取RLE块 */
                mrc_memcpy(&rleBlock, &data[offset], sizeof(DFA_RLE_BLOCK));
                offset += sizeof(DFA_RLE_BLOCK);

                /* 检查边界 */
                if (rleBlock.pixelIdx + rleBlock.repeatCount > totalPixels) {
                    LOG_VAR("RLE块超出图像边界: idx=%d, repeat=%d, total=%d",
                           rleBlock.pixelIdx, rleBlock.repeatCount, totalPixels);
                    return -6;
                }

                /* 应用RLE */
                for (i = 0; i < rleBlock.repeatCount; i++) {
                    player->frameBuffer[rleBlock.pixelIdx + i] = rleBlock.color;
                }
            }
            break;

        default:
            LOG_VAR("不支持的压缩模式: %d", compressionMode);
            return -7;
    }

    return 0;
}

/* 解码跳过帧 */
int dfa_decodeSkipFrame(WALLPAPER_PLAYER *player) {
    /* 跳过帧：frameBuffer已经包含前一帧数据，什么都不用做 */
    return 0;
}

/* 解码矩形区域差分帧 */
int dfa_decodeRectFrame(WALLPAPER_PLAYER *player, uint8 *data, uint32 dataSize) {
    uint8 rectCount;
    uint32 offset;
    uint32 totalPixels;
    uint32 i;

    if (dataSize < 1) {
        LOG_MSG("矩形区域帧数据太小");
        return -1;
    }

    totalPixels = player->header.width * player->header.height;

    /* frameBuffer已经包含前一帧数据，直接在其基础上更新矩形区域 */

    /* 读取矩形数量 */
    rectCount = data[0];
    offset = 1;

    for (i = 0; i < rectCount; i++) {
        DFA_RECT_HEADER rectHeader;
        uint32 rectSize;
        uint32 row, col;

        if (offset + sizeof(DFA_RECT_HEADER) > dataSize) {
            LOG_MSG("矩形头数据不完整");
            return -2;
        }

        /* 读取矩形头 */
        mrc_memcpy(&rectHeader, &data[offset], sizeof(DFA_RECT_HEADER));
        offset += sizeof(DFA_RECT_HEADER);

        /* 检查矩形边界 */
        if (rectHeader.x + rectHeader.width > player->header.width ||
            rectHeader.y + rectHeader.height > player->header.height) {
            LOG_VAR("矩形超出图像边界: x=%d, y=%d, w=%d, h=%d",
                   rectHeader.x, rectHeader.y, rectHeader.width, rectHeader.height);
            return -3;
        }

        /* 计算矩形数据大小 */
        rectSize = rectHeader.width * rectHeader.height * 2;

        if (offset + rectSize > dataSize) {
            LOG_MSG("矩形像素数据不完整");
            return -4;
        }

        /* 应用矩形区域更新 */
        for (row = 0; row < rectHeader.height; row++) {
            uint32 frameIdx = (rectHeader.y + row) * player->header.width + rectHeader.x;
            uint32 dataIdx = offset + row * rectHeader.width * 2;

            mrc_memcpy(&player->frameBuffer[frameIdx], &data[dataIdx], rectHeader.width * 2);
        }

        offset += rectSize;
    }

    return 0;
}
