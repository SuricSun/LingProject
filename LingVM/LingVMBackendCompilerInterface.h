#pragma once

#include"../LingCommon/__LingCommonShared.h"
#include<string>

using namespace std;
using namespace LingCommon;

namespace LingVM {

	namespace API {
		
		class LingVMBackendCompileInterface {
		public:
			/// <summary>
			/// 获取此compiler可以编译的文件扩展名，支持正则表达式；
			/// 另外返回的u8string字符串指针必须是没有被任何其他指针引用过的
			/// </summary>
			/// <returns></returns>
			virtual u8string* getFileExtension() = 0;
			virtual u8* compile(const u8* p_in, u64 inSize) = 0;
		};

		/// <summary>
		/// LingVMBackendCompileInterface* __GetLingVMBackendCompiler__();
		/// </summary>
		using FuncGetLingVMBackendCompiler = LingVMBackendCompileInterface * (*)();
		/// <summary>
		/// void __DestroyLingVMBackendCompiler__(LingVMBackendCompileInterface*)
		/// </summary>
		using FuncDestroyLingVMBackendCompiler = void (*)(LingVMBackendCompileInterface*);
	}
}
