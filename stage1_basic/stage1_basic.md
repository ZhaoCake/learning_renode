# stage1_basic 记录

这个目录是一个 Renode + RISC-V RV32 bare-metal 的最小实验工程，用来练习：
- 自己描述 CPU/内存/外设平台（不依赖现成 SiFive 开发板脚本）
- 用链接脚本 + 启动汇编把程序跑起来
- 在 Renode 中观察 GPIO 寄存器访问与行为
- 理解“复位向量 / ResetVector”与“ELF 加载入口”的区别

## 1. 平台与内存映射

平台描述见 [my_platform.repl](my_platform.repl)。当前关键映射：
- Flash（用 `Memory.MappedMemory` 简化模拟）：`0x2000_0000`，大小 `16MiB`
- RAM：`0x8000_0000`，大小 `64KiB`
- GPIO（SiFive_GPIO 模型，但挂在自定义地址）：`0x4000_0000`
- UART（可选）：`0x4001_0000`

说明：这里的 “flash” 只是一个映射内存块（MappedMemory），并不包含真实 SPI/QSPI Flash 控制器与 XIP 控制器的时序/协议。它更像是“ROM/Flash 地址空间里可读的存储器”，用于学习最小启动链路。

链接脚本见 [linker.ld](linker.ld)：
- `.text/.rodata` 放在 `flash` 区域（因此 PC 会在 `0x2000_0000` 附近执行）
- `.data` 运行于 RAM，但加载镜像位于 flash（`> ram AT > flash`）
- 启动汇编 [startup.s](startup.s) 在 `_start` 做：设置 SP、拷贝 `.data`、清零 `.bss`，然后 `call main`

## 2. ISA 扩展：为什么需要 zicsr

编译/汇编使用 `-march=rv32imac_zicsr`（见 [Makefile](Makefile)）。原因：
- 新版 RISC-V 工具链里 CSR 指令（如 `csrr/csrw`）属于 `Zicsr` 扩展
- 如果 `-march` 不包含 `zicsr`，汇编会报：`extension 'zicsr' required`

## 3. 复位向量（ResetVector）与 ELF 入口

### 3.1 Renode 的默认 ResetVector
Renode 的 `CPU.RiscV32` 模型有 `ResetVector` 属性：
- 默认 `ResetVector = 0x1000`（这和很多 SoC/仿真器“片上 ROM 起点”一致）
- 纯粹从“复位”角度看：CPU reset 后会从 `ResetVector` 开始取指

### 3.2 为什么会看到 PC 跳到 0x20000000
我们当前脚本 [stage1.resc](stage1.resc) 使用：
- `sysbus LoadELF firmware.elf`

`LoadELF` 会解析 ELF，把段加载到对应地址后，**直接把 CPU 的 PC 设置为 ELF 的 entry（即 `_start`）**。
所以日志中出现的：
- `cpu: Setting PC value to 0x20000000`

这一步是 **ELF 加载导致的入口设置**，不是“CPU 自己从 0x1000 又跳到了 0x20000000”。
如果希望更贴近真实芯片的“BootROM(0x1000) → 跳转到 flash”，需要显式提供一段 bootrom 代码并避免 `LoadELF` 直接改 PC（例如改用 `LoadBinary` + `cpu Reset`）。本阶段先不做。

### 3.3 在脚本里显式设置 ResetVector
平台文件 `my_platform.repl` 里不能用 `resetVector:` 属性（该 CPU 构造不接受，会报 unused attributes）。
因此我们在 [stage1.resc](stage1.resc) 里设置：
- `cpu ResetVector 0x20000000`

这能让“ResetVector 语义”与我们的 flash 基址一致（便于理解/实验）。

## 4. GPIO 观察与日志机制

### 4.1 GPIO 基址与寄存器
固件见 [main.c](main.c)：
- `GPIO_BASE = 0x40000000`
- 使用 SiFive 风格寄存器偏移：
  - `0x08`：`GPIO_OUTPUT_EN`（使能输出）
  - `0x00`：`GPIO_VALUE`（输出值）

LED 使用 pin：19 / 21 / 22（与平台文件 LED 连接一致）。

### 4.2 为什么 logLevel=DEBUG 还看不到寄存器访问日志
Renode 里：
- `logLevel` 只控制“是否允许打印某个级别的日志”
- **是否打印每一次外设读写** 需要额外打开追踪开关

在 [stage1.resc](stage1.resc) 里我们启用：
- `sysbus LogPeripheralAccess sysbus.gpio true`

即可看到类似：
- `gpio: [cpu: 0x200000A6] WriteUInt32 to 0x8 (PinOutputEnable), value 0x680000.`
- `gpio: [cpu: 0x200000B0] WriteUInt32 to 0x0 (PinValue), value 0x400000.`

### 4.3 为什么日志里地址看起来“不对”
上面日志中的 `to 0x8` / `to 0x0` 是 **外设内部偏移**（offset），不是绝对总线地址。
绝对地址应理解为：
- `GPIO_BASE + offset`
例如：
- `0x40000000 + 0x8 = 0x40000008`

方括号里的 `cpu: 0x200000A6` 是 **CPU 当前 PC**（取指地址），不是 GPIO 地址。

## 5. GDB 调试提示（remote stub）

Renode 提供 GDB server（脚本里 `machine StartGdbServer 3333`）。
注意：
- 连接远程目标后，GDB 的 `run` 语义通常不可用（它是“本机启动进程”, 这个跟GDB Stub的实现有关）
- 常用流程是：Renode 先起 GDB server，CPU 暂停；GDB 连接后打断点，再 `start/continue`

---

更新日期：2026-02-01
