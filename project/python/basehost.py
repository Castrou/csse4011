## Basehost stuff
import numpy as np 
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import math
import kalman
import localization as lx
import baselisten
## Baselisten stuff
import argparse

import serial
import socket
import sys
import os
import time
import datetime
import struct
import threading
import queue
import re
import copy
import select
import logging

from MessageTransport import PacpTransportSerial, PacpTransportSocket
import PacpMessage
##
# Constants
ADDRESS_LENGTH = 6

class Node:
	def __init__(self, address, mmDist):
		self.address = address
		self.mmDist = mmDist

class mobileNode(Node):
	def __init__(self, address):
		super().__init__(address, 0)
		self.type = 0
		self.lxP = lx.Project(mode="2D",solver="LSE")
		self.lxObj, address = self.lxP.add_target()
		self.statNodes = dict()
		self.mobNodes = dict()
		self.kx = 0
		self.ky = 0

class staticNode(Node):
	def __init__(self, address, mmDist):
		super().__init__(address, mmDist)
		self.type = 1
		self.xPos = 0
		self.yPos = 0

class usStaticNode(staticNode):
	def __init__(self, address, mmDist):
		super().__init__(address, mmDist)
		self.type = 2
		self.usDist = 0

# Init basehost
basehost = None
# Init Mobile Node dictionary
mobileList = dict()
# Init x, y lists
xMobile,yMobile = [],[]
xStatic, yStatic = [],[]

def update_xy():
	# Init variables
	global x, y, xMobile, yMobile, xStatic, yStatic
	i, j = 0, 0
	x = 0
	y = 0
	xStatic, yStatic = [],[]
	xMobile, yMobile = [],[]
	try:
		for mobileAddr, mobile in mobileList.items():
			# Update Statics:
			for staticAddr, static in mobile.statNodes.items():
				xStatic.append(static.xPos)
				yStatic.append(static.yPos)
				j += 1
				
			# Update Mobile
			mobile.lxP.solve()
			xMobile.append(mobile.lxObj.loc.x)
			yMobile.append(mobile.lxObj.loc.y)
			i += 1

	except AttributeError:
		pass

# Create figure for plotting
fig, ax = plt.subplots()
# Style
left,right = ax.get_xlim()
low,high = ax.get_ylim()
axes = plt.gca()
axes.set_xlim([0, 3])
axes.set_ylim([0, 3])
plt.grid()

def animate(i):
	global xStatic, yStatic, xMobile, yMobile
	update_xy()
	grid = (plt.scatter(xStatic, yStatic, s=100, marker='o', c='b'), plt.scatter(xMobile, yMobile, s=100, marker='o', c='m'))
	return grid

def run_animate_thread():
	anim = animation.FuncAnimation(fig, animate,frames=30, interval=1000, blit=True)
	plt.show()

def run_baselisten_thread():
	global basehost 
	basehost = baselisten.Baselisten(**vars(args))
	basehost.run()

def run_data_thread():
	# Init
	global basehost
	packet = []
	while (basehost == None):
		pass

	# Loop
	while True:
		# Receive packet from Baselisten
		for i in range(int(len(basehost.serialData)/2)):
			hexVal = int(basehost.serialData[2*i:2*i+2], 16)
			try:
				packet[i] = hexVal
			except IndexError:
				packet.append(hexVal)

		# Extract packet data
		## Tx Device Address 
		index = 0
		recvAddressRaw = packet[index:ADDRESS_LENGTH]
		recvAddress = str(recvAddressRaw)
		index += ADDRESS_LENGTH

		if recvAddress in mobileList:
			pass
		elif (recvAddressRaw != []):
			mobileList[recvAddress] = mobileNode(recvAddress)

		## Number of Nodes in payload
		try:
			recvSize = packet[index]
			index += 1
		except IndexError:
			recvSize = 0

		## Cycle through nodes and add to respected arrays
		for i in range(recvSize):
			## Get node type
			nodeType = packet[index]
			index += 1

			## Get Address
			nodeAddr = str(packet[index:index + ADDRESS_LENGTH])
			index += ADDRESS_LENGTH

			## Mobile node process
			if (nodeType == 0):
				### Get distance
				mmDist = (packet[index] << 8) | packet[index+1]
				index += 2
				### Add to mobile node dist

			## Static node process
			elif (nodeType == 1):
				### Get distance
				mmDist = (packet[index] << 8) | packet[index+1]
				index += 2
				### Get X/Y pos
				x_pos, y_pos = packet[index], packet[index+1]
				index += 2
				### Add to mobile node list
				newStatic = staticNode(nodeAddr, mmDist)
				newStatic.xPos, newStatic.yPos = x_pos, y_pos
				if nodeAddr in mobileList[recvAddress].statNodes:
					mobileList[recvAddress].statNodes[nodeAddr] = newStatic
					mobileList[recvAddress].lxObj.add_measure(mobileList[recvAddress].statNodes[nodeAddr].address, 
														mobileList[recvAddress].statNodes[nodeAddr].mmDist/1000)
					pass
				else:
					mobileList[recvAddress].statNodes[nodeAddr] = newStatic
					mobileList[recvAddress].lxP.add_anchor(mobileList[recvAddress].statNodes[nodeAddr].address, 
															(mobileList[recvAddress].statNodes[nodeAddr].xPos, 
															mobileList[recvAddress].statNodes[nodeAddr].yPos))
				mobileList[recvAddress].lxObj.add_measure(mobileList[recvAddress].statNodes[nodeAddr].address, 
														mobileList[recvAddress].statNodes[nodeAddr].mmDist/1000)

			## Ultrasonic node process
			elif (nodeType == 2):
				### Get distance
				mmDist = (packet[index] << 8) | packet[index+1]
				index += 2
				### Get X/Y pos
				x_pos, y_pos = packet[index], packet[index+1]
				index += 2
				### Get ultrasonic dist
				usDist = (packet[index] << 8) | packet[index+1]
				index += 2
				### Add to mobile node list
				newStatic = usStaticNode(nodeAddr, mmDist)
				newStatic.xPos, newStatic.yPos = x_pos, y_pos
				newStatic.usDist = usDist
				if nodeAddr in mobileList[recvAddress].statNodes:
					mobileList[recvAddress].statNodes[nodeAddr] = newStatic
					mobileList[recvAddress].lxObj.add_measure(mobileList[recvAddress].statNodes[nodeAddr].address, 
														mobileList[recvAddress].statNodes[nodeAddr].mmDist/1000)
					pass
				else:
					mobileList[recvAddress].statNodes[nodeAddr] = newStatic
					mobileList[recvAddress].lxP.add_anchor(mobileList[recvAddress].statNodes[nodeAddr].address, 
															(mobileList[recvAddress].statNodes[nodeAddr].xPos, 
															mobileList[recvAddress].statNodes[nodeAddr].yPos))
				mobileList[recvAddress].lxObj.add_measure(mobileList[recvAddress].statNodes[nodeAddr].address, 
														mobileList[recvAddress].statNodes[nodeAddr].mmDist/1000)

		# End of function
		packet = []
		time.sleep(1)


if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Serial listener for FreeRTOS. Receives messages from a root node via serial and distributes them to clients connected via sockets')
	parser.add_argument('--serial', '-s', dest='serial_port', type=str, default="/dev/ttyUSB0", help='Serial device to connect to')
	parser.add_argument('--device', '-d', dest='device_type', type=str, default="usb", help='Device type')
	parser.add_argument('--baudrate', '--baud', '-b', dest='baudrate', type=int, default=115200, help='Baudrate of connected device')
	parser.add_argument('--port', '-p', dest='base_socket_port', type=int, default=9001, help='Port for whole packet messages')
	parser.add_argument('--show', dest='show', type=str, default="rtc", help='show (r)x messages, (t)x messages, serial (h)eader and/or (d)ebug data in (c)olor')
	parser.add_argument('--logfile', dest='log_file', type=str, default="stderr", help='Filename to direct log messages to')
	parser.add_argument('--loglevel', dest='log_level', type=str, default="INFO", help='Filename to direct log messages to')
    
	args = parser.parse_args()

	baselisten.logging_init(args.log_file, args.log_level)
	logging.critical("Baselisten started...")

	delattr(args, "log_file")
	delattr(args, "log_level")

	baselisten_thread = threading.Thread(target=run_baselisten_thread)
	animate_thread = threading.Thread(target=run_animate_thread)
	data_thread = threading.Thread(target=run_data_thread)
	

	baselisten_thread.start()
	animate_thread.start()
	data_thread.start()