# stage2_sensor Makefile

ifeq ($(CC_PREFIX),)
    ifneq ($(shell which riscv32-none-elf-gcc 2>/dev/null),)
        CC_PREFIX = riscv32-none-elf-
    else
        CC_PREFIX = riscv32-unknown-elf-
    endif
endif

CC=$(CC_PREFIX)gcc
CFLAGS=-march=rv32imac_zicsr_zifencei -mabi=ilp32 -g -O0 -mcmodel=medany -ffreestanding
LDFLAGS=-T linker.ld -nostartfiles -Wl,-Map=firmware.map

all: firmware.elf

firmware.elf: main.o startup.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<

startup.o: startup.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.elf *.map
