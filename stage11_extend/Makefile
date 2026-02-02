# stage1_extend Makefile
# Build firmware.bin (loaded to flash at 0x20000000) and bootrom.bin (loaded to 0x00001000)

ifeq ($(CC_PREFIX),)
    ifneq ($(shell which riscv32-none-elf-gcc 2>/dev/null),)
        CC_PREFIX = riscv32-none-elf-
    else
        CC_PREFIX = riscv32-unknown-elf-
    endif
endif

CC      = $(CC_PREFIX)gcc
OBJCOPY = $(CC_PREFIX)objcopy

CFLAGS  = -march=rv32imac_zicsr -mabi=ilp32 -g -O0 -mcmodel=medany -ffreestanding

FW_LDFLAGS      = -T linker.ld -nostartfiles -Wl,-Map=firmware.map
BOOTROM_LDFLAGS = -T bootrom.ld -nostdlib -nostartfiles -Wl,-Map=bootrom.map

.PHONY: all clean

all: firmware.bin bootrom.bin

# ---- Firmware ----
firmware.elf: main.o startup.o
	$(CC) $(CFLAGS) $(FW_LDFLAGS) -o $@ $^

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<

startup.o: startup.s
	$(CC) $(CFLAGS) -c -o $@ $<

# ---- BootROM ----
bootrom.elf: bootrom.o
	$(CC) $(CFLAGS) $(BOOTROM_LDFLAGS) -o $@ $^

bootrom.bin: bootrom.elf
	$(OBJCOPY) -O binary $< $@

bootrom.o: bootrom.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.elf *.bin *.map
