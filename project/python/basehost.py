## Basehost stuff
import numpy as np 
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import math
import kalman
import localization as lx
import baselisten
import tdf3listen
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
	def __init__(self, name, mmDist):
		self.name = name
		self.mmDist = mmDist

class mobileNode(Node):
	def __init__(self, name):
		super().__init__(name, 0)
		self.type = 0
		# self.statNodes = dict()
		# self.mobNodes = dict()

class staticNode(Node):
	def __init__(self, name, x_pos, y_pos, mmDist):
		super().__init__(name, mmDist)
		self.type = 1
		self.xPos = x_pos
		self.yPos = y_pos
		self.kalman = kalman.Kalman(np.array([0]), np.eye(1), 0.01, 1e-5)
		self.kDist = 0

	def updateKalman(self):
		arr = np.array([self.mmDist])
		self.kalman.update(arr)
		self.kDist = self.kalman.x_hat

	def updateInfo(self, x_pos, y_pos, mmDist):
		self.x_pos = x_pos
		self.y_pos = y_pos
		self.mmDist = mmDist
		self.updateKalman()

class usStaticNode(staticNode):
	def __init__(self, address, x_pos, y_pos, mmDist, usDist):
		super().__init__(address, mmDist)
		self.type = 2
		self.xPos = x_pos
		self.yPos = y_pos
		self.usDist = 0

# Initialise
## Init basehost
basehost = None
tdf3 = None
## Kalman
ndim = 2
nsteps = 50
k = dict()
## Init Mobile Node dictionary
mobileList = dict()
## Init x, y lists
xMobile,yMobile = [],[]
xStatic, yStatic = [],[]

def update_xy():
	# Init variables
	global x, y, xMobile, yMobile, xStatic, yStatic, tdf3
	x, y = 0, 0
	xStatic, yStatic = [],[]
	xMobile, yMobile = [],[]
	lxP = lx.Project(mode="2D",solver="LSE")
	lxObj = None
	# staticAP = []
	# rssiVal = []
	try:
		# Update Statics:
		# print(tdf3.mobile)
		for mobileAddr, mobile in tdf3.mobile.items():
			lxObj, mobile.name = lxP.add_target()

			for staticName, static in tdf3.static.items():
				xStatic.append(static.xPos)
				yStatic.append(static.yPos)
				lxP.add_anchor(static.name, (static.xPos, static.yPos))
				lxObj.add_measure(static.name, (static.kDist+0.00000000001))
				
			# Update Mobile
			try:
				lxP.solve()
			except ZeroDivisionError:
				pass
			xMobile.append(lxObj.loc.x)
			yMobile.append(lxObj.loc.y)

	except AttributeError:
		pass

# Create figure for plotting
fig, ax = plt.subplots()
# Style
left,right = ax.get_xlim()
low,high = ax.get_ylim()
axes = plt.gca()
axes.set_xlim([-1000, 4000])
axes.set_ylim([-1000, 4000])
plt.grid()

def animate(i):
	global xStatic, yStatic, xMobile, yMobile
	update_xy()
	grid = (plt.scatter(xStatic, yStatic, s=100, marker='x', c='b'), plt.scatter(xMobile, yMobile, s=100, marker='o', c='m'))
	return grid

def run_animate_thread():
	anim = animation.FuncAnimation(fig, animate,frames=60, interval=500, blit=True)
	plt.show()

def run_baselisten_thread():
	global basehost 
	basehost = baselisten.Baselisten(**vars(args))
	basehost.run()

def run_tdf3listen_thread():
	global tdf3
	tdf3 = tdf3listen.tdf3listen(base_host="localhost", base_port=vars(args)['base_socket_port'], tdf_server=None)
	tdf3.run()

def run_data_thread():
	# Init
	global basehost, ndim
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
				x_pos, y_pos = packet[index]*100, packet[index+1]*100
				index += 2
				### Add to mobile node list
				if nodeAddr in mobileList[recvAddress].statNodes:
					mobileList[recvAddress].statNodes[nodeAddr].updateInfo(x_pos, y_pos, mmDist)
					pass
				else:
					newStatic = staticNode(nodeAddr, x_pos, y_pos, mmDist)
					mobileList[recvAddress].statNodes[nodeAddr] = newStatic
				# mobileList[recvAddress].lxObj.add_measure(mobileList[recvAddress].statNodes[nodeAddr].address, 
				# 										mobileList[recvAddress].statNodes[nodeAddr].mmDist)

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
				newStatic = usStaticNode(nodeAddr, x_pos, y_pos, mmDist, usDist)
				if nodeAddr in mobileList[recvAddress].statNodes:
					mobileList[recvAddress].statNodes[nodeAddr] = newStatic
					# mobileList[recvAddress].lxObj.add_measure(mobileList[recvAddress].statNodes[nodeAddr].address, 
					# 									mobileList[recvAddress].statNodes[nodeAddr].mmDist)
					pass
				else:
					mobileList[recvAddress].statNodes[nodeAddr] = newStatic
					# mobileList[recvAddress].lxP.add_anchor(mobileList[recvAddress].statNodes[nodeAddr].address, 
					# 										(mobileList[recvAddress].statNodes[nodeAddr].xPos, 
					# 										mobileList[recvAddress].statNodes[nodeAddr].yPos))
					# k[recvAddress] = kalman.Kalman(np.array([0, 0]), np.eye(ndim),0.01, 1e-5)
					# k[recvAddress].update([mmDist, usDist])
				# mobileList[recvAddress].lxObj.add_measure(mobileList[recvAddress].statNodes[nodeAddr].address, 
				# 										(k[recvAddress].x_hat[-1]/1000 + 0.0000000001))

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
	tdf3listen_thread = threading.Thread(target=run_tdf3listen_thread)
	animate_thread = threading.Thread(target=run_animate_thread)
	data_thread = threading.Thread(target=run_data_thread)
	
	baselisten_thread.start()
	tdf3listen_thread.start()
	animate_thread.start()
	data_thread.start()