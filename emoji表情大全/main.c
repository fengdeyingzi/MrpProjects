#include <mrc_base.h>

#include "uc3_font.h"
#include "mpc.h"
#include "mrc_graphics.h"
#include "fopen.h"
#include "xl_debug.h"

/* bmas图集相关结构定义 */
typedef struct {
    char magic[4];        /* "IMGS" */
    int32 imageCount;     /* 图像数量 */
} BMAS_HEADER;

typedef struct {
    char name[32];        /* 图片名(31字节+\0) */
    char mapType[4];      /* "MAP5" or "MAP8" */
    int32 width;         /* 宽度 */
    int32 height;        /* 高度 */
    uint16 transColor;    /* 透明色 */
    int32 offset;        /* 像素数据偏移 */
} BMAS_IMAGE_INFO;

typedef struct {
    FILE *file;                 /* bmas文件句柄 */
    int32 imageCount;          /* 图片总数 */
    BMAS_IMAGE_INFO *imageInfos; /* 图片信息表 */
} BMAS_PLAYER;

/* 窗口类型枚举 */
enum {
    WINDOW_LIST = 0,    /* emoji列表窗口 */
    WINDOW_DETAIL = 1,   /* emoji详情窗口 */
    WINDOW_UNPACK = 2, /* 解压资源 */
};

/* 全局变量声明 */
int32 timer_cd;
int32 currentWindow;
int32 current_page;
int32 current_selection;
int32 total_emojis;
int32 emoji_per_page;
int32 emoji_rows;
int32 emoji_cols;
int32 emoji_size;
int32 emoji_spacing;
char *emoji_list;
int32 emoji_list_len;
BITMAP_565 *emoji_bitmaps[64];
BITMAP_565 *current_emoji;
int isUnPacking;
BMAS_PLAYER bmas_player;  /* bmas图集播放器 */

/* 函数声明 */
void drawListWindow(void);
void drawDetailWindow(void);
void drawSoftKeyHints(char* left, char* right);
int getUtf8CharLength(unsigned char first_byte);
void loadEmojiList(void);
int getEmojiCharOffset(int index);
void getEmojiChar(int index, char* buffer, int buffer_size);
void getEmojiHexValue(int index, char* buffer, int buffer_size);
void loadEmojiBitmaps(int32 start_index);
void freeEmojiBitmaps(void);
void handleListKeyEvent(int32 key);
void handleDetailKeyEvent(int32 key);
void handleListTouchEvent(int32 x, int32 y);
void handleDetailTouchEvent(int32 x, int32 y);
void drawCenterUI(char *text);
int extractResources(void);

/* bmas图集相关函数 */
int loadBmasFile(const char *filename);
int findEmojiIndexInBmas(const char *emoji_name);
BITMAP_565* loadEmojiFromBmas(const char *emoji_name);
void freeBmasResources(void);


/* 定时器回调函数 */
void timer_run(int32 id)
{
    /* 只在列表窗口时更新显示 */
    if (currentWindow == WINDOW_LIST) {
        drawListWindow();
    } else if (currentWindow == WINDOW_DETAIL) {
        drawDetailWindow();
    }
    else if(currentWindow == WINDOW_UNPACK){
        drawCenterUI("解压资源中...");
        if(isUnPacking){
            return;
        }
        extractResources();
    }
    return;
}

/* 绘制软键提示 */
void drawSoftKeyHints(char* left, char* right)
{
    int leftX;
    int rightX;
    int y;
    int textW;

    y = SCRH - 25;  /* 距离底部25像素 */

    /* 左软键提示（屏幕左下） */
    if (left != NULL) {
        leftX = 10;
        uc3_drawText(left, leftX, y, 255, 255, 255, 0);
    }

    /* 右软键提示（屏幕右下） */
    if (right != NULL) {
        textW = uc3_getWidth(right, 0);
        rightX = SCRW - textW - 10;
        uc3_drawText(right, rightX, y, 255, 255, 255, 0);
    }
}

/* 判断UTF-8字符长度 */
int getUtf8CharLength(unsigned char first_byte)
{
    if ((first_byte & 0x80) == 0x00) {
        return 1;  /* ASCII字符 */
    } else if ((first_byte & 0xE0) == 0xC0) {
        return 2;  /* 2字节UTF-8 */
    } else if ((first_byte & 0xF0) == 0xE0) {
        return 3;  /* 3字节UTF-8 */
    } else if ((first_byte & 0xF8) == 0xF0) {
        return 4;  /* 4字节UTF-8 */
    }
    return 1;  /* 默认返回1 */
}

void drawCenterUI(char *text){
  int textWidth;
  mrc_clearScreen(20, 20, 20);
  textWidth = uc3_getWidth(text, 0);
  uc3_drawText(text, (SCRW - textWidth) / 2, (SCRH - 16) / 2, 255, 255, 255, 0);
  mrc_refreshScreen(0, 0, SCRW, SCRH);
}

/* 资源解压：合并emoji.bmas.001到emoji.bmas.xxx */
int extractResources(void) {
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
    if(isUnPacking){
        return 0;
    }
    isUnPacking = TRUE;
    drawCenterUI("解压资源中...");
     mrc_GetMrpInfo((char*)mrc_getPackName(), MRP_FILENAME, packName, 32);
      endName = mrc_strrchr((const char *)packName, '.');
      if(endName != NULL){
        *endName = 0;
      }
    LOG_MSG("开始资源解压...");

    /* 构造输出文件路径：内存卡/包名/24.bmas */
    mrc_sprintf(outputPath, "%s", packName);
    LOG_VAR("应用程序目录：%s", outputPath);
    /* 检查目录是否存在，不存在则创建 */
    if (mrc_fileState(outputPath) != MR_IS_DIR) {
        LOG_MSG("创建目录");
        ret = mrc_mkDir(outputPath);
        if (ret != MR_SUCCESS) {
            LOG_MSG("创建目录失败");
            return -1;
        }
    }

    /* 追加文件名 */
    mrc_strcat(outputPath, "/emoji.bmas");

    /* 检查是否已解压 */
    if (mrc_fileState(outputPath) == MR_IS_FILE) {
        LOG_MSG("资源已存在，跳过解压");
        return 0;
    }

    /* 打开输出文件 */
    LOG_MSG("创建输出文件");
    outFile = fopen(outputPath, "w+");
    if (!outFile) {
        LOG_MSG("无法创建输出文件");
        return -2;
    }

    /* 循环读取并合并分片 */
    index = 1;
    totalWritten = 0;
    while (1) {
        /* 构造资源文件名：24.bmas.001, 24.bmas.002, ... */
        mrc_sprintf(assetName, "emoji.bmas.%03d", index);

        /* 从assets读取 */
        data = mrc_readFileFromAssets(assetName, &len);
        if (data == NULL || len <= 0) {
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
            return -3;
        }

        totalWritten += written;

        /* 释放读取的数据 */
        mrc_freeFileFromAssets(data, len);

        index++;
    }

    fclose(outFile);

    mrc_printf("[DEBUG] 资源解压完成，共 %d 个分片，总大小: %d 剩余内存：%d", index - 1, totalWritten, mrc_getMemoryRemain());

    /* 加载bmas文件 */
    if (loadBmasFile(outputPath) != 0) {
        LOG_MSG("加载bmas文件失败");
        currentWindow = WINDOW_LIST;
        isUnPacking = FALSE;
        return -4;
    }

    currentWindow = WINDOW_LIST;
    
    // 加载第一页
    loadEmojiBitmaps(0);
    isUnPacking = FALSE;
    return 0;
}

/* 加载bmas文件 */
int loadBmasFile(const char *filename) {
    BMAS_HEADER header;
    uint32 i;
    int32 readSize;

    mrc_printf("[DEBUG] 加载bmas文件: %s", filename);

    /* 打开文件 */
    bmas_player.file = fopen(filename, "rb");
    if (!bmas_player.file) {
        LOG_MSG("无法打开bmas文件");
        return -1;
    }

    /* 读取文件头 */
    readSize = fread(&header, 1, sizeof(BMAS_HEADER), bmas_player.file);
    if (readSize != sizeof(BMAS_HEADER)) {
        LOG_MSG("读取文件头失败");
        fclose(bmas_player.file);
        bmas_player.file = NULL;
        return -2;
    }

    /* 验证魔数 */
    if (mrc_memcmp(header.magic, "IMGS", 4) != 0) {
        LOG_MSG("文件格式错误");
        fclose(bmas_player.file);
        bmas_player.file = NULL;
        return -3;
    }

    bmas_player.imageCount = header.imageCount;
    LOG_VAR("imageCount = %d", bmas_player.imageCount);

    /* 申请图片信息表 */
    bmas_player.imageInfos = (BMAS_IMAGE_INFO*)mrc_malloc(sizeof(BMAS_IMAGE_INFO) * bmas_player.imageCount);
    if (!bmas_player.imageInfos) {
        LOG_MSG("申请图片信息表失败");
        fclose(bmas_player.file);
        bmas_player.file = NULL;
        return -4;
    }

    LOG_VAR("图片信息结构体长度：%d", sizeof(BMAS_IMAGE_INFO));
    /* 读取所有图片信息 */
    for (i = 0; i < bmas_player.imageCount; i++) {
        fseek(bmas_player.file, sizeof(BMAS_HEADER) + i * 50, SEEK_SET);
        readSize = fread(bmas_player.imageInfos[i].name, 1, sizeof(bmas_player.imageInfos[i].name), bmas_player.file);
        readSize += fread(bmas_player.imageInfos[i].mapType, 1, sizeof(bmas_player.imageInfos[i].mapType), bmas_player.file);
        readSize += fread(&bmas_player.imageInfos[i].width, 1, sizeof(bmas_player.imageInfos[i].width), bmas_player.file);
        readSize += fread(&bmas_player.imageInfos[i].height, 1, sizeof(bmas_player.imageInfos[i].height), bmas_player.file);
        readSize += fread(&bmas_player.imageInfos[i].transColor, 1, sizeof(bmas_player.imageInfos[i].transColor), bmas_player.file);
        readSize += fread(&bmas_player.imageInfos[i].offset, 1, sizeof(bmas_player.imageInfos[i].offset), bmas_player.file);
        if (readSize != 50) {
            mrc_printf("[DEBUG] 读取图片信息失败: %d", i);
            return -5;
        }
        if(i<32)
            mrc_printf("读取图片 图片宽度：%d 图片高度：%d 文件名:%s 数据偏移：%d",
                      bmas_player.imageInfos[i].width,
                      bmas_player.imageInfos[i].height,
                      bmas_player.imageInfos[i].name,
                      bmas_player.imageInfos[i].offset);
    }

    LOG_MSG("bmas加载成功");
    return 0;
}

/* 通过emoji名称在bmas中查找对应的索引 */
int findEmojiIndexInBmas(const char *emoji_name) {
    uint32 i;
    char expected_name[32];

    /* 构建预期的文件名：emoji名称_32x32 */
    mrc_sprintf(expected_name, "%s_32x32", emoji_name);

    /* 在图片信息表中查找匹配的文件名 */
    for (i = 0; i < bmas_player.imageCount; i++) {
        if (mrc_strcmp(bmas_player.imageInfos[i].name, expected_name) == 0) {
            mrc_printf("[DEBUG] 找到emoji '%s' 在bmas中的索引: %d", emoji_name, i);
            return i;
        }
    }

    mrc_printf("[WARN] 未在bmas中找到emoji: %s", emoji_name);
    return -1;
}

/* 从bmas文件加载指定名称的emoji */
BITMAP_565* loadEmojiFromBmas(const char *emoji_name) {
    BMAS_IMAGE_INFO *info;
    int32 imageSize;
    int32 readSize;
    int index;
    BITMAP_565 *bitmap;

    /* 通过名称查找索引 */
    index = findEmojiIndexInBmas(emoji_name);
    if (index < 0) {
        return NULL;
    }

    if (!bmas_player.file) {
        return NULL;
    }

    if (index >= bmas_player.imageCount) {
        return NULL;
    }

    /* 获取图片信息 */
    info = &bmas_player.imageInfos[index];

    /* 计算图像数据大小 */
    imageSize = info->width * info->height * 2; /* RGB565 = 2字节/像素 */

    /* 创建bitmap结构 */
    bitmap = (BITMAP_565*)mrc_malloc(sizeof(BITMAP_565));
    if (!bitmap) {
        LOG_MSG("申请bitmap结构失败");
        return NULL;
    }
    mrc_memset(bitmap, 0, sizeof(BITMAP_565));
    bitmap->bitmap = NULL;
    bitmap->buflen = 0;

    /* 申请bitmap缓存 */
    mrc_printf("开始申请图像内存：%d 剩余内存：%d", imageSize, mrc_getMemoryRemain());
    bitmap->bitmap = (uint16*)mr_malloc(imageSize);
    if (!bitmap->bitmap) {
        mrc_printf("[DEBUG] 申请bitmap缓存失败，大小: %d", imageSize);
        mrc_printf("剩余内存：%d", mrc_getMemoryRemain());
        mrc_free(bitmap);
        return NULL;
    }
    bitmap->buflen = imageSize;

    /* 更新bitmap属性 */
    bitmap->width = info->width;
    bitmap->height = info->height;
    bitmap->color_bit = 16;
    bitmap->transcolor = info->transColor;
    bitmap->mode = (info->transColor != 0) ? BM_TRANSPARENT : BM_COPY;

    /* 定位到图像数据位置 */
    fseek(bmas_player.file, info->offset, SEEK_SET);

    /* 读取图像数据到bitmap缓存 */
    readSize = fread(bitmap->bitmap, 1, imageSize, bmas_player.file);
    if (readSize != imageSize) {
        LOG_VAR("读取图像数据失败 offset = %d", info->offset);
        mrc_printf("readSize = %d, imageSize = %d", readSize, imageSize);
        mrc_freeFileData(bitmap->bitmap, bitmap->buflen);
        mrc_free(bitmap);
        return NULL;
    }

    return bitmap;
}

/* 释放bmas资源 */
void freeBmasResources(void) {
    /* 释放图片信息表 */
    if (bmas_player.imageInfos) {
        mrc_free(bmas_player.imageInfos);
        bmas_player.imageInfos = NULL;
    }

    /* 关闭文件 */
    if (bmas_player.file) {
        fclose(bmas_player.file);
        bmas_player.file = NULL;
    }

    bmas_player.imageCount = 0;
    LOG_MSG("bmas资源已释放");
}

/* 加载emoji列表 */
void loadEmojiList(void)
{
    int32 filelen;
    int i;
    int char_len;
    int emoji_count;

    mrc_printf("[INFO] Loading emoji list...\n");

    /* 从mrp中读取emoji.txt文件 */
    emoji_list = mrc_readFileFromMrp("emoji.txt", &filelen, 0);
    if (emoji_list == NULL) {
        mrc_printf("[ERROR] Failed to load emoji.txt\n");
        emoji_list_len = 0;
        total_emojis = 0;
        return;
    }

    emoji_list_len = filelen;

    /* 统计emoji数量（正确处理UTF-8字符） */
    emoji_count = 0;
    i = 0;
    while (i < emoji_list_len) {
        char_len = getUtf8CharLength(emoji_list[i]);
        i += char_len;
        emoji_count++;
    }

    total_emojis = emoji_count;
    mrc_printf("[INFO] Loaded %d emojis, file size: %d\n", total_emojis, emoji_list_len);
}

/* 获取第n个emoji字符的起始位置 */
int getEmojiCharOffset(int index)
{
    int i;
    int current_index;
    int char_len;

    current_index = 0;
    i = 0;

    while (current_index < index && i < emoji_list_len) {
        char_len = getUtf8CharLength(emoji_list[i]);
        i += char_len;
        current_index++;
    }

    return i;
}

/* 获取第n个emoji字符 */
void getEmojiChar(int index, char* buffer, int buffer_size)
{
    int offset;
    int char_len;
    int i;

    offset = getEmojiCharOffset(index);
    if (offset >= emoji_list_len) {
        buffer[0] = '\0';
        return;
    }

    char_len = getUtf8CharLength(emoji_list[offset]);
    if (char_len > buffer_size - 1) {
        char_len = buffer_size - 1;
    }

    for (i = 0; i < char_len; i++) {
        buffer[i] = emoji_list[offset + i];
    }
    buffer[char_len] = '\0';
}

/* 获取第n个emoji字符的十六进制值 */
void getEmojiHexValue(int index, char* buffer, int buffer_size)
{
    int offset;
    int char_len;
    int i;
    unsigned char byte;
    char hex_byte[3];

    offset = getEmojiCharOffset(index);
    if (offset >= emoji_list_len) {
        buffer[0] = '\0';
        return;
    }

    char_len = getUtf8CharLength(emoji_list[offset]);

    /* 确保缓冲区足够大：每个字节最多需要2个十六进制字符 + 空格 + 结尾符 */
    if (char_len * 3 > buffer_size - 1) {
        buffer[0] = '\0';
        return;
    }

    /* 将每个字节转换为十六进制 */
    buffer[0] = '\0';
    for (i = 0; i < char_len; i++) {
        byte = (unsigned char)emoji_list[offset + i];
        if (i > 0) {
            mrc_strcat(buffer, " ");
        }
        /* 将字节转换为十六进制字符串 */
        
        hex_byte[0] = "0123456789ABCDEF"[byte >> 4];
        hex_byte[1] = "0123456789ABCDEF"[byte & 0x0F];
        hex_byte[2] = '\0';
        mrc_strcat(buffer, hex_byte);
    }
}

/* 加载emoji图片 */
void loadEmojiBitmaps(int32 start_index)
{
    int i;
    int loaded_count;
    char emoji_char[5];
    int ret;

    /* 先释放之前加载的图片 */
    freeEmojiBitmaps();

    /* 计算本次需要加载的数量（最多64个） */
    loaded_count = total_emojis - start_index;
    if (loaded_count > 64) {
        loaded_count = 64;
    }

    mrc_printf("[INFO] Loading emoji bitmaps from index %d, count: %d\n", start_index, loaded_count);

    /* 检查bmas文件是否已加载 */
    if (!bmas_player.file || bmas_player.imageCount == 0) {
        mrc_printf("[WARN] bmas文件未加载，无法加载emoji图片");
        return;
    }

    /* 加载指定范围的emoji图片 */
    for (i = 0; i < loaded_count; i++) {
        int emoji_index = start_index + i;

        /* 确保索引有效 */
        if (emoji_index >= total_emojis) {
            break;
        }

        /* 获取emoji字符 */
        getEmojiChar(emoji_index, emoji_char, sizeof(emoji_char));

        /* 从bmas文件加载emoji图片（通过名称匹配） */
        emoji_bitmaps[i] = loadEmojiFromBmas(emoji_char);
        if (emoji_bitmaps[i] == NULL) {
            mrc_printf("[WARN] Failed to load emoji '%s' from bmas", emoji_char);
        } else {
            mrc_printf("[DEBUG] Successfully loaded emoji '%s' from bmas", emoji_char);
        }
    }
}

/* 释放emoji图片 */
void freeEmojiBitmaps(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        if (emoji_bitmaps[i] != NULL) {
            bitmapFree(emoji_bitmaps[i]);
            emoji_bitmaps[i] = NULL;
        }
    }
}

/* 绘制列表窗口 */
void drawListWindow(void)
{
    int i;
    int j;
    int index;
    int start_x;
    int start_y;
    int cell_width;
    int cell_height;
    int emoji_x;
    int emoji_y;
    int cursor_x;
    int cursor_y;
    char page_info[50];

    /* 清屏 */
    mrc_clearScreen(0, 0, 0);

    /* 计算布局 */
    cell_width = emoji_size + emoji_spacing;
    cell_height = emoji_size + emoji_spacing;
    emoji_cols = (SCRW - emoji_spacing) / cell_width;
    emoji_rows = (SCRH - 50) / cell_height;  /* 预留底部空间给软键提示 */
    emoji_per_page = emoji_cols * emoji_rows;

    start_x = (SCRW - emoji_cols * cell_width) / 2;
    start_y = 10;

    /* 绘制当前页的emoji */
    for (i = 0; i < emoji_rows; i++) {
        for (j = 0; j < emoji_cols; j++) {
            index = current_page * emoji_per_page + i * emoji_cols + j;
            if (index >= total_emojis) {
                break;
            }

            emoji_x = start_x + j * cell_width;
            emoji_y = start_y + i * cell_height;

            /* 绘制emoji图片 */
            if (emoji_bitmaps[(i * emoji_cols + j) % 64] != NULL) {
                drawBitmap(emoji_bitmaps[(i * emoji_cols + j) % 64], emoji_x, emoji_y);
            }

            /* 绘制光标（如果当前选中） */
            if (index == current_selection) {
                cursor_x = emoji_x - 2;
                cursor_y = emoji_y - 2;
                gl_drawHollowRect(cursor_x, cursor_y, emoji_size + 4, emoji_size + 4, 0xFFFF0000);
            }
        }
    }

    /* 绘制页码信息 */
    mrc_sprintf(page_info, "第 %d/%d 页", current_page + 1, (total_emojis + emoji_per_page - 1) / emoji_per_page);
    uc3_drawText(page_info, 10, SCRH - 40, 255, 255, 255, 0);

    /* 绘制软键提示 */
    drawSoftKeyHints("查看", "退出");

    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

/* 绘制详情窗口 */
void drawDetailWindow(void)
{
    int center_x;
    int center_y;
    int detail_size;
    char emoji_char[4];
    char hex_value[32];

    /* 清屏 */
    mrc_clearScreen(0, 0, 0);

    if (current_emoji != NULL) {
        /* 计算放大后的尺寸（屏幕宽度的70%） */
        detail_size = SCRW * 70 / 100;
        if (detail_size > SCRH - 100) {
            detail_size = SCRH - 100;
        }

        center_x = (SCRW - detail_size) / 2;
        center_y = (SCRH - detail_size) / 2 - 20;

        /* 绘制放大的emoji */
        drawBitmapEx(current_emoji, center_x, center_y, detail_size, detail_size, 0, 0, 32, 32);

        /* 显示emoji字符和十六进制值（如果可能） */
        if (current_selection < total_emojis) {
            getEmojiChar(current_selection, emoji_char, sizeof(emoji_char));
            uc3_drawText(emoji_char, center_x, center_y + detail_size + 10, 255, 255, 255, 0);

            /* 获取并显示十六进制值 */
            getEmojiHexValue(current_selection, hex_value, sizeof(hex_value));
            uc3_drawText(hex_value, center_x, center_y + detail_size + 30, 200, 200, 200, 0);
        }
    }

    /* 绘制软键提示 */
    drawSoftKeyHints(NULL, "返回");

    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

/* 列表窗口按键事件处理 */
void handleListKeyEvent(int32 key)
{
    int new_selection;
    int total_pages;

    total_pages = (total_emojis + emoji_per_page - 1) / emoji_per_page;

    switch (key) {
        case _UP:
            new_selection = current_selection - emoji_cols;
            if (new_selection >= 0) {
                current_selection = new_selection;
                if (current_selection < current_page * emoji_per_page) {
                    current_page--;
                    loadEmojiBitmaps(current_page * emoji_per_page);
                }
            }
            break;

        case _DOWN:
            new_selection = current_selection + emoji_cols;
            if (new_selection < total_emojis) {
                current_selection = new_selection;
                if (current_selection >= (current_page + 1) * emoji_per_page) {
                    current_page++;
                    loadEmojiBitmaps(current_page * emoji_per_page);
                }
            }
            break;

        case _LEFT:
            if (current_selection % emoji_cols > 0) {
                current_selection--;
            }
            break;

        case _RIGHT:
            if (current_selection % emoji_cols < emoji_cols - 1 && current_selection + 1 < total_emojis) {
                current_selection++;
            }
            break;

        case _SELECT:
        case _SLEFT:
            /* 进入详情窗口 */
            if (current_selection < total_emojis) {
                current_emoji = emoji_bitmaps[(current_selection - current_page * emoji_per_page) % 64];
                currentWindow = WINDOW_DETAIL;
                drawDetailWindow();
            }
            break;

        case _SRIGHT:
            /* 退出程序 */
            mrc_exit();
            break;
    }
}

/* 详情窗口按键事件处理 */
void handleDetailKeyEvent(int32 key)
{
    if (key == _SRIGHT || key == _SELECT) {
        /* 返回列表窗口 */
        currentWindow = WINDOW_LIST;
        drawListWindow();
    }
}

/* 列表窗口触摸事件处理 */
void handleListTouchEvent(int32 x, int32 y)
{
    int i;
    int j;
    int index;
    int start_x;
    int start_y;
    int cell_width;
    int cell_height;
    int emoji_x;
    int emoji_y;

    /* 计算布局 */
    cell_width = emoji_size + emoji_spacing;
    cell_height = emoji_size + emoji_spacing;
    start_x = (SCRW - emoji_cols * cell_width) / 2;
    start_y = 10;

    /* 检查点击了哪个emoji */
    for (i = 0; i < emoji_rows; i++) {
        for (j = 0; j < emoji_cols; j++) {
            index = current_page * emoji_per_page + i * emoji_cols + j;
            if (index >= total_emojis) {
                break;
            }

            emoji_x = start_x + j * cell_width;
            emoji_y = start_y + i * cell_height;

            /* 检查点击是否在emoji区域内 */
            if (x >= emoji_x && x < emoji_x + emoji_size &&
                y >= emoji_y && y < emoji_y + emoji_size) {
                current_selection = index;
                current_emoji = emoji_bitmaps[(i * emoji_cols + j) % 64];
                currentWindow = WINDOW_DETAIL;
                drawDetailWindow();
                return;
            }
        }
    }
}

/* 详情窗口触摸事件处理 */
void handleDetailTouchEvent(int32 x, int32 y)
{
    /* 点击任意位置返回列表 */
    currentWindow = WINDOW_LIST;
    drawListWindow();
}

int32 mrc_init(void)
{
    int i;

    mrc_printf("[INIT] Starting emoji app...\n");

    /* 初始化基础功能 */
    mpc_init();

    /* 初始化字体 */
    uc3_init();

    /* 初始化随机数种子 */
    sand(getuptime());

    /* 初始化全局变量 */
    currentWindow = WINDOW_UNPACK;
    current_page = 0;
    current_selection = 0;
    total_emojis = 0;
    emoji_per_page = 0;
    emoji_rows = 0;
    emoji_cols = 0;
    emoji_size = 32;
    emoji_spacing = 10;
    emoji_list = NULL;
    emoji_list_len = 0;
    current_emoji = NULL;
    isUnPacking = FALSE;

    /* 初始化bmas播放器 */
    bmas_player.file = NULL;
    bmas_player.imageCount = 0;
    bmas_player.imageInfos = NULL;

    for (i = 0; i < 64; i++) {
        emoji_bitmaps[i] = NULL;
    }

    /* 加载emoji列表 */
    loadEmojiList();

    /* 加载第一页emoji图片 */
    if (total_emojis > 0) {
        loadEmojiBitmaps(0);
    }

    /* 创建定时器用于屏幕刷新（30fps） */
    timer_cd = mrc_timerCreate();
    drawCenterUI("解压资源中...");
    mrc_timerStart(timer_cd, 33, 0, timer_run, 1);

    mrc_printf("[INIT] Emoji app initialized successfully\n");

    return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    if (currentWindow == WINDOW_LIST) {
        if (code == KY_UP) {
            /* 列表窗口按键松开事件 */
            handleListKeyEvent(param0);
        } else if (code == MS_UP) {
            /* 列表窗口触摸松开事件 */
            handleListTouchEvent(param0, param1);
        }
    } else if (currentWindow == WINDOW_DETAIL) {
        if (code == KY_UP) {
            /* 详情窗口按键松开事件 */
            handleDetailKeyEvent(param0);
        } else if (code == MS_UP) {
            /* 详情窗口触摸松开事件 */
            handleDetailTouchEvent(param0, param1);
        }
    }
    return 0;
}

int32 mrc_pause(void)
{
    /* 暂停时停止定时器 */
    mrc_timerStop(timer_cd);
    return 0;
}

int32 mrc_resume(void)
{
    /* 恢复时重新启动定时器 */
    mrc_timerStart(timer_cd, 33, 0, timer_run, 1);
    return 0;
}

int32 mrc_exitApp(void)
{
    /* 停止定时器 */
    mrc_timerStop(timer_cd);
    mrc_timerDelete(timer_cd);

    /* 释放emoji图片 */
    freeEmojiBitmaps();

    /* 释放bmas资源 */
    freeBmasResources();

    /* 释放emoji列表 */
    if (emoji_list != NULL) {
        mrc_freeFileData(emoji_list, emoji_list_len);
        emoji_list = NULL;
    }

    /* 释放字体 */
    uc3_free();

    mrc_printf("[EXIT] Emoji app exited\n");

    return 0;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
    mrc_printf("mrc_extRecvAppEvent");
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3,
                            int32 p4, int32 p5)
{
    mrc_printf("mrc_extRecvAppEventEx");
    return 0;
}