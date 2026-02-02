# learning_renode

这是一个用于学习 Renode 的实验项目，包含从裸机到外设交互，再到脚本扩展的逐步实践。

## 目录结构

- stage1_basic：最小 RISC-V 裸机实验（ELF 启动）
- stage1_extend：BootROM + LoadBinary 启动机制实验
- stage2_sensor：GPIO 中断 + UART + I2C 温度传感器读取
- stage2_extend：使用 Renode IronPython 脚本替代 resc，并加入 socket 注入温度
- stage3_rtos：RTOS 预备阶段（待扩展）

## 许可证

本项目遵循仓库中的 LICENSE 文件。
