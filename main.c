#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>

#include "graphics.h"

char menu_items[][50] = {" 	 Continuar - ir a la secuencia de inicio normal"," 	 Boot - arranque en diferentes modos"," 	 Fixes - soluciones comunes para problemas al detener el arranque"," 	 Mount - Montar puntos de particiones"," 	 Extras - Algo más"};

char menu_options [][6][26] = {  {"Normal","shell2.self"} , {"Suspender","Restaurar","IDU activo","IDU desactivado","Modo Seguro"} , {"Borrar id.dat","Borrar act.dat","Borrar config.txt de tai","Restaurar registro"} , {"Montar Memory Card","Desmontar Memory Card"} , {"Iniciar vitashell","Obtener información del sistema","Testear botones","Limpiar LOG"}  };

int selected = 0;
int sub_selected = 0;
int item_count = 5;
int i;
int pressed;

char log_text[800];

void select_menu(){
	psvDebugScreenClear(COLOR_BLACK);
	psvDebugScreenSetFgColor(COLOR_RED);
	psvDebugScreenPrintf("                     --[Menu Recovery]--                         \n");
	psvDebugScreenPrintf("                     --[HaiMenu  v0.2]--                        \n");
	psvDebugScreenSetFgColor(COLOR_GREEN);
	psvDebugScreenPrintf("Opcion(%d,%d): %s.\n\n",selected,sub_selected,menu_options[selected][sub_selected]);
	psvDebugScreenSetFgColor(COLOR_WHITE);
	
	for(i = 0; i < item_count; i++){
		if(selected==i){
			psvDebugScreenSetFgColor(COLOR_CYAN);
		}
		
		psvDebugScreenPrintf("%s\n", menu_items[i]);
		psvDebugScreenSetFgColor(COLOR_WHITE);
	}
	
	psvDebugScreenSetFgColor(COLOR_YELLOW);
	psvDebugScreenPrintf("\n\nLog:\n\n%s",log_text);
	
	if(strlen(log_text) > 780 ){
		strcpy(log_text,"");
	}
}

int main()
{
	psvDebugScreenInit();
	psvDebugScreenClear(0x333333);
	SceCtrlData pad;
	int ret;
	char con_data[128];
	
	int found = 0;// /shrug
	for(int tries = 0; tries < 30; tries++){//Check if clicked
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons == SCE_CTRL_RTRIGGER || pad.buttons == SCE_CTRL_LTRIGGER || pad.buttons == SCE_CTRL_L1 || pad.buttons == SCE_CTRL_R1 ) {
			found = 1;
			break;
		}
	
		sceKernelDelayThread(0.1 * 1000 * 1000);
	}
	
	
	if (found == 0) {
		sceKernelExitProcess(0);
	}
	
	select_menu();
		
	while (1) {
			sceCtrlPeekBufferPositive(0, &pad, 1);

			if (pad.buttons == SCE_CTRL_CROSS) {
				switch (selected){//ADD CHECKS TO LOG
					case 0://Continue
						switch (sub_selected){
							case 0:
								sprintf(con_data, "Continuar secuencia de arranque... ");
								strcat(log_text,con_data);
								select_menu();
								sceKernelDelayThread(1 * 1000 * 1000);
								sceKernelExitProcess(0);
								break;
							case 1:
								ret = sceAppMgrLoadExec("vs0:vsh/shell/shell2.self", NULL, NULL);//DOESNT WORK
								sprintf(con_data, "Cargando shell2.self: %d ",ret);
								strcat(log_text,con_data);
								select_menu();
								break;
						}
						break;
						
					case 1://Boot
						switch (sub_selected){
							case 0://Suspend
								ret = scePowerRequestSuspend();
								sprintf(con_data, "Suspender: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 1://Restart
								ret = scePowerRequestColdReset();
								sprintf(con_data, "Restaurar: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 2://IDU Enable
								ret = vshSysconIduModeSet();
								sprintf(con_data, "IDU activado: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 3://IDU Disable
								ret = vshSysconIduModeClear();
								sprintf(con_data, "IDU desactivado: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 4://Safe mode
								ret = sceAppMgrLoadExec("os0:ue/safemode.self", NULL, NULL);//DOESNT WORK
								sprintf(con_data, "Modo seguro: %d ", ret);
								strcat(log_text,con_data);
								break;
							
						}
						break;
					case 2:
						switch (sub_selected){
							case 0://Delete id.dat
								ret = sceIoRemove("ux0:/id.dat");
								sprintf(con_data, "Borrar id.dat: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 1://Delete act.dat
								ret = sceIoRemove("tm0:npdrm/act.dat");
								sprintf(con_data, "Borrar act.dat: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 2://Delete tai config
								ret = sceIoRemove("ux0:tai/config.txt");
								sprintf(con_data, "Borrar configuracion tai de la tarjeta de memoria: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 3://Restore registry 
								ret = sceIoRemove("vd0:registry/system.ireg");
								sprintf(con_data, "Restaurando iregistry: %d ", ret);
								strcat(log_text,con_data);
								
								ret = sceIoRemove("vd0:registry/system.dreg");
								sprintf(con_data, "Restaurando dregistry: %d ", ret);
								strcat(log_text,con_data);
								
								break;
							
						}
						break;
					case 3:
						switch (sub_selected){
							case 0://Mount mem card
								ret = _vshIoMount(0x800, NULL, 0, 0, 0, 0);
								sprintf(con_data, "Montar Tarjeta de memoria: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 1://Unmount mem card
								ret = vshIoUmount(0x800, 0, 0, 0);
								sprintf(con_data, "Desmontar Tarjeta de memoria: %d ", ret);
								strcat(log_text,con_data);
								break;
							
						}
						break;
					case 4:
						switch (sub_selected){
							case 0://Start vitashell
								ret = sceAppMgrLoadExec("ux0:app/MLCL00001/eboot.bin", NULL, NULL);//DOESNT WORK
								sprintf(con_data, "Arrancando comando: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 1://SYS INFO
								sceRegMgrGetKeyStr("/CONFIG/TEL", "sim_unique_id", con_data, 6 * 16);//IMEI
								strcat(log_text,"IMEI: ");
								strcat(log_text,con_data);
								strcat(log_text," ");
								
								char CID[16];
								_vshSblAimgrGetConsoleId(CID);//CID
								
								strcat(log_text,"CID: ");
								
								for (i = 0; i < 16; i++) {
										sprintf(con_data,"%02X", CID[i]);
										strcat(log_text,con_data);
								}
								
								
								strcat(log_text," ");
								break;
								
								
							case 2:
								for(int tries = 0; tries < 10; tries++){//Check if clicked
									sceCtrlPeekBufferPositive(0, &pad, 1);
									
									sprintf(con_data, "Botones(%d): %d ", tries, pad.buttons);
									strcat(log_text,con_data);
									select_menu();
								
									sceKernelDelayThread(1 * 1000 * 1000);
								}
								break;
							case 3:
								strcpy(log_text,"");
								break;
							
						}
						break;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
			
			if (pad.buttons == SCE_CTRL_START) {
				sceKernelExitProcess(0);
			}
			
			if (pad.buttons == SCE_CTRL_UP) {
				if(selected!=0){
					selected--;
					sub_selected = 0;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
			
			if (pad.buttons == SCE_CTRL_DOWN) {
				if(selected+1<item_count){
					selected++;
					sub_selected = 0;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
			
			
			if (pad.buttons == SCE_CTRL_LEFT) {
				if(sub_selected!=0){
					sub_selected--;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
			
			if (pad.buttons == SCE_CTRL_RIGHT) {
				if(strlen(menu_options[selected][sub_selected+1]) > 2 && sub_selected+1 < 6){
					sub_selected++;
				}
				select_menu();
				sceKernelDelayThread(0.3 * 1000 * 1000);
			}
	}
	
	sceKernelDelayThread(10 * 1000 * 1000);
	sceKernelExitProcess(0);
    return 0;
}
