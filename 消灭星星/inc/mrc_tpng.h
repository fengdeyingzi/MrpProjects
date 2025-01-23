/*
 * @Author: 小蟀
 * @Date: 2022-03-13 22:30:26
 * @LastEditors: 小蟀
 * @LastEditTime: 2022-03-14 00:33:09
 * @description: 文件说明
 */
#include <mrc_base.h>
#define CHUNK_SIZE 1024

typedef struct _mrc_tpng
{
    uint8 *data;
    uint32 size;
    uint32 width;
    uint32 height;
} MRC_TPNG;

//将rgb888转rgb565
#define MAKECOLOR565(color) ((uint16)((color >> 8) & 0xf800) | (uint16)((color >> 5) & 0x07e0) | (uint16)((color >> 3) & 0x1f))
#ifndef MAKERGB
#define MAKERGB(r, g, b) (((uint16)(r >> 3) << 11) | ((uint16)(g >> 2) << 5) | ((uint16)(b >> 3)))
#endif

extern void mrc_draw_tpng(MRC_TPNG *png, int16 x, int16 y, uint16 rop);

// 读取mrp内部png图片
extern MRC_TPNG *read_mrp_tpng(char *filename);

/// 从系统目录读取文件，需释放内存
extern uint8 *read_file_data(char *filename, int32 *size);
extern void free_file_data(uint8 *data, int32 len);

/// 读取文件数据(普通的读取文件)
extern uint8 *read_mrp_data(char *filename, int32 *size);
extern void free_mrp_data(uint8 *data, int32 len);

/// 转换数据为png结构体
extern MRC_TPNG *mrc_decode_png(uint8 *pngdata, int32 size);

/// 释放png结构体内数据
extern void mrc_free_png(MRC_TPNG *png);

/// png结构体内rpga数据转bitmap的rgb565数据
extern uint16 *mrc_png2bmp_data(MRC_TPNG *png);
