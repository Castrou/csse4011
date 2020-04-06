# CSSE4011 Practical 1

## Cameron Stroud - 44344968

## Design Tasks

### Part A - Build Environment

#### Design Task 1A: EI-FreeRTOS Installation and Integration

EI-FreeRTOS successfully installed and integrated into development environment

#### Design Task 2A: Basic Example

Example program has been made. LEDs cycle through RED > GREEN > BLUE in a repeating pattern.

### PART B - Command Line Interface

#### Design Task 1B: System Timer Control Example

System Timer Control Implemented. Arguments are as follows:
'f': Format as HH:MM:SS
No arguments: Time in seconds

example usage: ```time f

#### Design Task 2B: LED Control Example

LED Control Implemented. Arguments are as follows:
'o': Turn on
'f': Turn off
't': Toggle

'r': Red LED
'g': Green LED
'b': Blue LED
'c': LED Cycle loop from Task 2A

example usage: ```led o r

#### Design Task 3B: Log Message

Not yet attempted

### Part C - AHU Base Hardware

#### Design Task: Create a base schematic for the AHU

Base schematic has been created for the AHU in the Grove Shield Feather Wing format (complete with Argon platform symbol).

---

## Folder Structure

├── ei-freertos
├── csse4011-s4434496
|    ├── ei-changes
|    ├── myoslib
|    **├── pracs**
|    **│   ├── prac1**     
|   │   |   ├── inc
|   |   |   |   ├── application.h
|   |   |   |   └── gatt_nrf52.h        
|   │   |   ├── src
|   |   |   |   ├── gatt_nrf52.c
|   |   |   |   ├── gatt.xml
|   |   |   |   └── prac1.c
|   |   |   ├── Makefile
|   |   |   ├── filelist.mk
**|   |   |   └── README.md**
|    └── project

## References

[SnapEDA 2821 Symbol & Footprint](https://www.snapeda.com/parts/2821/Adafruit%20Industries%20LLC/view-part/)
[FreeRTOS+CLI](https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_CLI/Download_FreeRTOS_Plus_CLI.html)
Tom Steele's cli_frontend script (myoslib/tools/cli_frontend)
