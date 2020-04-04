

#include "muCom.h"



/*
#####Frame structure####
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
*/




muCom::muCom(Stream &ser, uint8_t num_var, uint8_t num_func)
{
	//Link interface
	this->_ser = &ser;
	
	//Reset receive statemachine
	this->_rcv_buf_cnt = 0;
	
	//Reserve space for linked variables
	this->_linked_var_num = num_var;
	this->_linked_var = (muCom_LinkedVariable_str*)calloc(num_var, sizeof(struct muCom_LinkedVariable_str));
	
	//Reserve space for linked functions
	this->_linked_func_num = num_func;
	this->_linked_func = (muComFunc*)calloc(num_func, sizeof(muComFunc));
	
	//Setup default timeout
	this->_timeout = MUCOM_DEFAULT_TIMEOUT;
}



void muCom::setTimeout(int16_t timeout)
{
	if(timeout < 2) //Limit timeout to minimum value
	{
		timeout  = 2;
	}
	this->_timeout = timeout;
}



uint8_t muCom::handle(void)
{
	int8_t bytePos;
	uint8_t dataPos;
	uint8_t tmp;
	uint8_t dataCnt;
	uint8_t frameDesc;
	
	//Read all available data bytes
	while(this->_ser->available() != 0)
	{
		//Get byte from receive buffer
		tmp = this->_ser->read();
		
		if(tmp & MUCOM_HEADER_BIT_MASK)
		{
			//Header received! Reset receive statemachine, e.g. data counter
			this->_rcv_buf_cnt = 1;
			this->_rcv_buf[0] = tmp;
			continue;
		}
		
		if(this->_rcv_buf_cnt == 0)
		{
			//Waiting for the header... Discard any non-header bytes
			continue;
		}
		
		if(this->_rcv_buf_cnt > 10)
		{
			//Buffer overflow! Invalid number of bytes received for the current frame! Reset statemachine...
			this->_rcv_buf_cnt = 0;
			continue;
		}
		
		//Store data byte and calculate desired frame length
		this->_rcv_buf[_rcv_buf_cnt] = tmp;
		dataCnt = (this->_rcv_buf[0] & MUCOM_DATA_BYTE_CNT_MASK) >> 2;
		dataCnt++;
		
		if(dataCnt >= this->_rcv_buf_cnt)
		{
			//Sufficient data received. Decode it and do stuff if required
			frameDesc = this->_rcv_buf[0] & MUCOM_FRAME_DESC_MASK;
			
			//Reconstruct data from frame
			//_rcv_buf[0..8] = Data bytes (first byte will be index)
			dataPos = 0;
			bytePos = 6;
			this->_rcv_buf[0] &= !MUCOM_HEADER_BIT_MASK;
			for(tmp = 0; tmp <= dataCnt; tmp++)
			{
				this->_rcv_buf[tmp] = this->_rcv_buf[dataPos] << bytePos;
				dataPos++;
				bytePos -= 7;
				if(bytePos != 0)
				{
					this->_rcv_buf[tmp] |= this->_rcv_buf[dataPos] >> -bytePos;
					bytePos += 8;
				}
				else
				{
					this->_rcv_buf[tmp] |= this->_rcv_buf[dataPos];
					bytePos += 1;
					dataPos++;
				}
			}
			this->_rcv_buf_cnt = 0; //Reset statemachine
			
			//Execute command
			switch(frameDesc)
			{
				case MUCOM_READ_RESPONSE:
					this->_rcv_buf[9] = dataCnt; //Store number of data bytes
					return 1; //Received a read response! Return 1 as this function should have been used after sending a read request in muCom::read()
					
				case MUCOM_READ_REQUEST:
					//Check index, whether a variable is linked and whether the read size is not greater than the linked variable size
					if((this->_rcv_buf[0] <= this->_linked_var_num) && (this->_linked_var[this->_rcv_buf[0]].addr != NULL) && (dataCnt <= this->_linked_var[this->_rcv_buf[0]].size))
					{
						this->writeRaw(MUCOM_READ_RESPONSE, this->_rcv_buf[0], this->_linked_var[this->_rcv_buf[0]].addr, this->_linked_var[this->_rcv_buf[0]].size);
					}
					break;
					
				case MUCOM_WRITE_REQUEST:
					//Check index, whether a variable is linked and whether the read size is not greater than the linked variable size
					if((this->_rcv_buf[0] <= this->_linked_var_num) && (this->_linked_var[this->_rcv_buf[0]].addr != NULL) && (dataCnt <= this->_linked_var[this->_rcv_buf[0]].size))
					{
						memcpy(this->_linked_var[this->_rcv_buf[0]].addr, this->_rcv_buf, this->_linked_var[this->_rcv_buf[0]].size);
					}
					break;
					
				case MUCOM_EXECUTE_REQUEST:
					//Check index and whether a function is linked
					if((this->_rcv_buf[0] <= this->_linked_func_num) && (this->_linked_func[this->_rcv_buf[0]] != NULL))
					{
						(this->_linked_func[this->_rcv_buf[0]])(this->_rcv_buf, dataCnt);
					}
					break;
					
				default:
					//Error! Ignore frame and return immediatly
					return 0;
			}
			
			continue;
		}
		
		//Increase data byte counter
		this->_rcv_buf_cnt++;
	}
	
	return 0;
}



int8_t muCom::linkFunction(uint8_t index, muComFunc function)
{
	if(index >= this->_linked_func_num)
	{
		return MUCOM_ERR;
	}
	
	this->_linked_func[index] = function;
	
	return MUCOM_OK;
}



int8_t muCom::linkVariable(uint8_t index, uint8_t *var, uint8_t size)
{
	if(index >= this->_linked_var_num)
	{
		return MUCOM_ERR;
	}
	
	this->_linked_var[index].addr = var;
	this->_linked_var[index].size = size;
	
	return MUCOM_OK;
}



void muCom::writeRaw(uint8_t frameDesc, uint8_t index, uint8_t *data, uint8_t size)
{
	uint8_t buf[11];
	int8_t data_pos, byte_pos, payload_pos;
	
	size--;
	if(size > 7)
	{
		size = 7;
	}
	
	//Create first bytes with header and variable index
	buf[0] = MUCOM_HEADER_BIT_MASK + frameDesc + (size << 2) + (index >> 6);
	buf[1] = (index >> 2) & 0x7E;
	
	//Fill payload with data bytes
	payload_pos = 0;
	byte_pos = 0;
	for(data_pos = 0; data_pos <= size; data_pos++)
	{
		if(byte_pos < 0)
		{
			byte_pos = 6;
			payload_pos++;
		}
		buf[payload_pos] += data[data_pos] >> (7 - byte_pos);
		payload_pos++;
		buf[payload_pos] = (data[data_pos] << byte_pos) & 0x7F;
		byte_pos--;
	}
	
	this->_ser->write(buf, payload_pos + 1); //Send write variable request to slave
}



int8_t muCom::read(uint8_t index, uint8_t *data, uint8_t size)
{
	uint8_t buf[11];
	int16_t time_start;
	
	if((size == 0) || (size > 8))
	{
		return MUCOM_ERR;
	}
	
	//Create first bytes with header and variable index
	buf[0] = MUCOM_HEADER_BIT_MASK + MUCOM_READ_REQUEST + ((size - 1) << 2) + (index >> 6);
	buf[1] = (index << 1) & 0x7F;
	
	//Flush receive buffer
	this->handle();
	
	this->_ser->write(buf, 2); //Send read variable request to slave
	
	this->_ser->flush(); //Wait for all bytes to be transmitted
	
	//Receive answer from slave
	
	//Receive answer from slave with timeout
	time_start = (int16_t)millis();
	while(handle() == 0)
	{
		if(((int16_t)millis() - time_start) >= _timeout)
		{
			return MUCOM_ERR_TIMEOUT; //Timeout
		}
	}
	
	//Answer received!
	//_rcv_buf[0]    = Index
	//_rcv_buf[1..8]  = Data bytes
	//_rcv_buf[9]     = Number of data bytes
	if((this->_rcv_buf[9] == size) || (this->_rcv_buf[0] != index))
	{
		return MUCOM_ERR_COMM;
	}
	
	//Copy data to the receive array
	while(_rcv_buf[9] != 0)
	{
		data[this->_rcv_buf[9] - 1] = this->_rcv_buf[this->_rcv_buf[9]];
		this->_rcv_buf[9]--;
	}
	
	return MUCOM_OK;
}



uint8_t muCom::readByte(uint8_t index)
{
	uint8_t data;
	if(this->read(index, &data, 1) != MUCOM_OK)
	{
		return -1;
	}
	return data;
}



uint16_t muCom::readShort(uint8_t index)
{
	uint16_t data;
	if(this->read(index, (uint8_t*)&data, 2) != MUCOM_OK)
	{
		return -1;
	}
	return data;
}



uint32_t muCom::readLong(uint8_t index)
{
	uint32_t data;
	if(this->read(index, (uint8_t*)&data, 4) != MUCOM_OK)
	{
		return -1;
	}
	return data;
}



uint64_t muCom::readLongLong(uint8_t index)
{
	uint64_t data;
	if(this->read(index, (uint8_t*)&data, 8) != MUCOM_OK)
	{
		return -1;
	}
	return data;
}



float muCom::readFloat(uint8_t index)
{
	float data;
	if(this->read(index, (uint8_t*)&data, 4) != MUCOM_OK)
	{
		return -1.0;
	}
	return data;
}



double muCom::readDouble(uint8_t index)
{
	double data;
	if(this->read(index, (uint8_t*)&data, 8) != MUCOM_OK)
	{
		return -1.0;
	}
	return data;
}
