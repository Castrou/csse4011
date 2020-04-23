# Machine is a MicroPython library.
from machine import Pin, UART, I2C
import time

### PROGRAM START ###
# Initialise UART
serial = UART(4, 115200)
serial.init(115200, bits=8, parity=None, stop=1)

# Initialise I2C
i2c2 = I2C(scl=Pin('PB10'), sda=Pin('PB11'), freq=100000)

# Get PB14 and PA5 and treat them as GPIO Output pins
led_1 = Pin('PB14', Pin.OUT)
led_2 = Pin('PA5', Pin.OUT)
value1 = 1
value2 = 0

### Function Definitions
def read_packet(packet: bytearray):

	preamble = packet[0]
	sid = []
	i2caddr = []
	regaddr = []
	regval = []

	if(preamble == 0xaa):

		type = (packet[1] >> 4)
		length = int(packet[1] & 0x0F)

		# Demo Correct Packet Formatting
		print("RECV PACKET: TYPE=", end="")
		if (type == 1):
			print("REQUEST", end=" { ")
		elif (type == 2):
			print("RESPONSE", end=" { ")
		for i in range(length*4+2):
			print(hex(packet[i]), end=" ")
		print("}")

		# Get values and do things
		for i in range(length/4):
			sid.append(packet[2 + 4*i + 0]) 
			i2caddr.append(packet[2 + 4*i + 1])
			regaddr.append(packet[2 + 4*i + 2]) 
			regval.append(packet[2 + 4*i + 3])

			if(i2caddr[i] % 2):
				# Read command
				retVal = i2c2.readfrom_mem((i2caddr[i]>>1), regaddr[i], 1)
				serial.write("ACK READ\nVALUE: ")
				# serial.write(hex(retVal))
				serial.write("\n")
				time.sleep(1)
			else:
				serial.write("ACK WRITE\n")
				i2c2.writeto_mem(i2caddr[i], regaddr[i], regval[i])

		print("Type: " + str(type))
		print("Length: " + str(length))
		print(sid)
		print(i2caddr)
		print(regaddr)
		print(regval)
		print("\n\n")

		# End of function
		sid.clear()
		i2caddr.clear()
		regaddr.clear()
		regval.clear()
		
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

	buff = bytearray(100)
	var = serial.readinto(buff)
    
	if (var != None):
		read_packet(buff)

	# Somewhat similar to HAL_Delay()
	time.sleep(1)