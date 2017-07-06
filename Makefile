TARGET = homerecovery
OBJS   = main.o font.o graphics.o

LIBS = -lSceCtrl_stub -lSceDisplay_stub -lScePower_stub -lSceVshBridge_stub -lSceAppMgr_stub -lSceRegistryMgr_stub

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
