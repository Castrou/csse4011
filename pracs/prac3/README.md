# Cameron Stroud - 44344968

## CSSE4011 Practical 3

## Source Instructions

This documentation assumes that the devices are connected to ports in the following configuration:  

| Device          | Port         |
| --------------- | ------------ |  
| Particle Argon  | /dev/ttyACM0 |  
| B-L475E-IOT01A  | /dev/ttyACM1 |
| Particle Xenon  | /dev/ttyACM2 |

Unless otherwise specified, the device in question will be the Particle Argon.

## User Instructions

To run the tdf_receiver.py you will need the Tago python library:
`$ sudo pip3 install -U tago`  

### Hardware Guide

The following table shows the hardware configuration for Part B Tasks:  

| Mode  | Argon | B\_L475E\_IOT01A  |  
| ----- | :---: | :---------------: |  
| Rx    | D8    | D0                |  
| Tx    | D7    | D1                |  

To connect the boards connect Argon D8 to B\_L475E\_IOT01A D1 and Argon D7 to B\_L475E\_IOT01A D0.  

## Design Tasks

### Part A - Bluetooth Wireless Communications

#### Design Task 1A: Tagged Data Format

Implemented sending TDF over Bluetooth for:  
Accelerometer Data  
3D Pose Data  
Height Data  
Uptime

#### Design Task 2A: Base Station Unit (BSU)

By running unifiedbase on the Xenon, tdf packets can be read using tdf3listen:  

\*TERMINAL 1\*  
`baselisten -s /dev/ttyACM2 --port=9003`  
\*TERMINAL 2\*  
`tdf3listen --port=9003`  

#### Design Task 3A: AHU Bluetooth Scan Library

Bluetooth scanning can be enabled/disabled using the following commands:  
Enable: `ble s o`  
Disable: `ble s f`  

To test functionality, run tdf_demo on the Xenon.

### PART B - Measuring Altitude and Orientation

#### Design Task 1B: Host Controller Interface (HCI)

Orientation is calculated and can be seen on tdf3listen using:
`ble c 472`  

#### Design Task 2B: Altitude Calculation

Coarse Altitude (elevation from sea level in m) is calculated and can be seen on tdf3listen using:
`ble c 476`  

Pressing the on-board button will also begin streaming altitude calculations over Bluetooth.  

#### Design Task 3B: AHU HCI MyOSLib Implementation

Altitude and Orientation is transferred to BSU over Bluetooth.  

### Part C - Data Viewer Dashboard Interface

A dashboard was implemented using Tago.IO and can be seen here: [TagoIO Dashboard](http://admin.tago.io/public/dashboard/5eb36f0ce0003e001e84b5f6/cd9ec31c-6ef0-4b3d-95f0-2dab399f35ad)  

Data that can be viewed:  
Coarse Altitude  
Roll  
Pitch  

## Folder Structure

```bash
┌── ei-freertos  
├── csse4011-s4434496  
|   ├── ei-changes  
|   ├── myoslib  
|   |   ├── bt
|   |   |   ├── cli_bt.c            (+)
|   |   |   ├── cli_bt.h            (+)  
|   |   |   ├── lib_bt.c            (+)  
|   |   |   ├── lib_bt.h            (+)  
|   |   |   ├── os_bt.c             (+)  
|   |   |   └── os_bt.h             (+)  
|   |   ├── cli  
|   |   ├── hci  
|   |   |   ├── cli_hci.c  
|   |   |   ├── cli_hci.h  
|   |   |   ├── hal_hci.c  
|   |   |   ├── hal_hci.h  
|   |   |   ├── hci_packet.h  
|   |   |   ├── lib_hci.c           (+)  
|   |   |   ├── lib_hci.h           (+)  
|   |   |   ├── os_hci.c  
|   |   |   └── os_hci.h  
|   |   ├── log  
|   |   ├── scu  
|   |   └── util  
|   └── pracs  
|   |   ├── prac1  
|   |   ├── prac2  
|   |   └── prac3
|   |       ├── inc  
|   |       |   ├── application.h   (+)  
|   |       |   └── gatt_nrf52.h    (+)  
|   |       ├── src  
|   |       |   ├── gatt_nrf52.c    (+)  
|   |       |   ├── gatt.xml        (+)  
|   |       |   └── prac3.c         (+)  
|   |       ├── sw
|   |       |   └── tdf_receiver.py (+)  
|   |       ├── filelist.mk         (+)  
|   |       └── Makefile            (+)  
|   └── project  
└── micropython  
```

---

## References

[FreeRTOS+CLI](https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/Download_FreeRTOS_Plus_CLI.html)  
[TagoIO Python SDK](https://github.com/tago-io/tago-sdk-python)