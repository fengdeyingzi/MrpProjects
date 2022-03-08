#include <tcc.h>
#include <mrc_bmp.h>


int32 l_bmp_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"mrc_bmpCreate", (void*)mrc_bmpCreate);
	tcc_add_symbol(s,"mrc_bmpFree", (void*)mrc_bmpFree);
	tcc_add_symbol(s,"mrc_bmpLoadFromFile", (void*)mrc_bmpLoadFromFile);
	tcc_add_symbol(s,"mrc_bmpLoadFromBuffer", (void*)mrc_bmpLoadFromBuffer);
	tcc_add_symbol(s,"mrc_bmpLoadFromMrp", (void*)mrc_bmpLoadFromMrp);
	tcc_add_symbol(s,"mrc_bmpWriteFile", (void*)mrc_bmpWriteFile);
	tcc_add_symbol(s,"mrc_bmpGetWidth", (void*)mrc_bmpGetWidth);
	tcc_add_symbol(s,"mrc_bmpGetHeight", (void*)mrc_bmpGetHeight);
	tcc_add_symbol(s,"mrc_bmpGetDepth", (void*)mrc_bmpGetDepth);
	tcc_add_symbol(s,"mrc_bmpGetPixelRGB", (void*)mrc_bmpGetPixelRGB);
	tcc_add_symbol(s,"mrc_bmpSetPixelRGB", (void*)mrc_bmpSetPixelRGB);
	tcc_add_symbol(s,"mrc_bmpGetPixelIndex", (void*)mrc_bmpGetPixelIndex);
	tcc_add_symbol(s,"mrc_bmpSetPixelIndex", (void*)mrc_bmpSetPixelIndex);
	tcc_add_symbol(s,"mrc_bmpGetColorUsed", (void*)mrc_bmpGetColorUsed);
	tcc_add_symbol(s,"mrc_bmpGetPaletteColor", (void*)mrc_bmpGetPaletteColor);
	tcc_add_symbol(s,"mrc_bmpSetPaletteColor", (void*)mrc_bmpSetPaletteColor);
	tcc_add_symbol(s,"mrc_bitmapGetInfo", (void*)mrc_bitmapGetInfo);
	tcc_add_symbol(s,"w_getScreenBuffer", (void*)w_getScreenBuffer);
	tcc_add_symbol(s,"w_setScreenBuffer", (void*)w_setScreenBuffer);
	tcc_add_symbol(s,"mrc_getScreenSize", (void*)mrc_getScreenSize);
	tcc_add_symbol(s,"mrc_setScreenSize", (void*)mrc_setScreenSize);
	tcc_add_symbol(s,"mrc_bmpGet16BitLineFrom8BitData", (void*)mrc_bmpGet16BitLineFrom8BitData);
	tcc_add_symbol(s,"mrc_bmpBufferDraw", (void*)mrc_bmpBufferDraw);
	tcc_add_symbol(s,"mrc_bmpBufferDrawEX", (void*)mrc_bmpBufferDrawEX);
	tcc_add_symbol(s,"mrc_bmpShow", (void*)mrc_bmpShow);
	tcc_add_symbol(s,"mrc_bmpShowEx", (void*)mrc_bmpShowEx);
	tcc_add_symbol(s,"mrc_bmpGetError", (void*)mrc_bmpGetError);
	tcc_add_symbol(s,"mrc_bmpBufferDrawOld", (void*)mrc_bmpBufferDrawOld);
	tcc_add_symbol(s,"mrc_bmpShowOld", (void*)mrc_bmpShowOld);
	tcc_add_symbol(s,"mrc_PrintScreen", (void*)mrc_PrintScreen);
	tcc_add_symbol(s,"bitmapShowFlip", (void*)bitmapShowFlip);
	tcc_add_symbol(s,"mrc_bitmapShowFlip", (void*)mrc_bitmapShowFlip);

	return 0;
}