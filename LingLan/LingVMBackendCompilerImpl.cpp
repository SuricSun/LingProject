#include "LingVMBackendCompilerImpl.h"

u8string* LingLan::APIImpl::LingVMBackendCompilerImpl::getFileExtension() {
	
	return new u8string(u8"lingmod");
}

u8* LingLan::APIImpl::LingVMBackendCompilerImpl::compile(const u8* p_in, u64 inSize) {
	
	return (u8*)inSize;
}

LingVM::API::LingVMBackendCompileInterface* __GetLingVMBackendCompiler__() {
	
	return new LingLan::APIImpl::LingVMBackendCompilerImpl();
}

void __DestroyLingVMBackendCompiler__(LingVM::API::LingVMBackendCompileInterface* p_c) {

	delete p_c;
}
