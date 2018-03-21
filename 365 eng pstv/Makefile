TARGET = homerecovery
OBJS   = main.o font.o graphics.o file.o

LIBS = -lSceIofilemgr_stub -lSceKernelThreadMgr_stub -lSceCommonDialog_stub -lSceDisplay_stub -lSceGxm_stub -lvita2d -lfreetype -lSceShellSvc_stub -lScePgf_stub -lSceSysmodule_stub -lSceAppUtil_stub -lSceCtrl_stub -lScePower_stub -lSceVshBridge_stub -lSceAppMgr_stub -lSceRegistryMgr_stub  -ltaihen_stub -lSceLibKernel_stub -lSceKernelThreadMgr_stub -lSceAppMgr_stub_weak -lSceIofilemgr_stub_weak -lk -lgcc


PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3
ASFLAGS = $(CFLAGS)

all: $(TARGET).self

$(TARGET).self: $(TARGET).velf
	vita-make-fself -c $< $(TARGET).self

%.velf: %.elf
	vita-elf-create $< $@

%.elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf *.velf *.self $(OBJS) param.sfo eboot.bin
