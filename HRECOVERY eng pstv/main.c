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

char menu_items[][60] = {" 	 Continue - Normal Boot"," 	 Boot - boot in different modes"," 	 Fixes - solve start-up problems"," 	 Mount - Mount partition points"," 	 Backup - Backups config files"," 	 Extras - Molecular, Vitashell, ..."};

char menu_options [][8][28] = {  {"exit"} , {"Reboot","IDU ON","IDU OFF (DEMO MODE)"} , {"delete id.dat","delete act.dat","delete ux0:tai/config.txt","delete ur0:tai/config.txt","Erase registry","delete database"} , {"Mount MemCard","unmount MemCard"} , {"Copy activation","Restore activation","Copy ur0 tai","Restore ur0 tai","Copy ux0 tai","Restore ux0 tai","Copy database","Restore database"} , {"Install VITASHELL","Molecular to NEAR", "Molecular (mode2) to NEAR","Restore NEAR","Inject VITASHELL x NEARMOD","System info","Uninstall RECOVERY","clear LOG"}  };

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
int doesFileExist();
int doesDirExist();
int copyFile();
int getFileSize();
int createEmptyFile();
int makePath();
int removePath();

char log_text[800];

void select_menu(){
	psvDebugScreenClear(0x00000000);
	psvDebugScreenSetFgColor(COLOR_YELLOW);
	psvDebugScreenPrintf("Ninoh-FOX            --[Menu Recovery]--                         \n");
	psvDebugScreenPrintf("                     --[HaiMenu v1.00]--            EOL.net      \n");
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
	psvDebugScreenClear(0x0F8F0F0F);
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
								sprintf(con_data, "continuing boot... ");
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
								sprintf(con_data, "Reboot: %d ", ret);
								strcat(log_text,con_data);
								break;
							case 1://IDU Enable
								ret = vshSysconIduModeSet();
								sprintf(con_data, "IDU activated: %d \n", ret);
								strcat(log_text,con_data);
								break;
							case 2://IDU Disable
								ret = vshSysconIduModeClear();
								sprintf(con_data, "IDU deactivate (DEMO MODE): %d \n", ret);
								strcat(log_text,con_data);
								break;
							
						}
						break;
					case 2: //deletes
						switch (sub_selected){
							case 0://Delete id.dat
								ret = sceIoRemove("ux0:/id.dat");
								sprintf(con_data, "Erased id.dat: %d \n", ret);
								strcat(log_text,con_data);
								break;
							case 1://Delete act.dat
								ret = sceIoRemove("tm0:npdrm/act.dat");
								sprintf(con_data, "Erased act.dat: rebooting in 5s %d", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							case 2://Delete tai config UX0
								ret = sceIoRemove("ux0:tai/config.txt");
								sprintf(con_data, "Erased ux0:tai/config.txt rebooting in 5s %d", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
					                case 3://Delete tai config UR0
								ret = sceIoRemove("ur0:tai/config.txt");
								sprintf(con_data, "Erased ur0:tai/config.txt rebooting in 5s %d", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							case 4://Restore registry 
								ret = sceIoRemove("vd0:registry/system.ireg");
								sprintf(con_data, "Erased iregistry: %d \n", ret);
								strcat(log_text,con_data);
								
								ret = sceIoRemove("vd0:registry/system.dreg");
								sprintf(con_data, "Erased dregistry: %d rebooting in 5s", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								
								break;
							case 5://Erase DATA BASE
							        ret = sceIoRemove("ur0:shell/db/app.db");
								sprintf(con_data, "Erased database. rebooting in 5s %d", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							
						}
						break;
					case 3: //mounts
						switch (sub_selected){
							case 0://Mount mem card
								ret = _vshIoMount(0x800, NULL, 0, 0, 0, 0);
								sprintf(con_data, "Mount MemCard: %d \n", ret);
								strcat(log_text,con_data);
								break;
							case 1://Unmount mem card
								ret = vshIoUmount(0x800, 0, 0, 0);
								sprintf(con_data, "Unmount MemCard: %d \n", ret);
								strcat(log_text,con_data);
								break;
							
						}
						break;
					case 4: //backups
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
                                                                sprintf(con_data, "Copying activation files: OK! XD  \n");
								strcat(log_text,con_data); } 
                                                                else 
                                                                { sceIoMkdir("ux0:/Backup_act" , 0777);
								copyFile("tm0:/npdrm/act.dat" ,"ux0:/Backup_act/act.dat");
								copyFile("vd0:/registry/system.dreg" ,"ux0:/Backup_act/system.dreg");
								copyFile("vd0:/registry/system.ireg" ,"ux0:/Backup_act/system.ireg");
								copyFile("ur0:/user/00/np/myprofile.dat" ,"ux0:/Backup_act/myprofile.dat");
                                                                sprintf(con_data, "Copying activation files: OK! XD  \n");
								strcat(log_text,con_data); }
								break;
							case 1://Restore activation
                                                                ret = copyFile("ux0:/Backup_act/act.dat" ,"tm0:/npdrm/act.dat");
								copyFile("ux0:/Backup_act/system.dreg" ,"vd0:/registry/system.dreg");
								copyFile("ux0:/Backup_act/system.ireg" ,"vd0:/registry/system.ireg");
								copyFile("ux0:/Backup_act/myprofile.dat" ,"ur0:/user/00/np/myprofile.dat");
                                                                sprintf(con_data, "restoring activation files: Ok! rebooting in 5s %d", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							case 2://Copy tai config UR0
								if (doesDirExist("ur0:tai/backup")) {
                                                                sceIoRemove("ur0:tai/backup/config.txt");
                                                                copyFile("ur0:tai/config.txt" ,"ur0:tai/backup/config.txt");
								sprintf(con_data, "Copying configuration ur0:tai : Ok! XD  \n");
								strcat(log_text,con_data); } 
                                                                else 
                                                                { sceIoMkdir("ur0:tai/backup" , 0777);
                                                                copyFile("ur0:tai/config.txt" ,"ur0:tai/backup/config.txt");
								sprintf(con_data, "Copying configuration ur0:tai : Ok! XD  \n");
								strcat(log_text,con_data); }
								break;
                                                        case 3://Restore tai config UR0
								ret = copyFile("ur0:tai/backup/config.txt" ,"ur0:tai/config.txt");
								sprintf(con_data, "restoring configuration ur0:tai Ok! rebooting in 5s %d", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
                                                        case 4://Copy tai config UX0
								if (doesDirExist("ux0:tai/backup")) {
                                                                sceIoRemove("ux0:tai/backup/config.txt");
                                                                copyFile("ux0:tai/config.txt" ,"ux0:tai/backup/config.txt");
								sprintf(con_data, "Copying configuration ux0:tai : Ok! XD  \n");
								strcat(log_text,con_data); } 
                                                                else 
                                                                { sceIoMkdir("ux0:tai/backup" , 0777);
                                                                copyFile("ux0:tai/config.txt" ,"ux0:tai/backup/config.txt");
								sprintf(con_data, "Copying configuration ux0:tai : Ok! XD  \n");
								strcat(log_text,con_data); }
								break;
							case 5://Restore tai config UX0 
								ret = copyFile("ux0:tai/backup/config.txt" ,"ux0:tai/config.txt");
								sprintf(con_data, "Restoring configuration ux0:tai : Ok! rebooting in 5s %d", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
							case 6://Copy DATA BASE
								ret = copyFile("ur0:shell/db/app.db" ,"ur0:shell/db/app.bkp");
								copyFile("ux0:iconlayout.ini" ,"ux0:iconlayout.bkp");
								sprintf(con_data, "Copying database: Ok! XD %d\n", ret);
								strcat(log_text,con_data);
								break;
							case 7://Restore DATA BASE 
								ret = copyFile("ur0:shell/db/app.bkp" ,"ur0:shell/db/app.db");
								copyFile("ux0:iconlayout.bkp" ,"ux0:iconlayout.ini");
								sprintf(con_data, "Restoring database: Ok! rebooting in 5s %d", ret);
								strcat(log_text,con_data);
                                                                select_menu();
								sceKernelDelayThread(5 * 1000 * 1000);
                                                                scePowerRequestColdReset();
								break;
						}
						break;
					case 5: //Vitashel Molecular
						switch (sub_selected){
							case 0://Install VITASHELL
                                                                printf("Installing VitaShell...\n");
								sprintf(con_data, "Installing VitaShell...\n");
								sceIoMkdir("ux0:app/VITASHELL" , 0777);
								sceIoMkdir("ux0:app/VITASHELL/sce_sys" , 0777);
								sceIoMkdir("ux0:app/VITASHELL/sce_sys/livearea" , 0777);
								sceIoMkdir("ux0:app/VITASHELL/sce_sys/package" , 0777);
								sceIoMkdir("ux0:app/VITASHELL/sce_sys/livearea/contents" , 0777);
								sceIoMkdir("ux0:appmeta/VITASHELL" , 0777);
								sceIoMkdir("ux0:appmeta/VITASHELL/sce_sys" , 0777);
								sceIoMkdir("ux0:appmeta/VITASHELL/livearea" , 0777);
								sceIoMkdir("ux0:appmeta/VITASHELL/sce_pfs" , 0777);
								sceIoMkdir("ux0:appmeta/VITASHELL/sce_pfs/icv.db" , 0777);
								sceIoMkdir("ux0:appmeta/VITASHELL/livearea/contents" , 0777);
								
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/eboot.bin" ,"ux0:/app/VITASHELL/eboot.bin");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/param.sfo", "ux0:app/VITASHELL/sce_sys/param.sfo");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/icon0.png", "ux0:app/VITASHELL/sce_sys/icon0.png");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/package/head.bin", "ux0:app/VITASHELL/sce_sys/package/head.bin");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/package/work.bin", "ux0:app/VITASHELL/sce_sys/package/work.bin");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/livearea/contents/bg.png", "ux0:app/VITASHELL/sce_sys/livearea/contents/bg.png");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/livearea/contents/startup.png", "ux0:app/VITASHELL/sce_sys/livearea/contents/starup.png");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/livearea/contents/template.xml", "ux0:app/VITASHELL/sce_sys/livearea/contents/template.xml");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/icon0.png", "ux0:appmeta/VITASHELL/icon0.png");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/ebootparam.bin", "ux0:appmeta/VITASHELL/ebootparam.bin");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/param.sfo", "ux0:appmeta/VITASHELL/param.sfo");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/icon0.png", "ux0:appmeta/VITASHELL/icon0.png");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/livearea/contents/bg.png", "ux0:appmeta/VITASHELL/livearea/contents/bg.png");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/livearea/contents/startup.png", "ux0:appmeta/VITASHELL/livearea/contents/startup.png");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/livearea/contents/template.xml", "ux0:appmeta/VITASHELL/livearea/contents/template.xml");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_sys/sealedkey", "ux0:appmeta/VITASHELL/sce_sys/sealedkey");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/files.db", "ux0:appmeta/VITASHELL/sce_pfs/files.db");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/4a7d8188.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/4a7d8188.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/70a147ff.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/70a147ff.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/125c92f0.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/125c92f0.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/455a7a2a.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/455a7a2a.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/583d5815.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/583d5815.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/977df42f.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/977df42f.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/b90dec2b.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/b90dec2b.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/cd81206f.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/cd81206f.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/f60ff9f9.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/f60ff9f9.icv");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APPMETA/sce_pfs/icv.db/f306a512.icv", "ux0:appmeta/VITASHELL/sce_pfs/icv.db/f306a512.icv");
						                
								sceIoRemove("ur0:shell/db/app.db");
								printf("\n\nNice!! Rebooting in 5s...");
								sprintf(con_data, "\n\nNice!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();
							        break;

							case 1://Molecular to Near
                                                                if (doesDirExist("ux0:/app/MLCL00001")) {printf("Changing NEAR and making a backup \n");{for (i = 0; i < 15; i++) {
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
                                                                sceIoRemove("ux0:/backup_NEAR/DB/iconlayout_bkp.ini");
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
                                                                copyFile("ux0:iconlayout.ini", "ux0:/backup_NEAR/DB/iconlayout_bkp.ini");
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
	                                                        printf("\n\nNice!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
                                                                else if (doesDirExist("vs0:/app/NPXS10000/MLCL")) {sprintf(con_data, "You have already converted MOLECUAR to NEAR!!\n");
								strcat(log_text,con_data); }
								else if (doesDirExist("vs0:/app/NPXS10000/MLCL2")) {sprintf(con_data, "You already have MOLECUAR installed in NEAR!!\n");
								strcat(log_text,con_data); }
                                                                else
                                                                {sprintf(con_data, "You do not have molecular shell installed ... :( \nBut do not worry, use mode 2.;)\n");
								strcat(log_text,con_data); }
                                                                break;

                                                        case 2://Molecular to Near mode 2 without molecular install
                                                                if (doesDirExist("vs0:/app/NPXS10000/MLCL2")) {sprintf(con_data, "You already have MOLECUAR installed in NEAR!!\n");
								strcat(log_text,con_data); }
								else if (doesDirExist("vs0:/app/NPXS10000/MLCL")) {sprintf(con_data, "You have already converted MOLECUAR to NEAR!!\n");
								strcat(log_text,con_data); }
                                                                else if (doesFileExist("vs0:/app/NPXS10000/sce_sys/pic0.png")) {printf("Installing Molecular in NEAR and making a backup\n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoMkdir("vs0:/app/NPXS10000/MLCL2" , 0777);
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
                                                                sceIoRemove("ux0:/backup_NEAR/DB/iconlayout_bkp.ini");
                                                                sceIoRmdir("ux0:/backup_NEAR/MOLECULAR/livearea/ur0");
                                                                sceIoRmdir("ux0:/backup_NEAR/MOLECULAR/livearea");
                                                                sceIoRmdir("ux0:/backup_NEAR/MOLECULAR");
                                                                sceIoRmdir("ux0:/backup_NEAR/NEAR");
                                                                sceIoRmdir("ux0:/backup_NEAR/DB");
                                                                sceIoRmdir("ux0:/backup_NEAR");
                                                                sceIoMkdir("ux0:/backup_NEAR" , 0777);
                                                                sceIoMkdir("ux0:/backup_NEAR/NEAR2" , 0777);
                                                                sceIoMkdir("ux0:/backup_NEAR/DB" , 0777);
                                                                 
								copyFile("vs0:/app/NPXS10000/eboot.bin" ,"ux0:/backup_NEAR/NEAR2/eboot.bin");
                                                                copyFile("vs0:/app/NPXS10000/sce_sys/icon0.png" ,"ux0:/backup_NEAR/NEAR2/icon0.png");
                                                                copyFile("vs0:/app/NPXS10000/sce_sys/livearea/contents/template.xml" ,"ux0:/backup_NEAR/NEAR2/template.xml");
                                                                copyFile("vs0:/app/NPXS10000/sce_sys/pic0.png" ,"ux0:/backup_NEAR/NEAR2/pic0.png");

                                                                copyFile("ux0:/app/HMRC00001/data/MOLECULAR/eboot.bin" ,"vs0:/app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:/app/HMRC00001/data/MOLECULAR/icon0.png" ,"vs0:/app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:/app/HMRC00001/data/MOLECULAR/bg.png" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/bg.png");
                                                                copyFile("ux0:/app/HMRC00001/data/MOLECULAR/install_button.png" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/install_button.png");
                                                                copyFile("ux0:/app/HMRC00001/data/MOLECULAR/startup.png" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/startup.png");
                                                                copyFile("ux0:/app/HMRC00001/data/MOLECULAR/template.xml" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                
								sceIoRemove("vs0:/app/NPXS10000/sce_sys/pic0.png");
                                                                copyFile("ur0:shell/db/app.db", "ux0:/backup_NEAR/DB/app_bkp.db");
	                                                        sceIoRemove("ur0:shell/db/app.db");
                                                                copyFile("ux0:iconlayout.ini", "ux0:/backup_NEAR/DB/iconlayout_bkp.ini");
                                                                sceIoRemove("ux0:iconlayout.ini");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nNice!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset(); }
								else {sprintf(con_data, "Nothing to do here...\n");
								strcat(log_text,con_data); }
                                                                break;
                                                         
							 case 3://Near restore
                                                                if (doesFileExist("vs0:/app/NPXS10000/sce_sys/pic0.png")) {
                                                                sprintf(con_data, "You already have original NEAR!!...\n");
								strcat(log_text,con_data); }
								else if (doesDirExist("ux0:/backup_NEAR/NEAR2") && doesDirExist("vs0:/app/NPXS10000/MLCL2")) {printf("Restoring NEAR \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoRmdir("vs0:/app/NPXS10000/MLCL2");
                                                                
                                                                copyFile("ux0:/backup_NEAR/NEAR/eboot.bin" ,"vs0:/app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:/backup_NEAR/NEAR/icon0.png" ,"vs0:/app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:/backup_NEAR/NEAR/template.xml" ,"vs0:/app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                copyFile("ux0:/backup_NEAR/NEAR/pic0.png" ,"vs0:/app/NPXS10000/sce_sys/pic0.png");
                                                                
	                                                        copyFile("ux0:/backup_NEAR/DB/app_bkp.db" ,"ur0:shell/db/app.db");
                                                                copyFile("ux0:/backup_NEAR/DB/iconlayout_bkp.ini" , "ux0:iconlayout.ini");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nRestored!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
                                                                else if (doesDirExist("ux0:/backup_NEAR/NEAR") && doesDirExist("vs0:/app/NPXS10000/MLCL")) {printf("Restoring NEAR \n");
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
                                                                copyFile("ux0:/backup_NEAR/DB/iconlayout_bkp.ini" , "ux0:iconlayout.ini");
                                                                sceIoRename("ur0:/appmeta/BLCL00001" ,"ur0:/appmeta/MLCL00001");
                                                                sceIoRename("ux0:/appmeta/BLCL00001" ,"ux0:/appmeta/MLCL00001");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nRestored!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
                                                                else
                                                                {sprintf(con_data, "You do not have a backup of NEAR... :(  \n");
								strcat(log_text,con_data); }
                                                                break;

                                                        
							case 4://Molecular x Vitashell
							        if (doesDirExist("vs0:/app/NPXS10000/MLCL")) {
							        printf("changing MOLECULAR for VITASHELL!! XD \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
								vshIoUmount(i * 0x100, 0, 0, 0);
								printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100));
								}}
								strcpy(log_text,"");
								copyFile("ux0:/app/VITASHELL/eboot.bin" ,"vs0:/app/NPXS10000/eboot.bin");
								printf("\n\nNice!!! Rebooting in 5s \n"); 
								sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();
								strcat(log_text,con_data); }
								else if (doesDirExist("vs0:/app/NPXS10000/MLCL2")) {
							        printf("changing MOLECULAR for VITASHELL!! XD \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
								vshIoUmount(i * 0x100, 0, 0, 0);
								printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100));
								}}
								strcpy(log_text,"");
								copyFile("ux0:/app/VITASHELL/eboot.bin" ,"vs0:/app/NPXS10000/eboot.bin");
								printf("\n\nNice!!! Rebooting in 5s \n"); 
								sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();
								strcat(log_text,con_data); }
                                                                else
                                                                {sprintf(con_data, "First, copy Molecular to NEAR ... :(  \n");
								strcat(log_text,con_data);
	                                                        }
                                                                break;
							
							case 5://SYS INFO
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
								
							case 6://Uninstall RECOVERY
							        if (doesFileExist("vs0:tai/boot_config.bkp")) {
							        printf("Uninstalled RECOVERY.");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }}
								strcpy(log_text,"");
								copyFile("vs0:tai/boot_config.bkp" ,"vs0:tai/boot_config.txt");
								sceIoRemove("vs0:tai/boot_config.bkp");	
								sceIoRemove("vs0:tai/homerecovery.self");
								scePowerRequestColdReset();}
								else if (doesFileExist("ur0:tai/boot_config.bkp")) {
							        printf("Uninstalled recovery RECOVERY.");
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
								{printf("Install HomeRecovery with the VPK to uninstall\n");}
								break;
								
							case 7:
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
				if(strlen(menu_options[selected][sub_selected+1]) > 2 && sub_selected+1 < 8){
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
