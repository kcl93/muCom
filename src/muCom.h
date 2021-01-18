/**
	\brief		File containing the main class for the muCom interface when being used for an Arduino
	\version	1.0
	\author		Kai Liebich
*/


#ifndef MUCOM_H
#define MUCOM_H

#include <Arduino.h>
#include "muComBase.h"


/**
	\brief		Main class for the muCom interface when being used for an Arduino
	\details	This class inherits all functions from the muCom base class (see muComBase)
				and additionally implements the interface for the HW access when using the muCom interface on an Arduino.
				It uses the Stream class for communication which can be implemented by the hardware and software UARTS, as well as USB serial emulations.
*/
class muCom : public muComBase
{
	private:
		Stream *_ser;
		
		inline void _write(uint8_t* data, uint8_t cnt)
			{	this->_ser->write(data, cnt);	}
		
		inline uint8_t _read(void)
			{	return this->_ser->read();	}
		
		inline uint8_t _available(void)
			{	return this->_ser->available();	}
		
		inline void _flushTx(void)
			{	this->_ser->flush();	}
		
		inline uint32_t _getTimestamp(void)
			{	return millis();	}
		
		
	public:
		muCom(Stream &ser, uint8_t num_var, uint8_t num_func);
};


#endif //MUCOM_H
