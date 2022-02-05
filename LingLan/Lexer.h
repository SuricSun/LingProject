#pragma once

#include"__CompileShared.h"

#include"..\LingCommon\ExtStd.h"
#include"CompileCommon.h"

namespace LingLan {

	namespace Compile {

		namespace Lexical {

			class Lexer {
			public:
				/// <summary>
				/// Main Owner of terminal
				/// 所有的从文件读取的非终止符
				/// 保存有顺序要求所以不用map用list
				/// </summary>
				vector<SymbolTerminal*> allSymbolTerminal;
				/// <summary>
				/// 这个Terminal特殊，只是和上面的allSymbolTerminal区分存放而已
				/// </summary>
				SymbolEndMarker endMarker;
				/// <summary>
				/// 主要是给AugGrammar使用的
				/// </summary>
				map<u8string*, SymbolTerminal*, Util::ExtStd::DereferenceLess<u8string*>> nameToSymbolTerminalMap;
				bool shallowClear = false;
			public:
				Lexer();
				Lexer(const Lexer& lxr);
				void operator=(const Lexer& lxr);
				void init(LanguageRulesFile* p_rulesfFile);
				vector<Token*>* lexicalAnalyze(const char8_t* p_inputProgramStr, u64 sizeInByte);
				vector<Token*>* lexicalAnalyze(const u8string* p_inputProgramStr);
				void clear();
				void setShallowClear(bool yesOrNo);
				~Lexer();
			};
		}
	}
}
