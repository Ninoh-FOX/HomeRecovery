#include <psp2/display.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "graphics.h"
#include "file.h"

#define printf psvDebugScreenPrintf

int scePowerRequestColdReset();
int _vshIoMount();
int vshIoUmount();
int doesFileExist();
int copyFile();
int WriteFile();
int main(int argc, char *argv[]) {
	int i;
	void *buf = malloc(0x100);

	psvDebugScreenInit();

	if (doesFileExist("vs0:tai/homerecovery.self")) {printf("ya tienes HomeRecovery instalado \n\n");
	printf("Saliendo en 5s...");
	sceKernelDelayThread(5 * 1000 * 1000);
	sceKernelExitProcess(0);}
	else
    {printf("Instalando HomeRecovery.");
	for (i = 0; i < 15; i++) {
		printf(".", (i * 0x100));
		vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		printf(".", (i * 0x100));
		_vshIoMount(i * 0x100, 0, 2, buf); // id, unk, permission, work_buffer
	}

	copyFile("vs0:tai/boot_config.txt" ,"vs0:tai/boot_config.bkp");
	copyFile("ux0:app/HMRC00001/data/boot_config.txt", "vs0:tai/boot_config.txt");
	copyFile("ux0:app/HMRC00001/data/homerecovery.self" ,"vs0:tai/homerecovery.self");

	printf("\n\nHecho, reiniciando en 5s.");

	sceKernelDelayThread(5 * 1000 * 1000);
	scePowerRequestColdReset();}

	return 0;
}
