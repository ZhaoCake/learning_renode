# Renode IronPython script: listen on a TCP socket and set TMP108 temperature
import clr
clr.AddReference('System')
clr.AddReference('System.Core')

import System
from System.Threading import Thread
from System.Net import IPAddress, IPEndPoint
from System.Net.Sockets import Socket, AddressFamily, SocketType, ProtocolType
from System.Text import Encoding
from System.Collections.Generic import List

PORTS = [5555, 5556, 5557]


def log(msg):
    try:
        print(msg)
    except Exception:
        try:
            System.Console.WriteLine(msg)
        except Exception:
            pass


log("[temp-socket] script loaded")


def handle_client(client):
    try:
        buffer = List[System.Byte]()
        recv_buf = System.Array.CreateInstance(System.Byte, 1024)
        while True:
            received = client.Receive(recv_buf)
            if received is None or received == 0:
                break
            for i in range(received):
                b = recv_buf[i]
                if b == 10:  # '\n'
                    line = Encoding.ASCII.GetString(buffer.ToArray()).strip()
                    buffer = List[System.Byte]()
                    if len(line) == 0:
                        continue
                    try:
                        value = float(line)
                        monitor.Parse("sysbus.i2c.tempSensor Temperature {}".format(value))
                        monitor.Parse("sysbus.i2c.tempSensor Temperature")
                        log("[temp-socket] set Temperature = {}".format(value))
                    except Exception as e:
                        log("[temp-socket] parse error: {} ({})".format(line, e))
                else:
                    buffer.Add(b)
    finally:
        try:
            client.Close()
        except Exception:
            pass


def server_loop():
    server = Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp)
    bound_port = None
    for p in PORTS:
        try:
            server.SetSocketOption(System.Net.Sockets.SocketOptionLevel.Socket,
                                   System.Net.Sockets.SocketOptionName.ReuseAddress, True)
            server.Bind(IPEndPoint(IPAddress.Any, p))
            bound_port = p
            break
        except Exception as e:
            log("[temp-socket] bind failed on port {}: {}".format(p, e))
    if bound_port is None:
        try:
            server.Close()
        except Exception:
            pass
        return
    server.Listen(1)
    log("[temp-socket] listening on tcp://0.0.0.0:{}".format(bound_port))
    while True:
        client = server.Accept()
        log("[temp-socket] client connected")
        handle_client(client)


t = Thread(System.Threading.ThreadStart(server_loop))
t.IsBackground = True
t.Start()
