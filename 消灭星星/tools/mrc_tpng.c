/*
 * @Author: 小蟀
 * @Date: 2022-03-13 22:30:58
 * @LastEditors: 小蟀
 * @LastEditTime: 2022-03-14 00:25:26
 * @description: 文件说明
 */

#include "mrc_tpng.h"
#include "tpng.h"

// 读取mrp内部png图片, 优先从mythroad读取，读取不到从mrp内部读取
MRC_TPNG *read_mrp_tpng(char *filename)
{
    int32 size = 0;
    uint8 *data = NULL;
    MRC_TPNG *png = NULL;

    data = read_file_data(filename, &size);
    if (data != NULL)
    {
        png = mrc_decode_png(data, size);
        free_file_data(data, size);
        return png;
    }

    size = 0;
    data = NULL;
    png = NULL;
    data = read_mrp_data(filename, &size);
    if (data != NULL)
    {
        png = mrc_decode_png(data, size);
        free_mrp_data(data, size);
        return png;
    }

    return png;
}

void mrc_draw_tpng(MRC_TPNG *png, int16 x, int16 y, uint16 rop)
{
    uint16 *data = mrc_png2bmp_data(png);
    mrc_bitmapShowEx(data, x, y, png->width, png->width, png->height, rop, 0, 0);
    mrc_free(data);
}

/// 从系统目录读取文件，需释放内存
extern int mrc_readFileMalloc(int32 file, int32 pos, int32 len, char **data);
uint8 *read_file_data(char *filename, int32 *size)
{
    uint8 *out = NULL;
    int32 file = mrc_open(filename, MR_FILE_RDONLY);
    *size = mrc_getLen(filename);
    if (*size < 0 || (void *)file == NULL)
        return NULL;

    mrc_readFileMalloc(file, MR_SEEK_SET, *size, (char **)&out);
    mrc_close(file);
    return out;
}

void free_file_data(uint8 *data, int32 len)
{
    mrc_freeOrigin(data, len);
}

/// 读取mrp内部文件，需释放内存
uint8 *read_mrp_data(char *filename, int32 *size)
{
    uint8 *out = NULL;
    out = (uint8 *)mrc_readFileFromMrp(filename, size, 0);
    return out;
}

void free_mrp_data(uint8 *data, int32 len)
{
    mrc_freeFileData(data, len);
}

/// 需释放返回的png结构体内存
MRC_TPNG *mrc_decode_png(uint8 *pngdata, int32 pngsize)
{
    MRC_TPNG *png = (void *)mrc_malloc(sizeof(MRC_TPNG));
    png->size = pngsize;

    png->data = tpng_get_rgba(pngdata, png->size, &(png->width), &(png->height));
    mrc_printf("tpng_get_rgba:width = %d, height = %d\n", png->width, png->height);
    return png;
}

/// 仅释放data
void mrc_free_png(MRC_TPNG *png)
{
    if (png->data != NULL)
        mrc_freeOrigin(png->data, png->width * png->height * sizeof(uint8));
}

/// 需释放返回的内存
uint16 *mrc_png2bmp_data(MRC_TPNG *png)
{
    uint32 x, y;
    uint8 *data = png->data;
    uint16 *bmp = (void *)mrc_malloc(png->width * png->height * 2);
    for (y = 0; y < png->height; ++y)
    {
        for (x = 0; x < png->width; ++x)
        {
            bmp[y * png->width + x] = MAKERGB(data[0], data[1], data[2]);
            data += 4;
        }
    }
    return bmp;
}