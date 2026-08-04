import socket

# 0.0.0.0 le dice al script que escuche en todas las tarjetas de red (incluyendo tu wlo1)
UDP_IP = "0.0.0.0" 
UDP_PORT = 1234

# Creamos el socket UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Escuchando telemetría en el puerto {UDP_PORT}...")

while True:
    data, addr = sock.recvfrom(1024) # Buffer de 1024 bytes
    print(f"IP Origen {addr[0]} -> {data.decode('utf-8')}")