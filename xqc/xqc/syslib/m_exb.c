#include "mrc_base.h"

int32 mrc_init()

{

	mrc_printf("ExtWorld be loaded!*****************");

	mrc_printf("ExtWorld Reg Self FuncPool***********");


	mrc_clearScreen(233,2,2);
	mrc_drawText("wwwwwwwwwddd",2,2,2,2,2,0,0);
	mrc_refreshScreen(0,0,240,320);
	mrc_sleep(1000);
	return MR_SUCCESS;

}



int32 mrc_appEvent(int32 code, int32 param0, int32 param1)

{

	mrc_printf("ExtWorld event code = %d, p0 = %d, p1 = %d", code, param0, param1);

	return MR_IGNORE;

}



int32 mrc_pause()

{

	return MR_SUCCESS;

}



int32 mrc_resume()

{

	return MR_SUCCESS;

}



int32 mrc_exitApp()

{

	mrc_printf("extWorld  mrc_exitApp");

	return MR_SUCCESS;

}
