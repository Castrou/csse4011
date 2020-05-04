# Machine is a MicroPython library.
from machine import Pin, UART, I2C
import time

### PROGRAM START ###
# Initialise UART
serial = UART(4, 115200)
serial.init(115200, bits=8, parity=None, stop=1)

# Initialise I2C
i2c2 = I2C(scl=Pin('PB10'), sda=Pin('PB11'), freq=100000)

val = int.from_bytes(i2c2.readfrom_mem(0x6A, 0x10, 1), 'big') # Sample rate accel set to normal mode
i2c2.writeto_mem(0x6A, 0x10, (val | 0x40).to_bytes(1, 'big'))

val = int.from_bytes(i2c2.readfrom_mem(0x6A, 0x15, 1), 'big') # 
i2c2.writeto_mem(0x6A, 0x15, (val | 0x10).to_bytes(1, 'big'))

# Get PB14 and PA5 and treat them as GPIO Output pins
led_1 = Pin('PB14', Pin.OUT)
led_2 = Pin('PA5', Pin.OUT)
value1 = 1
value2 = 0

### Function Definitions
def build_packet(type, sid, i2caddr, regaddr, regval):
	preamble = 0xAA
	packet = bytearray(4*len(sid)+2)
	tl_byte = (type << 4) | (len(regval) & 0x0F)
	packet[0] = preamble
	packet[1] = tl_byte

	for i in range(len(regval)):
		packet[4*i+2] = sid[i]
		packet[4*i+3] = i2caddr[i]
		packet[4*i+4] = regaddr[i]
		packet[4*i+5] = regval[i]

	return packet

def read_packet(packet: bytearray):

	preamble = packet[0]
	sid = []
	i2caddr = []
	regaddr = []
	regval = []
	newRegval = []

	if(preamble == 0xaa):

		type = (packet[1] >> 4)
		length = int(packet[1] & 0x0F) * 4

		# Demo Correct Packet Formatting
		print("RECV PACKET: TYPE=", end="")
		if (type == 1):
			print("REQUEST", end=" { ")
		elif (type == 2):
			print("RESPONSE", end=" { ")
		for i in range(length + 2):
			print(hex(packet[i]), end=" ")
		print("}")

		# Get values and do things
		for i in range(length):
			sid.append(packet[2 + 4*i + 0]) 
			i2caddr.append(packet[2 + 4*i + 1])
			regaddr.append(packet[2 + 4*i + 2]) 
			regval.append(packet[2 + 4*i + 3])

			if(i2caddr[i] == 0):
				pass
			elif(i2caddr[i] % 2):
				# Read command
				val = i2c2.readfrom_mem((i2caddr[i]>>1), regaddr[i], 1)
				newRegval.append(int.from_bytes(val, 'big'))
			else:
				# Write command
				i2c2.writeto_mem((i2caddr[i]>>1), regaddr[i], regval[i].to_bytes(1, 'big'))
				newRegval.append(regval[i])

		TxPacket = build_packet(2, sid, i2caddr, regaddr, newRegval)
		print("SENT", end=" { ")
		for i in range(length + 2):
			print(hex(TxPacket[i]), end=" ")
		print("}")
		serial.write(TxPacket)

		# End of function
		sid.clear()
		i2caddr.clear()
		regaddr.clear()
		regval.clear()

def uart_handler():
	buff = bytearray(100)
	var = serial.readinto(buff)
    
	if (var != None):
		read_packet(buff)

# Setup UART Interrupt
# serial.irq(trigger=UART.RX_ANY, priority=1, uart_handler());

### MAIN ###
# Toggle the pin every one second
while 1:
	led_1.value(value1)
	led_2.value(value2)
	if value1 == 1:
		value1 = 0
		value2 = 1
	else:
		value1 = 1
		value2 = 0

	uart_handler()

	# Somewhat similar to HAL_Delay()
	time.sleep(1)