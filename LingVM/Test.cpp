#include"Test.h"

using namespace std;

void Test() {

	try {
		LingVM::Startup::StartupInfoCollector info;
		info.collectMachineInfo();
		u8string name = u8"backendcompilerpath";
		info.addVMArgument(addr(name), new u8string(u8"va=l=ue"));
		#ifdef _DEBUG
		info.addVMArgument(addr(name), new u8string(u8"C:\\Work\\Solutions\\VS Solutions\\LingProject\\x64\\Debug"));
		info.addVMArgument(addr(name), new u8string(u8"va=l=ue"));
		//info.scanBackendCompiler();
		//cout << (char*)name.c_str() << "  =  " << (char*)info.vmArguments.find(addr(name))->second->c_str();
		LingVM::Startup::BackendCompilerWrapper w;
		w.tryGetBackendCompilerFromDLL(u8"C:\\Work\\Solutions\\VS Solutions\\LingProject\\x64\\Debug\\LingLan.dll");
		cout << "\ncompile returns " << (int)w.p_backendCompiler->compile(nullptr, 187);
		u8string* p = w.p_backendCompiler->getFileExtension();
		cout << "\n" << (char*)p->c_str();
		cout << "\nand getFileExtension returns " << (char*)w.p_backendCompiler->getFileExtension()->c_str();
		#else
		info.addVMArgument(addr(name), new u8string(u8"C:\\Work\\Solutions\\VS Solutions\\LingProject\\x64\\Release"));
		info.addVMArgument(addr(name), new u8string(u8"va=l=ue"));
		//info.scanBackendCompiler();
		//cout << (char*)name.c_str() << "  =  " << (char*)info.vmArguments.find(addr(name))->second->c_str();
		LingVM::Startup::BackendCompilerWrapper w;
		w.tryGetBackendCompilerFromDLL(u8"C:\\Work\\Solutions\\VS Solutions\\LingProject\\x64\\Release\\LingLan.dll");
		cout << "\ncompile returns " << (int)w.p_backendCompiler->compile(nullptr, 187);
		u8string* p = w.p_backendCompiler->getFileExtension();
		cout << "\n" << (char*)p->c_str();
		cout << "\nand getFileExtension returns " << (char*)w.p_backendCompiler->getFileExtension()->c_str();
		#endif
	} catch (Exception::BaseException* p) {
		p->showMsg();
		del(p);
	}
}