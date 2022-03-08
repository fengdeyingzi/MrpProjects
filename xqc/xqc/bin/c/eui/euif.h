
typedef struct
{int16 x;int16 y;int16 w;int16 h;}rects;

typedef struct
{int16 r;int16 g;int16 b;}color16;

typedef struct
{
void *first; 
void *last; 
void *now; 
uint16 ge;
uint16 dge;
} owp;

//控件的链式指针
typedef struct
{
void *qian; 
void *hou; 
}objp;



typedef struct
{
char *text;int16 x;int16 y;uint16 w;uint16 h;color16 rgb;color16 RGB;uint8 mode;uint8 font;uint8 leixing;objp point; 
}wxy;
typedef struct
{int16 x;int16 y;color16 r;}tcolor;

