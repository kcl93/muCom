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

writeByte():      42
writeShort():     62
writeLong():      99
writeLongLong():  165
writeFloat():     100
readByte():       312
readShort():      415
readLong():       533
readLongLong():   771
readFloat():      533
invokeFunction(): 41

Executed on an Atmega328p with a 250000kBaud serial interface in loopback mode.


##### Frame structure #####
Byte	Bit(s)	Function
0		7		Start of frame indicator (must be '1')
0		6-5		Frame description
					0	read data response
					1	read data request
					2	write data request
					3	Execute function request
0		4-2		Data byte count -1 (relevant for read and write frames)
0		1-0		Bits 6-7 of 1. payload byte	
1		7		Start of frame indicator (must be '0')
1		6-1		Bits 5-0 of 1. payload byte 
1		0		Bit 7 of 2. payload byte
2		7		Start of frame indicator (must be '0')
2		6-0		Bits 6-0 of 2. payload byte
3		7		Start of frame indicator (must be '0')
3		6-0		Bits 7-1 of 3. payload byte
4		7		Start of frame indicator (must be '0')
4		6		Bit 0 of 3. payload byte
4		5-0		Bits 7-2 of 4. payload byte
5		7		Start of frame indicator (must be '0')
5		6-5		Bits 1-0 of 4. payload byte
5		4-0		Bits 7-3 of 5. payload byte
6		7		Start of frame indicator (must be '0')
6		6-4		Bits 2-0 of 5. payload byte
6		3-0		Bits 7-4 of 6. payload byte
7		7		Start of frame indicator (must be '0')
7		6-3		Bits 3-0 of 6. payload byte
7		2-0		Bits 7-5 of 7. payload byte
8		7		Start of frame indicator (must be '0')
8		6-2		Bits 4-0 of 7. payload byte
8		1-0		Bits 7-6 of 8. payload byte
9		7		Start of frame indicator (must be '0')
9		6-1		Bits 5-0 of 8. payload byte
9		0		Bit 7 of 9. payload byte
10		7		Start of frame indicator (must be '0')
10		6-0		Bits 6-0 of 9. payload byte