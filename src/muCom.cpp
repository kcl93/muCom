

#include "muCom.h"



muCom::muCom(Stream &ser, uint8_t num_var, uint8_t num_func) : muComBase(num_var, num_func)
{
	//Link interface
	this->_ser = &ser;
}


