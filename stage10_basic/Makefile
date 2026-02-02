# 使用 flake.nix 中设置的 CC_PREFIX，如果没有则默认为 riscv32-none-elf-
# 兼容性处理：如果 CC_PREFIX 未设置，尝试常见的
ifeq ($(CC_PREFIX),)
    # 尝试查找 riscv32-none-elf-gcc
    ifneq ($(shell which riscv32-none-elf-gcc 2>/dev/null),)
        CC_PREFIX = riscv32-none-elf-
    else
        CC_PREFIX = riscv32-unknown-elf-
    endif
endif

CC=$(CC_PREFIX)gcc
# 自定义平台是 RV32IMAC，需要显式指定 zicsr 扩展以支持 CSR 指令
CFLAGS=-march=rv32imac_zicsr -mabi=ilp32 -g -O0 -mcmodel=medany
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
