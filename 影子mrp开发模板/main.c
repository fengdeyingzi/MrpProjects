#include <mrc_base.h>

#include "uc3_font.h"
#include "mpc.h"
#include "mrc_graphics.h"

int32 timer_cd;
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

  // mrc_printf("uc3 draw");
  uc3_drawText("测试字体 风的影子，制作", 10, 10, 255, 255, 255, 0);

  

  text_exit = "退出";
  width = uc3_getWidth(text_exit, 0);
  uc3_drawText(text_exit, SCRW - width, SCRH - 16, 255, 255, 255, 0);
  mrc_refreshScreen(0, 0, SCRW, SCRH);
}

int32 mrc_init(void)
{
  int i = 0;
  char temp[30];
  BITMAPINFO bitmapinfo;

  mpc_init();


  uc3_init();
  sand(getuptime());


  drawGame();

  mrc_printf("图片宽度:%d 高度:%d\n", bitmapinfo.width, bitmapinfo.height);
  timer_cd = timercreate();
  timerstart(timer_cd, 33, 0, timer_run, 1);

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
  timerstop(timer_cd);
  timerdel(timer_cd);
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