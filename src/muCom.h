/**
	\brief		File containing the main class for the muCom interface when being used for an Arduino
	\version	1.0
	\author		Kai Liebich
*/


#ifndef MUCOM_H
#define MUCOM_H

#include <Arduino.h>
#include "muComBase.h"

#define MUCOM_CREATE(name, serial, num_var, num_func)							\
	struct muCom_LinkedVariable_str _##name##_var_buf[ num_var ];				\
	muComFunc _##name##_func_buf[ num_func ];									\
	muCom name(serial, _##name##_var_buf, num_var, _##name##_func_buf, num_func);


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
		
		inline void _disableInterrupts(void)
			{	noInterrupts();	}
		
		inline void _enableInterrupts(void)
			{	interrupts();	}
		
	public:
		muCom(Stream &ser, struct muCom_LinkedVariable_str *var_buf, uint8_t num_var, muComFunc *func_buf, uint8_t num_func);
};


#endif //MUCOM_H
