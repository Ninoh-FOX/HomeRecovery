#include <psp2/display.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "init.h"
#include "graphics.h"
#include "file.h"

#define printf psvDebugScreenPrintf

int get_key();
int scePowerRequestColdReset();
int _vshIoMount(int id, const char *path, int permission, void *buf);
int vshIoUmount(int id, int a2, int a3, int a4);
int doesFileExist();
int copyFile();
int WriteFile();
int main() {
	int i;
	void *buf = malloc(0x100);

	psvDebugScreenInit();
	
    if (doesFileExist("vs0:tai/boot_config.txt")) 

        {printf("          Esta consola tiene ENSO 3.65\n\n");

        {if (doesFileExist("vs0:tai/homerecovery.self")) {
	         printf("ya tienes HomeRecovery instalado \n\n");
	         printf("Quieres actualizar/Reinstalar? (X) Si    (O) No \n\n");
	         switch(get_key()) {
	case SCE_CTRL_CROSS: printf("Instalando HomeRecovery.");
		         for (i = 0; i < 15; i++) {
		         printf(".", (i * 0x100));
		         vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		         printf(".", (i * 0x100));
		         _vshIoMount(i * 0x100, 0, 2, buf); // id, unk, permission, work_buffer
	             }

	             copyFile("ux0:app/HMRC00001/data/homerecovery.self" ,"vs0:tai/homerecovery.self");

	             printf("\n\nHecho, reiniciando en 5s.");

	             sceKernelDelayThread(5 * 1000 * 1000);
	             scePowerRequestColdReset();
				 break;
	case SCE_CTRL_CIRCLE: printf("Saliendo de la aplicacion");
		         sceKernelDelayThread(5 * 1000 * 1000);
	             sceKernelExitProcess(0);
	             break;
	             }}
	     else
             {printf("Instalando HomeRecovery.");
	         for (i = 0; i < 15; i++) {
	    	 printf(".", (i * 0x100));
		     vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		     printf(".", (i * 0x100));
		     _vshIoMount(i * 0x100, 0, 2, buf); // id, unk, permission, work_buffer
	         }

	         copyFile("vs0:tai/boot_config.txt" ,"vs0:tai/boot_config.bkp");
	         copyFile("ux0:app/HMRC00001/data/boot_config365.txt", "vs0:tai/boot_config.txt");
	         copyFile("ux0:app/HMRC00001/data/homerecovery.self" ,"vs0:tai/homerecovery.self");

	         printf("\n\nHecho, reiniciando en 5s.");

	         sceKernelDelayThread(5 * 1000 * 1000);
	         scePowerRequestColdReset();}
             };
			 }
 
  else if (doesFileExist("ur0:tai/boot_config.txt")) 

        {printf("          Esta consola tiene ENSO 3.60\n\n");

        {if (doesFileExist("ur0:tai/homerecovery.self")) {
	         printf("ya tienes HomeRecovery instalado \n\n");
	         printf("Quieres actualizar/Reinstalar? (X) Si    (O) No \n\n");
	         switch(get_key()) {
	case SCE_CTRL_CROSS: printf("Instalando HomeRecovery.");
		         for (i = 0; i < 15; i++) {
		         printf(".", (i * 0x100));
		         vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		         printf(".", (i * 0x100));
		         _vshIoMount(i * 0x100, 0, 2, buf); // id, unk, permission, work_buffer
	             }

	             copyFile("ux0:app/HMRC00001/data/homerecovery.self" ,"ur0:tai/homerecovery.self");

	             printf("\n\nHecho, reiniciando en 5s.");

	             sceKernelDelayThread(5 * 1000 * 1000);
	             scePowerRequestColdReset();
				 break;
	case SCE_CTRL_CIRCLE: printf("Saliendo de la aplicacion");
		         sceKernelDelayThread(5 * 1000 * 1000);
	             sceKernelExitProcess(0);
	             break;
	             }}
	     else
             {printf("Instalando HomeRecovery.");
	         for (i = 0; i < 15; i++) {
	    	 printf(".", (i * 0x100));
		     vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		     printf(".", (i * 0x100));
		     _vshIoMount(i * 0x100, 0, 2, buf); // id, unk, permission, work_buffer
	         }

	         copyFile("ur0:tai/boot_config.txt" ,"ur0:tai/boot_config.bkp");
	         copyFile("ux0:app/HMRC00001/data/boot_config360.txt", "ur0:tai/boot_config.txt");
	         copyFile("ux0:app/HMRC00001/data/homerecovery.self" ,"ur0:tai/homerecovery.self");

	         printf("\n\nHecho, reiniciando en 5s.");

	         sceKernelDelayThread(5 * 1000 * 1000);
	         scePowerRequestColdReset();}
             }
	    }
		else printf("Consola sin enso\nSaliendo del programa");
		sceKernelDelayThread(5 * 1000 * 1000);
		sceKernelExitProcess(0);
}
