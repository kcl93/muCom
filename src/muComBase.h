/**
	\brief		Base class for the muCom interface
	\details	This file includes the base class that handles the protocol itself without the actual HW implementation neccessary to transmit and receive data.
	\version	1.0
	\author		Kai Liebich
*/


#ifndef MUCOMBASE_H
#define MUCOMBASE_H

//Required includes
#include <stdint.h>

#ifdef __AVR__
	#pragma GCC optimize ("O2") //Roughly 4% more speed for 2% more flash usage compared to default "Os"
#endif

//Optional define for making muCom safe for being executed from different sources (e.g. loop() and a timer interrupt in parallel)
//Deactivate for improved performance and slightly redused flash usage at own risk
//#define MUCOM_DEACTIVATE_THREADLOCK

//Optional define to remove support for discovery of linked variables and functions
//Deactivating this functionality can be used in a closed system after debugging to save flash and RAM
//#define MUCOM_DEACTIVATE_DISCOVERY

//Defines for the return values of the muCom interface functions
#define MUCOM_OK			0	//!< OK. No error.
#define MUCOM_ERR			-1	//!< Misc. error!
#define MUCOM_ERR_TIMEOUT	-2	//!< Timeout occured (partner device not answering)
#define MUCOM_ERR_COMM		-3	//!< Misc. communication error. Consider using one or two parity bits.

//Define default timeout of read requests via the interface
#define MUCOM_DEFAULT_TIMEOUT		100	//!< Default read timeout

//Various defines for the interface itself
#define MUCOM_HEADER_BIT_MASK		0x80
#define MUCOM_FRAME_DESC_MASK		0x60
#define MUCOM_DATA_BYTE_CNT_MASK	0x1C
#define MUCOM_READ_RESPONSE			0x00
#define MUCOM_READ_REQUEST			0x20
#define MUCOM_WRITE_REQUEST			0x40
#define MUCOM_EXECUTE_REQUEST		0x60


/**
	\brief	Standard variable constants that can be linked via MUCOM
*/
enum muCom_LinkedVariableType : uint8_t
{
	MUCOM_INT8	 = 0,
	MUCOM_INT16	 = 1,
	MUCOM_INT32	 = 2,
	MUCOM_INT64	 = 3,
	MUCOM_UINT8	 = 4,
	MUCOM_UINT16 = 5,
	MUCOM_UINT32 = 6,
	MUCOM_UINT64 = 7,
	MUCOM_FLOAT	 = 8,
	MUCOM_DOUBLE = 9,
	MUCOM_ARRAY	 = 10
};


/**
	\brief	Function prototype for functions that can be invoked by the muCom interface
*/
typedef void (*muComFunc)(uint8_t *data, uint8_t cnt);


/**
	\brief	Internal structure to store references to linked variables
*/
struct muCom_LinkedVariable_str
{
	uint8_t* addr;
	uint8_t size;
	#ifndef MUCOM_DEACTIVATE_DISCOVERY
		muCom_LinkedVariableType type;
	#endif
};


/**
	\brief		Base muCom class
	\details	This class implements the muCom protocol itself and relies on being inherited in order to implement the actual serial interface.
*/
class muComBase 
{
	private:
		struct muCom_LinkedVariable_str *_linked_var;	//Array of all linked variables
		uint8_t _linked_var_num;						//Max. number of linked variables
		muComFunc *_linked_func;						//Array of all linked functions
		uint8_t _linked_func_num;						//Max. number of linked functions
		uint8_t _rcv_buf[11];							//Internal receive buffer
		uint8_t _rcv_buf_cnt;							//Number of valid bytes in the internal receive buffer (serves as statemachine)
		int16_t _timeout;								//Current timeout for read requests
		uint32_t _lastCommTime;							//Timestamp of last successful communication
		
		//Write a raw muCom frame
		void writeRaw(uint8_t frameDesc, uint8_t index, uint8_t *data, uint8_t cnt);
		
		//Internal write function to actual HW
		virtual void _write(uint8_t* data, uint8_t cnt) = 0;
		
		//Internal read function from actual HW
		virtual uint8_t _read(void) = 0;
		
		//Internal function to check for available data from the actual HW
		virtual uint8_t _available(void) = 0;
		
		//Internal function to check how many bytes are free in the serial buffer
		virtual uint8_t _availableTxBuffer(void) = 0;
		
		//Internal function to wait until all written bytes are acutally transmitted
		virtual void _flushTx(void) = 0;
		
		//Internal function to get the current timestamp in ms
		virtual uint32_t _getTimestamp(void) = 0;
		
		//Internal function to disable interrupts
		virtual void _disableInterrupts(void) = 0;
		
		//Internal function to enable interrupts
		virtual void _enableInterrupts(void) = 0;
		
		#ifndef MUCOM_DEACTIVATE_DISCOVERY
			int8_t _linkVariable(uint8_t index, uint8_t *var, uint8_t size, muCom_LinkedVariableType type);
		#endif
		
		
	public:
		/**
			\brief		Constructor of the base class
			\param[in]	var_buf		Fixed buffer for linked variables
			\param[in]	num_var		Max. number of variables to be linked
			\param[in]	func_buf	Fixed buffer for linked functions
			\param[in]	num_func	Max. number of functions to be linked
		*/
		muComBase(struct muCom_LinkedVariable_str *var_buf, uint8_t num_var, muComFunc *func_buf, uint8_t num_func);
		
		
		/**
			\brief	Get timestamp of last successful communication
			\return	Timestamp
		*/
		inline uint32_t getLastCommTime(void)
			{	return this->_lastCommTime;	}


		/**
			\brief		Set timeout for read requests
			\param[in]	timeout	Timeout in milliseconds
		*/
		void setTimeout(int16_t timeout);


		/**
			\brief		Handle the muCom interface
			\details	This function handles the muCom interface and decodes the received data.
						It should be executed as often as possible and will handle writing to and reading from linked variables
						as well as executing requested functions. This function is also used during reading data from the communication partner.
			\return		1 = answer from a read request was received, else 0
		*/
		uint8_t handle(void);
		
		
		/**
			\brief		Link function to the muCom interface
			\param[in]	index		Index used to invoke this functions
			\param[in]	function	Function to be linked to the interface
		*/
		int8_t linkFunction(uint8_t index, muComFunc function);
		
		
		/**
			\brief		Link a variable or a buffer to the muCom interface
			\param[in]	index	Index used to access the variable/buffer
			\param[in]	var		Pointer to the variable or buffer to be linked to the interface
			\param[in]	size	Size of the variable/buffer in bytes (only neccessary when linking buffers)
			\return		MUCOM_OK if all is alright
		*/
		#ifndef MUCOM_DEACTIVATE_DISCOVERY
			inline int8_t linkVariable(uint8_t index, uint8_t *var, uint8_t size)
				{	return this->_linkVariable(index, var, size, MUCOM_ARRAY);	}
			
			inline int8_t linkVariable(uint8_t index, uint8_t *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(uint8_t), MUCOM_UINT8);	}
				
			inline int8_t linkVariable(uint8_t index, int8_t *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(int8_t), MUCOM_INT8);	}
			
			inline int8_t linkVariable(uint8_t index, uint16_t *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(uint16_t), MUCOM_UINT16);	}
			
			inline int8_t linkVariable(uint8_t index, int16_t *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(int16_t), MUCOM_INT16);	}
			
			inline int8_t linkVariable(uint8_t index, uint32_t *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(uint32_t), MUCOM_UINT32);	}
			
			inline int8_t linkVariable(uint8_t index, int32_t *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(int32_t), MUCOM_INT32);	}
			
			inline int8_t linkVariable(uint8_t index, uint64_t *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(uint64_t), MUCOM_UINT64);	}
			
			inline int8_t linkVariable(uint8_t index, int64_t *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(int64_t), MUCOM_INT64);	}
			
			inline int8_t linkVariable(uint8_t index, float *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(float), MUCOM_FLOAT);	}
				
			inline int8_t linkVariable(uint8_t index, double *var)
				{	return this->_linkVariable(index, (uint8_t*)var, sizeof(double), MUCOM_DOUBLE);	}
		#else
			int8_t linkVariable(uint8_t index, uint8_t *var, uint8_t size)
			
			inline int8_t linkVariable(uint8_t index, uint8_t *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(uint8_t));	}
				
			inline int8_t linkVariable(uint8_t index, int8_t *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(int8_t));	}
			
			inline int8_t linkVariable(uint8_t index, uint16_t *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(uint16_t));	}
			
			inline int8_t linkVariable(uint8_t index, int16_t *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(int16_t));	}
			
			inline int8_t linkVariable(uint8_t index, uint32_t *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(uint32_t));	}
			
			inline int8_t linkVariable(uint8_t index, int32_t *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(int32_t));	}
			
			inline int8_t linkVariable(uint8_t index, uint64_t *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(uint64_t));	}
			
			inline int8_t linkVariable(uint8_t index, int64_t *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(int64_t));	}
			
			inline int8_t linkVariable(uint8_t index, float *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(float));	}
				
			inline int8_t linkVariable(uint8_t index, double *var)
				{	return this->linkVariable(index, (uint8_t*)var, sizeof(double));	}
		#endif
		
		/**
			\brief		Invoke a function at the communication partner
			\param[in]	index	Index of the function to be invoked
			\param[in]	data	Pointer to a buffer as a parameter for the function being invoked
			\param[in]	cnt		Number of data bytes in the buffer that will be sent to the function being invoked
		*/
		inline void invokeFunction(uint8_t index, uint8_t* data, uint8_t cnt)
			{	this->writeRaw(MUCOM_EXECUTE_REQUEST, index, data, cnt);	}
		
		/**
			\brief		Invoke a function at the communication partner
			\details	The target function will be invoked with one byte of random data.
			\param[in]	index	Index of the function to be invoked
		*/
		inline void invokeFunction(uint8_t index)
			{	uint8_t dummy; this->writeRaw(MUCOM_EXECUTE_REQUEST, index, &dummy, 1);	}
		

		/**
			\brief		Write a data array to a remote variable
			\param[in]	index	Index of the remote buffer to be written to
			\param[in]	data	Data array to be written to the remote buffer
			\param[in]	cnt		Size of the data array in bytes
		*/
		inline void write(uint8_t index, uint8_t *data, uint8_t cnt)
			{	this->writeRaw(MUCOM_WRITE_REQUEST, index, data, cnt);	}
		
		/**
			\brief		Write a byte (8 bit) to the communication partner
			\param[in]	index	Index of the remote variable to be written to
			\param[in]	data	Byte to be written to the communication partner
		*/
		inline void writeByte(uint8_t index, uint8_t data)
			{	this->write(index, (uint8_t*)&data, sizeof(uint8_t));	}
		
		/**
			\brief		Write a short (16 bit) to the communication partner
			\param[in]	index	Index of the remote variable to be written to
			\param[in]	data	Short to be written to the communication partner
		*/
		inline void writeShort(uint8_t index, uint16_t data)
			{	this->write(index, (uint8_t*)&data, sizeof(uint16_t));	}
		
		/**
			\brief		Write a long (32 bit) to the communication partner
			\param[in]	index	Index of the remote variable to be written to
			\param[in]	data	Long to be written to the communication partner
		*/
		inline void writeLong(uint8_t index, uint32_t data)
			{	this->write(index, (uint8_t*)&data, sizeof(uint32_t));	}

		/**
			\brief		Write a long long (64 bit) to the communication partner
			\param[in]	index	Index of the remote variable to be written to
			\param[in]	data	Long long to be written to the communication partner
		*/
		inline void writeLongLong(uint8_t index, uint64_t data)
			{	this->write(index, (uint8_t*)&data, sizeof(uint64_t));	}
		
		/**
			\brief		Write a float to the communication partner
			\param[in]	index	Index of the remote variable to be written to
			\param[in]	data	Float to be written to the communication partner
		*/
		inline void writeFloat(uint8_t index, float data)
			{	this->write(index, (uint8_t*)&data, sizeof(float));	}
		
		/**
			\brief		Write a double to the communication partner (not available on AVR microcontrollers)
			\param[in]	index	Index of the remote variable to be written to
			\param[in]	data	Float to be written to the communication partner
		*/
		inline void writeDouble(uint8_t index, double data)
			{	this->write(index, (uint8_t*)&data, sizeof(double));	}
			
		/**
			\brief		Read data from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\param[in]	data	Array to store the contents of the remote variable
			\param[in]	cnt		Number of data bytes to read
		*/
		int8_t read(uint8_t index, uint8_t *data, uint8_t cnt);
		
		/**
			\brief		Read a byte from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\param[in]	data	Pointer to the variable where the read data should be stored
			\return		See muCom error codes (0 = OK, <0 = Error)
		*/
		inline int8_t readByte(uint8_t index, uint8_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(uint8_t));	}
		
		inline int8_t readByte(uint8_t index, int8_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(int8_t));	}
		
		/**
			\brief		Read a short from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\param[in]	data	Pointer to the variable where the read data should be stored
			\return		See muCom error codes (0 = OK, <0 = Error)
		*/
		inline int8_t readShort(uint8_t index, uint16_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(uint16_t));	}
		
		inline int8_t readShort(uint8_t index, int16_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(int16_t));	}
		
		/**
			\brief		Read a long from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\param[in]	data	Pointer to the variable where the read data should be stored
			\return		See muCom error codes (0 = OK, <0 = Error)
		*/
		inline int8_t readLong(uint8_t index, uint32_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(uint32_t));	}
		
		inline int8_t readLong(uint8_t index, int32_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(int32_t));	}
		
		/**
			\brief		Read a long long from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\param[in]	data	Pointer to the variable where the read data should be stored
			\return		See muCom error codes (0 = OK, <0 = Error)
		*/
		inline int8_t readLongLong(uint8_t index, uint64_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(uint64_t));	}
		
		inline int8_t readLongLong(uint8_t index, int64_t *data)
			{	return this->read(index, (uint8_t*)data, sizeof(int64_t));	}
		
		/**
			\brief		Read a float from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\param[in]	data	Pointer to the variable where the read data should be stored
			\return		See muCom error codes (0 = OK, <0 = Error)
		*/
		inline int8_t readFloat(uint8_t index, float *data)
			{	return this->read(index, (uint8_t*)data, sizeof(float));	}
		
		/**
			\brief		Read a double from the communication partner (not available on AVR microcontrollers)
			\param[in]	index	Index of the remote variable to be read
			\param[in]	data	Pointer to the variable where the read data should be stored
			\return		See muCom error codes (0 = OK, <0 = Error)
		*/
		inline int8_t readDouble(uint8_t index, double *data)
			{	return this->read(index, (uint8_t*)data, sizeof(float));	}
			
		/**
			\brief		Read a byte from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\return		Byte read from the communication partner.
						<br>-1 in case of errors
		*/
		uint8_t readByte(uint8_t index);
		
		/**
			\brief		Read a short from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\return		Short read from the communication partner.
						<br>-1 in case of errors
		*/
		uint16_t readShort(uint8_t index);
		
		/**
			\brief		Read a long from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\return		Long read from the communication partner.
						<br>-1 in case of errors
		*/
		uint32_t readLong(uint8_t index);
		
		/**
			\brief		Read a long long from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\return		Long long read from the communication partner.
						<br>-1 in case of errors
		*/
		uint64_t readLongLong(uint8_t index);
		
		/**
			\brief		Read a float from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\return		Float read from the communication partner.
						<br>-1 in case of errors
		*/
		float readFloat(uint8_t index);
		
		/**
			\brief		Read a float from the communication partner
			\param[in]	index	Index of the remote variable to be read
			\return		Float read from the communication partner.
						<br>-1 in case of errors
		*/
		double readDouble(uint8_t index);
};


#endif //MUCOMBASE_H
