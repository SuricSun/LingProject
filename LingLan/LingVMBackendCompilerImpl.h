#pragma once

#include"__LingLanShared.h"
#include"../LingVM/LingVMBackendCompilerInterface.h"

namespace LingLan {

	namespace APIImpl {

		class LingVMBackendCompilerImpl : public LingVM::API::LingVMBackendCompileInterface {
		public:
			virtual u8string* getFileExtension();
			virtual u8* compile(const u8* p_in, u64 inSize);
		};
	}
}

extern "C" {
	LINGLAN_DLLIO LingVM::API::LingVMBackendCompileInterface* __GetLingVMBackendCompiler__();
	LINGLAN_DLLIO void __DestroyLingVMBackendCompiler__(LingVM::API::LingVMBackendCompileInterface* p_c);
}
