#pragma once

#include"__RuntimeShared.h"

using namespace std;
using namespace LingCommon;

namespace LingVM {

	namespace Runtime {

		class RuntimeException : public Exception::BaseException {
		public:
			RuntimeException(const char8_t* p_info);
		};
	}
}
