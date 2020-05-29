import numpy as np 
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import math
import kalman
import localization as lx

class Node:
    def __init__(self, address):
        self.address = address

class mobileNode(Node):
	def __init__(self, address):
		super().__init__(address)
		self.type = 0
		self.nodes = {}
		self.kx = 0
		self.ky = 0

class staticNode(Node):
	def __init__(self, address):
		super().__init__(address)
		self.type = 1
		self.xPos = 0
		self.yPos = 0

class usStaticNode(staticNode):
	def __init__(self, address):
		super().__init__(address)
		self.type = 2
		self.usDist = 0

registeredNodes = {}

# node message: PREAMBLE / ID / NODECOUNT / PAYLOAD[0:10]
# payload:		ADDRESS / NODETYPE / MMDIST / X / Y

# Declare mobile test nodes
testMobile0 = mobileNode(0x000000000000)
testMobile1 = mobileNode(0x000000000001)
# Declare static test nodes
testStatic0 = staticNode(0x000000000002)
testStatic0.xPos, testStatic0.yPos = 0, 0
testStatic1 = staticNode(0x000000000003)
testStatic1.xPos, testStatic1.yPos = 1000, 0
testStatic2 = staticNode(0x000000000004)
testStatic2.xPos, testStatic2.yPos = 1000, 1000
testStatic3 = staticNode(0x000000000005)
testStatic3.xPos, testStatic3.yPos = 0, 1000
# Initialise mobile test nodes with static 0:3


nodesList = []
nodesList.append(testMobile0)
nodesList.append(testMobile1)
nodesList.append(testStatic0)
nodesList.append(testStatic1)
nodesList.append(testStatic2)
nodesList.append(testStatic3)

def wifi_handler():
	pass
    # Recieve Wifi info on payload

    # if RxWifi is in nodes:
        # Filter positions
        # Calculate position
        # Update map/graph
        # If mobiles nodes are within proximity, send wifi flag
    # else:
        # create entry?

# Init x, y lists
x,y = [],[]
# Init localisation
lxP = lx.Project(mode="2D",solver="LSE")
lxP.add_anchor(testStatic0.address, (testStatic0.xPos, testStatic0.yPos))
lxP.add_anchor(testStatic1.address, (testStatic1.xPos, testStatic1.yPos))
lxP.add_anchor(testStatic2.address, (testStatic2.xPos, testStatic2.yPos))
lxP.add_anchor(testStatic3.address, (testStatic3.xPos, testStatic3.yPos))

mmDist = 707.1067811865476

mob1, (testMobile0.address) = lxP.add_target()

def update_xy():
	global x, y
	i = 0
	for node in nodesList:
		if node.type == 0:
			mob1.add_measure(testStatic0.address, mmDist)
			mob1.add_measure(testStatic1.address, mmDist)
			mob1.add_measure(testStatic2.address, mmDist)
			mob1.add_measure(testStatic3.address, mmDist)

			lxP.solve()
			try:
				x[i], y[i] = mob1.loc.x, mob1.loc.y
			except IndexError:
				x.append(mob1.loc.x)
				y.append(mob1.loc.y)
			
		elif node.type == 1:
			try:
				x[i], y[i] = node.xPos, node.yPos
			except IndexError:
				x.append(node.xPos)
				y.append(node.yPos)
		i += 1

# Create figure for plotting
fig, ax = plt.subplots()
# Style
left,right = ax.get_xlim()
low,high = ax.get_ylim()
plt.grid()

def animate(i):
	global x, y
	update_xy()
	grid = plt.scatter(x, y, s=100, marker='o')
	return grid,
    
anim = animation.FuncAnimation(fig, animate,frames=120, interval=100, blit=True)
plt.show()