/************************************************************
  FileName: draw.h
  Description:           模型场景绘制函数接口
***************************************************************/

#ifndef _DRAW
#define _DRAW

#define DMS_HIDE 1         /* 消隐标记 */
#define DMS_CUT3D 2         /* 视面裁剪标记 */ 
#define DMS_CUT2D 4         /* 视区裁剪标记 */
#define DMS_LINE 8       /* 线框绘制 */
#define DMS_FRAME 16       /* 填充绘制 */
#define DMS_FILL 32       /* 线型绘制 */

void draw_model(XM_MODEL *model, XM_VIEW *view, XM_WINDOW *window);    /* 绘制模型 */
void draw_field(XM_FIELD *field, XM_VIEW *view, XM_WINDOW *window);        /* 绘制场景 */

#endif