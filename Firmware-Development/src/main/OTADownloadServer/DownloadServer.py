import socket
import os

int_max_value = 4294967295

filename = "km0_km4_image2.bin"
fixed_ip = "192.168.2.148"  # Set your fixed IP address here
port = 80

f = open(filename, "rb")
checksum = 0
length = os.stat(filename).st_size
empty = 0

byte = f.read(1)
while byte != b"":
    checksum = checksum + int.from_bytes(byte, 'big')
    byte = f.read(1)
checksum = checksum % int_max_value
f.close()

print("Checksum: " + hex(checksum))
print("Length: " + str(length))

bytes = bytearray()
bytes += checksum.to_bytes(4, 'little')
bytes += empty.to_bytes(4, 'little')
bytes += length.to_bytes(4, 'little')

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((fixed_ip, port))  # Bind to the fixed IP address
    print(f"Listening on {fixed_ip}:{port}")
    s.listen()
    while True:
        c, addr = s.accept()
        print(f"Connected by {addr}")
        c.send(bytes)
        print("Sent header")
        with open(filename, 'rb') as f:
            c.send(f.read())
        print("Sent binary")
        c.close()