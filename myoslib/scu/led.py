# Machine is a MicroPython library.
from machine import Pin, UART, I2C
import hci
import time


# Get PB14 and treat it as a GPIO Output pin
led_1 = Pin('PB14', Pin.OUT)
led_2 = Pin('PA5', Pin.OUT)
value1 = 1
value2 = 0

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
    # Somewhat similar to HAL_Delay()
    time.sleep(1)