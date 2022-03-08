#include "mrc_base.h"
#include "test.h"

int ma()
{
	char aaa[15];
	sprintf(aaa,"%s",asdfg);
	mrc_clearScreen(255, 0, 240);
	mrc_drawText(aaa,33,66,0,250,0,0,0);
	mrc_refreshScreen(0, 0, 128, 128);
	return 0;
}
