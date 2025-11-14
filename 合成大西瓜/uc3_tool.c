#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <errno.h>
typedef  uint16_t     uint16;      //有符号16bit整型
typedef  uint32_t  uint32;      //无符号32bit整型
typedef  int32_t                int32;      //有符号32bit整型
typedef  unsigned char      uint8;        //无符号8bit整型
typedef  signed char          int8;        //有符号8bit整型
typedef  int16_t         int16;       //有符号16bit整型
// 计算字形数据大小的宏
#define DATA_SIZE(fontw, fonth) (((fontw * fonth / 8) + (((fontw * fonth % 8) > 0) ? 1 : 0)))
void gbk_to_unicode(const char *gbk_str, wchar_t *unicode_str, int unicode_size) {
    MultiByteToWideChar(CP_ACP, 0, gbk_str, -1, unicode_str, unicode_size);
}

int getUnicodeLength(uint16_t* pText){
    size_t length = 0;
    while (pText[length]!= 0) {
        length++;
    }
    return length;
}
void swapUnicode(uint16_t* pText, size_t length) {
    // 遍历每个字符，交换字节
    for (size_t i = 0; i < length; ++i) {
        // 使用位运算交换字节
        pText[i] = (pText[i] >> 8) | (pText[i] << 8);
    }
}
int32 uc3_small(char *filename, char *outfilename, uint16* pText) {
    printf("准备打开文件\r\n");
    FILE *inFile = fopen(filename, "r+");
    if (!inFile) {
        printf("打开失败\r\n");
         DWORD error = GetLastError();
        printf("Error opening %s: %s %ld\r\n", filename, strerror(errno), error);
        return -1; // 文件打开失败
    }
    printf("开始交换文字\r\n");
    swapUnicode(pText, getUnicodeLength(pText));
    // 读取文件头
    char prefix[4];
    fread(prefix, 1, 4, inFile);
    if (strncmp(prefix, "FT16", 4) != 0) {
        fclose(inFile);
        printf("文件格式错误\r\n");
        return -2; // 文件格式错误
    }
    printf("开始读取\r\n");
    uint32_t fontw, fonth;
    uint16 charCount;
    fread(&fontw, sizeof(uint32_t), 1, inFile);
    fread(&fonth, sizeof(uint32_t), 1, inFile);
    printf("fontw=%d fonth=%d\r\n", fontw, fonth);
    fread(&charCount, 1, 2, inFile);
  
    printf("读取到%d\r\n", (int)charCount);
    // 读取所有字形信息
    uint16_t *ids = malloc(charCount * sizeof(uint16_t));
    uint32_t *offsets = malloc(charCount * sizeof(uint32_t));
    fread(ids, sizeof(uint16_t), charCount, inFile);
    fread(offsets, sizeof(uint32_t), charCount, inFile);

    // 计算过滤的字形
    uint32_t outputSize = 0;
    uint16_t *filteredIds = malloc(charCount * sizeof(uint16_t));
    uint32_t *filteredOffsets = malloc(charCount * sizeof(uint32_t));
    uint16 filteredCount = 0;

    for (uint32_t i = 0; pText[i] != 0; ++i) { // 假设pText以0结尾
        for (uint32_t j = 0; j < charCount; ++j) {
            if (ids[j] == pText[i]) {
                filteredIds[filteredCount] = ids[j];
                filteredOffsets[filteredCount] = offsets[j];
                outputSize += DATA_SIZE(fontw, fonth);
                filteredCount++;
                break; // 找到后直接跳到下一个字符
            }
        }
    }
    printf("开始写文件");
    // 写入新文件
    FILE *outFile = fopen(outfilename, "w+");
    if (!outFile) {
        free(ids);
        free(offsets);
        free(filteredIds);
        free(filteredOffsets);
        fclose(inFile);
        printf("写入失败 文件打开失败");
        return -3; // 输出文件打开失败
    }

    fwrite("FT16", 1, 4, outFile);
    fwrite(&fontw, sizeof(uint32_t), 1, outFile);
    fwrite(&fonth, sizeof(uint32_t), 1, outFile);
    fwrite(&filteredCount, sizeof(uint16), 1, outFile);
    
    fwrite(filteredIds, sizeof(uint16_t), filteredCount, outFile);
    fwrite(filteredOffsets, sizeof(uint32_t), filteredCount, outFile);

    // 写入每个字形数据
    for (uint32_t i = 0; i < filteredCount; ++i) {
        fseek(inFile, offsets[i], SEEK_SET);
        uint32_t dataSize = DATA_SIZE(fontw, fonth);
        uint8_t *data = malloc(dataSize);
        fread(data, 1, dataSize, inFile);
        fwrite(data, 1, dataSize, outFile);
        free(data);
    }

    // 释放内存
    free(ids);
    free(offsets);
    free(filteredIds);
    free(filteredOffsets);

    fclose(inFile);
    fclose(outFile);
    printf("写入成功\r\n");

    return 0; // 成功
}

int main(int argc, char *argv[]){
    for(int i=0;i<argc;i++){
        printf("%s ", argv[i]);
    }
    if(argc < 4){
        printf("Usage: %s <input_font.uc3> <output_font.uc3> <unicode_text>\r\n", argv[0]);
        return -1;
    }
    int len = strlen(argv[3]) * 2 +2;
     wchar_t *unicode_str = malloc(len); // 定义一个足够大的数组来存储转换后的Unicode字符串
     memset(unicode_str, 0, len);
    printf("转换编码\r\n");
    gbk_to_unicode(argv[3], unicode_str, len);
    wprintf(unicode_str);
    printf("开始精简\r\n");
    uc3_small(argv[1], argv[2], (uint16*)unicode_str);
    return 0;
}