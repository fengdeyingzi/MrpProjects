#include <mrc_base.h>

#include "uc3_font.h"
#include "mpc.h"
#include "mrc_graphics.h"
#include "xl_debug.h"

int32 timer_cd;
BITMAP_565 *logo;
extern void drawGame(void);
void timer_run(int32 id)
{
  
  drawGame();
  return;
}

void drawGame(void)
{
  char *text_exit = NULL;
  int width = 0;
  mrc_clearScreen(0, 0, 0);
/*
  // mrc_printf("uc3 draw");
  uc3_drawText("测试字体 风的影子，制作", 10, 10, 255, 255, 255, 0);
  LOG_MSG("11");
  uc3_drawGradientText("测试渐变文字", 10, 40, 200,200,200, 90, 128, 240, 0);
  LOG_MSG("11");
  uc3_drawTextInRect("多行文本绘制\n测试测试\n这是多行文本哈哈哈哈哈哈哈",0, -8,   10, 60, 120, 32,   200, 120, 250, 0);
  LOG_MSG("11");
  

  text_exit = "退出";
  width = uc3_getWidth(text_exit, 0);
  
  uc3_drawText(text_exit, SCRW - width, SCRH - 16, 255, 255, 255, 0);
  drawBitmap565(logo, -40, -20);
  drawBitmap565(logo, 20, 30);

  mrc_refreshScreen(0, 0, SCRW, SCRH);
  */
  width = mrc_getVersion();
  mrc_printf("version = %d\n", width);
}

int32 mrc_init(void)
{

  LOG_MSG("mpc init");
  mpc_init();

  LOG_MSG("uc3 init");
  uc3_init();
  
  logo = readBitmap565FromAssets("logo_240.png");
  LOG_MSG("getuptime");
  sand(getuptime());

  LOG_MSG("draw....");
  drawGame();

  LOG_MSG("创建定时器");
  timer_cd = mrc_timerCreate();
  mrc_timerStart(timer_cd, 33, 0, timer_run, 0);

  return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
  if (code == MR_KEY_RELEASE)
  {
    if (param0 == MR_KEY_SOFTRIGHT)
    {
      mrc_exit();
    }
  }
  else if (code == MR_MOUSE_UP)
  {
    mrc_exit();
  }
  return 0;
}
int32 mrc_pause() { return 0; }
int32 mrc_resume() { return 0; }
int32 mrc_exitApp()
{
  int i = 0;
  mrc_timerStop(timer_cd);
  mrc_timerDelete(timer_cd);
  bitmapFree(logo);
  // 释放字体
  uc3_free();


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