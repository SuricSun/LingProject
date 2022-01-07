#pragma once

#include"__UtilShared.h"

#include<regex>

using namespace std;

namespace LingCommon {

	namespace Util {

		namespace ExtStd {

			template<class PointerType>
			class DereferenceLess {
			public:
				bool operator()(const PointerType& lhs, const PointerType& rhs) const;
			};
			template<class PointerType>
			inline bool DereferenceLess<PointerType>::operator()(const PointerType& lhs, const PointerType& rhs) const {

				return (*lhs) < (*rhs);
			}
		}
	}
}