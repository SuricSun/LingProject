#pragma once

#include"__StartupShared.h"
#include"LingVMBackendCompilerInterface.h"
#include"../LingCommon/ExtStd.h"
#include"../LingCommon/UTFStringAffair.h"
#include"../LingCommon/File.h"

using namespace LingCommon;
using namespace LingCommon::Util::IO;
using namespace LingCommon::Util::DSA;
using namespace LingCommon::Util::ExtStd;
using namespace LingVM::API;
using namespace std;

namespace LingVM {

	namespace Startup {

		class StartupException : public Exception::BaseException {
		public:
			StartupException(const char8_t* p_info);
		};

		class BackendCompilerWrapper {
		protected:
			void setAllMemberNullptr();
		public:
			FuncGetLingVMBackendCompiler getLingVMBackendCompiler = nullptr;
			FuncDestroyLingVMBackendCompiler destroyLingVMBackendCompiler = nullptr;
			LingVMBackendCompileInterface* p_backendCompiler = nullptr;
		public:
			void tryGetBackendCompilerFromDLL(const u8string* p_filePath);
			void tryGetBackendCompilerFromDLL(const u16string* p_filePath);
			void tryGetBackendCompilerFromDLL(const char8_t* p_filePath);
			void tryGetBackendCompilerFromDLL(const char16_t* p_filePath);
			~BackendCompilerWrapper();
		};
		/// <summary>
		/// 包含两类信息: 机器信息和VMArgument
		/// 另外可以收集已注册的Compiler
		/// </summary>
		class StartupInfoCollector {
		protected:
		public:
			SYSTEM_INFO machineInfo = {};
			map<u8string*, set<u8string*, DereferenceLess<u8string*>>*, DereferenceLess<u8string*>> vmArguments;
			map<u8string*, BackendCompilerWrapper*, DereferenceLess<u8string*>> backendCompiler;
		public:
			StartupInfoCollector();
			/// <summary>
			/// 收集机器信息
			/// </summary>
			void collectMachineInfo();
			/// <summary>
			/// 格式name1=value1;name2=value2;
			/// 函数会把p_name 和 p_value在内部复制一份新的所以不用在外面手动复制
			/// </summary>
			void addVMArgument(u8string* p_name, u8string* p_value);
			set<u8string*, DereferenceLess<u8string*>>* getVMArgement(u8string* p_name);
			void addVMArgument(const char8_t* p_name, const char8_t* p_value);
			set<u8string*, DereferenceLess<u8string*>>* getVMArgement(const char8_t* p_name);
			void scanBackendCompiler();
			~StartupInfoCollector();
		};
	}
}