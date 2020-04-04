/**
	\brief		
	\details	
	\version	1.0
	\author		Kai Liebich
*/


#ifndef MUCOM_H
#define MUCOM_H

#include <Arduino.h>


#define MUCOM_OK			0
#define MUCOM_ERR			-1
#define MUCOM_ERR_TIMEOUT	-2
#define MUCOM_ERR_COMM		-3


typedef void (*muComFunc)(uint8_t *data, uint8_t cnt);


struct muCom_LinkedVariable_str
{
	uint8_t* addr;
	uint8_t size;
};



class muCom 
{
	private:
		Stream *_ser;
		struct muCom_LinkedVariable_str *_linked_var;
		uint8_t _linked_var_num;
		muComFunc *_linked_func;
		uint8_t _linked_func_num;
		uint8_t _rcv_buf[11];
		uint8_t _rcv_buf_cnt;
		int16_t _timeout;
	
		void writeRaw(uint8_t frameDesc, uint8_t *data, uint8_t cnt);
		
		
	public:
		muCom(Stream &ser, uint8_t num_var, uint8_t num_func);


		void setTimeout(int16_t timeout);


		uint8_t handle(void);
		
		
		int8_t linkFunction(uint8_t index, muComFunc function);
		
		
		int8_t linkVariable(uint8_t index, uint8_t *var, uint8_t size);
		
		inline void linkVariable(uint8_t index, uint8_t *var)
			{	this->linkVariable(index, (uint8_t*)var, 1);	}
		
		inline void linkVariable(uint8_t index, uint16_t *var)
			{	this->linkVariable(index, (uint8_t*)var, 2);	}
		
		inline void linkVariable(uint8_t index, uint32_t *var)
			{	this->linkVariable(index, (uint8_t*)var, 4);	}
		
		inline void linkVariable(uint8_t index, uint64_t *var)
			{	this->linkVariable(index, (uint8_t*)var, 8);	}
		
		inline void linkVariable(uint8_t index, float *var)
			{	this->linkVariable(index, (uint8_t*)var, 4);	}
		
		inline void linkVariable(uint8_t index, double *var)
			{	this->linkVariable(index, (uint8_t*)var, 8);	}
		

		
		void write(uint8_t index, uint8_t *data, uint8_t cnt);
		
		inline void writeByte(uint8_t index, uint8_t data)
			{	this->write(index, &data, 1);	}
		
		inline void writeShort(uint8_t index, uint16_t data)
			{	this->write(index, (uint8_t*)&data, 2);	}
		
		inline void writeLong(uint8_t index, uint32_t data)
			{	this->write(index, (uint8_t*)&data, 4);	}
		
		inline void writeLongLong(uint8_t index, uint64_t data)
			{	this->write(index, (uint8_t*)&data, 8);	}
		
		inline void writeFloat(uint8_t index, float data)
			{	this->write(index, (uint8_t*)&data, 4);	}
		
		inline void writeDouble(uint8_t index, double data)
			{	this->write(index, (uint8_t*)&data, 8);	}
		

		
		int8_t read(uint8_t index, uint8_t *data, uint8_t cnt);
		
		inline int8_t readByte(uint8_t index, uint8_t *data)
			{	return this->read(index, data, 1);	}
		
		inline int8_t readShort(uint8_t index, uint16_t *data)
			{	return this->read(index, (uint8_t*)data, 2);	}
		
		inline int8_t readLong(uint8_t index, uint32_t *data)
			{	return this->read(index, (uint8_t*)data, 4);	}
		
		inline int8_t readLongLong(uint8_t index, uint64_t *data)
			{	return this->read(index, (uint8_t*)data, 8);	}
		
		inline int8_t readFloat(uint8_t index, float *data)
			{	return this->read(index, (uint8_t*)data, 4);	}
		
		inline int8_t readDouble(uint8_t index, double *data)
			{	return this->read(index, (uint8_t*)data, 8);	}
		
		uint8_t readByte(uint8_t index);
		
		uint16_t readShort(uint8_t index);
		
		uint32_t readLong(uint8_t index);
		
		uint64_t readLongLong(uint8_t index);
		
		float readFloat(uint8_t index);
		
		double readDouble(uint8_t index);
};


#endif //MUCOM_H
