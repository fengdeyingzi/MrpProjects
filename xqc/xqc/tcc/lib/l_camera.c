#include <tcc.h>
#include <mrc_camera.h>


int32 l_camera_init(TCCState *s)
{
	if(s==NULL)
		return -1;

	tcc_add_symbol(s,"mrc_Camera_init", (void*)mrc_Camera_init);
	tcc_add_symbol(s,"mrc_Camera_creat", (void*)mrc_Camera_creat);
	tcc_add_symbol(s,"mrc_Camera_capture", (void*)mrc_Camera_capture);
	tcc_add_symbol(s,"mrc_Camera_destroy", (void*)mrc_Camera_destroy);
	tcc_add_symbol(s,"mrc_Camera_pause", (void*)mrc_Camera_pause);
	tcc_add_symbol(s,"mrc_Camera_resume", (void*)mrc_Camera_resume);
	tcc_add_symbol(s,"mrc_Camera_getZoom", (void*)mrc_Camera_getZoom);
	tcc_add_symbol(s,"mrc_Camera_zoomOut", (void*)mrc_Camera_zoomOut);
	tcc_add_symbol(s,"mrc_Camera_zoomIn", (void*)mrc_Camera_zoomIn);
	tcc_add_symbol(s,"mrc_Camera_status", (void*)mrc_Camera_status);

	return 0;
}