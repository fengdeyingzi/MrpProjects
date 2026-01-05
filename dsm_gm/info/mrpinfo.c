#include <mrc_base.h>
#include "mrpinfo.h"




//读取mrp信息
int32 readMrpInfo(char* path, Config* config){
    int32 f;
    f = mrc_open(path, MR_FILE_RDONLY);
    if(f!=0){
        mrc_read(f, config, sizeof(Config));
        mrc_close(f);
        if(mrc_strncmp(config->magic, "MRPG", 4) == 0){
            return MR_SUCCESS;
        }
        else{
            return -1;
        }
    }
    return -1;
}