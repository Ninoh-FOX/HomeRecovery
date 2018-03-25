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

char menu_options [][8][28] = {  {"exit"} , {"Reboot","IDU ON","IDU OFF (DEMO MODE)"} , {"delete id.dat","delete act.dat","delete ux0:tai/config.txt","delete ur0:tai/config.txt","Erase registry","delete database"} , {"Mount MemCard","unmount MemCard"} , {"Copy activation","Restore activation","Copy ur0 tai","Restore ur0 tai","Copy ux0 tai","Restore ux0 tai","Copy database","Restore database"} , {"Install VITASHELL","Install Molecular", "Molecular to NEAR","VITASHELL to NEAR","Restore NEAR","System info","Uninstall RECOVERY","clear LOG"}  };

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
	psvDebugScreenPrintf("                     --[HaiMenu v1.03]--            EOL.net      \n");
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
								ret = sceIoRemove("ux0:id.dat");
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
								if (doesDirExist("ux0:Backup_act")) {
                                                                sceIoRemove("ux0:Backup_act/act.dat"); 
                                                                sceIoRemove("ux0:Backup_act/system.dreg"); 
                                                                sceIoRemove("ux0:Backup_act/system.ireg"); 
                                                                sceIoRemove("ux0:Backup_act/myprofile.dat");
                                                                copyFile("tm0:/npdrm/act.dat" ,"ux0:Backup_act/act.dat");
								copyFile("vd0:/registry/system.dreg" ,"ux0:Backup_act/system.dreg");
								copyFile("vd0:/registry/system.ireg" ,"ux0:Backup_act/system.ireg");
								copyFile("ur0:user/00/np/myprofile.dat" ,"ux0:Backup_act/myprofile.dat");
                                                                sprintf(con_data, "Copying activation files: OK! XD  \n");
								strcat(log_text,con_data); } 
                                                                else 
                                                                { sceIoMkdir("ux0:Backup_act" , 0777);
								copyFile("tm0:/npdrm/act.dat" ,"ux0:Backup_act/act.dat");
								copyFile("vd0:/registry/system.dreg" ,"ux0:Backup_act/system.dreg");
								copyFile("vd0:/registry/system.ireg" ,"ux0:Backup_act/system.ireg");
								copyFile("ur0:user/00/np/myprofile.dat" ,"ux0:Backup_act/myprofile.dat");
                                                                sprintf(con_data, "Copying activation files: OK! XD  \n");
								strcat(log_text,con_data); }
								break;
							case 1://Restore activation
                                                                ret = copyFile("ux0:Backup_act/act.dat" ,"tm0:/npdrm/act.dat");
								copyFile("ux0:Backup_act/system.dreg" ,"vd0:/registry/system.dreg");
								copyFile("ux0:Backup_act/system.ireg" ,"vd0:/registry/system.ireg");
								copyFile("ux0:Backup_act/myprofile.dat" ,"ur0:user/00/np/myprofile.dat");
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
								copyFile("ux0:id.dat" ,"ux0:id.bkp");
								sprintf(con_data, "Copying database: Ok! XD %d\n", ret);
								strcat(log_text,con_data);
								break;
							case 7://Restore DATA BASE 
								ret = copyFile("ur0:shell/db/app.bkp" ,"ur0:shell/db/app.db");
								copyFile("ux0:iconlayout.bkp" ,"ux0:iconlayout.ini");
								copyFile("ux0:id.bkp" ,"ux0:id.dat");
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
								
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/eboot.bin" ,"ux0:app/VITASHELL/eboot.bin");
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
						                
								sceIoRemove("ux0:id.dat");
								printf("\n\nNice!! Rebooting in 5s...");
								sprintf(con_data, "\n\nNice!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();
							        break;

							case 1://Install Molecular
                                                                printf("Installing Molecular...\n");
								sprintf(con_data, "Installing Molecular...\n");
								sceIoMkdir("ux0:app/MLCL00001" , 0777);
								sceIoMkdir("ux0:app/MLCL00001/sce_sys" , 0777);
								sceIoMkdir("ux0:app/MLCL00001/sce_sys/livearea" , 0777);
								sceIoMkdir("ux0:app/MLCL00001/sce_sys/package" , 0777);
								sceIoMkdir("ux0:app/MLCL00001/sce_sys/livearea/contents" , 0777);
								sceIoMkdir("ux0:appmeta/MLCL00001" , 0777);
								sceIoMkdir("ux0:appmeta/MLCL00001/sce_sys" , 0777);
								sceIoMkdir("ux0:appmeta/MLCL00001/livearea" , 0777);
								sceIoMkdir("ux0:appmeta/MLCL00001/sce_pfs" , 0777);
								sceIoMkdir("ux0:appmeta/MLCL00001/sce_pfs/icv.db" , 0777);
								sceIoMkdir("ux0:appmeta/MLCL00001/livearea/contents" , 0777);
								sceIoMkdir("ux0:temp/app_work/MLCL00001" , 0777);
								sceIoMkdir("ux0:temp/app_work/MLCL00001/rec" , 0777);

								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/eboot.bin" ,"ux0:app/MLCL00001/eboot.bin");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/param.sfo", "ux0:app/MLCL00001/sce_sys/param.sfo");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/icon0.png", "ux0:app/MLCL00001/sce_sys/icon0.png");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/package/head.bin", "ux0:app/MLCL00001/sce_sys/package/head.bin");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/package/work.bin", "ux0:app/MLCL00001/sce_sys/package/work.bin");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/livearea/contents/bg.png", "ux0:app/MLCL00001/sce_sys/livearea/contents/bg.png");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/livearea/contents/startup.png", "ux0:app/MLCL00001/sce_sys/livearea/contents/starup.png");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/livearea/contents/template.xml", "ux0:app/MLCL00001/sce_sys/livearea/contents/template.xml");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/livearea/contents/install_button.png", "ux0:app/MLCL00001/sce_sys/livearea/contents/install_button.png");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/icon0.png", "ux0:appmeta/MLCL00001/icon0.png");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/ebootparam.bin", "ux0:appmeta/MLCL00001/ebootparam.bin");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/param.sfo", "ux0:appmeta/MLCL00001/param.sfo");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/livearea/contents/install_button.png", "ux0:appmeta/MLCL00001/livearea/contents/install_button.png");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/livearea/contents/bg.png", "ux0:appmeta/MLCL00001/livearea/contents/bg.png");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/livearea/contents/startup.png", "ux0:appmeta/MLCL00001/livearea/contents/startup.png");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/livearea/contents/template.xml", "ux0:appmeta/MLCL00001/livearea/contents/template.xml");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_sys/sealedkey", "ux0:appmeta/MLCL00001/sce_sys/sealedkey");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/files.db", "ux0:appmeta/MLCL00001/sce_pfs/files.db");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/01f0f674.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/01f0f674.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/2ea25b11.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/2ea25b11.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/5bd505be.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/5bd505be.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/9c136fd0.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/9c136fd0.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/335d1b4f.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/335d1b4f.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/338cf704.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/338cf704.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/2842bf3b.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/2842bf3b.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/41654e51.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/41654e51.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/d9f3c059.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/d9f3c059.icv");
								copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/e7116468.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/e7116468.icv");
						                copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/appmeta/MLCL00001/sce_pfs/icv.db/aec3e5d4.icv", "ux0:appmeta/MLCL00001/sce_pfs/icv.db/aec3e5d4.icv");
						                copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/temp/app_work/MLCL00001/rec/first_boot.bin", "ux0:temp/app_work/MLCL00001/rec/first_boot.bin");
								sceIoRemove("ux0:id.dat");
								printf("\n\nNice!! Rebooting in 5s...");
								sprintf(con_data, "\n\nNice!! Rebooting 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();
							        break;

                                                        case 2://Molecular in Near
                                                                if (doesDirExist("vs0:app/NPXS10000/MLCL")) {sprintf(con_data, "You have already become MOLECULAR in NEAR!!\n");
								strcat(log_text,con_data); }
                                                                else if (doesFileExist("vs0:app/NPXS10000/sce_sys/pic0.png")) {printf("Installing Molecular in NEAR\nand making a copy of the Database\n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoMkdir("vs0:app/NPXS10000/MLCL" , 0777);
								sceIoRemove("ux0:backup_NEAR/DB/app_bkp.db");
                                                                sceIoRemove("ux0:backup_NEAR/DB/iconlayout_bkp.ini");
								sceIoRemove("ux0:backup_NEAR/DB/id_bkp.dat");
                                                                sceIoRmdir("ux0:backup_NEAR/DB");
                                                                sceIoRmdir("ux0:backup_NEAR");
                                                                sceIoMkdir("ux0:backup_NEAR" , 0777);
                                                                sceIoMkdir("ux0:backup_NEAR/DB" , 0777);

                                                                copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/eboot.bin" ,"vs0:app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/icon0.png" ,"vs0:app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/livearea/contents/bg.png" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/bg.png");
                                                                copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/livearea/contents/install_button.png" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/install_button.png");
                                                                copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/livearea/contents/startup.png" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/startup.png");
                                                                copyFile("ux0:app/HMRC00001/data/MOLECULAR_INSTALL/app/MLCL00001/sce_sys/livearea/contents/template.xml" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                
								sceIoRemove("vs0:app/NPXS10000/sce_sys/pic0.png");
                                                                copyFile("ur0:shell/db/app.db", "ux0:backup_NEAR/DB/app_bkp.db");
                                                                copyFile("ux0:iconlayout.ini", "ux0:backup_NEAR/DB/iconlayout_bkp.ini");
								copyFile("ux0:id.dat", "ux0:backup_NEAR/DB/id_bkp.dat");
                                                                sceIoRemove("ux0:id.dat");
								sceIoRemove("ur0:shell/db/app.db");
								sceIoRemove("ux0:iconlayout.ini");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nNice!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset(); }
								else {sprintf(con_data, "Restores NEAR first...\n");
								strcat(log_text,con_data); }
                                                                break;
                                                         
							case 3://Vitashell in NEAR
							        if (doesDirExist("vs0:app/NPXS10000/MLCL2")) {sprintf(con_data, "You already have VITASHELL installed in NEAR!!\n");
								strcat(log_text,con_data); }
                                                                else if (doesFileExist("vs0:app/NPXS10000/sce_sys/pic0.png")) {printf("Installing VITASHELL in NEAR\nand making a copy of the Database\n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoMkdir("vs0:app/NPXS10000/MLCL2" , 0777);
								sceIoRemove("ux0:backup_NEAR/DB/app_bkp.db");
                                                                sceIoRemove("ux0:backup_NEAR/DB/iconlayout_bkp.ini");
								sceIoRemove("ux0:backup_NEAR/DB/id_bkp.dat");
                                                                sceIoRmdir("ux0:backup_NEAR/DB");
                                                                sceIoRmdir("ux0:backup_NEAR");
                                                                sceIoMkdir("ux0:backup_NEAR" , 0777);
                                                                sceIoMkdir("ux0:backup_NEAR/DB" , 0777);

                                                                copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/eboot.bin" ,"vs0:app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/icon0.png" ,"vs0:app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/livearea/contents/bg.png" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/bg.png");
								copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/livearea/contents/startup.png" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/startup.png");
                                                                copyFile("ux0:app/HMRC00001/data/VITASHELL_APP/sce_sys/livearea/contents/template.xml" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                
								sceIoRemove("vs0:app/NPXS10000/sce_sys/pic0.png");
                                                                copyFile("ur0:shell/db/app.db", "ux0:backup_NEAR/DB/app_bkp.db");
                                                                copyFile("ux0:iconlayout.ini", "ux0:backup_NEAR/DB/iconlayout_bkp.ini");
								copyFile("ux0:id.dat", "ux0:backup_NEAR/DB/id_bkp.dat");
                                                                sceIoRemove("ux0:id.dat");
								sceIoRemove("ur0:shell/db/app.db");
								sceIoRemove("ux0:iconlayout.ini");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nNice!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset(); }
								else {sprintf(con_data, "Restores NEAR first...\n");
								strcat(log_text,con_data); }
                                                                break;
							
							case 4://Near restore
                                                                if (doesFileExist("vs0:app/NPXS10000/sce_sys/pic0.png")) {
                                                                sprintf(con_data, "You already have original NEAR!!...\n");
								strcat(log_text,con_data); }
								else if (doesDirExist("vs0:app/NPXS10000/MLCL2") && doesFileExist("ur0:tai/boot_config.txt")) {printf("restoring NEAR \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoRmdir("vs0:app/NPXS10000/MLCL2");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/eboot360.bin" ,"vs0:app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/icon0.png" ,"vs0:app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/template.xml" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/pic0.png" ,"vs0:app/NPXS10000/sce_sys/pic0.png");
								sceIoRemove("vs0:app/NPXS10000/sce_sys/livearea/contents/bg.png");
								sceIoRemove("vs0:app/NPXS10000/sce_sys/livearea/contents/startup.png");

	                                                        copyFile("ux0:backup_NEAR/DB/app_bkp.db" ,"ur0:shell/db/app.db");
                                                                copyFile("ux0:backup_NEAR/DB/iconlayout_bkp.ini" , "ux0:iconlayout.ini");
								copyFile("ux0:backup_NEAR/DB/id_bkp.dat", "ux0:id.dat");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nRestored!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
								else if (doesDirExist("vs0:app/NPXS10000/MLCL2") && doesFileExist("vs0:tai/boot_config.txt")) {printf("Restoring NEAR \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoRmdir("vs0:app/NPXS10000/MLCL2");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/eboot365.bin" ,"vs0:app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/icon0.png" ,"vs0:app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/template.xml" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/pic0.png" ,"vs0:app/NPXS10000/sce_sys/pic0.png");
								sceIoRemove("vs0:app/NPXS10000/sce_sys/livearea/contents/bg.png");
								sceIoRemove("vs0:app/NPXS10000/sce_sys/livearea/contents/startup.png");

	                                                        copyFile("ux0:backup_NEAR/DB/app_bkp.db" ,"ur0:shell/db/app.db");
                                                                copyFile("ux0:backup_NEAR/DB/iconlayout_bkp.ini" , "ux0:iconlayout.ini");
								copyFile("ux0:backup_NEAR/DB/id_bkp.dat", "ux0:id.dat");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nRestored!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
                                                                else if (doesDirExist("vs0:app/NPXS10000/MLCL") && doesFileExist("ur0:tai/boot_config.txt")) {printf("Restoring NEAR \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoRmdir("vs0:app/NPXS10000/MLCL");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/eboot360.bin" ,"vs0:app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/icon0.png" ,"vs0:app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/template.xml" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/pic0.png" ,"vs0:app/NPXS10000/sce_sys/pic0.png");
								sceIoRemove("vs0:app/NPXS10000/sce_sys/livearea/contents/bg.png");
								sceIoRemove("vs0:app/NPXS10000/sce_sys/livearea/contents/startup.png");

	                                                        copyFile("ux0:backup_NEAR/DB/app_bkp.db" ,"ur0:shell/db/app.db");
                                                                copyFile("ux0:backup_NEAR/DB/iconlayout_bkp.ini" , "ux0:iconlayout.ini");
								copyFile("ux0:backup_NEAR/DB/id_bkp.dat", "ux0:id.dat");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nRestored!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
								else if (doesDirExist("vs0:app/NPXS10000/MLCL") && doesFileExist("vs0:tai/boot_config.txt")) {printf("Restoring NEAR \n");
								{for (i = 0; i < 15; i++) {
		                                                printf(".");
		                                                vshIoUmount(i * 0x100, 0, 0, 0); // id, unk1, unk2, unk3 (flags ?)

		                                                printf(".");
		                                                _vshIoMount(i * 0x100, 0, 2, malloc(0x100)); // id, unk, permission, work_buffer
	                                                        }
                                                                }
                                                                sceIoRmdir("vs0:app/NPXS10000/MLCL");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/eboot365.bin" ,"vs0:app/NPXS10000/eboot.bin");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/icon0.png" ,"vs0:app/NPXS10000/sce_sys/icon0.png");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/template.xml" ,"vs0:app/NPXS10000/sce_sys/livearea/contents/template.xml");
                                                                copyFile("ux0:app/HMRC00001/data/NEAR/pic0.png" ,"vs0:app/NPXS10000/sce_sys/pic0.png");
								sceIoRemove("vs0:app/NPXS10000/sce_sys/livearea/contents/bg.png");
								sceIoRemove("vs0:app/NPXS10000/sce_sys/livearea/contents/startup.png");

	                                                        copyFile("ux0:backup_NEAR/DB/app_bkp.db" ,"ur0:shell/db/app.db");
                                                                copyFile("ux0:backup_NEAR/DB/iconlayout_bkp.ini" , "ux0:iconlayout.ini");
								copyFile("ux0:backup_NEAR/DB/id_bkp.dat", "ux0:id.dat");
                                                                strcpy(log_text,"");
                                                                select_menu();
	                                                        printf("\n\nRestored!! Rebooting in 5s...");
								strcat(log_text,con_data);
                                                                sceKernelDelayThread(6 * 1000 * 1000);
	                                                        scePowerRequestColdReset();}
                                                                else
                                                                {sprintf(con_data, "You do not have a copy of NEAR... :(  \n");
								strcat(log_text,con_data); }
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
							        printf("Desinstalando RECOVERY.");
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
								{printf("Instala HomeRecovery con la VPK para desinstalar\n");}
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
