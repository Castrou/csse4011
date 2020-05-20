import numpy as np 
import matplotlib.pyplot as plt 
import math

x = 0

while(1):
    y = math.sin(x)

    plt.plot((x, y))
    x += 1;

    if x >= 360:
        x = 0