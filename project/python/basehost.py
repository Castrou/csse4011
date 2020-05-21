import numpy as np 
import matplotlib.pyplot as plt 
import math
import kalman
import tracking

class Node:
    # Attributes
    kx
    ky
    # Init
    def __init__(self, payload):
        self.address = payload[]
        self.x = payload[]
        self.y = payload[]
        self.us = payload[]

nodes = {}

def wifi_handler():
    # Recieve Wifi info on payload
    RxWifi = Node(payload)

    if RxWifi is in nodes:
        # Filter positions
        # Calculate position
        # Update map/graph
        # If mobiles nodes are within proximity, send wifi flag
    else:
        # create entry


if __name__ == "__main__":

    while(1):
        # Listen on Wifi