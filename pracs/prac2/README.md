# CSSE4011 Practical 2

## Cameron Stroud - 44344968

## Design Tasks

### Part A - Build Environment

#### Design Task 1A: Micropython Installation and Integration

Micropython installed and integrated into development environment

#### Design Task 2A: Basic Example

Example program has been made in led.py which toggles between two LEDs.  
Assuming the CSSE4011 Setup Guide has been completed, the program can be flashed using `pyboard --device /dev/ttyACMx -f cp /../../myoslib/scu/led.py :main.py` relative to the prac2 folder, where ttyACMx is the B_L475E_IOT01A (typically ttyACM0 or ttyACM1)  

### PART B - SCU and HCI

#### Design Task 1B: Host Controller Interface (HCI)

Host controller interface successfully implemented. Both Argon and B_L4753_IOT01A communicate using the following packet format, where each item is 1 byte:  
`<PREAMBLE><TYPExLENGTH><DATAFIELD1>[<DATAFIELD2>]`  
PREAMBLE: 0xAA - ensures signal is a packet  
TYPExLENGTH: 4 type bits masked combined with 4 length bits.  
TYPE: 0x01 for REQUEST; 0x02 for RESPONSE.  
LENGTH: Total number of bytes from datafields.  
DATAFIELD1: First datafield.  
DATAFIELD2: Optional second datafield, used for reading multiple registers at a time (ie. accelerometer axis data).  

Datafields are formatted using the following structure, again each item is 1 byte:  
`<SID><I2CADDR><REGADDR>[<REGVAL>]`  
SID: Sensor ID (See table in Task 2B for more information).  
I2CADDR: I2C Address including R/W bit.  
REGADDR: Register address to read from or write to.  
REGVAL: In a REQUEST packet this is the value to write, in a RESPONSE this is the value of a register that was read.  

#### Design Task 2B: AHU HCI HAL/OS MyOSLib Implementation

Can successfully read from and write to sensor registers using HCI and I2C Address.  
The following table shows the SID and respective I2C Addresses:  

#### Design Task 3B: AHU HCI MyOSLib Implementation

CLI Commands implemented for interfacing with SCU:  
`i2creg <r/w> <sid> <regaddr> [<regval>]` can be used to read from or write to a specific register at a given I2C address.  
`lsm6dsl r <x/y/z/a>` can be used to read acceleration data from a given axis (or all). Note there can be a delay between reads when using the '`a`' command.  

##### Example commands

`i2creg r 1 0x0F`:  
B_L475E_IOT01A Display: `RECV PACKET: { 0xaa 0x14 0x1 0xd5 0xf 0x0 }`  
Argon Display: `RECV(SID1): REGADDR=0x0f REGVAL=0x6a`  

`i2creg w 1 0x10 0x01`:  
B_L475E_IOT01A Display:  
`RECV PACKET: { 0xaa 0x14 0x1 0xd5 0x10 0x1 }`  
Argon Display:  
`RECV(SID1): REGADDR=0x10 REGVAL=0x01`

`lsm6dsl r x`:  
B_L475E_IOT01A Display:  
`RECV PACKET: { 0xaa 0x14 0x1 0xd5 0x29 0x0 0x1 0xd5 0x28 0x0 }`  
Argon Display:  
`X Acceleration: -0.05g(s)`  

`lsm6dsl r y`:  
B_L475E_IOT01A Display:  
`RECV PACKET: { 0xaa 0x14 0x1 0xd5 0x2b 0x0 0x1 0xd5 0x2a 0x0 }`  
Argon Display: `Y Acceleration: -0.03g(s)`  


`lsm6dsl r z`:  
B_L475E_IOT01A Display:  
`RECV PACKET: { 0xaa 0x14 0x1 0xd5 0x2d 0x0 0x1 0xd5 0x2c 0x0 }`  
Argon Display: `Z Acceleration: 0.99g(s)`  

`lsmdsl r a`:  
B_L475E_IOT01A Display:  
`RECV PACKET: { 0xaa 0x14 0x1 0xd5 0x29 0x0 0x1 0xd5 0x28 0x0 }`  
`RECV PACKET: { 0xaa 0x14 0x1 0xd5 0x2b 0x0 0x1 0xd5 0x2a 0x0 }`  
`RECV PACKET: { 0xaa 0x14 0x1 0xd5 0x2d 0x0 0x1 0xd5 0x2c 0x0 }`  
Argon Display:  
`X Acceleration: -0.05g(s)`  
`Y Acceleration: -0.03g(s)`  
`Z Acceleration: 0.99g(s)`  

### Part C - AHU and SCU Hardware

#### Design Task: Create a schematic and PCB for the SCU Interface Board

A schematic has been created for the SCU Interface Board and can be seen in `/pracs/prac2/hw/scu_interface_sch.pdf`  
A PCB has been created for the SCU Interface Board and Gerber Files generated to `/pracs/prac2/hw/scu_interface_gerber`  

---

## Folder Structure

```bash
┌── ei-freertos  
├── csse4011-s4434496  
|   ├── ei-changes  
|   ├── myoslib  
|   |   ├── cli  
|   |   ├── hci  
|   |   |   ├── cli_hci.c       (+)  
|   |   |   ├── cli_hci.h       (+)  
|   |   |   ├── hal_hci.c       (+)  
|   |   |   ├── hal_hci.h       (+)  
|   |   |   ├── hci_packet.h    (+)  
|   |   |   ├── os_hci.c        (+)  
|   |   |   └── os_hci.h        (+)  
|   |   ├── log  
|   |   ├── scu  
|   |   |   ├── hci.py          (+)  
|   |   |   └── led.py          (+)  
|   |   └── util  
|   └── pracs  
|   |   ├── prac1  
|   |   └── prac2  
|   |       ├── hw  
|   |       |   ├──  scu_interface_gerber   (+)  
|   |       |   ├──  scu_interface_sch.pdf  (+)  
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
[SnapEDA 2821 Symbol & Footprint](https://www.snapeda.com/parts/2821/Adafruit%20Industries%20LLC/view-part/)  
[KiCAD Header Footprints](https://kicad.github.io/footprints/Connector_PinHeader_2.54mm)  