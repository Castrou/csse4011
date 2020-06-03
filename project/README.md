# Cameron Stroud - 44344968

## CSSE4011 Project: COVID-Safe IoT Wearable

## Source Instructions

This documentation assumes that the devices are connected to ports in the following configuration:  

| Device          | Port         |
| --------------- | ------------ |  
| Particle Argon  | /dev/ttyACM0 |  
| B-L475E-IOT01A  | /dev/ttyACM1 |

Unless otherwise specified, the device in question will be the Particle Argon.

## User Instructions

You will need the following library:
`$ sudo pip3 install Localization`

### Hardware Guide

The following table shows the hardware configuration for Part B Tasks:  

| Mode  | Argon | B\_L475E\_IOT01A  |  
| ----- | :---: | :---------------: |  
| Rx    | D8    | D0                |  
| Tx    | D7    | D1                |  

To connect the boards connect Argon D8 to B\_L475E\_IOT01A D1 and Argon D7 to B\_L475E\_IOT01A D0.  

## Application Guide

### Listening to the base

To listen to the AHU/Argon, run the following command from `project/python/`:  
`python3 ./basehost.py -s /dev/ttyACM0`  

### Registering Nodes

To register nodes for use in the CSIW setup the following commands must be used:  
`node add <11:22:33:44:55:66>`  
`node type <11:22:33:44:55:66> <type>`  
`node xy <11:22:33:44:55:66> <x> <y>`  
Note the formatting of the address.  

To review the list of nodes the following command can be used:
`node print`
This will print the full list of nodes along with their address, type, X/Y positions, RSSI, Equivalent mm distance and ultrasonic readings

### Localizing the Mobile Node

When all nodes have been prepared, use the following to start transmitting information to the base:  
`node send o`  
Should you wish to cease transmission, the following can be run:  
`node send f`  

## Folder Structure

```bash
┌── ei-freertos  
├── csse4011-s4434496  
|   ├── ei-changes  
|   ├── myoslib  
|   |   ├── bt
|   |   ├── cli  
|   |   ├── flash  
|   |   |   ├── cli_flash.c             (+)  
|   |   |   ├── cli_flash.h             (+)  
|   |   |   ├── hal_flash.c             (+)  
|   |   |   ├── hal_flash.h             (+)  
|   |   |   ├── os_flash.c              (+)  
|   |   |   └── os_flash.c              (+)  
|   |   ├── hci  
|   |   ├── localisation  
|   |   |   ├── cli_loc.c               (+)  
|   |   |   ├── cli_loc.h               (+)  
|   |   |   ├── os_loc.c                (+)  
|   |   |   └── os_loc.h                (+)  
|   |   ├── log  
|   |   ├── scu  
|   |   ├── ultrasonic  
|   |   |   ├── hal_ultrasonic.c        (+)  
|   |   |   ├── hal_ultrasonic.h        (+)  
|   |   |   ├── os_ultrasonic.c         (+)  
|   |   |   └── os_ultrasonic.h         (+)  
|   |   └── util  
|   ├── pracs  
|   └── project  
|       ├── mobile  (+)  
|       ├── python  
|       |   ├── __pycache__  
|       |   ├── basehost.py             (+)  
|       |   ├── baselisten.py           (+)  
|       |   ├── kalman.py               (+)  
|       |   ├── tdf3listen.py           (+)  
|       |   └── tracking.py             (+)  
|       └── static  (+)  
└── micropython  
```

---

## References

[FreeRTOS+CLI](https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/Download_FreeRTOS_Plus_CLI.html)  
[Python Localization Package](https://pypi.org/project/Localization/)