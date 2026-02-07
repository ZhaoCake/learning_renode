# learning_renode

这是一个用于学习 Renode 的实验项目，包含从裸机到外设交互，再到脚本扩展的逐步实践。

## 相关视频

[【renode】01 为什么要使用renode](https://www.bilibili.com/video/BV1FXFVzsE4u/?share_source=copy_web&vd_source=99d9a9488d6d14ace3c7925a3e19793e)

[【Renode】02 实验一|Renode上的平台描述与裸机开发](https://www.bilibili.com/video/BV1rSFVz4E42/?share_source=copy_web&vd_source=99d9a9488d6d14ace3c7925a3e19793e)

[【Renode】03 实验一|关于复位向量与程序加载机制的扩展实验](https://www.bilibili.com/video/BV1dFF5zUExX/?share_source=copy_web&vd_source=99d9a9488d6d14ace3c7925a3e19793e)


## 目录结构

- stage1_basic：最小 RISC-V 裸机实验（ELF 启动）
- stage1_extend：BootROM + LoadBinary 启动机制实验
- stage2_sensor：GPIO 中断 + UART + I2C 温度传感器读取
- stage2_extend：使用 Renode IronPython 脚本替代 resc，并加入 socket 注入温度
- stage3_rtos：RTOS 预备阶段（待扩展）

## 许可证

本项目遵循仓库中的 LICENSE 文件。
