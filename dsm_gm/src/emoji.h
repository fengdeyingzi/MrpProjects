#ifndef _EMOJI_H_
#define _EMOJI_H_

#include "mrc_base.h"

/* 绘制emoji, size表示emoji图标的宽高
 * emoji: emoji字符，例如 "🛸"
 * x, y: 绘制位置
 * size: emoji图标的宽高 (例如 24 表示 24x24 像素)
 * 返回: MR_SUCCESS 成功, MR_FAILED 失败
 */
int32 emoji_draw(char *emoji, int x, int y, int size);

/* 释放emoji资源
 * 返回: MR_SUCCESS 成功
 */
int32 emoji_free(void);

#endif
