# muCom
muCom - The reliable, fast and easy way to exchange binary data via UART

This library targets communication between microcontrollers and microcontroller or PC connected to it.
It supplies an easy and fast way to exchange data and does not rely on converting e.g. float variables to ASCII but just directly transmits its binary data.
This makes it much faster when used on slow microcontrollers as it does not only reduce the amount of data transmitted data but also decreases the CPU overhead by quite a lot.

This library allows to make different types of variables accessible via the serial interface in such a way that the external partner is able to update the variables or request their current value.
Additionally it is possible to remotely execute any linked function in the communication partner.

Variables and functions are accessed via indexes which need to be known to their respective communication partner along with the type of variable behind each index.
The muCom protocol does not define master or slave ECUs. Both act as equal partners and define their capabilities but the variables and functions they link to the muCom interface.


##### Benchmark results #####

| writeByte():      | 42 |
| writeShort():     | 62 |
| writeLong():      | 99 |
| writeLongLong():  | 165 |
| writeFloat():     | 100 |
| readByte():       | 312 |
| readShort():      | 415 |
| readLong():       | 533 |
| readLongLong():   | 771 |
| readFloat():      | 533 |
| invokeFunction(): | 41 |

Executed on an Atmega328p with a 250000kBaud serial interface in loopback mode.


##### Frame structure #####

See muComBase.h for details regarding the binary structure of muCom frames.
