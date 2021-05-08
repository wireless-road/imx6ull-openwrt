import socket
import binascii
from multiprocessing import Process

can_packet1 = [0x00, 0x00, 0x03, 0x33, 0x07, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE]
can_packet2 = [0x00, 0x00, 0x09, 0x99, 0x08, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11]

server_port = 15740
client_port = 15741
client_address = "192.168.1.91"


def run_udp_server():
    s1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = ("0.0.0.0", server_port)
    s1.bind(server_address)

    while True:
        data, address = s1.recvfrom(4096)
        data = binascii.hexlify(data).decode()
        print("UDP Server received: ", data, "\n")
        send_data = bytearray(can_packet1)
        s1.sendto(send_data, address)
        print("UDP Server sent: ", send_data, "\n")


if __name__ == '__main__':
    udp_server = Process(target=run_udp_server)
    udp_server.start()

    s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)

    while True:
        send_data = input(" => ")
        send_data = bytearray(can_packet2)
        s2.sendto(send_data, (client_address, client_port))
        print("UDP Client Sent : ", send_data, "\n")

    s2.close()
