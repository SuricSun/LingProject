#include "IOCommon.h"

LingCommon::Util::IO::IOException::IOException(const char8_t* p_info) : LingCommon::Exception::BaseException(p_info) {

	this->className.assign((char8_t*)u8"IOException");
}

LingCommon::Util::IO::FileException::FileException(const char8_t* p_info) : LingCommon::Exception::BaseException(p_info) {

	this->className.assign((char8_t*)u8"FileException");
}
