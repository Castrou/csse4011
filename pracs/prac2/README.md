# CSSE4011 Practical 2

## Cameron Stroud - 44344968

## Design Tasks

### Part A - Build Environment

#### Design Task 1A: Micropython Installation and Integration

Micropython installed and integrated into development environment

#### Design Task 2A: Basic Example

Example program has been made in led.py which toggles between two LEDs.  

### PART B - SCU and HCI

#### Design Task 1B: Host Controller Interface (HCI)

Not yet implemented

#### Design Task 2B: AHU HCI HAL/OS MyOSLib Implementation

Not yet implemented

#### Design Task 3B: AHU HCI MyOSLib Implementation



### Part C - AHU and SCU Hardware

#### Design Task: Create a schematic and PCB for the SCU Interface Board

Not yet attempted  

---

## Folder Structure

```bash
┌── ei-freertos
├── csse4011-s4434496
|   ├── ei-changes
|   ├── myoslib
|   |   ├── cli
|   |   ├── log
|   |   ├── scu     (+)
|   |   |   ├──         (+)
|   |   |   └──         (+)
|   |   └── util
|   └── pracs
|   |   ├── prac1
|   |   └── prac2
|   |       ├── inc
|   |       |   ├── application.h
|   |       |   └── gatt_nrf52.h
|   |       ├── src
|   |       |   ├── gatt_nrf52.c
|   |       |   ├── gatt.xml
|   |       |   └── prac2.c
|   |       ├── Makefile
|   |       ├── filelist.mk
|   |       └── README.md
|   └── project
└── micropython     (+)
```

---

## References

[FreeRTOS+CLI](https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/Download_FreeRTOS_Plus_CLI.html)