#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <errno.h>
typedef  uint16_t     uint16;      //�з���16bit����
typedef  uint32_t  uint32;      //�޷���32bit����
typedef  int32_t                int32;      //�з���32bit����
typedef  unsigned char      uint8;        //�޷���8bit����
typedef  signed char          int8;        //�з���8bit����
typedef  int16_t         int16;       //�з���16bit����
// �����������ݴ�С�ĺ�
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
    // ����ÿ���ַ��������ֽ�
    for (size_t i = 0; i < length; ++i) {
        // ʹ��λ���㽻���ֽ�
        pText[i] = (pText[i] >> 8) | (pText[i] << 8);
    }
}
int32 uc3_small(char *filename, char *outfilename, uint16* pText) {
    printf("׼�����ļ�\r\n");
    FILE *inFile = fopen(filename, "r+");
    if (!inFile) {
        printf("��ʧ��\r\n");
         DWORD error = GetLastError();
        printf("Error opening %s: %s %ld\r\n", filename, strerror(errno), error);
        return -1; // �ļ���ʧ��
    }
    printf("��ʼ��������\r\n");
    swapUnicode(pText, getUnicodeLength(pText));
    // ��ȡ�ļ�ͷ
    char prefix[4];
    fread(prefix, 1, 4, inFile);
    if (strncmp(prefix, "FT16", 4) != 0) {
        fclose(inFile);
        printf("�ļ���ʽ����\r\n");
        return -2; // �ļ���ʽ����
    }
    printf("��ʼ��ȡ\r\n");
    uint32_t fontw, fonth;
    uint16 charCount;
    fread(&fontw, sizeof(uint32_t), 1, inFile);
    fread(&fonth, sizeof(uint32_t), 1, inFile);
    printf("fontw=%d fonth=%d\r\n", fontw, fonth);
    fread(&charCount, 1, 2, inFile);
  
    printf("��ȡ��%d\r\n", (int)charCount);
    // ��ȡ����������Ϣ
    uint16_t *ids = malloc(charCount * sizeof(uint16_t));
    uint32_t *offsets = malloc(charCount * sizeof(uint32_t));
    fread(ids, sizeof(uint16_t), charCount, inFile);
    fread(offsets, sizeof(uint32_t), charCount, inFile);

    // ������˵�����
    uint32_t outputSize = 0;
    uint16_t *filteredIds = malloc(charCount * sizeof(uint16_t));
    uint32_t *filteredOffsets = malloc(charCount * sizeof(uint32_t));
    uint16 filteredCount = 0;

    for (uint32_t i = 0; pText[i] != 0; ++i) { // ����pText��0��β
        for (uint32_t j = 0; j < charCount; ++j) {
            if (ids[j] == pText[i]) {
                filteredIds[filteredCount] = ids[j];
                filteredOffsets[filteredCount] = offsets[j];
                outputSize += DATA_SIZE(fontw, fonth);
                filteredCount++;
                break; // �ҵ���ֱ��������һ���ַ�
            }
        }
    }
    printf("��ʼд�ļ�");
    // д�����ļ�
    FILE *outFile = fopen(outfilename, "w+");
    if (!outFile) {
        free(ids);
        free(offsets);
        free(filteredIds);
        free(filteredOffsets);
        fclose(inFile);
        printf("д��ʧ�� �ļ���ʧ��");
        return -3; // ����ļ���ʧ��
    }

    fwrite("FT16", 1, 4, outFile);
    fwrite(&fontw, sizeof(uint32_t), 1, outFile);
    fwrite(&fonth, sizeof(uint32_t), 1, outFile);
    fwrite(&filteredCount, sizeof(uint16), 1, outFile);
    
    fwrite(filteredIds, sizeof(uint16_t), filteredCount, outFile);
    fwrite(filteredOffsets, sizeof(uint32_t), filteredCount, outFile);

    // д��ÿ����������
    for (uint32_t i = 0; i < filteredCount; ++i) {
        fseek(inFile, offsets[i], SEEK_SET);
        uint32_t dataSize = DATA_SIZE(fontw, fonth);
        uint8_t *data = malloc(dataSize);
        fread(data, 1, dataSize, inFile);
        fwrite(data, 1, dataSize, outFile);
        free(data);
    }

    // �ͷ��ڴ�
    free(ids);
    free(offsets);
    free(filteredIds);
    free(filteredOffsets);

    fclose(inFile);
    fclose(outFile);
    printf("д��ɹ�\r\n");

    return 0; // �ɹ�
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
     wchar_t *unicode_str = malloc(len); // ����һ���㹻����������洢ת�����Unicode�ַ���
     memset(unicode_str, 0, len);
    printf("ת������\r\n");
    gbk_to_unicode(argv[3], unicode_str, len);
    wprintf(unicode_str);
    printf("��ʼ����\r\n");
    uc3_small(argv[1], argv[2], (uint16*)unicode_str);
    return 0;
}