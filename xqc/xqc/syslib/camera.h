typedef enum
{
STATUS_UNINIT,
STATUS_INIT,
STATUS_INIT_FAILED,
STATUS_START,
STATUS_START_FAILED,
STATUS_CAPTURE,
STATUS_CAPTURE_FAILED,
STATUS_PAUSE,
STATUS_RESUME_FAILED,
STATUS_CLOSE
}CAMERA_STATUS_T;
typedef enum
{
CAPTURE_MODE_MULTIPLE = 1,
CAPTURE_MODE_SINGLE = 2
}CAMERA_CAPTURE_MODE_T;
typedef enum
{
IMG_QUALITY_LOW,
IMG_QUALITY_NORMAL,
IMG_QUALITY_HIGH,
IMG_QUALITY_T;
typedef struct
{
void* img_data;
int32 img_data_len;
}CAMERA_IMG_DATA_T;