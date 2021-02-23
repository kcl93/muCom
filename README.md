# muCom
muCom - The reliable, fast and easy way to exchange binary data via UART

This library targets communication between microcontrollers and microcontroller or PC connected to it.
It supplies an easy and fast way to exchange data and does not rely on converting e.g. float variables to ASCII but just directly transmits its binary data.
This makes it much faster when used on slow microcontrollers as it does not only reduce the amount of data transmitted data but also decreases the CPU overhead by quite a lot.

This library allows to make different types of variables accessible via the serial interface in such a way that the external partner is able to update the variables or request their current value.
Additionally it is possible to remotely execute any linked function in the communication partner.

Variables and functions are accessed via indexes which need to be known to their respective communication partner along with the type of variable behind each index.
The muCom protocol does not define master or slave ECUs. Both act as equal partners and define their capabilities but the variables and functions they link to the muCom interface.


##### Benchmark results from v2.0 #####
| Function | Execution time in us |
| --- | --- |
| writeByte()      | 42 |
| writeShort()     | 62 |
| writeLong()      | 99 |
| writeLongLong()  | 164 |
| writeFloat()     | 100 |
| writeDouble()    | 100 |
| readByte()       | 307 |
| readShort()      | 411 |
| readLong()       | 527 |
| readLongLong()   | 767 |
| readFloat()      | 527 |
| readDouble()     | 527 |
| invokeFunction() | 42 |

Executed on an Atmega328p with a 250000kBaud serial interface in loopback mode.


##### Frame structure #####

See muComBase.cpp for details regarding the binary structure of muCom frames.
Each muCom frame is tuned for maximum transmission speed and efficiency resulting in a binary efficiency of 87.5% when comparing the useful transmitted data (frame type, payload byte count and target variable ID are considered useful) to the overall frame length.
The muCom protocol has no need for wait times for frame synchronization, allowing the serial interface to run at 100% load when streaming data as fast as possible.
