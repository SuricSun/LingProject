#include "RuntimeCommon.h"

LingVM::Runtime::RuntimeException::RuntimeException(const char8_t* p_info) : Exception::BaseException(p_info) {

	this->className.assign(u8"RuntimeException");
}
