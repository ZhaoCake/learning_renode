# stage2_extend

### Socket 注入温度（更接近“传感器数据流”）

我们提供了一个基于 TCP socket 的注入方式（Renode 内置 IronPython 监听）：

1. 启动 Renode：

```sh
renode stage2_extend/stage2_extend_socket.resc
```

2. 另开终端发送温度：

```sh
python3 stage2_extend/temp_sender.py 25 26 27 --interval=1.0
```

如果端口被占用，Renode 会自动尝试 5556/5557。此时发送端加上端口参数：

```sh
python3 stage2_extend/temp_sender.py 25 26 27 --interval=1.0 --port=5556
```

Renode 会把 TMP108 的 `Temperature` 更新为对应值，固件在按键中断时读取到最新温度。
