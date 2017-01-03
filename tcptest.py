import socket, struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('192.168.1.67', 420))

with open('pinup04.txt', 'rb') as infile:
	while 1:
		uart_msg = infile.read(128).replace(b'\n', b'\r\n')
		if not uart_msg:
			break


		s.send(struct.pack('HH', 0x10, len(uart_msg)) + uart_msg)
		d = b''
		while 1:
			d += s.recv(4)
			if len(d) == 4:
				print(struct.unpack('HH', d))
				break

s.close()
