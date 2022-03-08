#include "application.h"
#include "topwnd.h"


int InitApplication(VOID)
{	
	InitTopWindow(); 

	// TODO: Add your code here!
	ShowTopWindow(TOPWND_MAINWND, 0, 0);
	return 0;
}


int ExitApplication(VOID)
{
	// TODO: Add your code here!
	return 0;
}


int PauseApplication(VOID)
{
	// TODO: Add your code here!
	return 0;
}


int ResumeApplication(VOID)
{
	// TODO: Add your code here!
	return 0;
}

