/**
	\brief		
	\details	
	\version	1.0
	\author		Kai Liebich
*/


#ifndef MUCOM_H
#define MUCOM_H

#include <Arduino.h>
#include "muComBase.h"



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
		
		inline int16_t _getTimestamp(void)
			{	return millis();	}
		
		
	public:
		muCom(Stream &ser, uint8_t num_var, uint8_t num_func);
};


#endif //MUCOM_H
