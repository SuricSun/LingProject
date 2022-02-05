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
				/// ���еĴ��ļ���ȡ�ķ���ֹ��
				/// ������˳��Ҫ�����Բ���map��list
				/// </summary>
				vector<SymbolTerminal*> allSymbolTerminal;
				/// <summary>
				/// ���Terminal���⣬ֻ�Ǻ������allSymbolTerminal���ִ�Ŷ���
				/// </summary>
				SymbolEndMarker endMarker;
				/// <summary>
				/// ��Ҫ�Ǹ�AugGrammarʹ�õ�
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
