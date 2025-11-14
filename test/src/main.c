#include "mrc_base.h"
#include "sky_font16.h"

int32 mrc_init(void) {
	mrc_clearScreen(0, 0, 0);
	mrc_drawText("哈喽沃德", 0, 0, 255, 255, 255, 0, 1);
	mrc_refreshScreen(0, 0, 480, 640);

	/**
	 * 下面一行执行出错，错误信息：
	 * ERR:Error: L6218E: Undefined symbol xl_font_sky16_init (referred from main.o).
	 * ERR:Finished: 0 information, 0 warning and 1 error messages.
	 */
	xl_font_sky16_init();
	return 0;
}

int32 mrc_exitApp(void) {
	return 0;
}

int32 mrc_event(int32 code, int32 param0, int32 param1) {
	if (code == MR_KEY_PRESS && (param0 == MR_KEY_SOFTRIGHT || param0 == MR_KEY_POWER)) {
		mrc_exit();
	}
	return 0;
}

int32 mrc_pause() {
	return 0;
}

int32 mrc_resume() {
	return 0;
}

int32 mrc_extRecvAppEventEx(int32 code, int32 param0, int32 param1) {
	return MR_SUCCESS;
}

int32 mrc_extRecvAppEvent(int32 app, int32 code, int32 param0, int32 param1) {
	return MR_SUCCESS;
}