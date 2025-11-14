#include <mrc_base.h>



#ifdef SSS
int32 mrc_init(){
    char temp[10];
    // mrc_sprintf(temp, "%s", MR_VERSION);
    mrc_clearScreen(240, 240, 240);
    mrc_refreshScreen(0, 0, 240, 320);
    return 0;
}
#endif
int32 mrc_event(int32 type, int32 p1, int32 p2){
    int32 f;
    char temp[300];
    int32 len;
    if(type == MR_KEY_PRESS){
        if(p1 == MR_KEY_0){
             mrc_printf("open...");
            f = mrc_open("a.txt", MR_FILE_CREATE);
            mrc_close(f);
            mrc_printf("open success");
        }
        if(p1== MR_KEY_1){
            mrc_printf("open...");
            f = mrc_open("a.txt", MR_FILE_RDONLY);
            mrc_close(f);
        }
        if(p1== MR_KEY_2){
            f = mrc_open("a.txt", MR_FILE_WRONLY);
            mrc_close(f);
        }
        if(p1 == MR_KEY_3){
            f = mrc_open("a.txt", MR_FILE_WRONLY);
            mrc_close(f);
        }
        if(p1 == MR_KEY_4){
        }
        if(p1 == MR_KEY_5){
            mrc_fileState("mrcc");
        }
        if(p1 == MR_KEY_6){
            mrc_fileState("c");
        }
        if(p1 == MR_KEY_7){
            mrc_fileState("dsm_gm.mrp");
        }
    }
    return 0;
}


int32 mrc_pause(){
    return 0;
}

int32 mrc_resume(){
    return 0;
}

int32 mrc_exitApp(){
    return 0;
}


int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1)
{
    return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 p0, int32 p1, int32 p2, int32 p3, int32 p4, int32 p5)
{
    return 0;
}