# stage2_sensor

目标：在 RISC-V 平台上实现 **GPIO 按钮中断 + UART 输出**，并用 Renode Monitor 手动触发按钮。

## 目录结构

- `my_platform.repl`：RISC-V + PLIC + GPIO + UART 平台
- `stage2_sensor.resc`：Renode 启动脚本
- `startup.s` / `main.c` / `linker.ld`：固件
- `Makefile`

## 构建

在本目录执行：

```sh
make
```

## 运行

```sh
renode stage2_sensor.resc
```

运行后你应该在 UART Analyzer 里看到：

- `stage2_sensor: init`
- `stage2_sensor: ready, press button in Renode`

## 触发按钮中断（Renode Monitor）

在 Renode Monitor 里执行：

```text
sysbus.gpio.button Press
```

注意：请在 UART 显示“ready”之后再按按钮，确保固件已经配置好 GPIO 输入与中断。

每按一次会在 UART 输出里看到类似：

```
[IRQ] Button pressed, count = 1
[IRQ] Temp = 25 C
```

要再次触发，需要Release。或者一直Toggle来反转。

> 不妨查看这个button具备哪些方法和属性。

## 虚拟温度传感器（TMP108）

平台里增加了一个 I2C 温度传感器：`tempSensor`。

在 Renode Monitor 里可以直接设置/读取温度：

```text
sysbus.i2c.tempSensor Temperature 36.5
sysbus.i2c.tempSensor Temperature
```

说明：

- 该传感器挂在 `i2c` 总线上（`I2C.OpenCoresI2C`）
- I2C 设备地址为 `0x48`

## 说明

- GPIO pin 0 连接到 `Miscellaneous.Button`
- GPIO 的中断源映射到 PLIC：pin0 -> IRQ 8
- 固件中配置了：
  - GPIO 输入 + 上升沿中断（同时开启 `OUTPUT_EN` 以允许外部按钮驱动该引脚）
  - PLIC 优先级、使能、阈值
  - `mtvec` 指向 `trap_entry`，并打开 `MIE + MEIE`
