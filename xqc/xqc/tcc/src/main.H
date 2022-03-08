#ifndef _MAIN_H_
#define _MAIN_H

typedef enum
{
	E_PAY,
	E_MAX
}E_MODULE_VALUE;
typedef int32 (*MAINS)(void);
typedef int32 (*EVENT)(int32 code, int32 param0, int32 param1);

typedef struct
{
	MAINS mains;
	EVENT events;
	MAINS pauses;
	MAINS resumes;
	MAINS exitapps;
}INTERFACE;
int32 tcc_exitApp(void);

void SubModule_notifyMain(E_MODULE_VALUE moduleValue, int32 msg);

typedef void (*PayEntry_CB)(void);


#endif
