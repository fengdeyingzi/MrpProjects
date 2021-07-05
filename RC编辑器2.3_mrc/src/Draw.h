void DrawShadeRect_x(int16 x, int16 y, int16 w, int16 h, uint32 ulLight, uint32 ulDark);
void DrawRoundRect(int16 x, int16 y, int16 w, int16 h, uint32 ulFrame);
void DrawTextMid(mr_screenRectSt drawRect, char *drawText, uint32 ulColor);
void DrawTextMidA(mr_screenRectSt drawRect, char *drawText, uint32 ulColor);
void DrawCmd(mr_screenRectSt cmdRect, char *Tile, uint32 ulLight, uint32 ulDark, int32 IsPress, int32 Reflush);