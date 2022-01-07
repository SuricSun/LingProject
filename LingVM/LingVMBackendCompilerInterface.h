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
			/// ��ȡ��compiler���Ա�����ļ���չ����֧��������ʽ��
			/// ���ⷵ�ص�u8string�ַ���ָ�������û�б��κ�����ָ�����ù���
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
