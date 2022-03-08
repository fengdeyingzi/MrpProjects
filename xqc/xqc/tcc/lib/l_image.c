#include <tcc.h>
#include <mrc_image.h>


int32 l_image_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"mrc_getImageInfo", (void*)mrc_getImageInfo);
	tcc_add_symbol(s,"mrc_drawJpegToFrameBuffer", (void*)mrc_drawJpegToFrameBuffer);
	tcc_add_symbol(s,"mrc_drawGifToFrameBuffer", (void*)mrc_drawGifToFrameBuffer);
	tcc_add_symbol(s,"mrc_stopGif", (void*)mrc_stopGif);
	tcc_add_symbol(s,"mrc_decodeGifToBuffer", (void*)mrc_decodeGifToBuffer);
	tcc_add_symbol(s,"mrc_releaseDecGifRes", (void*)mrc_releaseDecGifRes);

	return 0;
}