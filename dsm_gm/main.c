#include <mrc_base.h>
#include "uc3_font.h"
#include "mpc.h"
#include "mrc_graphics.h"
#include "xl_debug.h"
#include "mrpinfo.h"

/* 窗口类型枚举 */
enum {
    WINDOW_LIST = 0,    /* 应用列表窗口 */
    WINDOW_MENU = 1,    /* 操作菜单窗口 */
    WINDOW_INFO = 2,    /* 应用信息窗口 */
    WINDOW_CONFIRM = 3  /* 确认对话框窗口 */
};

/* 菜单项枚举 */
enum {
    MENU_RUN = 0,       /* 启动应用 */
    MENU_UPDATE = 1,    /* 更新应用 */
    MENU_INFO = 2,      /* 应用说明 */
    MENU_DELETE = 3     /* 删除应用 */
};

/* 布局常量 */
#define HEADER_HEIGHT 26
#define FOOTER_HEIGHT 20
#define LIST_START_Y 32

/* 颜色常量 */
#define COLOR_BG 0xFF1fa2c3
#define COLOR_HEADER 0xFF165d6f
#define COLOR_TEXT 0xFFffffff
#define COLOR_SELECT_BG 0xFFdceef5
#define COLOR_SELECT_TEXT 0xFF000000
#define COLOR_SCROLL_BAR 0xFFffffff

/* MRP 应用信息结构 */
typedef struct {
    char *path;
    char displayName[25];
    char desc[65];
    char vendor[41];
    int32 appid;
    int32 version;
} MrpInfo;

/* 菜单项数据 */
typedef struct {
    char *text;
    char *icon;
} MenuItem;

MenuItem menuItems[] = {
    {"启动应用", "💾"},
    {"更新应用", "⬇️"},
    {"应用说明", "📄"},
    {"删除应用", "❌"}
};

/* 全局变量 */
int currentWindow;
int menuIndex;

/* 布局计算变量 */
int LINE_HEIGHT;
int VISIBLE_ITEMS;

int FONT_HEIGHT;

/* 应用列表数据 */
MrpInfo **appList;
int appListMax;
int appListLen;
int appListIndex;
int appListScrollTop;

/* 前向声明 */
void drawListScreen(void);
void drawMenuScreen(void);
void drawInfoScreen(void);
void drawConfirmScreen(void);
void switchWindow(int windex);
void listEvent(int32 code, int32 param0, int32 param1);
void menuEvent(int32 code, int32 param0, int32 param1);
void infoEvent(int32 code, int32 param0, int32 param1);
void confirmEvent(int32 code, int32 param0, int32 param1);
void timer_run(int32 id);
void scanMrpFiles(void);
void addMrpToList(char *filename, Config *config);
void freeAppList(void);
void drawScrollBar(int totalItems, int currentIndex);
void toolbar_draw(char *leftText, char *rightText);
void deleteCurrentApp(void);

/* 定时器回调 */
void timer_run(int32 id)
{
    return;
}

/* 绘制底部工具栏 */
void toolbar_draw(char *leftText, char *rightText)
{
    int width;

    if(leftText != NULL){
        uc3_drawText(leftText, 4, SCRH - 14, 255, 255, 255, 0);
    }

    if(rightText != NULL){
        width = uc3_getWidth(rightText, 0);
        uc3_drawText(rightText, SCRW - width - 4, SCRH - 14, 255, 255, 255, 0);
    }
}

/* 绘制滚动条 */
void drawScrollBar(int totalItems, int currentIndex)
{
    int scrollBarX;
    int startY;
    int endY;
    int trackHeight;
    int barHeight;
    int maxScrollDist;
    int barY;
    float scrollPercent;

    scrollBarX = SCRW - 3;
    startY = HEADER_HEIGHT;
    endY = SCRH - FOOTER_HEIGHT;
    trackHeight = endY - startY;

    /* 绘制槽线 */
    gl_drawLine(scrollBarX, startY, scrollBarX, endY, COLOR_SCROLL_BAR);

    /* 绘制滑块 */
    if(totalItems > 0){
        barHeight = trackHeight / (totalItems > VISIBLE_ITEMS ? totalItems : VISIBLE_ITEMS);
        if(barHeight < 20){
            barHeight = 20;
        }

        maxScrollDist = trackHeight - barHeight;
        scrollPercent = (float)currentIndex / (float)(totalItems - 1);
        barY = startY + (int)(maxScrollDist * scrollPercent);

        /* 绘制粗线作为滑块 */
        gl_drawLine(scrollBarX, barY, scrollBarX, barY + barHeight, COLOR_SCROLL_BAR);
        gl_drawLine(scrollBarX - 1, barY, scrollBarX - 1, barY + barHeight, COLOR_SCROLL_BAR);
        gl_drawLine(scrollBarX + 1, barY, scrollBarX + 1, barY + barHeight, COLOR_SCROLL_BAR);
    }
}

/* 绘制列表屏幕 */
void drawListScreen(void)
{
    char temp[32];
    int width;
    int i;
    int visualIndex;
    int y;
    int start;
    int end;

    /* 清屏 */
    gl_drawRect(0, 0, SCRW, SCRH, COLOR_BG);

    /* 绘制头部 */
    gl_drawRect(0, 0, SCRW, HEADER_HEIGHT, COLOR_HEADER);
    uc3_drawText("应用列表", SCRW/2 - uc3_getWidth("应用列表", 0)/2, 5, 255, 255, 255, 0);

    /* 在右上角显示应用数量 */
    mrc_sprintf(temp, "%d", appListLen);
    width = uc3_getWidth(temp, 0);
    uc3_drawText(temp, SCRW - width - 5, 5, 255, 255, 255, 0);

    /* 绘制列表项 */
    if(appListLen > 0){
        start = appListScrollTop;
        end = start + VISIBLE_ITEMS;
        if(end > appListLen){
            end = appListLen;
        }

        for(i = start; i < end; i++){
            visualIndex = i - start;
            y = LIST_START_Y + (visualIndex * LINE_HEIGHT);

            
            /* 绘制选中背景 */
            if(i == appListIndex){
                gl_drawRoundRect(2, y, SCRW - 10, LINE_HEIGHT, 3, COLOR_SELECT_BG);
                mrc_printf("draw text ... %d", i);
                gl_drawText(appList[i]->displayName, 28, y + (LINE_HEIGHT - FONT_HEIGHT) / 2, 0, 0, 0, 0, 1);
            }else{
                mrc_printf("draw text2 ... %d", i);
                gl_drawText(appList[i]->displayName, 28, y + (LINE_HEIGHT - FONT_HEIGHT) / 2, 255, 255, 255, 0, 1);
            }
            emoji_draw("🐥", 5, y + (LINE_HEIGHT-16)/2, 16);
            mrc_printf("name = %s", appList[i]->displayName);
        }

        /* 绘制滚动条 */
        drawScrollBar(appListLen, appListIndex);
    }

    /* 绘制底部软键提示 */
    toolbar_draw("选择", "返回");

    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

// 显示提示
void drawTip(const char *text){
    int32 textw,texth;
    textw = uc3_getWidth(text, 0);
    texth = uc3_getHeight(text, 0);
    gl_drawRect((SCRW - textw - 10)/2, (SCRH - texth - 10)/2, textw + 10, texth + 10, 0x80000000);
    uc3_drawText((char *)text, (SCRW - textw)/2, (SCRH - texth)/2, 255, 255, 255, 0);
    mrc_refreshScreen((SCRW - textw - 10)/2, (SCRH - texth - 10)/2, textw + 10, texth + 10);
}

/* 绘制菜单屏幕 */
void drawMenuScreen(void)
{
    int i;
    int y;
    int32 width;
    int32 height;
    char temp[32];
    /* 添加调试日志 */
    mrc_printf("drawMenuScreen: 开始绘制，当前应用索引=%d，菜单索引=%d", appListIndex, menuIndex);

    /* 清屏 */
    gl_drawRect(0, 0, SCRW, SCRH, COLOR_BG);

    /* 绘制头部 */
    gl_drawRect(0, 0, SCRW, HEADER_HEIGHT, COLOR_HEADER);

    /* 显示当前应用名称 */
    mrc_printf("drawMenuScreen: 绘制应用名称开始，displayName=%s", appList[appListIndex]->displayName);
    gl_textWidthHeight(appList[appListIndex]->displayName, 0, 1, &width, &height);
    mrc_printf("drawMenuScreen: 文字宽=%d，高=%d", width, height);

    
    
    gl_drawText(appList[appListIndex]->displayName, SCRW/2 - width/2, 5, 255, 255, 255, 0, 1);
    mrc_printf("drawMenuScreen: 应用名称绘制完成");
    
    mrc_sprintf(temp, "%d", menuIndex);
    /* 绘制右上角序号 */
    uc3_drawText(temp, SCRW - 15, 5, 255, 255, 255, 0);

    /* 绘制菜单项 */
    mrc_printf("drawMenuScreen: 开始绘制菜单项，共4项，LINE_HEIGHT=%d", LINE_HEIGHT);
    for(i = 0; i < 4; i++){
        y = LIST_START_Y + i * LINE_HEIGHT;
        mrc_printf("drawMenuScreen: 第%d项，y坐标=%d，是否选中=%s", i + 1, y, i == menuIndex ? "是" : "否");
        int textHeight = uc3_getHeight(menuItems[i].text, 0);
        
        /* 绘制选中背景 */
        if(i == menuIndex){
            mrc_printf("drawMenuScreen: 绘制选中项背景，icon=%s，text=%s", menuItems[i].icon, menuItems[i].text);
            gl_drawRoundRect(2, y, SCRW - 10, LINE_HEIGHT, 3, COLOR_SELECT_BG);
            
            uc3_drawText(menuItems[i].text, 28, (int16)(y + (LINE_HEIGHT - textHeight)/ 2), 0, 0, 0, 0);
        }else{
            mrc_printf("drawMenuScreen: 绘制未选中项，icon=%s，text=%s", menuItems[i].icon, menuItems[i].text);
            uc3_drawText(menuItems[i].text, 28, (int16)(y + (LINE_HEIGHT - textHeight) / 2), 255, 255, 255, 0);
        }
        emoji_draw(menuItems[i].icon, 5, (int16)(y + (LINE_HEIGHT - 16) / 2), 16);
        mrc_printf("drawMenuScreen: 第%d项绘制完成", i + 1);
    }
    mrc_printf("drawMenuScreen: 所有菜单项绘制完成");

    /* 绘制静态滚动条 */
    drawScrollBar(4, menuIndex);
    mrc_printf("drawline...");
    /* 绘制底部软键提示 */
    toolbar_draw("选择", "返回");

    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, SCRW, SCRH);
    mrc_printf("ref....");
}

/* 绘制应用信息屏幕 */
void drawInfoScreen(void)
{
    char *infoText;
    MrpInfo *info;

    info = appList[appListIndex];

    /* 清屏 */
    gl_drawRect(0, 0, SCRW, SCRH, COLOR_BG);

    /* 绘制头部 */
    gl_drawRect(0, 0, SCRW, HEADER_HEIGHT, COLOR_HEADER);
    uc3_drawText("应用信息", SCRW/2 - uc3_getWidth("应用信息", 0)/2, 5, 255, 255, 255, 0);

    /* 组合信息文本 */
    infoText = mrc_malloc(512);
    mrc_memset(infoText, 0, 512);
    // 应用名称: %s\n\nAppID: %d\n\n版本: %d\n\n供应商: %s\n\n描述: %s
    mrc_sprintf(infoText, "\xd3\xa6\xd3\xc3\xc3\xfb\xb3\xc6: %s\n\nAppID: %d\n\n\xb0\xe6\xb1\xbe: %d\n\n\xb9\xa9\xd3\xa6\xc9\xcc: %s\n\n\xc3\xe8\xca\xf6: %s",
                info->displayName,
                info->appid,
                info->version,
                info->vendor,
                info->desc);

    /* 一次性绘制所有信息 */
    gl_drawTextInRect(infoText, 0, 0, 10, 35, SCRW - 20, SCRH - 60, 255, 255, 255, 0, 1);

    mrc_free(infoText);

    /* 绘制底部软键提示 */
    toolbar_draw(NULL, "返回");

    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

/* 绘制确认对话框 */
void drawConfirmScreen(void)
{
    int dialogWidth;
    int dialogHeight;
    int dialogX;
    int dialogY;
    int32 textWidth;
    int32 textHeight;
    char *confirmText;

    dialogWidth = SCRW - 40;
    dialogHeight = 80;
    dialogX = 20;
    dialogY = (SCRH - dialogHeight) / 2;

    /* 清屏 */
    gl_drawRect(0, 0, SCRW, SCRH, COLOR_BG);

    /* 绘制头部 */
    gl_drawRect(0, 0, SCRW, HEADER_HEIGHT, COLOR_HEADER);
    uc3_drawText("应用列表", SCRW/2 - uc3_getWidth("应用列表", 0)/2, 5, 255, 255, 255, 0);

    /* 绘制半透明背景遮罩 */
    gl_drawRect(0, 0, SCRW, SCRH, 0x80000000);

    /* 绘制对话框背景 */
    gl_drawRoundRect(dialogX, dialogY, dialogWidth, dialogHeight, 5, 0xFFFFFFFF);

    /* 绘制提示文字 */
    confirmText = "确定要删除该应用吗？";
    textWidth = uc3_getWidth(confirmText, 0);
    uc3_drawText(confirmText, dialogX + (dialogWidth - textWidth) / 2, dialogY + 15, 0, 0, 0, 0);

    /* 显示应用名称 */
    // textWidth = uc3_getWidth(appList[appListIndex]->displayName, 0);
    gl_textWidthHeight(appList[appListIndex]->displayName, 0, 1, &textWidth, &textHeight);
    // uc3_drawText(appList[appListIndex]->displayName, dialogX + (dialogWidth - textWidth) / 2, dialogY + 40, 100, 100, 100, 0);
    gl_drawText(appList[appListIndex]->displayName, dialogX + (dialogWidth - textWidth) / 2, dialogY + 40, 100, 100, 100, 0, 1);

    /* 绘制底部软键提示 */
    toolbar_draw("确定", "取消");

    /* 刷新屏幕 */
    mrc_refreshScreen(0, 0, SCRW, SCRH);
}

/* 窗口切换函数 */
void switchWindow(int windex)
{
    currentWindow = windex;

    if(windex == WINDOW_LIST){
        drawListScreen();
    }else if(windex == WINDOW_MENU){
        menuIndex = 0;
        drawMenuScreen();
    }else if(windex == WINDOW_INFO){
        drawInfoScreen();
    }else if(windex == WINDOW_CONFIRM){
        drawConfirmScreen();
    }
}

/* 添加 MRP 到列表 */
void addMrpToList(char *filename, Config *config)
{
    int32 len;
    int32 f;
    MrpInfo *info;

    len = mrc_strlen(filename);
    if(len < 255){
        len = 255;
    }

    if(appListLen >= appListMax - 1){
        return;
    }

    info = mrc_malloc(sizeof(MrpInfo));
    mrc_memset(info, 0, sizeof(MrpInfo));

    info->path = mrc_malloc(len * sizeof(char));
    mrc_strcpy(info->path, filename);
    mrc_memcpy(info->displayName, config->displayName, 24);
    info->appid = config->appid;
    info->version = config->version;
    mrc_memcpy(info->vendor, config->vendor, 40);
    mrc_memcpy(info->desc, config->desc, 64);
    appList[appListLen++] = info;
    /* 读取 MRP 信息 */
    /*
    f = mrc_open(filename, MR_FILE_RDONLY);
    if(f > 0){
        mrc_seek(f, 28, MR_SEEK_SET);
        mrc_read(f, info->displayName, 24);
        mrc_seek(f, 68, MR_SEEK_SET);
        mrc_read(f, &info->appid, 4);
        mrc_seek(f, 72, MR_SEEK_SET);
        mrc_read(f, &info->version, 4);
        mrc_seek(f, 88, MR_SEEK_SET);
        mrc_read(f, info->vendor, 40);
        mrc_seek(f, 128, MR_SEEK_SET);
        mrc_read(f, info->desc, 64);
        mrc_close(f);

        appList[appListLen++] = info;
    }else{
        mrc_free(info->path);
        mrc_free(info);
    }
    */
}

/* 扫描 MRP 文件 */
void scanMrpFiles(void)
{
    int32 f;
    int32 ret;
    char *temp;
    char *endfile;
    Config *config;

    temp = mrc_malloc(255);
    config = mrc_malloc(sizeof(Config));

    f = mrc_findStart("", temp, 72);
    mrc_printf("findstart %s", temp);

    if(temp != NULL && *temp != '.'){
        endfile = mrc_strrchr(temp, '.');
        if(endfile != NULL){
            if(mrc_strcmp(endfile, ".mrp") == 0 || mrc_strcmp(endfile, ".MRP") == 0){
                if(readMrpInfo(temp, config) == MR_SUCCESS){
                    if(config->flag & 0x01){
                        addMrpToList(temp, config);
                    }
                }
            }
        }
    }

    ret = 0;
    while(ret == 0){
        ret = mrc_findGetNext(f, temp, 72);
        mrc_printf("findnext %s", temp);

        if(ret){
            break;
        }
        if(temp == NULL){
            break;
        }
        if(*temp != '.'){
            endfile = mrc_strrchr(temp, '.');
            if(endfile != NULL){
                if(mrc_strcmp(endfile, ".mrp") == 0 || mrc_strcmp(endfile, ".MRP") == 0){
                    if(readMrpInfo(temp, config) == MR_SUCCESS){
                        if(config->flag & 0x01){
                            addMrpToList(temp, config);
                        }
                    }
                }
            }
        }
    }

    mrc_findStop(f);
    mrc_free(config);
    mrc_free(temp);
}

/* 释放应用列表 */
void freeAppList(void)
{
    int i;

    for(i = 0; i < appListLen; i++){
        if(appList[i] != NULL){
            if(appList[i]->path != NULL){
                mrc_free(appList[i]->path);
            }
            mrc_free(appList[i]);
        }
    }
    mrc_free(appList);
}

/* 删除当前选中的应用 */
void deleteCurrentApp(void)
{
    int32 ret;
    int i;

    if(appListIndex < 0 || appListIndex >= appListLen){
        return;
    }

    /* 删除文件 */
    ret = mrc_remove(appList[appListIndex]->path);

    if(ret == MR_SUCCESS){
        /* 显示删除成功提示 */
        drawTip("删除成功");
        mrc_sleep(1000);

        /* 释放被删除应用的内存 */
        if(appList[appListIndex]->path != NULL){
            mrc_free(appList[appListIndex]->path);
        }
        mrc_free(appList[appListIndex]);

        /* 从列表中移除 */
        for(i = appListIndex; i < appListLen - 1; i++){
            appList[i] = appList[i + 1];
        }
        appListLen--;

        /* 调整选中索引 */
        if(appListIndex >= appListLen && appListLen > 0){
            appListIndex = appListLen - 1;
        }

        /* 调整滚动位置 */
        if(appListScrollTop > appListIndex){
            appListScrollTop = appListIndex;
        }
        if(appListScrollTop < 0){
            appListScrollTop = 0;
        }
    }else{
        /* 显示删除失败提示 */
        drawTip("删除失败");
        mrc_sleep(1000);
    }

    /* 返回列表窗口 */
    switchWindow(WINDOW_LIST);
}

/* 列表事件处理 */
void listEvent(int32 code, int32 param0, int32 param1)
{
    int clickedRow;
    int targetIndex;
    mrc_printf("listEvent: code=%d, param0=%d, param1=%d, appListIndex=%d, appListScrollTop=%d", code, param0, param1, appListIndex, appListScrollTop);
    if(code == MR_KEY_RELEASE){
        if(param0 == MR_KEY_SOFTRIGHT){
            mrc_setReloadFile("",  "");
            /* 右软键：退出 */
            mrc_exit();
        }
        else if(param0 == MR_KEY_SOFTLEFT || param0 == MR_KEY_SELECT){
            /* 左软键或确认键：进入菜单 */
            if(appListLen > 0){
                switchWindow(WINDOW_MENU);
            }
        }
        else if(param0 == MR_KEY_UP){
            /* 上键 */
            appListIndex--;
            if(appListIndex < appListScrollTop){
                appListScrollTop = appListIndex - VISIBLE_ITEMS + 1;
                if(appListScrollTop < 0){
                    appListScrollTop = 0;
                }
            }
            if(appListIndex < 0){
                appListIndex = appListLen - 1;
                appListScrollTop = appListLen - 1;
            }
            drawListScreen();
        }
        else if(param0 == MR_KEY_DOWN){
            /* 下键 */
            appListIndex++;
            if(appListIndex >= appListLen){
                appListIndex = 0;
                appListScrollTop = 0;
            }
            if(appListIndex - appListScrollTop >= VISIBLE_ITEMS){
                appListScrollTop = appListIndex;
            }
            drawListScreen();
        }
        else if(param0 == MR_KEY_LEFT){
            /* 左键：上翻页 */
            if(appListScrollTop > VISIBLE_ITEMS){
                appListIndex -= VISIBLE_ITEMS;
                appListScrollTop -= VISIBLE_ITEMS;
            }else if(appListIndex != 0){
                appListIndex = 0;
                appListScrollTop = 0;
            }else{
                appListIndex = appListLen - 1;
                appListScrollTop = appListLen - 1;
            }

            if(appListIndex < appListScrollTop){
                appListScrollTop = appListIndex - VISIBLE_ITEMS;
                if(appListScrollTop < 0){
                    appListScrollTop = 0;
                }
            }
            if(appListIndex < 0){
                appListIndex = appListLen - 1;
                appListScrollTop = appListLen - 1;
            }
            drawListScreen();
        }
        else if(param0 == MR_KEY_RIGHT){
            /* 右键：下翻页 */
            appListIndex += VISIBLE_ITEMS;
            appListScrollTop += VISIBLE_ITEMS;
            if(appListIndex >= appListLen){
                appListIndex = 0;
                appListScrollTop = 0;
            }
            if(appListIndex - appListScrollTop > VISIBLE_ITEMS){
                appListScrollTop = appListIndex;
            }
            drawListScreen();
        }
    }
    else if(code == MR_MOUSE_UP){
        /* 触摸事件 */
        if(param1 > SCRH - FOOTER_HEIGHT){
            /* 点击底部软键区域 */
            if(param0 < SCRW / 2){
                /* 点击左软键区域 */
                if(appListLen > 0){
                    switchWindow(WINDOW_MENU);
                }
            }else{
                /* 点击右软键区域 */
                mrc_exit();
            }
        }
        else if(param1 > HEADER_HEIGHT && param1 < SCRH - FOOTER_HEIGHT){
            /* 点击列表项 */
            clickedRow = (param1 - LIST_START_Y) / LINE_HEIGHT;
            targetIndex = appListScrollTop + clickedRow;
            if(targetIndex >= 0 && targetIndex < appListLen){
                if(appListIndex == targetIndex){
                    /* 双击进入菜单 */
                    switchWindow(WINDOW_MENU);
                }else{
                    /* 选中项 */
                    appListIndex = targetIndex;
                    drawListScreen();
                }
            }
        }
    }
}

/* 菜单事件处理 */
void menuEvent(int32 code, int32 param0, int32 param1)
{
    int clickedRow;
    
    /* 添加调试日志：记录事件参数 */
    mrc_printf("menuEvent: code=%d, param0=%d, param1=%d, menuIndex=%d", code, param0, param1, menuIndex);

    if(code == MR_KEY_RELEASE){
        if(param0 == MR_KEY_SOFTRIGHT){
            /* 右软键：返回列表 */
            mrc_printf("menuEvent: 右软键按下，返回列表");
            switchWindow(WINDOW_LIST);
        }
        else if(param0 == MR_KEY_SOFTLEFT || param0 == MR_KEY_SELECT){
            /* 左软键或确认键：执行菜单操作 */
            mrc_printf("menuEvent: 执行菜单操作, menuIndex=%d", menuIndex);
            if(menuIndex == MENU_RUN){
                /* 启动应用 */
                mrc_printf("menuEvent: 启动应用: %s %d", appList[appListIndex]->path, mrc_getVersion());
                drawTip("启动中...");
                if(mrc_getVersion() >= 2000){
                    mrc_setReloadFile(mr_table->pack_filename,  mr_table->start_filename);
                    mrc_runMrp(appList[appListIndex]->path, "cfunction.ext", NULL);
                }
                else{
                    mrc_setReloadFile(mr_table->pack_filename,  mr_table->start_filename);
                    mrc_runMrp(appList[appListIndex]->path, "start.mr", NULL);
                }
                
            }
            else if(menuIndex == MENU_UPDATE){
                /* 更新应用 */
                mrc_printf("menuEvent: 更新应用: %s", appList[appListIndex]->displayName);
            }
            else if(menuIndex == MENU_INFO){
                /* 应用说明 */
                mrc_printf("menuEvent: 切换到应用信息窗口");
                switchWindow(WINDOW_INFO);
            }
            else if(menuIndex == MENU_DELETE){
                /* 删除应用 */
                mrc_printf("menuEvent: 删除应用: %s", appList[appListIndex]->path);
                switchWindow(WINDOW_CONFIRM);
            }
        }
        else if(param0 == MR_KEY_UP){
            /* 上键 */
            mrc_printf("menuEvent: 上键，menuIndex从%d变为%d", menuIndex, menuIndex - 1);
            menuIndex--;
            if(menuIndex < 0){
                menuIndex = 3;
            }
            mrc_printf("menuEvent: 调整后menuIndex=%d", menuIndex);
            drawMenuScreen();
        }
        else if(param0 == MR_KEY_DOWN){
            /* 下键 */
            mrc_printf("menuEvent: 下键，menuIndex从%d变为%d", menuIndex, menuIndex + 1);
            menuIndex++;
            if(menuIndex > 3){
                menuIndex = 0;
            }
            mrc_printf("menuEvent: 调整后menuIndex=%d", menuIndex);
            drawMenuScreen();
        }
    }
    else if(code == MR_MOUSE_UP){
        /* 触摸事件 */
        mrc_printf("menuEvent: 触摸事件, 坐标(%d, %d)", param0, param1);
        if(param1 > SCRH - FOOTER_HEIGHT){
            /* 点击底部软键区域 */
            if(param0 < SCRW / 2){
                /* 点击左软键区域 */
                mrc_printf("menuEvent: 点击左软键区域, menuIndex=%d", menuIndex);
                if(menuIndex == MENU_RUN){
                    mrc_printf("menuEvent: 启动应用: %s", appList[appListIndex]->path);
                    mrc_runMrp(appList[appListIndex]->path, "start.mr", NULL);
                }
            }else{
                /* 点击右软键区域 */
                mrc_printf("menuEvent: 点击右软键区域，返回列表");
                switchWindow(WINDOW_LIST);
            }
        }
        else if(param1 >= LIST_START_Y && param1 < LIST_START_Y + 4 * LINE_HEIGHT){
            /* 点击菜单项 */
            clickedRow = (param1 - LIST_START_Y) / LINE_HEIGHT;
            mrc_printf("menuEvent: 点击菜单项，计算行号=%d", clickedRow);
            if(clickedRow >= 0 && clickedRow < 4){
                if(menuIndex == clickedRow){
                    /* 双击执行 */
                    mrc_printf("menuEvent: 双击菜单项%d", menuIndex);
                    if(menuIndex == MENU_RUN){
                        mrc_printf("menuEvent: 启动应用: %s", appList[appListIndex]->path);
                        mrc_runMrp(appList[appListIndex]->path, "start.mr", NULL);
                    }
                    else if(menuIndex == MENU_INFO){
                        mrc_printf("menuEvent: 切换到应用信息窗口");
                        switchWindow(WINDOW_INFO);
                    }
                }else{
                    mrc_printf("menuEvent: 选中新菜单项，从%d变为%d", menuIndex, clickedRow);
                    menuIndex = clickedRow;
                    drawMenuScreen();
                }
            }else{
                mrc_printf("menuEvent: 错误：计算的行号超出范围: %d", clickedRow);
            }
        }else{
            mrc_printf("menuEvent: 警告：触摸坐标不在菜单区域，param1=%d", param1);
        }
    }
    
    /* 函数执行结束日志 */
    mrc_printf("menuEvent: 函数执行结束");
}

/* 应用信息事件处理 */
void infoEvent(int32 code, int32 param0, int32 param1)
{
    mrc_printf("infoEvent ... ");
    if(code == MR_KEY_RELEASE){
        if(param0 == MR_KEY_SOFTRIGHT || param0 == MR_KEY_SELECT){
            /* 右软键或确认键：返回菜单 */
            switchWindow(WINDOW_MENU);
        }
    }
    else if(code == MR_MOUSE_UP){
        /* 触摸事件 */
        if(param1 > SCRH - FOOTER_HEIGHT){
            /* 点击底部软键区域：返回菜单 */
            switchWindow(WINDOW_MENU);
        }
        else{
            /* 点击其他区域：返回菜单 */
            switchWindow(WINDOW_MENU);
        }
    }
}

/* 确认对话框事件处理 */
void confirmEvent(int32 code, int32 param0, int32 param1)
{
    mrc_printf("confirmEvent: code=%d, param0=%d, param1=%d", code, param0, param1);

    if(code == MR_KEY_RELEASE){
        if(param0 == MR_KEY_SOFTLEFT || param0 == MR_KEY_SELECT){
            /* 左软键或确认键：确定删除 */
            mrc_printf("confirmEvent: 确定删除应用");
            deleteCurrentApp();
        }
        else if(param0 == MR_KEY_SOFTRIGHT){
            /* 右软键：取消，返回菜单 */
            mrc_printf("confirmEvent: 取消删除，返回菜单");
            switchWindow(WINDOW_MENU);
        }
    }
    else if(code == MR_MOUSE_UP){
        /* 触摸事件 */
        if(param1 > SCRH - FOOTER_HEIGHT){
            /* 点击底部软键区域 */
            if(param0 < SCRW / 2){
                /* 点击左软键区域：确定删除 */
                mrc_printf("confirmEvent: 点击确定按钮");
                deleteCurrentApp();
            }else{
                /* 点击右软键区域：取消 */
                mrc_printf("confirmEvent: 点击取消按钮");
                switchWindow(WINDOW_MENU);
            }
        }
        else{
            /* 点击对话框外区域：取消 */
            mrc_printf("confirmEvent: 点击对话框外区域，取消");
            switchWindow(WINDOW_MENU);
        }
    }
}

/* 初始化函数 */
int32 mrc_init(void)
{
    int32 textWidth = 0;
    int32 textHeight = 0;
    int availableHeight;

    mrc_printf("mpc init");
    mpc_init();
    
    mrc_printf("uc3 init");
    uc3_init();

    mrc_printf("sand init");
    mrc_sand(mrc_getUptime());

    /* 计算行高和可见行数 */
    gl_textWidthHeight("\xb2\xe2", 0, 1, &textWidth, &textHeight);
    if(textHeight<16){
        textHeight = 16;
    }
    FONT_HEIGHT = textHeight;
    LINE_HEIGHT = (textHeight > 24) ? textHeight : textHeight + 6;  /* 文字高度 + 上下边距 */
    availableHeight = SCRH - HEADER_HEIGHT - FOOTER_HEIGHT;
    VISIBLE_ITEMS = availableHeight / LINE_HEIGHT;
    mrc_printf("LINE_HEIGHT=%d, VISIBLE_ITEMS=%d", LINE_HEIGHT, VISIBLE_ITEMS);

    /* 初始化应用列表 */
    mrc_printf("init app list");
    appListMax = 1000;
    appListLen = 0;
    appListIndex = 0;
    appListScrollTop = 0;
    appList = mrc_malloc(appListMax * sizeof(MrpInfo*));
    mrc_memset(appList, 0, appListMax * sizeof(MrpInfo*));

    /* 扫描mrp文件 */
    mrc_printf("scan mrp files");
    scanMrpFiles();

    /* 切换到列表窗口 */
    currentWindow = WINDOW_LIST;
    drawListScreen();
    mrc_printf("old [%s] [%s] %d %d", mr_table->pack_filename, mr_table->start_filename, mrc_strlen(mr_table->old_pack_filename), mrc_strlen(mr_table->old_start_filename));
    

    return 0;
}

/* 事件处理函数 */
int32 mrc_event(int32 code, int32 param0, int32 param1)
{
    mrc_printf("mrc_event....");
    if(currentWindow == WINDOW_LIST){
        listEvent(code, param0, param1);
    }else if(currentWindow == WINDOW_MENU){
        menuEvent(code, param0, param1);
    }else if(currentWindow == WINDOW_INFO){
        infoEvent(code, param0, param1);
    }else if(currentWindow == WINDOW_CONFIRM){
        confirmEvent(code, param0, param1);
    }
    else{
        mrc_printf("window state %d", currentWindow);
    }

    return 0;
}

/* 暂停函数 */
int32 mrc_pause()
{
    return 0;
}

/* 恢复函数 */
int32 mrc_resume()
{
    return 0;
}

/* 退出函数 */
int32 mrc_exitApp()
{

    /* 释放应用列表 */
    freeAppList();

    /* 释放字体 */
    uc3_free();

    gl_free();

    return 0;
}

/* 插件事件函数 */
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
