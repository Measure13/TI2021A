import socket
import sys
import struct
import numpy as np
import matplotlib.pyplot as plt

PORT = 3333
sock:socket.socket = None
voltage = np.zeros(1024, np.float32)
last_time = 0

def tcp_client_init(address):
    global sock
    for res in socket.getaddrinfo(address, PORT, socket.AF_UNSPEC,
                                  socket.SOCK_STREAM, 0, socket.AI_PASSIVE):
        family_addr, socktype, proto, canonname, addr = res
    try:
        sock = socket.socket(family_addr, socket.SOCK_STREAM)
        sock.settimeout(60.0)
    except socket.error as msg:
        print(f"ERROR {msg[0]}:{msg[1]}")
        raise
    try:
        print(addr)
        sock.connect(addr)
    except socket.error as msg:
        print('Could not open socket: ', msg)
        sock.close()
        raise
    print("Connected.")
    return True

if __name__ == "__main__":
    if sys.argv[1]:
        res = tcp_client_init(sys.argv[1])
        if res:
            while True:
                data = sock.recv(4096)
                print(f"data:{data}")
                if (data_len := len(data)) > 0:
                    print(f"data_len:{data_len}")
                    for i in range(0, data_len, 4):
                        voltage[last_time + i // 4] = struct.unpack("!f", data[i:i + 4][::-1])[0]
                last_time += data_len // 4
                if (last_time >= 1024):
                    break
            plt.plot(np.arange(last_time), voltage)
            plt.show()