

#include "muCom.h"



muCom::muCom(Stream &ser, struct muCom_LinkedVariable_str *var_buf, uint8_t num_var, muComFunc *func_buf, uint8_t num_func) : muComBase(var_buf, num_var, func_buf, num_func)
{
	//Link interface
	this->_ser = &ser;
}


