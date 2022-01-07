#pragma once

#include"__LingCommonShared.h"

#include<string>

using namespace std;

namespace LingCommon {

	namespace Exception {

		class UTFStringAffair;

		class BaseException {
		protected:
		public:
			u8string className;
			u8string info;
			void* plsBeCarefulOfUsingMe = nullptr;
		public:
			BaseException(const char8_t* p_info);
			static void UTF8To16(const u8string* from, u16string* to);
			void showMsg();
		};

		class InvalidArgException : public BaseException {
		protected:
		public:
			InvalidArgException(const char8_t* p_info);
		};

		class ArgOutOfRangeException : public BaseException {
		protected:
		public:
			ArgOutOfRangeException(const char8_t* p_info);
		};

		class BufferTooSmallException : public BaseException {
		protected:
		public:
			BufferTooSmallException(const char8_t* p_info);
		};

		class NotFoundException : public BaseException {
		protected:
		public:
			NotFoundException(const char8_t* p_info);
		};
	}
}
