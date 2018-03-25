# HomeRecovery EOL for enso
Ps Vita and PSTV Recovery Adaptor using enso 3.60 and 3.65!!

# Install
Remember this can be a dangerous process see notes for more info.

Install and boot VPK

![](https://fotos.subefotos.com/3956162151dd9e07428187cf6d27886ao.jpg "the icon") ![](https://fotos.subefotos.com/75d4c212c98ba6e5631cfdb3a79e55b9o.jpg "")
![what the menu looks like](https://fotos.subefotos.com/6eb5c583b63b3ea5d1dc16e739db073co.jpg "The Menu")

# Usage
Press right trigger on boot while the blue screen is visible

-  Menu: ------continue: boot normal, exit of Recovery
-         |
-         ----BOOT:----Reboot
-         |          |--IDU on
-         |          |--IDU off (demo mode)
-         |           
-         ----Fixes:---Erase id.dat
-         |          |--Erase act.dat
-         |          |--Erase ux0 tai config
-         |          |--Erase ur0 tai config
-         |          |--Erase Registre
-         |          |--Erase Database
-         |
-         ----Mount:---Mount ux0 (Mcard)
-         |          |-Unmount ux0
-         |
-         ---Backup:---Copy activation
-         |          |-Restore activation
-         |          |-copy ur0 tai config
-         |          |-restore ur0 tai config
-         |          |-copy ux0 tai config
-         |          |-restore ux0 tai config
-         |          |-copy database
-         |          |-restore database
-         |
-         ---Extras:---Install VITASHELL
-                    |-Install MOLECULAR
-                    |-Inject Molecular in NEAR
-                    |-Inject Vitashell in NEAR
-                    |-Restore to factory NEAR
-                    |-Sistem info (imei and cid)
-                    |-Uninstall Recovery (restore boot_config)
-                    |-clean Log"
                    
# Releases

v1.03: Fixed NEAR operations, Now NEAR is restored from the same vpk data, in the menu. Now the option to install MOLECULAR has been implemented, the option of injecting VITASHELL in its entirety into NEAR has been implanted, the only thing that has resisted me is the icons when doing NEARMOD, the order is lost, I am sorry but could not do anything about it if I want it NEARMOD work.

v1.02: Fixed when icons / bubbles are messed up when doing operations on near or installing vitashell

v1.01: Fixed when restore NEAR mode2 missing pic0.png 

V1.00: Copy and restore Database, Erase Database, install Vitashell from recovery menu, Inject Molecular in NEAR without have Molecular          installer in the console (mode2). Clean and optimize de code. Fixs. Now the vpk detected firmware of the console for install the          correct module, now PSTV compatible. Change icon style from livearea.

v0.93: VPK installer HomeRecovery (add option uninstall inside recovery menu)

v0.92: only clean the code.

v0.91: Added the option to inject Vitashell after making the copy of Molecular. Corrected graphic failures in the processes.
                                            
v0.9: Now you can replace NEAR® with MolecularShell, create a backup in the process and restore to the previous state if necessary.

v0.5: Now you can make backups and restores of the activation files or the Tai config.txt files.

# Notes
This could also be used as a normal untility app
While this could help recover your vita it is still a risk to install and if done incorrectly could stop you booting into the shell, install at your own risk!
