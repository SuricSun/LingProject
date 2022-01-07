#pragma once

#include"__UtilShared.h"

using namespace std;

namespace LingCommon {

	namespace Util {

		namespace IO {

			class IOException : public Exception::BaseException {
			public:
				IOException(const char8_t* p_info);
			};

			class FileException : public Exception::BaseException {
			public:
				FileException(const char8_t* p_info);
			};
		}
	}
}
