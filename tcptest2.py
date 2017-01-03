import socket, struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('192.168.1.67', 420))


def gpio_enable(gpio):
	s.send(struct.pack('HHH', 0x11, 2, gpio))
	d = b''
	while 1:
		d += s.recv(4)
		if len(d) == 4:
			print(struct.unpack('HH', d))
			break

def gpio_write(gpio, state):
	s.send(struct.pack('HHHH', 0x12, 4, gpio, state))
	d = b''
	while 1:
		d += s.recv(4)
		if len(d) == 4:
			print(struct.unpack('HH', d))
			break

def uart_set_baud(baudrate):
	s.send(struct.pack('HHI', 0x14, 4, baudrate))
	d = b''
	while 1:
		d += s.recv(4)
		if len(d) == 4:
			print(struct.unpack('HH', d))
			break

def uart_write(data):
	s.send(struct.pack('HH', 0x10, len(data)) + data)
	d = b''
	while 1:
		d += s.recv(4)
		if len(d) == 4:
			print(struct.unpack('HH', d))
			break

def uart_read(length):
	s.send(struct.pack('HHH', 0x13, 2, length))
	d = b''
	while 1:
		d += s.recv(4)
		if len(d) == 4:
			print(struct.unpack('HH', d))
			break

#s.close()
