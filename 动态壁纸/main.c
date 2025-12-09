#include <mrc_base.h>

#include "uc3_font.h"
#include "mpc.h"
#include "mrc_graphics.h"
#include "xl_debug.h"
#include "wallpaper.h"
#include "game_menu.h"

/* 窗口类型 */
enum {
    WINDOW_MENU = 0,
    WINDOW_WALLPAPER = 1,
    WINDOW_HELP = 2,
    WINDOW_ABOUT = 3
};

/* 全局变量 */
int32 timer_cd;
int32 timer_init;
int currentWindow;
WALLPAPER_PLAYER wallpaperPlayer;
PMENU mainMenu;

/* 函数声明 */
void drawWallpaper(void);
void drawHelp(void);
void drawAbout(void);
void switchWindow(int windex, int level);

/* 定时器回调 */
void timer_run(int32 id)
{
  if (currentWindow == WINDOW_WALLPAPER)
  {
    if(!wallpaperPlayer.isPaused){
      if (wallpaper_nextFrame(&wallpaperPlayer) == 0)
    {
      wallpaper_drawFrame(&wallpaperPlayer);
    }
    else
    {
      wallpaper_togglePause(&wallpaperPlayer);
    }
    }
    
  }

  return;
}

void init_run(int32 id){
  int ret;
  /* 资源解压（包含读取plays.txt） */
  LOG_MSG("资源解压");
  ret = wallpaper_extractResources();
  if (ret != 0) {
    LOG_VAR("资源解压失败: %d", ret);
    drawCenterUI("资源解压失败");
  }

  /* 初始化壁纸播放器 */
  ret = wallpaper_init(&wallpaperPlayer);
  if (ret != 0) {
    LOG_MSG("壁纸播放器初始化失败");
    drawCenterUI("播放器初始化失败");
  }

  /* 读取壁纸列表 */
  ret = wallpaper_readPlaylist(&wallpaperPlayer.list);
  if (ret != 0) {
    LOG_MSG("读取壁纸列表失败");
    drawCenterUI("读取壁纸列表失败");
  }

  /* 检查壁纸列表 */
  if (wallpaperPlayer.list.count == 0) {
    LOG_MSG("没有找到壁纸");
    drawCenterUI("没有找到壁纸");
  } else {
    LOG_VAR("找到 %d 个壁纸", wallpaperPlayer.list.count);
  }

  /* 初始化菜单 */
  LOG_MSG("初始化菜单");
  currentWindow = WINDOW_MENU;
  pmenu_init(&mainMenu);
  pmenu_draw(&mainMenu);
}

/* 窗口切换 */
void switchWindow(int windex, int level)
{
  char bmasPath[128];
  uint8 packName[30];
  char *endName;
  int ret;

  if (windex == 1) {
    /* 从菜单进入子窗口 */
    if (level == 0) {
      /* 壁纸列表 */
      currentWindow = WINDOW_WALLPAPER;

      /* 检查壁纸列表 */
      if (wallpaperPlayer.list.count == 0) {
        LOG_MSG("没有可用的壁纸");
        drawCenterUI("没有可用的壁纸");
        switchWindow(0, 0);
        return;
      }

      /* 播放器已经在 init_run 中初始化，这里不需要再次初始化 */

      /* 获取包名 */
      mrc_GetMrpInfo((char*)mrc_getPackName(), MRP_FILENAME, packName, 32);
      endName = mrc_strrchr((const char *)packName, '.');
      if(endName != NULL){
        *endName = 0;
      }

      /* 构造dfa文件路径 */
      mrc_sprintf(bmasPath, "%s/%s", packName, wallpaperPlayer.list.wallpapers[0].dfaName);
      LOG_MSG("开始加载壁纸...");
      /* 检查文件是否存在，如果不存在则解压 */
      if (mrc_fileState(bmasPath) != MR_IS_FILE) {
        LOG_MSG("文件不存在");
        ret = wallpaper_extractWallpaper(wallpaperPlayer.list.wallpapers[0].dfaName);
        if (ret != 0) {
          LOG_MSG("解压壁纸失败");
          wallpaper_free(&wallpaperPlayer);
          switchWindow(0, 0);
          return;
        }
        wallpaperPlayer.list.wallpapers[0].isExtracted = 1;
      }

      /* 加载dfa */
      ret = wallpaper_loadDfa(&wallpaperPlayer, bmasPath);
      if (ret != 0) {
        LOG_MSG("加载dfa失败");
        wallpaper_free(&wallpaperPlayer);
        switchWindow(0, 0);
        return;
      }
      LOG_MSG("加载并绘制第一帧");
      /* 加载并绘制第一帧（第0帧） */
      wallpaperPlayer.currentFrame = -1;
      wallpaper_nextFrame(&wallpaperPlayer);
      wallpaper_drawFrame(&wallpaperPlayer);

    } else if (level == 1) {
      /* 帮助 */
      currentWindow = WINDOW_HELP;
      drawHelp();
    } else if (level == 2) {
      /* 关于 */
      currentWindow = WINDOW_ABOUT;
      drawAbout();
    } else if (level == 3) {
      /* 退出 */
      mrc_exit();
    }
  } else if (windex == 0) {
    /* 返回菜单 */
    if (currentWindow == WINDOW_WALLPAPER) {
      /* 释放壁纸资源 */
      wallpaper_free(&wallpaperPlayer);
    }
    currentWindow = WINDOW_MENU;
    pmenu_draw(&mainMenu);
  }
}

/* 绘制帮助 */
void drawHelp(void)
{
  mrc_clearScreen(240, 240, 240);
  uc3_drawText("动态壁纸播放器", 10, 10, 0, 0, 0, 0);
  uc3_drawText("", 10, 30, 0, 0, 0, 0);
  uc3_drawText("操作说明:", 10, 50, 0, 0, 0, 0);
  uc3_drawText("上方向键 - 上一张壁纸", 10, 70, 0, 0, 0, 0);
  uc3_drawText("下方向键 - 下一张壁纸", 10, 90, 0, 0, 0, 0);
  uc3_drawText("OK键 - 暂停/继续", 10, 110, 0, 0, 0, 0);
  uc3_drawText("右软键 - 返回菜单", 10, 130, 0, 0, 0, 0);
  uc3_drawText("", 10, 150, 0, 0, 0, 0);
  uc3_drawText("返回", SCRW - uc3_getWidth("返回", 0) - 10, SCRH - 40, 0, 0, 0, 0);
  mrc_refreshScreen(0, 0, SCRW, SCRH);
}

/* 绘制关于 */
void drawAbout(void)
{
  mrc_clearScreen(240, 240, 240);
  uc3_drawText("动态壁纸", 10, 10, 0, 0, 0, 0);
  uc3_drawText("", 10, 30, 0, 0, 0, 0);
  uc3_drawText("版本: 1.0", 10, 50, 0, 0, 0, 0);
  uc3_drawText("作者: 风的影子", 10, 70, 0, 0, 0, 0);
  uc3_drawText("", 10, 90, 0, 0, 0, 0);
  uc3_drawText("返回", SCRW - uc3_getWidth("返回", 0) - 10, SCRH - 40, 0, 0, 0, 0);
  mrc_refreshScreen(0, 0, SCRW, SCRH);
}





int32 mrc_init(void)
{
  int ret;

  LOG_MSG("mpc init");
  mpc_init();

  LOG_MSG("uc3 init");
  uc3_init();

  LOG_MSG("getuptime");
  sand(getuptime());

  drawCenterUI("解压资源中...");

  LOG_MSG("创建定时器");
  timer_cd = mrc_timerCreate();
  mrc_timerStart(timer_cd, 33, 0, timer_run, 1);
  timer_init = mrc_timerCreate();
  mrc_timerStart(timer_init, 10, 0, init_run, 0);

  return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1)
{
  if (currentWindow == WINDOW_MENU) {
    /* 菜单事件 */
    pmenu_event(&mainMenu, code, param0, param1);
  } else if (currentWindow == WINDOW_WALLPAPER) {
    /* 壁纸窗口事件 */
    if (code == KY_UP) {
      if (param0 == _SELECT) {
        /* OK键：暂停/继续 */
        wallpaper_togglePause(&wallpaperPlayer);
      } else if (param0 == _UP) {
         /* 清屏 */
    mrc_clearScreen(0, 0, 0);
        /* 方向键上：上一张壁纸 */
        wallpaper_switchWallpaper(&wallpaperPlayer, -1);
      } else if (param0 == _DOWN) {
         /* 清屏 */
        mrc_clearScreen(0, 0, 0);
        /* 方向键下：下一张壁纸 */
       
        wallpaper_switchWallpaper(&wallpaperPlayer, 1);
      } else if (param0 == _SRIGHT) {
        /* 右软键：返回菜单 */
        switchWindow(0, 0);
      }
    } else if (code == MS_UP) {
      /* 触摸屏：返回菜单 */
      switchWindow(0, 0);
    }
  } else if (currentWindow == WINDOW_HELP || currentWindow == WINDOW_ABOUT) {
    /* 帮助/关于窗口事件 */
    if (code == KY_UP) {
      if (param0 == _SRIGHT || param0 == _SELECT) {
        /* 返回菜单 */
        switchWindow(0, 0);
      }
    } else if (code == MS_UP) {
      /* 触摸屏：返回菜单 */
      switchWindow(0, 0);
    }
  }
  return 0;
}
int32 mrc_pause() { return 0; }
int32 mrc_resume() { return 0; }
int32 mrc_exitApp()
{
  mrc_timerStop(timer_cd);
  mrc_timerDelete(timer_cd);
  mrc_timerStop(timer_init);
  mrc_timerDelete(timer_init);

  /* 释放菜单 */
  pmenu_free(&mainMenu);

  /* 释放壁纸列表 */
  wallpaper_freeList(&wallpaperPlayer.list);

  /* 释放字体 */
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