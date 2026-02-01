# stage1_extend

在basic的实验中，我们提到了renode的程序起始地址的工作机制是加载的elf程序会找到entry。这个是在我们的ld脚本中定义的。那么在扩展实验中，我们会在上一个实验的基础上，不使用elf，转而objcopy得到.bin文件，并在bootrom中加载一个跳转作用的指令序列，模拟真实硬件中的跳转过程。

## 目标

- 用 `objcopy` 生成 `firmware.bin`
- 在 `0x00001000` 放一个最小 BootROM（`bootrom.bin`），复位后 CPU 从 BootROM 取指
- BootROM 执行一段“跳转到 flash 基址”的指令序列，跳到 `0x20000000` 的固件入口 `_start`
- Renode 脚本改用 `sysbus LoadBinary`（不再用 `LoadELF` 直接改 PC）

## 目录内容

- 平台描述：`my_platform.repl`
	- BootROM：`0x00001000`（MappedMemory，给 ResetVector 取指用）
	- Flash：`0x20000000`
	- RAM：`0x80000000`
- 固件：`startup.s` + `main.c` + `linker.ld`（链接到 flash，入口 `_start` 在 `0x20000000`）
- BootROM：`bootrom.s` + `bootrom.ld`（链接到 `0x1000`，只负责跳转）
- Renode 脚本：`stage1_extend.resc`

## 构建

在本目录执行：

```sh
make
```

产物：

- `firmware.elf` / `firmware.bin`
- `bootrom.elf` / `bootrom.bin`

## 运行（Renode）

在本目录执行：

```sh
renode stage1_extend.resc
```

## 实验步骤（建议按顺序做）

下面的步骤对应我们在 `stage1_extend.resc` 里设计的对比实验思路：

### Step 1：正常路径（BootROM → flash）

1. 确保脚本同时加载：
	- `sysbus LoadBinary @bootrom.bin 0x00001000`
	- `sysbus LoadBinary @firmware.bin 0x20000000`
2. 保持复位向量为默认的 `0x1000`（或显式设置 `cpu ResetVector 0x00001000`）
3. 运行 Renode
4. 现象预期：CPU 先执行 BootROM（0x1000），随后跳到 0x20000000 执行固件

### Step 2：只加载固件，不加载 BootROM

1. 注释掉脚本里的 BootROM 加载行（只保留固件的 `LoadBinary`）
2. 保持 `ResetVector = 0x1000`
3. 运行 Renode
4. 现象预期：CPU 会从 0x1000 开始取指，但那里没有有效指令（或是 0 值/垃圾），无法跳到 0x20000000

### Step 3：只加载固件 + ResetVector 直接指向 flash

1. 仍然只加载固件（不加载 BootROM）
2. 把复位向量改为 flash 基址：`cpu ResetVector 0x20000000`
3. 运行 Renode
4. 现象预期：CPU 复位后直接从 0x20000000 开始执行固件 `_start`

这三步能非常直观地对比：

- “真实硬件启动链路（ResetVector → BootROM → 跳转到 flash）”
- “仿真器直接把 PC 设置到入口（LoadELF 的语义）”
- “LoadBinary 只写内存，不负责改 PC”

## 在 Renode Monitor 里读取内存

Renode 的 Monitor 里可以直接读 sysbus 上任意地址的内存。常用命令：

- 读 1 字节：`sysbus ReadByte 0x00001000`
- 读 32-bit：`sysbus ReadDoubleWord 0x00001000`
- 连续读多个字节（适合查看一段指令/数据）：`sysbus ReadBytes 0x00001000 16`

可以通过这些指令检查你的程序加载情况。

## 也可以用 GDB 读取内存

脚本里开启了 GDB server（端口 3333），你可以用 GDB 连接后读内存：

```gdb
target remote :3333

# 以 32-bit word 形式查看内存
x/16wx 0x00001000
x/16wx 0x20000000
```

## 和 stage1_basic 的区别

- `stage1_basic` 用 `LoadELF`：Renode 解析 ELF 后会直接把 CPU 的 PC 设置为 ELF entry（看起来像“瞬移到 0x20000000”）
- `stage1_extend` 用 `LoadBinary`：Renode 只把二进制内容写进内存，**不会帮你改 PC**；PC 的变化完全来自“ResetVector + BootROM 跳转”


