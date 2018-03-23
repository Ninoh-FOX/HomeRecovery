#include "init.h"

int SCE_CTRL_ENTER = SCE_CTRL_CROSS;
int SCE_CTRL_CANCEL = SCE_CTRL_CIRCLE;
/// SceAppUtil /////////////////////////////////////////////////////////////////////////////////////////////

void initSceAppUtil() {
	SceAppUtilInitParam init_param;
	SceAppUtilBootParam boot_param;
	memset(&init_param, 0, sizeof(SceAppUtilInitParam));
	memset(&boot_param, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&init_param, &boot_param);

}

void finishSceAppUtil() {
	sceAppUtilShutdown();
}




int readInEnterButton() {
	int enter_button = 0;
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON, &enter_button);
	
	if (enter_button == SCE_SYSTEM_PARAM_ENTER_BUTTON_CIRCLE) { //switch butons
		SCE_CTRL_ENTER = SCE_CTRL_CIRCLE;
		SCE_CTRL_CANCEL = SCE_CTRL_CROSS;
	} 
	
	
	return enter_button;
}

char *getUsername() {
	static SceChar8 username[SCE_SYSTEM_PARAM_USERNAME_MAXSIZE];
	sceAppUtilSystemParamGetString(SCE_SYSTEM_PARAM_ID_USERNAME, username, SCE_SYSTEM_PARAM_USERNAME_MAXSIZE);
	
	return (char *)username;
}

int getSystemLanguageCode() {
	int systemlanguage = 0;
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &systemlanguage);
	
	return systemlanguage;
}

int get_key(void) {
	static unsigned buttons[] = {
		SCE_CTRL_SELECT,
		SCE_CTRL_START,
		SCE_CTRL_UP,
		SCE_CTRL_RIGHT,
		SCE_CTRL_DOWN,
		SCE_CTRL_LEFT,
		SCE_CTRL_LTRIGGER,
		SCE_CTRL_RTRIGGER,
		SCE_CTRL_TRIANGLE,
		SCE_CTRL_CIRCLE,
		SCE_CTRL_CROSS,
		SCE_CTRL_SQUARE,
	};

	static unsigned prev = 0;
	SceCtrlData pad;
	while (1) {
		memset(&pad, 0, sizeof(pad));
		sceCtrlPeekBufferPositive(0, &pad, 1);
		unsigned newb = prev ^ (pad.buttons & prev);
		prev = pad.buttons;
		for (int i = 0; i < sizeof(buttons)/sizeof(*buttons); ++i)
			if (newb & buttons[i])
				return buttons[i];
	}
};