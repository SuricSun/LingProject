#include "UTFStringAffair.h"

using namespace LingCommon;

void LingCommon::Util::DSA::UTFStringAffair::UTF8To16(const u8string* from, u16string* to) {

	if (from == nullptr || to == nullptr) {
		throw new Exception::InvalidArgException(u8"UTF8To16 nullptr arg");
	}
	u64 requiredCharCnt = MultiByteToWideChar(CP_UTF8, 0, (CHAR*)from->data(), from->size(), nullptr, 0);
	if (requiredCharCnt == 0) {
		throw new UTFStringAffairException(u8"Cant convert utf8 to utf16");
	}
	to->resize(requiredCharCnt);
	requiredCharCnt = MultiByteToWideChar(CP_UTF8, 0, (CHAR*)from->data(), from->size(), (WCHAR*)to->data(), to->size());
	if (requiredCharCnt == 0) {
		throw new UTFStringAffairException(u8"Cant convert utf8 to utf16");
	}
}

void LingCommon::Util::DSA::UTFStringAffair::UTF8To16(const u8string& from, u16string& to) {

	UTFStringAffair::UTF8To16(addr(from), addr(to));
}

void LingCommon::Util::DSA::UTFStringAffair::UTF16To8(const u16string* from, u8string* to) {

	if (from == nullptr || to == nullptr) {
		throw new Exception::InvalidArgException(u8"UTF8To16 nullptr arg");
	}
	u64 requiredByteCnt = WideCharToMultiByte(CP_UTF8, 0, (WCHAR*)from->data(), from->size(), nullptr, 0, nullptr, nullptr);
	if (requiredByteCnt == 0) {
		throw new UTFStringAffairException(u8"Cant convert utf16 to utf8");
	}
	to->resize(requiredByteCnt);
	requiredByteCnt = WideCharToMultiByte(CP_UTF8, 0, (WCHAR*)from->data(), from->size(), (CHAR*)to->data(), to->size(), nullptr, nullptr);
	if (requiredByteCnt == 0) {
		throw new UTFStringAffairException(u8"Cant convert utf16 to utf8");
	}
}

void LingCommon::Util::DSA::UTFStringAffair::UTF16To8(const u16string& from, u8string& to) {

	UTFStringAffair::UTF16To8(addr(from), addr(to));
}

u64 LingCommon::Util::DSA::UTFStringAffair::UTF8To16(const char8_t* from, u64 fromSizeInByte, char16_t* to, u64 toSizeInByte) {
	
	if (from == nullptr) {
		throw new Exception::InvalidArgException(u8"UTF8To16 nullptr arg <from>");
	}
	if (toSizeInByte == 0) {
		return (u64)MultiByteToWideChar(CP_UTF8, 0, (LPCCH)from, fromSizeInByte, nullptr, 0) * 2;
	}
	if (to == nullptr) {
		throw new Exception::InvalidArgException(u8"UTF8To16 nullptr arg <from>");
	}
	if (MultiByteToWideChar(CP_UTF8, 0, (LPCCH)from, fromSizeInByte, (LPWSTR)to, toSizeInByte / 2) == 0) {
		throw new UTFStringAffairException(u8"Cant convert utf16 to utf8");
	}
	return 0;
}

u64 LingCommon::Util::DSA::UTFStringAffair::UTF16To8(const char16_t* from, u64 fromSizeInByte, char8_t* to, u64 toSizeInByte) {
	
	if (from == nullptr) {
		throw new Exception::InvalidArgException(u8"UTF8To16 nullptr arg <from>");
	}
	if (toSizeInByte == 0) {
		return (u64)WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)from, fromSizeInByte / 2, nullptr, 0, nullptr, nullptr);
	}
	if (to == nullptr) {
		throw new Exception::InvalidArgException(u8"UTF8To16 nullptr arg <from>");
	}
	if (WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)from, fromSizeInByte / 2, (LPSTR)to, toSizeInByte, nullptr, nullptr) == 0) {
		throw new UTFStringAffairException(u8"Cant convert utf16 to utf8");
	}
	return 0;
}

u64 LingCommon::Util::DSA::UTFStringAffair::UTF8StrLen(const char8_t* str, u64 maxSize) {
	
	if (maxSize == 0) {
		maxSize = MAX_U64;
	}
	for (u64 i = 0; i < maxSize; i++) {
		if (str[i] == u8'\0') {
			return i + 1;
		}
	}
	return 0;
}

u64 LingCommon::Util::DSA::UTFStringAffair::UTF16StrLen(const char16_t* str, u64 maxSize) {
	
	if (maxSize == 0) {
		maxSize = MAX_U64;
	}
	for (u64 i = 0; i < maxSize; i++) {
		if (str[i] == u'\0') {
			return i + 1;
		}
	}
	return 0;
}

LingCommon::Util::DSA::UTFStringAffairException::UTFStringAffairException(const char8_t* p_info) : LingCommon::Exception::BaseException(p_info) {

	this->className.assign((char8_t*)u8"UTFStringAffairException");
}
