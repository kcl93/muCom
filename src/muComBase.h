/**
	\brief		
	\details	
	\version	1.0
	\author		Kai Liebich
*/


#ifndef MUCOMBASE_H
#define MUCOMBASE_H

#include <stdint.h>
#include "stdlib.h"


#define MUCOM_OK			0
#define MUCOM_ERR			-1
#define MUCOM_ERR_TIMEOUT	-2
#define MUCOM_ERR_COMM		-3

#define MUCOM_DEFAULT_TIMEOUT		100

#define MUCOM_HEADER_BIT_MASK		0x80
#define MUCOM_FRAME_DESC_MASK		0x60
#define MUCOM_DATA_BYTE_CNT_MASK	0x1C
#define MUCOM_READ_RESPONSE			0x00
#define MUCOM_READ_REQUEST			0x20
#define MUCOM_WRITE_REQUEST			0x40
#define MUCOM_EXECUTE_REQUEST		0x60


typedef void (*muComFunc)(uint8_t *data, uint8_t cnt);


struct muCom_LinkedVariable_str
{
	uint8_t* addr;
	uint8_t size;
};



class muComBase 
{
	private:
		struct muCom_LinkedVariable_str *_linked_var;
		uint8_t _linked_var_num;
		muComFunc *_linked_func;
		uint8_t _linked_func_num;
		uint8_t _rcv_buf[11];
		uint8_t _rcv_buf_cnt;
		int16_t _timeout;
		int16_t _lastCommTime;
	
		void writeRaw(uint8_t frameDesc, uint8_t index, uint8_t *data, uint8_t cnt);
		
		virtual void _write(uint8_t* data, uint8_t cnt) = 0;
		virtual uint8_t _read(void) = 0;
		virtual uint8_t _available(void) = 0;
		virtual void _flushTx(void) = 0;
		virtual int16_t _getTimestamp(void) = 0;
		
		
	public:
		muComBase(uint8_t num_var, uint8_t num_func);
		
		
		
		~muComBase(void);
		
		
		
		inline int16_t getLastCommTime(void)
			{	return (this->_getTimestamp() - this->_lastCommTime);	}



		void setTimeout(int16_t timeout);



		uint8_t handle(void);
		
		
		
		int8_t linkFunction(uint8_t index, muComFunc function);
		
		
		
		int8_t linkVariable(uint8_t index, uint8_t *var, uint8_t size);
		
		inline void linkVariable(uint8_t index, uint8_t *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(uint8_t));	}
			
		inline void linkVariable(uint8_t index, int8_t *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(int8_t));	}
		
		inline void linkVariable(uint8_t index, uint16_t *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(uint16_t));	}
		
		inline void linkVariable(uint8_t index, int16_t *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(int16_t));	}
		
		inline void linkVariable(uint8_t index, uint32_t *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(uint32_t));	}
		
		inline void linkVariable(uint8_t index, int32_t *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(int32_t));	}
		
		inline void linkVariable(uint8_t index, uint64_t *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(uint64_t));	}
		
		inline void linkVariable(uint8_t index, int64_t *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(int64_t));	}
		
		inline void linkVariable(uint8_t index, float *var)
			{	this->linkVariable(index, (uint8_t*)var, sizeof(float));	}
		
		
		
		inline void invokeFunction(uint8_t index)
			{	uint8_t dummy; this->writeRaw(MUCOM_EXECUTE_REQUEST, index, &dummy, 1);	}
		
		inline void invokeFunction(uint8_t index, uint8_t* data, uint8_t cnt)
			{	this->writeRaw(MUCOM_EXECUTE_REQUEST, index, data, cnt);	}


		
		inline void write(uint8_t index, uint8_t *data, uint8_t cnt)
			{	this->writeRaw(MUCOM_WRITE_REQUEST, index, data, cnt);	}
		
		inline void writeByte(uint8_t index, uint8_t data)
			{	this->write(index, (uint8_t*)&data, sizeof(uint8_t));	}
		
		inline void writeShort(uint8_t index, uint16_t data)
			{	this->write(index, (uint8_t*)&data, sizeof(uint16_t));	}
		
		inline void writeLong(uint8_t index, uint32_t data)
			{	this->write(index, (uint8_t*)&data, sizeof(uint32_t));	}
		
		inline void writeLongLong(uint8_t index, uint64_t data)
			{	this->write(index, (uint8_t*)&data, sizeof(uint64_t));	}
		
		inline void writeFloat(uint8_t index, float data)
			{	this->write(index, (uint8_t*)&data, sizeof(float));	}
		

		
		int8_t read(uint8_t index, uint8_t *data, uint8_t cnt);
		
		inline int8_t readByte(uint8_t index, uint8_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(uint8_t));	}
		
		inline int8_t readByte(uint8_t index, int8_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(int8_t));	}
		
		inline int8_t readShort(uint8_t index, uint16_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(uint16_t));	}
		
		inline int8_t readShort(uint8_t index, int16_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(int16_t));	}
		
		inline int8_t readLong(uint8_t index, uint32_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(uint32_t));	}
		
		inline int8_t readLong(uint8_t index, int32_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(int32_t));	}
		
		inline int8_t readLongLong(uint8_t index, uint64_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(uint64_t));	}
		
		inline int8_t readLongLong(uint8_t index, int64_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(int64_t));	}
		
		inline int8_t readFloat(uint8_t index, float *data)
			{	return this->read(index, (uint8_t*)data, sizeof(float));	}
		
		uint8_t readByte(uint8_t index);
		
		uint16_t readShort(uint8_t index);
		
		uint32_t readLong(uint8_t index);
		
		uint64_t readLongLong(uint8_t index);
		
		float readFloat(uint8_t index);
};


#endif //MUCOMBASE_H
