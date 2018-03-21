TITLE_ID = HMRC00001
TARGET   = HomeRecoveryTV
OBJS     = main.o graphics.o font.o file.o

LIBS    = -lSceVshBridge_stub -lSceDisplay_stub -lSceAppMgr_stub -lScePromoterUtil_stub -lSceSysmodule_stub -lSceIofilemgr_stub -lSceKernelThreadMgr_stub -lSceCommonDialog_stub -lSceDisplay_stub -lSceGxm_stub -lvita2d -lfreetype -lSceShellSvc_stub -lScePgf_stub -lSceSysmodule_stub -lSceAppUtil_stub -lSceCtrl_stub -lScePower_stub -lSceVshBridge_stub -lSceAppMgr_stub -lSceRegistryMgr_stub  -ltaihen_stub -lSceLibKernel_stub -lSceKernelThreadMgr_stub -lSceAppMgr_stub_weak -lSceIofilemgr_stub_weak -lk -lgcc


PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3 -marm
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

%.vpk: eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) $(TARGET) param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin -a data/boot_config.txt=data/boot_config.txt -a data/homerecovery.self=data/homerecovery.self -a pkg/sce_sys/icon0.png=sce_sys/icon0.png -a pkg/sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png -a pkg/sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml -a pkg/sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png $@

eboot.bin: $(TARGET).velf
	vita-make-fself $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.png
	$(PREFIX)-ld -r -b binary -o $@ $^	

clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS) \
		eboot.bin param.sfo

vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/
	@echo "Sent."

send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."
