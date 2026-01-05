//一个简单的列表
//加载系统头文件base.h
#include "filelist.h"
// #include <mrc_types.h>
#include <mrc_base.h>
#include "xl_coding.h"
#include "mrpinfo.h"


//创建控件
void *list_create(int x, int y, int w, int h) {
	ListView *view = malloc(sizeof(ListView));
	memset(view, 0, sizeof(ListView));
	view->x = x;
	view->y = y;
	view->w = w;
	view->h = h;
	view->isshow = 1;
	view->backgroundColor = 0x00000000;
	view->timer = timercreate();
	view->list_max = 1000;
    view->list_len = 0;
    view->list_index = 0;
	view->list = malloc(view->list_max * sizeof(int32));
	memset(view->list, 0, view->list_max * sizeof(int32));
    view->list_windowsize = h/26;

	return view;
}

//设置id
void list_setid(ListView *view, int id) { view->id = id; }

//获取id
int list_getid(ListView *view) { return view->id; }

//绘制控件
void list_draw(ListView *view) {
	int i = 0;
	int ix = view->x, iy = view->y;
	int32 fontw, fonth;
	mr_screeninfo info;
	char *temp = NULL;
    int32 ry,rh;
    char temp_utf[300];
	mrc_getScreenInfo(&info);
	mrc_textWidthHeight("\x5f\x71\0\0", 1, 1, &fontw, &fonth);
    mrc_printf("list_draw %d",0);
	if (view->isshow && view->list_len>0) {
        mrc_printf("list_draw %d",1);
        ry = view->y + view->h * view->list_progress/view->list_len;
        rh = view->h * (view->list_progress+view->list_windowsize)/view->list_len;
		// drawRect(view->x, view->y, view->w, view->h, view->backgroundColor);
		//画光标
		mrc_drawRect(ix, (view->list_index - view->list_progress) * 26 + view->y, info.width, 26, 65, 200, 240);
		//画item
		for (i = view->list_progress; i < view->list_len; i++) {
            mrc_printf("list_draw %d",2);
			temp = view->list[i]->DisplayName;
            if(view->list[i]->isUTF){
                UTF8ToUni(temp, temp_utf,300);
                mrc_drawText(temp_utf, ix + 10, iy + (26 - fonth) / 2, 20, 20, 20, 1, 1);
            }else{
                mrc_drawText(temp, ix + 10, iy + (26 - fonth) / 2, 20, 20, 20, 0, 1);
            }
			
			iy += 26;
			if (iy > info.height)
				break;
			if (iy > view->y + view->h - 26)
				break;
		}
        //画进度条
        mrc_drawRect(view->x+view->w - 4, ry, 4, rh, 200,200,200);
	}
}

//设置绘制模式
void list_setMode(ListView *view, int mode) { view->draw_mode = mode; }

//显示
void list_show(ListView *view) {
	view->isshow = 1;
	if (view->draw_mode == 0) {
		list_draw(view);
		ref(view->x, view->y, view->w, view->h);
	}
}

//隐藏
void list_hide(ListView *view) { view->isshow = 0; }

//判断是否显示
int list_isshow(ListView *view) { return view->isshow; }

//设置背景色 argb格式
void list_setBackgroundColor(ListView *view, int color) {
	view->backgroundColor = color;
	if (view->draw_mode == 0) {
		list_draw(view);
		ref(view->x, view->y, view->w, view->h);
	}
}

//设置点击事件
void list_setonclick(ListView *view, void *click) { view->onclick = click; }

// 设置list列表
void list_add(ListView *view, char *filename) {
	int32 len = strlen(filename);
    MRPHEADER *header = mrc_malloc(sizeof(MRPHEADER));
	char *temp = NULL;
    int32 f;
	if (len < 255)
		len = 255;
	if (view->list_len <= view->list_max - 1) {
		temp = mrc_malloc(len * sizeof(char));
        mrc_memset(header,0,sizeof(MRPHEADER));
		strcpy(temp, filename);
        header->path = temp;
        mrc_printf("lsit_add 1");
        
        mrc_printf("lsit_add 1");
        f = mrc_open(temp,1);
        mrc_printf("lsit_add 1");
        if(f!=0){
            mrc_seek(f,28, MR_SEEK_SET);
		
        mrc_printf("list_add 2");
        mrc_read(f, header->DisplayName, 24);
        mrc_printf("list_add 2");
        mrc_seek(f, 128, MR_SEEK_SET);
        mrc_printf("list_add 2");
        mrc_read(f, header->Desc, 64);
        mrc_printf("list_add 2");
        mrc_close(f);
        mrc_printf("list_add 2");
		view->list[view->list_len++] = header;
        mrc_printf("list_add 3");
        }
        else{
			mrc_printf("list_add 1 failed %s",temp);

            mrc_free(header);
            mrc_free(temp);
        }
        
        
	}
}

void list_clear(ListView *view){
	int i = 0;
	for(i=0;i<view->list_len;i++){
		mrc_free(view->list[i]->path);
		mrc_free(view->list[i]);
		view->list[i] = NULL;
	}
	view->list_index = 0;
	view->list_len = 0;
	view->list_progress = 0;
}

void list_addData(ListView *view, char *label,char *desc,char *downloadUrl) {
    char *temp = NULL;
    MRPHEADER *header = mrc_malloc(sizeof(MRPHEADER));
	mrc_memset(header,0,sizeof(header));
    
    header->path = mrc_malloc(300);
	mrc_memset(header->path,0,300);
    // memcpy(header->DisplayName, label, MIN(strlen(label),24));
    // memcpy(header->Desc,desc, MIN(strlen(desc),64));
    mrc_strcpy(header->path, downloadUrl);
    mrc_strncpy(header->DisplayName, label, MIN(mrc_strlen(label),24)+1);
    mrc_strncpy(header->Desc,desc, MIN(mrc_strlen(desc),64)+1);
    header->isDownload = 1;
    header->isUTF = 1;
    view->list[view->list_len++] = header;
	mrc_printf("addData %s %s %s",label,desc,header->path);
}

MRPHEADER *list_getCurItem(ListView *list){
    return list->list[list->list_index];
}

void list_find_txt(ListView *view, char *file)	//搜索文件到文本，参数。：路径 文本
{
	int32 f;
	char *endfile;
	// int32 i;
	int32 ret = 0;
	char *temp = mrc_malloc(255);
	int list_n = 0;
	Config* config;
	config = mrc_malloc(sizeof(Config));

	f = findstart(file, temp, 72);
	mrc_printf("findstart %s", temp);
	if (temp != NULL && *temp != '.') {
		list_n++;
		list_add(view, temp);
	}

	while (ret == 0) {
		ret = findnext(f, temp, 72);
		mrc_printf("findnext %s", temp);
		if (ret)
			break;
		if (temp == NULL)
			break;
		if (*temp != '.') {
			list_n++;
			endfile = mrc_strrchr(temp,'.');
			if(endfile != NULL)
			if(mrc_strcmp(endfile,".mrp") == 0 || mrc_strcmp(endfile,".MRP") == 0){
				if(readMrpInfo(temp, config) == MR_SUCCESS){
					if(config->flag & 0x01){
						list_add(view, temp);
					}
				}
			}
			

			// scr+=strlen(scr);
		}
	}
	findstop(f);
	mrc_free(config);
	mrc_free(temp);
}
//判断点(x,y)是否在矩形区域(rectx,recty,rectw,recth)内
static int isPointCollRect(int x,int y,int rectx,int recty,int rectw,int recth)
{
	if(x>=rectx && x<rectx+rectw && y>=recty && y<recty+recth)
	{
		return TRUE;
	}
	return FALSE;
}

static int isMove;
static int32 down_x,down_y;
//控件event事件
int list_event(ListView *view, int32 type, int32 p1, int32 p2) {
    int32 index_y;
	mrc_printf("list_event %d,%d,%d", type, p1, p2);
	if (view->isshow) {
        
		if (type == MR_MOUSE_DOWN) {
            isMove = 0;
			if (isPointCollRect(p1, p2, view->x, view->y, view->w, view->h)) {
				view->isdown = 1;
                down_x = p1;
                down_y = p2;
                
			}
            
            

		}
        else if(type == MR_MOUSE_MOVE){
            
            if(view->isdown){
                if(down_x != p1 || down_y!=p2){
                    isMove = 1;
                }
                
            }
        }
        else if(type == MR_MOUSE_UP){
            if(view->isdown && !isMove){
                //计算点中位置
                index_y = view->list_progress + (p2 - view->y) / 26;
                if(view->list_index != index_y){
                    view->list_index = index_y;
                    if(view->list_index>=view->list_len) view->list_index = view->list_len-1;
                }
                else{
                    timerstart(view->timer, 10, view->id, (mrc_timerCB)view->onclick, 0);
                }
            }
            view->isdown = 0;
            isMove = 0;
        }
         else if (type == MR_MOUSE_UP) {
			if (view->isdown) {
				view->isdown = 0;
				// if (view->onclick != NULL) {
				// 	timerstart(view->timer, 10, view->id, (mrc_timerCB)view->onclick, 0);
				// }
			}
		} else if (type == MR_KEY_RELEASE) {
			if (p1 == MR_KEY_DOWN) {
                mrc_printf("down");
				view->list_index++;
				if (view->list_index >= view->list_len) {
					view->list_index = 0;
					view->list_progress = 0;
				}
                if(view->list_index - view->list_progress >= view->list_windowsize){
                    view->list_progress = view->list_index;
                }
			}
			if (p1 == MR_KEY_UP) {
                mrc_printf("up");
				view->list_index--;
                if(view->list_index<view->list_progress){
                    view->list_progress = view->list_index-view->list_windowsize+1;
                    if(view->list_progress<0){
                        view->list_progress = 0;
                    }
                }
				if (view->list_index < 0) {
					view->list_index = view->list_len - 1;
					view->list_progress = view->list_len - 1;
				}
                
			}
            if(p1 == MR_KEY_LEFT){
                if(view->list_progress>view->list_windowsize){
view->list_index-= view->list_windowsize;
view->list_progress-= view->list_windowsize;
                }
                else if(view->list_index!=0){
                    view->list_index = 0;
view->list_progress = 0;
                }
                else{
                    view->list_index = view->list_len - 1;
                    view->list_progress = view->list_len - 1;
                }

                if(view->list_index<view->list_progress){
                    view->list_progress = view->list_index-view->list_windowsize;
                    if(view->list_progress<0){
                        view->list_progress = 0;
                    }
                }
				if (view->list_index < 0) {
					view->list_index = view->list_len - 1;
					view->list_progress = view->list_len - 1;
				}
            }
            else if(p1 == MR_KEY_RIGHT){
view->list_index+= view->list_windowsize;
view->list_progress+= view->list_windowsize;
				if (view->list_index >= view->list_len) {
					view->list_index = 0;
					view->list_progress = 0;
				}
                if(view->list_index - view->list_progress > view->list_windowsize){
                    view->list_progress = view->list_index;
                }
            }
		}
	}

	return 0;
}

//释放内存
void list_free(ListView *view) {
    int i;
	if (view->timer) {
		timerstop(view->timer);
		timerdel(view->timer);
		view->timer = 0;
	}
    for(i = 0;i<view->list_len;i++){
        free(view->list[i]->path);
        free(view->list[i]);
    }
    free(view->list);

	free(view);
}
