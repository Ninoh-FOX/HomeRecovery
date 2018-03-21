#include <vitasdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/devctl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/types.h>
#include <psp2/apputil.h>
#include <psp2/appmgr.h>
#include <psp2/ctrl.h>
#include <psp2/shellutil.h>
#include <psp2/sysmodule.h>
#include <psp2/system_param.h>
#include <psp2/rtc.h>
#include <psp2/touch.h>
#include <psp2/display.h>
#include <psp2/power.h>
#include <vita2d.h>
#include <taihen.h>
#include "graphics.h"
#include "file.h"

char menu_items[][60] = {" 	 Continuar - arranque normal"," 	 Boot - arranque en diferentes modos"," 	 Fixes - solucionar problemas de arranque"," 	 Mount - Montar puntos de particiones"," 	 Backup - copias de seguridad"," 	 Extras - Molecular, Vitashell, ..."};

char menu_options [][8][28] = {  {"salir"} , {"Reiniciar","IDU ON","IDU OFF (DEMO MODE)"} , {"Borrar id.dat","Borrar act.dat","Borrar ux0:tai/config.txt","Borrar ur0:tai/config.txt","Borrar registro"} , {"Montar MemCard","Desmontar MemCard"} , {"Copiar activacion","Restaurar activacion","Copiar ur0 tai","Resturar ur0 tai","Copiar ux0 tai","Restaurar ux0 tai"} , {"Molecular a NEAR","Restaurar NEAR","Cambiar NEARMOD x VITASHELL","informacion del sistema","Desinstalar RECOVERY","Limpiar LOG"}  };

int sceAppMgrLoadExec();
int scePowerRequestSuspend();
int scePowerRequestColdReset();
int vshSysconIduModeSet();
int vshSysconIduModeClear();
int _vshIoMount();
int vshIoUmount();
int _vshSblAimgrGetConsoleId();
int sceRegMgrGetKeyStr();
int selected = 0;
int sub_selected = 0;
int item_count = 6;
int i;
int pressed;
int WriteFile();
int mount();

char log_text[800];

void select_menu(){
	psvDebugScreenClear(0x00000000);
	psvDebugScreenSetFgColor(COLOR_YELLOW);
	psvDebugScreenPrintf("Ninoh-FOX            --[Menu Recovery]--                         \n");
	psvDebugScreenPrintf("                     --[HaiMenu v0.93]--            EOL.net      \n");
	psvDebugScreenSetFgColor(COLOR_GREY);
	psvDebugScreenPrintf("Opcion(%d,%d): %s.\n\n",selected,sub_selected,menu_options[selected][sub_selected]);
	psvDebugScreenSetFgColor(COLOR_GREY);
	
	for(i = 0; i < item_count; i++){
		if(selected==i){
			psvDebugScreenSetFgColor(COLOR_YELLOW);
		}
		
		psvDebugScreenPrintf("%s\n", menu_items[i]);
		psvDebugScreenSetFgColor(COLOR_GREY);
	}
	
	psvDebugScreenSetFgColor(COLOR_GREY);
	psvDebugScreenPrintf("\n\nLog:\n\n%s",log_text);
	
	if(strlen(log_text) > 780 ){
		strcpy(log_text,"");
	}
}

int main()
{
	psvDebugScreenInit();
	psvDebugScreenClear(0x0F0F0F0F);
	SceCtrlData pad;
	int ret;
	char con_data[128];
	
	int found = 0;// /shrug
	for(int tries = 0; tries < 30; tries++){//Check if clicked
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons == SCE_CTRL_RTRIGGER || pad.buttons == SCE_CTRL_R1 ) {
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
								sprintf(con_data, "Continuando arranque... ");
								strcat(log_text,con_data);
								select_menu();
								sceKernelDelayThread(2 * 1000 * 1000);
								sceKernelExitProcess(0);
								break;
						}
						break;
						
					case 1://Boot
						switch (sub_selected){
							case 0://Restart
								ret = scePowerRequestColdReset();
								sprintf(con_data, "Restaurar: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 1://IDU Enable
								ret = vshSysconIduModeSet();
								sprintf(con_data, "IDU activado: %d \n", ret);
								strcat(log_text,con_data);
								break;
							case 2://IDU Disable
								ret = vshSysconIduModeClear();
								sprintf(con_data, "IDU desactivado (DEMO MODE): %d \n", ret);
								strcat(log_text,con_data);
								break;
							
						}
						break;
					case 2:
						switch (sub_selected){
							case 0://Delete id.dat
								ret = sceIoRemove("ux0:/id.dat");
								sprintf(con_data, "Borrado id.dat: %d \n", ret);
								strcat(log_text,con_data);
								break;
							case 1://Delete act.dat
								ret = sceIoRemove("tm0:npdrm/act.dat");
								sprintf(con_data, "Borrando act.dat: %d reiniciando en 5s", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							case 2://Delete tai config UX0
								ret = sceIoRemove("ux0:tai/config.txt");
								sprintf(con_data, "Borrando configuracion ux0:tai %d reiniciando en 5s", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
					                case 3://Delete tai config UR0
								ret = sceIoRemove("ur0:tai/config.txt");
								sprintf(con_data, "Borrando configuracion ur0:tai %d reiniciando en 5s", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							case 4://Restore registry 
								ret = sceIoRemove("vd0:registry/system.ireg");
								sprintf(con_data, "Borrando iregistry: %d \n", ret);
								strcat(log_text,con_data);
								
								ret = sceIoRemove("vd0:registry/system.dreg");
								sprintf(con_data, "Borrando dregistry: %d reiniciando en 5s", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								
								break;
							
						}
						break;
					case 3:
						switch (sub_selected){
							case 0://Mount mem card
								ret = _vshIoMount(0x800, NULL, 0, 0, 0, 0);
								sprintf(con_data, "Montando MemCard: %d \n", ret);
								strcat(log_text,con_data);
								break;
							case 1://Unmount mem card
								ret = vshIoUmount(0x800, 0, 0, 0);
								sprintf(con_data, "Desmontando MemCard: %d \n", ret);
								strcat(log_text,con_data);
								break;
							
						}
						break;
					case 4:
						switch (sub_selected){
							case 0://Copy activation
								if (doesDirExist("ux0:/Backup_act")) {
                                                                sceIoRemove("ux0:/Backup_act/act.dat"); 
                                                                sceIoRemove("ux0:/Backup_act/system.dreg"); 
                                                                sceIoRemove("ux0:/Backup_act/system.ireg"); 
                                                                sceIoRemove("ux0:/Backup_act/myprofile.dat");
                                                                copyFile("tm0:/npdrm/act.dat" ,"ux0:/Backup_act/act.dat");
								copyFile("vd0:/registry/system.dreg" ,"ux0:/Backup_act/system.dreg");
								copyFile("vd0:/registry/system.ireg" ,"ux0:/Backup_act/system.ireg");
								copyFile("ur0:/user/00/np/myprofile.dat" ,"ux0:/Backup_act/myprofile.dat");
                                                                sprintf(con_data, "Copiando archivos de activacion: OK! XD  \n");
								strcat(log_text,con_data); } 
                                                                else 
                                                                { sceIoMkdir("ux0:/Backup_act" , 0777);
								copyFile("tm0:/npdrm/act.dat" ,"ux0:/Backup_act/act.dat");
								copyFile("vd0:/registry/system.dreg" ,"ux0:/Backup_act/system.dreg");
								copyFile("vd0:/registry/system.ireg" ,"ux0:/Backup_act/system.ireg");
								copyFile("ur0:/user/00/np/myprofile.dat" ,"ux0:/Backup_act/myprofile.dat");
                                                                sprintf(con_data, "Copiando archivos de activacion: OK! XD  \n");
								strcat(log_text,con_data); }
								break;
							case 1://Restore activation
                                                                ret = copyFile("ux0:/Backup_act/act.dat" ,"tm0:/npdrm/act.dat");
								copyFile("ux0:/Backup_act/system.dreg" ,"vd0:/registry/system.dreg");
								copyFile("ux0:/Backup_act/system.ireg" ,"vd0:/registry/system.ireg");
								copyFile("ux0:/Backup_act/myprofile.dat" ,"ur0:/user/00/np/myprofile.dat");
                                                                sprintf(con_data, "restaurando archivos de activacion: %d reiniciando en 5s", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							case 2://Copy tai config UR0
								if (doesDirExist("ur0:tai/backup")) {
                                                                sceIoRemove("ur0:tai/backup/config.txt");
                                                                copyFile("ur0:tai/config.txt" ,"ur0:tai/backup/config.txt");
								sprintf(con_data, "Copiando configuracion ur0:tai : Ok! XD  \n");
								strcat(log_text,con_data); } 
                                                                else 
                                                                { sceIoMkdir("ur0:tai/backup" , 0777);
                                                                copyFile("ur0:tai/config.txt" ,"ur0:tai/backup/config.txt");
								sprintf(con_data, "Copiando configuracion ur0:tai : Ok! XD  \n");
								strcat(log_text,con_data); }
								break;
                                                        case 3://Restore tai config UR0
								ret = copyFile("ur0:tai/backup/config.txt" ,"ur0:tai/config.txt");
								sprintf(con_data, "Restaurando configuracion ur0:tai %d reiniciando en 5s", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
                                                        case 4://Copy tai config UX0
								if (doesDirExist("ux0:tai/backup")) {
                                                                sceIoRemove("ux0:tai/backup/config.txt");
                                                                copyFile("ux0:tai/config.txt" ,"ux0:tai/backup/config.txt");
								sprintf(con_data, "Copiando configuracion ux0:tai : Ok! XD  \n");
								strcat(log_text,con_data); } 
                                                                else 
                                                                { sceIoMkdir("ux0:tai/backup" , 0777);
                                                                copyFile("ux0:tai/config.txt" ,"ux0:tai/backup/config.txt");
								sprintf(con_data, "Copiando configuracion ux0:tai : Ok! XD  \n");
								strcat(log_text,con_data); }
								break;
							case 5://Restore tai config UX0 
								ret = copyFile("ux0:tai/backup/config.txt" ,"ux0:tai/config.txt");
								sprintf(con_data, "Restaurando configuracion ux0:tai %d reiniciando en 5s", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							
						}
						break;
					case 5:
						switch (sub_selected){
							case 0://Molecular to Near
                                                                ret = mount(); {if (doesDirExist("ux0:/app/MLCL00001")) {printf("Cambiando NEAR y haciendo copia \n");{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoMkdir("vs0:/app/NPXS10000/MLCL" , 0777);
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/icon0.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/template.xml");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/bg.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/install_button.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/startup.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/template.xml");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/bg.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/install_button.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/livearea/startup.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/eboot.bin");
                                                                sceIoRemove("ux0:/backup_NEAR/NEAR/eboot.bin");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/icon0.png");
                                                                sceIoRemove("ux0:/backup_NEAR/NEAR/icon0.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/bg.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/install_button.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/startup.png");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/template.xml");
                                                                sceIoRemove("ux0:/backup_NEAR/NEAR/template.xml");
                                                                sceIoRemove("ux0:/backup_NEAR/MOLECULAR/param.sfo");
                                                                sceIoRemove("ux0:/backup_NEAR/NEAR/pic0.png");
                                                                sceIoRemove("ux0:/backup_NEAR/DB/app_bkp.db");
                                                                sceIoRemove("ux0:/backup_NEAR/DB/iconlayout_bkp.db");
                                                                sceIoRmdir("ux0:/backup_NEAR/MOLECULAR/livearea/ur0");
                                                                sceIoRmdir("ux0:/backup_NEAR/MOLECULAR/livearea");
                                                                sceIoRmdir("ux0:/backup_NEAR/MOLECULAR");
                                                                sceIoRmdir("ux0:/backup_NEAR/NEAR");
                                                                sceIoRmdir("ux0:/backup_NEAR/DB");
                                                                sceIoRmdir("ux0:/backup_NEAR");
                                                                sceIoMkdir("ux0:/backup_NEAR" , 0777);
                                                                sceIoMkdir("ux0:/backup_NEAR/MOLECULAR" , 0777);
                                                                sceIoMkdir("ux0:/backup_NEAR/NEAR" , 0777);
                                                                sceIoMkdir("ux0:/backup_NEAR/DB" , 0777);
                                                                sceIoMkdir("ux0:/backup_NEAR/MOLECULAR/livearea" , 0777);
                                                                sceIoMkdir("ux0:/backup_NEAR/MOLECULAR/livearea/ur0" , 0777);
                                                                            
                                                                copyFile("ur0:/appmeta/MLCL00001/icon0.png" ,"ux0:/backup_NEAR/MOLECULAR/livearea/ur0/icon0.png");
                                                                copyFile("ur0:/appmeta/MLCL00001/livearea/contents/template.xml" ,"ux0:/backup_NEAR/MOLECULAR/livearea/ur0/template.xml");
                                                                copyFile("ur0:/appmeta/MLCL00001/livearea/contents/bg.png" ,"ux0:/backup_NEAR/MOLECULAR/livearea/ur0/bg.png");
                                                                copyFile("ur0:/appmeta/MLCL00001/livearea/contents/install_button.png" ,"ux0:/backup_NEAR/MOLECULAR/livearea/ur0/install_button.png");
                                                                copyFile("ur0:/appmeta/MLCL00001/livearea/contents/startup.png" ,"ux0:/backup_NEAR/MOLECULAR/livearea/ur0/startup.png");
                                                                
								copyFile("ux0:/app/MLCL00001/eboot.bin" ,"ux0:/backup_NEAR/MOLECULAR/eboot.bin");
                                                                copyFile("vs0:/app/NPXS10000/eboot.bin" ,"ux0:/backup_NEAR/NEAR/eboot.bin");
                                                                copyFile("ux0:/app/MLCL00001/sce_sys/icon0.png" ,"ux0:/backup_NEAR/MOLECULAR/icon0.png");
                                                                copyFile("ux0:/app/MLCL00001/sce_sys/param.sfo" ,"ux0:/backup_NEAR/MOLECULAR/param.sfo");
                                                                copyFile("vs0:/app/NPXS10000/sce_sys/icon0.png" ,"ux0:/backup_NEAR/NEAR/icon0.png");
                                                                copyFile("ux0:/app/MLCL00001/sce_sys/livearea/contents/bg.png" ,"ux0:/backup_NEAR/MOLECULAR/bg.png");
                                                                copyFile("ux0:/app/MLCL00001/sce_sys/livearea/contents/install_button.png" ,"ux0:/backup_NEAR/MOLECULAR/install_button.png");
                                                                copyFile("ux0:/app/MLCL00001/sce_sys/livearea/contents/startup.png" ,"ux0:/backup_NEAR/MOLECULAR/startup.png");
                                                                copyFile("ux0:/app/MLCL00001/sce_sys/livearea/contents/template.xml" ,"ux0:/backup_NEAR/MOLECULAR/template.xml");
                                                                copyFile("vs0:/app/NPXS10000/sce_sys/livearea/contents/template.xml" ,"ux0:/backup_NEAR/NEAR/template.xml");
                                                                copyFile("vs0:/app/NPXS10000/sce_sys/pic0.png" ,"ux0:/backup_NEAR/NEAR/pic0.png");

                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/eboot.bin" ,"vs0:/app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/icon0.png" ,"vs0:/app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/bg.png" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/bg.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/install_button.png" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/install_button.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/startup.png" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/startup.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/template.xml" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                
                                                                
                                                                copyFile("ux0:/appmeta/MLCL00001/livearea/contents/template.xml", "ux0:/backup_NEAR/MOLECULAR/livearea/template.xml");
                                                                copyFile("ux0:/appmeta/MLCL00001/livearea/contents/bg.png", "ux0:/backup_NEAR/MOLECULAR/livearea/bg.png");
                                                                copyFile("ux0:/appmeta/MLCL00001/livearea/contents/install_button.png", "ux0:/backup_NEAR/MOLECULAR/livearea/install_button.png");
                                                                copyFile("ux0:/appmeta/MLCL00001/livearea/contents/startup.png", "ux0:/backup_NEAR/MOLECULAR/livearea/startup.png");
                                                                
                                                                sceIoRemove("vs0:/app/NPXS10000/sce_sys/pic0.png");
	                                                        copyFile("ur0:shell/db/app.db", "ux0:/backup_NEAR/DB/app_bkp.db");
	                                                        sceIoRemove("ur0:shell/db/app.db");
                                                                copyFile("ux0:iconlayout.ini", "ux0:/backup_NEAR/DB/iconlayout_bkp.db");
                                                                sceIoRemove("ux0:iconlayout.ini");
                                                                sceIoRename("ux0:/app/MLCL00001" , "ux0:/app/BLCL00001");
                                                                sceIoRemove("ux0:/app/BLCL00001/eboot.bin");
                                                                sceIoRemove("ux0:/app/BLCL00001/sce_sys/icon0.png");
                                                                sceIoRemove("ux0:/app/BLCL00001/sce_sys/param.sfo");
                                                                sceIoRemove("ux0:/app/BLCL00001/sce_sys/livearea/contents/template.xml");
                                                                sceIoRemove("ux0:/app/BLCL00001/sce_sys/livearea/contents/bg.png");
                                                                sceIoRemove("ux0:/app/BLCL00001/sce_sys/livearea/contents/install_button.png");
                                                                sceIoRemove("ux0:/app/BLCL00001/sce_sys/livearea/contents/startup.png");
                                                                sceIoRmdir("ux0:/app/BLCL00001/sce_sys/livearea/contents");
                                                                sceIoRmdir("ux0:/app/BLCL00001/sce_sys/livearea");
                                                                sceIoRemove("ur0:/appmeta/MLCL00001/icon0.png");
                                                                sceIoRemove("ur0:/appmeta/MLCL00001/livearea/contents/template.xml");
                                                                sceIoRemove("ur0:/appmeta/MLCL00001/livearea/contents/bg.png");
                                                                sceIoRemove("ur0:/appmeta/MLCL00001/livearea/contents/install_button.png");
                                                                sceIoRemove("ur0:/appmeta/MLCL00001/livearea/contents/startup.png");
                                                                sceIoRmdir("ur0:/appmeta/MLCL00001/livearea/contents");
                                                                sceIoRmdir("ur0:/appmeta/MLCL00001/livearea");
                                                                sceIoRmdir("ur0:/appmeta/MLCL00001");
                                                                sceIoRemove("ux0:/appmeta/MLCL00001/livearea/contents/template.xml");
                                                                sceIoRemove("ux0:/appmeta/MLCL00001/livearea/contents/bg.png");
                                                                sceIoRemove("ux0:/appmeta/MLCL00001/livearea/contents/install_button.png");
                                                                sceIoRemove("ux0:/appmeta/MLCL00001/livearea/contents/startup.png");
                                                                sceIoRemove("ux0:/appmeta/MLCL00001/sce_pfs/files.db");
                                                                sceIoRemove("ux0:/appmeta/MLCL00001/sce_pfs/icv.db/*.icv");
                                                                sceIoRmdir("ux0:/appmeta/MLCL00001/sce_pfs/icv.db");
                                                                sceIoRmdir("ux0:/appmeta/MLCL00001/sce_pfs");
                                                                sceIoRmdir("ux0:/appmeta/MLCL00001");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nHecho!! Reiniciando en 5s..." , ret);
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
                                                                else if (doesDirExist("vs0:/app/NPXS10000/MLCL")) {sprintf(con_data, "Ya tienes convertido MOLECUAR a NEAR!!\n");
								strcat(log_text,con_data); }
                                                                else
                                                                {sprintf(con_data, "No tienes molecular shell instalado... :(  \n");
								strcat(log_text,con_data); }
	                                                        }
                                                                break;

                                                        case 1://Near restore
                                                                ret = mount(); {if (doesFileExist("vs0:/app/NPXS10000/sce_sys/pic0.png")) {
                                                                sprintf(con_data, "Ya tienes NEAR original!!...\n");
								strcat(log_text,con_data); }
                                                                else if (doesDirExist("ux0:/backup_NEAR/NEAR")) {printf("Restaurando NEAR \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoRmdir("vs0:/app/NPXS10000/MLCL");
                                                                sceIoRename("ux0:/app/BLCL00001" , "ux0:/app/MLCL00001");
                                                                sceIoMkdir("ux0:/app/MLCL00001/sce_sys/livearea", 0777);
                                                                sceIoMkdir("ux0:/app/MLCL00001/sce_sys/livearea/contents", 0777);
                                                                sceIoMkdir("ur0:/appmeta/MLCL00001", 0777);
                                                                sceIoMkdir("ur0:/appmeta/MLCL00001/livearea", 0777);
                                                                sceIoMkdir("ur0:/appmeta/MLCL00001/livearea/contents", 0777);


                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/icon0.png" ,"ur0:/appmeta/MLCL00001/icon0.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/template.xml" ,"ur0:/appmeta/MLCL00001/livearea/contents/template.xml");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/bg.png" ,"ur0:/appmeta/MLCL00001/livearea/contents/bg.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/install_button.png" ,"ur0:/appmeta/MLCL00001/livearea/contents/install_button.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/ur0/startup.png" ,"ur0:/appmeta/MLCL00001/livearea/contents/startup.png");
                                                                
								copyFile("ux0:/backup_NEAR/MOLECULAR/eboot.bin","ux0:/app/MLCL00001/eboot.bin");
                                                                copyFile("ux0:/backup_NEAR/NEAR/eboot.bin" ,"vs0:/app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/icon0.png" ,"ux0:/app/MLCL00001/sce_sys/icon0.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/param.sfo" ,"ux0:/app/MLCL00001/sce_sys/param.sfo");
                                                                copyFile("ux0:/backup_NEAR/NEAR/icon0.png" ,"vs0:/app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/bg.png" ,"ux0:/app/MLCL00001/sce_sys/livearea/contents/bg.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/install_button.png" ,"ux0:/app/MLCL00001/sce_sys/livearea/contents/install_button.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/startup.png" ,"ux0:/app/MLCL00001/sce_sys/livearea/contents/startup.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/template.xml" ,"ux0:/app/MLCL00001/sce_sys/livearea/contents/template.xml");
                                                                copyFile("ux0:/backup_NEAR/NEAR/template.xml" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                copyFile("ux0:/backup_NEAR/NEAR/pic0.png" ,"vs0:/app/NPXS10000/sce_sys/pic0.png");   
							        copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/template.xml" ,"ux0:/appmeta/MLCL00001/livearea/contents/template.xml");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/bg.png" ,"ux0:/appmeta/MLCL00001/livearea/contents/bg.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/install_button.png" ,"ux0:/appmeta/MLCL00001/livearea/contents/install_button.png");
                                                                copyFile("ux0:/backup_NEAR/MOLECULAR/livearea/startup.png" ,"ux0:/appmeta/MLCL00001/livearea/contents/startup.png");

	                                                        copyFile("ux0:/backup_NEAR/DB/app_bkp.db" ,"ur0:shell/db/app.db");
                                                                copyFile("ux0:/backup_NEAR/DB/iconlayout_bkp.db" , "ux0:iconlayout.ini");
                                                                sceIoRename("ur0:/appmeta/BLCL00001" ,"ur0:/appmeta/MLCL00001");
                                                                sceIoRename("ux0:/appmeta/BLCL00001" ,"ux0:/appmeta/MLCL00001");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nRestaurado!! Reiniciando en 5s..." , ret);
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
                                                                else
                                                                {sprintf(con_data, "No tienes copia de NEAR... :(  \n");
								strcat(log_text,con_data); }
	                                                        }
                                                                break;

                                                        
							case 2://Molecular x Vitashell
							        if (doesDirExist("vs0:/app/NPXS10000/MLCL")) {
							        printf("cambiando MOLECULAR por VITASHELL!! XD \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
								vshIoUmount(i * 0x100, 0, 0, 0);
								printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100));
								}}
								strcpy(log_text,"");
								copyFile("ux0:/app/VITASHELL/eboot.bin" ,"vs0:/app/NPXS10000/eboot.bin");
								printf("\n\nHecho!!! Reiniciando en 5s \n"); 
								sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();
								strcat(log_text,con_data); }
                                                                else
                                                                {sprintf(con_data, "Has primero la copia de Molecular a NEAR... :(  \n");
								strcat(log_text,con_data);
	                                                        }
                                                                break;
							
							case 3://SYS INFO
								sceRegMgrGetKeyStr("/CONFIG/TEL", "sim_unique_id", con_data, 6 * 16);//IMEI
								strcat(log_text,"IMEI: ");
								strcat(log_text,con_data);
								strcat(log_text," \n\n");
								
								char CID[16];
								_vshSblAimgrGetConsoleId(CID);//CID
								
								strcat(log_text,"CID: ");
								
								for (i = 0; i < 16; i++) {
										sprintf(con_data,"%02X", CID[i]);
										strcat(log_text,con_data);
								}
								
								
								strcat(log_text," \n");
								break;
								
							case 4://Uninstall RECOVERY
							        if (doesFileExist("ur0:tai/boot_config.bkp")) {
							        printf("Desinstalando RECOVERY.");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }}
								strcpy(log_text,"");
								copyFile("ur0:tai/boot_config.bkp" ,"ur0:tai/boot_config.txt");
								sceIoRemove("ur0:tai/boot_config.bkp");	
								sceIoRemove("ur0:tai/homerecovery.self");
								scePowerRequestColdReset();}
								else
								{printf("Instala HomeRecovery con la VPK para desinstalar");}
								break;
								
							case 5:
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
