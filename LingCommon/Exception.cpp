#include"Exception.h"

LingCommon::Exception::BaseException::BaseException(const char8_t* p_info) {

	this->className.assign(u8"BaseException");
	this->info.assign(p_info);
}

void LingCommon::Exception::BaseException::UTF8To16(const u8string* from, u16string* to) {

	u64 requiredCharCnt = MultiByteToWideChar(CP_UTF8, 0, (CHAR*)from->data(), from->size(), nullptr, 0);
	if (requiredCharCnt == 0) {
		to->assign(u"Cant convert utf8 to utf16");
		return;
	}
	to->resize(requiredCharCnt);
	requiredCharCnt = MultiByteToWideChar(CP_UTF8, 0, (CHAR*)from->data(), from->size(), (WCHAR*)to->data(), to->size());
	if (requiredCharCnt == 0) {
		to->assign(u"Cant convert utf8 to utf16");
		return;
	}
}

void LingCommon::Exception::BaseException::showMsg() {

	u16string info = {};
	u16string className = {};
	this->UTF8To16(addr(this->info), addr(info));
	this->UTF8To16(addr(this->className), addr(className));
	MessageBox(nullptr, (WCHAR*)info.c_str(), (WCHAR*)((u16string(u"LING VM ") + (className)).c_str()), MB_OK);
}

LingCommon::Exception::InvalidArgException::InvalidArgException(const char8_t* p_info) : LingCommon::Exception::BaseException(p_info) {

	this->className.assign(u8"InvalidArgException");
}

LingCommon::Exception::ArgOutOfRangeException::ArgOutOfRangeException(const char8_t* p_info) : LingCommon::Exception::BaseException(p_info) {

	this->className.assign(u8"ArgOutOfRangeException");
}

LingCommon::Exception::BufferTooSmallException::BufferTooSmallException(const char8_t* p_info) : LingCommon::Exception::BaseException(p_info) {
	
	this->className.assign(u8"BufferTooSmallException");
}

LingCommon::Exception::NotFoundException::NotFoundException(const char8_t* p_info) : LingCommon::Exception::BaseException(p_info) {
	
	this->className.assign(u8"NotFoundException");
}
